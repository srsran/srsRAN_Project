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

#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"

using namespace srsran;
using namespace srsran::config_helpers;

static nr_band get_band(const cell_config_builder_params& params)
{
  return params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
}

static unsigned cell_nof_crbs(const cell_config_builder_params& params)
{
  return band_helper::get_n_rbs_from_bw(params.channel_bw_mhz,
                                        params.scs_common,
                                        params.band.has_value() ? band_helper::get_freq_range(params.band.value())
                                                                : frequency_range::FR1);
}

static min_channel_bandwidth min_channel_bw(const cell_config_builder_params& params)
{
  return band_helper::get_min_channel_bw(get_band(params), params.scs_common);
}

static unsigned get_csi_freq_occupation_nof_rbs(const cell_config_builder_params& params)
{
  const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz,
      params.scs_common,
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);

  // See TS 38.331, CSI-FrequencyOccupation. Only multiples of 4 allowed for nrofRBs. And, the smallest configurable
  // number is the minimum of 24 and the width of the associated BWP.
  // NOTE: If the configured value is larger than the width of the corresponding BWP, the UE shall assume that the
  // actual CSI-RS bandwidth is equal to the width of the BWP.
  const unsigned nof_rbs_in_multiples_of_4 = 4 * divide_ceil(nof_crbs, 4);

  if (nof_rbs_in_multiples_of_4 > 24) {
    return nof_rbs_in_multiples_of_4;
  }

  return 24;
}

carrier_configuration
srsran::config_helpers::make_default_carrier_configuration(const cell_config_builder_params& params)
{
  carrier_configuration cfg;
  cfg.carrier_bw_mhz                         = bs_channel_bandwidth_to_MHz(params.channel_bw_mhz);
  cfg.arfcn                                  = params.dl_arfcn;
  cfg.nof_ant                                = 1;
  cfg.band                                   = get_band(params);
  const min_channel_bandwidth min_channel_bw = band_helper::get_min_channel_bw(cfg.band, params.scs_common);
  srsran_assert(cfg.carrier_bw_mhz >= min_channel_bandwidth_to_MHz(min_channel_bw),
                "Carrier BW {}Mhz must be greater than or equal to minimum channel BW {}Mhz",
                cfg.carrier_bw_mhz,
                min_channel_bandwidth_to_MHz(min_channel_bw));
  return cfg;
}

tdd_ul_dl_config_common
srsran::config_helpers::make_default_tdd_ul_dl_config_common(const cell_config_builder_params& params)
{
  tdd_ul_dl_config_common cfg{};
  cfg.ref_scs                            = params.scs_common;
  cfg.pattern1.dl_ul_tx_period_nof_slots = 10;
  cfg.pattern1.nof_dl_slots              = 6;
  cfg.pattern1.nof_dl_symbols            = 0;
  cfg.pattern1.nof_ul_slots              = 3;
  cfg.pattern1.nof_ul_symbols            = 0;
  return cfg;
}

coreset_configuration srsran::config_helpers::make_default_coreset_config(const cell_config_builder_params& params)
{
  coreset_configuration cfg{};
  cfg.id = to_coreset_id(1);
  // PRBs spanning the maximnum number of CRBs possible.
  freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
  unsigned             coreset_nof_resources = cell_nof_crbs(params) / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE;
  freq_resources.fill(0, coreset_nof_resources, true);
  cfg.set_freq_domain_resources(freq_resources);
  // Number of symbols equal to max(CORESET#0, 2).
  pdcch_type0_css_coreset_description desc = pdcch_type0_css_coreset_get(
      min_channel_bw(params), params.scs_common, params.scs_common, params.coreset0_index, 0);
  cfg.duration             = std::max(2U, static_cast<unsigned>(desc.nof_symb_coreset));
  cfg.precoder_granurality = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
  return cfg;
}

/// Generates a default CORESET#0 configuration. The default CORESET#0 table index value used is equal to 6.
/// \remark See TS 38.213, Table 13-1.
coreset_configuration srsran::config_helpers::make_default_coreset0_config(const cell_config_builder_params& params)
{
  coreset_configuration cfg{};
  cfg.id                                   = to_coreset_id(0);
  pdcch_type0_css_coreset_description desc = pdcch_type0_css_coreset_get(
      min_channel_bw(params), params.scs_common, params.scs_common, params.coreset0_index, 0);

  cfg.duration = static_cast<unsigned>(desc.nof_symb_coreset);
  int rb_start = params.scs_common == subcarrier_spacing::kHz15
                     ? static_cast<int>(params.offset_to_point_a.to_uint()) - desc.offset
                     : static_cast<int>(params.offset_to_point_a.to_uint() / 2) - desc.offset;
  if (rb_start < 0) {
    report_error("Coreset#0 CRB starts before pointA.");
  }
  cfg.set_coreset0_crbs({static_cast<unsigned>(rb_start), static_cast<unsigned>(rb_start) + desc.nof_rb_coreset});
  // Implicit CORESET#0 parameters as per TS38.211-7.3.2.2.
  cfg.interleaved.emplace();
  cfg.interleaved->interleaver_sz = 2;
  cfg.interleaved->reg_bundle_sz  = 6;
  cfg.interleaved->shift_index    = params.pci;
  cfg.precoder_granurality        = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;

  return cfg;
}

