cmake_minimum_required(VERSION 3.0)

project(vhdl_parser)

add_executable(
    vhdl_parser 
    main.cpp
    src/parser/node.cpp
    src/parser/node_generator.cpp
    src/parser/parser.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -O3 -std=c++17")

install(TARGETS vhdl_parser RUNTIME DESTINATION bin)
