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

#include "srsran/ran/cause/e1ap_cause.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/ran/cu_types.h"

namespace srsran {

enum class cu_up_cn_support_t { c_epc = 0, c_5gc, both };

/// \brief CU-UP initiated E1 setup request.
struct cu_up_e1_setup_request {
  uint64_t                            gnb_cu_up_id;
  std::optional<std::string>          gnb_cu_up_name;
  cu_up_cn_support_t                  cn_support;
  std::vector<supported_plmns_item_t> supported_plmns;
  std::optional<uint16_t>             gnb_cu_up_capacity;

  unsigned max_setup_retries = 5;
};

/// \brief Response to CU-UP initiated E1 setup request.
struct cu_up_e1_setup_response {
  bool success = false;

  // e1 setup response
  std::optional<std::string> gnb_cu_cp_name;

  // e1 setup failure
  std::optional<e1ap_cause_t>       cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

} // namespace srsran
