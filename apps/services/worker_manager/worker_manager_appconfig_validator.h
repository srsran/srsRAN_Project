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

namespace srsran {

struct expert_execution_appconfig;

/// Validates the given expert configuration of the application. Returns true on success, false otherwise.
bool validate_expert_execution_appconfig(const expert_execution_appconfig& config);

} // namespace srsran
