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

set_directory_properties(PROPERTIES LABELS "cu;du;integrationtest")

add_executable(cu_du_test cu_du_test.cpp du_high_cu_test_simulator.cpp cu_multi_du_test.cpp)
target_link_libraries(cu_du_test srsran_cu_cp
        srsran_du_high
        srsran_f1ap_cu
        mac_test_doubles
        f1ap_test_doubles
        srsran_rrc
        srsran_support
        srslog
        ngap_test_helpers
        f1ap_asn1
        ngap_asn1
        gtest
        gtest_main)
target_include_directories(cu_du_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_du_test)
