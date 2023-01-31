/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/ran/duplex_mode.h"

using namespace srsgnb;
using namespace srsgnb::config_helpers;

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

carrier_configuration
srsgnb::config_helpers::make_default_carrier_configuration(const cell_config_builder_params& params)
{
  carrier_configuration cfg;
  cfg.carrier_bw_mhz = bs_channel_bandwidth_to_MHz(params.channel_bw_mhz);
  cfg.arfcn          = params.dl_arfcn;
  cfg.nof_ant        = 1;
  cfg.band           = get_band(params);
  return cfg;
}

tdd_ul_dl_config_common
srsgnb::config_helpers::make_default_tdd_ul_dl_config_common(const cell_config_builder_params& params)
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

coreset_configuration srsgnb::config_helpers::make_default_coreset_config(const cell_config_builder_params& params)
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
coreset_configuration srsgnb::config_helpers::make_default_coreset0_config(const cell_config_builder_params& params)
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
    srsgnb_terminate("Coreset#0 CRB starts before pointA.");
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

search_space_configuration srsgnb::config_helpers::make_default_search_space_zero_config()
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

search_space_configuration srsgnb::config_helpers::make_default_common_search_space_config()
{
  search_space_configuration cfg = make_default_search_space_zero_config();
  cfg.id                         = to_search_space_id(1);
  cfg.nof_candidates             = {0, 2, 1, 0, 0};
  cfg.monitoring_symbols_within_slot.emplace();
  cfg.monitoring_symbols_within_slot->set(cfg.monitoring_symbols_within_slot->size() - 1, true);
  return cfg;
}

search_space_configuration srsgnb::config_helpers::make_default_ue_search_space_config()
{
  search_space_configuration cfg = make_default_common_search_space_config();
  cfg.cs_id                      = to_coreset_id(1);
  cfg.id                         = to_search_space_id(2);
  cfg.type                       = search_space_configuration::type_t::ue_dedicated;
  cfg.ue_specific                = search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0;
  return cfg;
}

bwp_configuration srsgnb::config_helpers::make_default_init_bwp(const cell_config_builder_params& params)
{
  bwp_configuration cfg{};
  cfg.scs         = params.scs_common;
  cfg.crbs        = {0, cell_nof_crbs(params)};
  cfg.cp_extended = false;
  return cfg;
}

dl_config_common srsgnb::config_helpers::make_default_dl_config_common(const cell_config_builder_params& params)
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
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().symbols  = {
       std::max(2U, cfg.init_dl_bwp.pdcch_common.coreset0->duration), 14};

  // Configure PCCH.
  cfg.pcch_cfg.default_paging_cycle = paging_cycle::rf128;
  cfg.pcch_cfg.nof_pf               = pcch_config::nof_pf_per_drx_cycle::oneT;
  cfg.pcch_cfg.paging_frame_offset  = 0;
  cfg.pcch_cfg.ns                   = pcch_config::nof_po_per_pf::one;

  return cfg;
}

ul_config_common srsgnb::config_helpers::make_default_ul_config_common(const cell_config_builder_params& params)
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
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index           = 16;
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

ssb_configuration srsgnb::config_helpers::make_default_ssb_config(const cell_config_builder_params& params)
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

