/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_type0_helpers.h"
#include "srsran/ran/band_helper.h"

using namespace srsran;

slot_point srsran::precompute_type0_pdcch_css_n0(uint8_t                   searchspace0,
                                                 uint8_t                   coreset0,
                                                 const cell_configuration& cell_cfg,
                                                 subcarrier_spacing        scs_common,
                                                 unsigned                  ssb_index)
{
  const pdcch_type0_css_coreset_description coreset0_param =
      pdcch_type0_css_coreset_get(cell_cfg.dl_carrier.band,
                                  cell_cfg.ssb_cfg.scs,
                                  scs_common,
                                  coreset0,
                                  static_cast<uint8_t>(cell_cfg.ssb_cfg.k_ssb.to_uint()));

  srsran_assert(coreset0_param.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1,
                "SS/PBCH and CORESET multiplexing pattern not supported.");

  // Get Coreset0 num of symbols from Coreset0 config.
  const unsigned nof_symb_coreset0 = coreset0_param.nof_rb_coreset;

  srsran_assert(band_helper::get_freq_range(cell_cfg.dl_carrier.band) == frequency_range::FR1,
                "Only bands in FR1 supported.");

  const pdcch_type0_css_occasion_pattern1_description ss0_config_occasion_param =
      pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
          .is_fr2 = false, .ss_zero_index = searchspace0, .nof_symb_coreset = nof_symb_coreset0});

  const auto pdcch_slot = get_type0_pdcch_css_n0(
      static_cast<unsigned>(ss0_config_occasion_param.offset), ss0_config_occasion_param.M, scs_common, ssb_index);

  report_fatal_error_if_not(cell_cfg.is_dl_enabled(pdcch_slot), "PDCCH slot is not DL enabled.");

  return pdcch_slot;
}

slot_point srsran::precompute_type0_pdcch_css_n0_plus_1(uint8_t                   searchspace0,
                                                        uint8_t                   coreset0,
                                                        const cell_configuration& cell_cfg,
                                                        subcarrier_spacing        scs_common,
                                                        unsigned                  ssb_index)
{
  const auto pdcch_slot = precompute_type0_pdcch_css_n0(searchspace0, coreset0, cell_cfg, scs_common, ssb_index) + 1;
  report_fatal_error_if_not(cell_cfg.is_dl_enabled(pdcch_slot), "PDCCH slot is not DL enabled.");
  return pdcch_slot;
}
