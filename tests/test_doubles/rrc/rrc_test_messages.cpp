/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "rrc_test_messages.h"
#include "srsran/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"

using namespace srsran;
using namespace asn1::rrc_nr;

ul_ccch_msg_s srsran::test_helpers::create_rrc_setup_request()
{
  ul_ccch_msg_s msg;

  rrc_setup_request_s& req = msg.msg.set_c1().set_rrc_setup_request();

  req.rrc_setup_request.establishment_cause.value = establishment_cause_opts::mo_sig;
  req.rrc_setup_request.ue_id.set_random_value().from_string("111000010100010001111111000010001100111");

  return msg;
}

ul_ccch_msg_s srsran::test_helpers::create_rrc_reestablishment_request(rnti_t             old_crnti,
                                                                       pci_t              old_pci,
                                                                       const std::string& short_mac_i)
{
  ul_ccch_msg_s msg;

  rrc_reest_request_s& req = msg.msg.set_c1().set_rrc_reest_request();

  req.rrc_reest_request.ue_id.c_rnti = (uint32_t)old_crnti;
  req.rrc_reest_request.ue_id.pci    = (uint16_t)old_pci;
  req.rrc_reest_request.ue_id.short_mac_i.from_string(short_mac_i);
  req.rrc_reest_request.reest_cause.value = reest_cause_opts::other_fail;

  return msg;
}

ul_dcch_msg_s srsran::test_helpers::create_rrc_setup_complete(uint8_t sel_plmn_id)
{
  ul_dcch_msg_s msg;

  rrc_setup_complete_s& req = msg.msg.set_c1().set_rrc_setup_complete();

  req.rrc_transaction_id        = 0;
  rrc_setup_complete_ies_s& ies = req.crit_exts.set_rrc_setup_complete();

  ies.sel_plmn_id = sel_plmn_id;
  ies.ded_nas_msg.from_string("7e004179000d0100f110f0ff000010325476092e02e0e0");

  return msg;
}

asn1::rrc_nr::ul_dcch_msg_s srsran::test_helpers::create_rrc_reestablishment_complete()
{
  ul_dcch_msg_s msg;

  rrc_reest_complete_s& req = msg.msg.set_c1().set_rrc_reest_complete();
  req.rrc_transaction_id    = 0;
  req.crit_exts.set_rrc_reest_complete();

  return msg;
}

asn1::rrc_nr::ul_dcch_msg_s srsran::test_helpers::create_rrc_reconfiguration_complete(uint8_t transaction_id)
{
  ul_dcch_msg_s msg;

  rrc_recfg_complete_s& req = msg.msg.set_c1().set_rrc_recfg_complete();
  req.rrc_transaction_id    = transaction_id;
  req.crit_exts.set_rrc_recfg_complete();

  return msg;
}

byte_buffer srsran::test_helpers::pack_ul_ccch_msg(const ul_ccch_msg_s& msg)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}

byte_buffer srsran::test_helpers::pack_ul_dcch_msg(const ul_dcch_msg_s& msg)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}