search_space_configuration srsran::config_helpers::make_default_search_space_zero_config()
{
  search_space_configuration cfg{};
  cfg.id                     = to_search_space_id(0);
  cfg.cs_id                  = to_coreset_id(0);
  cfg.monitoring_slot_period = 1;
  cfg.monitoring_slot_offset = 0;
  cfg.duration               = 1;
  // TODO: Use TS38.213, Table 10.1-1.
  cfg.nof_candidates       = {0, 0, 1, 0, 0};
  cfg.type                 = search_space_configuration::type_t::common;
  cfg.common.f0_0_and_f1_0 = true;
  return cfg;
}

search_space_configuration srsran::config_helpers::make_default_common_search_space_config()
{
  search_space_configuration cfg = make_default_search_space_zero_config();
  cfg.id                         = to_search_space_id(1);
  cfg.nof_candidates             = {0, 0, 1, 0, 0};
  cfg.monitoring_symbols_within_slot.emplace();
  cfg.monitoring_symbols_within_slot->set(cfg.monitoring_symbols_within_slot->size() - 1, true);
  return cfg;
}

search_space_configuration srsran::config_helpers::make_default_ue_search_space_config()
{
  search_space_configuration cfg = make_default_common_search_space_config();
  cfg.cs_id                      = to_coreset_id(1);
  cfg.id                         = to_search_space_id(2);
  cfg.type                       = search_space_configuration::type_t::ue_dedicated;
  cfg.ue_specific                = search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0;
  return cfg;
}

bwp_configuration srsran::config_helpers::make_default_init_bwp(const cell_config_builder_params& params)
{
  bwp_configuration cfg{};
  cfg.scs         = params.scs_common;
  cfg.crbs        = {0, cell_nof_crbs(params)};
  cfg.cp_extended = false;
  return cfg;
}

dl_config_common srsran::config_helpers::make_default_dl_config_common(const cell_config_builder_params& params)
{
  dl_config_common cfg{};

  // Configure FrequencyInfoDL.
  cfg.freq_info_dl.freq_band_list.emplace_back();
  cfg.freq_info_dl.freq_band_list.back().band =
      params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  cfg.freq_info_dl.offset_to_point_a = params.offset_to_point_a.to_uint();
  cfg.freq_info_dl.scs_carrier_list.emplace_back();
  cfg.freq_info_dl.scs_carrier_list.back().scs               = params.scs_common;
  cfg.freq_info_dl.scs_carrier_list.back().offset_to_carrier = 0;

  unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz,
      params.scs_common,
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);
  cfg.freq_info_dl.scs_carrier_list.back().carrier_bandwidth = nof_crbs;

  // Configure initial DL BWP.
  cfg.init_dl_bwp.generic_params = make_default_init_bwp(params);
  cfg.init_dl_bwp.pdcch_common.coreset0.emplace(make_default_coreset0_config(params));
  cfg.init_dl_bwp.pdcch_common.search_spaces.push_back(make_default_search_space_zero_config());
  cfg.init_dl_bwp.pdcch_common.search_spaces.push_back(make_default_common_search_space_config());
  cfg.init_dl_bwp.pdcch_common.sib1_search_space_id     = to_search_space_id(0);
  cfg.init_dl_bwp.pdcch_common.other_si_search_space_id = MAX_NOF_SEARCH_SPACES;
  cfg.init_dl_bwp.pdcch_common.paging_search_space_id   = to_search_space_id(1);
  cfg.init_dl_bwp.pdcch_common.ra_search_space_id       = to_search_space_id(1);
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.emplace_back();
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().k0       = 0;
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().map_type = sch_mapping_type::typeA;
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().symbols =
      ofdm_symbol_range{std::max(2U, cfg.init_dl_bwp.pdcch_common.coreset0->duration), 14};

  // Configure PCCH.
  cfg.pcch_cfg.default_paging_cycle = paging_cycle::rf128;
  cfg.pcch_cfg.nof_pf               = pcch_config::nof_pf_per_drx_cycle::oneT;
  cfg.pcch_cfg.paging_frame_offset  = 0;
  cfg.pcch_cfg.ns                   = pcch_config::nof_po_per_pf::one;

  return cfg;
}

