#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "io_broker;tsan")

add_executable(network_test transport_layer_address_test.cpp io_broker_epoll_test.cpp io_timer_source_test.cpp)
target_link_libraries(network_test srsran_support srslog gtest gtest_main)
add_test(network_test network_test)
