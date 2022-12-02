#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(harq_process_test harq_process_test.cpp)
target_link_libraries(harq_process_test srsgnb_sched srslog gtest gtest_main)
gtest_discover_tests(harq_process_test)

add_executable(harq_entity_test harq_entity_test.cpp)
target_link_libraries(harq_entity_test srsgnb_sched srslog gtest gtest_main)
gtest_discover_tests(harq_entity_test)

add_executable(dl_logical_channel_test dl_logical_channel_test.cpp)
target_link_libraries(dl_logical_channel_test srsgnb_sched srslog gtest gtest_main)
add_test(dl_logical_channel_test dl_logical_channel_test)

add_executable(ul_logical_channel_test ul_logical_channel_test.cpp)
target_link_libraries(ul_logical_channel_test srsgnb_sched srslog gtest gtest_main)
add_test(ul_logical_channel_test ul_logical_channel_test)

add_executable(ue_configuration_test ue_configuration_test.cpp)
target_link_libraries(ue_configuration_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        sched_config)
add_test(ue_configuration_test ue_configuration_test)

add_executable(srb0_scheduler_test srb0_scheduler_test.cpp)
target_link_libraries(srb0_scheduler_test
        srsgnb_sched
        srslog
        mac_configuration_helpers
        scheduler_test_suite
        sched_config
        gtest
        gtest_main)
add_test(srb0_scheduler_test srb0_scheduler_test)
