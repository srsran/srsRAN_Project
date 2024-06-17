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

#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

/// UE configuration passed to CU-CP
struct ue_configuration {
  std::chrono::seconds inactivity_timer;
  unsigned             max_nof_supported_ues = MAX_NOF_CU_UES;
};

} // namespace srs_cu_cp
} // namespace srsran
