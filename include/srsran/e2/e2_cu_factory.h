/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "e2_cu.h"
#include "e2_factory.h"
#pragma once

namespace srsran {
/// Creates a instance of an E2 interface (with subscription manager)
std::unique_ptr<e2_interface> create_e2_cu_entity(e2ap_configuration&      e2ap_cfg_,
                                                  e2_connection_client*    e2_client_,
                                                  e2_cu_metrics_interface* e2_metrics_var,
                                                  timer_factory            timers_,
                                                  task_executor&           e2_exec_);

} // namespace srsran
