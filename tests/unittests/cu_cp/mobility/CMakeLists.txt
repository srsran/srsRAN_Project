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

add_library(mobility_test_helpers mobility_test_helpers.cpp ../du_processor_test_messages.cpp ../test_helpers.cpp inter_du_handover_routine_test_helpers.cpp)
target_include_directories(mobility_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mobility_test_helpers srsran_cu_cp srsran_e1ap e1ap_test_helpers f1ap_test_helpers rrc_ue_test_helpers ngap_test_helpers srsran_support f1ap_asn1 srslog)

set(SOURCES inter_du_handover_routine_test.cpp
            handover_reconfiguration_routine_test.cpp)

add_executable(cu_cp_mobility_test ${SOURCES})
target_include_directories(cu_cp_mobility_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_mobility_test mobility_test_helpers srsran_support srslog gtest gtest_main)
gtest_discover_tests(cu_cp_mobility_test PROPERTIES "LABELS;mobility")
