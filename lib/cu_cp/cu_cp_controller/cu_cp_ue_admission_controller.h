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

#include "srsran/ran/plmn_identity.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_ue_admission_controller
{
public:
  virtual ~cu_cp_ue_admission_controller() = default;

  /// Determines whether the CU-CP is in a condition to accept new UEs for a given PLMN.
  virtual bool request_ue_setup(plmn_identity plmn) const = 0;
};

} // namespace srs_cu_cp
} // namespace srsran