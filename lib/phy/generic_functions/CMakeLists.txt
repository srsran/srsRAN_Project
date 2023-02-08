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

# If FFTW is enabled and found, then add FFTW source and libraries.
if (ENABLE_FFTW AND FFTW3F_FOUND)
    list(APPEND SRSGNB_DFT_SOURCES dft_processor_fftw_impl.cpp)
    list(APPEND SRSGNB_DFT_DEFINITIONS ENABLE_FFTW)
    list(APPEND SRSGNB_DFT_LIBRARIES ${FFTW3F_LIBRARIES})
    list(APPEND SRSGNB_DFT_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIRS})
    list(APPEND SRSGNB_DFT_LIBRARY_DIRS ${FFTW3F_LIBRARY_DIRS})
endif (ENABLE_FFTW AND FFTW3F_FOUND)

include_directories(${SRSGNB_DFT_INCLUDE_DIRS})
link_directories(${SRSGNB_DFT_LIBRARY_DIRS})

message(STATUS "FFT_LIBRARIES: " ${SRSGNB_DFT_LIBRARIES})

add_library(srsgnb_generic_funcs_dft STATIC ${SRSGNB_DFT_SOURCES})
target_link_libraries(srsgnb_generic_funcs_dft ${SRSGNB_DFT_LIBRARIES} srsvec srslog)

add_library(srsgnb_generic_funcs STATIC generic_functions_factories.cpp)
set_source_files_properties(generic_functions_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${SRSGNB_DFT_DEFINITIONS}")
target_link_libraries(srsgnb_generic_funcs srsgnb_generic_funcs_dft srslog)

install(TARGETS srsgnb_generic_funcs
        srsgnb_generic_funcs_dft
        EXPORT srsgnb_export)