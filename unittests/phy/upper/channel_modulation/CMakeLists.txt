#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(modulation_mapper_test modulation_mapper_test.cpp)
target_link_libraries(modulation_mapper_test srsgnb_channel_modulation srsvec srslog)
add_test(modulation_mapper_test modulation_mapper_test)

add_custom_command(
        TARGET modulation_mapper_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/modulation_mapper_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/modulation_mapper_test_data.tar.gz
        VERBATIM)

add_executable(modulation_chain_benchmark modulation_chain_benchmark.cpp)
target_link_libraries(modulation_chain_benchmark srsgnb_channel_modulation srsvec srslog)
add_test(modulation_chain_benchmark modulation_chain_benchmark -F generic -s -R 1)

add_executable(demodulation_mapper_test demodulation_mapper_test.cpp)
target_link_libraries(demodulation_mapper_test srsgnb_channel_modulation srsvec srslog)

add_custom_command(
        TARGET demodulation_mapper_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/demodulation_mapper_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/demodulation_mapper_test_data.tar.gz
        VERBATIM)

add_test(demodulation_mapper_test demodulation_mapper_test)
