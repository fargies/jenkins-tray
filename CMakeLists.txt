
cmake_minimum_required(VERSION 2.8)

project(jenkins-tray)

find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)

include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)

option(TESTS "Compile and run unit tests" OFF)
if (TESTS)
    include(FindPkgConfig)
    pkg_check_modules(CPPUNIT REQUIRED cppunit)
endif (TESTS)


configure_file("config.h.in" "config.h")

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

