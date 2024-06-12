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

namespace srsran {

struct cu_appconfig;

/// Validates the given CU application configuration. Returns true on success, false otherwise.
bool validate_cu_appconfig(const cu_appconfig& config);

} // namespace srsran
