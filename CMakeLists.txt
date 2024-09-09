cmake_minimum_required(VERSION 3.0)

project(parsery)

include_directories(./src/)

add_library(
    parsery 
    src/frontend/syntax_generator.cpp
    src/frontend/syntax_converter.cpp
    src/frontend/rule_importer.cpp
    src/frontend/syntax_element.cpp
    src/backend/call_stack.cpp
    src/backend/node_stack.cpp
    src/parser.cpp
)

add_executable(
    parser_example 
    examples/main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -std=c++20 -g -O3")

target_link_libraries(parser_example parsery)
