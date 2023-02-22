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

add_subdirectory(adapters)

add_executable(gnb
        gnb.cpp
        gnb_appconfig_cli11_schema.cpp
        gnb_appconfig_validators.cpp
        gnb_appconfig_translators.cpp        
        fapi_factory.cpp
        phy_factory.cpp
        helpers/metrics_plotter_stdout.cpp
        helpers/gnb_console_helper.cpp
        )

install(TARGETS gnb
        CONFIGURATIONS Release
        RUNTIME)

target_link_libraries(gnb
        srsgnb_app_f1u_connector
        srsran_cu_up
        srsran_cu_cp
        srsran_io_broker
        srsran_ngap
        ngap_asn1
        srsran_du_high
        srsran_gateway
        mac_fapi_adaptor_factory
        phy_fapi_adaptor_factory
        srsran_fapi_message_loggers
        srsran_phy_support
        srsran_upper_phy
        srsran_signal_processors
        srsran_channel_processors
        srsran_channel_equalizer
        srsran_lower_phy
        srsran_radio
        srsran_pcap
        srsran_support
        srsran_build_info
        )
add_backward(gnb)

target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external)
