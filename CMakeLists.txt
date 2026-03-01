cmake_minimum_required(VERSION 3.20)
project(IniReader)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
    Sources/IniManager.cpp
    Sources/IniObject.cpp
        Sources/SQRLLParser.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC Sources)

set_target_properties(IniReader PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