ul_config_common srsran::config_helpers::make_default_ul_config_common(const cell_config_builder_params& params)
{
  ul_config_common cfg{};
  // This is the ARFCN of the UL f_ref, as per TS 38.104, Section 5.4.2.1.
  uint32_t ul_arfcn = band_helper::get_ul_arfcn_from_dl_arfcn(params.dl_arfcn);
  // This is f_ref frequency for UL, expressed in Hz and obtained from the corresponding ARFCN.

  unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz,
      params.scs_common,
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);

  double ul_f_ref = band_helper::get_abs_freq_point_a_from_f_ref(
      band_helper::nr_arfcn_to_freq(ul_arfcn), nof_crbs, params.scs_common);
  // absolute_freq_point_a needs to be expressed as in ARFCN, as per \c absoluteFrequencyPointA definition in 38.211,
  // Section 4.4.4.2.
  cfg.freq_info_ul.absolute_freq_point_a = band_helper::freq_to_nr_arfcn(ul_f_ref);
  cfg.freq_info_ul.scs_carrier_list.resize(1);
  cfg.freq_info_ul.scs_carrier_list[0].scs               = params.scs_common;
  cfg.freq_info_ul.scs_carrier_list[0].offset_to_carrier = 0;
  cfg.freq_info_ul.scs_carrier_list[0].carrier_bandwidth = nof_crbs;
  cfg.freq_info_ul.freq_band_list.emplace_back();
  cfg.freq_info_ul.freq_band_list.back().band =
      params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
  cfg.init_ul_bwp.generic_params = make_default_init_bwp(params);
  cfg.init_ul_bwp.rach_cfg_common.emplace();
  cfg.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles            = 64;
  cfg.init_ul_bwp.rach_cfg_common->prach_root_seq_index_l839_present = true;
  cfg.init_ul_bwp.rach_cfg_common->prach_root_seq_index              = 1;
  // Msg1-SCS invalid in case the PRACH SCS is derived from prach-ConfigurationIndex in RACH-ConfigGeneric.
  cfg.init_ul_bwp.rach_cfg_common->msg1_scs                                      = subcarrier_spacing::invalid;
  cfg.init_ul_bwp.rach_cfg_common->restricted_set                                = restricted_set_config::UNRESTRICTED;
  cfg.init_ul_bwp.rach_cfg_common->msg3_transform_precoder                       = false;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index           = 1;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_fdm                     = 1;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start         = 3;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.zero_correlation_zone_config = 15;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window = 10U << to_numerology_value(params.scs_common);
  cfg.init_ul_bwp.pusch_cfg_common.emplace();
  auto get_pusch_res = [](unsigned k2) {
    return pusch_time_domain_resource_allocation{.k2 = k2, .map_type = sch_mapping_type::typeA, .symbols = {0, 14}};
  };
  if (band_helper::get_duplex_mode(get_band(params)) == duplex_mode::FDD) {
    cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list = {get_pusch_res(4)};
  } else {
    // TDD
    cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list = {
        get_pusch_res(4), get_pusch_res(5), get_pusch_res(6), get_pusch_res(7)};
  }
  cfg.init_ul_bwp.pucch_cfg_common.emplace();
  cfg.init_ul_bwp.pucch_cfg_common->pucch_resource_common = 11;
  cfg.init_ul_bwp.pucch_cfg_common->group_hopping         = pucch_group_hopping::NEITHER;
  cfg.init_ul_bwp.pucch_cfg_common->p0_nominal            = -90;

  return cfg;
}

ssb_configuration srsran::config_helpers::make_default_ssb_config(const cell_config_builder_params& params)
{
  ssb_configuration cfg{};

  cfg.scs = band_helper::get_most_suitable_ssb_scs(
      params.band.has_value() ? *params.band : band_helper::get_band_from_dl_arfcn(params.dl_arfcn), params.scs_common);
  cfg.offset_to_point_A = ssb_offset_to_pointA{params.offset_to_point_a};
  cfg.ssb_period        = ssb_periodicity::ms10;
  cfg.k_ssb             = params.k_ssb;

  const unsigned beam_index = 63;
  cfg.ssb_bitmap            = uint64_t(1) << beam_index;
  cfg.beam_ids[beam_index]  = 0;

  // The values we assign to these parameters are implementation-defined.
  cfg.ssb_block_power = -16;
  cfg.pss_to_sss_epre = ssb_pss_to_sss_epre::dB_0;

  return cfg;
}

