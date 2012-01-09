
if (TESTS)

link_directories(${CMAKE_BINARY_DIR}/src)
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_library(testHelper SHARED
    test_main.cpp
    test_helpers.cpp test_helpers.hh)
target_link_libraries(testHelper ${CPPUNIT_LIBRARIES})

set(test_HEADERS stub_Jenkins.hh stub_Tray.hh)

QT4_WRAP_CPP(test_HEADERS_MOC ${test_HEADERS})
QT4_ADD_RESOURCES(test_RESOURCES_RCC test_resources.qrc)

set(test_SRCS
    stub_Tray.cpp
    stub_Settings.cpp stub_Settings.hh
    stub_Jenkins.cpp
    test_simple.cpp ${test_HEADERS}
    ${test_HEADERS_MOC} ${test_RESOURCES_RCC})

add_executable(test_all ${test_SRCS})
target_link_libraries(test_all testHelper libjenkinsTray)

add_test(test_all ${CMAKE_CURRENT_BINARY_DIR}/test_all)

endif()

