/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "asn1_rrc_config_helpers.h"
#include "../../asn1/asn1_diff_utils.h"
#include "asn1_csi_meas_config_helpers.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srsran::srs_du;
using namespace asn1::rrc_nr;

static asn1::rrc_nr::subcarrier_spacing_e get_asn1_scs(subcarrier_spacing scs)
{
  return asn1::rrc_nr::subcarrier_spacing_e{static_cast<asn1::rrc_nr::subcarrier_spacing_opts::options>(scs)};
}

asn1::rrc_nr::coreset_s srsran::srs_du::make_asn1_rrc_coreset(const coreset_configuration& cfg)
{
  coreset_s cs;
  cs.coreset_id = cfg.id;
  cs.freq_domain_res.from_number(cfg.freq_domain_resources().to_uint64());
  cs.dur = cfg.duration;
  if (cfg.interleaved.has_value()) {
    auto& interv = cs.cce_reg_map_type.set_interleaved();
    asn1::number_to_enum(interv.reg_bundle_size, cfg.interleaved->reg_bundle_sz);
    asn1::number_to_enum(interv.interleaver_size, cfg.interleaved->interleaver_sz);
    interv.shift_idx_present = true;
    interv.shift_idx         = cfg.interleaved->shift_index;
  } else {
    cs.cce_reg_map_type.set_non_interleaved();
  }
  cs.precoder_granularity.value =
      cfg.precoder_granurality == coreset_configuration::precoder_granularity_type::same_as_reg_bundle
          ? coreset_s::precoder_granularity_opts::same_as_reg_bundle
          : coreset_s::precoder_granularity_opts::all_contiguous_rbs;
  cs.pdcch_dmrs_scrambling_id_present = cfg.pdcch_dmrs_scrambling_id.has_value();
  if (cs.pdcch_dmrs_scrambling_id_present) {
    cs.pdcch_dmrs_scrambling_id = *cfg.pdcch_dmrs_scrambling_id;
  }
  return cs;
}

asn1::rrc_nr::search_space_s srsran::srs_du::make_asn1_rrc_search_space(const search_space_configuration& cfg)
{
  search_space_s ss;
  ss.search_space_id                                = cfg.get_id();
  ss.coreset_id_present                             = true;
  ss.coreset_id                                     = cfg.get_coreset_id();
  ss.monitoring_slot_periodicity_and_offset_present = true;
  search_space_s::monitoring_slot_periodicity_and_offset_c_::types period;
  bool success = asn1::number_to_enum(period, cfg.get_monitoring_slot_periodicity());
  srsran_assert(success, "Invalid slot period");
  ss.monitoring_slot_periodicity_and_offset.set(period);
  switch (ss.monitoring_slot_periodicity_and_offset.type().value) {
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl1:
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl2:
      ss.monitoring_slot_periodicity_and_offset.sl2() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl4:
      ss.monitoring_slot_periodicity_and_offset.sl4() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl5:
      ss.monitoring_slot_periodicity_and_offset.sl5() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl8:
      ss.monitoring_slot_periodicity_and_offset.sl8() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl10:
      ss.monitoring_slot_periodicity_and_offset.sl10() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl16:
      ss.monitoring_slot_periodicity_and_offset.sl16() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl20:
      ss.monitoring_slot_periodicity_and_offset.sl20() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl40:
      ss.monitoring_slot_periodicity_and_offset.sl40() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl80:
      ss.monitoring_slot_periodicity_and_offset.sl80() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl160:
      ss.monitoring_slot_periodicity_and_offset.sl160() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl320:
      ss.monitoring_slot_periodicity_and_offset.sl320() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl640:
      ss.monitoring_slot_periodicity_and_offset.sl640() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl1280:
      ss.monitoring_slot_periodicity_and_offset.sl1280() = cfg.get_monitoring_slot_offset();
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl2560:
      ss.monitoring_slot_periodicity_and_offset.sl2560() = cfg.get_monitoring_slot_offset();
      break;
    default:
      srsran_assertion_failure("Invalid PDCCH slot offset={}", cfg.get_monitoring_slot_offset());
  }
  if (cfg.get_duration() != 1) {
    ss.dur_present = true;
    ss.dur         = cfg.get_duration();
  }
  if (cfg.get_monitoring_symbols_within_slot().any()) {
    ss.monitoring_symbols_within_slot_present = true;
    ss.monitoring_symbols_within_slot.from_number(cfg.get_monitoring_symbols_within_slot().to_uint64());
  }
  ss.nrof_candidates_present = true;
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level1, cfg.get_nof_candidates()[0]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level2, cfg.get_nof_candidates()[1]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level4, cfg.get_nof_candidates()[2]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level8, cfg.get_nof_candidates()[3]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level16, cfg.get_nof_candidates()[4]);
  ss.search_space_type_present = true;
  if (cfg.is_common_search_space()) {
    const auto dci_fmt = variant_get<search_space_configuration::common_dci_format>(cfg.get_monitored_dci_formats());
    ss.search_space_type.set_common();
    ss.search_space_type.common().dci_format0_0_and_format1_0_present = dci_fmt.f0_0_and_f1_0;
    ss.search_space_type.common().dci_format2_0_present               = dci_fmt.f2_0;
    ss.search_space_type.common().dci_format2_1_present               = dci_fmt.f2_1;
    ss.search_space_type.common().dci_format2_2_present               = dci_fmt.f2_2;
    ss.search_space_type.common().dci_format2_3_present               = dci_fmt.f2_3;
  } else {
    const auto dci_fmt =
        variant_get<search_space_configuration::ue_specific_dci_format>(cfg.get_monitored_dci_formats());
    ss.search_space_type.set_ue_specific();
    ss.search_space_type.ue_specific().dci_formats.value =
        dci_fmt == srsran::search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0
            ? search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_neg0_and_neg1_neg0
            : search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_neg1_and_neg1_neg1;
  }
  return ss;
}

rlc_bearer_cfg_s make_asn1_rrc_rlc_bearer(const rlc_bearer_config& cfg)
{
  rlc_bearer_cfg_s out;

  out.lc_ch_id = cfg.lcid;

  out.served_radio_bearer_present = true;
  if (is_srb(cfg.lcid)) {
    out.served_radio_bearer.set_srb_id() = srb_id_to_uint(to_srb_id(cfg.lcid));
  } else {
    out.served_radio_bearer.set_drb_id() = drb_id_to_uint(*cfg.drb_id);
  }

  out.rlc_cfg_present = true;
  switch (cfg.rlc_cfg.mode) {
    case rlc_mode::am: {
      rlc_cfg_c::am_s_& am              = out.rlc_cfg.set_am();
      am.ul_am_rlc.sn_field_len_present = true;
      asn1::number_to_enum(am.ul_am_rlc.sn_field_len, to_number(cfg.rlc_cfg.am.tx.sn_field_length));
      asn1::number_to_enum(am.ul_am_rlc.t_poll_retx, cfg.rlc_cfg.am.tx.t_poll_retx);
      asn1::number_to_enum(am.ul_am_rlc.poll_pdu, cfg.rlc_cfg.am.tx.poll_pdu);
      asn1::number_to_enum(am.ul_am_rlc.poll_byte, cfg.rlc_cfg.am.tx.poll_byte);
      asn1::number_to_enum(am.ul_am_rlc.max_retx_thres, cfg.rlc_cfg.am.tx.max_retx_thresh);

      am.dl_am_rlc.sn_field_len_present = true;
      asn1::number_to_enum(am.dl_am_rlc.sn_field_len, to_number(cfg.rlc_cfg.am.rx.sn_field_length));
      asn1::number_to_enum(am.dl_am_rlc.t_reassembly, cfg.rlc_cfg.am.rx.t_reassembly);
      asn1::number_to_enum(am.dl_am_rlc.t_status_prohibit, cfg.rlc_cfg.am.rx.t_status_prohibit);
    } break;
    case rlc_mode::um_bidir: {
      auto& um                          = out.rlc_cfg.set_um_bi_dir();
      um.ul_um_rlc.sn_field_len_present = true;
      asn1::number_to_enum(um.ul_um_rlc.sn_field_len, to_number(cfg.rlc_cfg.um.tx.sn_field_length));
      um.dl_um_rlc.sn_field_len_present = true;
      asn1::number_to_enum(um.dl_um_rlc.sn_field_len, to_number(cfg.rlc_cfg.um.rx.sn_field_length));
      asn1::number_to_enum(um.dl_um_rlc.t_reassembly, cfg.rlc_cfg.um.rx.t_reassembly);
    } break;
    case rlc_mode::um_unidir_dl: {
      auto& um                          = out.rlc_cfg.set_um_uni_dir_dl();
      um.dl_um_rlc.sn_field_len_present = true;
      asn1::number_to_enum(um.dl_um_rlc.sn_field_len, to_number(cfg.rlc_cfg.um.rx.sn_field_length));
      asn1::number_to_enum(um.dl_um_rlc.t_reassembly, um.dl_um_rlc.t_reassembly);
    } break;
    case rlc_mode::um_unidir_ul: {
      auto& um                          = out.rlc_cfg.set_um_uni_dir_ul();
      um.ul_um_rlc.sn_field_len_present = true;
      asn1::number_to_enum(um.ul_um_rlc.sn_field_len, to_number(cfg.rlc_cfg.um.tx.sn_field_length));
    } break;
    default:
      report_fatal_error("Invalid RLC bearer configuration type");
  }

  out.mac_lc_ch_cfg_present                    = true;
  out.mac_lc_ch_cfg.ul_specific_params_present = true;
  out.mac_lc_ch_cfg.ul_specific_params.prio    = is_srb(cfg.lcid) ? 1 : 2;
  out.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate.value =
      lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::infinity;
  out.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur.value =
      lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms5;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present          = true;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group                  = is_srb(cfg.lcid) ? 0 : 1;
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present     = true;
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id             = 0;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_mask                = false;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_delay_timer_applied = false;

  return out;
}

asn1::dyn_array<scs_specific_carrier_s>
srsran::srs_du::make_asn1_rrc_scs_specific_carrier_list(span<const scs_specific_carrier> scs_carrier_list)
{
  asn1::dyn_array<scs_specific_carrier_s> list(scs_carrier_list.size());
  for (unsigned i = 0; i < scs_carrier_list.size(); ++i) {
    list[i].offset_to_carrier                  = scs_carrier_list[i].offset_to_carrier;
    list[i].subcarrier_spacing.value           = get_asn1_scs(scs_carrier_list[i].scs);
    list[i].carrier_bw                         = scs_carrier_list[i].carrier_bandwidth;
    list[i].tx_direct_current_location_present = scs_carrier_list[i].tx_direct_current_location.has_value();
    list[i].ext                                = list[i].tx_direct_current_location_present;
    if (list[i].tx_direct_current_location_present) {
      list[i].tx_direct_current_location = scs_carrier_list[i].tx_direct_current_location.value();
    }
  }
  return list;
}

asn1::rrc_nr::bwp_dl_common_s srsran::srs_du::make_asn1_init_dl_bwp(const dl_config_common& cfg)
{
  asn1::rrc_nr::bwp_dl_common_s init_dl_bwp;

  // > generic params
  init_dl_bwp.generic_params.cp_present               = cfg.init_dl_bwp.generic_params.cp == cyclic_prefix::EXTENDED;
  init_dl_bwp.generic_params.subcarrier_spacing.value = get_asn1_scs(cfg.init_dl_bwp.generic_params.scs);
  // See TS 38.331, BWP.locationAndBandwidth and TS 38.213 clause 12.
  init_dl_bwp.generic_params.location_and_bw =
      sliv_from_s_and_l(275, cfg.init_dl_bwp.generic_params.crbs.start(), cfg.init_dl_bwp.generic_params.crbs.length());

  // > PDCCH-ConfigCommon.
  init_dl_bwp.pdcch_cfg_common_present = true;
  pdcch_cfg_common_s& pdcch            = init_dl_bwp.pdcch_cfg_common.set_setup();
  // Sent by MIB.
  pdcch.coreset_zero_present   = false;
  pdcch.common_coreset_present = cfg.init_dl_bwp.pdcch_common.common_coreset.has_value();
  if (pdcch.common_coreset_present) {
    pdcch.common_coreset = srsran::srs_du::make_asn1_rrc_coreset(cfg.init_dl_bwp.pdcch_common.common_coreset.value());
  }
  // Sent by MIB.
  pdcch.search_space_zero_present = false;
  for (size_t ss_idx = 1; ss_idx < cfg.init_dl_bwp.pdcch_common.search_spaces.size(); ++ss_idx) {
    const search_space_configuration& ss = cfg.init_dl_bwp.pdcch_common.search_spaces[ss_idx];
    pdcch.common_search_space_list.push_back(srsran::srs_du::make_asn1_rrc_search_space(ss));
  }
  pdcch.search_space_sib1_present           = true;
  pdcch.search_space_sib1                   = cfg.init_dl_bwp.pdcch_common.sib1_search_space_id;
  pdcch.search_space_other_sys_info_present = false;
  pdcch.paging_search_space_present         = cfg.init_dl_bwp.pdcch_common.paging_search_space_id.has_value();
  if (pdcch.paging_search_space_present) {
    pdcch.paging_search_space = cfg.init_dl_bwp.pdcch_common.paging_search_space_id.value();
  }
  pdcch.ra_search_space_present = true;
  pdcch.ra_search_space         = (unsigned)cfg.init_dl_bwp.pdcch_common.ra_search_space_id;

  // > PDSCH-ConfigCommon.
  init_dl_bwp.pdsch_cfg_common_present = true;
  pdsch_cfg_common_s& pdsch            = init_dl_bwp.pdsch_cfg_common.set_setup();
  pdsch.pdsch_time_domain_alloc_list.resize(cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.size());
  for (unsigned i = 0; i != pdsch.pdsch_time_domain_alloc_list.size(); ++i) {
    pdsch.pdsch_time_domain_alloc_list[i].k0_present = cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].k0 != 0;
    if (pdsch.pdsch_time_domain_alloc_list[i].k0_present) {
      pdsch.pdsch_time_domain_alloc_list[i].k0 = cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].k0;
    }
    pdsch.pdsch_time_domain_alloc_list[i].map_type.value =
        cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].map_type == sch_mapping_type::typeA
            ? pdsch_time_domain_res_alloc_s::map_type_opts::type_a
            : pdsch_time_domain_res_alloc_s::map_type_opts::type_b;
    pdsch.pdsch_time_domain_alloc_list[i].start_symbol_and_len =
        sliv_from_s_and_l(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                          cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].symbols.start(),
                          cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[i].symbols.length());
  }

  return init_dl_bwp;
}

static asn1::rrc_nr::dl_cfg_common_s make_asn1_rrc_dl_cfg_common(const du_cell_config& cfg)
{
  using namespace asn1::rrc_nr;
  dl_cfg_common_s out;

  // > frequencyInfoDL   FrequencyInfoDL   OPTIONAL   -- Cond InterFreqHOAndServCellAdd
  out.freq_info_dl_present = true;
  for (const auto& dl_band : cfg.dl_cfg_common.freq_info_dl.freq_band_list) {
    out.freq_info_dl.freq_band_list.push_back(nr_band_to_uint(dl_band.band));
  }
  out.freq_info_dl.absolute_freq_ssb_present = true;
  // TODO: Check how to derive this value.
  out.freq_info_dl.absolute_freq_ssb = cfg.dl_cfg_common.freq_info_dl.absolute_frequency_ssb;
  // TODO: Check how to derive absoluteFreqPointA.
  out.freq_info_dl.absolute_freq_point_a = cfg.dl_cfg_common.freq_info_dl.absolute_freq_point_a;
  out.freq_info_dl.scs_specific_carrier_list =
      make_asn1_rrc_scs_specific_carrier_list(cfg.dl_cfg_common.freq_info_dl.scs_carrier_list);

  // > initialDownlinkBWP    BWP-DownlinkCommon    OPTIONAL   -- Cond ServCellAdd
  out.init_dl_bwp_present = true;
  out.init_dl_bwp         = make_asn1_init_dl_bwp(cfg.dl_cfg_common);

  return out;
}

