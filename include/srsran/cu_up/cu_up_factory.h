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

#include "srsran/cu_up/cu_up.h"
#include "srsran/cu_up/cu_up_config.h"
#include <memory>

namespace srsran {
namespace srs_cu_up {

/// Creates an instance of a CU-UP with the given parameters.
std::unique_ptr<cu_up_interface> create_cu_up(const cu_up_config& cfg, const cu_up_dependencies& dependencies);

} // namespace srs_cu_up
} // namespace srsran
