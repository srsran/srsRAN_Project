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

set(HWACC_FACTORIES_DEFINITIONS "")

if (ENABLE_PUSCH_HWACC)
    list(APPEND HWACC_FACTORIES_DEFINITIONS ENABLE_PUSCH_HWACC)
endif ()


add_library(hal_hwacc_pusch STATIC hw_accelerator_factories.cpp
                                   hw_accelerator_pusch_dec_impl.cpp
                                   hw_accelerator_pusch_dec_acc100_impl.cpp
                                   ext_harq_buffer_context_repository_factory.cpp)

set_source_files_properties(hw_accelerator_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${HWACC_FACTORIES_DEFINITIONS}")

target_link_libraries(hal_hwacc_pusch hal_bbdev hal_dpdk ${DPDK_LIBRARIES})
