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

add_subdirectory(pusch)

set(HWACC_FACTORIES_DEFINITIONS "")

if (ENABLE_PDSCH_HWACC)
    list(APPEND HWACC_FACTORIES_DEFINITIONS ENABLE_PDSCH_HWACC)
endif ()


add_library(hal_hwacc_pdsch STATIC hw_accelerator_factories.cpp
                                   hw_accelerator_pdsch_enc_impl.cpp
                                   hw_accelerator_pdsch_enc_acc100_impl.cpp)

set_source_files_properties(hw_accelerator_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${HWACC_FACTORIES_DEFINITIONS}")

target_link_libraries(hal_hwacc_pdsch hal_bbdev hal_dpdk ${DPDK_LIBRARIES})
