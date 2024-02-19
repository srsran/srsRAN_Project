/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "f1ap_configuration_helpers.h"
#include "asn1_rrc_config_helpers.h"
#include "srsran/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

byte_buffer srsran::srs_du::make_asn1_rrc_cell_mib_buffer(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;

  mib_s mib;
  switch (du_cfg.scs_common) {
    case srsran::subcarrier_spacing::kHz15:
    case srsran::subcarrier_spacing::kHz60:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs15or60;
      break;
    case srsran::subcarrier_spacing::kHz30:
    case srsran::subcarrier_spacing::kHz120:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs30or120;
      break;
    default:
      srsran_assertion_failure("Invalid SCS common");
      mib.sub_carrier_spacing_common.value = asn1::rrc_nr::mib_s::sub_carrier_spacing_common_opts::scs15or60;
  }
  /// As per TS 38.331, MIB, the field "ssb-SubcarrierOffset" in the MIB only encodes the 4 LSB of k_SSB.
  mib.ssb_subcarrier_offset            = static_cast<uint8_t>(du_cfg.ssb_cfg.k_ssb.to_uint() & 0b00001111U);
  mib.dmrs_type_a_position.value       = du_cfg.dmrs_typeA_pos == dmrs_typeA_position::pos2
                                             ? mib_s::dmrs_type_a_position_opts::pos2
                                             : mib_s::dmrs_type_a_position_opts::pos3;
  mib.pdcch_cfg_sib1.coreset_zero      = du_cfg.coreset0_idx;
  mib.pdcch_cfg_sib1.search_space_zero = du_cfg.searchspace0_idx;
  mib.cell_barred.value = du_cfg.cell_barred ? mib_s::cell_barred_opts::barred : mib_s::cell_barred_opts::not_barred;
  mib.intra_freq_resel.value =
      du_cfg.intra_freq_resel ? mib_s::intra_freq_resel_opts::allowed : mib_s::intra_freq_resel_opts::not_allowed;

  byte_buffer       buf;
  asn1::bit_ref     bref{buf};
  asn1::SRSASN_CODE ret = mib.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack MIB");
  return buf;
}

static asn1::rrc_nr::subcarrier_spacing_e get_asn1_scs(subcarrier_spacing scs)
{
  return asn1::rrc_nr::subcarrier_spacing_e{static_cast<asn1::rrc_nr::subcarrier_spacing_opts::options>(scs)};
}

