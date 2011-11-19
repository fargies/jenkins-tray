
cmake_minimum_required(VERSION 2.8)

project(jenkins-tray CXX)

find_package(Qt4 REQUIRED)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)

add_subdirectory(src)

