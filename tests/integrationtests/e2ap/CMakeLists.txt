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

add_executable(e2ap_integration_test e2ap_integration_test.cpp ../../../apps/gnb/adapters/e2_gateway_remote_connector.cpp)
target_link_libraries(e2ap_integration_test srslog srsran_network srsran_e2 e2ap_asn1 srsran_gateway srsran_support gtest gtest_main)
target_include_directories(e2ap_integration_test PRIVATE ${CMAKE_SOURCE_DIR})
#gtest_discover_tests(e2ap_integration_test)
