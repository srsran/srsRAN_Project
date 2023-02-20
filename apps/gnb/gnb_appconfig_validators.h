/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "gnb_appconfig.h"

namespace srsran {

/// Validates the given GNB application configuration. Returns true on success, false otherwise.
bool validate_appconfig(const gnb_appconfig& config);

} // namespace srsran
