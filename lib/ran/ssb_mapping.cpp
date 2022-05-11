/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/scheduler/sched_consts.h"

using namespace srsgnb;

ssb_pattern_case srsgnb::ssb_get_ssb_pattern(subcarrier_spacing ssb_scs, unsigned dl_arfcn)
{
  uint16_t dl_idx_nr_band = band_helper::get_band_from_dl_arfcn(dl_arfcn);
  srsran_assert(dl_idx_nr_band < UINT16_MAX, "Invalid NR band index");
  return band_helper::get_ssb_pattern(dl_idx_nr_band, ssb_scs);
}

uint8_t srsgnb::ssb_get_L_max(subcarrier_spacing ssb_scs, unsigned dl_arfcn)
{
  uint8_t L_max = 0;

  // Derive the SSB-specific parameters (SSB pattern case, SSB L_max and SSB paired_spectrum flag) from those in the
  // MAC Cell config.
  uint16_t dl_idx_nr_band = band_helper::get_band_from_dl_arfcn(dl_arfcn);
  srsran_assert(dl_idx_nr_band < UINT16_MAX, "Invalid NR band index");
  ssb_pattern_case ssb_case = band_helper::get_ssb_pattern(dl_idx_nr_band, ssb_scs);
  // Flag indicating whether cell is on paired spectrum (FDD) or unpaired (TDD, SDL, SUL).
  bool paired_spectrum = band_helper::is_paired_spectrum(dl_idx_nr_band);

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
