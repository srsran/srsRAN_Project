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

set(SOURCES
        dynamic_du_application_unit_impl.cpp
        dynamic_du_factory.cpp
        dynamic_du_impl.cpp
        dynamic_du_translators.cpp
        dynamic_du_unit_cli11_schema.cpp
        dynamic_du_unit_config_validator.cpp
        dynamic_du_unit_config_yaml_writer.cpp)

add_library(srsran_flexible_du_dynamic STATIC ${SOURCES})
target_include_directories(srsran_flexible_du_dynamic PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_du_dynamic
        srsran_du_wrapper
        srsran_ru_dummy
        srsran_pcap
        srsran_app_services
        srsran_fapi_app_unit
        srsran_du_low_unit_helpers
        srsran_split_8_app_unit_helpers
        srsran_split_7_2_app_unit_helpers
        srsran_du_high_unit_helpers)
