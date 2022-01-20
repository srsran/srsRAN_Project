cmake_minimum_required(VERSION 2.6)
project(srsgnb)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include(CTest)

include_directories(include)

add_subdirectory(lib)
add_subdirectory(units)
add_subdirectory(unittests)

