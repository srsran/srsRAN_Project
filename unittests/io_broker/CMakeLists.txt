#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "io_broker")

add_executable(io_broker_epoll_test io_broker_epoll_test.cpp)
target_link_libraries(io_broker_epoll_test srsgnb_io_broker srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(io_broker_epoll_test)