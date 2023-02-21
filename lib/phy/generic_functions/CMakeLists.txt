#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Initialise DFT sources and definitions for DFT libraries.
set(SRSRAN_DFT_SOURCES dft_processor_generic_impl.cpp)
set(SRSRAN_DFT_DEFINITIONS "")
set(SRSRAN_DFT_LIBRARIES "")
set(SRSRAN_DFT_INCLUDE_DIRS "")
set(SRSRAN_DFT_LIBRARY_DIRS "")

# If FFTW is enabled and found, then add FFTW source and libraries.
if (ENABLE_FFTW AND FFTW3F_FOUND)
    list(APPEND SRSRAN_DFT_SOURCES dft_processor_fftw_impl.cpp)
    list(APPEND SRSRAN_DFT_DEFINITIONS ENABLE_FFTW)
    list(APPEND SRSRAN_DFT_LIBRARIES ${FFTW3F_LIBRARIES})
    list(APPEND SRSRAN_DFT_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIRS})
    list(APPEND SRSRAN_DFT_LIBRARY_DIRS ${FFTW3F_LIBRARY_DIRS})
endif (ENABLE_FFTW AND FFTW3F_FOUND)

include_directories(${SRSRAN_DFT_INCLUDE_DIRS})
link_directories(${SRSRAN_DFT_LIBRARY_DIRS})

message(STATUS "FFT_LIBRARIES: " ${SRSRAN_DFT_LIBRARIES})

add_library(srsran_generic_funcs_dft STATIC ${SRSRAN_DFT_SOURCES})
target_link_libraries(srsran_generic_funcs_dft ${SRSRAN_DFT_LIBRARIES} srsvec srslog)

add_library(srsran_generic_funcs STATIC generic_functions_factories.cpp)
set_source_files_properties(generic_functions_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${SRSRAN_DFT_DEFINITIONS}")
target_link_libraries(srsran_generic_funcs srsran_generic_funcs_dft srslog)

install(TARGETS srsran_generic_funcs
        srsran_generic_funcs_dft
        EXPORT srsran_export)