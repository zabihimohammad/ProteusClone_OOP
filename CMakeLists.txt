cmake_minimum_required(VERSION 3.5)
project(ProteusClone_OOP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# اگر کیوتی در سیستم شما بدون این خط شناسایی می‌شود، این خط کامنت می‌ماند.
# در غیر این صورت، هشتگ را بردارید و فقط مسیر سیستم خودتان را با یک جفت کوتیشن بنویسید.
# set(CMAKE_PREFIX_PATH "C:/Qt/6.x.x/mingw_64")

# این سه خط قابلیت‌های تولید خودکار کدهای مخفی Qt را روشن می‌کنند
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

# ترکیب فایل‌های هر دو نفر (Merge)
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
        src/components/peripherals.cpp
        src/components/peripherals.h
        src/gui/hex_upload_dialog.cpp
        src/gui/hex_upload_dialog.h
        src/gui/properties_dialog.cpp
        src/gui/properties_dialog.h
        src/core/element.cpp
        src/core/element.h
        src/core/terminal.cpp
        src/core/terminal.h
        src/core/wire.cpp
        src/core/wire.h
        src/core/auto_router.cpp
        src/core/auto_router.h

        # --- کدهای شما (موتور شبیه‌ساز و پروب هوشمند) ---
        src/core/probe_item.cpp
        src/core/probe_item.h
        src/core/simulation_engine.cpp
        src/core/simulation_engine.h

        # --- کدهای هم‌گروهی شما (مدیریت فایل‌های مدار) ---
        src/io/file_manager.cpp
        src/io/file_manager.h
        )

add_executable(ProteusClone_OOP WIN32 ${PROJECT_SOURCES})
target_link_libraries(ProteusClone_OOP PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)