/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_SSB_MAPPING_H
#define SRSGNB_RAN_SSB_MAPPING_H

#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <array>
#include <cassert>

namespace srsgnb {

/// \brief Describes the different SS/PBCH block patterns defined in 38.213 section 4.1 Cell search.
enum class ssb_pattern_case { A, B, C, D, E, invalid };

/// \brief Calculates the first OFDM symbol in a 5ms SS/PBCH block burst
/// \param [in] pattern_case Provides the pattern
/// \param [in] ssb_idx Provides the SS/PBCH block opportunity index
/// \return the first OFDM symbol index in a half-frame
inline unsigned ssb_get_l_first(ssb_pattern_case pattern_case, unsigned ssb_idx)
{
  // Case A - 15 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes of { 2 , 8 } + 14 ⋅ n . For
  // carrier frequencies smaller than or equal to 3 GHz, n = 0 , 1 . For carrier frequencies within FR1 larger than 3
  // GHz, n = 0 , 1 , 2 , 3 .
  if (pattern_case == ssb_pattern_case::A) {
    constexpr std::array<unsigned, 2> first_symbols = {2, 8};
    return first_symbols[ssb_idx % first_symbols.size()] + 14 * (ssb_idx / first_symbols.size());
  }

  // Case B - 30 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes { 4 , 8 , 16 , 20 } + 28 ⋅ n .
  // For carrier frequencies smaller than or equal to 3 GHz, n = 0 . For carrier frequencies within FR1 larger than 3
  // GHz, n = 0 , 1 .
  if (pattern_case == ssb_pattern_case::B) {
    constexpr std::array<unsigned, 4> first_symbols = {4, 8, 16, 20};
    return first_symbols[ssb_idx % first_symbols.size()] + 28 * (ssb_idx / first_symbols.size());
  }

  // Case C - 30 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes { 2 , 8 } + 14 ⋅ n .
  // - For paired spectrum operation
  // For carrier frequencies smaller than or equal to 3 GHz, n = 0 , 1 . For carrier frequencies within FR1 larger
  // than 3 GHz, n = 0 , 1 , 2 , 3 .
  // - For unpaired spectrum operation
  // For carrier frequencies smaller than 1.88 GHz, n = 0 , 1 . For carrier frequencies within FR1 equal to or
  // larger than 1.88 GHz, n = 0 , 1 , 2 , 3 .
  if (pattern_case == ssb_pattern_case::C) {
    constexpr std::array<unsigned, 2> first_symbols = {2, 8};
    return first_symbols[ssb_idx % first_symbols.size()] + 14 * (ssb_idx / first_symbols.size());
  }

  // Case D - 120 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes { 4 , 8 , 16 , 20 } + 28 ⋅ n .
  // For carrier frequencies within FR2, n = 0 , 1 , 2 , 3 , 5 , 6 , 7 , 8 , 10 , 11 , 12 , 13 , 15 , 16 , 17 , 18 .
  if (pattern_case == ssb_pattern_case::D) {
    constexpr std::array<unsigned, 4> first_symbols = {4, 8, 16, 20};
    constexpr std::array<unsigned, 16> n            = {0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18};
    assert(ssb_idx < first_symbols.size() * n.size());
    return first_symbols[ssb_idx % first_symbols.size()] + 28 * n[ssb_idx / first_symbols.size()];
  }

  // Case E - 240 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes
  //{ 8 , 12 , 16 , 20 , 32 , 36 , 40 , 44 } + 56 ⋅ n . For carrier frequencies within FR2, n = 0 , 1 , 2 , 3 , 5 , 6 ,
  // 7 , 8 .
  if (pattern_case == ssb_pattern_case::E) {
    constexpr std::array<unsigned, 8> first_symbols = {8, 12, 16, 20, 32, 36, 40, 44};
    constexpr std::array<unsigned, 16> n            = {0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18};
    assert(ssb_idx < first_symbols.size() * n.size());
    return first_symbols[ssb_idx % first_symbols.size()] + 56 * n[ssb_idx / first_symbols.size()];
  }

  // Impossible!
  srsran_assert(false, "Invalid SSB pattern case");
  return {};
}

/// Calculates the position of the SS/PBCH block first subcarrier relative to the bottom of the grid (pointA)
inline unsigned ssb_get_k_first(unsigned numerology, unsigned pointA_offset, unsigned ssb_offset)
{
  return pointA_offset * NRE + (ssb_offset >> numerology);
}

/// Calculates SSB pattern from SSB subcarrier spacing and DL ARFCN.
ssb_pattern_case ssb_get_ssb_pattern(subcarrier_spacing ssb_scs, unsigned dl_arfcn);

/// \brief Calculates L_max, ie max number of SSB occasions per SSB period. Possible values are {4, 8, 64}.
/// \remark See TS 38.213, Section 4.1.
/// \param ssb_scs SSB Subcarrier Spacing.
/// \param dl_arfcn DL ARFCN.
/// \return L_max value.
uint8_t ssb_get_L_max(subcarrier_spacing ssb_scs, unsigned dl_arfcn);

} // namespace srsgnb

#endif // SRSGNB_RAN_SSB_MAPPING_H