// Helper function that converts msg1-fdm rach parameter into asn1 type.
static asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::options rach_msg1_fdm_convert_to_asn1(unsigned msg1_fdm_value)
{
  switch (msg1_fdm_value) {
    case 1:
      return asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::one;
    case 2:
      return asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::two;
    case 4:
      return asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::four;
    case 8:
      return asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::eight;
    default:
      report_fatal_error("Invalid msg1-fdm field. Return default value 1");
  }
  return asn1::rrc_nr::rach_cfg_generic_s::msg1_fdm_opts::one;
}

static asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::options
pucch_group_hop_convert_to_asn1(pucch_group_hopping group_hop_value)
{
  switch (group_hop_value) {
    case pucch_group_hopping::DISABLE:
      return asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::disable;
    case pucch_group_hopping::ENABLE:
      return asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::enable;
    case pucch_group_hopping::NEITHER:
      return asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::neither;
    default:
      report_fatal_error("Invalid msg1-fdm field. Return default value 1");
  }
  return asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::disable;
}

asn1::rrc_nr::bwp_ul_common_s srsran::srs_du::make_asn1_rrc_initial_up_bwp(const ul_config_common& cfg)
{
  asn1::rrc_nr::bwp_ul_common_s init_ul_bwp;

  // > genericParameters BWP
  init_ul_bwp.generic_params.subcarrier_spacing.value = get_asn1_scs(cfg.init_ul_bwp.generic_params.scs);
  init_ul_bwp.generic_params.location_and_bw =
      sliv_from_s_and_l(275, cfg.init_ul_bwp.generic_params.crbs.start(), cfg.init_ul_bwp.generic_params.crbs.length());

  // > rach-ConfigCommon SetupRelease { RACH-ConfigCommon } OPTIONAL, -- Need M
  const rach_config_common& rach_cfg    = *cfg.init_ul_bwp.rach_cfg_common;
  init_ul_bwp.rach_cfg_common_present   = true;
  rach_cfg_common_s& rach               = init_ul_bwp.rach_cfg_common.set_setup();
  rach.rach_cfg_generic.prach_cfg_idx   = rach_cfg.rach_cfg_generic.prach_config_index;
  rach.rach_cfg_generic.msg1_fdm.value  = rach_msg1_fdm_convert_to_asn1(rach_cfg.rach_cfg_generic.msg1_fdm);
  rach.rach_cfg_generic.msg1_freq_start = static_cast<uint16_t>(rach_cfg.rach_cfg_generic.msg1_frequency_start);
  rach.rach_cfg_generic.zero_correlation_zone_cfg =
      static_cast<uint8_t>(rach_cfg.rach_cfg_generic.zero_correlation_zone_config);
  rach.rach_cfg_generic.preamb_rx_target_pwr = rach_cfg.rach_cfg_generic.preamble_rx_target_pw.to_int();
  switch (rach_cfg.rach_cfg_generic.preamble_trans_max) {
    case 3:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n3;
      break;
    case 4:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n4;
      break;
    case 5:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n5;
      break;
    case 6:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n6;
      break;
    case 7:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7;
      break;
    case 8:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n8;
      break;
    case 10:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n10;
      break;
    case 20:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n20;
      break;
    case 50:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n50;
      break;
    case 100:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n100;
      break;
    case 200:
      rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n200;
      break;
    default:
      report_fatal_error("Invalid preamble transmission max value");
  }
  switch (rach_cfg.rach_cfg_generic.power_ramping_step_db) {
    case 0:
      rach.rach_cfg_generic.pwr_ramp_step.value = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db0;
      break;
    case 2:
      rach.rach_cfg_generic.pwr_ramp_step.value = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db2;
      break;
    case 4:
      rach.rach_cfg_generic.pwr_ramp_step.value = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4;
      break;
    case 6:
      rach.rach_cfg_generic.pwr_ramp_step.value = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db6;
      break;
    default:
      report_fatal_error("Invalid power ramping step value");
  }

  bool success = asn1::number_to_enum(rach.rach_cfg_generic.ra_resp_win, rach_cfg.rach_cfg_generic.ra_resp_window);
  srsran_assert(success, "Invalid ra-WindowSize");
  if (rach_cfg.total_nof_ra_preambles.has_value()) {
    rach.total_nof_ra_preambs_present = true;
    rach.total_nof_ra_preambs         = rach_cfg.total_nof_ra_preambles.value();
    rach.total_nof_ra_preambs -= 1; // Account for zero-indexed ASN field.
  }
  rach.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  rach.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one().value =
      asn1::rrc_nr::rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_opts::n4;
  rach.ra_contention_resolution_timer.value =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  if (rach_cfg.is_prach_root_seq_index_l839) {
    rach.prach_root_seq_idx.set_l839() = rach_cfg.prach_root_seq_index;
  } else {
    rach.prach_root_seq_idx.set_l139() = rach_cfg.prach_root_seq_index;
  }
  if (rach_cfg.msg1_scs != subcarrier_spacing::invalid) {
    rach.msg1_subcarrier_spacing_present = true;
    rach.msg1_subcarrier_spacing         = get_asn1_scs(rach_cfg.msg1_scs);
  }
  switch (rach_cfg.restricted_set) {
    case restricted_set_config::UNRESTRICTED:
      rach.restricted_set_cfg.value = rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;
      break;
    case restricted_set_config::TYPE_A:
      rach.restricted_set_cfg.value = rach_cfg_common_s::restricted_set_cfg_opts::restricted_set_type_a;
      break;
    case restricted_set_config::TYPE_B:
      rach.restricted_set_cfg.value = rach_cfg_common_s::restricted_set_cfg_opts::restricted_set_type_b;
      break;
    default:
      report_fatal_error("Invalid restricted set");
  }

  // pusch-ConfigCommon SetupRelease { PUSCH-ConfigCommon } OPTIONAL, -- Need M
  const pusch_config_common& pusch_cfg = cfg.init_ul_bwp.pusch_cfg_common.value();
  init_ul_bwp.pusch_cfg_common_present = true;
  pusch_cfg_common_s& pusch            = init_ul_bwp.pusch_cfg_common.set_setup();
  pusch.pusch_time_domain_alloc_list.resize(pusch_cfg.pusch_td_alloc_list.size());
  for (unsigned i = 0; i < pusch_cfg.pusch_td_alloc_list.size(); ++i) {
    pusch.pusch_time_domain_alloc_list[i].k2_present = true;
    pusch.pusch_time_domain_alloc_list[i].k2         = pusch_cfg.pusch_td_alloc_list[i].k2;
    pusch.pusch_time_domain_alloc_list[i].map_type.value =
        pusch_cfg.pusch_td_alloc_list[i].map_type == sch_mapping_type::typeA
            ? pusch_time_domain_res_alloc_s::map_type_opts::type_a
            : pusch_time_domain_res_alloc_s::map_type_opts::type_b;
    pusch.pusch_time_domain_alloc_list[i].start_symbol_and_len =
        sliv_from_s_and_l(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP,
                          pusch_cfg.pusch_td_alloc_list[i].symbols.start(),
                          pusch_cfg.pusch_td_alloc_list[i].symbols.length());
  }
  pusch.msg3_delta_preamb_present     = true;
  pusch.msg3_delta_preamb             = pusch_cfg.msg3_delta_preamble.to_int();
  pusch.p0_nominal_with_grant_present = true;
  pusch.p0_nominal_with_grant         = pusch_cfg.p0_nominal_with_grant.to_int();

  // pucch-ConfigCommon SetupRelease { PUCCH-ConfigCommon } OPTIONAL, -- Need M
  const pucch_config_common& pucch_cfg = cfg.init_ul_bwp.pucch_cfg_common.value();
  init_ul_bwp.pucch_cfg_common_present = true;
  pucch_cfg_common_s& pucch            = init_ul_bwp.pucch_cfg_common.set_setup();
  pucch.pucch_res_common_present       = true;
  pucch.pucch_res_common               = pucch_cfg.pucch_resource_common;
  pucch.pucch_group_hop.value          = pucch_group_hop_convert_to_asn1(pucch_cfg.group_hopping);
  pucch.p0_nominal_present             = true;
  pucch.p0_nominal                     = pucch_cfg.p0_nominal;
  if (pucch_cfg.hopping_id.has_value()) {
    pucch.hop_id_present = true;
    pucch.hop_id         = static_cast<uint16_t>(pucch_cfg.hopping_id.value());
  } else {
    pucch.hop_id_present = false;
  }

  return init_ul_bwp;
}

static asn1::rrc_nr::ul_cfg_common_s make_asn1_rrc_ul_cfg_common(const ul_config_common& cfg)
{
  using namespace asn1::rrc_nr;
  ul_cfg_common_s out;

  // > frequencyInfoUL FrequencyInfoUL OPTIONAL, -- Cond InterFreqHOAndServCellAdd
  out.freq_info_ul_present = true;
  for (const auto& ul_band : cfg.freq_info_ul.freq_band_list) {
    out.freq_info_ul.freq_band_list.push_back(nr_band_to_uint(ul_band.band));
  }
  out.freq_info_ul.absolute_freq_point_a_present = true;
  out.freq_info_ul.absolute_freq_point_a         = cfg.freq_info_ul.absolute_freq_point_a;
  if (cfg.freq_info_ul.p_max.has_value()) {
    out.freq_info_ul.p_max_present = true;
    out.freq_info_ul.p_max         = cfg.freq_info_ul.p_max->value();
  }
  out.freq_info_ul.scs_specific_carrier_list =
      make_asn1_rrc_scs_specific_carrier_list(cfg.freq_info_ul.scs_carrier_list);

  // > initialUplinkBWP BWP-UplinkCommon OPTIONAL, -- Cond ServCellAdd
  out.init_ul_bwp_present = true;
  out.init_ul_bwp         = make_asn1_rrc_initial_up_bwp(cfg);

  out.dummy.value = time_align_timer_opts::infinity;

  return out;
}

static asn1::rrc_nr::tdd_ul_dl_pattern_s make_asn1_rrc_tdd_ul_dl_pattern(subcarrier_spacing       ref_scs,
                                                                         const tdd_ul_dl_pattern& pattern)
{
  static constexpr std::array<float, 8> basic_periods = {0.5, 0.625, 1.0, 1.25, 2.0, 2.5, 5.0, 10.0};
  static constexpr std::array<float, 2> ext_periods   = {3.0, 4.0};

  asn1::rrc_nr::tdd_ul_dl_pattern_s out;

  out.nrof_dl_slots   = pattern.nof_dl_slots;
  out.nrof_ul_slots   = pattern.nof_ul_slots;
  out.nrof_dl_symbols = pattern.nof_dl_symbols;
  out.nrof_ul_symbols = pattern.nof_ul_symbols;

  // Set period in ms.
  const float periodicity_ms =
      static_cast<float>(pattern.dl_ul_tx_period_nof_slots) / static_cast<float>(get_nof_slots_per_subframe(ref_scs));
  auto same_period_func = [periodicity_ms](float v) { return std::abs(v - periodicity_ms) < 0.001F; };
  auto it               = std::find_if(basic_periods.begin(), basic_periods.end(), same_period_func);
  if (it != basic_periods.end()) {
    out.dl_ul_tx_periodicity.value =
        (asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::options)std::distance(basic_periods.begin(), it);
  } else {
    // If TDD period is part of the v1530 extension.
    it = std::find_if(ext_periods.begin(), ext_periods.end(), same_period_func);
    if (it != ext_periods.end()) {
      // The non-ext period will be ignored as per TS 38.331, if the extension is enabled.
      out.dl_ul_tx_periodicity.value         = asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms0p5;
      out.ext                                = true;
      out.dl_ul_tx_periodicity_v1530_present = true;
      out.dl_ul_tx_periodicity_v1530.value =
          (asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_opts::options)(
              std::distance(ext_periods.begin(), it));
    } else {
      report_fatal_error("Unsupported TDD UL/DL periodicity {}ms", periodicity_ms);
    }
  }

  return out;
}

asn1::rrc_nr::tdd_ul_dl_cfg_common_s
srsran::srs_du::make_asn1_rrc_tdd_ul_dl_cfg_common(const tdd_ul_dl_config_common& tdd_cfg)
{
  asn1::rrc_nr::tdd_ul_dl_cfg_common_s out;

  out.ref_subcarrier_spacing = get_asn1_scs(tdd_cfg.ref_scs);

  out.pattern1 = make_asn1_rrc_tdd_ul_dl_pattern(tdd_cfg.ref_scs, tdd_cfg.pattern1);

  if (tdd_cfg.pattern2.has_value()) {
    out.pattern2_present = true;
    out.pattern2         = make_asn1_rrc_tdd_ul_dl_pattern(tdd_cfg.ref_scs, *tdd_cfg.pattern2);
  }

  return out;
}

void calculate_pdcch_config_diff(asn1::rrc_nr::pdcch_cfg_s& out, const pdcch_config& src, const pdcch_config& dest)
{
  calculate_addmodremlist_diff(
      out.coreset_to_add_mod_list,
      out.coreset_to_release_list,
      src.coresets,
      dest.coresets,
      [](const coreset_configuration& cs_cfg) { return make_asn1_rrc_coreset(cs_cfg); },
      [](const coreset_configuration& cs_cfg) { return (uint8_t)cs_cfg.id; });

  calculate_addmodremlist_diff(
      out.search_spaces_to_add_mod_list,
      out.search_spaces_to_release_list,
      src.search_spaces,
      dest.search_spaces,
      [](const search_space_configuration& ss) { return make_asn1_rrc_search_space(ss); },
      [](const search_space_configuration& ss) { return (uint8_t)ss.get_id(); });

  // TODO: Remaining.
}

void make_asn1_rrc_dmrs_dl_for_pdsch(asn1::rrc_nr::dmrs_dl_cfg_s& out, const dmrs_downlink_config& cfg)
{
  if (cfg.is_dmrs_type2) {
    out.dmrs_type_present = true;
  }

  if (cfg.additional_positions != srsran::dmrs_additional_positions::pos2) {
    out.dmrs_add_position_present = true;
    switch (cfg.additional_positions) {
      case dmrs_additional_positions::pos0:
        out.dmrs_add_position = dmrs_dl_cfg_s::dmrs_add_position_opts::pos0;
        break;
      case dmrs_additional_positions::pos1:
        out.dmrs_add_position = dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;
        break;
      case dmrs_additional_positions::pos3:
        out.dmrs_add_position = dmrs_dl_cfg_s::dmrs_add_position_opts::pos3;
        break;
      default:
        srsran_assertion_failure("Invalid DMRS DL Add. Position={}", cfg.additional_positions);
    }
  }

  if (cfg.is_max_length_len2) {
    out.max_len_present = true;
  }

  if (cfg.scrambling_id0.has_value()) {
    out.scrambling_id0_present = true;
    out.scrambling_id0         = cfg.scrambling_id0.value();
  }

  if (cfg.scrambling_id1.has_value()) {
    out.scrambling_id1_present = true;
    out.scrambling_id1         = cfg.scrambling_id1.value();
  }
}

