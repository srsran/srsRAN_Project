

########################################################################
# Prevent in-tree builds
########################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.10)
project(srsgnb)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
   message(STATUS "Build type not specified: defaulting to Release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# Options
########################################################################

option(ASSERTS_ENABLED       "Enable srsRAN asserts"                    ON)
option(STOP_ON_WARNING       "Interrupt application on warning"         OFF)
option(ENABLE_TSAN           "Enable clang thread sanitizer"            OFF)

if (ASSERTS_ENABLED)
    add_definitions(-DASSERTS_ENABLED)
endif()

if (ENABLE_TSAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -DHAVE_TSAN")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif (ENABLE_TSAN)

########################################################################
# Compiler specific setup
########################################################################
macro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${flag} ${have})
    if (${have})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endif (${have})
endmacro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

# Make sure no instance of abstract class is left without a destructor
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wnon-virtual-dtor HAVE_NON_VIRTUAL_DTOR)

# Make sure all overridden methods are marked as override
ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wsuggest-override HAVE_SUGGEST_OVERRIDE)

## Avoid shadow variables which can be caused due to C code ported into C++
#ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wshadow HAVE_SHADOW)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
else(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  if(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
  else(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-trapping-math -fno-math-errno -DBUILD_TYPE_RELEASE")
  endif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

########################################################################
# Find dependencies
########################################################################

# FFT
find_package(FFTW3F REQUIRED)
if (FFTW3F_FOUND)
    include_directories(${FFTW3F_INCLUDE_DIRS})
    link_directories(${FFTW3F_LIBRARY_DIRS})
    if (BUILD_STATIC)
        set(FFT_LIBRARIES "${FFTW3F_STATIC_LIBRARIES}")
    else (BUILD_STATIC)
        set(FFT_LIBRARIES "${FFTW3F_LIBRARIES}")
    endif (BUILD_STATIC)
    message(STATUS "FFT_LIBRARIES: " ${FFT_LIBRARIES})
endif (FFTW3F_FOUND)

########################################################################
# Compiler launcher setup
########################################################################

# Enable ccache if not already enabled
find_program(CCACHE_EXECUTABLE ccache)
mark_as_advanced(CCACHE_EXECUTABLE)
if (CCACHE_EXECUTABLE)
    foreach (LANG C CXX)
        if (NOT DEFINED CMAKE_${LANG}_COMPILER_LAUNCHER AND NOT CMAKE_${LANG}_COMPILER MATCHES ".*/ccache$")
            message(STATUS "Enabling ccache for ${LANG}")
            set(CMAKE_${LANG}_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE} CACHE STRING "")
        endif ()
    endforeach ()
endif ()


include(CTest)

include_directories(include)

add_subdirectory(lib)
add_subdirectory(units)
add_subdirectory(unittests)
add_subdirectory(docs)

