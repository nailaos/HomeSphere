cmake_minimum_required(VERSION 3.10)
project(HomeSphere)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找线程库
find_package(Threads REQUIRED)

# 包含目录
include_directories(include)

# 源文件
set(SOURCES
    src/main.cpp
    src/room.cpp
    src/device.cpp
    src/light.cpp
    src/sensor.cpp
    src/airConditioner.cpp
    src/sceneSimulation.cpp
    src/SmartLogger.cpp
    src/user.cpp
)

# 创建可执行文件
add_executable(HomeSphere ${SOURCES})

# 链接线程库
target_link_libraries(HomeSphere Threads::Threads)

# 创建logs目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)