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

add_subdirectory(messages)

add_library(srsran_mac_to_fapi_translator STATIC mac_to_fapi_translator.cpp)
target_link_libraries(srsran_mac_to_fapi_translator srsran_mac_fapi_adaptors srsran_fapi_precoding_matrix_tools srsran_fapi)


set(SOURCES
        fapi_to_mac_data_msg_translator.cpp
        fapi_to_mac_error_msg_translator.cpp
        fapi_to_mac_time_msg_translator.cpp)

add_library(srsran_fapi_to_mac_translator STATIC ${SOURCES})
target_link_libraries(srsran_fapi_to_mac_translator srsvec srsran_support)

add_library(mac_fapi_adaptor STATIC mac_fapi_adaptor_impl.cpp)
target_link_libraries(mac_fapi_adaptor srsran_mac_to_fapi_translator srsran_fapi_to_mac_translator)

add_library(mac_fapi_adaptor_factory STATIC mac_fapi_adaptor_factory_impl.cpp)
target_link_libraries(mac_fapi_adaptor_factory mac_fapi_adaptor)
