#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(du_high)
add_subdirectory(du_high_cu)
add_subdirectory(e2ap)
add_subdirectory(ngap)
if (DU_SPLIT_7_2 OR DU_SPLIT_DYNAMIC)
    add_subdirectory(ofh)
endif ()
if (NOT DU_SPLIT_6)
    add_subdirectory(phy)
endif ()
add_subdirectory(rlc)
