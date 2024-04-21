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

#include "srsran/ran/s_nssai.h"

namespace srsran {

struct RRM_policy_member {
  std::string plmn_id;
  s_nssai_t   s_nssai;
};

struct RRM_policy_ratio_group {
  RRM_policy_member pol_member;
  int               min_PRB_policy_ratio;
  int               max_PRB_policy_ratio;
  int               ded_PRB_policy_ratio;
};

} // namespace srsran
