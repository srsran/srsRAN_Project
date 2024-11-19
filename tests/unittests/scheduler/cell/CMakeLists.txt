#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(scheduler_cell_resource_test
        sched_prb_test.cpp
        cell_resource_grid_test.cpp
        cell_harq_manager_test.cpp)
target_link_libraries(scheduler_cell_resource_test
        srsran_sched
        srslog
        mac_configuration_helpers
        sched_config
        scheduler_test_utils
        sched_test_doubles
        gtest
        gtest_main)
gtest_discover_tests(scheduler_cell_resource_test)

