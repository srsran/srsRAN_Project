/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "gnb_id.h"
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
inline uint64_t make_nr_cell_identity(gnb_id_t gnb_id, uint16_t cell_identity)
{
  // The leftmost (22-32) bits of the NR Cell Identity correspond to the gNB ID and remaining (4-14) bits for Cell ID.
  uint64_t       nci                = 0;
  const uint64_t gnb_id_mask        = ((uint64_t)1U << gnb_id.bit_length) - 1;
  const uint16_t cell_identity_mask = ((uint16_t)1U << (36U - gnb_id.bit_length)) - 1;
  nci                               = (uint64_t)(gnb_id.id & gnb_id_mask) << (36U - gnb_id.bit_length);
  nci |= (cell_identity & cell_identity_mask);
  return nci;
}

/// Returns the gNB ID from the NR Cell Identity.
inline gnb_id_t get_gnb_id(nr_cell_id_t nr_cell_id, uint8_t gnb_id_bit_length)
{
  gnb_id_t gnb_id;
  gnb_id.id         = (uint32_t)nr_cell_id >> (36 - gnb_id_bit_length);
  gnb_id.bit_length = gnb_id_bit_length;
  return gnb_id;
};

} // namespace config_helpers
} // namespace srsran
