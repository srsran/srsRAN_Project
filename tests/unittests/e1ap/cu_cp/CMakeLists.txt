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

add_library(e1ap_cu_cp_test_helpers e1ap_cu_cp_test_helpers.cpp)
target_include_directories(e1ap_cu_cp_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_cp_test_helpers e1ap_test_helpers srsran_e1ap srsran_pcap srsran_support e1ap_asn1 srslog)

set(SOURCES
        e1ap_cu_cp_ue_context_test.cpp
        e1ap_cu_cp_setup_procedure_test.cpp
        e1ap_cu_cp_bearer_context_setup_procedure_test.cpp
        e1ap_cu_cp_bearer_context_modification_procedure_test.cpp
        e1ap_cu_cp_bearer_context_release_procedure_test.cpp
        e1ap_cu_cp_test.cpp)

add_executable(e1ap_cu_cp_test ${SOURCES})
target_include_directories(e1ap_cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_cp_test e1ap_cu_cp_test_helpers srsran_e1ap srsran_pcap srsran_support srslog e1ap_asn1 gtest gtest_main)
gtest_discover_tests(e1ap_cu_cp_test PROPERTIES "LABELS;e1ap_cu_cp")
