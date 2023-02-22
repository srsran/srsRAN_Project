#
# Copyright 2021-2023 Software Radio Systems Limited
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

set(SCHED_TEST_LIBS
        scheduler_test_suite
        uci_test_utils
        sched_config
        mac_configuration_helpers
        gtest)

add_executable(pucch_allocator_test pucch_allocator_test.cpp)
target_link_libraries(pucch_allocator_test srsran_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(pucch_allocator_test)

add_executable(pucch_alloc_multi_ue_test pucch_alloc_multi_ue_test.cpp)
target_link_libraries(pucch_alloc_multi_ue_test srsran_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(pucch_alloc_multi_ue_test)

add_executable(pucch_res_manager_test pucch_res_manager_test.cpp)
target_link_libraries(pucch_res_manager_test srsran_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(pucch_res_manager_test)

add_executable(uci_allocator_test uci_allocator_test.cpp)
target_link_libraries(uci_allocator_test srsran_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(uci_allocator_test)

add_executable(test_pucch_guardbands_sched pucch_guardbands_sched_test.cpp)
target_link_libraries(test_pucch_guardbands_sched srsran_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(test_pucch_guardbands_sched)

add_executable(uci_scheduling_test uci_scheduling_test.cpp)
target_link_libraries(uci_scheduling_test srsran_sched ${SCHED_TEST_LIBS})
gtest_discover_tests(uci_scheduling_test)

add_executable(scheduler_uci_indication_test scheduler_uci_indication_test.cpp)
target_link_libraries(scheduler_uci_indication_test
        srsran_sched
        scheduler_test_suite
        sched_config
        gtest)
gtest_discover_tests(scheduler_uci_indication_test)
