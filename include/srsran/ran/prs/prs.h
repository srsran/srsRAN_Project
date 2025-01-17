/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

/// \file
/// \brief Positioning Reference Signals (PRS) parameters.

#pragma once

namespace srsran {

//// PRS transmission time domain duration.
enum class prs_num_symbols : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// PRS transmission comb size.
enum class prs_comb_size : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// \brief Determines whether the combination of time domain duration and comb size is valid.
///
/// The valid combinations are given in TS38.211 Section 7.4.1.7.3.
inline bool prs_valid_num_symbols_and_comb_size(prs_num_symbols nsymb, prs_comb_size comb_sz)
{
  uint8_t nsymb_u8   = static_cast<uint8_t>(nsymb);
  uint8_t comb_sz_u8 = static_cast<uint8_t>(comb_sz);
  return (nsymb_u8 >= comb_sz_u8) && (nsymb_u8 % comb_sz_u8 == 0);
}

} // namespace srsran
