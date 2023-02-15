/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_configuration_helpers.h"
#include "asn1_cell_group_config_helpers.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/ran/bcd_helpers.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

byte_buffer srsgnb::srs_du::make_asn1_rrc_cell_mib_buffer(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;

  mib_s mib;
  switch (du_cfg.scs_common) {
    case srsgnb::subcarrier_spacing::kHz15:
    case srsgnb::subcarrier_spacing::kHz60:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs15or60;
      break;
    case srsgnb::subcarrier_spacing::kHz30:
    case srsgnb::subcarrier_spacing::kHz120:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs30or120;
      break;
    default:
      srsgnb_assertion_failure("Invalid SCS common");
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
  srsgnb_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack MIB");
  return buf;
}

static asn1::rrc_nr::subcarrier_spacing_e get_asn1_scs(subcarrier_spacing scs)
{
  return asn1::rrc_nr::subcarrier_spacing_e{static_cast<asn1::rrc_nr::subcarrier_spacing_opts::options>(scs)};
}

static asn1::rrc_nr::dl_cfg_common_sib_s make_asn1_rrc_dl_config_common(const dl_config_common& cfg)
{
  using namespace asn1::rrc_nr;
  dl_cfg_common_sib_s out;
  // freq info DL
  for (const auto& dl_band : cfg.freq_info_dl.freq_band_list) {
    nr_multi_band_info_s asn1_band;
    asn1_band.freq_band_ind_nr_present = true;
    asn1_band.freq_band_ind_nr         = nr_band_to_uint(dl_band.band);
    out.freq_info_dl.freq_band_list.push_back(asn1_band);
  }
  out.freq_info_dl.offset_to_point_a = cfg.freq_info_dl.offset_to_point_a;
  out.freq_info_dl.scs_specific_carrier_list.resize(cfg.freq_info_dl.scs_carrier_list.size());
  for (unsigned i = 0; i < cfg.freq_info_dl.scs_carrier_list.size(); ++i) {
    out.freq_info_dl.scs_specific_carrier_list[i].offset_to_carrier =
        cfg.freq_info_dl.scs_carrier_list[i].offset_to_carrier;
    out.freq_info_dl.scs_specific_carrier_list[i].subcarrier_spacing.value =
        get_asn1_scs(cfg.freq_info_dl.scs_carrier_list[i].scs);
    out.freq_info_dl.scs_specific_carrier_list[i].carrier_bw = cfg.freq_info_dl.scs_carrier_list[i].carrier_bandwidth;
  }
  // generic params
  out.init_dl_bwp.generic_params.cp_present               = cfg.init_dl_bwp.generic_params.cp_extended;
  out.init_dl_bwp.generic_params.subcarrier_spacing.value = get_asn1_scs(cfg.init_dl_bwp.generic_params.scs);
  // See TS 38.331, BWP.locationAndBandwidth and TS 38.213 clause 12.
  out.init_dl_bwp.generic_params.location_and_bw =
      sliv_from_s_and_l(275, cfg.init_dl_bwp.generic_params.crbs.start(), cfg.init_dl_bwp.generic_params.crbs.length());
  // PDCCH-ConfigCommon.
  out.init_dl_bwp.pdcch_cfg_common_present = true;
  pdcch_cfg_common_s& pdcch                = out.init_dl_bwp.pdcch_cfg_common.set_setup();
  pdcch.coreset_zero_present               = false; // Sent by MIB.
  pdcch.common_coreset_present             = cfg.init_dl_bwp.pdcch_common.common_coreset.has_value();
  if (pdcch.common_coreset_present) {
    pdcch.common_coreset = srsgnb::srs_du::make_asn1_rrc_coreset(cfg.init_dl_bwp.pdcch_common.common_coreset.value());
  }
  pdcch.search_space_zero_present = false; // Sent by MIB.
  for (size_t ss_idx = 1; ss_idx < cfg.init_dl_bwp.pdcch_common.search_spaces.size(); ++ss_idx) {
    const search_space_configuration& ss = cfg.init_dl_bwp.pdcch_common.search_spaces[ss_idx];
    pdcch.common_search_space_list.push_back(srsgnb::srs_du::make_asn1_rrc_search_space(ss));
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
  // PDSCH-ConfigCommon.
  out.init_dl_bwp.pdsch_cfg_common_present = true;
  pdsch_cfg_common_s& pdsch                = out.init_dl_bwp.pdsch_cfg_common.set_setup();
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
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.half_t();
      nof_pf       = cfg.pcch_cfg.paging_frame_offset;
    } break;
    case pcch_config::nof_pf_per_drx_cycle::quarterT: {
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.quarter_t();
      nof_pf       = cfg.pcch_cfg.paging_frame_offset;
    } break;
    case pcch_config::nof_pf_per_drx_cycle::oneEighthT: {
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.one_eighth_t();
      nof_pf       = cfg.pcch_cfg.paging_frame_offset;
    } break;
    case pcch_config::nof_pf_per_drx_cycle::oneSixteethT: {
      auto& nof_pf = out.pcch_cfg.nand_paging_frame_offset.one_sixteenth_t();
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
          scs120khzQuarterT_scs60khzOneEighthT_scs30khzOneSixteenthT: {
        auto& first_pmo_of_po = out.pcch_cfg.first_pdcch_monitoring_occasion_of_po
                                    .scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::scs120khzOneEighthT_scs60khzOneSixteenthT: {
        auto& first_pmo_of_po =
            out.pcch_cfg.first_pdcch_monitoring_occasion_of_po.scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
        for (const auto& v : cfg.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value) {
          first_pmo_of_po.push_back(v);
        }
      } break;
      case pcch_config::first_pdcch_monitoring_occasion_of_po_type::scs120khzOneSixteenthT: {
        auto& first_pmo_of_po = out.pcch_cfg.first_pdcch_monitoring_occasion_of_po.scs120_kh_zone_sixteenth_t();
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

static asn1::rrc_nr::ul_cfg_common_sib_s make_asn1_rrc_ul_config_common(const ul_config_common& cfg)
{
  using namespace asn1::rrc_nr;

  ul_cfg_common_sib_s out;
  for (const auto& ul_band : cfg.freq_info_ul.freq_band_list) {
    nr_multi_band_info_s asn1_band;
    asn1_band.freq_band_ind_nr_present = true;
    asn1_band.freq_band_ind_nr         = nr_band_to_uint(ul_band.band);
    out.freq_info_ul.freq_band_list.push_back(asn1_band);
  }
  out.freq_info_ul.absolute_freq_point_a_present = true;
  out.freq_info_ul.absolute_freq_point_a         = cfg.freq_info_ul.absolute_freq_point_a;
  out.freq_info_ul.scs_specific_carrier_list.resize(cfg.freq_info_ul.scs_carrier_list.size());
  for (unsigned i = 0; i < cfg.freq_info_ul.scs_carrier_list.size(); ++i) {
    out.freq_info_ul.scs_specific_carrier_list[i].offset_to_carrier =
        cfg.freq_info_ul.scs_carrier_list[i].offset_to_carrier;
    out.freq_info_ul.scs_specific_carrier_list[i].subcarrier_spacing.value =
        get_asn1_scs(cfg.freq_info_ul.scs_carrier_list[i].scs);
    out.freq_info_ul.scs_specific_carrier_list[i].carrier_bw = cfg.freq_info_ul.scs_carrier_list[i].carrier_bandwidth;
  }
  out.init_ul_bwp.generic_params.subcarrier_spacing.value = get_asn1_scs(cfg.init_ul_bwp.generic_params.scs);
  out.init_ul_bwp.generic_params.location_and_bw =
      sliv_from_s_and_l(275, cfg.init_ul_bwp.generic_params.crbs.start(), cfg.init_ul_bwp.generic_params.crbs.length());

  // RACH-ConfigCommon.
  const rach_config_common& rach_cfg      = *cfg.init_ul_bwp.rach_cfg_common;
  out.init_ul_bwp.rach_cfg_common_present = true;
  rach_cfg_common_s& rach                 = out.init_ul_bwp.rach_cfg_common.set_setup();
  rach.rach_cfg_generic.prach_cfg_idx     = rach_cfg.rach_cfg_generic.prach_config_index;
  rach.rach_cfg_generic.msg1_fdm.value    = rach_msg1_fdm_convert_to_asn1(rach_cfg.rach_cfg_generic.msg1_fdm);
  rach.rach_cfg_generic.msg1_freq_start   = static_cast<uint16_t>(rach_cfg.rach_cfg_generic.msg1_frequency_start);
  rach.rach_cfg_generic.zero_correlation_zone_cfg =
      static_cast<uint8_t>(rach_cfg.rach_cfg_generic.zero_correlation_zone_config);
  rach.rach_cfg_generic.preamb_rx_target_pwr   = -110;
  rach.rach_cfg_generic.preamb_trans_max.value = asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7;
  rach.rach_cfg_generic.pwr_ramp_step.value    = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  bool success = asn1::number_to_enum(rach.rach_cfg_generic.ra_resp_win, rach_cfg.rach_cfg_generic.ra_resp_window);
  srsgnb_assert(success, "Invalid ra-WindowSize");
  rach.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  rach.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one().value =
      asn1::rrc_nr::rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_opts::n64;
  rach.ra_contention_resolution_timer.value =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  if (rach_cfg.prach_root_seq_index_l839_present) {
    rach.prach_root_seq_idx.set_l839() = rach_cfg.prach_root_seq_index;
  } else {
    rach.prach_root_seq_idx.set_l139() = rach_cfg.prach_root_seq_index;
  }
  rach.msg1_subcarrier_spacing = get_asn1_scs(rach_cfg.msg1_scs);
  switch (rach_cfg.restricted_set) {
    case srsgnb::restricted_set_config::UNRESTRICTED:
      rach.restricted_set_cfg.value = rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;
      break;
    case srsgnb::restricted_set_config::TYPE_A:
      rach.restricted_set_cfg.value = rach_cfg_common_s::restricted_set_cfg_opts::restricted_set_type_a;
      break;
    case srsgnb::restricted_set_config::TYPE_B:
      rach.restricted_set_cfg.value = rach_cfg_common_s::restricted_set_cfg_opts::restricted_set_type_b;
      break;
    default:
      report_fatal_error("Invalid restricted set");
  }

  // PUSCH-ConfigCommon.
  const pusch_config_common& pusch_cfg     = cfg.init_ul_bwp.pusch_cfg_common.value();
  out.init_ul_bwp.pusch_cfg_common_present = true;
  pusch_cfg_common_s& pusch                = out.init_ul_bwp.pusch_cfg_common.set_setup();
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
  pusch.p0_nominal_with_grant_present = true;
  pusch.p0_nominal_with_grant         = -76;

  // PUCCH-ConfigCommon.
  const pucch_config_common& pucch_cfg     = cfg.init_ul_bwp.pucch_cfg_common.value();
  out.init_ul_bwp.pucch_cfg_common_present = true;
  pucch_cfg_common_s& pucch                = out.init_ul_bwp.pucch_cfg_common.set_setup();
  pucch.pucch_res_common_present           = true;
  pucch.pucch_res_common                   = pucch_cfg.pucch_resource_common;
  pucch.pucch_group_hop.value              = pucch_group_hop_convert_to_asn1(pucch_cfg.group_hopping);
  pucch.p0_nominal_present                 = true;
  pucch.p0_nominal                         = -90;
  if (pucch_cfg.hopping_id.has_value()) {
    pucch.hop_id_present = true;
    pucch.hop_id         = static_cast<uint16_t>(pucch_cfg.hopping_id.value());
  } else {
    pucch.hop_id_present = false;
  }

  out.time_align_timer_common.value = asn1::rrc_nr::time_align_timer_opts::infinity;
  return out;
}

static asn1::rrc_nr::serving_cell_cfg_common_sib_s make_asn1_rrc_cell_serving_cell_common(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;
  serving_cell_cfg_common_sib_s cell;
  cell.dl_cfg_common         = make_asn1_rrc_dl_config_common(du_cfg.dl_cfg_common);
  cell.ul_cfg_common_present = true;
  cell.ul_cfg_common         = make_asn1_rrc_ul_config_common(du_cfg.ul_cfg_common);
  // SSB params.
  cell.ssb_positions_in_burst.in_one_group.from_number(static_cast<uint64_t>(du_cfg.ssb_cfg.ssb_bitmap) >>
                                                       static_cast<uint64_t>(56U));
  asn1::number_to_enum(cell.ssb_periodicity_serving_cell, ssb_periodicity_to_value(du_cfg.ssb_cfg.ssb_period));
  cell.ss_pbch_block_pwr               = du_cfg.ssb_cfg.ssb_block_power;
  cell.n_timing_advance_offset_present = true;
  cell.n_timing_advance_offset.value   = asn1::rrc_nr::serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n0;

  // TDD config
  if (du_cfg.tdd_ul_dl_cfg_common.has_value()) {
    cell.tdd_ul_dl_cfg_common_present                = true;
    cell.tdd_ul_dl_cfg_common.ref_subcarrier_spacing = get_asn1_scs(du_cfg.tdd_ul_dl_cfg_common.value().ref_scs);
    float periodicity_ms = du_cfg.tdd_ul_dl_cfg_common.value().pattern1.dl_ul_tx_period_nof_slots /
                           get_nof_slots_per_subframe(du_cfg.tdd_ul_dl_cfg_common.value().ref_scs);
    if (periodicity_ms == 10.0) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
    } else if (periodicity_ms == 5.0) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms5;
    } else if (periodicity_ms == 2.5) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2p5;
    } else if (periodicity_ms == 2.0) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2;
    } else if (periodicity_ms == 1.25) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1p25;
    } else if (periodicity_ms == 2) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1;
    } else if (periodicity_ms == 0.625) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms0p625;
    } else if (periodicity_ms == 0.5) {
      cell.tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity =
          asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms0p5;
    } else {
      report_fatal_error("Unsupported TDD UL/DL periodicity {}ms", periodicity_ms);
    }
    cell.tdd_ul_dl_cfg_common.pattern1.nrof_dl_slots   = du_cfg.tdd_ul_dl_cfg_common.value().pattern1.nof_dl_slots;
    cell.tdd_ul_dl_cfg_common.pattern1.nrof_dl_symbols = du_cfg.tdd_ul_dl_cfg_common.value().pattern1.nof_dl_symbols;
    cell.tdd_ul_dl_cfg_common.pattern1.nrof_ul_slots   = du_cfg.tdd_ul_dl_cfg_common.value().pattern1.nof_ul_slots;
    cell.tdd_ul_dl_cfg_common.pattern1.nrof_ul_symbols = du_cfg.tdd_ul_dl_cfg_common.value().pattern1.nof_ul_symbols;
  }

  // TODO: Fill remaining fields.

  return cell;
}

