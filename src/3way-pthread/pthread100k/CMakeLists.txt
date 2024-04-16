cmake_minimum_required (VERSION 3.0)
project(pthread)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror -D_XOPEN_SOURCE=500")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror -Wno-sign-compare -D_XOPEN_SOURCE=500")

# make an executable
add_executable(pthread1 pthread100k.c)

target_compile_definitions(pthread1 PRIVATE)

