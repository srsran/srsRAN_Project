#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if (DU_SPLIT_8 OR DU_SPLIT_DYNAMIC)
    add_subdirectory(phy)
    add_subdirectory(radio)
endif ()

