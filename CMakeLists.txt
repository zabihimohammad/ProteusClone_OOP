cmake_minimum_required(VERSION 3.5)
project(ProteusClone_OOP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# این سه خط قابلیت‌های تولید خودکار کدهای مخفی Qt را روشن می‌کنند
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

# لیست تمام فایل‌های پروژه (فراموش نکنید اگر فایلی اضافه شد اینجا هم بنویسید)
set(PROJECT_SOURCES
        src/main.cpp
        src/gui/mainwindow.cpp
        src/gui/mainwindow.h
        src/canvas/circuit_scene.cpp
        src/canvas/circuit_scene.h
        src/canvas/circuit_view.cpp
        src/canvas/circuit_view.h
        src/components/basic_components.cpp
        src/components/basic_components.h
        src/components/logic_gates.cpp
        src/components/logic_gates.h
        src/components/mcu.cpp
        src/components/mcu.h
        # (در بخش PROJECT_SOURCES)
        src/components/peripherals.cpp
        src/components/peripherals.h
        # (در بخش PROJECT_SOURCES)
        src/gui/hex_upload_dialog.cpp
        src/gui/hex_upload_dialog.h
        # (در بخش PROJECT_SOURCES خطوط مربوط به core را اضافه کنید)
        src/core/element.cpp
        src/core/element.h
        src/core/terminal.cpp
        src/core/terminal.h
        src/core/wire.cpp
        src/core/wire.h
        )

add_executable(ProteusClone_OOP ${PROJECT_SOURCES})
target_link_libraries(ProteusClone_OOP PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)