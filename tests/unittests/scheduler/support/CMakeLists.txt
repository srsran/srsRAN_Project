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

add_executable(pdcch_type0_css_occassions_test pdcch/pdcch_type0_css_occassions_test.cpp)
target_link_libraries(pdcch_type0_css_occassions_test srsran_sched srslog)
add_test(pdcch_type0_css_occassions_test pdcch_type0_css_occassions_test)

add_executable(pdsch_default_time_allocation_test pdsch/pdsch_default_time_allocation_test.cpp)
target_link_libraries(pdsch_default_time_allocation_test srsran_sched srslog gtest gtest_main)
add_test(pdsch_default_time_allocation_test pdsch_default_time_allocation_test)

add_executable(pdsch_dmrs_symbol_mask_test pdsch/pdsch_dmrs_symbol_mask_test.cpp)
target_link_libraries(pdsch_dmrs_symbol_mask_test srsran_sched srslog gtest gtest_main)
add_test(pdsch_dmrs_symbol_mask_test pdsch_dmrs_symbol_mask_test)

add_executable(pucch_default_resource_test pucch/pucch_default_resource_test.cpp)
target_link_libraries(pucch_default_resource_test srsran_sched srslog gtest gtest_main)
add_test(pucch_default_resource_test pucch_default_resource_test)

add_executable(pusch_default_time_allocation_test pusch/pusch_default_time_allocation_test.cpp)
target_link_libraries(pusch_default_time_allocation_test srsran_sched srslog gtest gtest_main)
add_test(pusch_default_time_allocation_test pusch_default_time_allocation_test)

add_executable(pusch_dmrs_symbol_mask_test pusch/pusch_dmrs_symbol_mask_test.cpp)
target_link_libraries(pusch_dmrs_symbol_mask_test srsran_sched srslog gtest gtest_main)
add_test(pusch_dmrs_symbol_mask_test pusch_dmrs_symbol_mask_test)

add_executable(tbs_calculator_test tbs_calculator_test.cpp)
target_link_libraries(tbs_calculator_test srsran_sched srslog)
add_test_vector(tbs_calculator_test none "")

add_executable(prbs_calculator_test prbs_calculator_test.cpp)
target_link_libraries(prbs_calculator_test srsran_sched srslog gtest gtest_main)
add_test(prbs_calculator_test prbs_calculator_test)

add_executable(dmrs_helpers_test dmrs_helpers_test.cpp)
target_link_libraries(dmrs_helpers_test srsran_sched srslog)
add_test(dmrs_helpers_test dmrs_helpers_test)

add_executable(mcs_calculator_test mcs_calculator_test.cpp)
target_link_libraries(mcs_calculator_test srsran_sched gtest gtest_main)
add_test(mcs_calculator_test.cpp mcs_calculator_test)

add_executable(mcs_tbs_calculator_test mcs_tbs_calculator_test.cpp)
target_link_libraries(mcs_tbs_calculator_test
        srsran_sched
        scheduler_test_suite
        mac_configuration_helpers
        srsran_du_manager
        gtest
        gtest_main)
add_test(mcs_tbs_calculator_test mcs_tbs_calculator_test)

add_executable(pdcch_aggregation_level_calculator_test pdcch_aggregation_level_calculator_test.cpp)
target_link_libraries(pdcch_aggregation_level_calculator_test srsran_sched gtest gtest_main)
add_test(pdcch_aggregation_level_calculator_test.cpp pdcch_aggregation_level_calculator_test)
