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

set_directory_properties(PROPERTIES LABELS "cu_up")

add_executable(cu_up_test cu_up_test.cpp)
target_link_libraries(cu_up_test srsran_cu_up srsran_support srslog e1ap_asn1 gtest gtest_main)
target_include_directories(cu_up_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_up_test)

add_executable(cu_up_ue_manager_test ue_manager_test.cpp)
target_link_libraries(cu_up_ue_manager_test srsran_cu_up srsran_support srslog gtest gtest_main)
target_include_directories(cu_up_ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_up_ue_manager_test)

add_executable(pdu_session_manager_test pdu_session_manager_test.cpp)
target_link_libraries(pdu_session_manager_test srsran_cu_up srsran_support e1ap_asn1 srslog gtest gtest_main)
target_include_directories(pdu_session_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(pdu_session_manager_test)
