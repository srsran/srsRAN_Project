/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_type0_helpers.h"

using namespace srsgnb;

unsigned srsgnb::type0_pdcch_css_n0_is_even_frame(double   table_13_11_and_13_12_O,
                                                  double   table_13_11_and_13_12_M,
                                                  uint8_t  numerology_mu,
                                                  unsigned ssb_index)
{
  // This is only used to retrieve the nof_slots_per_frame.
  slot_point sl_point{numerology_mu, 0};

  // Compute floor( ( O * 2^mu + floor(i*M) ) / nof_slots_per_frame  ) mod 2, as per TS 38.213, Section 13.
  unsigned is_even = static_cast<unsigned>(floor(static_cast<double>(table_13_11_and_13_12_O * (1U << numerology_mu)) +
                                                 floor(static_cast<double>(ssb_index) * table_13_11_and_13_12_M)) /
                                           sl_point.nof_slots_per_frame()) %
                     2;
  return is_even;
}

slot_point srsgnb::get_type0_pdcch_css_n0(double             table_13_11_and_13_12_O,
                                          double             table_13_11_and_13_12_M,
                                          subcarrier_spacing scs_common,
                                          unsigned           ssb_index)
{
  // Initialize n0 to a slot_point = 0.
  const uint8_t numerology_mu = to_numerology_value(scs_common);
  slot_point    type0_pdcch_css_n0{numerology_mu, 0};

  // Compute n0 = ( O * 2^mu + floor(i*M)  )  % nof_slots_per_frame, as per TS 38.213, Section 13.
  type0_pdcch_css_n0 += static_cast<unsigned>(static_cast<double>(table_13_11_and_13_12_O * (1U << numerology_mu)) +
                                              floor(static_cast<double>(ssb_index) * table_13_11_and_13_12_M)) %
                        type0_pdcch_css_n0.nof_slots_per_frame();

  // We want to express n0 as a value from 0 to max_nof_slots. Since the mod operation above cap n0 to
  // (nof_slots_per_frame - 1), we need to add nof_slots_per_frame to n0 if this falls into an odd frame.
  type0_pdcch_css_n0 +=
      type0_pdcch_css_n0_is_even_frame(table_13_11_and_13_12_O, table_13_11_and_13_12_M, numerology_mu, ssb_index) *
      type0_pdcch_css_n0.nof_slots_per_frame();

  return type0_pdcch_css_n0;
}

slot_point srsgnb::precompute_type0_pdcch_css_n0(uint8_t                   searchspace0,
                                                 uint8_t                   coreset0,
                                                 const cell_configuration& cell_cfg,
                                                 subcarrier_spacing        scs_common,
                                                 unsigned                  ssb_index)
{
  const min_channel_bandwidth min_channel_bw = band_helper::get_min_channel_bw(cell_cfg.dl_carrier.band, scs_common);
  srsgnb_assert(min_channel_bw < min_channel_bandwidth::MHz40, "Bands with minimum channel BW 40MHz not supported.");

  pdcch_type0_css_coreset_description coreset0_param =
      pdcch_type0_css_coreset_get(min_channel_bw,
                                  cell_cfg.ssb_cfg.scs,
                                  scs_common,
                                  coreset0,
                                  static_cast<uint8_t>(cell_cfg.ssb_cfg.k_ssb.to_uint()));

  srsgnb_assert(coreset0_param.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1,
                "SS/PBCH and CORESET multiplexing pattern not supported.");

  // Get Coreset0 num of symbols from Coreset0 config.
  const unsigned nof_symb_coreset0 = coreset0_param.nof_rb_coreset;

  srsgnb_assert(band_helper::get_freq_range(cell_cfg.dl_carrier.band) == frequency_range::FR1,
                "Only bands in FR1 supported.");

  const pdcch_type0_css_occasion_pattern1_description ss0_config_occasion_param =
      pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
          .is_fr2 = false, .ss_zero_index = searchspace0, .nof_symb_coreset = nof_symb_coreset0});

  const auto pdcch_slot = get_type0_pdcch_css_n0(
      static_cast<unsigned>(ss0_config_occasion_param.offset), ss0_config_occasion_param.M, scs_common, ssb_index);

  srsgnb_assert(cell_cfg.is_dl_enabled(pdcch_slot), "PDCCH slot is not DL enabled.");

  return pdcch_slot;
}

slot_point srsgnb::precompute_type0_pdcch_css_n0_plus_1(uint8_t                   searchspace0,
                                                        uint8_t                   coreset0,
                                                        const cell_configuration& cell_cfg,
                                                        subcarrier_spacing        scs_common,
                                                        unsigned                  ssb_index)
{
  const auto pdcch_slot = precompute_type0_pdcch_css_n0(searchspace0, coreset0, cell_cfg, scs_common, ssb_index) + 1;
  srsgnb_assert(cell_cfg.is_dl_enabled(pdcch_slot), "PDCCH slot is not DL enabled.");
  return pdcch_slot;
}