pusch_config srsran::config_helpers::make_default_pusch_config()
{
  pusch_config cfg{};
  cfg.tx_cfg = pusch_config::tx_config::not_set;
  cfg.pusch_mapping_type_a_dmrs.emplace();
  cfg.pusch_mapping_type_a_dmrs.value().additional_positions = dmrs_additional_positions::pos1;

  cfg.pusch_pwr_ctrl = pusch_config::pusch_power_control{.msg3_alpha               = alpha::alpha1,
                                                         .p0_nominal_without_grant = -76,
                                                         .p0_alphasets             = {},
                                                         .pathloss_ref_rs          = {},
                                                         .sri_pusch_mapping        = {}};
  cfg.pusch_pwr_ctrl.value().p0_alphasets.push_back(pusch_config::pusch_power_control::p0_pusch_alphaset{
      .id = static_cast<p0_pusch_alphaset_id>(0), .p0 = 0, .p0_pusch_alpha = alpha::alpha1});
  cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.push_back(pusch_config::pusch_power_control::pusch_pathloss_ref_rs{
      .id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .rs = static_cast<ssb_id_t>(0)});
  cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.push_back(pusch_config::pusch_power_control::sri_pusch_pwr_ctrl{
      .id                           = static_cast<pusch_config::pusch_power_control::sri_pusch_pwr_ctrl_id>(0),
      .sri_pusch_pathloss_ref_rs_id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .sri_p0_pusch_alphaset_id     = static_cast<p0_pusch_alphaset_id>(0),
      .closed_loop_idx = pusch_config::pusch_power_control::sri_pusch_pwr_ctrl::sri_pusch_closed_loop_index::i0});
  cfg.res_alloc      = pusch_config::resource_allocation::resource_allocation_type_1;
  cfg.trans_precoder = pusch_config::transform_precoder::disabled;
  cfg.cb_subset      = pusch_config::codebook_subset::non_coherent;
  cfg.max_rank       = 1;

  cfg.uci_cfg.emplace();
  auto& uci_cfg   = cfg.uci_cfg.value();
  uci_cfg.scaling = alpha_scaling_opt::f1;
  beta_offsets b_offset{};
  b_offset.beta_offset_ack_idx_1    = 9;
  b_offset.beta_offset_ack_idx_2    = 9;
  b_offset.beta_offset_ack_idx_3    = 9;
  b_offset.beta_offset_csi_p1_idx_1 = 9;
  b_offset.beta_offset_csi_p1_idx_2 = 9;
  b_offset.beta_offset_csi_p2_idx_1 = 9;
  b_offset.beta_offset_csi_p2_idx_2 = 9;
  uci_cfg.beta_offsets_cfg          = uci_on_pusch::beta_offsets_semi_static{b_offset};

  return cfg;
}

