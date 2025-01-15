/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_high/du_manager/du_configurator.h"
#include "srsran/e2/e2_du.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/f1ap/du/f1ap_du.h"

namespace srsran {
/// Creates a instance of an E2 interface (with subscription manager)
std::unique_ptr<e2_agent> create_e2_du_agent(const e2ap_configuration&      e2ap_cfg_,
                                             e2_connection_client&          e2_client_,
                                             e2_du_metrics_interface*       e2_metrics_var,
                                             srs_du::f1ap_ue_id_translator* f1ap_ue_id_translator_,
                                             srs_du::du_configurator*       du_configurator_,
                                             timer_factory                  timers_,
                                             task_executor&                 e2_exec_);

} // namespace srsran
