#
# Copyright 2021-2023 Software Radio Systems Limited
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

set(SOURCES
      cu_cp_factory.cpp
      cu_cp_impl.cpp
      cu_up_processor_impl.cpp
      cu_up_processor_factory.cpp
      ue_manager_impl.cpp
      du_processor/du_processor_impl.cpp
      du_processor/du_processor_factory.cpp
      du_processor/du_processor_repository.cpp
      routine_managers/cu_cp_routine_manager.cpp
      routine_managers/cu_up_processor_routine_manager.cpp
      routine_managers/du_processor_routine_manager.cpp
      routines/initial_cu_cp_setup_routine.cpp
      routines/initial_cu_up_processor_setup_routine.cpp
      routines/pdu_session_routine_helpers.cpp
      routines/pdu_session_resource_setup_routine.cpp
      routines/pdu_session_resource_release_routine.cpp
      routines/pdu_session_resource_modification_routine.cpp
      routines/ue_context_release_routine.cpp
      routines/reestablishment_context_modification_routine.cpp
      routines/mobility/mobility_helpers.cpp
      routines/mobility/inter_du_handover_routine.cpp
      routines/mobility/inter_ngran_node_n2_handover_routine.cpp
      routines/mobility/inter_cu_handover_target_routine.cpp
      routines/mobility/handover_reconfiguration_routine.cpp
      task_schedulers/ue_task_scheduler.cpp
      task_schedulers/cu_up_task_scheduler.cpp
      task_schedulers/du_task_scheduler.cpp
)

add_library(srsran_cu_cp STATIC ${SOURCES})

target_link_libraries(srsran_cu_cp srsran_cu_cp_cell_meas_manager srsran_cu_cp_mobility_manager srslog srsran_support srsran_e1ap srsran_f1ap_cu srsran_pdcp srsran_ngap srsran_rrc srsran_security)