void make_asn1_rrc_qcl_info(asn1::rrc_nr::qcl_info_s& out, const qcl_info& cfg)
{
  if (cfg.cell.has_value()) {
    out.cell_present = true;
    out.cell         = cfg.cell.value();
  }
  if (cfg.bwp_id.has_value()) {
    out.bwp_id_present = true;
    out.bwp_id         = cfg.bwp_id.value();
  }
  if (cfg.ref_sig.type == srsran::qcl_info::reference_signal::reference_signal_type::ssb) {
    auto& ssb = out.ref_sig.set_ssb();
    ssb       = cfg.ref_sig.ssb;
  } else if (cfg.ref_sig.type == srsran::qcl_info::reference_signal::reference_signal_type::csi_rs) {
    auto& csi_rs = out.ref_sig.set_csi_rs();
    csi_rs       = cfg.ref_sig.csi_rs;
  }
  switch (cfg.qcl_type) {
    case qcl_info::qcl_type::type_a:
      out.qcl_type = qcl_info_s::qcl_type_opts::type_a;
      break;
    case qcl_info::qcl_type::type_b:
      out.qcl_type = qcl_info_s::qcl_type_opts::type_b;
      break;
    case qcl_info::qcl_type::type_c:
      out.qcl_type = qcl_info_s::qcl_type_opts::type_c;
      break;
    case qcl_info::qcl_type::type_d:
      out.qcl_type = qcl_info_s::qcl_type_opts::type_d;
      break;
    default:
      srsran_assertion_failure("Invalid QCL Type={}", cfg.qcl_type);
  }
}

asn1::rrc_nr::pdsch_time_domain_res_alloc_s
make_asn1_rrc_pdsch_time_domain_alloc_list(const pdsch_time_domain_resource_allocation& cfg)
{
  pdsch_time_domain_res_alloc_s out{};
  out.k0_present = true;
  out.k0         = cfg.k0;
  switch (cfg.map_type) {
    case sch_mapping_type::typeA:
      out.map_type = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;
      break;
    case sch_mapping_type::typeB:
      out.map_type = pdsch_time_domain_res_alloc_s::map_type_opts::type_b;
      break;
    default:
      srsran_assertion_failure("Invalid SCH mapping Type={}", cfg.map_type);
  }

  out.start_symbol_and_len = ofdm_symbol_range_to_sliv(cfg.symbols);

  return out;
}

asn1::rrc_nr::tci_state_s srsran::srs_du::make_asn1_rrc_tci_state(const tci_state& cfg)
{
  tci_state_s tci_st;
  tci_st.tci_state_id = cfg.state_id;
  make_asn1_rrc_qcl_info(tci_st.qcl_type1, cfg.qcl_type1);
  if (cfg.qcl_type2.has_value()) {
    tci_st.qcl_type2_present = true;
    make_asn1_rrc_qcl_info(tci_st.qcl_type2, cfg.qcl_type2.value());
  }

  return tci_st;
}

void calculate_pdsch_config_diff(asn1::rrc_nr::pdsch_cfg_s& out, const pdsch_config& src, const pdsch_config& dest)
{
  out.data_scrambling_id_pdsch_present = dest.data_scrambling_id_pdsch.has_value();
  if (out.data_scrambling_id_pdsch_present) {
    out.data_scrambling_id_pdsch = dest.data_scrambling_id_pdsch.value();
  }

  // DMRS Type A.
  if ((dest.pdsch_mapping_type_a_dmrs.has_value() && not src.pdsch_mapping_type_a_dmrs.has_value()) ||
      (dest.pdsch_mapping_type_a_dmrs.has_value() && src.pdsch_mapping_type_a_dmrs.has_value() &&
       dest.pdsch_mapping_type_a_dmrs != src.pdsch_mapping_type_a_dmrs)) {
    out.dmrs_dl_for_pdsch_map_type_a_present = true;
    make_asn1_rrc_dmrs_dl_for_pdsch(out.dmrs_dl_for_pdsch_map_type_a.set_setup(),
                                    dest.pdsch_mapping_type_a_dmrs.value());
  } else if (src.pdsch_mapping_type_a_dmrs.has_value() && not dest.pdsch_mapping_type_a_dmrs.has_value()) {
    out.dmrs_dl_for_pdsch_map_type_a_present = true;
    out.dmrs_dl_for_pdsch_map_type_a.set_release();
  }

  // DMRS Type B.
  if ((dest.pdsch_mapping_type_b_dmrs.has_value() && not src.pdsch_mapping_type_b_dmrs.has_value()) ||
      (dest.pdsch_mapping_type_b_dmrs.has_value() && src.pdsch_mapping_type_b_dmrs.has_value() &&
       dest.pdsch_mapping_type_b_dmrs != src.pdsch_mapping_type_b_dmrs)) {
    out.dmrs_dl_for_pdsch_map_type_b_present = true;
    make_asn1_rrc_dmrs_dl_for_pdsch(out.dmrs_dl_for_pdsch_map_type_b.set_setup(),
                                    dest.pdsch_mapping_type_b_dmrs.value());
  } else if (src.pdsch_mapping_type_b_dmrs.has_value() && not dest.pdsch_mapping_type_b_dmrs.has_value()) {
    out.dmrs_dl_for_pdsch_map_type_b_present = true;
    out.dmrs_dl_for_pdsch_map_type_b.set_release();
  }

  // TCI states.
  calculate_addmodremlist_diff(
      out.tci_states_to_add_mod_list,
      out.tci_states_to_release_list,
      src.tci_states,
      dest.tci_states,
      [](const tci_state& st) { return make_asn1_rrc_tci_state(st); },
      [](const tci_state& st) { return st.state_id; });

  // VRB-to-PRB Interleaver.
  if (dest.vrb_to_prb_itlvr.has_value()) {
    out.vrb_to_prb_interleaver_present = true;
    switch (dest.vrb_to_prb_itlvr.value()) {
      case pdsch_config::vrb_to_prb_interleaver::n2:
        out.vrb_to_prb_interleaver = pdsch_cfg_s::vrb_to_prb_interleaver_opts::n2;
        break;
      case pdsch_config::vrb_to_prb_interleaver::n4:
        out.vrb_to_prb_interleaver = pdsch_cfg_s::vrb_to_prb_interleaver_opts::n4;
        break;
      default:
        srsran_assertion_failure("Invalid VRB-to-PRB Interleaver={}", dest.vrb_to_prb_itlvr.value());
    }
  }

  // Resource Allocation type.
  switch (dest.res_alloc) {
    case pdsch_config::resource_allocation::resource_allocation_type_0:
      out.res_alloc = pdsch_cfg_s::res_alloc_opts::res_alloc_type0;
      break;
    case pdsch_config::resource_allocation::resource_allocation_type_1:
      out.res_alloc = pdsch_cfg_s::res_alloc_opts::res_alloc_type1;
      break;
    case pdsch_config::resource_allocation::dynamic_switch:
      out.res_alloc = pdsch_cfg_s::res_alloc_opts::dyn_switch;
      break;
    default:
      srsran_assertion_failure("Invalid resource allocation type={}", dest.res_alloc);
  }

  // PDSCH Time Domain Allocation.
  if ((not dest.pdsch_td_alloc_list.empty() && src.pdsch_td_alloc_list.empty()) ||
      (not dest.pdsch_td_alloc_list.empty() && not src.pdsch_td_alloc_list.empty() &&
       dest.pdsch_td_alloc_list != src.pdsch_td_alloc_list)) {
    out.pdsch_time_domain_alloc_list_present = true;
    auto& alloc_list                         = out.pdsch_time_domain_alloc_list.set_setup();
    for (const auto& td_alloc : dest.pdsch_td_alloc_list) {
      alloc_list.push_back(make_asn1_rrc_pdsch_time_domain_alloc_list(td_alloc));
    }
  } else if (not src.pdsch_td_alloc_list.empty() && dest.pdsch_td_alloc_list.empty()) {
    out.pdsch_time_domain_alloc_list_present = true;
    out.pdsch_time_domain_alloc_list.set_release();
  }

  // RBG Size.
  switch (dest.rbg_sz) {
    case rbg_size::config1:
      out.rbg_size = pdsch_cfg_s::rbg_size_opts::cfg1;
      break;
    case rbg_size::config2:
      out.rbg_size = pdsch_cfg_s::rbg_size_opts::cfg2;
      break;
    default:
      srsran_assertion_failure("Invalid RBG size={}", dest.rbg_sz);
  }

  // PRB Bundling.
  if (variant_holds_alternative<prb_bundling::static_bundling>(dest.prb_bndlg.bundling)) {
    auto& st_bundling               = out.prb_bundling_type.set_static_bundling();
    st_bundling.bundle_size_present = true;
    const auto& bdlng               = variant_get<prb_bundling::static_bundling>(dest.prb_bndlg.bundling);
    switch (bdlng.sz.value()) {
      case prb_bundling::static_bundling::bundling_size::n4:
        st_bundling.bundle_size = pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::n4;
        break;
      case prb_bundling::static_bundling::bundling_size::wideband:
        st_bundling.bundle_size = pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::wideband;
        break;
      default:
        srsran_assertion_failure("Invalid static PRB bundling size={}", bdlng.sz.value());
    }
  } else {
    // Dynamic bundling.
    auto& dy_bundling                    = out.prb_bundling_type.set_dyn_bundling();
    dy_bundling.bundle_size_set1_present = true;
    const auto& bdlng                    = variant_get<prb_bundling::dynamic_bundling>(dest.prb_bndlg.bundling);
    switch (bdlng.sz_set1.value()) {
      case prb_bundling::dynamic_bundling::bundling_size_set1::n4:
        dy_bundling.bundle_size_set1 = pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set1_opts::n4;
        break;
      case prb_bundling::dynamic_bundling::bundling_size_set1::wideband:
        dy_bundling.bundle_size_set1 =
            pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set1_opts::wideband;
        break;
      case prb_bundling::dynamic_bundling::bundling_size_set1::n2_wideband:
        dy_bundling.bundle_size_set1 =
            pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set1_opts::n2_wideband;
        break;
      case prb_bundling::dynamic_bundling::bundling_size_set1::n4_wideband:
        dy_bundling.bundle_size_set1 =
            pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set1_opts::n4_wideband;
        break;
      default:
        srsran_assertion_failure("Invalid dynamic PRB bundling set 1 size={}", bdlng.sz_set1.value());
    }
    dy_bundling.bundle_size_set2_present = true;
    switch (bdlng.sz_set2.value()) {
      case prb_bundling::dynamic_bundling::bundling_size_set2::n4:
        dy_bundling.bundle_size_set2 = pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set2_opts::n4;
        break;
      case prb_bundling::dynamic_bundling::bundling_size_set2::wideband:
        dy_bundling.bundle_size_set2 =
            pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set2_opts::wideband;
        break;
      default:
        srsran_assertion_failure("Invalid dynamic PRB bundling set 2 size={}", bdlng.sz_set2.value());
    }
  }

  if (dest.mcs_table != pdsch_mcs_table::qam64) {
    out.mcs_table_present = true;
    switch (dest.mcs_table) {
      case pdsch_mcs_table::qam64LowSe:
        out.mcs_table.value = pdsch_cfg_s::mcs_table_opts::qam64_low_se;
        break;
      case pdsch_mcs_table::qam256:
        out.mcs_table.value = pdsch_cfg_s::mcs_table_opts::qam256;
        break;
      default:
        report_fatal_error("Invalid PDSCH MCS Table={}", dest.mcs_table);
    }
  }

  // Compute zp CSI-RS resources and resource sets.
  calculate_addmodremlist_diff(
      out.zp_csi_rs_res_to_add_mod_list,
      out.zp_csi_rs_res_to_release_list,
      src.zp_csi_rs_res_list,
      dest.zp_csi_rs_res_list,
      [](const zp_csi_rs_resource& r) { return make_asn1_zp_csi_rs_resource(r); },
      [](const zp_csi_rs_resource& r) { return r.id; });
  out.p_zp_csi_rs_res_set_present = calculate_setup_release(
      out.p_zp_csi_rs_res_set, src.p_zp_csi_rs_res, dest.p_zp_csi_rs_res, [](const zp_csi_rs_resource_set& s) {
        return make_asn1_zp_csi_rs_resource_set(s);
      });

  // TODO: Remaining.
}

static bool calculate_bwp_dl_dedicated_diff(asn1::rrc_nr::bwp_dl_ded_s&   out,
                                            const bwp_downlink_dedicated& src,
                                            const bwp_downlink_dedicated& dest)
{
  if ((dest.pdcch_cfg.has_value() && not src.pdcch_cfg.has_value()) ||
      (dest.pdcch_cfg.has_value() && src.pdcch_cfg.has_value() && dest.pdcch_cfg != src.pdcch_cfg)) {
    out.pdcch_cfg_present = true;
    calculate_pdcch_config_diff(out.pdcch_cfg.set_setup(),
                                src.pdcch_cfg.has_value() ? src.pdcch_cfg.value() : pdcch_config{},
                                dest.pdcch_cfg.value());
  } else if (src.pdcch_cfg.has_value() && not dest.pdcch_cfg.has_value()) {
    out.pdcch_cfg_present = true;
    out.pdcch_cfg.set_release();
  }

  if ((dest.pdsch_cfg.has_value() && not src.pdsch_cfg.has_value()) ||
      (dest.pdsch_cfg.has_value() && src.pdsch_cfg.has_value() && dest.pdsch_cfg != src.pdsch_cfg)) {
    out.pdsch_cfg_present = true;
    calculate_pdsch_config_diff(out.pdsch_cfg.set_setup(),
                                src.pdsch_cfg.has_value() ? src.pdsch_cfg.value() : pdsch_config{},
                                dest.pdsch_cfg.value());
  } else if (src.pdsch_cfg.has_value() && not dest.pdsch_cfg.has_value()) {
    out.pdsch_cfg_present = true;
    out.pdsch_cfg.set_release();
  }
  // TODO: sps-Config and radioLinkMonitoringConfig.

  return out.pdcch_cfg_present || out.pdsch_cfg_present;
}

asn1::rrc_nr::pucch_res_set_s srsran::srs_du::make_asn1_rrc_pucch_resource_set(const pucch_resource_set& cfg)
{
  pucch_res_set_s pucch_res_set;
  pucch_res_set.pucch_res_set_id = cfg.pucch_res_set_id;
  for (const auto& it : cfg.pucch_res_id_list) {
    pucch_res_set.res_list.push_back(it);
  }
  if (cfg.max_payload_size.has_value()) {
    pucch_res_set.max_payload_size = cfg.max_payload_size.value();
  }
  return pucch_res_set;
}

void make_asn1_rrc_pucch_formats_common_param(asn1::rrc_nr::pucch_format_cfg_s& out,
                                              const pucch_common_all_formats&   cfg)
{
  out.interslot_freq_hop_present = cfg.interslot_freq_hop;
  out.add_dmrs_present           = cfg.additional_dmrs;
  out.pi2_bpsk_present           = cfg.pi_2_bpsk;
  out.simul_harq_ack_csi_present = cfg.simultaneous_harq_ack_csi;
  if (cfg.max_c_rate == max_pucch_code_rate::not_set) {
    out.max_code_rate_present = false;
  } else {
    out.max_code_rate_present = true;
    switch (cfg.max_c_rate) {
      case max_pucch_code_rate::dot_08:
        out.max_code_rate = pucch_max_code_rate_opts::zero_dot08;
        break;
      case max_pucch_code_rate::dot_15:
        out.max_code_rate = pucch_max_code_rate_opts::zero_dot15;
        break;
      case max_pucch_code_rate::dot_25:
        out.max_code_rate = pucch_max_code_rate_opts::zero_dot25;
        break;
      case max_pucch_code_rate::dot_35:
        out.max_code_rate = pucch_max_code_rate_opts::zero_dot35;
        break;
      case max_pucch_code_rate::dot_45:
        out.max_code_rate = pucch_max_code_rate_opts::zero_dot45;
        break;
      case max_pucch_code_rate::dot_60:
        out.max_code_rate = pucch_max_code_rate_opts::zero_dot60;
        break;
      case max_pucch_code_rate::dot_80:
        out.max_code_rate = pucch_max_code_rate_opts::zero_dot80;
        break;
      default:
        srsran_assertion_failure("Invalid PUCCH Common Format 1 Max. Code Rate={}", cfg.max_c_rate);
    }
  }
  if (cfg.nof_slots == pucch_common_all_formats::num_of_slots::not_set) {
    out.nrof_slots_present = false;
  } else {
    out.nrof_slots_present = true;
    switch (cfg.nof_slots) {
      case pucch_common_all_formats::num_of_slots::n2:
        out.nrof_slots = pucch_format_cfg_s::nrof_slots_opts::n2;
        break;
      case pucch_common_all_formats::num_of_slots::n4:
        out.nrof_slots = pucch_format_cfg_s::nrof_slots_opts::n4;
        break;
      case pucch_common_all_formats::num_of_slots::n8:
        out.nrof_slots = pucch_format_cfg_s::nrof_slots_opts::n8;
        break;
      default:
        srsran_assertion_failure("Invalid PUCCH Common Format 1 No. of slots={}", cfg.nof_slots);
    }
  }
}

