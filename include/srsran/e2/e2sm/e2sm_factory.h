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

#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/e2/e2sm/e2sm_kpm.h"

namespace srsran {

/// create instance of e2sm handler
std::unique_ptr<e2sm_handler> create_e2sm(std::string e2sm_version, e2sm_kpm_meas_provider& meas_provider);
} // namespace srsran
