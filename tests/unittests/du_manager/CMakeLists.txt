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

set_directory_properties(PROPERTIES LABELS "du_manager")

include_directories(../../..)

add_library(du_manager_test_helpers du_manager_test_helpers.cpp procedures/du_manager_procedure_test_helpers.cpp)
target_include_directories(du_manager_test_helpers PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(du_manager_test_helpers srsran_du_manager srsran_support srslog)

add_executable(du_bearer_test du_ue/du_bearer_test.cpp)
target_link_libraries(du_bearer_test du_manager_test_helpers srsran_du_manager srsran_support srslog gtest gtest_main)
gtest_discover_tests(du_bearer_test)

add_executable(ue_manager_test du_ue/ue_manager_test.cpp)
target_link_libraries(ue_manager_test
        du_manager_test_helpers
        srsran_du_manager
        srsran_support
        srslog
        gtest
        gtest_main)
target_include_directories(ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ue_manager_test)


add_executable(sib_test sib_test.cpp)
target_link_libraries(sib_test
        du_manager_test_helpers
        srsran_du_manager
        srsran_support
        srslog
        gtest
        gtest_main)
target_include_directories(sib_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(sib_test)

add_executable(serving_cell_config_converter_test ../du_manager/serving_cell_config_converter_test.cpp)
target_link_libraries(serving_cell_config_converter_test srsran_du_manager srsran_support asn1_utils rrc_nr_asn1 srslog gtest gtest_main)
add_test(serving_cell_config_converter_test serving_cell_config_converter_test)
gtest_discover_tests(serving_cell_config_converter_test)

add_executable(mac_cell_group_config_converter_test ../du_manager/mac_cell_group_config_converter_test.cpp)
target_link_libraries(mac_cell_group_config_converter_test srsran_du_manager srsran_support asn1_utils rrc_nr_asn1 srslog gtest gtest_main)
add_test(mac_cell_group_config_converter_test mac_cell_group_config_converter_test)
gtest_discover_tests(mac_cell_group_config_converter_test)

add_executable(du_manager_ue_creation_test procedures/ue_creation_test.cpp)
target_include_directories(du_manager_ue_creation_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_manager_ue_creation_test du_manager_test_helpers gtest gtest_main)
gtest_discover_tests(du_manager_ue_creation_test)

add_executable(du_ue_config_test
        procedures/ue_configuration_test.cpp
        procedures/ue_deletion_test.cpp
        procedures/du_ue_ric_configuration_procedure_test.cpp)
target_include_directories(du_ue_config_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_ue_config_test du_manager_test_helpers gtest gtest_main)
gtest_discover_tests(du_ue_config_test)

add_executable(du_ran_resource_manager_test du_ran_resource_manager_test.cpp)
target_include_directories(du_ran_resource_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_ran_resource_manager_test du_manager_test_helpers gtest gtest_main)
gtest_discover_tests(du_ran_resource_manager_test)

add_executable(pucch_resource_generator_test pucch_resource_generator_test.cpp)
target_include_directories(pucch_resource_generator_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(pucch_resource_generator_test du_manager_test_helpers gtest gtest_main)
gtest_discover_tests(pucch_resource_generator_test)