uplink_config srsran::config_helpers::make_default_ue_uplink_config(const cell_config_builder_params& params)
{
  // > UL Config.
  uplink_config ul_config{};
  ul_config.init_ul_bwp.pucch_cfg.emplace();

  // >> PUCCH.
  auto& pucch_cfg = ul_config.init_ul_bwp.pucch_cfg.value();
  // PUCCH Resource Set ID 0.
  auto& pucch_res_set_0            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_0.pucch_res_set_id = 0;
  pucch_res_set_0.pucch_res_id_list.emplace_back(0);
  pucch_res_set_0.pucch_res_id_list.emplace_back(1);
  pucch_res_set_0.pucch_res_id_list.emplace_back(2);

  auto& pucch_res_set_1            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_1.pucch_res_set_id = 1;
  pucch_res_set_1.pucch_res_id_list.emplace_back(3);
  pucch_res_set_1.pucch_res_id_list.emplace_back(4);
  pucch_res_set_1.pucch_res_id_list.emplace_back(5);
  pucch_res_set_1.pucch_res_id_list.emplace_back(6);
  pucch_res_set_1.pucch_res_id_list.emplace_back(7);
  pucch_res_set_1.pucch_res_id_list.emplace_back(8);

  unsigned nof_rbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz,
      params.scs_common,
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);

  // PUCCH resource format 1, for HARQ-ACK.
  // >>> PUCCH resource 0.
  pucch_resource res_basic{.res_id                 = 0,
                           .starting_prb           = nof_rbs - 1,
                           .second_hop_prb         = 0,
                           .intraslot_freq_hopping = false,
                           .format                 = pucch_format::FORMAT_1};
  res_basic.format_1.initial_cyclic_shift = 0;
  res_basic.format_1.nof_symbols          = 14;
  res_basic.format_1.starting_sym_idx     = 0;
  res_basic.format_1.time_domain_occ      = 0;
  pucch_cfg.pucch_res_list.push_back(res_basic);
  // >>> PUCCH resource 1.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res1 = pucch_cfg.pucch_res_list.back();
  res1.res_id          = 1;
  res1.starting_prb    = 1;
  res1.second_hop_prb  = nof_rbs - res1.starting_prb - 1;
  // >>> PUCCH resource 2.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res2 = pucch_cfg.pucch_res_list.back();
  res2.res_id          = 2;
  res2.second_hop_prb  = 1;
  res2.starting_prb    = nof_rbs - res2.second_hop_prb - 1;

  // PUCCH resource format 2, for HARQ-ACK and CSI.
  // >>> PUCCH resource 3.
  pucch_resource res_basic_f2{
      .starting_prb = 2, .second_hop_prb = 0, .intraslot_freq_hopping = false, .format = pucch_format::FORMAT_2};
  res_basic_f2.res_id                    = 3;
  res_basic_f2.format_2.nof_prbs         = 1;
  res_basic_f2.format_2.nof_symbols      = 2;
  res_basic_f2.format_2.starting_sym_idx = 0;
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  // >>> PUCCH resource 4.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res4           = pucch_cfg.pucch_res_list.back();
  res4.res_id                    = 4;
  res4.format_2.starting_sym_idx = 2;
  // >>> PUCCH resource 5.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res5           = pucch_cfg.pucch_res_list.back();
  res5.res_id                    = 5;
  res5.format_2.starting_sym_idx = 4;
  // >>> PUCCH resource 6.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res6           = pucch_cfg.pucch_res_list.back();
  res6.res_id                    = 6;
  res6.format_2.starting_sym_idx = 6;
  // >>> PUCCH resource 7.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res7           = pucch_cfg.pucch_res_list.back();
  res7.res_id                    = 7;
  res7.format_2.starting_sym_idx = 8;
  // >>> PUCCH resource 8.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res8           = pucch_cfg.pucch_res_list.back();
  res8.res_id                    = 8;
  res8.format_2.starting_sym_idx = 10;
  // >>> PUCCH resource 9.
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  pucch_resource& res9           = pucch_cfg.pucch_res_list.back();
  res9.res_id                    = 9;
  res9.format_2.starting_sym_idx = 12;

  // >>> PUCCH resource 10.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res10 = pucch_cfg.pucch_res_list.back();
  res10.res_id          = 10;
  res10.starting_prb    = 0;
  res10.second_hop_prb  = nof_rbs - 1;

  // TODO: add more PUCCH resources.

  // >>> SR Resource.
  const unsigned pucch_sr_res_id = pucch_cfg.pucch_res_list.size() - 1;
  pucch_cfg.sr_res_list.push_back(scheduling_request_resource_config{.sr_res_id    = 1,
                                                                     .sr_id        = uint_to_sched_req_id(0),
                                                                     .period       = sr_periodicity::sl_40,
                                                                     .offset       = 0,
                                                                     .pucch_res_id = pucch_sr_res_id});

  pucch_cfg.format_1_common_param.emplace();
  pucch_cfg.format_2_common_param.emplace(
      pucch_common_all_formats{.max_c_rate = max_pucch_code_rate::dot_25, .simultaneous_harq_ack_csi = true});

  // >>> dl-DataToUl-Ack
  // TS38.213, 9.1.2.1 - "If a UE is provided dl-DataToUL-ACK, the UE does not expect to be indicated by DCI format 1_0
  // a slot timing value for transmission of HARQ-ACK information that does not belong to the intersection of the set
  // of slot timing values {1, 2, 3, 4, 5, 6, 7, 8} and the set of slot timing values provided by dl-DataToUL-ACK for
  // the active DL BWP of a corresponding serving cell.
  // Inactive for format1_0."
  // Note2: Only k1 >= 4 supported.
  if (band_helper::get_duplex_mode(get_band(params)) == duplex_mode::FDD) {
    pucch_cfg.dl_data_to_ul_ack = {4};
  } else {
    // TDD
    pucch_cfg.dl_data_to_ul_ack = {4, 5, 6, 7};
  }

  // > PUSCH config.
  ul_config.init_ul_bwp.pusch_cfg.emplace(make_default_pusch_config());

  return ul_config;
}

pdsch_serving_cell_config srsran::config_helpers::make_default_pdsch_serving_cell_config()
{
  pdsch_serving_cell_config serv_cell;
  serv_cell.nof_harq_proc   = pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n8;
  serv_cell.max_mimo_layers = 1;

  return serv_cell;
}

nzp_csi_rs_resource_set srsran::config_helpers::make_default_nzp_csi_rs_resource_set()
{
  nzp_csi_rs_resource_set res_set{};
  res_set.res_set_id = static_cast<nzp_csi_rs_res_set_id_t>(0);
  res_set.nzp_csi_rs_res.push_back(static_cast<const nzp_csi_rs_res_id_t>(0));

  return res_set;
}

/// \brief Compute default CSI-RS signalling period to use, while constrained by TS38.214, 5.1.6.1.1.
static csi_resource_periodicity get_max_csi_rs_period(const cell_config_builder_params& params)
{
  csi_resource_periodicity max_csi_period =
      static_cast<csi_resource_periodicity>(std::min(80U * get_nof_slots_per_subframe(params.scs_common), 640U));
  return max_csi_period;
}

