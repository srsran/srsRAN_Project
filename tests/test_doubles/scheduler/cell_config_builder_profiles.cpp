/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_config_builder_profiles.h"
#include "srsran/ran/band_helper.h"

using namespace srsran;

static void derive_coreset0(cell_config_builder_params& params)
{
  const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz, params.scs_common, band_helper::get_freq_range(*params.band));
  const uint8_t                                          ss0_idx = 0;
  std::optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
      band_helper::get_ssb_coreset0_freq_location(params.dl_f_ref_arfcn,
                                                  *params.band,
                                                  nof_crbs,
                                                  params.scs_common,
                                                  params.scs_common,
                                                  ss0_idx,
                                                  params.max_coreset0_duration);
  if (!ssb_freq_loc.has_value()) {
    report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", params.pci);
  }
  params.offset_to_point_a   = (*ssb_freq_loc).offset_to_point_A;
  params.k_ssb               = (*ssb_freq_loc).k_ssb;
  params.coreset0_index      = (*ssb_freq_loc).coreset0_idx;
  params.search_space0_index = ss0_idx;
}

cell_config_builder_params cell_config_builder_profiles::tdd(subcarrier_spacing scs)
{
  cell_config_builder_params params{};
  params.scs_common     = scs;
  params.dl_f_ref_arfcn = 520002;
  params.band           = band_helper::get_band_from_dl_arfcn(params.dl_f_ref_arfcn);
  params.channel_bw_mhz = bs_channel_bandwidth::MHz20;

  derive_coreset0(params);

  return params;
}

cell_config_builder_params cell_config_builder_profiles::fdd()
{
  cell_config_builder_params params{};
  params.scs_common     = subcarrier_spacing::kHz15;
  params.dl_f_ref_arfcn = 530000;
  params.band           = band_helper::get_band_from_dl_arfcn(params.dl_f_ref_arfcn);
  params.channel_bw_mhz = bs_channel_bandwidth::MHz20;

  derive_coreset0(params);

  return params;
}
