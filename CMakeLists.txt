
cmake_minimum_required(VERSION 2.8)

project(jenkins-tray)

find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)

include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)

configure_file("config.h.in" "config.h")

add_subdirectory(src)
add_subdirectory(tests)

