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

add_library(cu_cp_routine_manager_test_helpers cu_cp_routine_manager_test_helpers.cpp)
target_include_directories(cu_cp_routine_manager_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_routine_manager_test_helpers
        cu_cp_test_helpers
        srsran_cu_cp
        e1ap_test_helpers
        f1ap_test_helpers
        ngap_test_helpers
        rrc_ue_test_helpers
        f1ap_test_doubles
        srsran_support
        f1ap_asn1
        srslog)

set(SOURCES
        pdu_session_resource_setup_routine_test.cpp
        pdu_session_resource_release_routine_test.cpp
        pdu_session_resource_modification_routine_test.cpp
        ue_context_release_routine_test.cpp
        )

add_executable(cu_cp_routine_test ${SOURCES})
target_include_directories(cu_cp_routine_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_routine_test cu_cp_routine_manager_test_helpers srsran_support srslog gtest gtest_main)
gtest_discover_tests(cu_cp_routine_test PROPERTIES "LABELS;cu_cp_routine_test")