static asn1::rrc_nr::dl_cfg_common_sib_s make_asn1_rrc_dl_cfg_common_sib(const dl_config_common& cfg)
{
  using namespace asn1::rrc_nr;
  dl_cfg_common_sib_s out;
  // > frequencyInfoDL FrequencyInfoDL-SIB
  for (const auto& dl_band : cfg.freq_info_dl.freq_band_list) {
    nr_multi_band_info_s asn1_band;
    asn1_band.freq_band_ind_nr_present = true;
    asn1_band.freq_band_ind_nr         = nr_band_to_uint(dl_band.band);
    out.freq_info_dl.freq_band_list.push_back(asn1_band);
  }
  out.freq_info_dl.offset_to_point_a = cfg.freq_info_dl.offset_to_point_a;
  out.freq_info_dl.scs_specific_carrier_list =
      srs_du::make_asn1_rrc_scs_specific_carrier_list(cfg.freq_info_dl.scs_carrier_list);
  // > initialDownlinkBWP BWP-DownlinkCommon
  out.init_dl_bwp = srs_du::make_asn1_init_dl_bwp(cfg);
  // BCCH-Config
  out.bcch_cfg.mod_period_coeff.value = bcch_cfg_s::mod_period_coeff_opts::n4;
  // PCCH-Config
  switch (cfg.pcch_cfg.default_paging_cycle) {
    case paging_cycle::rf32:
      out.pcch_cfg.default_paging_cycle.value = paging_cycle_opts::rf32;
      break;
    case paging_cycle::rf64:
      out.pcch_cfg.default_paging_cycle.value = paging_cycle_opts::rf64;
      break;
    case paging_cycle::rf128:
      out.pcch_cfg.default_paging_cycle.value = paging_cycle_opts::rf128;
      break;
    case paging_cycle::rf256:
      out.pcch_cfg.default_paging_cycle.value = paging_cycle_opts::rf256;
      break;
    default:
      report_fatal_error("Invalid default paging cycle set");
  }
  switch (cfg.pcch_cfg.nof_pf) {
    case pcch_config::nof_pf_per_drx_cycle::oneT:
      out.pcch_cfg.nand_paging_frame_offset.set_one_t();
      break;
    case pcch_config::nof_pf_per_drx_cycle::halfT: {
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.set_half_t();
      nof_pf       = cfg.pcch_cfg.paging_frame_offset;
    } break;
    case pcch_config::nof_pf_per_drx_cycle::quarterT: {
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.set_quarter_t();
      nof_pf       = cfg.pcch_cfg.paging_frame_offset;
    } break;
    case pcch_config::nof_pf_per_drx_cycle::oneEighthT: {
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.set_one_eighth_t();
      nof_pf       = cfg.pcch_cfg.paging_frame_offset;
    } break;
    case pcch_config::nof_pf_per_drx_cycle::oneSixteethT: {
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.set_one_sixteenth_t();
      nof_pf       = cfg.pcch_cfg.paging_frame_offset;
    } break;
    default:
      report_fatal_error("Invalid nof. paging frames per DRX cycle and paging frame offset set");
  }
  switch (cfg.pcch_cfg.ns) {
    case pcch_config::nof_po_per_pf::four:
      out.pcch_cfg.ns.value = pcch_cfg_s::ns_opts::four;
      break;
    case pcch_config::nof_po_per_pf::two:
      out.pcch_cfg.ns.value = pcch_cfg_s::ns_opts::two;
      break;
    case pcch_config::nof_po_per_pf::one:
      out.pcch_cfg.ns.value = pcch_cfg_s::ns_opts::one;
      break;
    default:
      report_fatal_error("Invalid nof. paging occasions per paging frame set");
  }
  if (cfg.pcch_cfg.first_pdcch_mo_of_po_type.has_value()) {
    out.pcch_cfg.first_pdcch_monitoring_occasion_of_po_present = true;
    switch (cfg.pcch_cfg.first_pdcch_mo_of_po_type.value()) {
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::scs15khzOneT: {
        auto& first_pmo_of_po = out.pcch_cfg.first_pdcch_monitoring_occasion_of_po.scs15_kh_zone_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::scs30khzOneT_scs15khzHalfT: {
        auto& first_pmo_of_po = out.pcch_cfg.first_pdcch_monitoring_occasion_of_po.scs30_kh_zone_t_scs15_kh_zhalf_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::scs60khzOneT_scs30khzHalfT_scs15khzQuarterT: {
        auto& first_pmo_of_po =
            out.pcch_cfg.first_pdcch_monitoring_occasion_of_po.scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::
          scs120khzOneT_scs60khzHalfT_scs30khzQuarterT_scs15khzOneEighthT: {
        auto& first_pmo_of_po = out.pcch_cfg.first_pdcch_monitoring_occasion_of_po
                                    .scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::
          scs120khzHalfT_scs60khzQuarterT_scs30khzOneEighthT_scs15khzOneSixteenthT: {
        auto& first_pmo_of_po =
            out.pcch_cfg.first_pdcch_monitoring_occasion_of_po
                .scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::
          scs480khzOneT_scs120khzQuarterT_scs60khzOneEighthT_scs30khzOneSixteenthT: {
        auto& first_pmo_of_po =
            out.pcch_cfg.first_pdcch_monitoring_occasion_of_po
                .scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::
          scs480khzHalfT_scs120khzOneEighthT_scs60khzOneSixteenthT: {
        auto& first_pmo_of_po = out.pcch_cfg.first_pdcch_monitoring_occasion_of_po
                                    .scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::scs480khzQuarterT_scs120khzOneSixteenthT: {
        auto& first_pmo_of_po =
            out.pcch_cfg.first_pdcch_monitoring_occasion_of_po.scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      default:
        report_fatal_error("Invalid first PDCCH monitoring occasion of paging occasion set");
    }
  }
  // TODO: Fill remaining fields.

  return out;
}

static asn1::rrc_nr::ul_cfg_common_sib_s make_asn1_rrc_ul_config_common(const ul_config_common& cfg)
{
  using namespace asn1::rrc_nr;
  ul_cfg_common_sib_s out;

  // > frequencyInfoUL FrequencyInfoUL-SIB
  for (const auto& ul_band : cfg.freq_info_ul.freq_band_list) {
    nr_multi_band_info_s asn1_band;
    asn1_band.freq_band_ind_nr_present = true;
    asn1_band.freq_band_ind_nr         = nr_band_to_uint(ul_band.band);
    out.freq_info_ul.freq_band_list.push_back(asn1_band);
  }
  out.freq_info_ul.absolute_freq_point_a_present = true;
  out.freq_info_ul.absolute_freq_point_a         = cfg.freq_info_ul.absolute_freq_point_a;
  if (cfg.freq_info_ul.p_max.has_value()) {
    out.freq_info_ul.p_max_present = true;
    out.freq_info_ul.p_max         = cfg.freq_info_ul.p_max->value();
  }
  out.freq_info_ul.scs_specific_carrier_list =
      srs_du::make_asn1_rrc_scs_specific_carrier_list(cfg.freq_info_ul.scs_carrier_list);

  // > initialUplinkBWP BWP-UplinkCommon
  out.init_ul_bwp = srs_du::make_asn1_rrc_initial_up_bwp(cfg);

  // > timeAlignmentTimerCommon TimeAlignmentTimer
  out.time_align_timer_common.value = time_align_timer_opts::infinity;

  return out;
}

static asn1::rrc_nr::serving_cell_cfg_common_sib_s make_asn1_rrc_cell_serving_cell_common(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;
  serving_cell_cfg_common_sib_s cell;
  cell.dl_cfg_common         = make_asn1_rrc_dl_cfg_common_sib(du_cfg.dl_cfg_common);
  cell.ul_cfg_common_present = true;
  cell.ul_cfg_common         = make_asn1_rrc_ul_config_common(du_cfg.ul_cfg_common);
  // SSB params.
  cell.ssb_positions_in_burst.in_one_group.from_number(static_cast<uint64_t>(du_cfg.ssb_cfg.ssb_bitmap) >>
                                                       static_cast<uint64_t>(56U));
  asn1::number_to_enum(cell.ssb_periodicity_serving_cell, ssb_periodicity_to_value(du_cfg.ssb_cfg.ssb_period));
  cell.ss_pbch_block_pwr = du_cfg.ssb_cfg.ssb_block_power;

  n_ta_offset ta_offset                = band_helper::get_ta_offset(du_cfg.dl_carrier.band);
  cell.n_timing_advance_offset_present = true;
  if (ta_offset == n_ta_offset::n0) {
    cell.n_timing_advance_offset.value = asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n0;
  } else if (ta_offset == n_ta_offset::n25600) {
    cell.n_timing_advance_offset.value =
        asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n25600;
  } else if (ta_offset == n_ta_offset::n39936) {
    cell.n_timing_advance_offset.value =
        asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n39936;
  }

  // TDD config
  if (du_cfg.tdd_ul_dl_cfg_common.has_value()) {
    cell.tdd_ul_dl_cfg_common_present = true;
    cell.tdd_ul_dl_cfg_common         = srs_du::make_asn1_rrc_tdd_ul_dl_cfg_common(du_cfg.tdd_ul_dl_cfg_common.value());
  }
  // TODO: Fill remaining fields.

  return cell;
}

static asn1::rrc_nr::sib1_s make_asn1_rrc_cell_sib1(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;

  sib1_s sib1;

  sib1.cell_sel_info_present            = true;
  sib1.cell_sel_info.q_rx_lev_min       = du_cfg.cell_sel_info.q_rx_lev_min.value();
  sib1.cell_sel_info.q_qual_min_present = true;
  sib1.cell_sel_info.q_qual_min         = du_cfg.cell_sel_info.q_qual_min.value();

  sib1.cell_access_related_info.plmn_id_info_list.resize(1);
  sib1.cell_access_related_info.plmn_id_info_list[0].plmn_id_list.resize(1);
  plmn_id_s& plmn  = sib1.cell_access_related_info.plmn_id_info_list[0].plmn_id_list[0];
  plmn.mcc_present = true;
  plmn.mcc[0]      = du_cfg.nr_cgi.plmn[0] - '0';
  plmn.mcc[1]      = du_cfg.nr_cgi.plmn[1] - '0';
  plmn.mcc[2]      = du_cfg.nr_cgi.plmn[2] - '0';
  plmn.mnc.resize(du_cfg.nr_cgi.plmn.size() == 5 ? 2 : 3);
  for (unsigned i = 3; i < du_cfg.nr_cgi.plmn.size(); ++i) {
    plmn.mnc[i - 3] = du_cfg.nr_cgi.plmn[i] - '0';
  }
  sib1.cell_access_related_info.plmn_id_info_list[0].tac_present = true;
  sib1.cell_access_related_info.plmn_id_info_list[0].tac.from_number(du_cfg.tac);
  sib1.cell_access_related_info.plmn_id_info_list[0].cell_id.from_number(du_cfg.nr_cgi.nci);
  sib1.cell_access_related_info.plmn_id_info_list[0].cell_reserved_for_oper.value =
      plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;

  sib1.conn_est_fail_ctrl_present                   = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_count.value = asn1::rrc_nr::conn_est_fail_ctrl_s::conn_est_fail_count_opts::n1;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_validity.value =
      conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::s30;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_present = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset         = 1;

  if (du_cfg.si_config.has_value()) {
    for (auto sib : du_cfg.si_config->sibs) {
      if (variant_holds_alternative<sib2_info>(sib)) {
        sib1.si_sched_info_present = true;
        bool ret = asn1::number_to_enum(sib1.si_sched_info.si_win_len, du_cfg.si_config.value().si_window_len_slots);
        srsran_assert(ret, "Invalid SI window length");
        sib1.si_sched_info.sched_info_list.resize(du_cfg.si_config->si_sched_info.size());
        for (unsigned i = 0; i != du_cfg.si_config->si_sched_info.size(); ++i) {
          const auto& cfg_si                = du_cfg.si_config->si_sched_info[i];
          auto&       asn1_si               = sib1.si_sched_info.sched_info_list[i];
          asn1_si.si_broadcast_status.value = sched_info_s::si_broadcast_status_opts::broadcasting;
          ret = asn1::number_to_enum(asn1_si.si_periodicity, cfg_si.si_period_radio_frames);
          srsran_assert(ret, "Invalid SI period");
          asn1_si.sib_map_info.resize(cfg_si.sib_mapping_info.size());
          for (unsigned j = 0; j != cfg_si.sib_mapping_info.size(); ++j) {
            const uint8_t sib_id                      = static_cast<uint8_t>(cfg_si.sib_mapping_info[j]);
            ret                                       = asn1::number_to_enum(asn1_si.sib_map_info[j].type, sib_id);
            asn1_si.sib_map_info[j].value_tag_present = true;
            asn1_si.sib_map_info[j].value_tag         = 0;
            srsran_assert(ret, "Invalid SIB id {}", sib_id);
          }
        }
      } else if (variant_holds_alternative<sib19_info>(sib)) {
        sib1.non_crit_ext_present                                               = true;
        sib1.non_crit_ext.non_crit_ext_present                                  = true;
        sib1.non_crit_ext.non_crit_ext.non_crit_ext_present                     = true;
        sib1.non_crit_ext.non_crit_ext.non_crit_ext.si_sched_info_v1700_present = true;
        sib1.non_crit_ext.non_crit_ext.non_crit_ext.cell_barred_ntn_r17_present = true;
        sib1.non_crit_ext.non_crit_ext.non_crit_ext.cell_barred_ntn_r17 =
            sib1_v1700_ies_s::cell_barred_ntn_r17_opts::not_barred;
        auto& si_sched_info_r17 = sib1.non_crit_ext.non_crit_ext.non_crit_ext.si_sched_info_v1700;
        si_sched_info_r17.sched_info_list2_r17.resize(du_cfg.si_config->si_sched_info.size());
        for (unsigned i = 0; i != du_cfg.si_config->si_sched_info.size(); ++i) {
          auto&       asn1_si_r17                   = si_sched_info_r17.sched_info_list2_r17[0];
          const auto& cfg_si                        = du_cfg.si_config->si_sched_info[i];
          asn1_si_r17.si_broadcast_status_r17.value = sched_info2_r17_s::si_broadcast_status_r17_opts::broadcasting;
          bool ret = asn1::number_to_enum(asn1_si_r17.si_periodicity_r17, cfg_si.si_period_radio_frames);
          srsran_assert(ret, "Invalid SI period");
          asn1_si_r17.sib_map_info_r17.resize(cfg_si.sib_mapping_info.size());
          for (unsigned j = 0; j != cfg_si.sib_mapping_info.size(); ++j) {
            const uint8_t sib_id_r17 = static_cast<uint8_t>(cfg_si.sib_mapping_info[j]);
            asn1_si_r17.sib_map_info_r17[j].sib_type_r17.set_type1_r17();
            ret = asn1::number_to_enum(asn1_si_r17.sib_map_info_r17[j].sib_type_r17.type1_r17(), sib_id_r17);
            srsran_assert(ret, "Invalid SIB id {}", sib_id_r17);
          }
        }
      }
    }
  }

  sib1.serving_cell_cfg_common_present = true;
  sib1.serving_cell_cfg_common         = make_asn1_rrc_cell_serving_cell_common(du_cfg);

  sib1.ue_timers_and_consts_present = true;

  bool ret = asn1::number_to_enum(sib1.ue_timers_and_consts.t300, du_cfg.ue_timers_and_constants.t300.count());
  srsran_assert(ret, "Invalid value for T300: {}", du_cfg.ue_timers_and_constants.t300.count());

  ret = asn1::number_to_enum(sib1.ue_timers_and_consts.t301, du_cfg.ue_timers_and_constants.t301.count());
  srsran_assert(ret, "Invalid value for T301: {}", du_cfg.ue_timers_and_constants.t301.count());

  ret = asn1::number_to_enum(sib1.ue_timers_and_consts.t310, du_cfg.ue_timers_and_constants.t310.count());
  srsran_assert(ret, "Invalid value for T310: {}", du_cfg.ue_timers_and_constants.t310.count());

  ret = asn1::number_to_enum(sib1.ue_timers_and_consts.n310, du_cfg.ue_timers_and_constants.n310);
  srsran_assert(ret, "Invalid value for N310: {}", du_cfg.ue_timers_and_constants.n310);

  ret = asn1::number_to_enum(sib1.ue_timers_and_consts.t311, du_cfg.ue_timers_and_constants.t311.count());
  srsran_assert(ret, "Invalid value for T311: {}", du_cfg.ue_timers_and_constants.t311.count());

  ret = asn1::number_to_enum(sib1.ue_timers_and_consts.n311, du_cfg.ue_timers_and_constants.n311);
  srsran_assert(ret, "Invalid value for N311: {}", du_cfg.ue_timers_and_constants.n311);

  ret = asn1::number_to_enum(sib1.ue_timers_and_consts.t319, du_cfg.ue_timers_and_constants.t319.count());
  srsran_assert(ret, "Invalid value for T319: {}", du_cfg.ue_timers_and_constants.t319.count());

  return sib1;
}

asn1::rrc_nr::sib2_s make_asn1_rrc_cell_sib2(const sib2_info& sib2_params)
{
  using namespace asn1::rrc_nr;
  sib2_s sib2;

  if (sib2_params.q_hyst_db) {
    sib2.cell_resel_info_common.q_hyst = sib2_s::cell_resel_info_common_s_::q_hyst_opts::db3;
  }
  sib2.cell_resel_serving_freq_info.thresh_serving_low_p = sib2_params.thresh_serving_low_p;
  sib2.cell_resel_serving_freq_info.cell_resel_prio      = sib2_params.cell_reselection_priority;

  sib2.intra_freq_cell_resel_info.q_rx_lev_min     = sib2_params.q_rx_lev_min;
  sib2.intra_freq_cell_resel_info.s_intra_search_p = sib2_params.s_intra_search_p;
  sib2.intra_freq_cell_resel_info.t_resel_nr       = sib2_params.t_reselection_nr;
  return sib2;
}

asn1::rrc_nr::sib19_r17_s make_asn1_rrc_cell_sib19(const sib19_info& sib19_params)
{
  using namespace asn1::rrc_nr;
  sib19_r17_s sib19;

  if (sib19_params.distance_thres.has_value()) {
    sib19.distance_thresh_r17_present = true;
    sib19.distance_thresh_r17         = sib19_params.distance_thres.value();
  }
  if (sib19_params.ref_location.has_value()) {
    sib19.ref_location_r17.from_string(sib19_params.ref_location.value());
  }

  sib19.t_service_r17_present = false;
  sib19.ntn_cfg_r17_present   = true;

  if (sib19_params.cell_specific_koffset.has_value()) {
    sib19.ntn_cfg_r17.cell_specific_koffset_r17_present = true;
    sib19.ntn_cfg_r17.cell_specific_koffset_r17         = sib19_params.cell_specific_koffset.value();
  }

  if (sib19_params.ephemeris_info.has_value()) {
    if (variant_holds_alternative<ecef_coordinates_t>(sib19_params.ephemeris_info.value())) {
      auto& pos_vel = variant_get<ecef_coordinates_t>(sib19_params.ephemeris_info.value());
      sib19.ntn_cfg_r17.ephemeris_info_r17_present = true;
      sib19.ntn_cfg_r17.ephemeris_info_r17.set_position_velocity_r17();
      sib19.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17().position_x_r17  = pos_vel.position_x;
      sib19.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17().position_y_r17  = pos_vel.position_y;
      sib19.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17().position_z_r17  = pos_vel.position_z;
      sib19.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17().velocity_vx_r17 = pos_vel.velocity_vx;
      sib19.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17().velocity_vy_r17 = pos_vel.velocity_vy;
      sib19.ntn_cfg_r17.ephemeris_info_r17.position_velocity_r17().velocity_vz_r17 = pos_vel.velocity_vz;
    } else {
      auto& orbital_elem = variant_get<orbital_coordinates_t>(sib19_params.ephemeris_info.value());
      sib19.ntn_cfg_r17.ephemeris_info_r17_present = true;
      sib19.ntn_cfg_r17.ephemeris_info_r17.set_orbital_r17();
      sib19.ntn_cfg_r17.ephemeris_info_r17.orbital_r17().semi_major_axis_r17 = (uint64_t)orbital_elem.semi_major_axis;
      sib19.ntn_cfg_r17.ephemeris_info_r17.orbital_r17().eccentricity_r17    = (uint32_t)orbital_elem.eccentricity;
      sib19.ntn_cfg_r17.ephemeris_info_r17.orbital_r17().periapsis_r17       = (uint32_t)orbital_elem.periapsis;
      sib19.ntn_cfg_r17.ephemeris_info_r17.orbital_r17().longitude_r17       = (uint32_t)orbital_elem.longitude;
      sib19.ntn_cfg_r17.ephemeris_info_r17.orbital_r17().inclination_r17     = (int32_t)orbital_elem.inclination;
      sib19.ntn_cfg_r17.ephemeris_info_r17.orbital_r17().mean_anomaly_r17    = (uint32_t)orbital_elem.mean_anomaly;
    }
  }
  if (sib19_params.epoch_time.has_value()) {
    sib19.ntn_cfg_r17.epoch_time_r17_present          = true;
    sib19.ntn_cfg_r17.epoch_time_r17.sfn_r17          = sib19_params.epoch_time.value().sfn;
    sib19.ntn_cfg_r17.epoch_time_r17.sub_frame_nr_r17 = sib19_params.epoch_time.value().subframe_number;
  }
  if (sib19_params.k_mac.has_value()) {
    sib19.ntn_cfg_r17.kmac_r17_present = true;
    sib19.ntn_cfg_r17.kmac_r17         = sib19_params.k_mac.value();
  }

  sib19.ntn_cfg_r17.ntn_polarization_dl_r17_present      = false;
  sib19.ntn_cfg_r17.ntn_polarization_ul_r17_present      = false;
  sib19.ntn_cfg_r17.ntn_ul_sync_validity_dur_r17_present = false;
  if (sib19_params.ta_info.has_value()) {
    sib19.ntn_cfg_r17.ta_info_r17_present                             = true;
    sib19.ntn_cfg_r17.ta_info_r17.ta_common_drift_r17_present         = false;
    sib19.ntn_cfg_r17.ta_info_r17.ta_common_drift_variant_r17_present = false;
    sib19.ntn_cfg_r17.ta_info_r17.ta_common_r17                       = sib19_params.ta_info.value().ta_common;
    sib19.ntn_cfg_r17.ta_info_r17.ta_common_drift_r17                 = sib19_params.ta_info.value().ta_common_drift;
    sib19.ntn_cfg_r17.ta_info_r17.ta_common_drift_variant_r17 = sib19_params.ta_info.value().ta_common_drift_variant;
  }

  return sib19;
}

byte_buffer srsran::srs_du::make_asn1_rrc_cell_sib1_buffer(const du_cell_config& du_cfg, std::string* js_str)
{
  byte_buffer          buf;
  asn1::bit_ref        bref{buf};
  asn1::rrc_nr::sib1_s sib1 = make_asn1_rrc_cell_sib1(du_cfg);
  asn1::SRSASN_CODE    ret  = sib1.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack SIB1");

  if (js_str != nullptr) {
    asn1::json_writer js;
    sib1.to_json(js);
    *js_str = js.to_string();
  }
  return buf;
}

static asn1::rrc_nr::sys_info_ies_s::sib_type_and_info_item_c_ make_asn1_rrc_sib_item(const sib_info& sib)
{
  using namespace asn1::rrc_nr;

  sys_info_ies_s::sib_type_and_info_item_c_ ret;

  switch (get_sib_info_type(sib)) {
    case sib_type::sib2: {
      const sib2_info& cfg     = variant_get<sib2_info>(sib);
      sib2_s&          out_sib = ret.set_sib2();
      out_sib                  = make_asn1_rrc_cell_sib2(cfg);
      if (cfg.nof_ssbs_to_average.has_value()) {
        out_sib.cell_resel_info_common.nrof_ss_blocks_to_average_present = true;
        out_sib.cell_resel_info_common.nrof_ss_blocks_to_average         = cfg.nof_ssbs_to_average.value();
      }
      // TODO
    } break;
    case sib_type::sib19: {
      const sib19_info& cfg     = variant_get<sib19_info>(sib);
      sib19_r17_s&      out_sib = ret.set_sib19_v1700();
      out_sib                   = make_asn1_rrc_cell_sib19(cfg);
    } break;
    default:
      srsran_assertion_failure("Invalid SIB type");
  }

  return ret;
}

std::vector<byte_buffer> srsran::srs_du::make_asn1_rrc_cell_bcch_dl_sch_msgs(const du_cell_config& du_cfg)
{
  std::vector<byte_buffer> msgs;

  // Pack SIB1.
  {
    byte_buffer                     buf;
    asn1::bit_ref                   bref{buf};
    asn1::rrc_nr::bcch_dl_sch_msg_s msg;
    msg.msg.set_c1().set_sib_type1() = make_asn1_rrc_cell_sib1(du_cfg);
    asn1::SRSASN_CODE ret            = msg.pack(bref);
    srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack SIB1");
    msgs.push_back(std::move(buf));
  }

  // Pack SI messages.
  if (du_cfg.si_config.has_value()) {
    const auto& sibs = du_cfg.si_config.value().sibs;

    for (const auto& si_sched : du_cfg.si_config.value().si_sched_info) {
      byte_buffer                     buf;
      asn1::bit_ref                   bref{buf};
      asn1::rrc_nr::bcch_dl_sch_msg_s msg;
      asn1::rrc_nr::sys_info_ies_s&   si_ies = msg.msg.set_c1().set_sys_info().crit_exts.set_sys_info();

      // Search for SIB contained in this SI message.
      for (sib_type sib_id : si_sched.sib_mapping_info) {
        auto it = std::find_if(
            sibs.begin(), sibs.end(), [sib_id](const sib_info& sib) { return get_sib_info_type(sib) == sib_id; });
        srsran_assert(it != sibs.end(), "SIB{} in SIB mapping info has no defined config", (unsigned)sib_id);

        si_ies.sib_type_and_info.push_back(make_asn1_rrc_sib_item(*it));
      }

      asn1::SRSASN_CODE ret = msg.pack(bref);
      srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack other SIBs");
      msgs.push_back(std::move(buf));
    }
  }

  return msgs;
}

byte_buffer srsran::srs_du::make_asn1_meas_time_cfg_buffer(const du_cell_config& du_cfg)
{
  byte_buffer                     buf;
  asn1::bit_ref                   bref{buf};
  asn1::rrc_nr::meas_timing_cfg_s cfg;
  auto&                           meas_timing = cfg.crit_exts.set_c1().set_meas_timing_conf();
  meas_timing.meas_timing.resize(1);
  auto& meas_item = meas_timing.meas_timing[0];

  // MeasTiming
  meas_item.freq_and_timing_present = true;
  auto& freq_time                   = meas_item.freq_and_timing;
  freq_time.ssb_subcarrier_spacing  = get_asn1_scs(du_cfg.ssb_cfg.scs);
  freq_time.carrier_freq            = du_cfg.dl_cfg_common.freq_info_dl.absolute_frequency_ssb;

  // > Derive SSB periodicity, duration and offset.
  // TODO: Derive the correct duration.
  freq_time.ssb_meas_timing_cfg.dur.value = asn1::rrc_nr::ssb_mtc_s::dur_opts::sf5;
  // TODO: Derive the correct offset.
  switch (du_cfg.ssb_cfg.ssb_period) {
    case ssb_periodicity::ms5:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf5() = 0;
      break;
    case ssb_periodicity::ms10:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf10() = 0;
      break;
    case ssb_periodicity::ms20:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf20() = 0;
      break;
    case ssb_periodicity::ms40:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf40() = 0;
      break;
    case ssb_periodicity::ms80:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf80() = 0;
      break;
    case ssb_periodicity::ms160:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf160() = 0;
      break;
    default:
      report_fatal_error("Invalid SSB periodicity {}.", du_cfg.ssb_cfg.ssb_period);
  }
  meas_item.pci_present = true;
  meas_item.pci         = du_cfg.pci;

  asn1::SRSASN_CODE ret = cfg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack meas_time_cfg");
  return buf;
}

void srsran::srs_du::fill_f1_setup_request(f1_setup_request_message&            req,
                                           const du_manager_params::ran_params& ran_params,
                                           std::vector<std::string>*            sib1_jsons)
{
  req.gnb_du_id   = ran_params.gnb_du_id;
  req.gnb_du_name = ran_params.gnb_du_name;
  req.rrc_version = ran_params.rrc_version;

  req.served_cells.resize(ran_params.cells.size());

  for (unsigned i = 0; i != ran_params.cells.size(); ++i) {
    const du_cell_config& cell_cfg  = ran_params.cells[i];
    f1_cell_setup_params& serv_cell = req.served_cells[i];

    // Fill serving cell info.
    serv_cell.nr_cgi     = cell_cfg.nr_cgi;
    serv_cell.pci        = cell_cfg.pci;
    serv_cell.tac        = cell_cfg.tac;
    serv_cell.duplx_mode = cell_cfg.tdd_ul_dl_cfg_common.has_value() ? duplex_mode::TDD : duplex_mode::FDD;
    serv_cell.scs_common = cell_cfg.scs_common;
    serv_cell.dl_carrier = cell_cfg.dl_carrier;
    if (serv_cell.duplx_mode == duplex_mode::FDD) {
      serv_cell.ul_carrier = cell_cfg.ul_carrier;
    }

    serv_cell.packed_meas_time_cfg = make_asn1_meas_time_cfg_buffer(cell_cfg);

    // Pack RRC ASN.1 Serving Cell system info.
    serv_cell.packed_mib = make_asn1_rrc_cell_mib_buffer(cell_cfg);
    std::string js_str;
    serv_cell.packed_sib1 = make_asn1_rrc_cell_sib1_buffer(cell_cfg, sib1_jsons != nullptr ? &js_str : nullptr);

    if (sib1_jsons != nullptr) {
      sib1_jsons->push_back(js_str);
    }
  }
}

byte_buffer srsran::srs_du::make_asn1_rrc_cell_sib19_buffer(const sib19_info& sib19_params, std::string* js_str)
{
  byte_buffer               buf;
  asn1::bit_ref             bref{buf};
  asn1::rrc_nr::sib19_r17_s sib19 = make_asn1_rrc_cell_sib19(sib19_params);
  asn1::SRSASN_CODE         ret   = sib19.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack SIB19");

  if (js_str != nullptr) {
    asn1::json_writer js;
    sib19.to_json(js);
    *js_str = js.to_string();
  }
  return buf;
}
