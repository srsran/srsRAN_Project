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

#include "srsran/adt/interval.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/sliv.h"

namespace srsran {

/// Range [start,stop) of OFDM symbols.
struct ofdm_symbol_range : public interval<uint8_t> {
  using interval<uint8_t>::interval;
};

/// \brief Converts SLIV to OFDM symbol start S and length L.
/// \param[in] sliv An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
/// \param[out] symbols Symbol interval as [S, S+L).
inline ofdm_symbol_range sliv_to_ofdm_symbols(uint32_t sliv)
{
  uint8_t symbol_S, symbol_L;
  sliv_to_s_and_l(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, sliv, symbol_S, symbol_L);
  return {symbol_S, symbol_S + symbol_L};
}

/// \brief Converts OFDM symbol range to SLIV.
/// \param[out] symbols Symbol interval as [S, S+L).
/// \param[in] sliv An index giving a combination (jointly encoded) of start symbols and length indicator (SLIV).
inline uint32_t ofdm_symbol_range_to_sliv(ofdm_symbol_range symb_range)
{
  return sliv_from_s_and_l(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, symb_range.start(), symb_range.length());
}

} // namespace srsran

namespace fmt {

/// Format intervals with the notation [start, stop)
template <>
struct formatter<srsran::ofdm_symbol_range> : public formatter<srsran::interval<uint8_t>> {
  template <typename FormatContext>
  auto format(const srsran::ofdm_symbol_range& symbols, FormatContext& ctx)
  {
    return formatter<srsran::interval<uint8_t>>::format(symbols, ctx);
  }
};

} // namespace fmt
