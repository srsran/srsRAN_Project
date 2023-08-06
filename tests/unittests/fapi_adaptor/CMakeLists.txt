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

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(mac)
add_subdirectory(phy)

add_executable(fapi_adaptor_performance_dl_tti_request adaptor_performance_dl_tti_request.cpp)
target_link_libraries(fapi_adaptor_performance_dl_tti_request srsran_fapi_phy_message_adaptors srsran_mac_fapi_adaptors srslog)
add_test(fapi_adaptor_performance_dl_tti_request fapi_adaptor_performance_dl_tti_request)

add_executable(precoding_matrix_table_generator_test precoding_matrix_table_generator_test.cpp)
target_link_libraries(precoding_matrix_table_generator_test srsran_fapi_phy_message_adaptors srslog gtest gtest_main)
gtest_discover_tests(precoding_matrix_table_generator_test)
