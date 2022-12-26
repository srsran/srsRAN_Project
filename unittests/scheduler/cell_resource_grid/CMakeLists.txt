#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(sched_prb_test sched_prb_test.cpp)
target_link_libraries(sched_prb_test srsgnb_sched srslog)
add_test(sched_prb_test sched_prb_test)

add_executable(resource_allocator_test cell_resource_grid_test.cpp)
target_link_libraries(resource_allocator_test srsgnb_sched srslog mac_configuration_helpers sched_config)
add_test(resource_allocator_test resource_allocator_test)

