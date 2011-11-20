
set(jenkinsTray_SOURCES
    Tray.cpp
    RSSParser.cpp
    BuildParser.cpp
    Downloader.cpp
    Menu.cpp
    Project.cpp
    main.cpp)
set(jenkinsTray_HEADERS
    Tray.hh
    RSSParser.hh
    BuildParser.hh
    Menu.hh
    Project.hh
    Downloader.hh)
set(jenkinsTray_RESOURCES Tray.qrc)

QT4_WRAP_CPP(jenkinsTray_HEADERS_MOC ${jenkinsTray_HEADERS})
QT4_ADD_RESOURCES(jenkinsTray_RESOURCES_RCC ${jenkinsTray_RESOURCES})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(jenkinsTray
    ${jenkinsTray_SOURCES} ${jenkinsTray_HEADERS}
    ${jenkinsTray_HEADERS_MOC} ${jenkinsTray_RESOURCES_RCC})
target_link_libraries(jenkinsTray ${QT_LIBRARIES})

