/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {
namespace config_validators {

/// \brief Validates \c sched_cell_configuration_request_message used to add a cell.
/// \param[in] msg scheduler cell configuration message to be validated.
void validate_sched_cell_configuration_request_message(const sched_cell_configuration_request_message& msg);

} // namespace config_validators
} // namespace srsgnb
