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

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(messages)

add_executable(fapi_to_mac_data_msg_translator_test fapi_to_mac_data_msg_translator_test.cpp)
target_link_libraries(fapi_to_mac_data_msg_translator_test
        srsran_support
        srsran_fapi_to_mac_translator
        srslog
        srsran_mac_translator_helpers
        gtest
        gtest_main)
gtest_discover_tests(fapi_to_mac_data_msg_translator_test)

add_executable(mac_to_fapi_translator_test mac_to_fapi_translator_test.cpp)
target_link_libraries(mac_to_fapi_translator_test
        srsran_support
        srsran_mac_to_fapi_translator
        srsran_fapi_precoding_matrix_tools
        srslog
        srsran_mac_translator_helpers
        gtest
        gtest_main)
gtest_discover_tests(mac_to_fapi_translator_test)
