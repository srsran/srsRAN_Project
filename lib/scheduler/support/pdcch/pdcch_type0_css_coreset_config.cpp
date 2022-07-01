/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_type0_css_coreset_config.h"
#include "srsgnb/adt/span.h"

using namespace srsgnb;

pdcch_type0_css_coreset_description srsgnb::pdcch_type0_css_coreset_get(uint8_t            band_minimum_bandwidth_MHz,
                                                                        subcarrier_spacing ssb_scs,
                                                                        subcarrier_spacing pdcch_scs,
                                                                        uint8_t            coreset_zero_index)
{
  // TS38.213 Table 13-1. {SS/PBCH block, PDCCH} SCS is {15, 15} kHz and minimum channel bandwidth 5 MHz or 10 MHz.
  static const std::array<pdcch_type0_css_coreset_description, 15> TABLE_13_1 = {{
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 2},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 2},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 12},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 16},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 12},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 16},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 12},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 16},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 1, 38},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 2, 38},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 3, 38},
  }};
  // TS38.213 Table 13-2. {SS/PBCH block, PDCCH} SCS is {15, 30} kHz and minimum channel bandwidth 5 MHz or 10 MHz.
  static const std::array<pdcch_type0_css_coreset_description, 14> TABLE_13_2 = {{
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 5},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 6},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 7},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 8},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 5},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 6},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 7},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 8},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 18},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 20},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 18},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 20},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 18},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 20},
  }};
  // TS38.213 Table 13-3. {SS/PBCH block, PDCCH} SCS is {30, 15} kHz and minimum channel bandwidth 5 MHz or 10 MHz.
  static const std::array<pdcch_type0_css_coreset_description, 9> TABLE_13_3 = {{
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 2},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 6},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 2},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 6},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 2},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 6},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 1, 28},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 2, 28},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 3, 28},
  }};
  // TS38.213 Table 13-4. {SS/PBCH block, PDCCH} SCS is {30, 30} kHz and minimum channel bandwidth 5 MHz or 10 MHz.
  static const std::array<pdcch_type0_css_coreset_description, 16> TABLE_13_4 = {{
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 1},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 2},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 3},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 1},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 2},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 3},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 12},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 14},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 16},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 12},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 14},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 16},
  }};
  // TS38.213 Table 13-5. {SS/PBCH block, PDCCH} SCS is {30, 15} kHz and minimum channel bandwidth 40 MHz.
  static const std::array<pdcch_type0_css_coreset_description, 9> TABLE_13_5 = {{
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 1, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 1, 56},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 2, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 2, 56},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 3, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 96, 3, 56},
  }};
  // TS38.213 Table 13-6. {SS/PBCH block, PDCCH} SCS is {30, 15} kHz and minimum channel bandwidth 40 MHz.
  static const std::array<pdcch_type0_css_coreset_description, 10> TABLE_13_6 = {{
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 2, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 24, 3, 4},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 1, 28},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 2, 28},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 0},
      {ssb_coreset0_mplex_pattern::mplx_pattern1, 48, 3, 28},
  }};

  span<const pdcch_type0_css_coreset_description> table = {};
  if (ssb_scs == subcarrier_spacing::kHz15 && pdcch_scs == subcarrier_spacing::kHz15 &&
      (band_minimum_bandwidth_MHz == 5 || band_minimum_bandwidth_MHz == 10)) {
    table = TABLE_13_1;
  } else if (ssb_scs == subcarrier_spacing::kHz15 && pdcch_scs == subcarrier_spacing::kHz30 &&
             (band_minimum_bandwidth_MHz == 5 || band_minimum_bandwidth_MHz == 10)) {
    table = TABLE_13_2;
  } else if (ssb_scs == subcarrier_spacing::kHz30 && pdcch_scs == subcarrier_spacing::kHz15 &&
             (band_minimum_bandwidth_MHz == 5 || band_minimum_bandwidth_MHz == 10)) {
    table = TABLE_13_3;
  } else if (ssb_scs == subcarrier_spacing::kHz30 && pdcch_scs == subcarrier_spacing::kHz30 &&
             (band_minimum_bandwidth_MHz == 5 || band_minimum_bandwidth_MHz == 10)) {
    table = TABLE_13_4;
  } else if (ssb_scs == subcarrier_spacing::kHz30 && pdcch_scs == subcarrier_spacing::kHz15 &&
             band_minimum_bandwidth_MHz == 40) {
    table = TABLE_13_5;
  } else if (ssb_scs == subcarrier_spacing::kHz30 && pdcch_scs == subcarrier_spacing::kHz30 &&
             band_minimum_bandwidth_MHz == 40) {
    table = TABLE_13_6;
  }

  if (coreset_zero_index < table.size()) {
    return table[coreset_zero_index];
  }

  return PDCCH_TYPE0_CSS_CORESET_RESERVED;
}