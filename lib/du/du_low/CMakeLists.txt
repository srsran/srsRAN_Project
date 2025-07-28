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
        du_low_executor_mapper.cpp
        du_low_factory.cpp
        du_low_impl.cpp
        du_low_metrics_collector_impl.cpp
        o_du_low_factory.cpp
        o_du_low_impl.cpp
        o_du_low_metrics_collector_impl.cpp)

add_library(srsran_o_du_low STATIC ${SOURCES})
set(DU_LOW_LIBRARIES srsran_phy_fapi_adaptor srsran_upper_phy)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(${SOURCES} PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES srsran_hal_pusch
        srsran_hal_pdsch
        srsran_hal_bbdev)
endif (DPDK_FOUND)
target_link_libraries(srsran_o_du_low PUBLIC ${DU_LOW_LIBRARIES})
