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

#include "cu_cp_unit_config.h"

namespace srsran {

/// Validates the given CU-CP unit configuration. Returns true on success, false otherwise.
bool validate_cu_cp_unit_config(const cu_cp_unit_config& config);

} // namespace srsran