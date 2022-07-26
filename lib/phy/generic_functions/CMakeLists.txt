#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Initialise DFT sources and definitions for DFT libraries.
set(SRSGNB_DFT_SOURCES dft_processor_generic_impl.cpp)
set(SRSGNB_DFT_DEFINITIONS "")
set(SRSGNB_DFT_LIBRARIES "")
set(SRSGNB_DFT_INCLUDE_DIRS "")
set(SRSGNB_DFT_LIBRARY_DIRS "")

# Find and add FFTX sources.
file(GLOB FFTX_DFT_SOURCES "fftx/lib_fftx_dftbat_srcs/*.cpp")
file(GLOB FFTX_IDFT_SOURCES "fftx/lib_fftx_idftbat_srcs/*.cpp")
list(APPEND SRSGNB_DFT_SOURCES ${FFTX_DFT_SOURCES} ${FFTX_IDFT_SOURCES} dft_processor_fftx_impl.cpp)

# If FFTW is enabled and found, then add FFTW source and libraries.
if (ENABLE_FFTW AND FFTW3F_FOUND)
    list(APPEND SRSGNB_DFT_SOURCES dft_processor_fftw_impl.cpp)
    list(APPEND SRSGNB_DFT_DEFINITIONS -DENABLE_FFTW)
    list(APPEND SRSGNB_DFT_LIBRARIES ${FFTW3F_LIBRARIES})
    list(APPEND SRSGNB_DFT_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIRS})
    list(APPEND SRSGNB_DFT_LIBRARY_DIRS ${FFTW3F_LIBRARY_DIRS})
endif (ENABLE_FFTW AND FFTW3F_FOUND)

add_definitions(${SRSGNB_DFT_DEFINITIONS})
include_directories(${SRSGNB_DFT_INCLUDE_DIRS})
link_directories(${SRSGNB_DFT_LIBRARY_DIRS})

message(STATUS "FFT_LIBRARIES: " ${SRSGNB_DFT_LIBRARIES})

add_library(srsgnb_generic_funcs_dft STATIC ${SRSGNB_DFT_SOURCES})
target_link_libraries(srsgnb_generic_funcs_dft ${SRSGNB_DFT_LIBRARIES} srsvec srslog)

# Set specific compilation options for certain FFTX implementations.
foreach (FFTX_SOURCE_FILE ${FFTX_DFT_SOURCES} ${FFTX_IDFT_SOURCES})
    set_source_files_properties(${FFTX_SOURCE_FILE} PROPERTIES COMPILE_OPTIONS "-Wno-unused-function;")
endforeach ()
if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-fno-aggressive-loop-optimizations" HAVE)
    if (${HAVE})
        set_source_files_properties(fftx/lib_fftx_dftbat_srcs/fftx_dftbat_1_36864_1d_CPU.cpp PROPERTIES COMPILE_OPTIONS "-fno-aggressive-loop-optimizations;")
        set_source_files_properties(fftx/lib_fftx_dftbat_srcs/fftx_dftbat_1_49152_1d_CPU.cpp PROPERTIES COMPILE_OPTIONS "-fno-aggressive-loop-optimizations;")
    endif (${HAVE})
endif (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

add_library(srsgnb_generic_funcs STATIC generic_functions_factories.cpp)
target_link_libraries(srsgnb_generic_funcs srsgnb_generic_funcs_dft)
