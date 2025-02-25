#
# Copyright 2021-2025 Software Radio Systems Limited
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


add_subdirectory(helpers)

# Build the flexible du split 7.2 library when it is selected in the definition.
set(SOURCES
        split_7_2_o_du_application_unit_impl.cpp
        split_7_2_o_du_factory.cpp
        split_7_2_o_du_unit_cli11_schema.cpp
        split_7_2_o_du_unit_config_validator.cpp
        split_7_2_o_du_unit_config_yaml_writer.cpp)

add_library(srsran_flexible_o_du_split_7_2 STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_split_7_2 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_o_du_split_7_2
        srsran_split_7_2_app_unit_helpers
        srsran_flexible_o_du_helpers)
