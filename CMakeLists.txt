cmake_minimum_required(VERSION 3.16)

project(TeamSpeak-Overlay VERSION 1.0.3 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Network WebSockets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Network WebSockets Sql)

add_subdirectory(src/thirdparty/singleapplication)

set(PROJECT_SOURCES
        src/main.cpp
        src/ui/OverlayWindow.cpp
        src/ui/OverlayWindow.h
        src/ui/OverlayWindow.ui
        src/ui/UserBubble.h
        src/ui/UserBubble.cpp
        src/ui/MessageBubble.h
        src/ui/MessageBubble.cpp
        src/ui/SettingsWindow.h
        src/ui/SettingsWindow.cpp
        src/ui/SettingsWindow.ui
        src/network/WebsocketManager.h
        src/network/WebsocketManager.cpp
        src/core/Logger.cpp
        src/core/Logger.h
        src/core/Constants.h
        src/core/DatabaseManager.h
        src/core/DatabaseManager.cpp
        ${TS_FILES}
)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TeamSpeak-Overlay
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            resources.qrc
            resources.rc
            ${app_icon_resource_windows}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(TeamSpeak-Overlay
            ${PROJECT_SOURCES}
            resources.qrc
            resources.rc
            ${app_icon_resource_windows}
    )

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(TeamSpeak-Overlay PRIVATE ${X11_LIBRARIES} ${X11_Xext_LIB})
    target_include_directories(TeamSpeak-Overlay PRIVATE ${X11_INCLUDE_DIR})
endif()

target_link_libraries(TeamSpeak-Overlay PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::WebSockets Qt${QT_VERSION_MAJOR}::Sql SingleApplication::SingleApplication)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.dex.TeamSpeak-Overlay)
endif()
set_target_properties(TeamSpeak-Overlay PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TeamSpeak-Overlay
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TeamSpeak-Overlay)
endif()
