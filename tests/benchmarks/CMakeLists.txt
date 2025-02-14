#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

remove_definitions(-DASSERTS_ENABLED)
remove_definitions(-DPARANOID_ASSERTS_ENABLED)

add_subdirectory(adt)
add_subdirectory(gateways)
add_subdirectory(phy)
add_subdirectory(scheduler)
add_subdirectory(du_high)
add_subdirectory(ofh)
add_subdirectory(rlc)
add_subdirectory(pdcp)
add_subdirectory(security)
