/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/du_manager/du_manager_factory.h"
#include "du_manager_impl.h"
#include "srsgnb/support/timers.h"

using namespace srsgnb;
using namespace srs_du;

std::unique_ptr<du_manager_interface>
srsgnb::srs_du::create_du_manager(timer_manager&                               timers,
                                  mac_ue_configurator&                         mac_ue_mng,
                                  mac_cell_manager&                            mac_cell_mng,
                                  f1ap_connection_manager&                     f1ap_conn_mng,
                                  f1ap_ue_context_manager&                     f1ap_ue_ctx_mng,
                                  f1ap_rrc_message_transfer_procedure_handler& f1ap_rrc,
                                  task_executor&                               du_mng_exec,
                                  const std::vector<du_cell_config>&           cells)
{
  du_manager_config_t cfg{};
  cfg.timers          = &timers;
  cfg.mac_ue_mng      = &mac_ue_mng;
  cfg.mac_cell_mng    = &mac_cell_mng;
  cfg.f1ap_conn_mng   = &f1ap_conn_mng;
  cfg.f1ap_ue_ctx_mng = &f1ap_ue_ctx_mng;
  cfg.f1ap_rrc        = &f1ap_rrc;
  cfg.du_mng_exec     = &du_mng_exec;
  cfg.du_cells        = cells;
  auto du_manager     = std::make_unique<du_manager_impl>(std::move(cfg));
  return du_manager;
}
