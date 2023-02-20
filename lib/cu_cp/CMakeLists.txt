#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
      cu_cp_factory.cpp
      cu_cp.cpp
      cu_up_processor_impl.cpp
      cu_up_processor_factory.cpp
      du_processor_impl.cpp
      du_processor_factory.cpp
      ue_manager_impl.cpp
      routine_managers/cu_cp_routine_manager.cpp
      routine_managers/cu_up_processor_routine_manager.cpp
      routine_managers/du_processor_routine_manager.cpp
      routines/initial_cu_cp_setup_routine.cpp
      routines/initial_cu_up_processor_setup_routine.cpp
      routines/pdu_session_resource_setup_routine.cpp
      routines/ue_context_release_routine.cpp
      task_schedulers/ue_task_scheduler.cpp
      task_schedulers/cu_up_task_scheduler.cpp
      task_schedulers/du_task_scheduler.cpp
)

add_library(srsran_cu_cp STATIC ${SOURCES})

target_link_libraries(srsran_cu_cp srslog srsran_support srsran_e1ap srsran_f1ap_cu srsran_pdcp srsran_ngap srsran_rrc srsran_security)
