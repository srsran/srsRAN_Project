/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_MAC_CELL_CONFIGURATION_HELPERS_H
#define SRSGNB_MAC_CELL_CONFIGURATION_HELPERS_H

#include "../du_manager_config.h"
#include "srsgnb/du/du_cell_config_factory.h"
#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/mac/mac_ue_configurator.h"
#include "srsgnb/ran/tdd_ul_dl_config.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the DU config file.

namespace srsgnb {

namespace test_helpers {

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
  cfg.freq_domain_resources.resize(pdcch_constants::MAX_NOF_FREQ_RESOUCES);
  for (size_t i = 0; i < 6; ++i) {
    cfg.freq_domain_resources.set(i);
  }
  cfg.duration             = 1;
  cfg.precoder_granurality = coreset_configuration::precoder_granularity_type::same_as_reg_bundle;
  return cfg;
}

inline coreset_configuration make_default_coreset0_config()
{
  coreset_configuration cfg = make_default_coreset_config();
  cfg.id                    = to_coreset_id(0);
  cfg.coreset0_rb_start     = 1;
  return cfg;
}

inline search_space_configuration make_default_search_space_zero_config()
{
  search_space_configuration cfg{};
  cfg.id             = to_search_space_id(0);
  cfg.cs_id          = to_coreset_id(0);
  cfg.duration       = 1;
  cfg.type           = search_space_configuration::common;
  cfg.nof_candidates = {0, 0, 0, 0, 0};
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
  cfg.freq_info_dl.offset_to_point_a = 0;
  cfg.freq_info_dl.scs_carrier_list.emplace_back();
  cfg.freq_info_dl.scs_carrier_list.back().scs               = subcarrier_spacing::kHz15;
  cfg.freq_info_dl.scs_carrier_list.back().offset_to_carrier = 0;
  cfg.freq_info_dl.scs_carrier_list.back().carrier_bandwidth = 52;

  // Configure initial DL BWP.
  cfg.init_dl_bwp.generic_params = make_default_init_bwp();
  cfg.init_dl_bwp.pdcch_common.coreset0.emplace(make_default_coreset0_config());
  cfg.init_dl_bwp.pdcch_common.search_spaces.emplace(0, make_default_search_space_zero_config());
  cfg.init_dl_bwp.pdcch_common.search_spaces.emplace(1, make_default_common_search_space_config());
  cfg.init_dl_bwp.pdcch_common.ra_search_space_id = to_search_space_id(1);
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.emplace_back();
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().k0 = 0;
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().map_type =
      pdsch_time_domain_resource_allocation::mapping_type::typeA;
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.back().symbols = {2, 14};

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
  ssb_configuration cfg{};
  cfg.scs        = subcarrier_spacing::kHz15;
  cfg.ssb_period = 10;
  cfg.ssb_bitmap = static_cast<uint64_t>(0b10101010) << static_cast<uint64_t>(56U);
  // TODO: Add remaining.
  return cfg;
}

// TODO: Temporary. Remove once DU manager takes config from file or orchestrator.
inline mac_cell_creation_request make_default_mac_cell_creation_request()
{
  mac_cell_creation_request msg{};

  msg.cell_index = to_du_cell_index(0);
  msg.pci        = 1;

  msg.scs_common    = subcarrier_spacing::kHz15;
  msg.ssb_scs       = subcarrier_spacing::kHz15;
  msg.ssb_cfg       = make_default_ssb_config();
  msg.dl_carrier    = make_default_carrier_configuration();
  msg.ul_carrier    = make_default_carrier_configuration();
  msg.dl_cfg_common = make_default_dl_config_common();
  msg.ul_cfg_common = make_default_ul_config_common();

  /// SIB1 parameters.
  msg.pdcch_config_sib1     = 0b10000000U;
  msg.sib1_mcs              = 5;
  msg.sib1_rv               = 0;
  msg.sib1_dci_aggr_lev     = aggregation_level::n4;
  msg.sib1_rxtx_periodicity = 0;

  // TODO: Remaining fields.

  return msg;
}

inline mac_ue_create_request_message make_default_ue_creation_request()
{
  mac_ue_create_request_message msg{};

  msg.ue_index   = to_du_ue_index(0);
  msg.crnti      = to_rnti(0x4601);
  msg.cell_index = to_du_cell_index(0);

  msg.serv_cell_cfg.init_dl_bwp.emplace();
  bwp_downlink_dedicated& dl_bwp = *msg.serv_cell_cfg.init_dl_bwp;
  dl_bwp.pdcch_cfg.emplace();
  dl_bwp.pdcch_cfg->coreset_to_addmod_list.emplace_back(make_default_coreset_config());
  coreset_configuration& cs_cfg = dl_bwp.pdcch_cfg->coreset_to_addmod_list.back();
  cs_cfg.id                     = to_coreset_id(1);

  dl_bwp.pdcch_cfg->ss_to_addmod_list.emplace_back(make_default_ue_search_space_config());
  return msg;
}

} // namespace test_helpers

inline mac_cell_creation_request make_mac_cell_config(du_cell_index_t cell_index, const du_cell_config& du_cfg)
{
  mac_cell_creation_request mac_cfg = test_helpers::make_default_mac_cell_creation_request();
  mac_cfg.cell_index                = cell_index;
  mac_cfg.pci                       = du_cfg.pci;
  return mac_cfg;
}

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_HELPERS_H
