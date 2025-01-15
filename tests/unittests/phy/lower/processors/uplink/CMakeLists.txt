#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(prach)
add_subdirectory(puxch)

add_executable(lower_phy_uplink_processor_test lower_phy_uplink_processor_test.cpp)
target_link_libraries(lower_phy_uplink_processor_test srsran_lower_phy_uplink_processor srslog gtest gtest_main)
add_test(lower_phy_uplink_processor_test lower_phy_uplink_processor_test)