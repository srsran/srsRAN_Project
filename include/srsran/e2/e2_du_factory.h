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

#include "srsran/du/du_high/du_manager/du_configurator.h"
#include "srsran/e2/e2_du.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/support/timers.h"

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
