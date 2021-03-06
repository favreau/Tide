# Copyright (c) 2013-2018, EPFL/Blue Brain Project
#                          Raphael Dumusc <raphael.dumusc@epfl.ch>

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TIDEMASTER_LINK_LIBRARIES
  PUBLIC
    TideCore
    Qt5::Widgets
  PRIVATE
    Qt5::Concurrent
    DeflectQt
)

list(APPEND TIDEMASTER_PUBLIC_HEADERS
  CommandLineParameters.h
  ContentLoader.h
  control/AutomaticLayout.h
  control/CanvasNode.h
  control/CanvasTree.h
  control/ContentController.h
  control/ContextMenuController.h
  control/DisplayGroupController.h
  control/KeyboardController.h
  control/LayoutEngine.h
  control/LayoutPolicy.h
  control/MultiChannelWindowController.h
  control/PixelStreamController.h
  control/SceneController.h
  control/SideController.cpp
  control/WindowController.h
  control/WindowTouchController.h
  control/ZoomController.h
  gui/BackgroundWidget.h
  gui/DisplayGroupListWidget.h
  gui/MasterQuickView.h
  gui/WindowListWidgetItem.h
  gui/MasterWindow.h
  localstreamer/CommandLineOptions.h
  localstreamer/HtmlSelectReplacer.h
  localstreamer/LauncherPlacer.h
  localstreamer/PixelStreamerLauncher.h
  localstreamer/ProcessForker.h
  localstreamer/QmlKeyInjector.h
  MasterApplication.h
  network/MasterFromWallChannel.h
  network/MasterToForkerChannel.h
  network/MasterToWallChannel.h
  PixelStreamWindowManager.h
  qml/FileInfoHelper.h
  qml/MasterDisplayGroupRenderer.h
  qml/MasterSurfaceRenderer.h
  State.h
  StateSerializationHelper.h
  tools/ActivityLogger.h
  tools/InactivityTimer.h
  tools/ScreenshotAssembler.h
)

list(APPEND TIDEMASTER_SOURCES
  CommandLineParameters.cpp
  ContentLoader.cpp
  control/AutomaticLayout.cpp
  control/CanvasNode.cpp
  control/CanvasTree.cpp
  control/ContentController.cpp
  control/ContextMenuController.cpp
  control/DisplayGroupController.cpp
  control/KeyboardController.cpp
  control/LayoutEngine.cpp
  control/LayoutPolicy.cpp
  control/MultiChannelWindowController.cpp
  control/PixelStreamController.cpp
  control/SceneController.cpp
  control/SideController.cpp
  control/WindowController.cpp
  control/WindowTouchController.cpp
  control/ZoomController.cpp
  gui/BackgroundWidget.cpp
  gui/DisplayGroupListWidget.cpp
  gui/MasterQuickView.cpp
  gui/MasterWindow.cpp
  gui/WindowListWidgetItem.cpp
  localstreamer/HtmlSelectReplacer.cpp
  localstreamer/CommandLineOptions.cpp
  localstreamer/LauncherPlacer.cpp
  localstreamer/PixelStreamerLauncher.cpp
  localstreamer/ProcessForker.cpp
  localstreamer/QmlKeyInjector.cpp
  MasterApplication.cpp
  network/MasterFromWallChannel.cpp
  network/MasterToForkerChannel.cpp
  network/MasterToWallChannel.cpp
  PixelStreamWindowManager.cpp
  qml/MasterDisplayGroupRenderer.cpp
  qml/MasterSurfaceRenderer.cpp
  resources/master.qrc
  State.cpp
  StateSerializationHelper.cpp
  tools/ActivityLogger.cpp
  tools/InactivityTimer.cpp
  tools/ScreenshotAssembler.cpp
)

if(TIDE_ENABLE_WEBBROWSER_SUPPORT)
  list(APPEND TIDEMASTER_PUBLIC_HEADERS
    gui/WebbrowserWidget.h
  )
  list(APPEND TIDEMASTER_SOURCES
    gui/WebbrowserWidget.cpp
  )
endif()

if(TIDE_ENABLE_PDF_SUPPORT)
  list(APPEND TIDEMASTER_PUBLIC_HEADERS
    control/PDFController.h
  )
  list(APPEND TIDEMASTER_SOURCES
    control/PDFController.cpp
  )
endif()

if(TIDE_ENABLE_MOVIE_SUPPORT)
  list(APPEND TIDEMASTER_PUBLIC_HEADERS
    control/MovieController.h
  )
  list(APPEND TIDEMASTER_SOURCES
    control/MovieController.cpp
  )
endif()

if(TIDE_ENABLE_PLANAR_CONTROLLER)
  list(APPEND TIDEMASTER_HEADERS hardware/MultiScreenController.h)
  list(APPEND TIDEMASTER_SOURCES hardware/MultiScreenController.cpp)
  list(APPEND TIDEMASTER_HEADERS hardware/PlanarController.h)
  list(APPEND TIDEMASTER_SOURCES hardware/PlanarController.cpp)
  list(APPEND TIDEMASTER_SOURCES hardware/ScreenController.h)
  list(APPEND TIDEMASTER_SOURCES hardware/ScreenControllerFactory.h)
  list(APPEND TIDEMASTER_SOURCES hardware/ScreenControllerFactory.cpp)
  list(APPEND TIDEMASTER_LINK_LIBRARIES PRIVATE Qt5::SerialPort)
endif()

set(TIDEMASTER_INCLUDE_NAME tide/master)

if(TIDE_ENABLE_REST_INTERFACE)
  list(APPEND TIDEMASTER_PUBLIC_HEADERS
    rest/AppRemoteController.h
    rest/FileBrowser.h
    rest/FileReceiver.h
    rest/HtmlContent.h
    rest/jsonschema.h
    rest/RestInterface.h
    rest/RestServer.h
    rest/ThumbnailCache.h
    rest/SceneRemoteController.h
    rest/serialization.h
    rest/templates.h
  )
  list(APPEND TIDEMASTER_SOURCES
    rest/AppRemoteController.cpp
    rest/FileBrowser.cpp
    rest/FileReceiver.cpp
    rest/HtmlContent.cpp
    rest/jsonschema.cpp
    rest/RestInterface.cpp
    rest/RestServer.cpp
    rest/ThumbnailCache.cpp
    rest/SceneRemoteController.cpp
    rest/serialization.cpp
  )
  list(APPEND TIDEMASTER_LINK_LIBRARIES PRIVATE Rockets)
endif()

common_library(TideMaster)
