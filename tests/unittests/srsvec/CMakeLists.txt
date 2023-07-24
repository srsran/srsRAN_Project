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

add_executable(srsvec_add_test srsvec_add_test.cpp)
target_link_libraries(srsvec_add_test srsvec srslog)
add_test(srsvec_add_test srsvec_add_test)

add_executable(srsvec_aligned_test srsvec_aligned_test.cpp)
target_link_libraries(srsvec_aligned_test srsvec srslog)
add_test(srsvec_aligned_test srsvec_aligned_test)

add_executable(srsvec_bit_test srsvec_bit_test.cpp)
target_link_libraries(srsvec_bit_test srsvec srslog)
add_test(srsvec_bit_test srsvec_bit_test)

add_executable(srsvec_compare_test srsvec_compare_test.cpp)
target_link_libraries(srsvec_compare_test srsvec srslog)
add_test(srsvec_compare_test srsvec_compare_test)

add_executable(srsvec_binary_test srsvec_binary_test.cpp)
target_link_libraries(srsvec_binary_test srsvec srslog)
add_test(srsvec_binary_test srsvec_binary_test)

add_executable(srsvec_convert_test srsvec_convert_test.cpp)
target_link_libraries(srsvec_convert_test srsvec srslog)
add_test(srsvec_convert_test srsvec_convert_test)

add_executable(srsvec_division_test srsvec_division_test.cpp)
target_link_libraries(srsvec_division_test srsvec srslog)
add_test(srsvec_division_test srsvec_division_test)

add_executable(srsvec_dot_prod_test srsvec_dot_prod_test.cpp)
target_link_libraries(srsvec_dot_prod_test srsvec srslog)
add_test(srsvec_dot_prod_test srsvec_dot_prod_test)

add_executable(srsvec_modulus_square_test srsvec_modulus_square_test.cpp)
target_link_libraries(srsvec_modulus_square_test srsvec srslog)
add_test(srsvec_modulus_square_test srsvec_modulus_square_test)

add_executable(srsvec_prod_test srsvec_prod_test.cpp)
target_link_libraries(srsvec_prod_test srsvec srslog)
add_test(srsvec_prod_test srsvec_prod_test)

add_executable(srsvec_sc_prod_test srsvec_sc_prod_test.cpp)
target_link_libraries(srsvec_sc_prod_test srsvec srslog)
add_test(srsvec_sc_prod_test srsvec_sc_prod_test)

add_executable(srsvec_clipping_test srsvec_clipping_test.cpp)
target_link_libraries(srsvec_clipping_test srsvec srslog)
add_test(srsvec_clipping_test srsvec_clipping_test)

add_executable(srsvec_convolution_test srsvec_convolution_test.cpp)
target_link_libraries(srsvec_convolution_test srsvec srslog gtest gtest_main)
add_test(srsvec_convolution_test srsvec_convolution_test)
