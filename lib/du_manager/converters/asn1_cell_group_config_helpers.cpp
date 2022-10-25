/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "asn1_cell_group_config_helpers.h"
#include "../../asn1/asn1_diff_utils.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;
using namespace asn1::rrc_nr;

asn1::rrc_nr::coreset_s srsgnb::srs_du::make_asn1_rrc_coreset(const coreset_configuration& cfg)
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
  cs.pdcch_dmrs_scrambling_id_present = true;
  cs.pdcch_dmrs_scrambling_id         = cfg.pdcch_dmrs_scrambling_id;
  return cs;
}

asn1::rrc_nr::search_space_s srsgnb::srs_du::make_asn1_rrc_search_space(const search_space_configuration& cfg)
{
  search_space_s ss;
  ss.search_space_id                                = cfg.id;
  ss.coreset_id_present                             = true;
  ss.coreset_id                                     = cfg.cs_id;
  ss.monitoring_slot_periodicity_and_offset_present = true;
  search_space_s::monitoring_slot_periodicity_and_offset_c_::types period;
  bool success = asn1::number_to_enum(period, cfg.monitoring_slot_period);
  srsgnb_assert(success, "Invalid slot period");
  ss.monitoring_slot_periodicity_and_offset.set(period);
  switch (ss.monitoring_slot_periodicity_and_offset.type().value) {
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl1:
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl2:
      ss.monitoring_slot_periodicity_and_offset.sl2() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl4:
      ss.monitoring_slot_periodicity_and_offset.sl4() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl5:
      ss.monitoring_slot_periodicity_and_offset.sl5() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl8:
      ss.monitoring_slot_periodicity_and_offset.sl8() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl10:
      ss.monitoring_slot_periodicity_and_offset.sl10() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl16:
      ss.monitoring_slot_periodicity_and_offset.sl16() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl20:
      ss.monitoring_slot_periodicity_and_offset.sl20() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl40:
      ss.monitoring_slot_periodicity_and_offset.sl40() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl80:
      ss.monitoring_slot_periodicity_and_offset.sl80() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl160:
      ss.monitoring_slot_periodicity_and_offset.sl160() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl320:
      ss.monitoring_slot_periodicity_and_offset.sl320() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl640:
      ss.monitoring_slot_periodicity_and_offset.sl640() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl1280:
      ss.monitoring_slot_periodicity_and_offset.sl1280() = cfg.monitoring_slot_offset;
      break;
    case search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::sl2560:
      ss.monitoring_slot_periodicity_and_offset.sl2560() = cfg.monitoring_slot_offset;
      break;
    default:
      srsgnb_assertion_failure("Invalid PDCCH slot offset={}", cfg.monitoring_slot_offset);
  }
  if (cfg.duration != 1) {
    ss.dur_present = true;
    ss.dur         = cfg.duration;
  }
  if (cfg.monitoring_symbols_within_slot.has_value()) {
    ss.monitoring_symbols_within_slot_present = true;
    ss.monitoring_symbols_within_slot.from_number(cfg.monitoring_symbols_within_slot->to_ulong());
  }
  ss.nrof_candidates_present = true;
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level1, cfg.nof_candidates[0]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level2, cfg.nof_candidates[1]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level4, cfg.nof_candidates[2]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level8, cfg.nof_candidates[3]);
  asn1::number_to_enum(ss.nrof_candidates.aggregation_level16, cfg.nof_candidates[4]);
  ss.search_space_type_present = true;
  if (cfg.type == search_space_configuration::type::common) {
    ss.search_space_type.set_common();
    ss.search_space_type.common().dci_format0_minus0_and_format1_minus0_present = cfg.common.f0_0_and_f1_0;
    ss.search_space_type.common().dci_format2_minus0_present                    = cfg.common.f2_0;
    ss.search_space_type.common().dci_format2_minus1_present                    = cfg.common.f2_1;
    ss.search_space_type.common().dci_format2_minus2_present                    = cfg.common.f2_2;
    ss.search_space_type.common().dci_format2_minus3_present                    = cfg.common.f2_3;
  } else {
    ss.search_space_type.set_ue_specific();
    ss.search_space_type.ue_specific().dci_formats.value =
        cfg.ue_specific == srsgnb::search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0
            ? search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_minus0_and_minus1_minus0
            : search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_minus1_and_minus1_minus1;
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
    out.served_radio_bearer.set_drb_id() = drb_id_to_uint(cfg.drb_id);
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
      um.ul_um_rlc.sn_field_len.value   = asn1::rrc_nr::sn_field_len_um_opts::size12;
      // TODO
    } break;
    case rlc_mode::um_unidir_dl: {
      auto& um                          = out.rlc_cfg.set_um_uni_dir_dl();
      um.dl_um_rlc.sn_field_len_present = true;
      um.dl_um_rlc.sn_field_len.value   = asn1::rrc_nr::sn_field_len_um_opts::size12;
      // TODO
    } break;
    case rlc_mode::um_unidir_ul: {
      auto& um                          = out.rlc_cfg.set_um_uni_dir_ul();
      um.ul_um_rlc.sn_field_len_present = true;
      um.ul_um_rlc.sn_field_len.value   = asn1::rrc_nr::sn_field_len_um_opts::size12;
      // TODO
    } break;
    default:
      report_fatal_error("Invalid RLC bearer configuration type");
  }

  out.mac_lc_ch_cfg_present                    = true;
  out.mac_lc_ch_cfg.ul_specific_params_present = true;
  out.mac_lc_ch_cfg.ul_specific_params.prio    = 1;
  out.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate.value =
      lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::infinity;
  out.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur.value =
      lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms5;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present          = true;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group                  = 0;
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present     = true;
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id             = 0;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_mask                = false;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_delay_timer_applied = false;

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
      [](const search_space_configuration& ss) { return (uint8_t)ss.id; });

  // TODO: Remaining.
}

