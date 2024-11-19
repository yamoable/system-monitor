cmake_minimum_required(VERSION 3.10)

# 設定版本變數
if(NOT DEFINED APP_VERSION)
    set(APP_VERSION "1.0.0")
endif()

project(system-monitor VERSION ${APP_VERSION})

# 設置 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 設置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 創建版本配置檔
configure_file(
    ${CMAKE_SOURCE_DIR}/include/version.h.in
    ${CMAKE_BINARY_DIR}/include/version.h
)

# 找尋 Qt 套件
find_package(Qt5 COMPONENTS 
    Core
    Gui
    Widgets
    Charts
    Network
    REQUIRED
)

# 設置源文件
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/system_monitor.cpp
    src/ota_manager.cpp
)

# 設置頭文件
set(HEADERS
    include/mainwindow.h
    include/system_monitor.h
    include/ota_manager.h
)

# 創建可執行文件
add_executable(system-monitor
    ${SOURCES}
    ${HEADERS}
)

# 包含目錄
target_include_directories(system-monitor PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

# 連結 Qt 庫
target_link_libraries(system-monitor PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Charts
    Qt5::Network
)

# 設定版本定義
target_compile_definitions(system-monitor PRIVATE
    APP_VERSION="${APP_VERSION}"
)