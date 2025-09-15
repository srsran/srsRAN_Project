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

#include "asn1_sys_info_packer.h"
#include "asn1_ntn_config_helpers.h"
#include "asn1_rrc_config_helpers.h"
#include "srsran/asn1/rrc_nr/bcch_bch_msg.h"
#include "srsran/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "srsran/asn1/rrc_nr/sys_info.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_high/du_manager/cbs/cbs_encoder.h"
#include "srsran/ran/band_helper.h"

using namespace srsran;
using namespace srs_du;

byte_buffer asn1_packer::pack_mib(const du_cell_config& du_cfg)
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

static asn1::rrc_nr::dl_cfg_common_sib_s make_asn1_rrc_dl_cfg_common_sib(const dl_config_common& cfg)
{
  using namespace asn1::rrc_nr;

  dl_cfg_common_sib_s out;
  // > frequencyInfoDL FrequencyInfoDL-SIB.
  for (const auto& dl_band : cfg.freq_info_dl.freq_band_list) {
    nr_multi_band_info_s asn1_band;
    asn1_band.freq_band_ind_nr_present = true;
    asn1_band.freq_band_ind_nr         = nr_band_to_uint(dl_band.band);
    out.freq_info_dl.freq_band_list.push_back(asn1_band);
  }
  out.freq_info_dl.offset_to_point_a = cfg.freq_info_dl.offset_to_point_a;
  out.freq_info_dl.scs_specific_carrier_list =
      srs_du::make_asn1_rrc_scs_specific_carrier_list(cfg.freq_info_dl.scs_carrier_list);

  // > initialDownlinkBWP BWP-DownlinkCommon.
  out.init_dl_bwp = srs_du::make_asn1_init_dl_bwp(cfg);

  // BCCH-Config.
  out.bcch_cfg.mod_period_coeff.value = bcch_cfg_s::mod_period_coeff_opts::n4;

  // PCCH-Config.
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

  // > frequencyInfoUL FrequencyInfoUL-SIB.
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

  // > initialUplinkBWP BWP-UplinkCommon.
  out.init_ul_bwp = srs_du::make_asn1_rrc_initial_up_bwp(cfg);

  // > timeAlignmentTimerCommon TimeAlignmentTimer.
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

  n_ta_offset ta_offset = band_helper::get_ta_offset(du_cfg.dl_carrier.band);
  switch (ta_offset) {
    case n_ta_offset::n0:
      cell.n_timing_advance_offset_present = true;
      cell.n_timing_advance_offset.value =
          asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n0;
      break;
    case n_ta_offset::n25600:
      cell.n_timing_advance_offset_present = true;
      cell.n_timing_advance_offset.value =
          asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n25600;
      break;
    case n_ta_offset::n39936:
      cell.n_timing_advance_offset_present = true;
      cell.n_timing_advance_offset.value =
          asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n39936;
      break;
    case n_ta_offset::n13792:
      // The parameter is ignored.
      break;
    default:
      report_fatal_error("Invalid timing advance offset");
  }

  // TDD config.
  if (du_cfg.tdd_ul_dl_cfg_common.has_value()) {
    cell.tdd_ul_dl_cfg_common_present = true;
    cell.tdd_ul_dl_cfg_common         = srs_du::make_asn1_rrc_tdd_ul_dl_cfg_common(du_cfg.tdd_ul_dl_cfg_common.value());
  }
  // TODO: Fill remaining fields.

  return cell;
}

static asn1::rrc_nr::plmn_id_s make_asn1_plmn_id(const plmn_identity& plmn)
{
  using namespace asn1::rrc_nr;

  plmn_id_s asn1_plmn;
  asn1_plmn.mcc_present         = true;
  asn1_plmn.mcc                 = plmn.mcc().to_bytes();
  static_vector<uint8_t, 3> mnc = plmn.mnc().to_bytes();
  asn1_plmn.mnc.resize(mnc.size());
  for (unsigned i = 0, sz = mnc.size(); i != sz; ++i) {
    asn1_plmn.mnc[i] = mnc[i];
  }
  return asn1_plmn;
}