asn1::rrc_nr::sib1_s srsran::test_helpers::create_sib1()
{
  asn1::rrc_nr::sib1_s sib1;

  // Fill cell selection info.
  sib1.cell_sel_info_present            = true;
  sib1.cell_sel_info.q_rx_lev_min       = -70;
  sib1.cell_sel_info.q_qual_min_present = true;
  sib1.cell_sel_info.q_qual_min         = -20;

  // Fill cell access related info.
  sib1.cell_access_related_info.plmn_id_info_list.resize(1);
  plmn_id_info_s& plmn_info = sib1.cell_access_related_info.plmn_id_info_list[0];
  plmn_info.plmn_id_list.resize(1);
  plmn_info.plmn_id_list[0].mcc_present = true;
  plmn_info.plmn_id_list[0].mcc[0]      = 0;
  plmn_info.plmn_id_list[0].mcc[1]      = 0;
  plmn_info.plmn_id_list[0].mcc[2]      = 1;
  plmn_info.plmn_id_list[0].mnc.resize(2);
  plmn_info.plmn_id_list[0].mnc[0] = 0;
  plmn_info.plmn_id_list[0].mnc[1] = 1;
  plmn_info.tac.from_number(7);
  plmn_info.cell_id.from_number(0x66c0000);
  plmn_info.cell_reserved_for_oper = asn1::rrc_nr::plmn_id_info_s::cell_reserved_for_oper_opts::options::not_reserved;

  // Fill connection establishment failure control.
  sib1.conn_est_fail_ctrl_present = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_count =
      asn1::rrc_nr::conn_est_fail_ctrl_s::conn_est_fail_count_opts::options::n1;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_validity =
      asn1::rrc_nr::conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::options::s30;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_present = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset         = 1;

  // Fill serving cell config common.
  sib1.serving_cell_cfg_common_present = true;
  // > Fill DL config common.
  // >> Fill frequency info DL.
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.freq_band_list.resize(1);
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.freq_band_list[0].freq_band_ind_nr_present = true;
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.freq_band_list[0].freq_band_ind_nr         = 78;
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.offset_to_point_a                          = 0;
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.scs_specific_carrier_list.resize(1);
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.scs_specific_carrier_list[0].offset_to_carrier = 0;
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.scs_specific_carrier_list[0].subcarrier_spacing =
      asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.scs_specific_carrier_list[0].carrier_bw = 51;
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.scs_specific_carrier_list[0]
      .tx_direct_current_location_present = true;
  sib1.serving_cell_cfg_common.dl_cfg_common.freq_info_dl.scs_specific_carrier_list[0].tx_direct_current_location = 306;
  // >> Fill initial DL BWP.
  // >>> Fill generic parameters.
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params.location_and_bw = 13750;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params.subcarrier_spacing =
      asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
  // >>> Fill PDCCH config common.
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common_present = true;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.set_setup();
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().common_search_space_list.resize(1);
  // >>>> Fill common search space.
  search_space_s& css =
      sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().common_search_space_list[0];
  css.search_space_id                                = 1;
  css.coreset_id_present                             = true;
  css.coreset_id                                     = 0;
  css.monitoring_slot_periodicity_and_offset_present = true;
  css.monitoring_slot_periodicity_and_offset.set_sl1();
  css.monitoring_symbols_within_slot.from_number(8192);
  css.nrof_candidates_present = true;
  css.nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::options::n0;
  css.nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::options::n0;
  css.nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::options::n1;
  css.nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::options::n0;
  css.nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::options::n0;
  css.search_space_type_present = true;
  css.search_space_type.set_common();
  css.search_space_type.common().dci_format0_0_and_format1_0_present                                          = true;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().search_space_sib1_present   = true;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().search_space_sib1           = 0;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().paging_search_space_present = true;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().paging_search_space         = 1;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().ra_search_space_present     = true;
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup().ra_search_space             = 1;
  // >>> Fill PDSCH config common.
  sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present = true;
  auto& pdsch_cfg = sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common.set_setup();
  pdsch_cfg.pdsch_time_domain_alloc_list.resize(2);
  pdsch_cfg.pdsch_time_domain_alloc_list[0].map_type =
      asn1::rrc_nr::pdsch_time_domain_res_alloc_s::map_type_opts::options::type_a;
  pdsch_cfg.pdsch_time_domain_alloc_list[0].start_symbol_and_len = 53;
  pdsch_cfg.pdsch_time_domain_alloc_list[1].map_type =
      asn1::rrc_nr::pdsch_time_domain_res_alloc_s::map_type_opts::options::type_a;
  pdsch_cfg.pdsch_time_domain_alloc_list[1].start_symbol_and_len = 72;
  // >> Fill BCCH config.
  sib1.serving_cell_cfg_common.dl_cfg_common.bcch_cfg.mod_period_coeff =
      asn1::rrc_nr::bcch_cfg_s::mod_period_coeff_opts::options::n4;
  // >> Fill PCCH config.
  sib1.serving_cell_cfg_common.dl_cfg_common.pcch_cfg.default_paging_cycle =
      asn1::rrc_nr::paging_cycle_opts::options::rf128;
  sib1.serving_cell_cfg_common.dl_cfg_common.pcch_cfg.nand_paging_frame_offset.set_one_t();
  sib1.serving_cell_cfg_common.dl_cfg_common.pcch_cfg.ns = asn1::rrc_nr::pcch_cfg_s::ns_opts::options::one;
  // > Fill UL config common.
  sib1.serving_cell_cfg_common.ul_cfg_common_present = true;
  // >> Fill frequency info UL.
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.freq_band_list.resize(1);
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.freq_band_list[0].freq_band_ind_nr_present = true;
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.freq_band_list[0].freq_band_ind_nr         = 78;
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.absolute_freq_point_a_present              = true;
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.absolute_freq_point_a                      = 620688;
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.scs_specific_carrier_list.resize(1);
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.scs_specific_carrier_list[0].offset_to_carrier = 0;
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.scs_specific_carrier_list[0].subcarrier_spacing =
      asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
  sib1.serving_cell_cfg_common.ul_cfg_common.freq_info_ul.scs_specific_carrier_list[0].carrier_bw = 51;
  // >> Fill initial UL BWP.
  // >>> Fill generic parameters.
  sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.location_and_bw = 13750;
  sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.subcarrier_spacing =
      asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
  // >>> Fill RACH config common.
  sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present = true;
  auto& rach_cfg = sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common.set_setup();
  rach_cfg.rach_cfg_generic.prach_cfg_idx             = 159;
  rach_cfg.rach_cfg_generic.msg1_fdm                  = asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::options::one;
  rach_cfg.rach_cfg_generic.msg1_freq_start           = 11;
  rach_cfg.rach_cfg_generic.zero_correlation_zone_cfg = 0;
  rach_cfg.rach_cfg_generic.preamb_rx_target_pwr      = -100;
  rach_cfg.rach_cfg_generic.preamb_trans_max = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::options::n7;
  rach_cfg.rach_cfg_generic.pwr_ramp_step    = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::options::db4;
  rach_cfg.rach_cfg_generic.ra_resp_win      = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::options::sl20;
  rach_cfg.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  rach_cfg.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one() =
      asn1::rrc_nr::rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_opts::options::n64;
  rach_cfg.ra_contention_resolution_timer =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::options::sf64;
  rach_cfg.prach_root_seq_idx.set_l139()   = 1;
  rach_cfg.msg1_subcarrier_spacing_present = true;
  rach_cfg.msg1_subcarrier_spacing         = asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
  rach_cfg.restricted_set_cfg = asn1::rrc_nr::rach_cfg_common_s::restricted_set_cfg_opts::options::unrestricted_set;
  // >>> Fill PUSCH config common.
  sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common_present = true;
  auto& pusch_cfg = sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common.set_setup();
  pusch_cfg.pusch_time_domain_alloc_list.resize(5);
  pusch_cfg.pusch_time_domain_alloc_list[0].k2 = 4;
  pusch_cfg.pusch_time_domain_alloc_list[0].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::options::type_a;
  pusch_cfg.pusch_time_domain_alloc_list[0].start_symbol_and_len = 27;
  pusch_cfg.pusch_time_domain_alloc_list[1].k2                   = 5;
  pusch_cfg.pusch_time_domain_alloc_list[1].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::options::type_a;
  pusch_cfg.pusch_time_domain_alloc_list[1].start_symbol_and_len = 27;
  pusch_cfg.pusch_time_domain_alloc_list[2].k2                   = 6;
  pusch_cfg.pusch_time_domain_alloc_list[2].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::options::type_a;
  pusch_cfg.pusch_time_domain_alloc_list[2].start_symbol_and_len = 27;
  pusch_cfg.pusch_time_domain_alloc_list[3].k2                   = 7;
  pusch_cfg.pusch_time_domain_alloc_list[3].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::options::type_a;
  pusch_cfg.pusch_time_domain_alloc_list[3].start_symbol_and_len = 27;
  pusch_cfg.pusch_time_domain_alloc_list[4].k2                   = 11;
  pusch_cfg.pusch_time_domain_alloc_list[4].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::options::type_a;
  pusch_cfg.pusch_time_domain_alloc_list[4].start_symbol_and_len = 27;
  pusch_cfg.msg3_delta_preamb_present                            = true;
  pusch_cfg.msg3_delta_preamb                                    = 6;
  pusch_cfg.p0_nominal_with_grant_present                        = true;
  pusch_cfg.p0_nominal_with_grant                                = -76;
  // >>> Fill PUCCH config common.
  sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.pucch_cfg_common_present = true;
  auto& pucch_cfg = sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.pucch_cfg_common.set_setup();
  pucch_cfg.pucch_res_common_present = true;
  pucch_cfg.pucch_res_common         = 11;
  pucch_cfg.pucch_group_hop          = asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::options::neither;
  pucch_cfg.p0_nominal_present       = true;
  pucch_cfg.p0_nominal               = -90;
  // >> Fill time alignment timer common.
  sib1.serving_cell_cfg_common.ul_cfg_common.time_align_timer_common =
      asn1::rrc_nr::time_align_timer_opts::options::infinity;
  // > Fill n-Timing advance offset.
  sib1.serving_cell_cfg_common.n_timing_advance_offset_present = true;
  sib1.serving_cell_cfg_common.n_timing_advance_offset =
      asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::options::n25600;
  // > Fill SSB position in burst.
  sib1.serving_cell_cfg_common.ssb_positions_in_burst.in_one_group.from_number(128);
  // > Fill SSB periodicity serving cell.
  sib1.serving_cell_cfg_common.ssb_periodicity_serving_cell =
      asn1::rrc_nr::serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::options::ms10;
  // > Fill TDD UL DL configuration common.
  sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common_present = true;
  sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common.ref_subcarrier_spacing =
      asn1::rrc_nr::subcarrier_spacing_opts::options::khz30;
  sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
      asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::options::ms5;
  sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common.pattern1.nrof_dl_slots   = 6;
  sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common.pattern1.nrof_dl_symbols = 8;
  sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common.pattern1.nrof_ul_slots   = 3;
  sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common.pattern1.nrof_ul_symbols = 0;
  // > Fill ss PBCH block power.
  sib1.serving_cell_cfg_common.ss_pbch_block_pwr = -16;

  // Fill ue timers and constants.
  sib1.ue_timers_and_consts_present = true;
  sib1.ue_timers_and_consts.t300    = asn1::rrc_nr::ue_timers_and_consts_s::t300_opts::options::ms1000;
  sib1.ue_timers_and_consts.t301    = asn1::rrc_nr::ue_timers_and_consts_s::t301_opts::options::ms1000;
  sib1.ue_timers_and_consts.t310    = asn1::rrc_nr::ue_timers_and_consts_s::t310_opts::options::ms1000;
  sib1.ue_timers_and_consts.n310    = asn1::rrc_nr::ue_timers_and_consts_s::n310_opts::options::n1;
  sib1.ue_timers_and_consts.t311    = asn1::rrc_nr::ue_timers_and_consts_s::t311_opts::options::ms3000;
  sib1.ue_timers_and_consts.n311    = asn1::rrc_nr::ue_timers_and_consts_s::n311_opts::options::n1;
  sib1.ue_timers_and_consts.t319    = asn1::rrc_nr::ue_timers_and_consts_s::t319_opts::options::ms1000;

  return sib1;
}
