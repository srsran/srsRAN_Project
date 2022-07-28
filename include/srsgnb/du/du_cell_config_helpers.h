/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_cell_config.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/channel_bandwidth.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {

/// \brief Main cell parameters from which other cell parameters (e.g. coreset0, BWP RBs) will be derived.
/// \remark Only fields that may affect many different fields in du_cell_config (e.g. number of PRBs) should be added
/// in this struct.
struct du_cell_config_default_params {
  pci_t              pci            = 1;
  subcarrier_spacing scs_common     = subcarrier_spacing::kHz15;
  bs_channel_bw_fr1  channel_bw_mhz = bs_channel_bw_fr1::MHz10;
  unsigned           nof_crbs       = band_helper::get_n_rbs_from_bw(channel_bw_mhz, scs_common, frequency_range::FR1);
  // TODO: we should decide whether this represents "f_ref" or pointA.
  unsigned dl_arfcn          = 365000;
  unsigned offset_to_point_a = 12;
  unsigned coreset0_index    = 6;
};

namespace du_config_helpers {

inline carrier_configuration make_default_carrier_configuration(const du_cell_config_default_params& params = {})
{
  carrier_configuration cfg;
  cfg.carrier_bw_mhz = bs_ch_bw_to_uint(params.channel_bw_mhz);
  cfg.arfcn          = params.dl_arfcn;
  cfg.nof_ant        = 1;
  return cfg;
}

inline tdd_ul_dl_config_common make_default_tdd_ul_dl_config_common(const du_cell_config_default_params& params = {})
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

inline coreset_configuration make_default_coreset_config(const du_cell_config_default_params& params = {})
{
  coreset_configuration cfg{};
  cfg.id = to_coreset_id(1);
  freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
  for (size_t i = 0; i < 6; ++i) {
    freq_resources.set(i);
  }
  cfg.set_freq_domain_resources(freq_resources);
  cfg.duration             = 1;
  cfg.precoder_granurality = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
  return cfg;
}

/// Generates a default CORESET#0 configuration. The default CORESET#0 table index value used is equal to 6.
/// \remark See TS 38.213, Table 13-1.
inline coreset_configuration make_default_coreset0_config(const du_cell_config_default_params& params = {})
{
  coreset_configuration cfg = make_default_coreset_config(params);
  cfg.id                    = to_coreset_id(0);
  // TODO: derive this from some table.
  uint8_t                             minimum_bw_MHz = 5;
  pdcch_type0_css_coreset_description desc =
      pdcch_type0_css_coreset_get(minimum_bw_MHz, params.scs_common, params.scs_common, params.coreset0_index, 0);

  cfg.duration = static_cast<unsigned>(desc.nof_symb_coreset);
  int rb_start = params.scs_common == subcarrier_spacing::kHz15
                     ? static_cast<int>(params.offset_to_point_a) - desc.offset
                     : static_cast<int>(params.offset_to_point_a / 2) - desc.offset;
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

inline search_space_configuration make_default_search_space_zero_config()
{
  search_space_configuration cfg{};
  cfg.id                     = to_search_space_id(0);
  cfg.cs_id                  = to_coreset_id(0);
  cfg.monitoring_slot_period = 1;
  cfg.monitoring_slot_offset = 0;
  cfg.duration               = 1;
  // TODO: Use TS38.213, Table 10.1-1.
  cfg.nof_candidates       = {0, 0, 1, 0, 0};
  cfg.type                 = search_space_configuration::type::common;
  cfg.common.f0_0_and_f1_0 = true;
  return cfg;
}

inline search_space_configuration make_default_common_search_space_config()
{
  search_space_configuration cfg = make_default_search_space_zero_config();
  cfg.id                         = to_search_space_id(1);
  cfg.nof_candidates             = {1, 1, 1, 0, 0};
  cfg.monitoring_symbols_within_slot.emplace();
  cfg.monitoring_symbols_within_slot->set(cfg.monitoring_symbols_within_slot->size() - 1, true);
  return cfg;
}

inline search_space_configuration make_default_ue_search_space_config()
{
  search_space_configuration cfg = make_default_common_search_space_config();
  cfg.cs_id                      = to_coreset_id(1);
  cfg.id                         = to_search_space_id(2);
  return cfg;
}

inline bwp_configuration make_default_init_bwp(const du_cell_config_default_params& params = {})
{
  bwp_configuration cfg{};
  cfg.scs         = params.scs_common;
  cfg.crbs        = {0, params.nof_crbs};
  cfg.cp_extended = false;
  return cfg;
}

inline dl_config_common make_default_dl_config_common(const du_cell_config_default_params& params = {})
{
  dl_config_common cfg{};

  // Configure FrequencyInfoDL.
  cfg.freq_info_dl.offset_to_point_a = params.offset_to_point_a;
  cfg.freq_info_dl.scs_carrier_list.emplace_back();
  cfg.freq_info_dl.scs_carrier_list.back().scs               = params.scs_common;
  cfg.freq_info_dl.scs_carrier_list.back().offset_to_carrier = 0;
  cfg.freq_info_dl.scs_carrier_list.back().carrier_bandwidth = params.nof_crbs;

  // Configure initial DL BWP.
  cfg.init_dl_bwp.generic_params = make_default_init_bwp(params);
  cfg.init_dl_bwp.pdcch_common.coreset0.emplace(make_default_coreset0_config(params));
  cfg.init_dl_bwp.pdcch_common.search_spaces.emplace(0, make_default_search_space_zero_config());
  cfg.init_dl_bwp.pdcch_common.search_spaces.emplace(1, make_default_common_search_space_config());
  cfg.init_dl_bwp.pdcch_common.sib1_search_space_id     = to_search_space_id(0);
  cfg.init_dl_bwp.pdcch_common.other_si_search_space_id = MAX_NOF_SEARCH_SPACES;
  cfg.init_dl_bwp.pdcch_common.paging_search_space_id   = to_search_space_id(1);
  cfg.init_dl_bwp.pdcch_common.ra_search_space_id       = to_search_space_id(1);
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.emplace_back();
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().k0       = 0;
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().map_type = pdsch_mapping_type::typeA;
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().symbols  = {2, 14};

  return cfg;
}

inline ul_config_common make_default_ul_config_common(const du_cell_config_default_params& params = {})
{
  ul_config_common cfg{};
  // TODO: This way of computing the UL-ARFCN is not entirely correct. We should derive ul_arfcn from f_ref for DL, and
  // then computing cfg.freq_info_ul.absolute_freq_point_a from f_ref_UL.
  uint32_t ul_arfcn                      = band_helper::get_ul_arfcn_from_dl_arfcn(params.dl_arfcn);
  cfg.freq_info_ul.absolute_freq_point_a = band_helper::get_abs_freq_point_a_arfcn(params.nof_crbs, ul_arfcn);
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
  cfg.init_ul_bwp.rach_cfg_common->restricted_set                                = restricted_set_config::TYPE_A;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index           = 16;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_fdm                     = 1;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start         = 0;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.zero_correlation_zone_config = 15;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window               = 10;
  cfg.init_ul_bwp.pusch_cfg_common.emplace();
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.resize(1);
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].k2 = 4;
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].map_type =
      pusch_time_domain_resource_allocation::mapping_type::typeA;
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols = {0, 14};
  return cfg;
}

inline ssb_configuration make_default_ssb_config(const du_cell_config_default_params& params = {})
{
  ssb_configuration cfg{};

  cfg.scs               = band_helper::get_lowest_ssb_scs(band_helper::get_band_from_dl_arfcn(params.dl_arfcn));
  cfg.offset_to_point_A = ssb_offset_to_pointA{params.offset_to_point_a};
  cfg.ssb_period        = ssb_periodicity::ms10;
  cfg.k_ssb             = 0;

  const unsigned beam_index = 63;
  cfg.ssb_bitmap            = uint64_t(1) << beam_index;
  cfg.beam_ids[beam_index]  = 0;

  /// The values we assign to these parameters are implementation-defined.
  cfg.ssb_block_power = -16;
  cfg.pss_to_sss_epre = ssb_pss_to_sss_epre::dB_0;

  return cfg;
}

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline du_cell_config make_default_du_cell_config(const du_cell_config_default_params& params = {})
{
  du_cell_config cfg{};
  cfg.pci     = params.pci;
  cfg.plmn    = "00101";
  cfg.tac     = 1;
  cfg.cell_id = 1;

  cfg.dl_carrier       = make_default_carrier_configuration(params);
  cfg.ul_carrier       = make_default_carrier_configuration(params);
  cfg.coreset0_idx     = params.coreset0_index;
  cfg.searchspace0_idx = 0U;
  cfg.dl_cfg_common    = make_default_dl_config_common(params);
  cfg.ul_cfg_common    = make_default_ul_config_common(params);
  cfg.scs_common       = params.scs_common;
  cfg.ssb_cfg          = make_default_ssb_config(params);
  cfg.dmrs_typeA_pos   = dmrs_typeA_position::pos2;
  cfg.cell_barred      = false;
  cfg.intra_freq_resel = false;
  return cfg;
}

} // namespace du_config_helpers

} // namespace srsgnb
