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
        common/e1ap_asn1_packer.cpp
        cu_cp/e1ap_cu_cp_factory.cpp
        cu_cp/e1ap_cu_cp_impl.cpp
        cu_cp/procedures/bearer_context_setup_procedure.cpp
        cu_cp/procedures/bearer_context_release_procedure.cpp
        cu_cp/procedures/bearer_context_modification_procedure.cpp
        cu_up/e1ap_cu_up_factory.cpp
        cu_up/e1ap_cu_up_impl.cpp
        cu_up/e1ap_cu_up_connection_handler.cpp
        cu_up/procedures/e1ap_cu_up_setup_procedure.cpp
    )

add_library(srsran_e1ap STATIC ${SOURCES})
target_link_libraries(srsran_e1ap e1ap_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
