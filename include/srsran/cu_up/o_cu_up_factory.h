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

#include "srsran/cu_up/o_cu_up.h"
#include "srsran/cu_up/o_cu_up_config.h"
#include <memory>

namespace srsran {
namespace srs_cu_up {

/// ORAN CU-UP interface with the given configuration and dependencies.
std::unique_ptr<o_cu_up> create_o_cu_up(const o_cu_up_config& config, o_cu_up_dependencies&& dependencies);

} // namespace srs_cu_up
} // namespace srsran
