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
        cu_up_executor_mapper.cpp
        cu_up_factory.cpp
        cu_up_impl.cpp
        cu_up_manager_impl.cpp
        ue_manager.cpp
        pdu_session_manager_impl.cpp
        ngu_session_manager_impl.cpp
        routines/cu_up_setup_routine.cpp
        routines/cu_up_e1_connection_loss_routine.cpp
        routines/cu_up_bearer_context_modification_routine.cpp
        routines/cu_up_test_mode_routines.cpp
)

add_library(srsran_cu_up STATIC ${SOURCES})
target_link_libraries(srsran_cu_up
        srsran_e1ap_common
        srsran_e1ap_cu_up
        srsran_gtpu
        srsran_pdcp
        srsran_f1u_cu_up
        srsran_gateway
        e1ap_asn1
        srsran_sdap
        srsran_security
        srsran_support
        srslog)

add_library(srsran_o_cu_up STATIC
        o_cu_up_factory.cpp
        o_cu_up_impl.cpp)
target_link_libraries(srsran_o_cu_up PRIVATE srsran_cu_up srsran_e2)
