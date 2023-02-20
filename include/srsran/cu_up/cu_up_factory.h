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

#include "srsran/cu_up/cu_up.h"
#include "srsran/cu_up/cu_up_configuration.h"
#include <memory>

namespace srsran {

/// Creates an instance of an CU-UP.
std::unique_ptr<srs_cu_up::cu_up_interface> create_cu_up(const srs_cu_up::cu_up_configuration& cfg_);

} // namespace srsran
