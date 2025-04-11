#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(pdcch)
add_subdirectory(pdsch)
add_subdirectory(pucch)
add_subdirectory(pusch)
add_subdirectory(ssb)
add_subdirectory(uci)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

if (USE_PHY_TESTVECTORS)
    add_executable(prach_detector_vectortest prach_detector_vectortest.cpp)
    target_link_libraries(prach_detector_vectortest srsran_channel_processors srsran_generic_funcs srsran_phy_support srslog gtest gtest_main)
    add_test_vector(prach_detector_vectortest prach_detector_test_data.tar.gz "")

    add_executable(prach_generator_vectortest prach_generator_vectortest.cpp)
    target_link_libraries(prach_generator_vectortest srsran_channel_processors srsran_generic_funcs srslog gtest gtest_main)
    add_test_vector(prach_generator_vectortest prach_generator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)
