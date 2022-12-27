#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ue_scheduler_test
        dl_logical_channel_test.cpp
        harq_entity_test.cpp
        harq_process_test.cpp
        srb0_scheduler_test.cpp
        ue_configuration_test.cpp
        ul_logical_channel_test.cpp)
target_link_libraries(ue_scheduler_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        scheduler_test_suite
        sched_config
        gtest
        gtest_main)
gtest_discover_tests(ue_scheduler_test)