asn1::rrc_nr::pucch_res_s srsran::srs_du::make_asn1_rrc_pucch_resource(const pucch_resource& cfg)
{
  pucch_res_s pucch_res;
  pucch_res.pucch_res_id                = cfg.res_id;
  pucch_res.start_prb                   = cfg.starting_prb;
  pucch_res.intra_slot_freq_hop_present = cfg.second_hop_prb.has_value();
  pucch_res.second_hop_prb_present      = cfg.second_hop_prb.has_value();
  if (cfg.second_hop_prb.has_value()) {
    pucch_res.second_hop_prb = cfg.second_hop_prb.value();
  }
  switch (cfg.format) {
    case pucch_format::FORMAT_0: {
      const auto& f0            = variant_get<pucch_format_0_cfg>(cfg.format_params);
      auto&       format0       = pucch_res.format.set_format0();
      format0.init_cyclic_shift = f0.initial_cyclic_shift;
      format0.nrof_symbols      = f0.nof_symbols;
      format0.start_symbol_idx  = f0.starting_sym_idx;
    } break;
    case pucch_format::FORMAT_1: {
      const auto& f1            = variant_get<pucch_format_1_cfg>(cfg.format_params);
      auto&       format1       = pucch_res.format.set_format1();
      format1.init_cyclic_shift = f1.initial_cyclic_shift;
      format1.nrof_symbols      = f1.nof_symbols;
      format1.start_symbol_idx  = f1.starting_sym_idx;
      format1.time_domain_occ   = f1.time_domain_occ;
    } break;
    case pucch_format::FORMAT_2: {
      const auto& f2           = variant_get<pucch_format_2_3_cfg>(cfg.format_params);
      auto&       format2      = pucch_res.format.set_format2();
      format2.start_symbol_idx = f2.starting_sym_idx;
      format2.nrof_symbols     = f2.nof_symbols;
      format2.nrof_prbs        = f2.nof_prbs;
    } break;
    case pucch_format::FORMAT_3: {
      const auto& f3           = variant_get<pucch_format_2_3_cfg>(cfg.format_params);
      auto&       format3      = pucch_res.format.set_format3();
      format3.start_symbol_idx = f3.starting_sym_idx;
      format3.nrof_symbols     = f3.nof_symbols;
      format3.nrof_prbs        = f3.nof_prbs;
    } break;
    case pucch_format::FORMAT_4: {
      const auto& f4           = variant_get<pucch_format_4_cfg>(cfg.format_params);
      auto&       format4      = pucch_res.format.set_format4();
      format4.start_symbol_idx = f4.starting_sym_idx;
      format4.nrof_symbols     = f4.nof_symbols;
      switch (f4.occ_index) {
        case pucch_f4_occ_idx::n0:
          format4.occ_idx = pucch_format4_s::occ_idx_opts::n0;
          break;
        case pucch_f4_occ_idx::n1:
          format4.occ_idx = pucch_format4_s::occ_idx_opts::n1;
          break;
        case pucch_f4_occ_idx::n2:
          format4.occ_idx = pucch_format4_s::occ_idx_opts::n2;
          break;
        case pucch_f4_occ_idx::n3:
          format4.occ_idx = pucch_format4_s::occ_idx_opts::n3;
          break;
        default:
          srsran_assertion_failure("Invalid PUCCH Format 4 index={}", f4.occ_index);
      }
      switch (f4.occ_length) {
        case pucch_f4_occ_len::n2:
          format4.occ_len = pucch_format4_s::occ_len_opts::n2;
          break;
        case pucch_f4_occ_len::n4:
          format4.occ_len = pucch_format4_s::occ_len_opts::n4;
          break;
        default:
          srsran_assertion_failure("Invalid PUCCH Format 4 length={}", f4.occ_length);
      }
    } break;
    default:
      srsran_assertion_failure("Invalid PDCCH resource format={}", cfg.format);
  }
  return pucch_res;
}

asn1::rrc_nr::sched_request_res_cfg_s
srsran::srs_du::make_asn1_rrc_sr_resource(const scheduling_request_resource_config& cfg)
{
  sched_request_res_cfg_s sr_res_cfg;
  sr_res_cfg.sched_request_res_id           = cfg.sr_res_id;
  sr_res_cfg.sched_request_id               = cfg.sr_id;
  sr_res_cfg.res_present                    = true;
  sr_res_cfg.res                            = cfg.pucch_res_id;
  sr_res_cfg.periodicity_and_offset_present = true;
  switch (cfg.period) {
    case sr_periodicity::sym_2:
      sr_res_cfg.periodicity_and_offset.set_sym2();
      break;
    case sr_periodicity::sym_6_or_7:
      sr_res_cfg.periodicity_and_offset.set_sym6or7();
      break;
    case sr_periodicity::sl_1:
      sr_res_cfg.periodicity_and_offset.set_sl1();
      break;
    case sr_periodicity::sl_2: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl2();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_4: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl4();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_5: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl5();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_8: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl8();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_10: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl10();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_16: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl16();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_20: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl20();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_40: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl40();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_80: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl80();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_160: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl160();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_320: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl320();
      period_and_offset       = cfg.offset;
    } break;
    case sr_periodicity::sl_640: {
      auto& period_and_offset = sr_res_cfg.periodicity_and_offset.set_sl640();
      period_and_offset       = cfg.offset;
    } break;
    default:
      srsran_assertion_failure("Invalid SR periodicity={}", cfg.period);
  }
  return sr_res_cfg;
}

void calculate_pucch_config_diff(asn1::rrc_nr::pucch_cfg_s& out, const pucch_config& src, const pucch_config& dest)
{
  // PUCCH Resource Set.
  calculate_addmodremlist_diff(
      out.res_set_to_add_mod_list,
      out.res_set_to_release_list,
      src.pucch_res_set,
      dest.pucch_res_set,
      [](const pucch_resource_set& res_set) { return make_asn1_rrc_pucch_resource_set(res_set); },
      [](const pucch_resource_set& res_set) { return res_set.pucch_res_set_id; });

  // PUCCH Resource.
  calculate_addmodremlist_diff(
      out.res_to_add_mod_list,
      out.res_to_release_list,
      src.pucch_res_list,
      dest.pucch_res_list,
      [](const pucch_resource& res) { return make_asn1_rrc_pucch_resource(res); },
      [](const pucch_resource& res) { return res.res_id; });

  if ((dest.format_1_common_param.has_value() && not src.format_1_common_param.has_value()) ||
      (dest.format_1_common_param.has_value() && src.format_1_common_param.has_value() &&
       dest.format_1_common_param != src.format_1_common_param)) {
    out.format1_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format1.set_setup(), dest.format_1_common_param.value());
  } else if (src.format_1_common_param.has_value() && not dest.format_1_common_param.has_value()) {
    out.format1_present = true;
    out.format1.set_release();
  }
  if ((dest.format_2_common_param.has_value() && not src.format_2_common_param.has_value()) ||
      (dest.format_2_common_param.has_value() && src.format_2_common_param.has_value() &&
       dest.format_2_common_param != src.format_2_common_param)) {
    out.format2_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format2.set_setup(), dest.format_2_common_param.value());
  } else if (src.format_2_common_param.has_value() && not dest.format_2_common_param.has_value()) {
    out.format2_present = true;
    out.format2.set_release();
  }
  if ((dest.format_3_common_param.has_value() && not src.format_3_common_param.has_value()) ||
      (dest.format_3_common_param.has_value() && src.format_3_common_param.has_value() &&
       dest.format_3_common_param != src.format_3_common_param)) {
    out.format3_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format3.set_setup(), dest.format_3_common_param.value());
  } else if (src.format_3_common_param.has_value() && not dest.format_3_common_param.has_value()) {
    out.format3_present = true;
    out.format3.set_release();
  }
  if ((dest.format_4_common_param.has_value() && not src.format_4_common_param.has_value()) ||
      (dest.format_4_common_param.has_value() && src.format_4_common_param.has_value() &&
       dest.format_4_common_param != src.format_4_common_param)) {
    out.format4_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format4.set_setup(), dest.format_4_common_param.value());
  } else if (src.format_4_common_param.has_value() && not dest.format_4_common_param.has_value()) {
    out.format4_present = true;
    out.format4.set_release();
  }

  // SR Resource.
  calculate_addmodremlist_diff(
      out.sched_request_res_to_add_mod_list,
      out.sched_request_res_to_release_list,
      src.sr_res_list,
      dest.sr_res_list,
      [](const scheduling_request_resource_config& res) { return make_asn1_rrc_sr_resource(res); },
      [](const scheduling_request_resource_config& res) { return res.sr_id; });

  // dl-DataToUL-ACK.
  for (const auto& timing : dest.dl_data_to_ul_ack) {
    out.dl_data_to_ul_ack.push_back(timing);
  }
}

void make_asn1_rrc_ptrs_ul_cfg(asn1::rrc_nr::ptrs_ul_cfg_s& out, const ptrs_uplink_config& cfg)
{
  if (cfg.trans_precoder_disabled.has_value()) {
    out.transform_precoder_disabled_present = true;

    const auto& cfg_trans_preco_dis = cfg.trans_precoder_disabled.value();
    auto&       out_trans_preco_dis = out.transform_precoder_disabled;

    for (unsigned idx = 0; idx < cfg_trans_preco_dis.t_density.size(); idx++) {
      out_trans_preco_dis.time_density[idx] = cfg_trans_preco_dis.t_density[idx];
    }

    for (unsigned idx = 0; idx < cfg_trans_preco_dis.f_density.size(); idx++) {
      out_trans_preco_dis.freq_density[idx] = cfg_trans_preco_dis.f_density[idx];
    }

    switch (cfg_trans_preco_dis.max_ports) {
      case ptrs_uplink_config::transform_precoder_disabled::max_nof_ports::n1:
        out_trans_preco_dis.max_nrof_ports = ptrs_ul_cfg_s::transform_precoder_disabled_s_::max_nrof_ports_opts::n1;
        break;
      case ptrs_uplink_config::transform_precoder_disabled::max_nof_ports::n2:
        out_trans_preco_dis.max_nrof_ports = ptrs_ul_cfg_s::transform_precoder_disabled_s_::max_nrof_ports_opts::n2;
        break;
      default:
        srsran_assertion_failure("Invalid PTRS UL Cfg Max. Ports={}", cfg_trans_preco_dis.max_ports);
    }

    if (cfg_trans_preco_dis.res_elem_offset !=
        srsran::ptrs_uplink_config::transform_precoder_disabled::resource_element_offset::not_set) {
      switch (cfg_trans_preco_dis.res_elem_offset) {
        case ptrs_uplink_config::transform_precoder_disabled::resource_element_offset::offset01:
          out_trans_preco_dis.res_elem_offset =
              ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_opts::offset01;
          break;
        case ptrs_uplink_config::transform_precoder_disabled::resource_element_offset::offset10:
          out_trans_preco_dis.res_elem_offset =
              ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_opts::offset10;
          break;
        case ptrs_uplink_config::transform_precoder_disabled::resource_element_offset::offset11:
          out_trans_preco_dis.res_elem_offset =
              ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_opts::offset11;
          break;
        default:
          srsran_assertion_failure("Invalid Resource Element Offset={}", cfg_trans_preco_dis.res_elem_offset);
      }
    }

    switch (cfg_trans_preco_dis.power) {
      case ptrs_uplink_config::transform_precoder_disabled::ptrs_power::p00:
        out_trans_preco_dis.ptrs_pwr = ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_opts::p00;
        break;
      case ptrs_uplink_config::transform_precoder_disabled::ptrs_power::p01:
        out_trans_preco_dis.ptrs_pwr = ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_opts::p01;
        break;
      case ptrs_uplink_config::transform_precoder_disabled::ptrs_power::p10:
        out_trans_preco_dis.ptrs_pwr = ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_opts::p10;
        break;
      case ptrs_uplink_config::transform_precoder_disabled::ptrs_power::p11:
        out_trans_preco_dis.ptrs_pwr = ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_opts::p11;
        break;
      default:
        srsran_assertion_failure("Invalid PTRS UL Cfg Power={}", cfg_trans_preco_dis.power);
    }
  }

  if (cfg.trans_precoder_enabled.has_value()) {
    out.transform_precoder_enabled_present = true;

    const auto& cfg_trans_preco_enbl = cfg.trans_precoder_enabled.value();
    auto&       out_trans_preco_enbl = out.transform_precoder_enabled;

    for (unsigned idx = 0; idx < cfg_trans_preco_enbl.sampl_density.size(); idx++) {
      out_trans_preco_enbl.sample_density[idx] = cfg_trans_preco_enbl.sampl_density[idx];
    }
    if (cfg_trans_preco_enbl.is_t_density_trans_precoding_d2) {
      out_trans_preco_enbl.time_density_transform_precoding_present = true;
    }
  }
}

void make_asn1_rrc_dmrs_ul_for_pusch(asn1::rrc_nr::dmrs_ul_cfg_s& out,
                                     const dmrs_uplink_config&    src,
                                     const dmrs_uplink_config&    dest)
{
  if (dest.is_dmrs_type2) {
    out.dmrs_type_present = true;
  }

  if (dest.additional_positions != srsran::dmrs_additional_positions::pos2) {
    out.dmrs_add_position_present = true;
    switch (dest.additional_positions) {
      case dmrs_additional_positions::pos0:
        out.dmrs_add_position = dmrs_ul_cfg_s::dmrs_add_position_opts::pos0;
        break;
      case dmrs_additional_positions::pos1:
        out.dmrs_add_position = dmrs_ul_cfg_s::dmrs_add_position_opts::pos1;
        break;
      case dmrs_additional_positions::pos3:
        out.dmrs_add_position = dmrs_ul_cfg_s::dmrs_add_position_opts::pos3;
        break;
      default:
        srsran_assertion_failure("Invalid UL DMRS Add. pos={}", dest.additional_positions);
    }
  }

  if ((dest.ptrs.has_value() && not src.ptrs.has_value()) ||
      (dest.ptrs.has_value() && src.ptrs.has_value() && dest.ptrs != src.ptrs)) {
    out.phase_tracking_rs_present = true;
    make_asn1_rrc_ptrs_ul_cfg(out.phase_tracking_rs.set_setup(), dest.ptrs.value());
  } else if (src.ptrs.has_value() && not dest.ptrs.has_value()) {
    out.phase_tracking_rs_present = true;
    out.phase_tracking_rs.set_release();
  }

  if (dest.is_max_length_len2) {
    out.max_len_present = true;
  }

  if (dest.trans_precoder_disabled.has_value()) {
    out.transform_precoding_disabled_present = true;

    if (dest.trans_precoder_disabled.value().scrambling_id0.has_value()) {
      out.transform_precoding_disabled.scrambling_id0_present = true;
      out.transform_precoding_disabled.scrambling_id0 = dest.trans_precoder_disabled.value().scrambling_id0.value();
    }

    if (dest.trans_precoder_disabled.value().scrambling_id1.has_value()) {
      out.transform_precoding_disabled.scrambling_id1_present = true;
      out.transform_precoding_disabled.scrambling_id1 = dest.trans_precoder_disabled.value().scrambling_id1.value();
    }
  }

  if (dest.trans_precoder_enabled.has_value()) {
    out.transform_precoding_enabled_present = true;

    if (dest.trans_precoder_enabled.value().n_pusch_id.has_value()) {
      out.transform_precoding_enabled.npusch_id_present = true;
      out.transform_precoding_enabled.npusch_id         = dest.trans_precoder_enabled.value().n_pusch_id.value();
    }

    if (dest.trans_precoder_enabled.value().is_seq_grp_hopping_disabled) {
      out.transform_precoding_enabled.seq_group_hop_present = true;
    }

    if (dest.trans_precoder_enabled.value().is_seq_hopping_enabled) {
      out.transform_precoding_enabled.seq_hop_present = true;
    }
  }
}

