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

add_subdirectory(cell_meas_manager)
add_subdirectory(mobility_manager)
add_subdirectory(up_resource_manager)
add_subdirectory(ue_security_manager)

set(SOURCES
        cu_cp_factory.cpp
        cu_cp_impl.cpp
        cu_configurator_impl.cpp
        cu_cp_executor_mapper.cpp
        cu_cp_controller/cu_cp_controller.cpp
        cu_cp_controller/amf_connection_manager.cpp
        cu_cp_controller/du_connection_manager.cpp
        cu_cp_controller/cu_up_connection_manager.cpp
        cu_up_processor/cu_up_processor_impl.cpp
        cu_up_processor/cu_up_processor_factory.cpp
        cu_up_processor/cu_up_processor_repository.cpp
        du_processor/du_configuration_manager.cpp
        du_processor/du_processor_impl.cpp
        du_processor/du_processor_factory.cpp
        du_processor/du_processor_repository.cpp
        metrics_handler/metrics_handler_impl.cpp
        ngap_repository.cpp
        routines/amf_connection_loss_routine.cpp
        routines/amf_connection_setup_routine.cpp
        routines/amf_connection_removal_routine.cpp
        routines/amf_reconnection_routine.cpp
        routines/cell_activation_routine.cpp
        routines/cell_deactivation_routine.cpp
        routines/initial_context_setup_routine.cpp
        routines/pdu_session_routine_helpers.cpp
        routines/pdu_session_resource_setup_routine.cpp
        routines/pdu_session_resource_release_routine.cpp
        routines/pdu_session_resource_modification_routine.cpp
        routines/ue_context_release_routine.cpp
        routines/reestablishment_context_modification_routine.cpp
        routines/ue_removal_routine.cpp
        routines/ue_transaction_info_release_routine.cpp
        routines/ue_amf_context_release_request_routine.cpp
        routines/mobility/mobility_helpers.cpp
        routines/mobility/intra_cu_handover_routine.cpp
        routines/mobility/intra_cu_handover_target_routine.cpp
        routines/mobility/inter_cu_handover_source_routine.cpp
        routines/mobility/inter_cu_handover_target_routine.cpp
        routines/mobility/inter_cu_handover_execution_target_routine.cpp
        routines/mobility/handover_reconfiguration_routine.cpp
        task_schedulers/ngap_task_scheduler.cpp
        ue_manager/cu_cp_ue_impl.cpp
        ue_manager/ue_manager_impl.cpp
        ue_manager/ue_task_scheduler_impl.cpp
        paging/paging_message_handler.cpp
)

add_library(srsran_cu_cp STATIC ${SOURCES})

target_link_libraries(srsran_cu_cp
        srsran_cu_cp_cell_meas_manager
        srsran_cu_cp_mobility_manager
        srsran_ue_security_manager
        srsran_e1ap_cu_cp
        srsran_f1ap_cu
        srsran_ngap
        srsran_nrppa
        srsran_rrc
        srsran_ran
        srslog
        srsran_support
        srsran_security
        srsran_e2
        ${CMAKE_DL_LIBS}
)

add_library(srsran_o_cu_cp o_cu_cp_factory.cpp o_cu_cp_impl.cpp)
target_link_libraries(srsran_o_cu_cp PRIVATE srsran_cu_cp)
