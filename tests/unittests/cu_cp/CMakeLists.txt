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

set_directory_properties(PROPERTIES LABELS "cu_cp")

include_directories(../../..)

add_subdirectory(cell_meas_manager)
add_subdirectory(du_processor)
add_subdirectory(cu_up_processor)
add_subdirectory(ue_manager)
add_subdirectory(up_resource_manager)
add_subdirectory(mobility)
add_subdirectory(metrics_handler)

add_library(cu_cp_test_helpers
        test_doubles/mock_cu_up.cpp
        test_doubles/mock_amf.cpp
        test_doubles/mock_du.cpp
        cu_cp_test_environment.cpp
        cu_cp_test_helpers.cpp
        du_processor_test_messages.cpp
        test_helpers.cpp)
target_include_directories(cu_cp_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_test_helpers srsran_cu_cp srsran_support srslog f1ap_test_helpers e1ap_test_helpers f1ap_asn1 ngap_asn1 e1ap_asn1)


add_executable(cu_cp_test cu_cp_test.cpp cu_cp_connectivity_test.cpp)
target_link_libraries(cu_cp_test
        cu_cp_test_helpers
        f1ap_cu_test_helpers
        ngap_test_helpers
        rrc_ue_test_helpers
        srsran_cu_cp
        srsran_support
        srslog
        f1ap_asn1
        ngap_asn1
        gtest
        gtest_main)
target_include_directories(cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_test)

add_executable(cu_cp_config_test cu_cp_config_test.cpp)
target_link_libraries(cu_cp_config_test cu_cp_test_helpers srsran_cu_cp gtest gtest_main)
target_include_directories(cu_cp_config_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_cp_config_test)
