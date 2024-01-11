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

#include "nr_cgi.h"
#include "fmt/format.h"

namespace srsran {
namespace config_helpers {

/// Returns true if the given struct is valid, otherwise false.
inline bool is_valid(const nr_cell_global_id_t& cgi)
{
  // MCC and MNC cannot be null (init value is 0xf000).
  if (cgi.mcc == 0 || cgi.mcc == 0xf000 || cgi.mnc == 0 || cgi.mnc == 0xf000 || cgi.plmn.empty() ||
      cgi.plmn_hex.empty()) {
    fmt::print("Invalid MCC, MNC or PLMN configuration.\n");
    return false;
  }

  // Allow gNB ID and cell ID to be zero.

  return true;
}

/// Returns NR Cell Identity.
inline uint64_t make_nr_cell_identity(uint32_t gnb_id, uint8_t gnb_id_bit_length, uint16_t cell_identity)
{
  // The leftmost (22-32) bits of the NR Cell Identity correspond to the gNB ID and remaining (4-14) bits for Cell ID.
  const uint64_t gnb_id_mask        = ((uint64_t)1U << gnb_id_bit_length) - 1;
  const uint16_t cell_identity_mask = ((uint16_t)1U << (36U - gnb_id_bit_length)) - 1;
  uint64_t       nci                = (uint64_t)(gnb_id & gnb_id_mask) << (36U - gnb_id_bit_length);
  nci |= (cell_identity & cell_identity_mask);
  return nci;
}

} // namespace config_helpers
} // namespace srsran
