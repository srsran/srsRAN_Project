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

set(SOURCES
        phy_to_fapi_results_event_translator.cpp
        phy_to_fapi_time_event_translator.cpp)

add_library(fapi_to_phy_translator STATIC fapi_to_phy_translator.cpp)
target_link_libraries(fapi_to_phy_translator
        srsran_fapi_phy_message_adaptors
        srsran_fapi_precoding_matrix_tools
        srsran_instrumentation)

add_library(phy_to_fapi_translator STATIC ${SOURCES})
target_link_libraries(phy_to_fapi_translator srslog)

add_library(phy_fapi_adaptor STATIC phy_fapi_adaptor_impl.cpp)
target_link_libraries(phy_fapi_adaptor fapi_to_phy_translator phy_to_fapi_translator)

add_library(phy_fapi_adaptor_factory STATIC phy_fapi_adaptor_factory_impl.cpp)
target_link_libraries(phy_fapi_adaptor_factory phy_fapi_adaptor)