asn1::rrc_nr::pusch_pathloss_ref_rs_s
srsran::srs_du::make_asn1_rrc_pusch_pathloss_ref_rs(const pusch_config::pusch_power_control::pusch_pathloss_ref_rs& cfg)
{
  pusch_pathloss_ref_rs_s ploss_ref_rs;
  ploss_ref_rs.pusch_pathloss_ref_rs_id = cfg.id;
  if (variant_holds_alternative<nzp_csi_rs_res_id_t>(cfg.rs)) {
    auto& csi_rs_idx = ploss_ref_rs.ref_sig.set_csi_rs_idx();
    csi_rs_idx       = variant_get<nzp_csi_rs_res_id_t>(cfg.rs);
  } else if (variant_holds_alternative<ssb_id_t>(cfg.rs)) {
    auto& ssb_idx = ploss_ref_rs.ref_sig.set_ssb_idx();
    ssb_idx       = variant_get<ssb_id_t>(cfg.rs);
  }
  return ploss_ref_rs;
}

asn1::rrc_nr::sri_pusch_pwr_ctrl_s
srsran::srs_du::make_asn1_rrc_sri_pusch_pwr_ctrl(const pusch_config::pusch_power_control::sri_pusch_pwr_ctrl& cfg)
{
  sri_pusch_pwr_ctrl_s sri_pwr_ctl;
  sri_pwr_ctl.sri_pusch_pwr_ctrl_id        = cfg.id;
  sri_pwr_ctl.sri_p0_pusch_alpha_set_id    = cfg.sri_p0_pusch_alphaset_id;
  sri_pwr_ctl.sri_pusch_pathloss_ref_rs_id = cfg.sri_pusch_pathloss_ref_rs_id;
  switch (cfg.closed_loop_idx) {
    case pusch_config::pusch_power_control::sri_pusch_pwr_ctrl::sri_pusch_closed_loop_index::i0:
      sri_pwr_ctl.sri_pusch_closed_loop_idx = sri_pusch_pwr_ctrl_s::sri_pusch_closed_loop_idx_opts::i0;
      break;
    case pusch_config::pusch_power_control::sri_pusch_pwr_ctrl::sri_pusch_closed_loop_index::i1:
      sri_pwr_ctl.sri_pusch_closed_loop_idx = sri_pusch_pwr_ctrl_s::sri_pusch_closed_loop_idx_opts::i1;
      break;
    default:
      srsran_assertion_failure("Invalid SRI Closed loop idx={}", cfg.closed_loop_idx);
  }
  return sri_pwr_ctl;
}

void make_asn1_rrc_alpha(asn1::rrc_nr::alpha_e& out, const alpha& cfg)
{
  switch (cfg) {
    case alpha::alpha0:
      out = alpha_opts::alpha0;
      break;
    case alpha::alpha04:
      out = alpha_opts::alpha04;
      break;
    case alpha::alpha05:
      out = alpha_opts::alpha05;
      break;
    case alpha::alpha06:
      out = alpha_opts::alpha06;
      break;
    case alpha::alpha07:
      out = alpha_opts::alpha07;
      break;
    case alpha::alpha08:
      out = alpha_opts::alpha08;
      break;
    case alpha::alpha09:
      out = alpha_opts::alpha09;
      break;
    case alpha::alpha1:
      out = alpha_opts::alpha1;
      break;
    default:
      srsran_assertion_failure("Invalid alpha={}", cfg);
  }
}

void make_asn1_rrc_pusch_pwr_ctrl(asn1::rrc_nr::pusch_pwr_ctrl_s&          out,
                                  const pusch_config::pusch_power_control& src,
                                  const pusch_config::pusch_power_control& dest)
{
  if (dest.is_tpc_accumulation_disabled) {
    out.tpc_accumulation_present = true;
  }

  if (dest.msg3_alpha != srsran::alpha::not_set) {
    out.msg3_alpha_present = true;
    make_asn1_rrc_alpha(out.msg3_alpha, dest.msg3_alpha);
  }

  if (dest.p0_nominal_without_grant.has_value()) {
    out.p0_nominal_without_grant_present = true;
    out.p0_nominal_without_grant         = dest.p0_nominal_without_grant.value();
  }

  for (unsigned idx = 0; idx < dest.p0_alphasets.size(); idx++) {
    p0_pusch_alpha_set_s p0_alphaset{};
    p0_alphaset.p0_pusch_alpha_set_id = dest.p0_alphasets[idx].id;
    if (dest.p0_alphasets[idx].p0.has_value()) {
      p0_alphaset.p0_present = true;
      p0_alphaset.p0         = dest.p0_alphasets[idx].p0.value();
    }
    if (dest.p0_alphasets[idx].p0_pusch_alpha != srsran::alpha::not_set) {
      p0_alphaset.alpha_present = true;
      make_asn1_rrc_alpha(p0_alphaset.alpha, dest.p0_alphasets[idx].p0_pusch_alpha);
    }
    out.p0_alpha_sets.push_back(p0_alphaset);
  }

  calculate_addmodremlist_diff(
      out.pathloss_ref_rs_to_add_mod_list,
      out.pathloss_ref_rs_to_release_list,
      src.pathloss_ref_rs,
      dest.pathloss_ref_rs,
      [](const pusch_config::pusch_power_control::pusch_pathloss_ref_rs& res) {
        return make_asn1_rrc_pusch_pathloss_ref_rs(res);
      },
      [](const pusch_config::pusch_power_control::pusch_pathloss_ref_rs& res) { return res.id; });

  calculate_addmodremlist_diff(
      out.sri_pusch_map_to_add_mod_list,
      out.sri_pusch_map_to_release_list,
      src.sri_pusch_mapping,
      dest.sri_pusch_mapping,
      [](const pusch_config::pusch_power_control::sri_pusch_pwr_ctrl& res) {
        return make_asn1_rrc_sri_pusch_pwr_ctrl(res);
      },
      [](const pusch_config::pusch_power_control::sri_pusch_pwr_ctrl& res) { return res.id; });
}

void fill_uci_beta_offset(beta_offsets_s& beta_out, const beta_offsets& beta_in)
{
  if (beta_in.beta_offset_ack_idx_1.has_value()) {
    beta_out.beta_offset_ack_idx1_present = true;
    beta_out.beta_offset_ack_idx1         = beta_in.beta_offset_ack_idx_1.value();
  } else {
    beta_out.beta_offset_ack_idx1_present = false;
  }

  if (beta_in.beta_offset_ack_idx_2.has_value()) {
    beta_out.beta_offset_ack_idx2_present = true;
    beta_out.beta_offset_ack_idx2         = beta_in.beta_offset_ack_idx_2.value();
  } else {
    beta_out.beta_offset_ack_idx2_present = false;
  }

  if (beta_in.beta_offset_ack_idx_3.has_value()) {
    beta_out.beta_offset_ack_idx3_present = true;
    beta_out.beta_offset_ack_idx3         = beta_in.beta_offset_ack_idx_3.value();
  } else {
    beta_out.beta_offset_ack_idx3_present = false;
  }

  if (beta_in.beta_offset_csi_p1_idx_1.has_value()) {
    beta_out.beta_offset_csi_part1_idx1_present = true;
    beta_out.beta_offset_csi_part1_idx1         = beta_in.beta_offset_csi_p1_idx_1.value();
  } else {
    beta_out.beta_offset_csi_part1_idx1_present = false;
  }

  if (beta_in.beta_offset_csi_p1_idx_2.has_value()) {
    beta_out.beta_offset_csi_part1_idx2_present = true;
    beta_out.beta_offset_csi_part1_idx2         = beta_in.beta_offset_csi_p1_idx_2.value();
  } else {
    beta_out.beta_offset_csi_part1_idx2_present = false;
  }

  if (beta_in.beta_offset_csi_p2_idx_1.has_value()) {
    beta_out.beta_offset_csi_part2_idx1_present = true;
    beta_out.beta_offset_csi_part2_idx1         = beta_in.beta_offset_csi_p2_idx_1.value();
  } else {
    beta_out.beta_offset_csi_part2_idx1_present = false;
  }

  if (beta_in.beta_offset_csi_p2_idx_2.has_value()) {
    beta_out.beta_offset_csi_part2_idx2_present = true;
    beta_out.beta_offset_csi_part2_idx2         = beta_in.beta_offset_csi_p2_idx_2.value();
  } else {
    beta_out.beta_offset_csi_part2_idx2_present = false;
  }
}

void fill_uci_on_pusch(asn1::rrc_nr::uci_on_pusch_s& uci_asn1, const uci_on_pusch& uci_in)
{
  switch (uci_in.scaling) {
    case alpha_scaling_opt::f0p5:
      uci_asn1.scaling.value = uci_on_pusch_s::scaling_opts::f0p5;
      break;
    case alpha_scaling_opt::f0p65:
      uci_asn1.scaling.value = uci_on_pusch_s::scaling_opts::f0p65;
      break;
    case alpha_scaling_opt::f0p8:
      uci_asn1.scaling.value = uci_on_pusch_s::scaling_opts::f0p8;
      break;
    case alpha_scaling_opt::f1:
      uci_asn1.scaling.value = uci_on_pusch_s::scaling_opts::f1;
      break;
    default:
      srsran_assertion_failure("Invalid Alpha Value Scaling={}", uci_in.scaling);
  }

  if (uci_in.beta_offsets_cfg.has_value()) {
    uci_asn1.beta_offsets_present = true;
    if (variant_holds_alternative<uci_on_pusch::beta_offsets_semi_static>(uci_in.beta_offsets_cfg.value())) {
      const auto& in_semi_static = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_in.beta_offsets_cfg.value());
      auto&       out_semi_static = uci_asn1.beta_offsets.set_semi_static();

      fill_uci_beta_offset(out_semi_static, in_semi_static);
    } else {
      auto& input_dynamic = variant_get<uci_on_pusch::beta_offsets_dynamic>(uci_in.beta_offsets_cfg.value());
      auto& out_dynamic   = uci_asn1.beta_offsets.set_dyn();

      srsran_assert(input_dynamic.size() == out_dynamic.max_size(), "Mismatch between input and output vectors");

      for (size_t n = 0; n != input_dynamic.size(); ++n) {
        fill_uci_beta_offset(out_dynamic[n], input_dynamic[n]);
      }
    }
  } else {
    uci_asn1.beta_offsets_present = false;
  }
}

asn1::rrc_nr::pusch_time_domain_res_alloc_s
make_asn1_rrc_pusch_time_domain_alloc_list(const pusch_time_domain_resource_allocation& cfg)
{
  pusch_time_domain_res_alloc_s out{};
  out.k2_present = true;
  out.k2         = cfg.k2;
  switch (cfg.map_type) {
    case sch_mapping_type::typeA:
      out.map_type = pusch_time_domain_res_alloc_s::map_type_opts::type_a;
      break;
    case sch_mapping_type::typeB:
      out.map_type = pusch_time_domain_res_alloc_s::map_type_opts::type_b;
      break;
    default:
      srsran_assertion_failure("Invalid SCH mapping Type={}", cfg.map_type);
  }

  out.start_symbol_and_len = ofdm_symbol_range_to_sliv(cfg.symbols);

  return out;
}

void calculate_pusch_config_diff(asn1::rrc_nr::pusch_cfg_s& out, const pusch_config& src, const pusch_config& dest)
{
  if (dest.data_scrambling_id_pusch.has_value()) {
    out.data_scrambling_id_pusch_present = true;
    out.data_scrambling_id_pusch         = dest.data_scrambling_id_pusch.value();
  }
  if (dest.tx_cfg != srsran::pusch_config::tx_config::not_set) {
    out.tx_cfg_present = true;
    switch (dest.tx_cfg) {
      case pusch_config::tx_config::codebook:
        out.tx_cfg = pusch_cfg_s::tx_cfg_opts::codebook;
        break;
      case pusch_config::tx_config::non_codebook:
        out.tx_cfg = pusch_cfg_s::tx_cfg_opts::non_codebook;
        break;
      default:
        srsran_assertion_failure("Invalid PUSCH Tx cfg={}", dest.tx_cfg);
    }
  }

  if ((dest.pusch_mapping_type_a_dmrs.has_value() && not src.pusch_mapping_type_a_dmrs.has_value()) ||
      (dest.pusch_mapping_type_a_dmrs.has_value() && src.pusch_mapping_type_a_dmrs.has_value() &&
       dest.pusch_mapping_type_a_dmrs != src.pusch_mapping_type_a_dmrs)) {
    out.dmrs_ul_for_pusch_map_type_a_present = true;
    make_asn1_rrc_dmrs_ul_for_pusch(out.dmrs_ul_for_pusch_map_type_a.set_setup(),
                                    src.pusch_mapping_type_a_dmrs.has_value() ? src.pusch_mapping_type_a_dmrs.value()
                                                                              : dmrs_uplink_config{},
                                    dest.pusch_mapping_type_a_dmrs.value());
  } else if (src.pusch_mapping_type_a_dmrs.has_value() && not dest.pusch_mapping_type_a_dmrs.has_value()) {
    out.dmrs_ul_for_pusch_map_type_a_present = true;
    out.dmrs_ul_for_pusch_map_type_a.set_release();
  }

  if ((dest.pusch_mapping_type_b_dmrs.has_value() && not src.pusch_mapping_type_b_dmrs.has_value()) ||
      (dest.pusch_mapping_type_b_dmrs.has_value() && src.pusch_mapping_type_b_dmrs.has_value() &&
       dest.pusch_mapping_type_b_dmrs != src.pusch_mapping_type_b_dmrs)) {
    out.dmrs_ul_for_pusch_map_type_b_present = true;
    make_asn1_rrc_dmrs_ul_for_pusch(out.dmrs_ul_for_pusch_map_type_b.set_setup(),
                                    src.pusch_mapping_type_b_dmrs.has_value() ? src.pusch_mapping_type_b_dmrs.value()
                                                                              : dmrs_uplink_config{},
                                    dest.pusch_mapping_type_b_dmrs.value());
  } else if (src.pusch_mapping_type_b_dmrs.has_value() && not dest.pusch_mapping_type_b_dmrs.has_value()) {
    out.dmrs_ul_for_pusch_map_type_b_present = true;
    out.dmrs_ul_for_pusch_map_type_b.set_release();
  }

  if (dest.pusch_pwr_ctrl.has_value()) {
    out.pusch_pwr_ctrl_present = true;
    make_asn1_rrc_pusch_pwr_ctrl(out.pusch_pwr_ctrl,
                                 src.pusch_pwr_ctrl.has_value() ? src.pusch_pwr_ctrl.value()
                                                                : pusch_config::pusch_power_control{},
                                 dest.pusch_pwr_ctrl.value());
  }

  switch (dest.res_alloc) {
    case pusch_config::resource_allocation::resource_allocation_type_0:
      out.res_alloc = pusch_cfg_s::res_alloc_opts::res_alloc_type0;
      break;
    case pusch_config::resource_allocation::resource_allocation_type_1:
      out.res_alloc = pusch_cfg_s::res_alloc_opts::res_alloc_type1;
      break;
    case pusch_config::resource_allocation::dynamic_switch:
      out.res_alloc = pusch_cfg_s::res_alloc_opts::dyn_switch;
      break;
    default:
      srsran_assertion_failure("Invalid PUSCH Resource Allocation={}", dest.res_alloc);
  }

  // PUSCH Time Domain Allocation.
  if ((not dest.pusch_td_alloc_list.empty() && src.pusch_td_alloc_list.empty()) ||
      (not dest.pusch_td_alloc_list.empty() && not src.pusch_td_alloc_list.empty() &&
       dest.pusch_td_alloc_list != src.pusch_td_alloc_list)) {
    out.pusch_time_domain_alloc_list_present = true;
    auto& alloc_list                         = out.pusch_time_domain_alloc_list.set_setup();
    for (const auto& td_alloc : dest.pusch_td_alloc_list) {
      alloc_list.push_back(make_asn1_rrc_pusch_time_domain_alloc_list(td_alloc));
    }
  } else if (not src.pusch_td_alloc_list.empty() && dest.pusch_td_alloc_list.empty()) {
    out.pusch_time_domain_alloc_list_present = true;
    out.pusch_time_domain_alloc_list.set_release();
  }

  if (dest.mcs_table != srsran::pusch_mcs_table::qam64) {
    out.mcs_table_present = true;
    switch (dest.mcs_table) {
      case pusch_mcs_table::qam64LowSe:
        out.mcs_table.value = pusch_cfg_s::mcs_table_opts::qam64_low_se;
        break;
      case pusch_mcs_table::qam256:
        out.mcs_table.value = pusch_cfg_s::mcs_table_opts::qam256;
        break;
      default:
        report_fatal_error("Invalid PUSCH MCS Table={}", dest.mcs_table);
    }
  }

  if (dest.trans_precoder != srsran::pusch_config::transform_precoder::not_set) {
    out.transform_precoder_present = true;
    switch (dest.trans_precoder) {
      case pusch_config::transform_precoder::enabled:
        out.transform_precoder = pusch_cfg_s::transform_precoder_opts::enabled;
        break;
      case pusch_config::transform_precoder::disabled:
        out.transform_precoder = pusch_cfg_s::transform_precoder_opts::disabled;
        break;
      default:
        srsran_assertion_failure("Invalid PUSCH Transform Precoder={}", dest.trans_precoder);
    }
  }

  if (dest.cb_subset != srsran::pusch_config::codebook_subset::not_set) {
    out.codebook_subset_present = true;
    switch (dest.cb_subset) {
      case pusch_config::codebook_subset::fully_and_partial_and_non_coherent:
        out.codebook_subset = pusch_cfg_s::codebook_subset_opts::fully_and_partial_and_non_coherent;
        break;
      case pusch_config::codebook_subset::partial_and_non_coherent:
        out.codebook_subset = pusch_cfg_s::codebook_subset_opts::partial_and_non_coherent;
        break;
      case pusch_config::codebook_subset::non_coherent:
        out.codebook_subset = pusch_cfg_s::codebook_subset_opts::non_coherent;
        break;
      default:
        srsran_assertion_failure("Invalid Codebook subset={}", dest.cb_subset);
    }
  }

  if (dest.max_rank.has_value()) {
    out.max_rank_present = true;
    out.max_rank         = dest.max_rank.value();
  }

  if ((dest.uci_cfg.has_value() && not src.uci_cfg.has_value()) ||
      (dest.uci_cfg.has_value() && src.uci_cfg.has_value() && dest.uci_cfg.has_value() != src.uci_cfg.has_value())) {
    out.uci_on_pusch_present = true;
    auto& uci                = out.uci_on_pusch.set_setup();
    fill_uci_on_pusch(uci, dest.uci_cfg.value());
  } else if (src.uci_cfg.has_value() && not dest.uci_cfg.has_value()) {
    out.uci_on_pusch_present = true;
    out.uci_on_pusch.set_release();
  }
}

