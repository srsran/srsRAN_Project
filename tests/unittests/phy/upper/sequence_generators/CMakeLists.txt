#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

if (USE_PHY_TESTVECTORS)
    add_executable(low_papr_sequence_generator_test low_papr_sequence_generator_test.cpp)
    target_link_libraries(low_papr_sequence_generator_test srsran_sequence_generators srsvec srslog srsran_support gtest gtest_main)
    add_test_vector(low_papr_sequence_generator_test low_papr_sequence_generator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)

add_executable(pseudo_random_test pseudo_random_generator_test.cpp)
target_link_libraries(pseudo_random_test srsran_sequence_generators srsvec srslog gtest gtest_main)
add_test(pseudo_random_test pseudo_random_test)
