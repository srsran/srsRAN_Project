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
