/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/interval.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/sliv.h"

namespace srsgnb {

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

} // namespace srsgnb

namespace fmt {

/// Format intervals with the notation [start, stop)
template <>
struct formatter<srsgnb::ofdm_symbol_range> : public formatter<srsgnb::interval<uint8_t>> {
  template <typename FormatContext>
  auto format(const srsgnb::ofdm_symbol_range& symbols, FormatContext& ctx)
  {
    return formatter<srsgnb::interval<uint8_t>>::format(symbols, ctx);
  }
};

} // namespace fmt
