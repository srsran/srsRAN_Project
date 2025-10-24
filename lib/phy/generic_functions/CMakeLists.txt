#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_subdirectory(precoding)
add_subdirectory(transform_precoding)

# Initialise DFT sources and definitions for DFT libraries.
set(SRSRAN_DFT_SOURCES dft_processor_generic_impl.cpp generic_functions_factories.cpp)
set(SRSRAN_DFT_LIBRARIES "")
set(SRSRAN_DFT_INCLUDE_DIRS "")
set(SRSRAN_DFT_LIBRARY_DIRS "")
set(SRSRAN_DFT_DEFINITIONS "")

if (ENABLE_MKL AND MKL_FOUND)
    # If MKL is enabled and found, then compile DFT FFTW API based and link it to MKL.
    list(APPEND SRSRAN_DFT_LIBRARIES ${MKL_LIBRARIES})
    list(APPEND SRSRAN_DFT_INCLUDE_DIRS ${MKL_INCLUDE_DIRS})
    list(APPEND SRSRAN_DFT_LIBRARY_DIRS ${MKL_LIBRARY_DIRS})
elseif (ENABLE_ARMPL AND ARMPL_FOUND)
    # If ARMPL is enabled and found, then add FFTW source and libraries.
    list(APPEND SRSRAN_DFT_LIBRARIES ${ARMPL_LIBRARIES})
    list(APPEND SRSRAN_DFT_INCLUDE_DIRS ${ARMPL_INCLUDE_DIRS})
    list(APPEND SRSRAN_DFT_LIBRARY_DIRS ${ARMPL_LIBRARY_DIRS})
elseif (ENABLE_FFTW AND FFTW3F_FOUND)
    # If FFTW is enabled and found, then add FFTW source and libraries.
    list(APPEND SRSRAN_DFT_LIBRARIES ${FFTW3F_LIBRARIES})
    list(APPEND SRSRAN_DFT_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIRS})
    list(APPEND SRSRAN_DFT_LIBRARY_DIRS ${FFTW3F_LIBRARY_DIRS})
endif (ENABLE_MKL AND MKL_FOUND)

if ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))
    list(APPEND SRSRAN_DFT_SOURCES dft_processor_fftw_impl.cpp)
    list(APPEND SRSRAN_DFT_DEFINITIONS "HAVE_FFTW")
endif ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND SRSRAN_DFT_SOURCES dft_processor_ci16_avx2.cpp)
    set_source_files_properties(dft_processor_ci16_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

add_library(srsran_dft STATIC ${SRSRAN_DFT_SOURCES})
target_include_directories(srsran_dft PRIVATE ${SRSRAN_DFT_INCLUDE_DIRS})
target_link_directories(srsran_dft PRIVATE ${SRSRAN_DFT_LIBRARY_DIRS})
target_link_libraries(srsran_dft PRIVATE ${SRSRAN_DFT_LIBRARIES} srsvec srslog)
target_compile_definitions(srsran_dft PRIVATE ${SRSRAN_DFT_DEFINITIONS})

add_to_exported_libs(srsran_dft)
