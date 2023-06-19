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

add_library(e1ap_test_helpers e1ap_cu_cp_test_messages.cpp e1ap_cu_up_test_messages.cpp test_helpers.cpp)
target_include_directories(e1ap_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_test_helpers srsran_support e1ap_asn1 srslog)

add_executable(e1ap_asn1_packer_test e1ap_asn1_packer_test.cpp)
target_link_libraries(e1ap_asn1_packer_test
                      e1ap_test_helpers
                      srsran_e1ap
                      srsran_pcap
                      srsran_support
                      srslog
                      e1ap_asn1
                      srsran_ran
                      gtest
                      gtest_main)
target_include_directories(e1ap_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1ap_asn1_packer_test)
