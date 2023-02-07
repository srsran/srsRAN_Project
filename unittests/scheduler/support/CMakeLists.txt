#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(pdcch_type0_css_occassions_test pdcch/pdcch_type0_css_occassions_test.cpp)
target_link_libraries(pdcch_type0_css_occassions_test srsgnb_sched srslog)
add_test(pdcch_type0_css_occassions_test pdcch_type0_css_occassions_test)

add_executable(pdsch_default_time_allocation_test pdsch/pdsch_default_time_allocation_test.cpp)
target_link_libraries(pdsch_default_time_allocation_test srsgnb_sched srslog gtest gtest_main)
add_test(pdsch_default_time_allocation_test pdsch_default_time_allocation_test)

add_executable(pdsch_dmrs_symbol_mask_test pdsch/pdsch_dmrs_symbol_mask_test.cpp)
target_link_libraries(pdsch_dmrs_symbol_mask_test srsgnb_sched srslog gtest gtest_main)
add_test(pdsch_dmrs_symbol_mask_test pdsch_dmrs_symbol_mask_test)

add_executable(cqi_to_mcs_mapper_test pdsch/cqi_to_mcs_mapper_test.cpp)
target_link_libraries(cqi_to_mcs_mapper_test srsgnb_sched srslog gtest gtest_main)
add_test(cqi_to_mcs_mapper_test cqi_to_mcs_mapper_test)

add_executable(pucch_default_resource_test pucch/pucch_default_resource_test.cpp)
target_link_libraries(pucch_default_resource_test srsgnb_sched srslog gtest gtest_main)
add_test(pucch_default_resource_test pucch_default_resource_test)

add_executable(pusch_default_time_allocation_test pusch/pusch_default_time_allocation_test.cpp)
target_link_libraries(pusch_default_time_allocation_test srsgnb_sched srslog gtest gtest_main)
add_test(pusch_default_time_allocation_test pusch_default_time_allocation_test)

add_executable(pusch_dmrs_symbol_mask_test pusch/pusch_dmrs_symbol_mask_test.cpp)
target_link_libraries(pusch_dmrs_symbol_mask_test srsgnb_sched srslog gtest gtest_main)
add_test(pusch_dmrs_symbol_mask_test pusch_dmrs_symbol_mask_test)

add_executable(tbs_calculator_test tbs_calculator_test.cpp)
target_link_libraries(tbs_calculator_test srsgnb_sched srslog)
add_test_vector(tbs_calculator_test none "")

add_executable(prbs_calculator_test prbs_calculator_test.cpp)
target_link_libraries(prbs_calculator_test srsgnb_sched srslog gtest gtest_main)
add_test(prbs_calculator_test prbs_calculator_test)

add_executable(dmrs_helpers_test dmrs_helpers_test.cpp)
target_link_libraries(dmrs_helpers_test srsgnb_sched srslog)
add_test(dmrs_helpers_test dmrs_helpers_test)

add_executable(mcs_calculator_test mcs_calculator_test.cpp)
target_link_libraries(mcs_calculator_test srsgnb_sched gtest gtest_main)
add_test(mcs_calculator_test.cpp mcs_calculator_test)

add_executable(mcs_tbs_calculator_test mcs_tbs_calculator_test.cpp)
target_link_libraries(mcs_tbs_calculator_test
        srsgnb_sched
        scheduler_test_suite
        mac_configuration_helpers
        srsgnb_du_manager
        gtest
        gtest_main)
add_test(mcs_tbs_calculator_test mcs_tbs_calculator_test)