static asn1::rrc_nr::plmn_id_info_s
make_asn1_plmn_id_info(const plmn_identity& plmn, const tac_t& tac, const nr_cell_identity& nci)
{
  using namespace asn1::rrc_nr;

  plmn_id_info_s asn1_plmn_info;
  asn1_plmn_info.plmn_id_list.push_back(make_asn1_plmn_id(plmn));
  asn1_plmn_info.tac_present = true;
  asn1_plmn_info.tac.from_number(tac);
  asn1_plmn_info.cell_id.from_number(nci.value());
  asn1_plmn_info.cell_reserved_for_oper.value = plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;
  return asn1_plmn_info;
}

static asn1::rrc_nr::sib1_s make_asn1_rrc_cell_sib1(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;

  sib1_s sib1;

  sib1.cell_sel_info_present            = true;
  sib1.cell_sel_info.q_rx_lev_min       = du_cfg.cell_sel_info.q_rx_lev_min.value();
  sib1.cell_sel_info.q_qual_min_present = true;
  sib1.cell_sel_info.q_qual_min         = du_cfg.cell_sel_info.q_qual_min.value();

  auto& asn1_plmn_id_info_list = sib1.cell_access_related_info.plmn_id_info_list;
  asn1_plmn_id_info_list.push_back(make_asn1_plmn_id_info(du_cfg.nr_cgi.plmn_id, du_cfg.tac, du_cfg.nr_cgi.nci));
  for (auto& add_plmn : du_cfg.cell_acc_rel_info.additional_plmns) {
    asn1_plmn_id_info_list[0].plmn_id_list.push_back(make_asn1_plmn_id(add_plmn));
  }

  sib1.conn_est_fail_ctrl_present                   = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_count.value = asn1::rrc_nr::conn_est_fail_ctrl_s::conn_est_fail_count_opts::n1;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_validity.value =
      conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::s30;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_present = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset         = 1;

  if (du_cfg.si_config.has_value()) {
    // Populate the SI Scheduling info list.
    if (!du_cfg.si_config->si_sched_info.empty()) {
      bool ret = asn1::number_to_enum(sib1.si_sched_info.si_win_len, du_cfg.si_config.value().si_window_len_slots);
      srsran_assert(ret, "Invalid SI window length");

      // For each SI message in the configuration...
      for (const auto& cfg_si : du_cfg.si_config->si_sched_info) {
        // Prepare a SchedulingInfo element. This holds information for an SI message carrying SIBs 2, 6, 7 or 8.
        sched_info_s asn1_si;
        asn1_si.si_broadcast_status.value = sched_info_s::si_broadcast_status_opts::broadcasting;
        ret                               = asn1::number_to_enum(asn1_si.si_periodicity, cfg_si.si_period_radio_frames);
        srsran_assert(ret, "Invalid SI period");

        // Prepare a SchedulingInfo2-r17 element. This is used for SIB-19.
        sched_info2_r17_s asn1_si_r17;
        asn1_si_r17.si_broadcast_status_r17.value = sched_info2_r17_s::si_broadcast_status_r17_opts::broadcasting;
        ret = asn1::number_to_enum(asn1_si_r17.si_periodicity_r17, cfg_si.si_period_radio_frames);
        srsran_assert(ret, "Invalid SI period");
        if (cfg_si.si_window_position.has_value()) {
          asn1_si_r17.si_win_position_r17 = cfg_si.si_window_position.value();
        }

        for (auto mapping_info : cfg_si.sib_mapping_info) {
          // For each entry in the mapping info, find the matching SIB.
          auto sib_id = static_cast<unsigned>(mapping_info);
          for (const auto& sib : du_cfg.si_config->sibs) {
            sib_type type = get_sib_info_type(sib);
            if (static_cast<std::underlying_type_t<sib_type>>(type) == sib_id) {
              switch (type) {
                case sib_type::sib2:
                case sib_type::sib6:
                case sib_type::sib7:
                case sib_type::sib8: {
                  // If the mapping info entry is for a regular SIB, append the SIB type to the schedulingInfo element.
                  sib_type_info_s type_info;
                  ret                         = asn1::number_to_enum(type_info.type, sib_id);
                  type_info.value_tag_present = true;
                  type_info.value_tag         = 0;
                  if (ret) {
                    asn1_si.sib_map_info.push_back(type_info);
                  }
                } break;
                case sib_type::sib19: {
                  // If the mapping info entry is for a release 17 SIB, append to the schedulingInfo2 element.
                  sib_type_info_v1700_s type_info2;
                  auto                  sib_id_r17 = static_cast<uint8_t>(mapping_info);
                  type_info2.sib_type_r17.set_type1_r17();
                  ret = asn1::number_to_enum(type_info2.sib_type_r17.type1_r17(), sib_id_r17);
                  if (ret) {
                    asn1_si_r17.sib_map_info_r17.push_back(type_info2);
                  }
                } break;
                case sib_type::sib1:
                case sib_type::sib_invalid:
                default:
                  srsran_assertion_failure("Invalid SIB type (i.e., {}) for an SI message", fmt::underlying(type));
              }
              break;
            }
          }
        }

        srsran_assert((asn1_si.sib_map_info.size() == 0) || (asn1_si_r17.sib_map_info_r17.size() == 0),
                      "An SI message containing release 17 SIBs cannot hold other SIB types");

        // Append the SchedulingInfo element to the SchedulingInfo list.
        if (asn1_si.sib_map_info.size() > 0) {
          sib1.si_sched_info_present = true;
          sib1.si_sched_info.sched_info_list.push_back(asn1_si);
        }

        // Append the SchedulingInfo2-r17 element to the SchedulingInfo2-r17 list.
        if (asn1_si_r17.sib_map_info_r17.size() > 0) {
          sib1.non_crit_ext_present                                               = true;
          sib1.non_crit_ext.non_crit_ext_present                                  = true;
          sib1.non_crit_ext.non_crit_ext.non_crit_ext_present                     = true;
          sib1.non_crit_ext.non_crit_ext.non_crit_ext.si_sched_info_v1700_present = true;
          sib1.non_crit_ext.non_crit_ext.non_crit_ext.cell_barred_ntn_r17_present = true;
          sib1.non_crit_ext.non_crit_ext.non_crit_ext.cell_barred_ntn_r17 =
              sib1_v1700_ies_s::cell_barred_ntn_r17_opts::not_barred;
          auto& si_sched_info_r17 = sib1.non_crit_ext.non_crit_ext.non_crit_ext.si_sched_info_v1700;
          si_sched_info_r17.sched_info_list2_r17.push_back(asn1_si_r17);
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

byte_buffer asn1_packer::pack_sib1(const du_cell_config& du_cfg, std::string* js_str)
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

static asn1::rrc_nr::sib2_s make_asn1_rrc_cell_sib2(const sib2_info& sib2_params)
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

static asn1::rrc_nr::sib6_s make_asn1_rrc_cell_sib6(const sib6_info& sib6_params)
{
  using namespace asn1::rrc_nr;
  sib6_s sib6;
  sib6.msg_id.from_number(sib6_params.message_id);
  sib6.serial_num.from_number(sib6_params.serial_number);
  sib6.warning_type.from_number(sib6_params.warning_type);
  return sib6;
}

static std::vector<uint8_t> encode_warning_message(const std::string& warning_message, unsigned data_coding_scheme)
{
  // Encode the warning message.
  std::unique_ptr<cbs_encoder> encoder                 = create_cbs_encoder();
  std::vector<uint8_t>         encoded_warning_message = encoder->encode_cb_data(warning_message, data_coding_scheme);

  return encoded_warning_message;
}

/// Maximum number of segments (see TS38.331 Section 6.3.2. Information Element \e SIB7).
static constexpr unsigned max_nof_sib_segments = 64U;

/// Generates views of each segment of the CB-Data message.
static static_vector<span<const uint8_t>, max_nof_sib_segments> segment_warning_message(std::vector<uint8_t>& cb_data)
{
  // Size of the first data segment. It must be set to a value below the SIB capacity.
  static constexpr units::bytes first_segment_size{84};
  // Size of subsequent data segments. This is used in case of multiple segments to keep the generated SIB messages
  // equal in size (recall that the SIB carrying the first segment includes the data and coding scheme field).
  static constexpr units::bytes other_segment_size{first_segment_size.value() + 1};

  unsigned nof_segments = cb_data.size() > first_segment_size.value()
                              ? divide_ceil(cb_data.size() - first_segment_size.value(), other_segment_size.value()) + 1
                              : 1;

  interval<unsigned, true> nof_segments_range(1, max_nof_sib_segments);
  report_error_if_not(nof_segments_range.contains(nof_segments),
                      "The number of required warning message segments (i.e., {}) exceeds the valid range (i.e., {}).",
                      nof_segments,
                      nof_segments_range);

  static_vector<span<const uint8_t>, max_nof_sib_segments> segment_views(nof_segments);
  span<const uint8_t>                                      cb_data_view(cb_data);

  // Add padding if required to make sure that all segments have the same length.
  if ((nof_segments > 1) && (cb_data_view.size() - first_segment_size.value()) % other_segment_size.value() != 0) {
    unsigned padding_len =
        other_segment_size.value() - ((cb_data.size() - first_segment_size.value()) % other_segment_size.value());
    cb_data.insert(cb_data.end(), padding_len, 0x00);

    // Set the CB data view to include the padding.
    cb_data_view = span<const uint8_t>(cb_data);
  }

  // Generate view for the first segment.
  segment_views[0] = span<const uint8_t>(
      cb_data_view.first(std::min(first_segment_size.value(), static_cast<unsigned>(cb_data_view.size()))));

  if (nof_segments == 1) {
    return segment_views;
  }

  // Advance past the first segment.
  cb_data_view = cb_data_view.last(cb_data.size() - first_segment_size.value());

  // Generate views for the remaining segments.
  for (unsigned i_segment = 1; i_segment != nof_segments; ++i_segment) {
    // Determine the current segment size.
    units::bytes remaining_cb_data_size(cb_data_view.size());
    units::bytes i_segment_size = std::min(other_segment_size, remaining_cb_data_size);

    // Create view to the segment.
    segment_views[i_segment] = span<const uint8_t>(cb_data_view.first(i_segment_size.value()));

    // Advance CB-Data view.
    cb_data_view = cb_data_view.last((remaining_cb_data_size - i_segment_size).value());
  }

  return segment_views;
}

static std::vector<asn1::rrc_nr::sib7_s> make_asn1_rrc_cell_sib7(const sib7_info& sib7_params)
{
  using namespace asn1::rrc_nr;

  // Encode the warning message.
  std::vector<uint8_t> encoded_message =
      encode_warning_message(sib7_params.warning_message_segment, sib7_params.data_coding_scheme);

  // Segment the message.
  auto message_segments = segment_warning_message(encoded_message);

  unsigned                          nof_segments = message_segments.size();
  std::vector<asn1::rrc_nr::sib7_s> sib_segments(nof_segments);
  span<asn1::rrc_nr::sib7_s>        sib_segments_view(sib_segments);

  // Fill the SIB carrying the first segment.
  auto& sib7_first_segment = sib_segments_view[0];
  sib7_first_segment.msg_id.from_number(sib7_params.message_id);
  sib7_first_segment.serial_num.from_number(sib7_params.serial_number);
  sib7_first_segment.warning_msg_segment_num        = 0;
  sib7_first_segment.warning_msg_segment_type.value = (nof_segments > 1)
                                                          ? sib7_s::warning_msg_segment_type_opts::not_last_segment
                                                          : sib7_s::warning_msg_segment_type_opts::last_segment;
  sib7_first_segment.data_coding_scheme_present     = true;
  sib7_first_segment.data_coding_scheme.from_number(sib7_params.data_coding_scheme);

  // Fill the warning message field with the first message segment.
  sib7_first_segment.warning_msg_segment.from_bytes(message_segments[0]);

  // Nothing left to do if no more segments are present.
  if (nof_segments == 1) {
    return sib_segments;
  }

  // Fill the remaining SIBs.
  for (unsigned i_segment = 1; i_segment != nof_segments; ++i_segment) {
    auto& i_sib                          = sib_segments_view[i_segment];
    i_sib.msg_id                         = sib7_first_segment.msg_id;
    i_sib.serial_num                     = sib7_first_segment.serial_num;
    i_sib.data_coding_scheme_present     = false;
    i_sib.warning_msg_segment_num        = i_segment;
    i_sib.warning_msg_segment_type.value = (i_segment == (nof_segments - 1))
                                               ? sib7_s::warning_msg_segment_type_opts::last_segment
                                               : sib7_s::warning_msg_segment_type_opts::not_last_segment;
    i_sib.warning_msg_segment.from_bytes(message_segments[i_segment]);
  }

  return sib_segments;
}

static std::vector<asn1::rrc_nr::sib8_s> make_asn1_rrc_cell_sib8(const sib8_info& sib8_params)
{
  using namespace asn1::rrc_nr;
  sib8_s sib8;

  // Encode the warning message into a single segment.
  std::vector<uint8_t> encoded_message =
      encode_warning_message(sib8_params.warning_message_segment, sib8_params.data_coding_scheme);

  // Segment the message.
  auto message_segments = segment_warning_message(encoded_message);

  unsigned                          nof_segments = message_segments.size();
  std::vector<asn1::rrc_nr::sib8_s> sib_segments(nof_segments);
  span<asn1::rrc_nr::sib8_s>        sib_segments_view(sib_segments);

  // Fill the SIB carrying the first segment.
  auto& sib8_first_segment = sib_segments_view[0];
  sib8_first_segment.msg_id.from_number(sib8_params.message_id);
  sib8_first_segment.serial_num.from_number(sib8_params.serial_number);
  sib8_first_segment.warning_msg_segment_num        = 0;
  sib8_first_segment.warning_msg_segment_type.value = (nof_segments > 1)
                                                          ? sib8_s::warning_msg_segment_type_opts::not_last_segment
                                                          : sib8_s::warning_msg_segment_type_opts::last_segment;
  sib8_first_segment.data_coding_scheme_present     = true;
  sib8_first_segment.data_coding_scheme.from_number(sib8_params.data_coding_scheme);

  // Fill the warning message field with the first message segment.
  sib8_first_segment.warning_msg_segment.from_bytes(message_segments[0]);

  // Nothing left to do if no more segments are present.
  if (nof_segments == 1) {
    return sib_segments;
  }

  // Create as many SIBs as segments.
  for (unsigned i_segment = 1; i_segment != nof_segments; ++i_segment) {
    auto& i_sib                          = sib_segments_view[i_segment];
    i_sib.msg_id                         = sib8_first_segment.msg_id;
    i_sib.serial_num                     = sib8_first_segment.serial_num;
    i_sib.data_coding_scheme_present     = false;
    i_sib.warning_msg_segment_num        = i_segment;
    i_sib.warning_msg_segment_type.value = (i_segment == (nof_segments - 1))
                                               ? sib8_s::warning_msg_segment_type_opts::last_segment
                                               : sib8_s::warning_msg_segment_type_opts::not_last_segment;
    i_sib.warning_msg_segment.from_bytes(message_segments[i_segment]);
  }

  return sib_segments;
}

byte_buffer asn1_packer::pack_sib19(const sib19_info& sib19_params, std::string* js_str)
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

static std::vector<asn1::rrc_nr::sys_info_ies_s::sib_type_and_info_item_c_> make_asn1_rrc_sib_item(const sib_info& sib)
{
  using namespace asn1::rrc_nr;

  std::vector<sys_info_ies_s::sib_type_and_info_item_c_> ret(1);

  switch (get_sib_info_type(sib)) {
    case sib_type::sib2: {
      const auto& cfg     = std::get<sib2_info>(sib);
      sib2_s&     out_sib = ret.front().set_sib2();
      out_sib             = make_asn1_rrc_cell_sib2(cfg);
      if (cfg.nof_ssbs_to_average.has_value()) {
        out_sib.cell_resel_info_common.nrof_ss_blocks_to_average_present = true;
        out_sib.cell_resel_info_common.nrof_ss_blocks_to_average         = cfg.nof_ssbs_to_average.value();
      }
      break;
    }
    case sib_type::sib6: {
      const auto& cfg     = std::get<sib6_info>(sib);
      sib6_s&     out_sib = ret.front().set_sib6();
      out_sib             = make_asn1_rrc_cell_sib6(cfg);
      break;
    }
    case sib_type::sib7: {
      // Generate the SIB messages (one for each warning message segment).
      const auto& cfg          = std::get<sib7_info>(sib);
      auto        sib_msgs     = make_asn1_rrc_cell_sib7(cfg);
      unsigned    nof_segments = sib_msgs.size();

      srsran_assert(nof_segments != 0, "At least one SIB message must be generated.");

      if (nof_segments == 1) {
        sib7_s& out_sib = ret.front().set_sib7();
        out_sib         = sib_msgs.front();
      } else {
        // If there are multiple segments, copy each segment to the output.
        ret.resize(nof_segments);
        for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
          sib7_s& i_sib = ret[i_segment].set_sib7();
          i_sib         = sib_msgs[i_segment];
        }
      }

      break;
    }
    case sib_type::sib8: {
      const auto& cfg          = std::get<sib8_info>(sib);
      auto        sib_msgs     = make_asn1_rrc_cell_sib8(cfg);
      unsigned    nof_segments = sib_msgs.size();

      srsran_assert(nof_segments != 0, "At least one SIB message must be generated.");

      if (nof_segments == 1) {
        sib8_s& out_sib = ret.front().set_sib8();
        out_sib         = sib_msgs.front();
      } else {
        // If there are multiple segments, copy each segment to the output.
        ret.resize(nof_segments);
        for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
          sib8_s& i_sib = ret[i_segment].set_sib8();
          i_sib         = sib_msgs[i_segment];
        }
      }

      break;
    }
    case sib_type::sib19: {
      const auto&  cfg     = std::get<sib19_info>(sib);
      sib19_r17_s& out_sib = ret.front().set_sib19_v1700();
      out_sib              = make_asn1_rrc_cell_sib19(cfg);
      break;
    }
    default:
      srsran_assertion_failure("Invalid SIB type");
  }

  return ret;
}

/// Packs an SI message into a byte buffer.
static void pack_si_message(byte_buffer& buffer, const asn1::rrc_nr::bcch_dl_sch_msg_s& msg)
{
  // Pack into the buffer.
  asn1::bit_ref     bref{buffer};
  asn1::SRSASN_CODE ret = msg.pack(bref);

  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack SI message");
}

/// Packs an SI message from the contents of a single SIB.
static void pack_si_message(bcch_dl_sch_payload_type&                                      buffer,
                            const asn1::rrc_nr::sys_info_ies_s::sib_type_and_info_item_c_& sib)
{
  // Fill the SI message with the contents of a single SIB.
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  asn1::rrc_nr::sys_info_ies_s&   si_ies = msg.msg.set_c1().set_sys_info().crit_exts.set_sys_info();
  si_ies.sib_type_and_info.push_back(sib);

  byte_buffer buf;
  pack_si_message(buf, msg);

  buffer.emplace_back(std::move(buf));
}

std::vector<bcch_dl_sch_payload_type> asn1_packer::pack_all_bcch_dl_sch_msgs(const du_cell_config& du_cfg)
{
  std::vector<bcch_dl_sch_payload_type> msgs;

  // Pack SIB1.
  {
    asn1::rrc_nr::bcch_dl_sch_msg_s msg;
    msg.msg.set_c1().set_sib_type1() = make_asn1_rrc_cell_sib1(du_cfg);

    bcch_dl_sch_payload_type packed_sib(1);
    pack_si_message(packed_sib.front(), msg);
    msgs.emplace_back(std::move(packed_sib));
  }

  // Pack SI messages.
  if (du_cfg.si_config.has_value()) {
    const auto& sibs = du_cfg.si_config.value().sibs;

    for (const auto& si_sched : du_cfg.si_config.value().si_sched_info) {
      // Pack SI messages that contain multiple SIBs.
      if (si_sched.sib_mapping_info.size() > 1) {
        asn1::rrc_nr::bcch_dl_sch_msg_s msg;
        asn1::rrc_nr::sys_info_ies_s&   si_ies = msg.msg.set_c1().set_sys_info().crit_exts.set_sys_info();

        // Search for SIBs contained in this SI message.
        for (sib_type sib_id : si_sched.sib_mapping_info) {
          report_error_if_not((sib_id != sib_type::sib7) && (sib_id != sib_type::sib8),
                              "SIB-7 and SIB-8 cannot be on an SI message containing other SIBs, as they can hold a "
                              "segmented message.");
          auto it = std::find_if(
              sibs.begin(), sibs.end(), [sib_id](const sib_info& sib) { return get_sib_info_type(sib) == sib_id; });
          srsran_assert(it != sibs.end(), "SIB{} in SIB mapping info has no defined config", (unsigned)sib_id);

          // Obtain the SIB and make sure it does not hold a segmented message.
          auto sib = make_asn1_rrc_sib_item(*it);
          srsran_assert(sib.size() == 1, "SI messages holding multiple SIBs cannot contain segmented messages.");

          si_ies.sib_type_and_info.push_back(sib.front());
        }

        // Pack SI message into a buffer.
        bcch_dl_sch_payload_type packed_sib(1);
        pack_si_message(packed_sib.front(), msg);
        msgs.emplace_back(std::move(packed_sib));
      } else {
        // Pack SI messages that hold a single SIB.
        sib_type sib_id = si_sched.sib_mapping_info.front();
        auto     it     = std::find_if(
            sibs.begin(), sibs.end(), [sib_id](const sib_info& sib) { return get_sib_info_type(sib) == sib_id; });
        srsran_assert(it != sibs.end(), "SIB{} in SIB mapping info has no defined config", (unsigned)sib_id);

        // Buffer to hold the packed message. It may be necessary to store multiple SI messages (one for each segment).
        bcch_dl_sch_payload_type packed_sib;

        // Generate the SIB message. If the SIB carries a segmented message, one SIB is generated for each segment.
        auto sib_list = make_asn1_rrc_sib_item(*it);
        // If the SIB holds multiple segments, pack each of them in a distinct SI message.
        if (sib_list.size() > 1) {
          for (auto& sib : sib_list) {
            pack_si_message(packed_sib, sib);
          }
        } else {
          // Otherwise, pack the single SIB into a buffer.
          pack_si_message(packed_sib, sib_list.front());
        }
        msgs.emplace_back(std::move(packed_sib));
      }
    }
  }

  return msgs;
}
