/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
