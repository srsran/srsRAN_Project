#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(evm_calculator_test evm_calculator_test.cpp)
target_link_libraries(evm_calculator_test srsran_channel_modulation srsvec srslog gtest gtest_main)
add_test(evm_calculator_test evm_calculator_test)

if (USE_PHY_TESTVECTORS)
    add_executable(demodulation_mapper_test demodulation_mapper_test.cpp)
    target_link_libraries(demodulation_mapper_test srsran_channel_modulation srsvec srslog gtest gtest_main)
    add_test_vector(demodulation_mapper_test demodulation_mapper_test_data.tar.gz "")

    add_executable(modulation_mapper_test modulation_mapper_test.cpp)
    target_link_libraries(modulation_mapper_test srsran_channel_modulation srsvec srslog)
    add_test_vector(modulation_mapper_test modulation_mapper_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)
