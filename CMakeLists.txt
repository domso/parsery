cmake_minimum_required(VERSION 3.0)

project(vhdl_parser)

add_executable(
    vhdl_parser 
    main.cpp
    src/parser/node.cpp
    src/parser/node_generator.cpp
    src/parser/parser.cpp
)

install(TARGETS vhdl_parser RUNTIME DESTINATION bin)
