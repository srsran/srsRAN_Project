#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

add_subdirectory(channel_coding)
add_subdirectory(channel_modulation)
add_subdirectory(channel_processors)
add_subdirectory(sequence_generators)
add_subdirectory(signal_processors)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(dl_processor_vectortest dl_processor_vectortest.cpp)
target_link_libraries(dl_processor_vectortest srsgnb_sequence_generators srsgnb_channel_processors srsgnb_signal_processors srslog)
add_test(dl_processor_vectortest dl_processor_vectortest)

add_custom_command(
        TARGET dl_processor_vectortest
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/dl_processor_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dl_processor_test_data.tar.gz
        VERBATIM)

add_executable(downlink_processor_test downlink_processor_test.cpp)
target_link_libraries(downlink_processor_test srsgnb_downlink_processor srslog srsgnb_support)
add_test(downlink_processor_test downlink_processor_test)

add_executable(downlink_processor_pool_test downlink_processor_pool_test.cpp)
target_link_libraries(downlink_processor_pool_test srsgnb_downlink_processor srslog)
add_test(downlink_processor_pool_test downlink_processor_pool_test)

add_executable(re_pattern_test re_pattern_test.cpp)
target_link_libraries(re_pattern_test srsgnb_upper_phy_support srslog)
add_test(re_pattern_test re_pattern_test)

add_executable(rx_softbuffer_pool_test rx_softbuffer_pool_test.cpp)
target_link_libraries(rx_softbuffer_pool_test srsgnb_upper_phy_support srslog)
add_test(rx_softbuffer_pool_test rx_softbuffer_pool_test)

add_executable(log_likelihood_ratio_test log_likelihood_ratio_test.cpp)
target_link_libraries(log_likelihood_ratio_test log_likelihood_ratio srslog)
add_test(log_likelihood_ratio_test log_likelihood_ratio_test)

add_executable(uplink_processor_test uplink_processor_test.cpp)
target_link_libraries(uplink_processor_test srsgnb_uplink_processor srslog gtest gtest_main)
gtest_discover_tests(uplink_processor_test)

add_executable(vrb_to_prb_mapper_test vrb_to_prb_mapper_test.cpp)
target_link_libraries(vrb_to_prb_mapper_test srsgnb_upper_phy_support srslog)
add_test(vrb_to_prb_mapper_test vrb_to_prb_mapper_test)
