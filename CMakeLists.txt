cmake_minimum_required(VERSION 3.10)
project(ScokLibrary C)

set(CMAKE_C_COMPILER clang)

include_directories(include)

add_library(scok SHARED src/lib.c)

set_target_properties(scok PROPERTIES OUTPUT_NAME "scok")

install(TARGETS scok DESTINATION /usr/lib)
install(FILES include/scok.h DESTINATION /usr/include)