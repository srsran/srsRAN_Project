#
# Copyright 2021-2024 Software Radio Systems Limited
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

add_executable(ue_scheduler_test
        logical_channel_test.cpp
        harq_entity_test.cpp
        harq_process_test.cpp
        srb0_scheduler_test.cpp
        ue_configuration_test.cpp
        ue_grid_allocator_test.cpp
        ul_logical_channel_test.cpp
        ue_pdsch_param_candidate_searcher_test.cpp
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
gtest_discover_tests(ue_scheduler_test)
add_test(ue_scheduler_test ue_scheduler_test)
