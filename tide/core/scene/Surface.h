/*********************************************************************/
/* Copyright (c) 2018, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
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

#ifndef SURFACE_H
#define SURFACE_H

#include "Background.h"
#include "ContextMenu.h"
#include "DisplayGroup.h"
#include "serialization/includes.h"
#include "types.h"

/**
 * A uniform display surface, containing (currently) one DisplayGroup.
 */
class Surface : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Surface)

public:
    Surface(size_t index, DisplayGroupPtr group);
    Surface(size_t index, DisplayGroupPtr group, BackgroundPtr background);

    size_t getIndex() const;

    DisplayGroup& getGroup();
    const DisplayGroup& getGroup() const;
    DisplayGroupPtr getGroupPtr() const;

    Background& getBackground();
    const Background& getBackground() const;
    BackgroundPtr getBackgroundPtr() const;

    ContextMenu& getContextMenu();

    /**
     * Move this object and its member QObjects to the given QThread.
     *
     * This intentionally shadows the default QObject::moveToThread to include
     * member QObjects which are stored using shared_ptr and thus can't be made
     * direct children of this class.
     * @param thread the target thread.
     */
    void moveToThread(QThread* thread);

signals:
    void modified();

private:
    friend class boost::serialization::access;

    Surface() = default;

    /** Serialize for sending to Wall applications. */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        // clang-format off
        ar & _group;
        ar & _background;
        ar & _contextMenu;
        // clang-format on
    }

    /** Serialize for saving to an xml file */
    template <class Archive>
    void serialize_members_xml(Archive& ar, const unsigned int /*version*/)
    {
        // clang-format off
        ar & boost::serialization::make_nvp("group", _group);
        // clang-format on
    }

    /** Loading from xml. */
    void serialize_for_xml(boost::archive::xml_iarchive& ar,
                           const unsigned int version)
    {
        serialize_members_xml(ar, version);
    }

    /** Saving to xml. */
    void serialize_for_xml(boost::archive::xml_oarchive& ar,
                           const unsigned int version)
    {
        serialize_members_xml(ar, version);
    }

    size_t _index = 0u;
    DisplayGroupPtr _group;
    BackgroundPtr _background = Background::create();
    ContextMenuPtr _contextMenu = ContextMenu::create();

    void _forwardModifiedSignals();
};

DECLARE_SERIALIZE_FOR_XML(Surface)

#endif
