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

#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/gnb_id.h"
#include <optional>
#include <string>

namespace srsran {

/// \brief E2AP configuration
struct e2ap_configuration {
  gnb_id_t                   gnb_id = {0, 22};
  std::string                plmn; /// Full PLMN as string (without possible filler digit) e.g. "00101"
  std::optional<gnb_du_id_t> gnb_du_id;
  unsigned                   max_setup_retries = 5;
  bool                       e2sm_kpm_enabled  = false;
  bool                       e2sm_rc_enabled   = false;
};

} // namespace srsran