asn1::rrc_nr::sib1_s make_asn1_rrc_cell_sib1(const du_cell_config& du_cfg)
{
  using namespace asn1::rrc_nr;

  sib1_s sib1;

  sib1.cell_sel_info_present            = true;
  sib1.cell_sel_info.q_rx_lev_min       = -70;
  sib1.cell_sel_info.q_qual_min_present = true;
  sib1.cell_sel_info.q_qual_min         = -20;

  sib1.cell_access_related_info.plmn_id_list.resize(1);
  sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list.resize(1);
  plmn_id_s& plmn  = sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list[0];
  plmn.mcc_present = true;
  plmn.mcc[0]      = du_cfg.plmn[0] - '0';
  plmn.mcc[1]      = du_cfg.plmn[1] - '0';
  plmn.mcc[2]      = du_cfg.plmn[2] - '0';
  plmn.mnc.resize(du_cfg.plmn.size() == 5 ? 2 : 3);
  for (unsigned i = 3; i < du_cfg.plmn.size(); ++i) {
    plmn.mnc[i - 3] = du_cfg.plmn[i] - '0';
  }
  sib1.cell_access_related_info.plmn_id_list[0].tac_present = true;
  sib1.cell_access_related_info.plmn_id_list[0].tac.from_number(du_cfg.tac);
  sib1.cell_access_related_info.plmn_id_list[0].cell_id.from_number(du_cfg.cell_id);
  sib1.cell_access_related_info.plmn_id_list[0].cell_reserved_for_oper.value =
      plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;

  sib1.conn_est_fail_ctrl_present                   = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_count.value = asn1::rrc_nr::conn_est_fail_ctrl_s::conn_est_fail_count_opts::n1;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_validity.value =
      conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::s30;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_present = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset         = 1;

  sib1.serving_cell_cfg_common_present = true;
  sib1.serving_cell_cfg_common         = make_asn1_rrc_cell_serving_cell_common(du_cfg);

  sib1.ue_timers_and_consts_present    = true;
  sib1.ue_timers_and_consts.t300.value = ue_timers_and_consts_s::t300_opts::ms1000;
  sib1.ue_timers_and_consts.t301.value = ue_timers_and_consts_s::t301_opts::ms1000;
  sib1.ue_timers_and_consts.t310.value = ue_timers_and_consts_s::t310_opts::ms1000;
  sib1.ue_timers_and_consts.n310.value = ue_timers_and_consts_s::n310_opts::n1;
  sib1.ue_timers_and_consts.t311.value = ue_timers_and_consts_s::t311_opts::ms30000;
  sib1.ue_timers_and_consts.n311.value = ue_timers_and_consts_s::n311_opts::n1;
  sib1.ue_timers_and_consts.t319.value = ue_timers_and_consts_s::t319_opts::ms1000;

  return sib1;
}

