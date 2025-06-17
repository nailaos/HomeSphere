cmake_minimum_required(VERSION 3.10)
project(SmartHomeSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

find_package(OpenMP REQUIRED)

add_executable(SmartHomeSim ${SOURCES})

if(OpenMP_CXX_FOUND)
    target_link_libraries(SmartHomeSim PUBLIC OpenMP::OpenMP_CXX)
endif()