void calculate_pdsch_config_diff(asn1::rrc_nr::pdsch_cfg_s& out, const pdsch_config& src, const pdsch_config& dest)
{
  out.data_scrambling_id_pdsch_present = dest.data_scrambling_id_pdsch.has_value();
  if (out.data_scrambling_id_pdsch_present) {
    out.data_scrambling_id_pdsch = dest.data_scrambling_id_pdsch.value();
  }

  out.dmrs_dl_for_pdsch_map_type_a_present = true;
  dmrs_dl_cfg_s& dmrsA                     = out.dmrs_dl_for_pdsch_map_type_a.set_setup();
  dmrsA.dmrs_add_position_present          = true;
  dmrsA.dmrs_add_position.value            = dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;

  out.tci_states_to_add_mod_list.resize(1);
  out.tci_states_to_add_mod_list[0].tci_state_id = 0;
  out.tci_states_to_add_mod_list[0].qcl_type1.ref_sig.set_ssb();
  out.tci_states_to_add_mod_list[0].qcl_type1.ref_sig.ssb() = 0;
  out.tci_states_to_add_mod_list[0].qcl_type1.qcl_type      = asn1::rrc_nr::qcl_info_s::qcl_type_opts::type_d;

  out.res_alloc.value = asn1::rrc_nr::pdsch_cfg_s::res_alloc_opts::res_alloc_type1;
  out.rbg_size.value  = asn1::rrc_nr::pdsch_cfg_s::rbg_size_opts::cfg1;
  out.prb_bundling_type.set_static_bundling();
  out.prb_bundling_type.static_bundling().bundle_size_present = true;
  out.prb_bundling_type.static_bundling().bundle_size =
      pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::wideband;

  // TODO: Remaining.
}

void calculate_bwp_dl_dedicated_diff(asn1::rrc_nr::bwp_dl_ded_s&   out,
                                     const bwp_downlink_dedicated& src,
                                     const bwp_downlink_dedicated& dest)
{
  if (dest.pdcch_cfg.has_value()) {
    out.pdcch_cfg_present = true;
    calculate_pdcch_config_diff(
        out.pdcch_cfg.set_setup(), src.pdcch_cfg.has_value() ? *src.pdcch_cfg : pdcch_config{}, *dest.pdcch_cfg);
  }

  if (dest.pdsch_cfg.has_value()) {
    out.pdsch_cfg_present = true;
    calculate_pdsch_config_diff(
        out.pdsch_cfg.set_setup(), src.pdcch_cfg.has_value() ? *dest.pdsch_cfg : pdsch_config{}, *dest.pdsch_cfg);
  }
  // TODO: Remaining.
}

asn1::rrc_nr::pucch_res_set_s srsgnb::srs_du::make_asn1_rrc_pucch_resource_set(const pucch_resource_set& cfg)
{
  pucch_res_set_s pucch_res_s;
  pucch_res_s.pucch_res_set_id = cfg.pucch_res_set_id;
  for (const auto& it : cfg.pucch_res_id_list) {
    pucch_res_s.res_list.push_back(it);
  }
  if (cfg.max_payload_size.has_value()) {
    pucch_res_s.max_payload_size = cfg.max_payload_size.value();
  }
  return pucch_res_s;
}

