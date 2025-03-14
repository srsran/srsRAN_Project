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

#include "srsran/cu_cp/cu_configurator.h"
#include "srsran/e2/e2_cu.h"
#include "srsran/e2/e2_factory.h"

namespace srsran {

/// Creates a instance of an E2 interface (with subscription manager)
std::unique_ptr<e2_agent> create_e2_cu_cp_agent(const e2ap_configuration& e2ap_cfg_,
                                                e2_connection_client&     e2_client_,
                                                e2_cu_metrics_interface*  e2_metrics_,
                                                cu_configurator*          cu_configurator_,
                                                timer_factory             timers_,
                                                task_executor&            e2_exec_);

} // namespace srsran
