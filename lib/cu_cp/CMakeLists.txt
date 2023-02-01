#
# Copyright 2013-2022 Software Radio Systems Limited
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

add_library(srsgnb_cu_cp STATIC ${SOURCES})

target_link_libraries(srsgnb_cu_cp srslog srsgnb_support srsgnb_e1 srsgnb_f1c_cu srsgnb_pdcp srsgnb_ngap srsgnb_rrc srsgnb_security)
