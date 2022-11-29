#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_executable(ue_manager_test ue_manager_test.cpp)
target_link_libraries(ue_manager_test srsgnb_du_manager srsgnb_support srslog f1ap_asn1)
target_include_directories(ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(ue_manager_test ue_manager_test)

add_executable(serving_cell_config_converter_test ../du_manager/serving_cell_config_converter_test.cpp)
target_link_libraries(serving_cell_config_converter_test srsgnb_du_manager srsgnb_support asn1_utils rrc_nr_asn1 srslog gtest gtest_main)
add_test(serving_cell_config_converter_test serving_cell_config_converter_test)
gtest_discover_tests(serving_cell_config_converter_test)

add_executable(mac_cell_group_config_converter_test ../du_manager/mac_cell_group_config_converter_test.cpp)
target_link_libraries(mac_cell_group_config_converter_test srsgnb_du_manager srsgnb_support asn1_utils rrc_nr_asn1 srslog gtest gtest_main)
add_test(mac_cell_group_config_converter_test mac_cell_group_config_converter_test)
gtest_discover_tests(mac_cell_group_config_converter_test)