byte_buffer srsgnb::srs_du::make_asn1_rrc_cell_sib1_buffer(const du_cell_config& du_cfg, std::string* js_str)
{
  byte_buffer          buf;
  asn1::bit_ref        bref{buf};
  asn1::rrc_nr::sib1_s sib1 = make_asn1_rrc_cell_sib1(du_cfg);
  asn1::SRSASN_CODE    ret  = sib1.pack(bref);
  srsgnb_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack SIB1");

  if (js_str != nullptr) {
    asn1::json_writer js;
    sib1.to_json(js);
    *js_str = js.to_string();
  }
  return buf;
}

byte_buffer srsgnb::srs_du::make_asn1_rrc_cell_bcch_dl_sch_msg(const du_cell_config& du_cfg)
{
  byte_buffer                     buf;
  asn1::bit_ref                   bref{buf};
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  msg.msg.set_c1().set_sib_type1() = make_asn1_rrc_cell_sib1(du_cfg);
  asn1::SRSASN_CODE ret            = msg.pack(bref);
  srsgnb_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack SIB1");
  return buf;
}

void srsgnb::srs_du::fill_asn1_f1_setup_request(asn1::f1ap::f1_setup_request_s& request,
                                                const du_setup_params&          setup_params,
                                                span<const du_cell_config*>     cells_to_add,
                                                std::vector<std::string>*       cell_json_strs)
{
  byte_buffer buf;
  // TODO: Add other inputs and set values accordingly

  request->gnb_du_id.value = setup_params.gnb_du_id;
  request->gnb_du_name.value.from_string(setup_params.gnb_du_name);
  request->gnb_du_rrc_version.value.latest_rrc_version.from_number(setup_params.rrc_version);

  request->gnb_du_served_cells_list_present = true;
  for (const du_cell_config* cell_cfg : cells_to_add) {
    // Add Cell in list of served cells.
    request->gnb_du_served_cells_list->push_back({});
    request->gnb_du_served_cells_list->back().load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_LIST);
    asn1::f1ap::gnb_du_served_cells_item_s& f1ap_cell =
        request->gnb_du_served_cells_list->back()->gnb_du_served_cells_item();

    // Fill Served Cell Information.
    f1ap_cell.served_cell_info.nr_pci = cell_cfg->pci;
    f1ap_cell.served_cell_info.nr_cgi.plmn_id.from_number(plmn_string_to_bcd(cell_cfg->plmn));
    f1ap_cell.served_cell_info.nr_cgi.nr_cell_id.from_number(cell_cfg->cell_id); // TODO: add gnbID
    f1ap_cell.served_cell_info.five_gs_tac_present = true;
    f1ap_cell.served_cell_info.five_gs_tac.from_number(cell_cfg->tac);

    // Add System Information related to the cell.
    f1ap_cell.gnb_du_sys_info_present = true;
    buf                               = make_asn1_rrc_cell_mib_buffer(*cell_cfg);
    f1ap_cell.gnb_du_sys_info.mib_msg = std::move(buf);

    // Enable json conversion if argument is present.
    std::string* js_str = nullptr;
    if (cell_json_strs != nullptr) {
      cell_json_strs->emplace_back();
      js_str = &cell_json_strs->back();
    }
    buf                                = make_asn1_rrc_cell_sib1_buffer(*cell_cfg, js_str);
    f1ap_cell.gnb_du_sys_info.sib1_msg = std::move(buf);
  }
}

asn1::rrc_nr::paging_s make_asn1_rrc_cell_cn_paging_msg(const uint64_t five_g_s_tmsi)
{
  asn1::rrc_nr::paging_record_s pg_rec{};
  auto&                         rrc_pg_id = pg_rec.ue_id.set_ng_5_g_s_tmsi();
  rrc_pg_id                               = rrc_pg_id.from_number(five_g_s_tmsi);

  asn1::rrc_nr::paging_s rrc_pg{};
  rrc_pg.paging_record_list.push_back(pg_rec);

  return rrc_pg;
}

byte_buffer srsgnb::srs_du::make_asn1_rrc_cell_pcch_pch_msg(uint64_t five_g_s_tmsi)
{
  byte_buffer   buf;
  asn1::bit_ref bref{buf};

  asn1::rrc_nr::pcch_msg_s pcch_msg{};
  auto&                    pcch_c1 = pcch_msg.msg.set_c1();
  // TODO: Support RAN Paging.
  pcch_c1.set_paging() = make_asn1_rrc_cell_cn_paging_msg(five_g_s_tmsi);

  const asn1::SRSASN_CODE ret = pcch_msg.pack(bref);
  srsgnb_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack PCCH-PCH Paging message");
  return buf;
}
