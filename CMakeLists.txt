cmake_minimum_required(VERSION 3.8.0)

# Define C++ variables
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define lightex_root variable
get_filename_component(lightex_root . ABSOLUTE)

# Define LighTeX static library
include_directories(${lightex_root})
set(lightex_src_files ${lightex_src_files}
    ${lightex_root}/lightex/lightex.cc
    ${lightex_root}/lightex/utils/counter.cc
    ${lightex_root}/lightex/visitor/dot_visitor.cc)
add_library(lightex STATIC ${lightex_src_files})

# Find Boost
find_package(Boost 1.63 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

# Define LighTeX binaries
add_executable(parse_program_to_dot ${lightex_root}/lightex/binaries/parse_program_to_dot.cc)
target_link_libraries(parse_program_to_dot lightex)
