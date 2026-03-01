cmake_minimum_required(VERSION 3.20)
project(IniReader)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
    Sources/IniManager.cpp
    Sources/IniObject.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC Sources)
