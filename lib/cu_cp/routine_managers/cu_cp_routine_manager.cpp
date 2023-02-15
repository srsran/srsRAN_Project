/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_routine_manager.h"
#include "../routines/initial_cu_cp_setup_routine.h"

using namespace srsgnb;
using namespace srs_cu_cp;

cu_cp_routine_manager::cu_cp_routine_manager(
    cu_cp_ngap_control_notifier&                                                   ngap_ctrl_notifier_,
    ngap_cu_cp_connection_notifier&                                                cu_cp_ngap_ev_notifier_,
    std::unordered_map<cu_up_index_t, std::unique_ptr<cu_up_processor_interface>>& cu_up_db_) :
  ngap_ctrl_notifier(ngap_ctrl_notifier_),
  cu_cp_ngap_ev_notifier(cu_cp_ngap_ev_notifier_),
  cu_up_db(cu_up_db_),
  main_ctrl_loop(128)
{
}

void cu_cp_routine_manager::start_initial_cu_cp_setup_routine(const ngap_configuration& ngap_cfg)
{
  main_ctrl_loop.schedule(
      launch_async<initial_cu_cp_setup_routine>(ngap_cfg, ngap_ctrl_notifier, cu_cp_ngap_ev_notifier));

  // start E1 setup procedure(s)
  for (auto& cu_up : cu_up_db) {
    cu_up.second->start();
  }
}
