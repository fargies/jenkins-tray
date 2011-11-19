
set(jenkinsTray_SOURCES
    jenkinsTray.cpp
    jenkinsRSSEngine.cpp
    jenkinsRSSParser.cpp
    jenkinsDownloader.cpp
    jenkinsMenu.cpp
    jenkinsProject.cpp
    main.cpp)
set(jenkinsTray_HEADERS
    jenkinsTray.hh
    jenkinsRSSEngine.hh
    jenkinsRSSParser.hh
    jenkinsMenu.hh
    jenkinsProject.hh
    jenkinsDownloader.hh)
set(jenkinsTray_RESOURCES jenkinsTray.qrc)

QT4_WRAP_CPP(jenkinsTray_HEADERS_MOC ${jenkinsTray_HEADERS})
QT4_ADD_RESOURCES(jenkinsTray_RESOURCES_RCC ${jenkinsTray_RESOURCES})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(jenkinsTray
    ${jenkinsTray_SOURCES} ${jenkinsTray_HEADERS}
    ${jenkinsTray_HEADERS_MOC} ${jenkinsTray_RESOURCES_RCC})
target_link_libraries(jenkinsTray ${QT_LIBRARIES})

