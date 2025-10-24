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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_subdirectory(precoding)
add_subdirectory(transform_precoding)

add_executable(dft_processor_test dft_processor_test.cpp)
target_link_libraries(dft_processor_test srslog srsran_dft gtest gtest_main)
add_test(dft_processor_test dft_processor_test)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    add_executable(dft_processor_ci16_test dft_processor_ci16_test.cpp)
    target_link_libraries(dft_processor_ci16_test srslog srsran_dft gtest gtest_main)
    add_test(dft_processor_ci16_test dft_processor_ci16_test)
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))
    target_compile_definitions(dft_processor_test PRIVATE HAVE_FFTW)
endif ((ENABLE_MKL AND MKL_FOUND) OR (ENABLE_ARMPL AND ARMPL_FOUND) OR (ENABLE_FFTW AND FFTW3F_FOUND))

if (USE_PHY_TESTVECTORS)
    add_executable(dft_processor_vectortest dft_processor_vectortest.cpp)
    target_link_libraries(dft_processor_vectortest srslog srsran_dft gtest gtest_main)
    add_test_vector(dft_processor_vectortest dft_processor_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)
