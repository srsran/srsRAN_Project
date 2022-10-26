/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include <memory>

namespace srsgnb {

/// Creates an instance of an CU-CP.
std::unique_ptr<srs_cu_cp::cu_cp_interface> create_cu_cp(const srs_cu_cp::cu_cp_configuration& cfg_);

} // namespace srsgnb
