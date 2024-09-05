#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

remove_definitions(-DASSERTS_ENABLED)
remove_definitions(-DPARANOID_ASSERTS_ENABLED)

add_subdirectory(adt)
add_subdirectory(gateways)
if (NOT DU_SPLIT_6)
    add_subdirectory(phy)
endif ()
add_subdirectory(scheduler)
add_subdirectory(du_high)
if (DU_SPLIT_7_2 OR DU_SPLIT_DYNAMIC)
    add_subdirectory(ofh)
endif ()
add_subdirectory(rlc)
add_subdirectory(pdcp)
