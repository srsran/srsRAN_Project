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

#include "srsran/ran/ssb_mapping.h"
#include "srsran/ran/band_helper.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

ssb_pattern_case srsran::ssb_get_ssb_pattern(subcarrier_spacing ssb_scs, unsigned dl_arfcn)
{
  nr_band dl_idx_nr_band = band_helper::get_band_from_dl_arfcn(dl_arfcn);
  srsran_assert(dl_idx_nr_band != nr_band::invalid, "Invalid NR band index");
  return band_helper::get_ssb_pattern(dl_idx_nr_band, ssb_scs);
}

uint8_t srsran::ssb_get_L_max(subcarrier_spacing ssb_scs, unsigned dl_arfcn, optional<nr_band> band)
{
  uint8_t L_max = 0;

  // Derive the SSB-specific parameters (SSB pattern case, SSB L_max and SSB paired_spectrum flag) from those in the
  // MAC Cell config.
  if (not band.has_value()) {
    band.emplace(band_helper::get_band_from_dl_arfcn(dl_arfcn));
    srsran_assert(band.value() != nr_band::invalid, "Invalid NR band index");
  }
  ssb_pattern_case ssb_case = band_helper::get_ssb_pattern(band.value(), ssb_scs);
  // Flag indicating whether cell is on paired spectrum (FDD) or unpaired (TDD, SDL, SUL).
  bool paired_spectrum = band_helper::is_paired_spectrum(band.value());

  // Get L_max from SSB pattern case and carrier frequency and paired spectrum flag.
  uint32_t f_arfcn = dl_arfcn;
  if (ssb_case == ssb_pattern_case::A or ssb_case == ssb_pattern_case::B) {
    // See TS 38.311, Section 4.1, "Case A" and "Case B".
    L_max = f_arfcn < CUTOFF_FREQ_ARFCN_CASE_A_B_C ? 4 : 8;
  } else if (ssb_case == ssb_pattern_case::C) {
    // See TS 38.311, Section 4.1, "Case C".
    uint32_t ssb_cutoff_freq = paired_spectrum ? CUTOFF_FREQ_ARFCN_CASE_A_B_C : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
    L_max                    = f_arfcn < ssb_cutoff_freq ? 4 : 8;
  } else {
    srsran_assert(ssb_case < ssb_pattern_case::invalid, "Invalid SSB case");
  }

  return L_max;
}

crb_interval srsran::get_ssb_crbs(subcarrier_spacing    ssb_scs,
                                  subcarrier_spacing    scs_common,
                                  ssb_offset_to_pointA  offset_to_pA,
                                  ssb_subcarrier_offset k_ssb)
{
  report_fatal_error_if_not(ssb_scs == scs_common, "Mixed numerology not supported");
  report_fatal_error_if_not(scs_common <= subcarrier_spacing::kHz30, "Only FR1 frequency supported");

  // With SCScommon kHz30, offset_to_pointA must be a multiple of 2. This is because it is measured in 15kHz RB, while
  // it points at a CRB which is based on 30kHz.
  if (scs_common == subcarrier_spacing::kHz30) {
    srsran_sanity_check(offset_to_pA.to_uint() % 2 == 0,
                        "With SCScommon with 30kHz OffsetToPointA must be an even number");
  }
  unsigned ssb_crbs_start =
      scs_common == subcarrier_spacing::kHz15 ? offset_to_pA.to_uint() : offset_to_pA.to_uint() / 2;
  // TODO: Extent this function to cover mixed numerologies. Depending on the reference grid, NOF_SSB_PRBS might be
  //       multiplied or divided by 2.

  // As per TS 38.211, Section 7.4.3.1, the SSB occupies 240 subcarriers, or 20 PRBs. In the case of FR1, and SSB SCS ==
  // SCScommon, if k_SSB > 1, the SSB PRBs will be shifted with respect to the Common RBs grid; this means that the SSB
  // will overlap over 1 additional CRB (at the end of the SS/PBCH Block).
  unsigned ssb_crbs_stop = k_ssb.to_uint() > 0 ? ssb_crbs_start + NOF_SSB_PRBS + 1 : ssb_crbs_start + NOF_SSB_PRBS;

  return crb_interval{ssb_crbs_start, ssb_crbs_stop};
}