nzp_csi_rs_resource srsran::config_helpers::make_default_nzp_csi_rs_resource(const cell_config_builder_params& params)
{
  nzp_csi_rs_resource res;
  res.res_id = static_cast<nzp_csi_rs_res_id_t>(0);

  // Fill csi_rs_resource_mapping.
  res.res_mapping.fd_alloc                = {false, false, false, true};
  res.res_mapping.nof_ports               = 1;
  res.res_mapping.first_ofdm_symbol_in_td = 4;
  res.res_mapping.cdm                     = csi_rs_cdm_type::no_CDM;
  res.res_mapping.freq_density            = csi_rs_freq_density_type::three;
  res.res_mapping.freq_band_start_rb      = 0;
  res.res_mapping.freq_band_nof_rb        = get_csi_freq_occupation_nof_rbs(params);

  res.pwr_ctrl_offset       = 0;
  res.pwr_ctrl_offset_ss_db = 0;
  res.scrambling_id         = 1;

  res.csi_res_period = get_max_csi_rs_period(params);
  // Note: Avoid slots with SSB and SIB1.
  res.csi_res_offset = 12;

  res.qcl_info_periodic_csi_rs = static_cast<tci_state_id_t>(0);

  return res;
}

csi_im_resource_set srsran::config_helpers::make_default_csi_im_resource_set()
{
  csi_im_resource_set res_set;
  res_set.res_set_id        = static_cast<csi_im_res_set_id_t>(0);
  res_set.csi_ims_resources = {static_cast<csi_im_res_id_t>(0)};

  return res_set;
}

csi_im_resource srsran::config_helpers::make_default_csi_im_resource(const cell_config_builder_params& params)
{
  csi_im_resource res;
  res.res_id = static_cast<csi_im_res_id_t>(0);
  res.csi_im_res_element_pattern.emplace(csi_im_resource::csi_im_resource_element_pattern{
      .pattern_type        = csi_im_resource::csi_im_resource_element_pattern_type::pattern1,
      .subcarrier_location = 8,
      .symbol_location     = 8});
  res.freq_band_start_rb.emplace(0);
  res.freq_band_nof_rb.emplace(get_csi_freq_occupation_nof_rbs(params));
  res.csi_res_period.emplace(get_max_csi_rs_period(params));
  res.csi_res_offset.emplace(2);

  return res;
}

csi_resource_config srsran::config_helpers::make_default_csi_resource_config()
{
  csi_resource_config cfg;
  cfg.res_cfg_id = static_cast<csi_res_config_id_t>(0);
  csi_resource_config::nzp_csi_rs_ssb res_set{};
  res_set.nzp_csi_rs_res_set_list.push_back(static_cast<const nzp_csi_rs_res_set_id_t>(0));
  cfg.csi_rs_res_set_list = res_set;

  cfg.bwp_id   = to_bwp_id(0);
  cfg.res_type = csi_resource_config::resource_type::periodic;

  return cfg;
}

csi_report_config srsran::config_helpers::make_default_csi_report_config(const cell_config_builder_params& params)
{
  csi_report_config cfg;
  cfg.report_cfg_id               = static_cast<csi_report_config_id_t>(0);
  cfg.res_for_channel_meas        = static_cast<csi_res_config_id_t>(0);
  cfg.csi_im_res_for_interference = static_cast<csi_res_config_id_t>(1);

  csi_report_config::periodic_or_semi_persistent_report_on_pucch report_cfg_type{};
  report_cfg_type.report_type = csi_report_config::periodic_or_semi_persistent_report_on_pucch::report_type_t::periodic;
  csi_resource_periodicity csi_rs_period = get_max_csi_rs_period(params);
  // Set CSI report period equal to the CSI-RS period. No point in reporting more often than we get CSI-RS.
  report_cfg_type.report_slot_period = static_cast<csi_report_periodicity>(std::min((unsigned)csi_rs_period, 320U));
  report_cfg_type.report_slot_offset = 9;
  report_cfg_type.pucch_csi_res_list.push_back(
      csi_report_config::pucch_csi_resource{.ul_bwp = to_bwp_id(0), .pucch_res_id = 9});
  cfg.report_cfg_type = report_cfg_type;

  cfg.report_qty_type = csi_report_config::report_quantity_type_t::cri_ri_pmi_cqi;

  cfg.report_freq_cfg.emplace();
  cfg.report_freq_cfg.value().cqi_format_ind = csi_report_config::cqi_format_indicator::wideband_cqi;
  cfg.report_freq_cfg.value().pmi_format_ind = csi_report_config::pmi_format_indicator::wideband_pmi;

  cfg.codebook_cfg.emplace();
  auto ant_restriction = codebook_config::type1::single_panel::two_antenna_ports_two_tx_codebook_subset_restriction(6);
  // '111111'B.
  ant_restriction.from_uint64(0x3f);
  codebook_config::type1::single_panel sub_type{};
  sub_type.nof_antenna_ports = ant_restriction;
  // '03'H.
  sub_type.typei_single_panel_ri_restriction.resize(8);
  sub_type.typei_single_panel_ri_restriction.from_uint64(0x03);
  codebook_config::type1 type1{};
  type1.sub_type                         = sub_type;
  type1.codebook_mode                    = 1;
  cfg.codebook_cfg.value().codebook_type = type1;

  cfg.is_group_based_beam_reporting_enabled = false;
  cfg.cqi_table.emplace(csi_report_config::cqi_table_t::table1);
  cfg.subband_size = csi_report_config::subband_size_t::value1;

  return cfg;
}

