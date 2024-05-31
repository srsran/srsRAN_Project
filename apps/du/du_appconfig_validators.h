/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_appconfig.h"

namespace srsran {

/// Validates the given DU application configuration. Returns true on success, false otherwise.
bool validate_appconfig(const du_appconfig& config);

} // namespace srsran