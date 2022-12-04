cmake_minimum_required(VERSION 3.10)
project(GroundStation)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror -mtune=native -march=native")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5SerialPort REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        SerialPortHandler.hpp
        Controller.cpp
        Widgets/DataField.cpp
        Widgets/MainWindow.cpp
        Widgets/LoggingWidget.cpp
        Widgets/ConnectionWidget.cpp
        Widgets/FcWidget.cpp
        plot.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::SerialPort ToolboxPlaneMessageConversion)
add_subdirectory(Messages)
