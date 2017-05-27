cmake_minimum_required(VERSION 3.7.1)

# Define C++ variables
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

# Define lightex_root variable
get_filename_component(lightex_root . ABSOLUTE)

# Define LighTeX static library
include_directories(${lightex_root})
set(lightex_src_files ${lightex_src_files}
    ${lightex_root}/lightex/workspace.cc
    ${lightex_root}/lightex/workspace.h
    ${lightex_root}/lightex/ast/ast.h
    ${lightex_root}/lightex/ast/ast_adapted.h
    ${lightex_root}/lightex/dot_converter/dot_visitor.cc
    ${lightex_root}/lightex/dot_converter/dot_visitor.h
    ${lightex_root}/lightex/grammar/grammar.h
    ${lightex_root}/lightex/html_converter/html_visitor.cc
    ${lightex_root}/lightex/html_converter/html_visitor.h
    ${lightex_root}/lightex/utils/file_utils.cc
    ${lightex_root}/lightex/utils/file_utils.h
)
add_library(lightex STATIC ${lightex_src_files})

# Find Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.64 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})

# Define LighTeX binaries
add_executable(parse_program_to_dot ${lightex_root}/lightex/binaries/parse_program_to_dot.cc)
target_link_libraries(parse_program_to_dot lightex)

add_executable(parse_program_to_html ${lightex_root}/lightex/binaries/parse_program_to_html.cc)
target_link_libraries(parse_program_to_html lightex)

# Unittest
add_executable(tester ${lightex_root}/tests/main.cc)
target_link_libraries(tester lightex ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
