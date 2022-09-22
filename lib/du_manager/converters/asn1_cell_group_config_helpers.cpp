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

void calculate_pdcch_config_diff(const pdcch_config& src, const pdcch_config& dest, asn1::rrc_nr::pdcch_cfg_s& out)
{
  calculate_addmodremlist_diff(
      src.coreset_to_addmod_list,
      dest.coreset_to_addmod_list,
      out.coreset_to_add_mod_list,
      out.coreset_to_release_list,
      [](const coreset_configuration& cs_cfg) { return make_asn1_rrc_coreset(cs_cfg); },
      [](const coreset_configuration& cs_cfg) { return (uint8_t)cs_cfg.id; });

  calculate_addmodremlist_diff(
      src.ss_to_addmod_list,
      dest.ss_to_addmod_list,
      out.search_spaces_to_add_mod_list,
      out.search_spaces_to_release_list,
      [](const search_space_configuration& ss) { return make_asn1_rrc_search_space(ss); },
      [](const search_space_configuration& ss) { return (uint8_t)ss.id; });

  // TODO: Remaining.
}

void calculate_pdsch_config_diff(const pdsch_config& src, const pdsch_config& dest, asn1::rrc_nr::pdsch_cfg_s& out)
{
  out.data_scrambling_id_pdsch_present = dest.data_scrambling_id_pdsch.has_value();
  if (out.data_scrambling_id_pdsch_present) {
    out.data_scrambling_id_pdsch = dest.data_scrambling_id_pdsch.value();
  }

  out.dmrs_dl_for_pdsch_map_type_a_present = true;
  dmrs_dl_cfg_s& dmrsA                     = out.dmrs_dl_for_pdsch_map_type_a.set_setup();
  dmrsA.dmrs_add_position_present          = true;
  dmrsA.dmrs_add_position.value            = dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;

  // TODO: Remaining.
}

void calculate_bwp_dl_dedicated_diff(const bwp_downlink_dedicated& src,
                                     const bwp_downlink_dedicated& dest,
                                     asn1::rrc_nr::bwp_dl_ded_s&   out)
{
  if (dest.pdcch_cfg.has_value()) {
    out.pdcch_cfg_present = true;
    calculate_pdcch_config_diff(*src.pdcch_cfg, *dest.pdcch_cfg, out.pdcch_cfg.set_setup());
  }

  if (dest.pdsch_cfg.has_value()) {
    out.pdsch_cfg_present = true;
    calculate_pdsch_config_diff(*src.pdsch_cfg, *dest.pdsch_cfg, out.pdsch_cfg.set_setup());
  }

  // TODO: Remaining.
}

void calculate_serving_cell_config_diff(const serving_cell_config&        src,
                                        const serving_cell_config&        dest,
                                        asn1::rrc_nr::serving_cell_cfg_s& out)
{
  out.init_dl_bwp_present = true;
  calculate_bwp_dl_dedicated_diff(src.init_dl_bwp, dest.init_dl_bwp, out.init_dl_bwp);
}

void srsgnb::srs_du::calculate_cell_group_config_diff(const cell_group_config&        src,
                                                      const cell_group_config&        dest,
                                                      asn1::rrc_nr::cell_group_cfg_s& out)
{
  calculate_addmodremlist_diff(
      src.rlc_bearers,
      dest.rlc_bearers,
      out.rlc_bearer_to_add_mod_list,
      out.rlc_bearer_to_release_list,
      [](const rlc_bearer_config& b) { return make_asn1_rrc_rlc_bearer(b); },
      [](const rlc_bearer_config& b) { return (uint8_t)b.lcid; });

  out.sp_cell_cfg_present                 = true;
  out.sp_cell_cfg.serv_cell_idx_present   = true;
  out.sp_cell_cfg.serv_cell_idx           = dest.spcell_cfg.serv_cell_idx;
  out.sp_cell_cfg.sp_cell_cfg_ded_present = true;
  calculate_serving_cell_config_diff(
      src.spcell_cfg.spcell_cfg_ded, dest.spcell_cfg.spcell_cfg_ded, out.sp_cell_cfg.sp_cell_cfg_ded);
}
