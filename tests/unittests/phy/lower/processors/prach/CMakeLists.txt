#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(prach_processor_test prach_processor_test.cpp)
target_link_libraries(prach_processor_test srsgnb_lower_phy_prach_processor srslog gtest gtest_main)
add_test(prach_processor_test prach_processor_test)