void make_asn1_rrc_pucch_formats_common_param(asn1::rrc_nr::pucch_format_cfg_s& out,
                                              const pucch_common_all_formats&   cfg)
{
  out.interslot_freq_hop_present = cfg.interslot_freq_hop;
  out.add_dmrs_present           = cfg.additional_dmrs;
  out.pi2_bpsk_present           = cfg.pi_2_bpsk;
  out.simul_harq_ack_csi_present = cfg.simultaneous_harq_ack_csi;
  out.max_code_rate_present      = true;
  switch (cfg.max_c_rate) {
    case pucch_common_all_formats::max_code_rate::dot_08:
      out.max_code_rate = pucch_max_code_rate_opts::zero_dot08;
      break;
    case pucch_common_all_formats::max_code_rate::dot_15:
      out.max_code_rate = pucch_max_code_rate_opts::zero_dot15;
      break;
    case pucch_common_all_formats::max_code_rate::dot_25:
      out.max_code_rate = pucch_max_code_rate_opts::zero_dot25;
      break;
    case pucch_common_all_formats::max_code_rate::dot_35:
      out.max_code_rate = pucch_max_code_rate_opts::zero_dot35;
      break;
    case pucch_common_all_formats::max_code_rate::dot_45:
      out.max_code_rate = pucch_max_code_rate_opts::zero_dot45;
      break;
    case pucch_common_all_formats::max_code_rate::dot_60:
      out.max_code_rate = pucch_max_code_rate_opts::zero_dot60;
      break;
    case pucch_common_all_formats::max_code_rate::dot_80:
      out.max_code_rate = pucch_max_code_rate_opts::zero_dot80;
      break;
    default:
      srsgnb_assertion_failure("Invalid PUCCH Common Format 1 Max. Code Rate={}", cfg.max_c_rate);
  }
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
      srsgnb_assertion_failure("Invalid PUCCH Common Format 1 No. of slots={}", cfg.nof_slots);
  }
}

asn1::rrc_nr::pucch_res_s srsgnb::srs_du::make_asn1_rrc_pucch_resource(const pucch_resource& cfg)
{
  pucch_res_s pucch_res;
  pucch_res.pucch_res_id                = cfg.res_id;
  pucch_res.start_prb                   = cfg.starting_prb;
  pucch_res.intra_slot_freq_hop_present = cfg.intraslot_freq_hopping;
  pucch_res.second_hop_prb              = cfg.second_hop_prb;
  switch (cfg.format) {
    case pucch_format::FORMAT_0: {
      auto& format0             = pucch_res.format.set_format0();
      format0.init_cyclic_shift = cfg.format_0.initial_cyclic_shift;
      format0.nrof_symbols      = cfg.format_0.nof_symbols;
      format0.start_symbol_idx  = cfg.format_0.starting_sym_idx;
    } break;
    case pucch_format::FORMAT_1: {
      auto& format1             = pucch_res.format.set_format1();
      format1.init_cyclic_shift = cfg.format_1.initial_cyclic_shift;
      format1.nrof_symbols      = cfg.format_1.nof_symbols;
      format1.start_symbol_idx  = cfg.format_1.starting_sym_idx;
      format1.time_domain_occ   = cfg.format_1.time_domain_occ;
    } break;
    case pucch_format::FORMAT_2: {
      auto& format2            = pucch_res.format.set_format2();
      format2.start_symbol_idx = cfg.format_2.starting_sym_idx;
      format2.nrof_symbols     = cfg.format_2.nof_symbols;
      format2.nrof_prbs        = cfg.format_2.nof_prbs;
    } break;
    case pucch_format::FORMAT_3: {
      auto& format3            = pucch_res.format.set_format3();
      format3.start_symbol_idx = cfg.format_3.starting_sym_idx;
      format3.nrof_symbols     = cfg.format_3.nof_symbols;
      format3.nrof_prbs        = cfg.format_3.nof_prbs;
    } break;
    case pucch_format::FORMAT_4: {
      auto& format4            = pucch_res.format.set_format4();
      format4.start_symbol_idx = cfg.format_4.starting_sym_idx;
      format4.nrof_symbols     = cfg.format_4.nof_symbols;
      pucch_format4_s::occ_len_e_ format4_length;
      switch (cfg.format_4.occ_index) {
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
          srsgnb_assertion_failure("Invalid PUCCH Format 4 index={}", cfg.format_4.occ_index);
      }
      switch (cfg.format_4.occ_length) {
        case pucch_f4_occ_len::n2:
          format4.occ_len = pucch_format4_s::occ_len_opts::n2;
          break;
        case pucch_f4_occ_len::n4:
          format4.occ_len = pucch_format4_s::occ_len_opts::n4;
          break;
        default:
          srsgnb_assertion_failure("Invalid PUCCH Format 4 length={}", cfg.format_4.occ_length);
      }
    } break;
    default:
      srsgnb_assertion_failure("Invalid PDCCH resource format={}", cfg.format);
  }
  return pucch_res;
}