pusch_config srsgnb::config_helpers::make_default_pusch_config()
{
  pusch_config cfg{};
  cfg.tx_cfg = pusch_config::tx_config::codebook;
  cfg.pusch_mapping_type_b_dmrs.emplace();
  cfg.pusch_mapping_type_b_dmrs.value().trans_precoder_disabled.emplace();

  cfg.pusch_pwr_ctrl = pusch_config::pusch_power_control{.msg3_alpha               = alpha::alpha1,
                                                         .p0_nominal_without_grant = -76,
                                                         .p0_alphasets             = {},
                                                         .pathloss_ref_rs          = {},
                                                         .sri_pusch_mapping        = {}};
  cfg.pusch_pwr_ctrl.value().p0_alphasets.push_back(pusch_config::pusch_power_control::p0_pusch_alphaset{
      .id             = static_cast<pusch_config::pusch_power_control::p0_pusch_alphaset_id>(0),
      .p0             = 0,
      .p0_pusch_alpha = alpha::alpha1});
  cfg.pusch_pwr_ctrl.value().pathloss_ref_rs.push_back(pusch_config::pusch_power_control::pusch_pathloss_ref_rs{
      .id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .rs = static_cast<ssb_id_t>(0)});
  cfg.pusch_pwr_ctrl.value().sri_pusch_mapping.push_back(pusch_config::pusch_power_control::sri_pusch_pwr_ctrl{
      .id                           = static_cast<pusch_config::pusch_power_control::sri_pusch_pwr_ctrl_id>(0),
      .sri_pusch_pathloss_ref_rs_id = static_cast<pusch_config::pusch_power_control::pusch_pathloss_ref_rs_id>(0),
      .sri_p0_pusch_alphaset_id     = static_cast<pusch_config::pusch_power_control::p0_pusch_alphaset_id>(0),
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
  b_offset.beta_offset_csi_p1_idx_1 = 6;
  b_offset.beta_offset_csi_p1_idx_2 = 6;
  b_offset.beta_offset_csi_p2_idx_1 = 6;
  b_offset.beta_offset_csi_p2_idx_2 = 6;
  uci_cfg.beta_offsets_cfg          = uci_on_pusch::beta_offsets_semi_static{b_offset};

  return cfg;
}

uplink_config srsgnb::config_helpers::make_default_ue_uplink_config(const cell_config_builder_params& params)
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
  pucch_res_set_1.pucch_res_id_list.emplace_back(9);

  unsigned nof_rbs = band_helper::get_n_rbs_from_bw(
      params.channel_bw_mhz,
      params.scs_common,
      params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);

  // PUCCH resource format 1, for HARQ-ACK.
  // >>> PUCCH resource 0.
  pucch_resource res_basic{.res_id                 = 0,
                           .starting_prb           = nof_rbs - 1,
                           .second_hop_prb         = 0,
                           .intraslot_freq_hopping = true,
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
  pucch_cfg.format_2_common_param.emplace(pucch_common_all_formats{.max_c_rate = max_pucch_code_rate::dot_25});

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
    pucch_cfg.dl_data_to_ul_ack = {4, 5, 6, 7, 8};
  }

  // > PUSCH config.
  ul_config.init_ul_bwp.pusch_cfg.emplace(make_default_pusch_config());

  return ul_config;
}

pdsch_serving_cell_config srsgnb::config_helpers::make_default_pdsch_serving_cell_config()
{
  pdsch_serving_cell_config serv_cell;
  serv_cell.nof_harq_proc   = pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n16;
  serv_cell.max_mimo_layers = 2;

  return serv_cell;
}

serving_cell_config
srsgnb::config_helpers::create_default_initial_ue_serving_cell_config(const cell_config_builder_params& params)
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
      compute_max_nof_candidates(aggregation_level::n1, pdcch_cfg.coresets[0]),
      compute_max_nof_candidates(aggregation_level::n2, pdcch_cfg.coresets[0]),
      compute_max_nof_candidates(aggregation_level::n4, pdcch_cfg.coresets[0]),
      compute_max_nof_candidates(aggregation_level::n8, pdcch_cfg.coresets[0]),
      compute_max_nof_candidates(aggregation_level::n16, pdcch_cfg.coresets[0]),
  };

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

  return serv_cell;
}

cell_config_dedicated
srsgnb::config_helpers::create_default_initial_ue_spcell_cell_config(const cell_config_builder_params& params)
{
  cell_config_dedicated cfg;
  cfg.serv_cell_idx = to_serv_cell_index(0);
  cfg.serv_cell_cfg = create_default_initial_ue_serving_cell_config(params);
  return cfg;
}

uint8_t srsgnb::config_helpers::compute_max_nof_candidates(aggregation_level            aggr_lvl,
                                                           const coreset_configuration& cs_cfg)
{
  // 1 REG = 1 RB and 1 symbol.
  // 1 CCE = 6 {PRB, symbol}. e.g. 3 PRBs over 2 symbols or 6 PRBs over 1 symbol, etc.
  // Example: 3 Frequency domain resources, 2 symbol duration contains 6 CCEs.
  const unsigned max_coreset_cces = cs_cfg.freq_domain_resources().count() * cs_cfg.duration;
  return max_coreset_cces / to_nof_cces(aggr_lvl);
}
