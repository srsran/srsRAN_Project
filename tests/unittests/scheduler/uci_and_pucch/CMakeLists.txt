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


add_executable(uci_pucch_sched_test
        pucch_alloc_common_harq_test.cpp
        pucch_alloc_ded_resources_test.cpp
        pucch_alloc_format_0_test.cpp
        pucch_alloc_format_3_test.cpp
        pucch_alloc_format_4_test.cpp
        pucch_alloc_harq_sr_csi_test.cpp
        pucch_res_manager_test.cpp
        uci_allocator_test.cpp
        uci_scheduling_test.cpp
        uci_test_utils.cpp
        scheduler_uci_indication_test.cpp)
target_link_libraries(uci_pucch_sched_test
        srsran_sched
        scheduler_test_suite
        srslog
        uci_test_utils
        sched_config
        mac_configuration_helpers
        sched_test_doubles
        gtest
        gtest_main)
gtest_discover_tests(uci_pucch_sched_test)
