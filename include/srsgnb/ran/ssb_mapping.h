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
#include "srsgnb/ran/frequency_range.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/ssb_properties.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/support/math_utils.h"
#include <array>
#include <cassert>

namespace srsgnb {

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
    constexpr std::array<unsigned, 4>  first_symbols = {4, 8, 16, 20};
    constexpr std::array<unsigned, 16> n             = {0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18};
    assert(ssb_idx < first_symbols.size() * n.size());
    return first_symbols[ssb_idx % first_symbols.size()] + 28 * n[ssb_idx / first_symbols.size()];
  }

  // Case E - 240 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes
  //{ 8 , 12 , 16 , 20 , 32 , 36 , 40 , 44 } + 56 ⋅ n . For carrier frequencies within FR2, n = 0 , 1 , 2 , 3 , 5 , 6 ,
  // 7 , 8 .
  if (pattern_case == ssb_pattern_case::E) {
    constexpr std::array<unsigned, 8>  first_symbols = {8, 12, 16, 20, 32, 36, 40, 44};
    constexpr std::array<unsigned, 16> n             = {0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18};
    assert(ssb_idx < first_symbols.size() * n.size());
    return first_symbols[ssb_idx % first_symbols.size()] + 56 * n[ssb_idx / first_symbols.size()];
  }

  // Impossible!
  srsran_assert(false, "Invalid SSB pattern case");
  return {};
}

/// \brief Calculates the position of the SS/PBCH block first subcarrier relative to the bottom of the grid (pointA).
///
/// Assertions are triggered for:
/// - invalid SS/PBCH SCS and frequency combinations, or
/// - the result would point in a fraction of the subcarrier index.
///
/// \param[in] fr                Frequency range.
/// \param[in] ssb_scs           SS/PBCH block SCS.
/// \param[in] common_scs        Higher layer parameter \c subCarrierSpacingCommon as per TS38.331 \c MIB.
/// \param[in] offset_to_pointA  \see ssb_offset_to_pointA.
/// \param[in] subcarrier_offset \see ssb_subcarrier_offset.
/// \return The index of the lowest subcarrier of the SS/PBCH block.
inline unsigned ssb_get_k_first(frequency_range       fr,
                                subcarrier_spacing    ssb_scs,
                                subcarrier_spacing    common_scs,
                                ssb_offset_to_pointA  offset_to_pointA,
                                ssb_subcarrier_offset subcarrier_offset)
{
  // Verify the SCS are valid for the frequency range.
  srsran_assert(is_scs_valid(ssb_scs, fr),
                "Unsupported combination of FR{} and SSB SCS {}kHz.",
                fr == frequency_range::FR1 ? 1 : 2,
                scs_to_khz(ssb_scs));
  srsran_assert(is_scs_valid(common_scs, fr),
                "Unsupported combination of FR{} and  Common SCS {}kHz.",
                fr == frequency_range::FR1 ? 1 : 2,
                scs_to_khz(common_scs));

  // Select the Point A offset SCS depending on the frequency range and convert SCS to kHz.
  unsigned pointA_offset_scs_kHz =
      scs_to_khz((fr == frequency_range::FR1) ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz60);

  // Select the subcarrier offset SCS depending on the frequency range and convert it to kHz.
  unsigned subcarrier_offset_scs_kHz =
      scs_to_khz((fr == frequency_range::FR1) ? subcarrier_spacing::kHz15 : common_scs);

  // SS/PBCH block SCS in kHz.
  unsigned ssb_scs_kHz = scs_to_khz(ssb_scs);

  // Calculate number of subcarriers from point A to the first subcarrier of the SS/PBCH block with 15kHz reference.
  unsigned k_first_15kHz =
      (offset_to_pointA * NRE * pointA_offset_scs_kHz + subcarrier_offset * subcarrier_offset_scs_kHz) / 15;

  // Make sure the conversion from the reference in 15kHz to the SSB SCS is not fractional.
  srsran_assert((k_first_15kHz * 15) % ssb_scs_kHz == 0,
                "Unsupported combination of FR{}, SSB SCS {}kHz, Common SCS {}kHz, offsetToPointA {} and "
                "ssb-SubcarrierOffset {}.",
                fr == frequency_range::FR1 ? 1 : 2,
                ssb_scs_kHz,
                scs_to_khz(common_scs),
                offset_to_pointA,
                subcarrier_offset);

  // Calculate actual result.
  return (k_first_15kHz * 15) / ssb_scs_kHz;
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
