#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(precoding)
add_subdirectory(transform_precoding)

# Initialise DFT sources and definitions for DFT libraries.
set(SRSRAN_DFT_SOURCES dft_processor_generic_impl.cpp)
set(SRSRAN_DFT_DEFINITIONS "")
set(SRSRAN_DFT_LIBRARIES "")
set(SRSRAN_DFT_INCLUDE_DIRS "")
set(SRSRAN_DFT_LIBRARY_DIRS "")

# If MKL is enabled and found, then compile DFT FFTW API based and link it to MKL.
if (ENABLE_MKL AND MKL_FOUND)
    list(APPEND SRSRAN_DFT_SOURCES dft_processor_fftw_impl.cpp)
    list(APPEND SRSRAN_DFT_DEFINITIONS ENABLE_FFTW)
    list(APPEND SRSRAN_DFT_LIBRARIES ${MKL_LIBRARIES})
    list(APPEND SRSRAN_DFT_INCLUDE_DIRS ${MKL_INCLUDE_DIRS})
    list(APPEND SRSRAN_DFT_LIBRARY_DIRS ${MKL_LIBRARY_DIRS})
elseif (ENABLE_FFTW AND FFTW3F_FOUND)
    # If FFTW is enabled and found, then add FFTW source and libraries.
    list(APPEND SRSRAN_DFT_SOURCES dft_processor_fftw_impl.cpp)
    list(APPEND SRSRAN_DFT_DEFINITIONS ENABLE_FFTW)
    list(APPEND SRSRAN_DFT_LIBRARIES ${FFTW3F_LIBRARIES})
    list(APPEND SRSRAN_DFT_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIRS})
    list(APPEND SRSRAN_DFT_LIBRARY_DIRS ${FFTW3F_LIBRARY_DIRS})
else (ENABLE_MKL AND MKL_FOUND)
    message(FATAL_ERROR "Missing FFTW library. Please install libfftw3 or MKL.")
endif (ENABLE_MKL AND MKL_FOUND)

include_directories(${SRSRAN_DFT_INCLUDE_DIRS})
link_directories(${SRSRAN_DFT_LIBRARY_DIRS})

add_library(srsran_generic_funcs_dft STATIC ${SRSRAN_DFT_SOURCES})
target_link_libraries(srsran_generic_funcs_dft ${SRSRAN_DFT_LIBRARIES} srsvec srslog)

add_library(srsran_generic_funcs STATIC generic_functions_factories.cpp)
set_source_files_properties(generic_functions_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${SRSRAN_DFT_DEFINITIONS}")
target_link_libraries(srsran_generic_funcs srsran_generic_funcs_dft srslog)

add_to_exported_libs(srsran_generic_funcs srsran_generic_funcs_dft)
