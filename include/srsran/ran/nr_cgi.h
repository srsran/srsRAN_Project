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

#include <string>

namespace srsran {

/// \brief 36-bit identifying an NR Cell Id as specified in subclause 9.3.1.7 of 3GPP TS 38.413
/// \remark The leftmost (22-32) bits of the NR Cell Identity correspond to the gNB ID and remaining (4-14) bits for
/// Cell ID.
using nr_cell_id_t = uint64_t;

/// \brief The NR Cell Global Identity (NR-CGI)
struct nr_cell_global_id_t {
  /// 3 digits mobile country code (BCD encoded).
  uint16_t mcc;
  /// 2 or 3 digits mobile network code (BCD encoded).
  uint16_t mnc;
  /// Full PLMN as string (without possible filler digit).
  std::string plmn;
  /// Full PLMN as hex string with filler digit if needed.
  std::string plmn_hex;
  /// NR cell id.
  nr_cell_id_t nci;

  bool operator==(const nr_cell_global_id_t& rhs) const
  {
    return mcc == rhs.mcc && mnc == rhs.mnc && plmn == rhs.plmn && plmn_hex == rhs.plmn_hex && nci == rhs.nci;
  }
  bool operator!=(const nr_cell_global_id_t& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
