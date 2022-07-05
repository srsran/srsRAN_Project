/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_CELL_CONFIG_HELPERS_H
#define SRSGNB_DU_CELL_CONFIG_HELPERS_H

#include "du_cell_config.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {

namespace du_config_helpers {

inline carrier_configuration make_default_carrier_configuration()
{
  carrier_configuration cfg;
  cfg.carrier_bw_mhz = 10;
  cfg.arfcn          = 365000;
  cfg.nof_ant        = 1;
  return cfg;
}

inline tdd_ul_dl_config_common make_default_tdd_ul_dl_config_common()
{
  tdd_ul_dl_config_common cfg{};
  cfg.ref_scs                            = subcarrier_spacing::kHz15;
  cfg.pattern1.dl_ul_tx_period_nof_slots = 10;
  cfg.pattern1.nof_dl_slots              = 6;
  cfg.pattern1.nof_dl_symbols            = 0;
  cfg.pattern1.nof_ul_slots              = 3;
  cfg.pattern1.nof_ul_symbols            = 0;
  return cfg;
}

inline coreset_configuration make_default_coreset_config()
{
  coreset_configuration cfg{};
  cfg.id = to_coreset_id(1);
  cfg.freq_domain_resources.resize(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
  for (size_t i = 0; i < 6; ++i) {
    cfg.freq_domain_resources.set(i);
  }
  cfg.duration             = 1;
  cfg.precoder_granurality = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
  return cfg;
}

/// Generates a default CORESET#0 configuration. The default CORESET#0 table index value used is equal to 6.
/// \remark See TS 38.213, Table 13-1.
inline coreset_configuration make_default_coreset0_config()
{
  coreset_configuration cfg = make_default_coreset_config();
  cfg.id                    = to_coreset_id(0);
  cfg.duration              = 1;
  cfg.coreset0_rb_start     = 0;
  for (size_t i = 0; i < 48U / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE; ++i) {
    cfg.freq_domain_resources.set(i);
  }
  // Implicit CORESET#0 parameters as per TS38.211-7.3.2.2.
  cfg.interleaved.emplace();
  cfg.interleaved->interleaver_sz = 2;
  cfg.interleaved->reg_bundle_sz  = 6;
  cfg.precoder_granurality        = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
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
  cfg.nof_candidates         = {1, 1, 1, 0, 0}; // TODO.
  cfg.type                   = search_space_configuration::common;
  return cfg;
}

inline search_space_configuration make_default_common_search_space_config()
{
  search_space_configuration cfg = make_default_search_space_zero_config();
  cfg.id                         = to_search_space_id(1);
  return cfg;
}

inline search_space_configuration make_default_ue_search_space_config()
{
  search_space_configuration cfg = make_default_common_search_space_config();
  cfg.cs_id                      = to_coreset_id(1);
  cfg.id                         = to_search_space_id(2);
  return cfg;
}

inline bwp_configuration make_default_init_bwp()
{
  bwp_configuration cfg{};
  cfg.scs         = subcarrier_spacing::kHz15;
  cfg.crbs        = {0, 52};
  cfg.cp_extended = false;
  return cfg;
}

inline dl_config_common make_default_dl_config_common()
{
  dl_config_common cfg{};

  // Configure FrequencyInfoDL.
  cfg.freq_info_dl.offset_to_point_a = 12;
  cfg.freq_info_dl.scs_carrier_list.emplace_back();
  cfg.freq_info_dl.scs_carrier_list.back().scs               = subcarrier_spacing::kHz15;
  cfg.freq_info_dl.scs_carrier_list.back().offset_to_carrier = 0;
  cfg.freq_info_dl.scs_carrier_list.back().carrier_bandwidth = 52;

  // Configure initial DL BWP.
  cfg.init_dl_bwp.generic_params = make_default_init_bwp();
  cfg.init_dl_bwp.pdcch_common.coreset0.emplace(make_default_coreset0_config());
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

inline ul_config_common make_default_ul_config_common()
{
  ul_config_common cfg{};
  cfg.init_ul_bwp.generic_params = make_default_init_bwp();
  cfg.init_ul_bwp.rach_cfg_common.emplace();
  cfg.init_ul_bwp.rach_cfg_common->total_nof_ra_preambles            = 64;
  cfg.init_ul_bwp.rach_cfg_common->prach_root_seq_index_l839_present = true;
  cfg.init_ul_bwp.rach_cfg_common->prach_root_seq_index              = 1;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window   = 10;
  cfg.init_ul_bwp.pusch_cfg_common.emplace();
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.resize(1);
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].k2 = 2;
  return cfg;
}

inline ssb_configuration make_default_ssb_config()
{
  ssb_configuration cfg;

  cfg.scs                   = subcarrier_spacing::kHz15;
  cfg.ssb_offset_to_point_A = 0;
  cfg.ssb_period            = ssb_periodicity::ms10;
  cfg.ssb_subcarrier_offset = 0;

  const unsigned beam_index = 63;
  cfg.ssb_bitmap            = uint64_t(1) << beam_index;
  cfg.beam_ids[beam_index]  = 0;

  cfg.beta_pss = ssb_beta_pss::dB_0;

  return cfg;
}

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline du_cell_config make_default_du_cell_config()
{
  du_cell_config cfg{};
  cfg.pci     = 1;
  cfg.plmn    = "00101";
  cfg.tac     = 1;
  cfg.cell_id = 1;

  cfg.dl_carrier       = make_default_carrier_configuration();
  cfg.ul_carrier       = make_default_carrier_configuration();
  cfg.coreset0_idx     = 6U;
  cfg.searchspace0_idx = 0U;
  cfg.dl_cfg_common    = make_default_dl_config_common();
  cfg.ul_cfg_common    = make_default_ul_config_common();
  cfg.scs_common       = subcarrier_spacing::kHz15;
  cfg.ssb_cfg          = make_default_ssb_config();
  cfg.dmrs_typeA_pos   = dmrs_typeA_position::pos2;
  cfg.cell_barred      = false;
  cfg.intra_freq_resel = false;
  return cfg;
}

} // namespace du_config_helpers

} // namespace srsgnb

#endif // SRSGNB_DU_CELL_CONFIG_HELPERS_H
