cmake_minimum_required(VERSION 3.16)
project(DenemeVTableCorrupt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(deneme
    src/main.cpp
    src/CDeneme.cpp
    src/DenemeFactory.cpp
)

target_include_directories(deneme PRIVATE include)
