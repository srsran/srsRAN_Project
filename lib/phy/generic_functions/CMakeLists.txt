#
# Copyright 2021-2023 Software Radio Systems Limited
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