asn1::rrc_nr::srs_res_set_s srsran::srs_du::make_asn1_rrc_srs_res_set(const srs_config::srs_resource_set& cfg)
{
  srs_res_set_s srs_res_set;
  srs_res_set.srs_res_set_id = cfg.id;
  for (const auto& res_id : cfg.srs_res_id_list) {
    srs_res_set.srs_res_id_list.push_back(res_id);
  }

  if (variant_holds_alternative<srs_config::srs_resource_set::aperiodic_resource_type>(cfg.res_type)) {
    auto&       aper_res     = srs_res_set.res_type.set_aperiodic();
    const auto& cfg_aper_res = variant_get<srs_config::srs_resource_set::aperiodic_resource_type>(cfg.res_type);
    aper_res.aperiodic_srs_res_trigger = cfg_aper_res.aperiodic_srs_res_trigger;
    if (cfg_aper_res.csi_rs.has_value()) {
      aper_res.csi_rs_present = true;
      aper_res.csi_rs         = cfg_aper_res.csi_rs.value();
    }
    if (cfg_aper_res.slot_offset.has_value()) {
      aper_res.slot_offset_present = true;
      aper_res.slot_offset         = cfg_aper_res.slot_offset.value();
    }
    if (not cfg_aper_res.aperiodic_srs_res_trigger_list.empty()) {
      aper_res.aperiodic_srs_res_trigger_list.set_present();
      auto* aper_res_trig_list = aper_res.aperiodic_srs_res_trigger_list.get();
      for (const auto& ap_res_trig : cfg_aper_res.aperiodic_srs_res_trigger_list) {
        aper_res_trig_list->push_back(ap_res_trig);
      }
    }
  } else if (variant_holds_alternative<srs_config::srs_resource_set::semi_persistent_resource_type>(cfg.res_type)) {
    auto&       semi_p_res     = srs_res_set.res_type.set_semi_persistent();
    const auto& cfg_semi_p_res = variant_get<srs_config::srs_resource_set::semi_persistent_resource_type>(cfg.res_type);
    if (cfg_semi_p_res.associated_csi_rs.has_value()) {
      semi_p_res.associated_csi_rs_present = true;
      semi_p_res.associated_csi_rs         = cfg_semi_p_res.associated_csi_rs.value();
    }
  } else if (variant_holds_alternative<srs_config::srs_resource_set::periodic_resource_type>(cfg.res_type)) {
    auto&       per_res     = srs_res_set.res_type.set_periodic();
    const auto& cfg_per_res = variant_get<srs_config::srs_resource_set::periodic_resource_type>(cfg.res_type);
    if (cfg_per_res.associated_csi_rs.has_value()) {
      per_res.associated_csi_rs_present = true;
      per_res.associated_csi_rs         = cfg_per_res.associated_csi_rs.value();
    }
  }

  switch (cfg.srs_res_set_usage) {
    case srs_config::srs_resource_set::usage::beam_management:
      srs_res_set.usage = srs_res_set_s::usage_opts::beam_management;
      break;
    case srs_config::srs_resource_set::usage::codebook:
      srs_res_set.usage = srs_res_set_s::usage_opts::codebook;
      break;
    case srs_config::srs_resource_set::usage::non_codebook:
      srs_res_set.usage = srs_res_set_s::usage_opts::non_codebook;
      break;
    case srs_config::srs_resource_set::usage::antenna_switching:
      srs_res_set.usage = srs_res_set_s::usage_opts::ant_switching;
      break;
    default:
      srsran_assertion_failure("Invalid SRS resource set usage={}", cfg.srs_res_set_usage);
  }

  if (cfg.srs_pwr_ctrl_alpha != alpha::not_set) {
    srs_res_set.alpha_present = true;
    make_asn1_rrc_alpha(srs_res_set.alpha, cfg.srs_pwr_ctrl_alpha);
  }

  if (cfg.p0.has_value()) {
    srs_res_set.p0_present = true;
    srs_res_set.p0         = cfg.p0.value();
  }

  if (cfg.pwr_ctrl_adj_states != srs_config::srs_resource_set::srs_pwr_ctrl_adjustment_states::not_set) {
    srs_res_set.srs_pwr_ctrl_adjustment_states_present = true;
    switch (cfg.pwr_ctrl_adj_states) {
      case srs_config::srs_resource_set::srs_pwr_ctrl_adjustment_states::same_as_fci2:
        srs_res_set.srs_pwr_ctrl_adjustment_states = srs_res_set_s::srs_pwr_ctrl_adjustment_states_opts::same_as_fci2;
        break;
      case srs_config::srs_resource_set::srs_pwr_ctrl_adjustment_states::separate_closed_loop:
        srs_res_set.srs_pwr_ctrl_adjustment_states =
            srs_res_set_s::srs_pwr_ctrl_adjustment_states_opts::separate_closed_loop;
        break;
      default:
        srsran_assertion_failure("Invalid Power Control Adj. state={}", cfg.pwr_ctrl_adj_states);
    }
  }

  if (cfg.pathloss_ref_rs.has_value()) {
    srs_res_set.pathloss_ref_rs_present = true;
    if (variant_holds_alternative<ssb_id_t>(cfg.pathloss_ref_rs.value())) {
      auto& ssb_idx = srs_res_set.pathloss_ref_rs.set_ssb_idx();
      ssb_idx       = variant_get<ssb_id_t>(cfg.pathloss_ref_rs.value());
    } else if (variant_holds_alternative<nzp_csi_rs_res_id_t>(cfg.pathloss_ref_rs.value())) {
      auto& csi_res_idx = srs_res_set.pathloss_ref_rs.set_csi_rs_idx();
      csi_res_idx       = variant_get<nzp_csi_rs_res_id_t>(cfg.pathloss_ref_rs.value());
    }
  }

  return srs_res_set;
}

