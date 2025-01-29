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

add_subdirectory(du_high)
add_subdirectory(du_low)

set(SOURCES du_cell_config_validation.cpp du_update_config_helpers.cpp)
add_library(srsran_du_config_validators STATIC ${SOURCES})
target_link_libraries(srsran_du_config_validators sched_config)

add_library(srsran_o_du STATIC
        o_du_impl.cpp
        o_du_factory.cpp)

target_link_libraries(srsran_o_du PUBLIC srsran_o_du_high srsran_o_du_low)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(o_du_factory.cpp o_du_impl.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES srsran_hal_pusch
           srsran_hal_pdsch
           srsran_hal_bbdev)
endif (DPDK_FOUND)
