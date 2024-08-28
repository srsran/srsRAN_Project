#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(sched_cell OBJECT
        vrb_alloc.cpp
        resource_block_group.cpp
        resource_grid.cpp
        scheduler_prb.cpp
        cell_harq_manager.cpp)
