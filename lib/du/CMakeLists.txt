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

add_subdirectory(du_high)
if (NOT DU_SPLIT_6)
   add_subdirectory(du_low)
endif ()

set(SOURCES du_cell_config_validation.cpp du_update_config_helpers.cpp)
add_library(srsran_du_config_validators STATIC ${SOURCES})

add_library(srsran_du_wrapper STATIC
        du_wrapper_impl.cpp
        du_wrapper_factory.cpp)

target_link_libraries(srsran_du_wrapper PUBLIC srsran_du_high_wrapper srsran_du_low_wrapper)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(du_wrapper_factory.cpp du_wrapper_impl.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES hal_hwacc_pusch
           hal_hwacc_pdsch
           hal_bbdev_factory)
endif (DPDK_FOUND)
