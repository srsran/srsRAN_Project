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

#include "srsran/cu_cp/cu_configurator.h"
#include "srsran/e2/e2_cu.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/support/timers.h"

namespace srsran {

/// Creates a instance of an E2 interface (with subscription manager)
std::unique_ptr<e2_agent> create_e2_cu_cp_agent(const e2ap_configuration& e2ap_cfg_,
                                                e2_connection_client&     e2_client_,
                                                e2_cu_metrics_interface*  e2_metrics_,
                                                cu_configurator*          cu_configurator_,
                                                timer_factory             timers_,
                                                task_executor&            e2_exec_);

} // namespace srsran
