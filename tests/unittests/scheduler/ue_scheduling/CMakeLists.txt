#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ue_scheduler_test
        logical_channel_test.cpp
        fallback_scheduler_test.cpp
        ue_cell_test.cpp
        ue_configuration_test.cpp
        ue_grid_allocator_test.cpp
        ul_logical_channel_test.cpp
        ue_pxsch_alloc_param_candidate_searcher_test.cpp
        ue_link_adaptation_controller_test.cpp
        ta_manager_test.cpp
        ue_harq_link_adaptation_test.cpp)
target_link_libraries(ue_scheduler_test
        srsran_sched
        srslog
        mac_configuration_helpers
        scheduler_test_suite
        sched_config
        gtest
        gtest_main)
add_test(ue_scheduler_test ue_scheduler_test)
