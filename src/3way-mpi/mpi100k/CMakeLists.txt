cmake_minimum_required (VERSION 3.0)
project(mpi)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror -D_XOPEN_SOURCE=500")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror -Wno-sign-compare -D_XOPEN_SOURCE=500")

find_package(MPI REQUIRED)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

# make an executable
add_executable(mpi1 mpi100k.c)
target_link_libraries(mpi1 ${MPI_LIBRARIES})

