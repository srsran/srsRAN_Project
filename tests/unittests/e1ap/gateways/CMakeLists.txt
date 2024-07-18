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

add_executable(e1_gateway_test e1_gateway_test.cpp)
target_link_libraries(e1_gateway_test
        srsran_e1_gateway
        srsran_gateway
        e1ap_test_helpers
        srsran_e1ap_common
        srsran_support
        srsran_network
        srslog
        e1ap_asn1
        gtest
        gtest_main)
add_test(e1_gateway_test e1_gateway_test)
