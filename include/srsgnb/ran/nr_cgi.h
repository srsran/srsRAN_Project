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

/// 36-bit identifying an NR Cell Id as specified in subclause 9.3.1.7 of 3GPP TS 38.413
struct nr_cell_id_t {
  uint64_t packed;       /// All 36 bits combined
  uint32_t gnb_identity; /// 22 to 32 bits
  uint16_t cell_idenity; /// 4 to 14 bits
};

/// The NR Cell Global Identity (NR-CGI)
struct nr_cell_global_id_t {
  uint16_t     mcc;      /// 3 digits mobile country code (BCD encoded)
  uint16_t     mnc;      /// 2 or 3 digits mobile network code (BCD encoded)
  std::string  plmn;     /// Full PLMN as string (without possible filler digit)
  std::string  plmn_hex; /// Full PLMN as hex string with filler digit if needed
  nr_cell_id_t nci;      /// NR cell id
};

} // namespace srsran
