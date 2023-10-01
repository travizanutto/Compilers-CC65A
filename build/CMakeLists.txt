cmake_minimum_required(VERSION 3.21)

project(nag_to_jacomo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin )

set(SRC_DIR "../src")
set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}")

find_package(FLEX)
find_package(BISON)

set(LEXER_OUT "${PARSER_DIR}/lex.c")
set(PARSER_OUT "${PARSER_DIR}/parse.c")

flex_target(LEXER "${SRC_DIR}/lex.l" "${LEXER_OUT}" DEFINES_FILE "${PARSER_DIR}/lex.h")
bison_target(PARSER "${SRC_DIR}/parse.y" "${PARSER_OUT}" DEFINES_FILE "${PARSER_DIR}/parse.h")
add_flex_bison_dependency(LEXER PARSER)

add_executable(nag_to_jacomo "${LEXER_OUT}" "${PARSER_OUT}")
target_include_directories(nag_to_jacomo PRIVATE "${PARSER_DIR}")
