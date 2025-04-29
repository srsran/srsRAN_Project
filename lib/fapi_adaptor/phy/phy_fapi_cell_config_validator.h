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

#include "srsran/srslog/logger.h"

namespace srsran {

namespace fapi {
struct config_request;
}

namespace fapi_adaptor {

/// \brief Validates the FAPI config.request parameters of the given message and returns false if one or more parameters
/// are invalid or unsupported, otherwise true.
///
/// When the validation fails, a message is logged indicating the property that is unsupported/invalid.
bool validate_config_request_message(const fapi::config_request& msg, srslog::basic_logger& logger);

} // namespace fapi_adaptor
} // namespace srsran
