#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set_directory_properties(PROPERTIES LABELS "sched")

include_directories(../../..)

add_subdirectory(test_utils)
add_subdirectory(support)
add_subdirectory(cell)
add_subdirectory(common_scheduling)
add_subdirectory(ue_context)
add_subdirectory(ue_scheduling)
add_subdirectory(uci_and_pucch)
add_subdirectory(policy)
add_subdirectory(config)
add_subdirectory(slicing)
add_subdirectory(srs_scheduling)

add_executable(sched_no_ue_test scheduler_no_ue_test.cpp)
target_link_libraries(sched_no_ue_test
        srsran_sched
        scheduler_test_suite
        srslog
        mac_configuration_helpers
        sched_config
        sched_test_doubles
        gtest
        gtest_main)
add_test(sched_no_ue_test sched_no_ue_test)

add_executable(pdcch_resource_allocator_test pdcch/pdcch_resource_allocator_test.cpp)
target_link_libraries(pdcch_resource_allocator_test
	srsran_sched
	srslog
	mac_configuration_helpers
	sched_config
	scheduler_test_utils
	gtest
	gtest_main)
add_test(pdcch_resource_allocator_test pdcch_resource_allocator_test)

add_executable(scheduler_test
        scheduler_retx_test.cpp
        scheduler_ue_removal_test.cpp
        scheduler_tdd_test.cpp
        scheduler_ue_fallback_mode_test.cpp
        scheduler_ta_cmd_test.cpp
        multi_cell_scheduler_test.cpp
        multi_slice_scheduler_test.cpp
        scheduler_qos_test.cpp
        scheduler_multi_grants_test.cpp
        scheduler_multi_ue_test.cpp
)
target_link_libraries(scheduler_test
        srsran_sched
        sched_config
        scheduler_test_suite
        sched_test_doubles
        gtest
        gtest_main)
add_test(scheduler_test scheduler_test)

add_executable(scheduler_test_doubles_test test_doubles/test_pucch_res_test_builder_helper.cpp)
target_link_libraries(scheduler_test_doubles_test
        srsran_sched
        sched_config
        scheduler_test_suite
        sched_test_doubles
        gtest
        gtest_main)
gtest_discover_tests(scheduler_test_doubles_test)

add_executable(multiple_ue_sched_test multiple_ue_sched_test.cpp)
target_link_libraries(multiple_ue_sched_test
        srsran_sched
        sched_config
        scheduler_test_suite
        mac_configuration_helpers
        sched_test_doubles
        gtest
        gtest_main)
gtest_discover_tests(multiple_ue_sched_test)

add_executable(scheduler_metrics_handler_test scheduler_metrics_handler_test.cpp)
target_link_libraries(scheduler_metrics_handler_test
        srsran_sched
        sched_config
        sched_test_doubles
        gtest
        gtest_main)
add_test(scheduler_metrics_handler_test scheduler_metrics_handler_test)
