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

using namespace srsgnb;
using namespace srsgnb::config_helpers;

carrier_configuration
srsgnb::config_helpers::make_default_carrier_configuration(const du_cell_config_master_params& params)
{
  carrier_configuration cfg;
  cfg.carrier_bw_mhz = bs_channel_bandwidth_to_MHz(params.channel_bw_mhz);
  cfg.arfcn          = params.dl_arfcn;
  cfg.nof_ant        = 1;
  cfg.band           = params.band;
  return cfg;
}

tdd_ul_dl_config_common
srsgnb::config_helpers::make_default_tdd_ul_dl_config_common(const du_cell_config_master_params& params)
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

coreset_configuration srsgnb::config_helpers::make_default_coreset_config(const du_cell_config_master_params& params)
{
  coreset_configuration cfg{};
  cfg.id = to_coreset_id(1);
  freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
  const size_t         NOF_PRBS_CORESET1 = 6;
  freq_resources.fill(0, NOF_PRBS_CORESET1, true);
  cfg.set_freq_domain_resources(freq_resources);
  cfg.duration             = 1;
  cfg.precoder_granurality = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
  return cfg;
}

/// Generates a default CORESET#0 configuration. The default CORESET#0 table index value used is equal to 6.
/// \remark See TS 38.213, Table 13-1.
coreset_configuration srsgnb::config_helpers::make_default_coreset0_config(const du_cell_config_master_params& params)
{
  coreset_configuration cfg = make_default_coreset_config(params);
  cfg.id                    = to_coreset_id(0);
  // TODO: derive this from some table.
  min_channel_bandwidth               min_channel_bw = band_helper::get_min_channel_bw(params.band, params.scs_common);
  pdcch_type0_css_coreset_description desc =
      pdcch_type0_css_coreset_get(min_channel_bw, params.scs_common, params.scs_common, params.coreset0_index, 0);

  cfg.duration = static_cast<unsigned>(desc.nof_symb_coreset);
  int rb_start = params.scs_common == subcarrier_spacing::kHz15
                     ? static_cast<int>(params.offset_to_point_a.to_uint()) - desc.offset
                     : static_cast<int>(params.offset_to_point_a.to_uint() / 2) - desc.offset;
  if (rb_start < 0) {
    srsgnb_terminate("Coreset#0 CRB starts before point A.");
  }
  cfg.set_coreset0_crbs({static_cast<unsigned>(rb_start), static_cast<unsigned>(rb_start) + desc.nof_rb_coreset});
  // Implicit CORESET#0 parameters as per TS38.211-7.3.2.2.
  cfg.interleaved.emplace();
  cfg.interleaved->interleaver_sz = 2;
  cfg.interleaved->reg_bundle_sz  = 6;
  cfg.interleaved->shift_index    = params.pci;
  cfg.precoder_granurality        = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
  cfg.pdcch_dmrs_scrambling_id    = params.pci;

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
  return cfg;
}

bwp_configuration srsgnb::config_helpers::make_default_init_bwp(const du_cell_config_master_params& params)
{
  bwp_configuration cfg{};
  cfg.scs         = params.scs_common;
  cfg.crbs        = {0, params.nof_crbs};
  cfg.cp_extended = false;
  return cfg;
}

dl_config_common srsgnb::config_helpers::make_default_dl_config_common(const du_cell_config_master_params& params)
{
  dl_config_common cfg{};

  // Configure FrequencyInfoDL.
  cfg.freq_info_dl.offset_to_point_a = params.offset_to_point_a.to_uint();
  cfg.freq_info_dl.scs_carrier_list.emplace_back();
  cfg.freq_info_dl.scs_carrier_list.back().scs               = params.scs_common;
  cfg.freq_info_dl.scs_carrier_list.back().offset_to_carrier = 0;
  cfg.freq_info_dl.scs_carrier_list.back().carrier_bandwidth = params.nof_crbs;

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
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().symbols  = {2, 14};

  return cfg;
}

ul_config_common srsgnb::config_helpers::make_default_ul_config_common(const du_cell_config_master_params& params)
{
  ul_config_common cfg{};
  // This is the ARFCN of the UL f_ref, as per TS 38.104, Section 5.4.2.1.
  uint32_t ul_arfcn = band_helper::get_ul_arfcn_from_dl_arfcn(params.dl_arfcn);
  // This is f_ref frequency for UL, expressed in Hz and obtained from the corresponding ARFCN.
  double ul_f_ref = band_helper::get_abs_freq_point_a_from_f_ref(
      band_helper::nr_arfcn_to_freq(ul_arfcn), params.nof_crbs, params.scs_common);
  // absolute_freq_point_a needs to be expressed as in ARFCN, as per \c absoluteFrequencyPointA definition in 38.211,
  // Section 4.4.4.2.
  cfg.freq_info_ul.absolute_freq_point_a = band_helper::freq_to_nr_arfcn(ul_f_ref);
  cfg.freq_info_ul.scs_carrier_list.resize(1);
  cfg.freq_info_ul.scs_carrier_list[0].scs               = params.scs_common;
  cfg.freq_info_ul.scs_carrier_list[0].offset_to_carrier = 0;
  cfg.freq_info_ul.scs_carrier_list[0].carrier_bandwidth = params.nof_crbs;
  cfg.init_ul_bwp.generic_params                         = make_default_init_bwp(params);
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
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start         = 0;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.zero_correlation_zone_config = 15;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window               = 10;
  cfg.init_ul_bwp.pusch_cfg_common.emplace();
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.resize(1);
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].k2       = 4;
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].map_type = sch_mapping_type::typeA;
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols  = {0, 14};
  // TODO: Set these parameters according to some reliable source.
  cfg.init_ul_bwp.pucch_cfg_common.emplace();
  cfg.init_ul_bwp.pucch_cfg_common->pucch_resource_common = 11;
  cfg.init_ul_bwp.pucch_cfg_common->group_hopping         = pucch_group_hopping::NEITHER;
  cfg.init_ul_bwp.pucch_cfg_common->p0_nominal            = -90;

  return cfg;
}

