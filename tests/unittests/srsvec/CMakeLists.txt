#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(srsvec_add_test srsvec_add_test.cpp)
target_link_libraries(srsvec_add_test srsvec srslog)
add_test(srsvec_add_test srsvec_add_test)

add_executable(srsvec_bit_test srsvec_bit_test.cpp)
target_link_libraries(srsvec_bit_test srsvec srslog gtest gtest_main)
add_test(srsvec_bit_test srsvec_bit_test)

add_executable(srsvec_compare_test srsvec_compare_test.cpp)
target_link_libraries(srsvec_compare_test srsvec srslog)
add_test(srsvec_compare_test srsvec_compare_test)

add_executable(srsvec_binary_test srsvec_binary_test.cpp)
target_link_libraries(srsvec_binary_test srsvec srslog)
add_test(srsvec_binary_test srsvec_binary_test)

add_executable(srsvec_convert_test srsvec_convert_test.cpp)
target_link_libraries(srsvec_convert_test srsvec srslog gtest gtest_main)
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
target_link_libraries(srsvec_prod_test srsvec srslog gtest gtest_main)
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

add_executable(srsvec_unwrap_test srsvec_unwrap_test.cpp)
target_link_libraries(srsvec_unwrap_test srsvec srslog gtest gtest_main)
add_test(srsvec_unwrap_test srsvec_unwrap_test)
