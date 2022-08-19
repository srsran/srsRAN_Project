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

add_executable(channel_equalizer_test channel_equalizer_test.cpp)
target_link_libraries(channel_equalizer_test srslog)
add_test(channel_equalizer_test channel_equalizer_test)

add_custom_command(
        TARGET channel_equalizer_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/channel_equalizer_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/channel_equalizer_test_data.tar.gz
        VERBATIM)
