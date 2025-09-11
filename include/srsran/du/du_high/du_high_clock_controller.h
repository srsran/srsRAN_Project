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

#include "srsran/mac/mac_clock_controller.h"
#include <memory>

namespace srsran {

class task_executor;
class io_broker;
class timer_manager;

namespace srs_du {

/// Creates a clock controller for the DU-high that allows switching between automatic and manual ticking modes.
std::unique_ptr<mac_clock_controller>
create_du_high_clock_controller(timer_manager& timers, io_broker& broker, task_executor& tick_exec);

} // namespace srs_du
} // namespace srsran
