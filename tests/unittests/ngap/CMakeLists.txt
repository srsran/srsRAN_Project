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

add_library(ngap_test_helpers ngap_test_helpers.cpp ngap_test_messages.cpp)
target_include_directories(ngap_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ngap_test_helpers srsran_support ngap_asn1 srslog)

set(SOURCES
        ngap_asn1_packer_test.cpp
        ng_setup_procedure_test.cpp
        ngap_nas_message_test.cpp
        ngap_ue_context_management_procedure_test.cpp
        ngap_pdu_session_resource_setup_procedure_test.cpp
        ngap_pdu_session_resource_release_procedure_test.cpp
        ngap_paging_test.cpp
        ngap_error_indication_test.cpp
        ngap_handover_test.cpp
        )

add_executable(ngap_test ${SOURCES})
target_include_directories(ngap_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ngap_test ngap_test_helpers srsran_ngap srsran_support ngap_asn1 srsran_ran srslog gtest gtest_main)
gtest_discover_tests(ngap_test PROPERTIES "LABELS;ngap")
