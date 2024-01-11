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

#include <algorithm>
#include <cstdint>
#include <string>

namespace srsran {

/// \brief 36-bit identifying an NR Cell Id as specified in subclause 9.3.1.7 of 3GPP TS 38.413
/// \remark The leftmost (22-32) bits of the NR Cell Identity correspond to the gNB ID and remaining (4-14) bits for
/// Cell ID.
using nr_cell_id_t = uint64_t;

/// \brief The NR Cell Global Identity (NR-CGI)
struct nr_cell_global_id_t {
  nr_cell_global_id_t() = default;
  nr_cell_global_id_t(uint16_t mcc_, uint16_t mnc_, std::string plmn_, std::string plmn_hex_, nr_cell_id_t nci_) :
    mcc(mcc_), mnc(mnc_), plmn(plmn_), plmn_hex(plmn_hex_), nci(nci_)
  {
  }

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
    std::string plmn_copy{plmn}, rhs_plmn_copy{rhs.plmn};
    std::transform(plmn_copy.begin(), plmn_copy.end(), plmn_copy.begin(), ::toupper);
    std::transform(rhs_plmn_copy.begin(), rhs_plmn_copy.end(), rhs_plmn_copy.begin(), ::toupper);
    return nci == rhs.nci && plmn_copy == rhs_plmn_copy;
  }
  bool operator!=(const nr_cell_global_id_t& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