csi_meas_config srsran::config_helpers::make_default_csi_meas_config(const cell_config_builder_params& params)
{
  csi_meas_config meas_cfg{};

  // NZP-CSI-RS-Resource.
  // Resource 0.
  meas_cfg.nzp_csi_rs_res_list.push_back(make_default_nzp_csi_rs_resource(params));
  meas_cfg.nzp_csi_rs_res_list.back().res_id = static_cast<nzp_csi_rs_res_id_t>(0);
  meas_cfg.nzp_csi_rs_res_list.back().res_mapping.fd_alloc.reset();
  meas_cfg.nzp_csi_rs_res_list.back().res_mapping.fd_alloc.resize(12);
  meas_cfg.nzp_csi_rs_res_list.back().res_mapping.fd_alloc.set(0, true);
  meas_cfg.nzp_csi_rs_res_list.back().res_mapping.freq_density = csi_rs_freq_density_type::one;
  meas_cfg.nzp_csi_rs_res_list.back().csi_res_offset           = 2;
  // Resource 1.
  meas_cfg.nzp_csi_rs_res_list.push_back(make_default_nzp_csi_rs_resource(params));
  meas_cfg.nzp_csi_rs_res_list.back().res_id = static_cast<nzp_csi_rs_res_id_t>(1);
  // Resource 2.
  meas_cfg.nzp_csi_rs_res_list.push_back(make_default_nzp_csi_rs_resource(params));
  meas_cfg.nzp_csi_rs_res_list.back().res_id                              = static_cast<nzp_csi_rs_res_id_t>(2);
  meas_cfg.nzp_csi_rs_res_list.back().res_mapping.first_ofdm_symbol_in_td = 8;
  // Resource 3.
  meas_cfg.nzp_csi_rs_res_list.push_back(make_default_nzp_csi_rs_resource(params));
  meas_cfg.nzp_csi_rs_res_list.back().res_id         = static_cast<nzp_csi_rs_res_id_t>(3);
  meas_cfg.nzp_csi_rs_res_list.back().csi_res_offset = 13;
  // Resource 4.
  meas_cfg.nzp_csi_rs_res_list.push_back(make_default_nzp_csi_rs_resource(params));
  meas_cfg.nzp_csi_rs_res_list.back().res_id                              = static_cast<nzp_csi_rs_res_id_t>(4);
  meas_cfg.nzp_csi_rs_res_list.back().res_mapping.first_ofdm_symbol_in_td = 8;
  meas_cfg.nzp_csi_rs_res_list.back().csi_res_offset                      = 13;

  // NZP-CSI-RS-ResourceSet.
  // Resource Set 0.
  meas_cfg.nzp_csi_rs_res_set_list.push_back(make_default_nzp_csi_rs_resource_set());
  // Resource Set 1.
  meas_cfg.nzp_csi_rs_res_set_list.push_back(make_default_nzp_csi_rs_resource_set());
  meas_cfg.nzp_csi_rs_res_set_list.back().res_set_id          = static_cast<nzp_csi_rs_res_set_id_t>(1);
  meas_cfg.nzp_csi_rs_res_set_list.back().nzp_csi_rs_res      = {static_cast<nzp_csi_rs_res_id_t>(1),
                                                                 static_cast<nzp_csi_rs_res_id_t>(2),
                                                                 static_cast<nzp_csi_rs_res_id_t>(3),
                                                                 static_cast<nzp_csi_rs_res_id_t>(4)};
  meas_cfg.nzp_csi_rs_res_set_list.back().is_trs_info_present = true;

  // csi-IM-Resource.
  meas_cfg.csi_im_res_list.push_back(make_default_csi_im_resource(params));
  meas_cfg.csi_im_res_list.back().res_id = static_cast<csi_im_res_id_t>(0);

  // csi-IM-ResourceSet.
  meas_cfg.csi_im_res_set_list.push_back(make_default_csi_im_resource_set());

  // CSI-ResourceConfig.
  // Resource 0.
  meas_cfg.csi_res_cfg_list.push_back(make_default_csi_resource_config());
  // Resource 1.
  meas_cfg.csi_res_cfg_list.push_back(make_default_csi_resource_config());
  meas_cfg.csi_res_cfg_list.back().res_cfg_id           = static_cast<csi_res_config_id_t>(1);
  csi_resource_config::csi_im_resource_set_list im_list = {static_cast<csi_im_res_set_id_t>(0)};
  meas_cfg.csi_res_cfg_list.back().csi_rs_res_set_list  = im_list;
  // Resource 2.
  meas_cfg.csi_res_cfg_list.push_back(make_default_csi_resource_config());
  meas_cfg.csi_res_cfg_list.back().res_cfg_id = static_cast<csi_res_config_id_t>(2);
  meas_cfg.csi_res_cfg_list.back().csi_rs_res_set_list =
      csi_resource_config::nzp_csi_rs_ssb{.nzp_csi_rs_res_set_list = {static_cast<nzp_csi_rs_res_set_id_t>(1)}};

  // CSI-ReportConfig.
  meas_cfg.csi_report_cfg_list.push_back(make_default_csi_report_config(params));

  return meas_cfg;
}

