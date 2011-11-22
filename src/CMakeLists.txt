
set(jenkinsTray_SOURCES
    Tray.cpp
    RSSParser.cpp
    BuildParser.cpp
    Downloader.cpp
    Menu.cpp
    Project.cpp
    Settings.cpp
    main.cpp)
set(jenkinsTray_HEADERS
    Tray.hh
    RSSParser.hh
    BuildParser.hh
    Menu.hh
    Project.hh
    Settings.hh
    Downloader.hh)
set(jenkinsTray_RESOURCES Tray.qrc)
set(jenkinsTray_UI Config.ui)

QT4_WRAP_CPP(jenkinsTray_HEADERS_MOC ${jenkinsTray_HEADERS})
QT4_ADD_RESOURCES(jenkinsTray_RESOURCES_RCC ${jenkinsTray_RESOURCES})
QT4_WRAP_UI(jenkinsTray_UIC ${jenkinsTray_UI})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(jenkinsTray
    ${jenkinsTray_SOURCES} ${jenkinsTray_HEADERS}
    ${jenkinsTray_HEADERS_MOC} ${jenkinsTray_RESOURCES_RCC}
    ${jenkinsTray_UIC})
target_link_libraries(jenkinsTray ${QT_LIBRARIES})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

