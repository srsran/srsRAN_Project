#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(f1u_cu_up_bearer_test f1u_cu_up_bearer_test.cpp)
target_link_libraries(f1u_cu_up_bearer_test srsgnb_f1u_cu_up srsgnb_support srslog gtest gtest_main)
add_test(f1u_cu_up_bearer_test f1u_cu_up_bearer_test)
gtest_discover_tests(f1u_cu_up_bearer_test)
