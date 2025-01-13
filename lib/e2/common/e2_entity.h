/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "e2_connection_handler.h"
#include "e2_impl.h"
#include "procedures/e2_setup_procedure.h"
#include "procedures/e2_subscription_setup_procedure.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/du/du_high/du_manager/du_configurator.h"
#include "srsran/e2/e2_cu.h"
#include "srsran/e2/e2_du.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/e2sm/e2sm_factory.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include <map>
#include <memory>

namespace srsran {

/// Wrapper for the E2 interface that puts function calls into its own task executor
class e2_entity final : public e2_agent
{
public:
  e2_entity(const e2ap_configuration                                         cfg_,
            e2_connection_client&                                            e2_client_,
            std::variant<e2_du_metrics_interface*, e2_cu_metrics_interface*> e2_metrics_,
            srs_du::f1ap_ue_id_translator*                                   f1ap_ue_id_translator_,
            srs_du::du_configurator*                                         du_configurator_,
            timer_factory                                                    timers_,
            task_executor&                                                   task_exec_);

  // E2 Agent interface.
  void          start() override;
  void          stop() override;
  e2_interface& get_e2_interface() override { return *e2ap; };

  void on_e2_disconnection() override;

private:
  srslog::basic_logger&    logger;
  const e2ap_configuration cfg;

  void build_e2_kpm_du(std::variant<e2_du_metrics_interface*, e2_cu_metrics_interface*> e2_metrics_,
                       srs_du::f1ap_ue_id_translator*                                   f1ap_ue_id_translator);
  void build_e2_kpm_cu_up(std::variant<e2_du_metrics_interface*, e2_cu_metrics_interface*> e2_metrics_);
  void build_e2_kpm_cu_cp(std::variant<e2_du_metrics_interface*, e2_cu_metrics_interface*> e2_metrics_);

  // Handler for E2AP tasks.
  task_executor&            task_exec;
  fifo_async_task_scheduler main_ctrl_loop;

  std::unique_ptr<e2sm_manager>              e2sm_mngr         = nullptr;
  std::unique_ptr<e2_subscription_manager>   subscription_mngr = nullptr;
  std::unique_ptr<e2_interface>              e2ap              = nullptr;
  std::vector<std::unique_ptr<e2sm_handler>> e2sm_handlers;
};

} // namespace srsran
