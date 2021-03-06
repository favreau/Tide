/*********************************************************************/
/* Copyright (c) 2017, EPFL/Blue Brain Project                       */
/*                     Pawel Podhajski <pawel.podhajski@epfl.ch>     */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of Ecole polytechnique federale de Lausanne.          */
/*********************************************************************/

#include "PlanarController.h"

namespace
{
const int serialTimeout = 1000;    // in ms
const int powerStateTimer = 60000; // in ms
}

PlanarController::PlanarController(const QString& serialport, const Type type)
    : _config(_getConfig(type))
{
    _serial.setPortName(serialport);
    _serial.setBaudRate(_config.baudrate, QSerialPort::AllDirections);
    _serial.setDataBits(QSerialPort::Data8);
    _serial.setParity(QSerialPort::NoParity);
    _serial.setStopBits(QSerialPort::OneStop);
    _serial.setFlowControl(QSerialPort::NoFlowControl);
    if (!_serial.open(QIODevice::ReadWrite))
        throw std::runtime_error("Could not open " + serialport.toStdString());

    connect(&_serial, &QSerialPort::readyRead, [this, type]() {
        if (_serial.canReadLine())
        {
            QString output(_serial.readLine());
            output = output.trimmed();
            // TV_UR9850 returns "(0;PWR=0)"
            // Others return DISPLAY.POWER=O or DISPLAY.POWER=OFF
            if (type == Type::TV_UR9850)
                output.remove(")");
            ScreenState previousState = _state;
            if (output.endsWith("OFF") || output.endsWith("0"))
                _state = ScreenState::off;
            else if (output.endsWith("ON") || output.endsWith("1"))
                _state = ScreenState::on;
            else
                _state = ScreenState::undefined;

            if (_state != previousState)
                emit powerStateChanged(_state);
        }
    });

    checkPowerState();
    connect(&_timer, &QTimer::timeout, [this]() { checkPowerState(); });
    _timer.start(powerStateTimer);
}

bool PlanarController::powerOn()
{
    _serial.write(_config.powerOn);
    return _serial.waitForBytesWritten(serialTimeout);
}

bool PlanarController::powerOff()
{
    _serial.write(_config.powerOff);
    return _serial.waitForBytesWritten(serialTimeout);
}

ScreenState PlanarController::getState() const
{
    return _state;
}
void PlanarController::checkPowerState()
{
    _serial.write(_config.powerState);
    _serial.waitForBytesWritten(serialTimeout);
}

PlanarController::PlanarConfig PlanarController::_getConfig(
    const Type type) const
{
    switch (type)
    {
    case Type::Matrix:
        return {9600, "OPA1DISPLAY.POWER=ON\r", "OPA1DISPLAY.POWER=OFF\r",
                "OPA1DISPLAY.POWER?\r"};
    case Type::TV_UR9850:
        return {19200, "(PWR=1)\r", "(PWR=0)\r", "(PWR?)\r"};
    case Type::TV_UR9851:
        return {19200, "DISPLAY.POWER=ON\n", "DISPLAY.POWER=OFF\n",
                "DISPLAY.POWER?\n"};
    default:
        throw std::invalid_argument("Non existing serial type");
    }
}
