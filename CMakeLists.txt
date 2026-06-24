cmake_minimum_required(VERSION 3.5)
project(ProteusClone_OOP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
)

add_executable(ProteusClone_OOP ${PROJECT_SOURCES})
target_link_libraries(ProteusClone_OOP PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
