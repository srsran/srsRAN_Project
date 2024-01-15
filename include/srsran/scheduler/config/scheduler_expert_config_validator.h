/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/expected.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/config/scheduler_expert_config_validator.h"
#include <string>

namespace srsran {

/// \brief Checks whether the provided scheduler expert configuration is valid.
///
/// \param config Scheduler expert configuration.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> is_scheduler_expert_config_valid(const scheduler_expert_config& config)
{
  // :TODO: Implement me!
  return {};
}

} // namespace srsran
