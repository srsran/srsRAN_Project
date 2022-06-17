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
#include "srsgnb/asn1/asn1_utils.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/du/du_cell_config_helpers.h"
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
  cfg.nof_candidates = {1, 1, 1, 0, 0}; // TODO.
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
  cfg.init_dl_bwp.pdsch_common.dmrs_dl_pdsch_typeA.emplace();
  cfg.init_dl_bwp.pdsch_common.dmrs_dl_pdsch_typeA->type                 = dmrs_type::type1;
  cfg.init_dl_bwp.pdsch_common.dmrs_dl_pdsch_typeA->additional_positions = dmrs_additional_positions::pos2;
  cfg.init_dl_bwp.pdsch_common.dmrs_dl_pdsch_typeA->max_length           = dmrs_max_length::len1;

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

/// This is a temporary helper function.
inline byte_buffer generate_sib1_byte_buffer_from_asn1()
{
  asn1::rrc_nr::sib1_s sib1;

  sib1.cell_sel_info_present            = true;
  sib1.cell_sel_info.q_rx_lev_min       = -70;
  sib1.cell_sel_info.q_qual_min_present = true;
  sib1.cell_sel_info.q_qual_min         = -20;

  sib1.cell_access_related_info.plmn_id_list.resize(1);
  sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list.resize(1);
  // TODO: add missing pieces.
  // plmn.from_number(cfg.mcc, cfg.mnc);
  // srsran::to_asn1(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list[0], plmn);
  sib1.cell_access_related_info.plmn_id_list[0].tac_present = true;
  // sib1.cell_access_related_info.plmn_id_list[0].tac.from_number(cell_cfg.tac);
  sib1.cell_access_related_info.plmn_id_list[0].cell_id.from_number(/* random choice*/ 1);
  sib1.cell_access_related_info.plmn_id_list[0].cell_reserved_for_oper.value =
      asn1::rrc_nr::plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;

  sib1.conn_est_fail_ctrl_present                   = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_count.value = asn1::rrc_nr::conn_est_fail_ctrl_s::conn_est_fail_count_opts::n1;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_validity.value =
      asn1::rrc_nr::conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::s30;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_present = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset         = 1;

  // TODO: add missing pieces

  //  sib1.si_sched_info_present                                  = true;
  //  sib1.si_sched_info.si_request_cfg.rach_occasions_si_present = true;
  //  sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.ra_resp_win.value =
  //      rach_cfg_generic_s::ra_resp_win_opts::sl8;
  //  sib1.si_sched_info.si_win_len.value = si_sched_info_s::si_win_len_opts::s20;
  //  sib1.si_sched_info.sched_info_list.resize(1);
  //  sib1.si_sched_info.sched_info_list[0].si_broadcast_status.value =
  //  sched_info_s::si_broadcast_status_opts::broadcasting; sib1.si_sched_info.sched_info_list[0].si_periodicity.value =
  //  sched_info_s::si_periodicity_opts::rf16; sib1.si_sched_info.sched_info_list[0].sib_map_info.resize(1);
  //  // scheduling of SI messages
  //  sib1.si_sched_info.sched_info_list[0].sib_map_info[0].type.value        = sib_type_info_s::type_opts::sib_type2;
  //  sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag_present = true;
  //  sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag         = 0;

  sib1.serving_cell_cfg_common_present = true;
  // TODO: add missing pieces.
  // HANDLE_ERROR(fill_serv_cell_cfg_common_sib(cfg, cc, sib1.serving_cell_cfg_common));

  sib1.ue_timers_and_consts_present    = true;
  sib1.ue_timers_and_consts.t300.value = asn1::rrc_nr::ue_timers_and_consts_s::t300_opts::ms1000;
  sib1.ue_timers_and_consts.t301.value = asn1::rrc_nr::ue_timers_and_consts_s::t301_opts::ms1000;
  sib1.ue_timers_and_consts.t310.value = asn1::rrc_nr::ue_timers_and_consts_s::t310_opts::ms1000;
  sib1.ue_timers_and_consts.n310.value = asn1::rrc_nr::ue_timers_and_consts_s::n310_opts::n1;
  sib1.ue_timers_and_consts.t311.value = asn1::rrc_nr::ue_timers_and_consts_s::t311_opts::ms30000;
  sib1.ue_timers_and_consts.n311.value = asn1::rrc_nr::ue_timers_and_consts_s::n311_opts::n1;
  sib1.ue_timers_and_consts.t319.value = asn1::rrc_nr::ue_timers_and_consts_s::t319_opts::ms1000;

  byte_buffer   byte_buf{};
  asn1::bit_ref packed_sib1{byte_buf};
  sib1.pack(packed_sib1);

  return byte_buf;
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
  msg.sib1_payload.append(generate_sib1_byte_buffer_from_asn1());

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
