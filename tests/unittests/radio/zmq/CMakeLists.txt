#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(radio_zmq_e2e_test radio_zmq_e2e_test.cpp)
target_link_libraries(radio_zmq_e2e_test srsran_radio srsran_support)
add_test(radio_zmq_e2e_test radio_zmq_e2e_test)

add_executable(radio_zmq_validator_test radio_zmq_validator_test.cpp)
target_link_libraries(radio_zmq_validator_test srsran_radio srsran_support gtest gtest_main)
add_test(radio_zmq_e2e_test radio_zmq_e2e_test)