serving_cell_config
srsran::config_helpers::create_default_initial_ue_serving_cell_config(const cell_config_builder_params& params)
{
  serving_cell_config serv_cell;
  serv_cell.cell_index = to_du_cell_index(0);

  // > PDCCH-Config.
  serv_cell.init_dl_bwp.pdcch_cfg.emplace();
  pdcch_config& pdcch_cfg = serv_cell.init_dl_bwp.pdcch_cfg.value();
  // >> Add CORESET#1.
  pdcch_cfg.coresets.push_back(make_default_coreset_config(params));
  pdcch_cfg.coresets[0].id = to_coreset_id(1);
  // >> Add SearchSpace#2.
  pdcch_cfg.search_spaces.push_back(make_default_ue_search_space_config());
  pdcch_cfg.search_spaces[0].nof_candidates = {
      0, 0, std::min((uint8_t)4U, compute_max_nof_candidates(aggregation_level::n4, pdcch_cfg.coresets[0])), 0, 0};

  // > PDSCH-Config.
  serv_cell.init_dl_bwp.pdsch_cfg.emplace();
  pdsch_config& pdsch_cfg = serv_cell.init_dl_bwp.pdsch_cfg.value();
  pdsch_cfg.data_scrambling_id_pdsch.emplace(0);
  pdsch_cfg.pdsch_mapping_type_a_dmrs.emplace();
  dmrs_downlink_config& dmrs_type_a = pdsch_cfg.pdsch_mapping_type_a_dmrs.value();
  dmrs_type_a.additional_positions.emplace(dmrs_additional_positions::pos1);
  pdsch_cfg.tci_states.push_back(tci_state{
      .state_id  = static_cast<tci_state_id_t>(0),
      .qcl_type1 = {.ref_sig  = {.type = qcl_info::reference_signal::reference_signal_type::ssb,
                                 .ssb  = static_cast<ssb_id_t>(0)},
                    .qcl_type = qcl_info::qcl_type::type_d},
  });
  pdsch_cfg.res_alloc = pdsch_config::resource_allocation::resource_allocation_type_1;
  pdsch_cfg.rbg_sz    = rbg_size::config1;
  pdsch_cfg.prb_bndlg.bundling.emplace<prb_bundling::static_bundling>(
      prb_bundling::static_bundling({.sz = prb_bundling::static_bundling::bundling_size::wideband}));

  // > UL Config.
  serv_cell.ul_config.emplace(make_default_ue_uplink_config(params));

  // > pdsch-ServingCellConfig.
  serv_cell.pdsch_serv_cell_cfg.emplace(make_default_pdsch_serving_cell_config());

  // > CSI-MeasConfig.
  if (params.csi_rs_enabled) {
    serv_cell.csi_meas_cfg.emplace(make_default_csi_meas_config(params));
  }

  return serv_cell;
}

cell_config_dedicated
srsran::config_helpers::create_default_initial_ue_spcell_cell_config(const cell_config_builder_params& params)
{
  cell_config_dedicated cfg;
  cfg.serv_cell_idx = to_serv_cell_index(0);
  cfg.serv_cell_cfg = create_default_initial_ue_serving_cell_config(params);
  return cfg;
}

uint8_t srsran::config_helpers::compute_max_nof_candidates(aggregation_level            aggr_lvl,
                                                           const coreset_configuration& cs_cfg)
{
  // 1 REG = 1 RB and 1 symbol.
  // 1 CCE = 6 {PRB, symbol}. e.g. 3 PRBs over 2 symbols or 6 PRBs over 1 symbol, etc.
  // Example: 3 Frequency domain resources, 2 symbol duration contains 6 CCEs.
  const unsigned max_coreset_cces   = cs_cfg.freq_domain_resources().count() * cs_cfg.duration;
  unsigned       max_nof_candidates = max_coreset_cces / to_nof_cces(aggr_lvl);
  // See TS 38.331, SearchSpace IE.
  // aggregationLevelX - ENUMERATED {n0, n1, n2, n3, n4, n5, n6, n8}.
  if (max_nof_candidates == 7) {
    max_nof_candidates = 6;
  }
  return max_nof_candidates > PDCCH_MAX_NOF_CANDIDATES_SS ? PDCCH_MAX_NOF_CANDIDATES_SS : max_nof_candidates;
}