asn1::rrc_nr::sched_request_res_cfg_s
srsgnb::srs_du::make_asn1_rrc_sr_resource(const scheduling_request_resource_config& cfg)
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
      srsgnb_assertion_failure("Invalid SR periodicity={}", cfg.period);
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

  if (dest.format_1_common_param.has_value()) {
    out.format1_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format1.set_setup(), dest.format_1_common_param.value());
  }
  if (dest.format_2_common_param.has_value()) {
    out.format2_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format2.set_setup(), dest.format_2_common_param.value());
  }
  if (dest.format_3_common_param.has_value()) {
    out.format3_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format3.set_setup(), dest.format_3_common_param.value());
  }
  if (dest.format_4_common_param.has_value()) {
    out.format4_present = true;
    make_asn1_rrc_pucch_formats_common_param(out.format4.set_setup(), dest.format_4_common_param.value());
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

void calculate_bwp_ul_dedicated_diff(asn1::rrc_nr::bwp_ul_ded_s& out,
                                     const bwp_uplink_dedicated& src,
                                     const bwp_uplink_dedicated& dest)
{
  if (dest.pucch_cfg.has_value()) {
    out.pucch_cfg_present = true;
    calculate_pucch_config_diff(out.pucch_cfg.set_setup(),
                                src.pucch_cfg.has_value() ? src.pucch_cfg.value() : pucch_config{},
                                dest.pucch_cfg.value());
  }
  // TODO: Remaining.
}

void calculate_uplink_config_diff(asn1::rrc_nr::ul_cfg_s& out, const uplink_config& src, const uplink_config& dest)
{
  out.init_ul_bwp_present = true;
  calculate_bwp_ul_dedicated_diff(out.init_ul_bwp, src.init_ul_bwp, dest.init_ul_bwp);

  // TODO: Remaining.
}

void calculate_serving_cell_config_diff(const serving_cell_config&        src,
                                        const serving_cell_config&        dest,
                                        asn1::rrc_nr::serving_cell_cfg_s& out)
{
  out.init_dl_bwp_present = true;
  calculate_bwp_dl_dedicated_diff(out.init_dl_bwp, src.init_dl_bwp, dest.init_dl_bwp);
  if (dest.ul_config.has_value()) {
    out.ul_cfg_present = true;
    calculate_uplink_config_diff(
        out.ul_cfg, src.ul_config.has_value() ? *src.ul_config : uplink_config{}, *dest.ul_config);
  }
}

void srsgnb::srs_du::calculate_cell_group_config_diff(const cell_group_config&        src,
                                                      const cell_group_config&        dest,
                                                      asn1::rrc_nr::cell_group_cfg_s& out)
{
  calculate_addmodremlist_diff(
      out.rlc_bearer_to_add_mod_list,
      out.rlc_bearer_to_release_list,
      src.rlc_bearers,
      dest.rlc_bearers,
      [](const rlc_bearer_config& b) { return make_asn1_rrc_rlc_bearer(b); },
      [](const rlc_bearer_config& b) { return (uint8_t)b.lcid; });

  out.sp_cell_cfg_present                 = true;
  out.sp_cell_cfg.serv_cell_idx_present   = true;
  out.sp_cell_cfg.serv_cell_idx           = dest.spcell_cfg.serv_cell_idx;
  out.sp_cell_cfg.sp_cell_cfg_ded_present = true;
  calculate_serving_cell_config_diff(
      src.spcell_cfg.spcell_cfg_ded, dest.spcell_cfg.spcell_cfg_ded, out.sp_cell_cfg.sp_cell_cfg_ded);
}
