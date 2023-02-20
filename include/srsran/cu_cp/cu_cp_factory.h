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

#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include <memory>

namespace srsran {

/// Creates an instance of an CU-CP.
std::unique_ptr<srs_cu_cp::cu_cp_interface> create_cu_cp(const srs_cu_cp::cu_cp_configuration& cfg_);

} // namespace srsran
