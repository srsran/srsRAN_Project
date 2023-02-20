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

#include "nr_cgi.h"

namespace srsran {
namespace config_helpers {

/// Returns true if the given struct is valid, otherwise false.
inline bool is_valid(const nr_cell_global_id_t& cgi)
{
  // MCC and MNC cannot be null.
  if (cgi.mcc == 0 || cgi.mnc == 0 || cgi.plmn.empty() || cgi.plmn_hex.empty()) {
    fmt::print("Invalid MCC, MNC or PLMN configuration.\n");
    return false;
  }

  // Allow gNB ID and cell ID to be zero.

  return true;
}

} // namespace config_helpers
} // namespace srsran
