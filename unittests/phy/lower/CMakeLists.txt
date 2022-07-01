#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(modulation)

add_executable(lower_phy_test lower_phy_test.cpp)
target_link_libraries(lower_phy_test srsgnb_lower_phy srslog)
add_test(lower_phy_test lower_phy_test)