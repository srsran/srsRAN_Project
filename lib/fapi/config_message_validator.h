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

#include "srsran/srslog/logger.h"

namespace srsran {
namespace fapi {

struct config_request;

/// \brief Validates the FAPI config.request parameters of the given message and returns false if one or more parameters
/// are invalid or unsupported, otherwise true.
///
/// When the validation fails, a message is logged indicating the property that is unsupported/invalid.
bool validate_config_request_message(const config_request& msg, srslog::basic_logger& logger);

} // namespace fapi
} // namespace srsran
