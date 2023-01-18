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
#include "routines/initial_cu_cp_setup_routine.h"

using namespace srsgnb;
using namespace srs_cu_cp;

cu_cp_routine_manager::cu_cp_routine_manager(const ngc_configuration&       ngc_cfg_,
                                             ngc_connection_manager&        ngc_conn_mng_,
                                             cu_cp_ngc_connection_notifier& cu_cp_ngc_ev_notifier_) :
  ngc_cfg(ngc_cfg_), ngc_conn_mng(ngc_conn_mng_), cu_cp_ngc_ev_notifier(cu_cp_ngc_ev_notifier_), main_ctrl_loop(128)
{
}

void cu_cp_routine_manager::start_initial_cu_cp_setup_routine()
{
  main_ctrl_loop.schedule(launch_async<initial_cu_cp_setup_routine>(ngc_cfg, ngc_conn_mng, cu_cp_ngc_ev_notifier));
}
