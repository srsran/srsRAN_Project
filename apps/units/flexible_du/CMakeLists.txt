#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(du_high)
add_subdirectory(fapi)
add_subdirectory(support)
if (NOT DU_SPLIT_6)
    add_subdirectory(split_helpers)
endif ()
if (DU_SPLIT_6)
    add_subdirectory(split_6)
endif ()
if (DU_SPLIT_7_2)
    add_subdirectory(du_low)
    add_subdirectory(split_7_2)
endif ()
if (DU_SPLIT_8)
    add_subdirectory(du_low)
    add_subdirectory(split_8)
endif ()
if (DU_SPLIT_DYNAMIC)
    add_subdirectory(du_low)
    add_subdirectory(split_7_2)
    add_subdirectory(split_8)
    add_subdirectory(split_dynamic)
endif ()

