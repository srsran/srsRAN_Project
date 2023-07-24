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

#include "band_helper.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/frequency_range.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/ssb_properties.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/math_utils.h"
#include <array>
#include <cassert>

namespace srsran {

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

/// \brief Calculates the position of the first SS/PBCH block subcarrier relative to Point A.
///
/// The result is expressed in units of subcarriers of SS/PBCH block SCS \c ssb_scs assuming that:
/// - the lowest subcarrier of the resource grid overlaps with Point A, and
/// - the resource grid SCS matches the SS/PBCH block SCS.
///
/// In other words, it is not possible to determine an SS/PBCH block position in the grid if the SS/PBCH block
/// subcarrier offset does not match the position of the resource grid subcarriers. Because of this, the parameters must
/// result in an integer subcarrier index of the SS/PBCH block SCS.
///
/// Assertions are triggered if:
/// - the provided SS/PBCH SCS and frequency range combination is invalid, or
/// - the result would point to a non-integer subcarrier index.
///
/// \param[in] fr                Frequency range.
/// \param[in] ssb_scs           SS/PBCH block subcarrier spacing.
/// \param[in] common_scs        Higher layer parameter \e subCarrierSpacingCommon as per TS38.331 \e MIB.
/// \param[in] offset_to_pointA  Offset to Point A (see [here](\ref ssb_offset_to_pointA) for more information).
/// \param[in] subcarrier_offset Subcarrier offset (see [here](\ref ssb_subcarrier_offset) for more information).
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

  // Verify the offset to Point A and the subcarrier offset are valid.
  srsran_assert(offset_to_pointA.valid(),
                "Invalid offset to Point A {} (max {})",
                offset_to_pointA.to_uint(),
                ssb_offset_to_pointA::max());
  srsran_assert(subcarrier_offset.is_valid(fr),
                "Invalid subcarrier offset {} for FR{} (max {})",
                subcarrier_offset.to_uint(),
                fr == frequency_range::FR1 ? 1 : 2,
                ssb_subcarrier_offset::max(fr));

  // Select Point A offset SCS depending on the frequency range and convert SCS to kHz.
  unsigned pointA_offset_scs_kHz =
      scs_to_khz((fr == frequency_range::FR1) ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz60);

  // Select the subcarrier offset SCS depending on the frequency range and convert it to kHz.
  unsigned subcarrier_offset_scs_kHz =
      scs_to_khz((fr == frequency_range::FR1) ? subcarrier_spacing::kHz15 : common_scs);

  // SS/PBCH block SCS in kHz.
  unsigned ssb_scs_kHz = scs_to_khz(ssb_scs);

  // Calculate the number of 15kHz subcarriers from point A to the first subcarrier of the SS/PBCH block.
  unsigned k_first_15kHz = (offset_to_pointA.to_uint() * NRE * pointA_offset_scs_kHz +
                            subcarrier_offset.to_uint() * subcarrier_offset_scs_kHz) /
                           15;

  // Make sure the above conversion is exact and has no remainder.
  srsran_assert((k_first_15kHz * 15) % ssb_scs_kHz == 0,
                "Unsupported combination of FR{}, SSB SCS {}kHz, Common SCS {}kHz, offsetToPointA {} and "
                "ssb-SubcarrierOffset {}.",
                fr == frequency_range::FR1 ? 1 : 2,
                ssb_scs_kHz,
                scs_to_khz(common_scs),
                offset_to_pointA.to_uint(),
                subcarrier_offset.to_uint());

  // Calculate actual result.
  return (k_first_15kHz * 15) / ssb_scs_kHz;
}

/// Calculates SSB pattern from SSB subcarrier spacing and DL ARFCN.
ssb_pattern_case ssb_get_ssb_pattern(subcarrier_spacing ssb_scs, unsigned dl_arfcn);

/// \brief Calculates L_max, ie max number of SSB occasions per SSB period. Possible values are {4, 8, 64}.
/// \remark See TS 38.213, Section 4.1.
/// \param ssb_scs SSB Subcarrier Spacing.
/// \param dl_arfcn DL ARFCN.
/// \param band NR band. If not provided, it will be derived from the DL-ARFCN.
/// \return L_max value.
uint8_t ssb_get_L_max(subcarrier_spacing ssb_scs, unsigned dl_arfcn, optional<nr_band> band = {});

/// \brief Calculate the CRBs (with reference to SCS common grid) where the SSB is allocated.
/// \param[in] ssb_scs      is the SSB Sub-Carrier Spacing, as per TS 38.213, Section 4.1.
/// \param[in] scs_common   is the Sub-Carrier Spacing of the Initial DL BWP.
/// \param[in] offset_to_pA is the OffsetToPointA, , as per TS 38.211, Section 4.4.4.2.
/// \param[in] k_ssb        is the SSB subcarrier offset, or k_ssb, as per TS 38.211, Section 7.4.3.1.
/// \return The CRB interval (with reference to SCS common grid) where the SSB is allocated.
crb_interval get_ssb_crbs(subcarrier_spacing    ssb_scs,
                          subcarrier_spacing    scs_common,
                          ssb_offset_to_pointA  offset_to_pA,
                          ssb_subcarrier_offset k_ssb);

} // namespace srsran