void make_asn1_rrc_srs_config_perioidicity_and_offset(asn1::rrc_nr::srs_periodicity_and_offset_c&   out,
                                                      const srs_config::srs_periodicity_and_offset& cfg)
{
  switch (cfg.type) {
    case srs_config::srs_periodicity_and_offset::type_t::sl1:
      out.set_sl1();
      break;
    case srs_config::srs_periodicity_and_offset::type_t::sl2: {
      auto& p_and_o = out.set_sl2();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl4: {
      auto& p_and_o = out.set_sl4();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl5: {
      auto& p_and_o = out.set_sl5();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl8: {
      auto& p_and_o = out.set_sl8();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl10: {
      auto& p_and_o = out.set_sl10();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl16: {
      auto& p_and_o = out.set_sl16();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl20: {
      auto& p_and_o = out.set_sl20();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl32: {
      auto& p_and_o = out.set_sl32();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl40: {
      auto& p_and_o = out.set_sl40();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl64: {
      auto& p_and_o = out.set_sl64();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl80: {
      auto& p_and_o = out.set_sl80();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl160: {
      auto& p_and_o = out.set_sl160();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl320: {
      auto& p_and_o = out.set_sl320();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl640: {
      auto& p_and_o = out.set_sl640();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl1280: {
      auto& p_and_o = out.set_sl1280();
      p_and_o       = cfg.value;
    } break;
    case srs_config::srs_periodicity_and_offset::type_t::sl2560: {
      auto& p_and_o = out.set_sl2560();
      p_and_o       = cfg.value;
    } break;
    default:
      srsran_assertion_failure("Invalid periodicity and offset={}", cfg.type);
  }
}

asn1::rrc_nr::srs_res_s srsran::srs_du::make_asn1_rrc_srs_res(const srs_config::srs_resource& cfg)
{
  srs_res_s res;
  res.srs_res_id = cfg.id;

  switch (cfg.nof_ports) {
    case srs_config::srs_resource::nof_srs_ports::port1:
      res.nrof_srs_ports = srs_res_s::nrof_srs_ports_opts::port1;
      break;
    case srs_config::srs_resource::nof_srs_ports::ports2:
      res.nrof_srs_ports = srs_res_s::nrof_srs_ports_opts::ports2;
      break;
    case srs_config::srs_resource::nof_srs_ports::ports4:
      res.nrof_srs_ports = srs_res_s::nrof_srs_ports_opts::ports4;
      break;
    default:
      srsran_assertion_failure("Invalid number of SRS ports={}", cfg.nof_ports);
  }

  if (cfg.ptrs_port != srs_config::srs_resource::ptrs_port_index::not_set) {
    res.ptrs_port_idx_present = true;
    switch (cfg.ptrs_port) {
      case srs_config::srs_resource::ptrs_port_index::n0:
        res.ptrs_port_idx = srs_res_s::ptrs_port_idx_opts::n0;
        break;
      case srs_config::srs_resource::ptrs_port_index::n1:
        res.ptrs_port_idx = srs_res_s::ptrs_port_idx_opts::n1;
        break;
      default:
        srsran_assertion_failure("Invalid PTRS port={}", cfg.ptrs_port);
    }
  }

  if (cfg.trans_comb_value == 2) {
    auto& n2           = res.tx_comb.set_n2();
    n2.comb_offset_n2  = cfg.trans_comb_offset;
    n2.cyclic_shift_n2 = cfg.trans_comb_cyclic_shift;
  } else if (cfg.trans_comb_value == 4) {
    auto& n4           = res.tx_comb.set_n4();
    n4.comb_offset_n4  = cfg.trans_comb_offset;
    n4.cyclic_shift_n4 = cfg.trans_comb_cyclic_shift;
  }

  res.res_map.start_position = cfg.res_mapping.start_pos;
  switch (cfg.res_mapping.nof_symb) {
    case srs_config::srs_resource::resource_mapping::nof_symbols::n1:
      res.res_map.nrof_symbols = srs_res_s::res_map_s_::nrof_symbols_opts::n1;
      break;
    case srs_config::srs_resource::resource_mapping::nof_symbols::n2:
      res.res_map.nrof_symbols = srs_res_s::res_map_s_::nrof_symbols_opts::n2;
      break;
    case srs_config::srs_resource::resource_mapping::nof_symbols::n4:
      res.res_map.nrof_symbols = srs_res_s::res_map_s_::nrof_symbols_opts::n4;
      break;
    default:
      srsran_assertion_failure("Invalid number of OFDM symb={}", cfg.res_mapping.nof_symb);
  }
  switch (cfg.res_mapping.re_factor) {
    case srs_config::srs_resource::resource_mapping::repetition_factor::n1:
      res.res_map.repeat_factor = srs_res_s::res_map_s_::repeat_factor_opts::n1;
      break;
    case srs_config::srs_resource::resource_mapping::repetition_factor::n2:
      res.res_map.repeat_factor = srs_res_s::res_map_s_::repeat_factor_opts::n2;
      break;
    case srs_config::srs_resource::resource_mapping::repetition_factor::n4:
      res.res_map.repeat_factor = srs_res_s::res_map_s_::repeat_factor_opts::n4;
      break;
    default:
      srsran_assertion_failure("Invalid repetition factor={}", cfg.res_mapping.re_factor);
  }

  res.freq_domain_position = cfg.freq_domain_pos;
  res.freq_domain_shift    = cfg.freq_domain_shift;

  res.freq_hop.b_hop = cfg.freq_hop.b_hop;
  res.freq_hop.b_srs = cfg.freq_hop.b_srs;
  res.freq_hop.c_srs = cfg.freq_hop.c_srs;

  switch (cfg.grp_or_seq_hop) {
    case srs_config::srs_resource::group_or_sequence_hopping::neither:
      res.group_or_seq_hop = srs_res_s::group_or_seq_hop_opts::neither;
      break;
    case srs_config::srs_resource::group_or_sequence_hopping::groupHopping:
      res.group_or_seq_hop = srs_res_s::group_or_seq_hop_opts::group_hop;
      break;
    case srs_config::srs_resource::group_or_sequence_hopping::sequenceHopping:
      res.group_or_seq_hop = srs_res_s::group_or_seq_hop_opts::seq_hop;
      break;
    default:
      srsran_assertion_failure("Invalid grp or seq hop={}", cfg.grp_or_seq_hop);
  }

  switch (cfg.res_type) {
    case srs_config::srs_resource::resource_type::aperiodic: {
      res.res_type.set_aperiodic();
    } break;
    case srs_config::srs_resource::resource_type::semi_persistent: {
      auto& sp_res = res.res_type.set_semi_persistent();
      make_asn1_rrc_srs_config_perioidicity_and_offset(sp_res.periodicity_and_offset_sp,
                                                       cfg.semi_pers_res_type_periodicity_and_offset);
    } break;
    case srs_config::srs_resource::resource_type::periodic: {
      auto& p_res = res.res_type.set_semi_persistent();
      make_asn1_rrc_srs_config_perioidicity_and_offset(p_res.periodicity_and_offset_sp,
                                                       cfg.semi_pers_res_type_periodicity_and_offset);
    } break;
    default:
      srsran_assertion_failure("Invalid resource type={}", cfg.res_type);
  }

  res.seq_id = cfg.sequence_id;

  if (cfg.spatial_relation_info.has_value()) {
    res.spatial_relation_info_present = true;
    if (cfg.spatial_relation_info.value().serv_cell_id.has_value()) {
      res.spatial_relation_info.serving_cell_id_present = true;
      res.spatial_relation_info.serving_cell_id         = cfg.spatial_relation_info.value().serv_cell_id.value();
    }
    if (variant_holds_alternative<ssb_id_t>(cfg.spatial_relation_info.value().reference_signal)) {
      auto& ssb_idx = res.spatial_relation_info.ref_sig.set_ssb_idx();
      ssb_idx       = variant_get<ssb_id_t>(cfg.spatial_relation_info.value().reference_signal);
    } else if (variant_holds_alternative<nzp_csi_rs_res_id_t>(cfg.spatial_relation_info.value().reference_signal)) {
      auto& csi_rs_idx = res.spatial_relation_info.ref_sig.set_csi_rs_idx();
      csi_rs_idx       = variant_get<nzp_csi_rs_res_id_t>(cfg.spatial_relation_info.value().reference_signal);
    } else if (variant_holds_alternative<srs_config::srs_resource::srs_spatial_relation_info::srs_ref_signal>(
                   cfg.spatial_relation_info.value().reference_signal)) {
      auto&       srs_ref_sig     = res.spatial_relation_info.ref_sig.set_srs();
      const auto& cfg_srs_ref_sig = variant_get<srs_config::srs_resource::srs_spatial_relation_info::srs_ref_signal>(
          cfg.spatial_relation_info.value().reference_signal);
      srs_ref_sig.res_id = cfg_srs_ref_sig.res_id;
      srs_ref_sig.ul_bwp = cfg_srs_ref_sig.ul_bwp;
    }
  }

  return res;
}

void calculate_srs_config_diff(asn1::rrc_nr::srs_cfg_s& out, const srs_config& src, const srs_config& dest)
{
  calculate_addmodremlist_diff(
      out.srs_res_set_to_add_mod_list,
      out.srs_res_set_to_release_list,
      src.srs_res_set,
      dest.srs_res_set,
      [](const srs_config::srs_resource_set& res) { return make_asn1_rrc_srs_res_set(res); },
      [](const srs_config::srs_resource_set& res) { return res.id; });

  calculate_addmodremlist_diff(
      out.srs_res_to_add_mod_list,
      out.srs_res_to_release_list,
      src.srs_res,
      dest.srs_res,
      [](const srs_config::srs_resource& res) { return make_asn1_rrc_srs_res(res); },
      [](const srs_config::srs_resource& res) { return res.id; });

  if (dest.is_tpc_accum_disabled) {
    out.tpc_accumulation_present = true;
  }
}

static bool calculate_bwp_ul_dedicated_diff(asn1::rrc_nr::bwp_ul_ded_s& out,
                                            const bwp_uplink_dedicated& src,
                                            const bwp_uplink_dedicated& dest)
{
  if ((dest.pucch_cfg.has_value() && not src.pucch_cfg.has_value()) ||
      (dest.pucch_cfg.has_value() && src.pucch_cfg.has_value() && dest.pucch_cfg != src.pucch_cfg)) {
    out.pucch_cfg_present = true;
    calculate_pucch_config_diff(out.pucch_cfg.set_setup(),
                                src.pucch_cfg.has_value() ? src.pucch_cfg.value() : pucch_config{},
                                dest.pucch_cfg.value());
  } else if (src.pucch_cfg.has_value() && not dest.pucch_cfg.has_value()) {
    out.pucch_cfg_present = true;
    out.pucch_cfg.set_release();
  }

  if ((dest.pusch_cfg.has_value() && not src.pusch_cfg.has_value()) ||
      (dest.pusch_cfg.has_value() && src.pusch_cfg.has_value() && dest.pusch_cfg != src.pusch_cfg)) {
    out.pusch_cfg_present = true;
    calculate_pusch_config_diff(out.pusch_cfg.set_setup(),
                                src.pusch_cfg.has_value() ? src.pusch_cfg.value() : pusch_config{},
                                dest.pusch_cfg.value());
  } else if (src.pusch_cfg.has_value() && not dest.pusch_cfg.has_value()) {
    out.pusch_cfg_present = true;
    out.pusch_cfg.set_release();
  }

  if ((dest.srs_cfg.has_value() && not src.srs_cfg.has_value()) ||
      (dest.srs_cfg.has_value() && src.srs_cfg.has_value() && dest.srs_cfg != src.srs_cfg)) {
    out.srs_cfg_present = true;
    calculate_srs_config_diff(
        out.srs_cfg.set_setup(), src.srs_cfg.has_value() ? src.srs_cfg.value() : srs_config{}, dest.srs_cfg.value());
  } else if (src.srs_cfg.has_value() && not dest.srs_cfg.has_value()) {
    out.srs_cfg_present = true;
    out.srs_cfg.set_release();
  }
  // TODO: Remaining.

  return out.pucch_cfg_present || out.pusch_cfg_present || out.srs_cfg_present;
}

static bool
calculate_uplink_config_diff(asn1::rrc_nr::ul_cfg_s& out, const uplink_config& src, const uplink_config& dest)
{
  out.init_ul_bwp_present = calculate_bwp_ul_dedicated_diff(out.init_ul_bwp, src.init_ul_bwp, dest.init_ul_bwp);

  // TODO: Remaining.

  return out.init_ul_bwp_present;
}

void calculate_pdsch_serving_cell_cfg_diff(asn1::rrc_nr::pdsch_serving_cell_cfg_s& out,
                                           const pdsch_serving_cell_config&        src,
                                           const pdsch_serving_cell_config&        dest)
{
  if ((dest.code_block_group_tx.has_value() && not src.code_block_group_tx.has_value()) ||
      (dest.code_block_group_tx.has_value() && src.code_block_group_tx.has_value() &&
       dest.code_block_group_tx != src.code_block_group_tx)) {
    out.code_block_group_tx_present       = true;
    auto& cbg_grp_tx                      = out.code_block_group_tx.set_setup();
    cbg_grp_tx.code_block_group_flush_ind = dest.code_block_group_tx.value().code_block_group_flush_indicator;
    switch (dest.code_block_group_tx.value().max_cbg_per_tb) {
      case pdsch_code_block_group_transmission::max_code_block_groups_per_tb::n2:
        cbg_grp_tx.max_code_block_groups_per_transport_block =
            pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::n2;
        break;
      case pdsch_code_block_group_transmission::max_code_block_groups_per_tb::n4:
        cbg_grp_tx.max_code_block_groups_per_transport_block =
            pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::n4;
        break;
      case pdsch_code_block_group_transmission::max_code_block_groups_per_tb::n6:
        cbg_grp_tx.max_code_block_groups_per_transport_block =
            pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::n6;
        break;
      case pdsch_code_block_group_transmission::max_code_block_groups_per_tb::n8:
        cbg_grp_tx.max_code_block_groups_per_transport_block =
            pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::n8;
        break;
      default:
        srsran_assertion_failure("Invalid xOverhead={}", dest.x_ov_head);
    }
  } else if (src.code_block_group_tx.has_value() && not dest.code_block_group_tx.has_value()) {
    out.code_block_group_tx_present = true;
    out.code_block_group_tx.set_release();
  }

  if (dest.x_ov_head != x_overhead::not_set) {
    out.xoverhead_present = true;
    switch (dest.x_ov_head) {
      case x_overhead::xoh6:
        out.xoverhead = pdsch_serving_cell_cfg_s::xoverhead_opts::xoh6;
        break;
      case x_overhead::xoh12:
        out.xoverhead = pdsch_serving_cell_cfg_s::xoverhead_opts::xoh12;
        break;
      case x_overhead::xoh18:
        out.xoverhead = pdsch_serving_cell_cfg_s::xoverhead_opts::xoh18;
        break;
      default:
        srsran_assertion_failure("Invalid xOverhead={}", dest.x_ov_head);
    }
  }

  if (dest.nof_harq_proc != pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n8) {
    out.nrof_harq_processes_for_pdsch_present = true;
    switch (dest.nof_harq_proc) {
      case pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n2:
        out.nrof_harq_processes_for_pdsch = pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n2;
        break;
      case pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n4:
        out.nrof_harq_processes_for_pdsch = pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n4;
        break;
      case pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n6:
        out.nrof_harq_processes_for_pdsch = pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n6;
        break;
      case pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n10:
        out.nrof_harq_processes_for_pdsch = pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n10;
        break;
      case pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n12:
        out.nrof_harq_processes_for_pdsch = pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n12;
        break;
      case pdsch_serving_cell_config::nof_harq_proc_for_pdsch::n16:
        out.nrof_harq_processes_for_pdsch = pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n16;
        break;
      default:
        srsran_assertion_failure("Invalid max. nof.HARQ process for PDSCH={}", dest.nof_harq_proc);
    }
  }

  if (dest.pucch_cell.has_value()) {
    out.pucch_cell_present = true;
    out.pucch_cell         = dest.pucch_cell.value();
  }

  if (dest.max_mimo_layers > 0) {
    out.max_mimo_layers_present = true;
    out.max_mimo_layers         = dest.max_mimo_layers;
  }

  if (dest.processing_type_2_enabled.has_value()) {
    out.processing_type2_enabled_present = true;
    out.processing_type2_enabled         = dest.processing_type_2_enabled.value();
  }
}

static bool calculate_serving_cell_config_diff(asn1::rrc_nr::serving_cell_cfg_s& out,
                                               const serving_cell_config&        src,
                                               const serving_cell_config&        dest)
{
  out.init_dl_bwp_present = calculate_bwp_dl_dedicated_diff(out.init_dl_bwp, src.init_dl_bwp, dest.init_dl_bwp);

  // UplinkConfig.
  if (dest.ul_config.has_value()) {
    out.ul_cfg_present = calculate_uplink_config_diff(
        out.ul_cfg, src.ul_config.has_value() ? src.ul_config.value() : uplink_config{}, dest.ul_config.value());
  }

  // PDSCH-ServingCellConfig.
  if ((dest.pdsch_serv_cell_cfg.has_value() && not src.pdsch_serv_cell_cfg.has_value()) ||
      (dest.pdsch_serv_cell_cfg.has_value() && src.pdsch_serv_cell_cfg.has_value() &&
       dest.pdsch_serv_cell_cfg != src.pdsch_serv_cell_cfg)) {
    out.pdsch_serving_cell_cfg_present = true;
    calculate_pdsch_serving_cell_cfg_diff(out.pdsch_serving_cell_cfg.set_setup(),
                                          src.pdsch_serv_cell_cfg.has_value() ? src.pdsch_serv_cell_cfg.value()
                                                                              : pdsch_serving_cell_config{},
                                          dest.pdsch_serv_cell_cfg.value());
  } else if (src.pdsch_serv_cell_cfg.has_value() && not dest.pdsch_serv_cell_cfg.has_value()) {
    out.pdsch_serving_cell_cfg_present = true;
    out.pdsch_serving_cell_cfg.set_release();
  }

  // CSI-MeasConfig.
  if ((dest.csi_meas_cfg.has_value() && not src.csi_meas_cfg.has_value()) ||
      (dest.csi_meas_cfg.has_value() && src.csi_meas_cfg.has_value() && dest.csi_meas_cfg != src.csi_meas_cfg)) {
    out.csi_meas_cfg_present = true;
    calculate_csi_meas_config_diff(out.csi_meas_cfg.set_setup(),
                                   src.csi_meas_cfg.has_value() ? src.csi_meas_cfg.value() : csi_meas_config{},
                                   dest.csi_meas_cfg.value());
  } else if (src.csi_meas_cfg.has_value() && not dest.csi_meas_cfg.has_value()) {
    out.csi_meas_cfg_present = true;
    out.csi_meas_cfg.set_release();
  }

  // TAG-ID.
  out.tag_id = dest.tag_id;

  return out.init_dl_bwp_present || out.ul_cfg_present || out.pdsch_serving_cell_cfg_present ||
         out.csi_meas_cfg_present;
}

asn1::rrc_nr::sched_request_to_add_mod_s
srsran::srs_du::make_asn1_rrc_scheduling_request(const scheduling_request_to_addmod& cfg)
{
  sched_request_to_add_mod_s req{};
  req.sched_request_id = cfg.sr_id;
  if (cfg.prohibit_timer.has_value()) {
    req.sr_prohibit_timer_present = true;
    switch (cfg.prohibit_timer.value()) {
      case sr_prohib_timer::ms1:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms1;
        break;
      case sr_prohib_timer::ms2:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms2;
        break;
      case sr_prohib_timer::ms4:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms4;
        break;
      case sr_prohib_timer::ms8:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms8;
        break;
      case sr_prohib_timer::ms16:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms16;
        break;
      case sr_prohib_timer::ms32:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms32;
        break;
      case sr_prohib_timer::ms64:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms64;
        break;
      case sr_prohib_timer::ms128:
        req.sr_prohibit_timer = sched_request_to_add_mod_s::sr_prohibit_timer_opts::ms128;
        break;
      default:
        srsran_assertion_failure("Invalid SR prohibit timer={}", cfg.prohibit_timer.value());
    }
  }

  switch (cfg.max_tx) {
    case sr_max_tx::n4:
      req.sr_trans_max = sched_request_to_add_mod_s::sr_trans_max_opts::n4;
      break;
    case sr_max_tx::n8:
      req.sr_trans_max = sched_request_to_add_mod_s::sr_trans_max_opts::n8;
      break;
    case sr_max_tx::n16:
      req.sr_trans_max = sched_request_to_add_mod_s::sr_trans_max_opts::n16;
      break;
    case sr_max_tx::n32:
      req.sr_trans_max = sched_request_to_add_mod_s::sr_trans_max_opts::n32;
      break;
    case sr_max_tx::n64:
      req.sr_trans_max = sched_request_to_add_mod_s::sr_trans_max_opts::n64;
      break;
    default:
      srsran_assertion_failure("Invalid SR trans. max={}", cfg.max_tx);
  }

  return req;
}

void make_asn1_rrc_bsr_config(asn1::rrc_nr::bsr_cfg_s& out, const bsr_config& cfg)
{
  switch (cfg.retx_timer) {
    case retx_bsr_timer::sf10:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf10;
      break;
    case retx_bsr_timer::sf20:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf20;
      break;
    case retx_bsr_timer::sf40:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf40;
      break;
    case retx_bsr_timer::sf80:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf80;
      break;
    case retx_bsr_timer::sf160:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf160;
      break;
    case retx_bsr_timer::sf320:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf320;
      break;
    case retx_bsr_timer::sf640:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf640;
      break;
    case retx_bsr_timer::sf1280:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf1280;
      break;
    case retx_bsr_timer::sf2560:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf2560;
      break;
    case retx_bsr_timer::sf5120:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf5120;
      break;
    case retx_bsr_timer::sf10240:
      out.retx_bsr_timer = bsr_cfg_s::retx_bsr_timer_opts::sf10240;
      break;
    default:
      srsran_assertion_failure("Invalid BSR Retx. Timer={}", cfg.retx_timer);
  }

  switch (cfg.periodic_timer) {
    case periodic_bsr_timer::sf1:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf1;
      break;
    case periodic_bsr_timer::sf5:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf5;
      break;
    case periodic_bsr_timer::sf10:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf10;
      break;
    case periodic_bsr_timer::sf16:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf16;
      break;
    case periodic_bsr_timer::sf20:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf20;
      break;
    case periodic_bsr_timer::sf32:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf32;
      break;
    case periodic_bsr_timer::sf40:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf40;
      break;
    case periodic_bsr_timer::sf64:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf64;
      break;
    case periodic_bsr_timer::sf80:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf80;
      break;
    case periodic_bsr_timer::sf128:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf128;
      break;
    case periodic_bsr_timer::sf160:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf160;
      break;
    case periodic_bsr_timer::sf320:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf320;
      break;
    case periodic_bsr_timer::sf640:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf640;
      break;
    case periodic_bsr_timer::sf1280:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf1280;
      break;
    case periodic_bsr_timer::sf2560:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::sf2560;
      break;
    case periodic_bsr_timer::infinity:
      out.periodic_bsr_timer = bsr_cfg_s::periodic_bsr_timer_opts::infinity;
      break;
    default:
      srsran_assertion_failure("Invalid Periodic BSR Timer={}", cfg.periodic_timer);
  }

  if (cfg.lc_sr_delay_timer.has_value()) {
    out.lc_ch_sr_delay_timer_present = true;
    switch (cfg.lc_sr_delay_timer.value()) {
      case logical_channel_sr_delay_timer::sf20:
        out.lc_ch_sr_delay_timer = bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf20;
        break;
      case logical_channel_sr_delay_timer::sf40:
        out.lc_ch_sr_delay_timer = bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf40;
        break;
      case logical_channel_sr_delay_timer::sf64:
        out.lc_ch_sr_delay_timer = bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf64;
        break;
      case logical_channel_sr_delay_timer::sf128:
        out.lc_ch_sr_delay_timer = bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf128;
        break;
      case logical_channel_sr_delay_timer::sf512:
        out.lc_ch_sr_delay_timer = bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf512;
        break;
      case logical_channel_sr_delay_timer::sf1024:
        out.lc_ch_sr_delay_timer = bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf1024;
        break;
      case logical_channel_sr_delay_timer::sf2560:
        out.lc_ch_sr_delay_timer = bsr_cfg_s::lc_ch_sr_delay_timer_opts::sf2560;
        break;
      default:
        srsran_assertion_failure("Invalid BSR LC SR Delay Timer={}", cfg.lc_sr_delay_timer.value());
    }
  }
}

asn1::rrc_nr::tag_s srsran::srs_du::make_asn1_rrc_tag_config(const tag& cfg)
{
  tag_s tag_cfg{};

  tag_cfg.tag_id = cfg.tag_id;
  switch (cfg.ta_timer) {
    case time_alignment_timer::ms500:
      tag_cfg.time_align_timer = time_align_timer_opts::ms500;
      break;
    case time_alignment_timer::ms750:
      tag_cfg.time_align_timer = time_align_timer_opts::ms750;
      break;
    case time_alignment_timer::ms1280:
      tag_cfg.time_align_timer = time_align_timer_opts::ms1280;
      break;
    case time_alignment_timer::ms1920:
      tag_cfg.time_align_timer = time_align_timer_opts::ms1920;
      break;
    case time_alignment_timer::ms2560:
      tag_cfg.time_align_timer = time_align_timer_opts::ms2560;
      break;
    case time_alignment_timer::ms5120:
      tag_cfg.time_align_timer = time_align_timer_opts::ms5120;
      break;
    case time_alignment_timer::ms10240:
      tag_cfg.time_align_timer = time_align_timer_opts::ms10240;
      break;
    case time_alignment_timer::infinity:
      tag_cfg.time_align_timer = time_align_timer_opts::infinity;
      break;
    default:
      srsran_assertion_failure("Invalid Time Align. Timer={}", cfg.ta_timer);
  }

  return tag_cfg;
}

void make_asn1_rrc_phr_config(asn1::rrc_nr::phr_cfg_s& out, const phr_config& cfg)
{
  switch (cfg.periodic_timer) {
    case phr_periodic_timer::sf10:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::sf10;
      break;
    case phr_periodic_timer::sf20:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::sf20;
      break;
    case phr_periodic_timer::sf50:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::sf50;
      break;
    case phr_periodic_timer::sf100:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::sf100;
      break;
    case phr_periodic_timer::sf200:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::sf200;
      break;
    case phr_periodic_timer::sf500:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::sf500;
      break;
    case phr_periodic_timer::sf1000:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::sf1000;
      break;
    case phr_periodic_timer::infinity:
      out.phr_periodic_timer = phr_cfg_s::phr_periodic_timer_opts::infinity;
      break;
    default:
      srsran_assertion_failure("Invalid PHR Periodic Timer={}", cfg.periodic_timer);
  }

  switch (cfg.prohibit_timer) {
    case phr_prohibit_timer::sf0:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf0;
      break;
    case phr_prohibit_timer::sf10:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf10;
      break;
    case phr_prohibit_timer::sf20:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf20;
      break;
    case phr_prohibit_timer::sf50:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf50;
      break;
    case phr_prohibit_timer::sf100:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf100;
      break;
    case phr_prohibit_timer::sf200:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf200;
      break;
    case phr_prohibit_timer::sf500:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf500;
      break;
    case phr_prohibit_timer::sf1000:
      out.phr_prohibit_timer = phr_cfg_s::phr_prohibit_timer_opts::sf1000;
      break;
    default:
      srsran_assertion_failure("Invalid PHR Prohibit Timer={}", cfg.prohibit_timer);
  }

  switch (cfg.power_factor_change) {
    case phr_tx_power_factor_change::db1:
      out.phr_tx_pwr_factor_change = phr_cfg_s::phr_tx_pwr_factor_change_opts::db1;
      break;
    case phr_tx_power_factor_change::db3:
      out.phr_tx_pwr_factor_change = phr_cfg_s::phr_tx_pwr_factor_change_opts::db3;
      break;
    case phr_tx_power_factor_change::db6:
      out.phr_tx_pwr_factor_change = phr_cfg_s::phr_tx_pwr_factor_change_opts::db6;
      break;
    case phr_tx_power_factor_change::infinity:
      out.phr_tx_pwr_factor_change = phr_cfg_s::phr_tx_pwr_factor_change_opts::infinity;
      break;
    default:
      srsran_assertion_failure("Invalid PHR Prohibit Timer={}", cfg.power_factor_change);
  }

  out.multiple_phr         = cfg.multiple_phr;
  out.dummy                = cfg.dummy;
  out.phr_type2_other_cell = cfg.phr_type_to_other_cell;

  switch (cfg.phr_mode) {
    case phr_mode_other_cg::real:
      out.phr_mode_other_cg = phr_cfg_s::phr_mode_other_cg_opts::real;
      break;
    case phr_mode_other_cg::virtual_:
      out.phr_mode_other_cg = phr_cfg_s::phr_mode_other_cg_opts::virtual_value;
      break;
    default:
      srsran_assertion_failure("Invalid PHR Mode={}", cfg.phr_mode);
  }
}

static bool calculate_mac_cell_group_config_diff(asn1::rrc_nr::mac_cell_group_cfg_s& out,
                                                 const mac_cell_group_config&        src,
                                                 const mac_cell_group_config&        dest)
{
  calculate_addmodremlist_diff(
      out.sched_request_cfg.sched_request_to_add_mod_list,
      out.sched_request_cfg.sched_request_to_release_list,
      src.scheduling_request_config,
      dest.scheduling_request_config,
      [](const scheduling_request_to_addmod& req) { return make_asn1_rrc_scheduling_request(req); },
      [](const scheduling_request_to_addmod& req) { return req.sr_id; });
  if (out.sched_request_cfg.sched_request_to_add_mod_list.size() > 0 ||
      out.sched_request_cfg.sched_request_to_release_list.size() > 0) {
    out.sched_request_cfg_present = true;
  }

  if (dest.bsr_cfg.has_value()) {
    out.bsr_cfg_present = true;
    make_asn1_rrc_bsr_config(out.bsr_cfg, dest.bsr_cfg.value());
  }

  calculate_addmodremlist_diff(
      out.tag_cfg.tag_to_add_mod_list,
      out.tag_cfg.tag_to_release_list,
      src.tag_config,
      dest.tag_config,
      [](const tag& t) { return make_asn1_rrc_tag_config(t); },
      [](const tag& t) { return t.tag_id; });
  if (out.tag_cfg.tag_to_add_mod_list.size() > 0 || out.tag_cfg.tag_to_release_list.size() > 0) {
    out.tag_cfg_present = true;
  }

  if ((dest.phr_cfg.has_value() && not src.phr_cfg.has_value()) ||
      (dest.phr_cfg.has_value() && src.phr_cfg.has_value() && dest.phr_cfg != src.phr_cfg)) {
    out.phr_cfg_present = true;
    make_asn1_rrc_phr_config(out.phr_cfg.set_setup(), dest.phr_cfg.value());
  } else if (src.phr_cfg.has_value() && not dest.phr_cfg.has_value()) {
    out.phr_cfg_present = true;
    out.phr_cfg.set_release();
  }

  out.skip_ul_tx_dyn = dest.skip_uplink_tx_dynamic;

  return out.sched_request_cfg_present || out.bsr_cfg_present || out.tag_cfg_present || out.phr_cfg_present;
}

void srsran::srs_du::calculate_cell_group_config_diff(asn1::rrc_nr::cell_group_cfg_s& out,
                                                      const cell_group_config&        src,
                                                      const cell_group_config&        dest)
{
  calculate_addmodremlist_diff(
      out.rlc_bearer_to_add_mod_list,
      out.rlc_bearer_to_release_list,
      src.rlc_bearers,
      dest.rlc_bearers,
      [](const rlc_bearer_config& b) { return make_asn1_rrc_rlc_bearer(b); },
      [](const rlc_bearer_config& b) { return (uint8_t)b.lcid; });

  if (dest.cells.contains(0)) {
    out.sp_cell_cfg.sp_cell_cfg_ded_present =
        calculate_serving_cell_config_diff(out.sp_cell_cfg.sp_cell_cfg_ded,
                                           src.cells.contains(0) ? src.cells[0].serv_cell_cfg : serving_cell_config{},
                                           dest.cells[0].serv_cell_cfg);

    out.sp_cell_cfg_present = out.sp_cell_cfg.sp_cell_cfg_ded_present;
  }

  out.mac_cell_group_cfg_present =
      calculate_mac_cell_group_config_diff(out.mac_cell_group_cfg, src.mcg_cfg, dest.mcg_cfg);

  out.phys_cell_group_cfg_present = true;
  if (dest.pcg_cfg.p_nr_fr1.has_value()) {
    out.phys_cell_group_cfg.p_nr_fr1_present = true;
    out.phys_cell_group_cfg.p_nr_fr1         = dest.pcg_cfg.p_nr_fr1.value();
  }
  out.phys_cell_group_cfg.pdsch_harq_ack_codebook.value =
      dest.pcg_cfg.pdsch_harq_codebook == pdsch_harq_ack_codebook::dynamic
          ? phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dyn
          : asn1::rrc_nr::phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::semi_static;
}

bool srsran::srs_du::calculate_reconfig_with_sync_diff(asn1::rrc_nr::recfg_with_sync_s&    out,
                                                       const du_cell_config&               du_cell_cfg,
                                                       const cell_group_config&            dest,
                                                       const asn1::rrc_nr::ho_prep_info_s& ho_prep_info,
                                                       rnti_t                              rnti)
{
  out.sp_cell_cfg_common_present = true;

  // > physCellId PhysCellId OPTIONAL, -- Cond HOAndServCellAdd
  out.sp_cell_cfg_common.pci_present = true;
  out.sp_cell_cfg_common.pci         = (uint16_t)du_cell_cfg.pci;

  // > downlinkConfigCommon DownlinkConfigCommon OPTIONAL, -- Cond HOAndServCellAdd
  out.sp_cell_cfg_common.dl_cfg_common_present = true;
  out.sp_cell_cfg_common.dl_cfg_common         = make_asn1_rrc_dl_cfg_common(du_cell_cfg);

  // > uplinkConfigCommon UplinkConfigCommon OPTIONAL, -- Need M
  out.sp_cell_cfg_common.ul_cfg_common_present = true;
  out.sp_cell_cfg_common.ul_cfg_common         = make_asn1_rrc_ul_cfg_common(du_cell_cfg.ul_cfg_common);

  n_ta_offset ta_offset                                  = band_helper::get_ta_offset(du_cell_cfg.dl_carrier.band);
  out.sp_cell_cfg_common.n_timing_advance_offset_present = true;
  if (ta_offset == n_ta_offset::n0) {
    out.sp_cell_cfg_common.n_timing_advance_offset.value = serving_cell_cfg_common_s::n_timing_advance_offset_opts::n0;
  } else if (ta_offset == n_ta_offset::n25600) {
    out.sp_cell_cfg_common.n_timing_advance_offset.value =
        serving_cell_cfg_common_s::n_timing_advance_offset_opts::n25600;
  } else if (ta_offset == n_ta_offset::n39936) {
    out.sp_cell_cfg_common.n_timing_advance_offset.value =
        serving_cell_cfg_common_s::n_timing_advance_offset_opts::n39936;
  }

  // TODO: check how to fill this field.
  out.sp_cell_cfg_common.ssb_positions_in_burst_present = true;
  out.sp_cell_cfg_common.ssb_positions_in_burst.set_medium_bitmap().from_number(
      static_cast<uint64_t>(du_cell_cfg.ssb_cfg.ssb_bitmap) >> static_cast<uint64_t>(56U));

  out.sp_cell_cfg_common.ssb_periodicity_serving_cell_present = true;
  asn1::number_to_enum(out.sp_cell_cfg_common.ssb_periodicity_serving_cell,
                       ssb_periodicity_to_value(du_cell_cfg.ssb_cfg.ssb_period));

  out.sp_cell_cfg_common.dmrs_type_a_position.value = du_cell_cfg.dmrs_typeA_pos == dmrs_typeA_position::pos2
                                                          ? serving_cell_cfg_common_s::dmrs_type_a_position_opts::pos2
                                                          : serving_cell_cfg_common_s::dmrs_type_a_position_opts::pos3;

  // > ssbSubcarrierSpacing SubcarrierSpacing OPTIONAL, -- Cond HOAndServCellWithSSB
  out.sp_cell_cfg_common.ssb_subcarrier_spacing_present = true;
  out.sp_cell_cfg_common.ssb_subcarrier_spacing.value   = get_asn1_scs(du_cell_cfg.ssb_cfg.scs);

  // > tdd-UL-DL-ConfigurationCommon TDD-UL-DL-ConfigCommon OPTIONAL, -- Cond TDD
  out.sp_cell_cfg_common.tdd_ul_dl_cfg_common_present = du_cell_cfg.tdd_ul_dl_cfg_common.has_value();
  if (du_cell_cfg.tdd_ul_dl_cfg_common.has_value()) {
    out.sp_cell_cfg_common.tdd_ul_dl_cfg_common =
        make_asn1_rrc_tdd_ul_dl_cfg_common(du_cell_cfg.tdd_ul_dl_cfg_common.value());
  }

  // ss-PBCH-BlockPower INTEGER (-60..50)
  out.sp_cell_cfg_common.ss_pbch_block_pwr = du_cell_cfg.ssb_cfg.ssb_block_power;

  out.new_ue_id = rnti;

  out.t304.value = recfg_with_sync_s::t304_opts::ms2000;

  // TODO

  return true;
}
