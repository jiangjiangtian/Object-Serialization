cmake_minimum_required(VERSION 3.08)

project(serialization)

include_directories(include/)

add_executable(test_binary
    src/test_binary.cpp
)

add_executable(test_xml
    src/test_xml.cpp
    src/tinyxml2.cpp
)