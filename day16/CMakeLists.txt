cmake_minimum_required(VERSION 3.0)
project( day16 )
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-declarations>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wno-switch>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-DNO_EXPLICIT_INSTANTIATIONS>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wno-psabi>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-Wextra>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-pedantic>")

add_executable(p1 src/p1.cpp)
