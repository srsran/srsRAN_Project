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

inline mac_ue_create_request_message make_default_ue_creation_request()
{
  mac_ue_create_request_message msg{};

  msg.ue_index   = to_du_ue_index(0);
  msg.crnti      = to_rnti(0x4601);
  msg.cell_index = to_du_cell_index(0);

  msg.serv_cell_cfg.init_dl_bwp.emplace();
  bwp_downlink_dedicated& dl_bwp = *msg.serv_cell_cfg.init_dl_bwp;
  dl_bwp.pdcch_cfg.emplace();
  dl_bwp.pdcch_cfg->coreset_to_addmod_list.emplace_back(du_config_helpers::make_default_coreset_config());
  coreset_configuration& cs_cfg = dl_bwp.pdcch_cfg->coreset_to_addmod_list.back();
  cs_cfg.id                     = to_coreset_id(1);

  dl_bwp.pdcch_cfg->ss_to_addmod_list.emplace_back(du_config_helpers::make_default_ue_search_space_config());
  return msg;
}

} // namespace test_helpers

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
inline sched_cell_configuration_request_message make_sched_cell_config_req(du_cell_index_t       cell_index,
                                                                           const du_cell_config& du_cfg)
{
  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index    = cell_index;
  sched_req.pci           = du_cfg.pci;
  sched_req.dl_carrier    = du_cfg.dl_carrier;
  sched_req.ul_carrier    = du_cfg.ul_carrier;
  sched_req.dl_cfg_common = du_cfg.dl_cfg_common;
  sched_req.ul_cfg_common = du_cfg.ul_cfg_common;
  sched_req.scs_common    = du_cfg.scs_common;
  sched_req.ssb_config    = du_cfg.ssb_cfg;

  sched_req.nof_beams     = 1;
  sched_req.nof_ant_ports = 1;
  sched_req.nof_ant_ports = 1;

  sched_req.dmrs_typeA_pos = dmrs_typeA_position::pos2;

  /// SIB1 parameters.
  sched_req.pdcch_config_sib1     = 0b10000000U;
  sched_req.sib1_mcs              = 5;
  sched_req.sib1_rv               = 0;
  sched_req.sib1_dci_aggr_lev     = aggregation_level::n4;
  sched_req.sib1_retx_periodicity = 0;

  return sched_req;
}

/// Derives MAC Cell Configuration from DU Cell Configuration.
inline mac_cell_creation_request make_mac_cell_config(du_cell_index_t cell_index, const du_cell_config& du_cfg)
{
  mac_cell_creation_request mac_cfg{};
  mac_cfg.cell_index = cell_index;
  mac_cfg.pci        = du_cfg.pci;
  mac_cfg.scs_common = du_cfg.scs_common;
  mac_cfg.ssb_scs    = du_cfg.ssb_scs;
  mac_cfg.ssb_cfg    = du_cfg.ssb_cfg;
  mac_cfg.dl_carrier = du_cfg.dl_carrier;
  mac_cfg.ul_carrier = du_cfg.ul_carrier;
  mac_cfg.sib1_payload.append(test_helpers::generate_sib1_byte_buffer_from_asn1());
  mac_cfg.sched_req = make_sched_cell_config_req(cell_index, du_cfg);
  return mac_cfg;
}

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_HELPERS_H
