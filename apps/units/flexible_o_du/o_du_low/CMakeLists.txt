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

set(SOURCES
            du_low_config_cli11_schema.cpp
            du_low_config_translator.cpp
            du_low_config_validator.cpp
            du_low_config_yaml_writer.cpp
            du_low_hal_factory.cpp
            o_du_low_unit_factory.cpp)

add_library(srsran_o_du_low_unit_helpers STATIC ${SOURCES})
target_include_directories(srsran_o_du_low_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
set(DU_LOW_UNIT_HELPERS_LIBRARIES srsran_upper_phy srsran_cpu_affinities_helper srsran_o_du_low srsran_app_helpers)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(
           du_low_config_cli11_schema.cpp
           du_low_hal_factory.cpp
           PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_UNIT_HELPERS_LIBRARIES srsran_hal_pusch
        srsran_hal_pdsch
        srsran_hal_bbdev)
endif (DPDK_FOUND)

target_link_libraries(srsran_o_du_low_unit_helpers PUBLIC ${DU_LOW_UNIT_HELPERS_LIBRARIES})