ssb_configuration srsgnb::config_helpers::make_default_ssb_config(const du_cell_config_master_params& params)
{
  ssb_configuration cfg{};

  cfg.scs               = band_helper::get_lowest_ssb_scs(band_helper::get_band_from_dl_arfcn(params.dl_arfcn));
  cfg.offset_to_point_A = ssb_offset_to_pointA{params.offset_to_point_a};
  cfg.ssb_period        = ssb_periodicity::ms10;
  cfg.k_ssb             = 0;

  const unsigned beam_index = 63;
  cfg.ssb_bitmap            = uint64_t(1) << beam_index;
  cfg.beam_ids[beam_index]  = 0;

  // The values we assign to these parameters are implementation-defined.
  cfg.ssb_block_power = -16;
  cfg.pss_to_sss_epre = ssb_pss_to_sss_epre::dB_0;

  return cfg;
}

uplink_config srsgnb::config_helpers::make_default_ue_uplink_config()
{
  // > UL Config.
  uplink_config ul_config{};
  ul_config.init_ul_bwp.pucch_cfg.emplace();
  auto& pucch_cfg = ul_config.init_ul_bwp.pucch_cfg.value();

  // >> PUCCH.
  pucch_cfg.pucch_res_set.emplace_back();
  pucch_cfg.pucch_res_set.back().pucch_res_set_id = 0;
  pucch_cfg.pucch_res_set.back().pucch_res_id_list.emplace_back(0);

  // >>> PUCCH resource 0.
  pucch_resource res_basic{.res_id                 = 0,
                           .starting_prb           = 51,
                           .second_hop_prb         = 0,
                           .intraslot_freq_hopping = true,
                           .format                 = pucch_format::FORMAT_1};
  res_basic.format                        = pucch_format::FORMAT_1;
  res_basic.format_1.initial_cyclic_shift = 0;
  res_basic.format_1.nof_symbols          = 14;
  res_basic.format_1.starting_sym_idx     = 0;
  res_basic.format_1.time_domain_occ      = 0;
  pucch_cfg.pucch_res_list.push_back(res_basic);
  // >>> PUCCH resource 1.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res1 = pucch_cfg.pucch_res_list.back();
  res1.res_id          = 1;
  res1.starting_prb    = 0;
  res1.second_hop_prb  = 51;

  // TODO: add more PUCCH resources.

  pucch_cfg.dl_data_to_ul_ack.push_back(4);

  // >>> SR Resource.
  pucch_cfg.sr_res_list.push_back(scheduling_request_resource_config{.sr_res_id    = 1,
                                                                     .sr_id        = uint_to_sched_req_id(0),
                                                                     .period       = sr_periodicity::sl_40,
                                                                     .offset       = 0,
                                                                     .pucch_res_id = 1});

  pucch_cfg.format_1_common_param.emplace();

  return ul_config;
}

pdsch_serving_cell_config srsgnb::config_helpers::make_default_pdsch_serving_cell_config()
{
  pdsch_serving_cell_config serv_cell;
  serv_cell.nof_harq_proc   = pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n16;
  serv_cell.max_mimo_layers = 2;

  return serv_cell;
}

serving_cell_config srsgnb::config_helpers::make_default_initial_ue_serving_cell_config()
{
  serving_cell_config serv_cell;

  // > PDCCH-Config.
  serv_cell.init_dl_bwp.pdcch_cfg.emplace();
  pdcch_config& pdcch_cfg = serv_cell.init_dl_bwp.pdcch_cfg.value();
  // >> Add CORESET#1.
  pdcch_cfg.coresets.push_back(make_default_coreset_config());
  pdcch_cfg.coresets[0].id = to_coreset_id(1);
  pdcch_cfg.coresets[0].pdcch_dmrs_scrambling_id.emplace();
  pdcch_cfg.coresets[0].pdcch_dmrs_scrambling_id.value() = 0;
  // >> Add SearchSpace#2.
  pdcch_cfg.search_spaces.push_back(make_default_ue_search_space_config());

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
  serv_cell.ul_config.emplace(make_default_ue_uplink_config());

  // > pdsch-ServingCellConfig.
  serv_cell.pdsch_serv_cell_cfg.emplace(make_default_pdsch_serving_cell_config());

  return serv_cell;
}
