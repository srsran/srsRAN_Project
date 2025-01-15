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

#include "srsran/adt/expected.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

class cell_configuration;

namespace config_validators {

/// \brief Validates \c sched_ue_creation_request_message used to create a UE.
/// \param[in] msg scheduler ue creation request message to be validated.
/// \param[in] cell_cfg Cell configuration.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg,
                                                                   const cell_configuration&                cell_cfg);

} // namespace config_validators
} // namespace srsran
