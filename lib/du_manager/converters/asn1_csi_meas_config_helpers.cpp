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

#include "asn1_csi_meas_config_helpers.h"
#include "../../asn1/asn1_diff_utils.h"
#include "srsran/asn1/rrc_nr/serving_cell.h"

using namespace srsran;
using namespace srsran::srs_du;
using namespace asn1::rrc_nr;

static asn1::rrc_nr::csi_rs_res_map_s make_asn1_nzp_csi_rs_resource_mapping(const csi_rs_resource_mapping& cfg)
{
  csi_rs_res_map_s out{};
  if (cfg.fd_alloc.size() == 4) {
    asn1::fixed_bitstring<4>& row = out.freq_domain_alloc.set_row1();
    row.from_number(cfg.fd_alloc.to_uint64());
  } else if (cfg.fd_alloc.size() == 12) {
    asn1::fixed_bitstring<12>& row = out.freq_domain_alloc.set_row2();
    row.from_number(cfg.fd_alloc.to_uint64());
  } else if (cfg.fd_alloc.size() == 3) {
    asn1::fixed_bitstring<3>& row = out.freq_domain_alloc.set_row4();
    row.from_number(cfg.fd_alloc.to_uint64());
  } else if (cfg.fd_alloc.size() == 6) {
    asn1::fixed_bitstring<6>& row = out.freq_domain_alloc.set_other();
    row.from_number(cfg.fd_alloc.to_uint64());
  } else {
    report_fatal_error("Invalid freq domain row bitmap size={}", cfg.fd_alloc.size());
  }

  switch (cfg.nof_ports) {
    case 1:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p1;
      break;
    case 2:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p2;
      break;
    case 4:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p4;
      break;
    case 8:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p8;
      break;
    case 12:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p12;
      break;
    case 16:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p16;
      break;
    case 24:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p24;
      break;
    case 32:
      out.nrof_ports = csi_rs_res_map_s::nrof_ports_opts::p32;
      break;
    default:
      srsran_assertion_failure("Invalid nof. NZP CSI-RS ports={}", cfg.nof_ports);
  }

  out.first_ofdm_symbol_in_time_domain = cfg.first_ofdm_symbol_in_td;
  if (cfg.first_ofdm_symbol_in_td2.has_value()) {
    out.first_ofdm_symbol_in_time_domain2_present = true;
    out.first_ofdm_symbol_in_time_domain2         = cfg.first_ofdm_symbol_in_td2.value();
  }

  switch (cfg.cdm) {
    case csi_rs_cdm_type::no_CDM:
      out.cdm_type = csi_rs_res_map_s::cdm_type_opts::no_cdm;
      break;
    case csi_rs_cdm_type::fd_CDM2:
      out.cdm_type = csi_rs_res_map_s::cdm_type_opts::fd_cdm2;
      break;
    case csi_rs_cdm_type::cdm4_FD2_TD2:
      out.cdm_type = csi_rs_res_map_s::cdm_type_opts::cdm4_fd2_td2;
      break;
    case csi_rs_cdm_type::cdm8_FD2_TD4:
      out.cdm_type = csi_rs_res_map_s::cdm_type_opts::cdm8_fd2_td4;
      break;
    default:
      srsran_assertion_failure("Invalid CDM={}", cfg.cdm);
  }

  switch (cfg.freq_density) {
    case csi_rs_freq_density_type::dot5_even_RB: {
      auto& density = out.density.set_dot5();
      density       = csi_rs_res_map_s::density_c_::dot5_opts::even_prbs;
      break;
    }
    case csi_rs_freq_density_type::dot5_odd_RB: {
      auto& density = out.density.set_dot5();
      density       = csi_rs_res_map_s::density_c_::dot5_opts::odd_prbs;
      break;
    }
    case csi_rs_freq_density_type::one:
      out.density.set_one();
      break;
    case csi_rs_freq_density_type::three:
      out.density.set_three();
      break;
    default:
      srsran_assertion_failure("Invalid freq. density={}", cfg.freq_density);
  }

  out.freq_band.start_rb = cfg.freq_band_rbs.start();
  out.freq_band.nrof_rbs = cfg.freq_band_rbs.length();

  return out;
}

static void make_asn1_csi_resource_periodicity_and_offset(csi_res_periodicity_and_offset_c& out,
                                                          const csi_resource_periodicity&   periodicity,
                                                          const unsigned                    offset)
{
  switch (periodicity) {
    case csi_resource_periodicity::slots4: {
      auto& p_and_o = out.set_slots4();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots5: {
      auto& p_and_o = out.set_slots5();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots8: {
      auto& p_and_o = out.set_slots8();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots10: {
      auto& p_and_o = out.set_slots10();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots16: {
      auto& p_and_o = out.set_slots16();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots20: {
      auto& p_and_o = out.set_slots20();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots32: {
      auto& p_and_o = out.set_slots32();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots40: {
      auto& p_and_o = out.set_slots40();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots64: {
      auto& p_and_o = out.set_slots64();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots80: {
      auto& p_and_o = out.set_slots80();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots160: {
      auto& p_and_o = out.set_slots160();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots320: {
      auto& p_and_o = out.set_slots320();
      p_and_o       = offset;
      break;
    }
    case csi_resource_periodicity::slots640: {
      auto& p_and_o = out.set_slots640();
      p_and_o       = offset;
      break;
    }
    default:
      srsran_assertion_failure("Invalid CSI-RS periodicity={}", periodicity);
  }
}

asn1::rrc_nr::zp_csi_rs_res_s srsran::srs_du::make_asn1_zp_csi_rs_resource(const zp_csi_rs_resource& cfg)
{
  zp_csi_rs_res_s out{};
  out.zp_csi_rs_res_id = static_cast<uint8_t>(cfg.id);
  out.res_map          = make_asn1_nzp_csi_rs_resource_mapping(cfg.res_mapping);
  if (cfg.period.has_value() and cfg.offset.has_value()) {
    out.periodicity_and_offset_present = true;
    make_asn1_csi_resource_periodicity_and_offset(out.periodicity_and_offset, cfg.period.value(), cfg.offset.value());
  }
  return out;
}

asn1::rrc_nr::zp_csi_rs_res_set_s srsran::srs_du::make_asn1_zp_csi_rs_resource_set(const zp_csi_rs_resource_set& cfg)
{
  zp_csi_rs_res_set_s out{};
  out.zp_csi_rs_res_set_id = cfg.id;
  out.zp_csi_rs_res_id_list.resize(cfg.zp_csi_rs_res_list.size());
  for (unsigned i = 0; i != cfg.zp_csi_rs_res_list.size(); ++i) {
    out.zp_csi_rs_res_id_list[i] = static_cast<uint8_t>(cfg.zp_csi_rs_res_list[i]);
  }
  return out;
}

asn1::rrc_nr::nzp_csi_rs_res_s make_asn1_nzp_csi_rs_resource(const nzp_csi_rs_resource& cfg)
{
  nzp_csi_rs_res_s out{};
  out.nzp_csi_rs_res_id = cfg.res_id;
  out.res_map           = make_asn1_nzp_csi_rs_resource_mapping(cfg.res_mapping);
  out.pwr_ctrl_offset   = cfg.pwr_ctrl_offset;
  if (cfg.pwr_ctrl_offset_ss_db.has_value()) {
    out.pwr_ctrl_offset_ss_present = true;
    switch (cfg.pwr_ctrl_offset_ss_db.value()) {
      case -3:
        out.pwr_ctrl_offset_ss = nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::db_neg3;
        break;
      case 0:
        out.pwr_ctrl_offset_ss = nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::db0;
        break;
      case 3:
        out.pwr_ctrl_offset_ss = nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::db3;
        break;
      case 6:
        out.pwr_ctrl_offset_ss = nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::db6;
        break;
      default:
        srsran_assertion_failure("Invalid power control offset SS={}", cfg.pwr_ctrl_offset_ss_db.value());
    }
  }
  out.scrambling_id = cfg.scrambling_id;

  if (cfg.csi_res_period.has_value() and cfg.csi_res_offset.has_value()) {
    out.periodicity_and_offset_present = true;
    make_asn1_csi_resource_periodicity_and_offset(
        out.periodicity_and_offset, cfg.csi_res_period.value(), cfg.csi_res_offset.value());
  }

  if (cfg.qcl_info_periodic_csi_rs.has_value()) {
    out.qcl_info_periodic_csi_rs_present = true;
    out.qcl_info_periodic_csi_rs         = cfg.qcl_info_periodic_csi_rs.value();
  }

  return out;
}

asn1::rrc_nr::nzp_csi_rs_res_set_s make_asn1_nzp_csi_rs_resource_set(const nzp_csi_rs_resource_set& cfg)
{
  nzp_csi_rs_res_set_s out{};
  out.nzp_csi_res_set_id = cfg.res_set_id;

  for (const auto& res_id : cfg.nzp_csi_rs_res) {
    out.nzp_csi_rs_res.push_back(res_id);
  }

  if (cfg.is_repetition_on.has_value()) {
    out.repeat_present = true;
    out.repeat = cfg.is_repetition_on ? nzp_csi_rs_res_set_s::repeat_opts::on : nzp_csi_rs_res_set_s::repeat_opts::off;
  }

  if (cfg.aperiodic_trigger_offset.has_value()) {
    out.aperiodic_trigger_offset_present = true;
    out.aperiodic_trigger_offset         = cfg.aperiodic_trigger_offset.value();
  }

  out.trs_info_present = cfg.is_trs_info_present;

  return out;
}

asn1::rrc_nr::csi_im_res_s make_asn1_csi_im_resource(const csi_im_resource& cfg)
{
  csi_im_res_s out{};
  out.csi_im_res_id = cfg.res_id;

  if (cfg.csi_im_res_element_pattern.has_value()) {
    out.csi_im_res_elem_pattern_present = true;
    switch (cfg.csi_im_res_element_pattern.value().pattern_type) {
      case csi_im_resource::csi_im_resource_element_pattern_type::pattern0: {
        auto& pattern = out.csi_im_res_elem_pattern.set_pattern0();
        switch (cfg.csi_im_res_element_pattern.value().subcarrier_location) {
          case 0:
            pattern.subcarrier_location_p0 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::s0;
            break;
          case 2:
            pattern.subcarrier_location_p0 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::s2;
            break;
          case 4:
            pattern.subcarrier_location_p0 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::s4;
            break;
          case 6:
            pattern.subcarrier_location_p0 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::s6;
            break;
          case 8:
            pattern.subcarrier_location_p0 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::s8;
            break;
          case 10:
            pattern.subcarrier_location_p0 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::s10;
            break;
          default:
            srsran_assertion_failure("Invalid CSI IM resource subcarrier location={}",
                                     cfg.csi_im_res_element_pattern.value().subcarrier_location);
        }
        pattern.symbol_location_p0 = cfg.csi_im_res_element_pattern.value().symbol_location;
        break;
      }
      case csi_im_resource::csi_im_resource_element_pattern_type::pattern1: {
        auto& pattern = out.csi_im_res_elem_pattern.set_pattern1();
        switch (cfg.csi_im_res_element_pattern.value().subcarrier_location) {
          case 0:
            pattern.subcarrier_location_p1 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::s0;
            break;
          case 4:
            pattern.subcarrier_location_p1 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::s4;
            break;
          case 8:
            pattern.subcarrier_location_p1 =
                csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::s8;
            break;
          default:
            srsran_assertion_failure("Invalid CSI IM resource subcarrier location={}",
                                     cfg.csi_im_res_element_pattern.value().subcarrier_location);
        }
        pattern.symbol_location_p1 = cfg.csi_im_res_element_pattern.value().symbol_location;
        break;
      }
      default:
        srsran_assertion_failure("Invalid CSI IM resource element pattern={}",
                                 cfg.csi_im_res_element_pattern.value().pattern_type);
    }
  }

  if (not cfg.freq_band_rbs.empty()) {
    out.freq_band_present  = true;
    out.freq_band.start_rb = cfg.freq_band_rbs.start();
    out.freq_band.nrof_rbs = cfg.freq_band_rbs.length();
  }

  if (cfg.csi_res_period.has_value() and cfg.csi_res_offset.has_value()) {
    out.periodicity_and_offset_present = true;
    make_asn1_csi_resource_periodicity_and_offset(
        out.periodicity_and_offset, cfg.csi_res_period.value(), cfg.csi_res_offset.value());
  }

  return out;
}

static asn1::rrc_nr::csi_im_res_set_s make_asn1_csi_im_resource_set(const csi_im_resource_set& cfg)
{
  csi_im_res_set_s out{};
  out.csi_im_res_set_id = cfg.res_set_id;
  for (const auto& res_id : cfg.csi_ims_resources) {
    out.csi_im_res.push_back(res_id);
  }
  return out;
}

static asn1::rrc_nr::csi_ssb_res_set_s make_asn1_csi_ssb_resource_set(const csi_ssb_resource_set& cfg)
{
  csi_ssb_res_set_s out{};
  out.csi_ssb_res_set_id = cfg.res_set_id;
  for (const auto& res_id : cfg.csi_ssb_res_list) {
    out.csi_ssb_res_list.push_back(res_id);
  }
  return out;
}

static asn1::rrc_nr::csi_res_cfg_s make_asn1_csi_resource_config(const csi_resource_config& cfg)
{
  csi_res_cfg_s out{};
  out.csi_res_cfg_id = cfg.res_cfg_id;
  if (variant_holds_alternative<csi_resource_config::nzp_csi_rs_ssb>(cfg.csi_rs_res_set_list)) {
    auto&       res_set     = out.csi_rs_res_set_list.set_nzp_csi_rs_ssb();
    const auto& res_set_val = variant_get<csi_resource_config::nzp_csi_rs_ssb>(cfg.csi_rs_res_set_list);
    for (const auto& res_set_id : res_set_val.nzp_csi_rs_res_set_list) {
      res_set.nzp_csi_rs_res_set_list.push_back(res_set_id);
    }
    res_set.csi_ssb_res_set_list_present = not res_set_val.csi_ssb_res_set_list.empty();
    for (const auto& res_set_id : res_set_val.csi_ssb_res_set_list) {
      res_set.csi_ssb_res_set_list[0] = res_set_id;
    }
  } else if (variant_holds_alternative<csi_resource_config::csi_im_resource_set_list>(cfg.csi_rs_res_set_list)) {
    auto&       res_set     = out.csi_rs_res_set_list.set_csi_im_res_set_list();
    const auto& res_set_val = variant_get<csi_resource_config::csi_im_resource_set_list>(cfg.csi_rs_res_set_list);
    for (const auto& res_set_id : res_set_val) {
      res_set.push_back(res_set_id);
    }
  }

  out.bwp_id = cfg.bwp_id;

  switch (cfg.res_type) {
    case csi_resource_config::resource_type::aperiodic:
      out.res_type = csi_res_cfg_s::res_type_opts::aperiodic;
      break;
    case csi_resource_config::resource_type::semiPersistent:
      out.res_type = csi_res_cfg_s::res_type_opts::semi_persistent;
      break;
    case csi_resource_config::resource_type::periodic:
      out.res_type = csi_res_cfg_s::res_type_opts::periodic;
      break;
    default:
      srsran_assertion_failure("Invalid CSI resource type={}", cfg.res_type);
  }

  return out;
}

static void make_asn1_csi_report_periodicity_and_offset(csi_report_periodicity_and_offset_c& out,
                                                        const csi_report_periodicity&        periodicity,
                                                        const unsigned                       offset)
{
  switch (periodicity) {
    case csi_report_periodicity::slots4: {
      auto& p_and_o = out.set_slots4();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots5: {
      auto& p_and_o = out.set_slots5();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots8: {
      auto& p_and_o = out.set_slots8();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots10: {
      auto& p_and_o = out.set_slots10();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots16: {
      auto& p_and_o = out.set_slots16();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots20: {
      auto& p_and_o = out.set_slots20();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots40: {
      auto& p_and_o = out.set_slots40();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots80: {
      auto& p_and_o = out.set_slots80();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots160: {
      auto& p_and_o = out.set_slots160();
      p_and_o       = offset;
      break;
    }
    case csi_report_periodicity::slots320: {
      auto& p_and_o = out.set_slots320();
      p_and_o       = offset;
      break;
    }
    default:
      srsran_assertion_failure("Invalid CSI report periodicity={}", periodicity);
  }
}

static void make_asn1_codebook_config(codebook_cfg_s& out, const codebook_config& cfg)
{
  if (variant_holds_alternative<codebook_config::type1>(cfg.codebook_type)) {
    const auto& tp1_cfg_val = variant_get<codebook_config::type1>(cfg.codebook_type);
    auto&       out_tp1_cfg = out.codebook_type.set_type1();
    // Single Panel.
    if (variant_holds_alternative<codebook_config::type1::single_panel>(tp1_cfg_val.sub_type)) {
      const auto& sp_cfg_val = variant_get<codebook_config::type1::single_panel>(tp1_cfg_val.sub_type);
      auto&       out_sp_cfg = out_tp1_cfg.sub_type.set_type_i_single_panel();
      // Two antennas.
      if (variant_holds_alternative<
              codebook_config::type1::single_panel::two_antenna_ports_two_tx_codebook_subset_restriction>(
              sp_cfg_val.nof_antenna_ports)) {
        const auto& ant_restriction =
            variant_get<codebook_config::type1::single_panel::two_antenna_ports_two_tx_codebook_subset_restriction>(
                sp_cfg_val.nof_antenna_ports);
        auto& out_ant_restriction = out_sp_cfg.nr_of_ant_ports.set_two();
        out_ant_restriction.two_tx_codebook_subset_restrict.from_number(ant_restriction.to_uint64());
      }
      // More than two antennas.
      else if (variant_holds_alternative<codebook_config::type1::single_panel::more_than_two_antenna_ports>(
                   sp_cfg_val.nof_antenna_ports)) {
        const auto& ant_restriction = variant_get<codebook_config::type1::single_panel::more_than_two_antenna_ports>(
            sp_cfg_val.nof_antenna_ports);
        auto& out_ant_restriction = out_sp_cfg.nr_of_ant_ports.set_more_than_two();
        switch (ant_restriction.n1_n2_restriction_type) {
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::two_one: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_two_one_type_i_single_panel_restrict();
            n1_n2.from_number(ant_restriction.n1_n2_restriction_value.to_uint64());
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::two_two: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_two_two_type_i_single_panel_restrict();
            n1_n2.from_number(ant_restriction.n1_n2_restriction_value.to_uint64());
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::four_one: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_four_one_type_i_single_panel_restrict();
            n1_n2.from_number(ant_restriction.n1_n2_restriction_value.to_uint64());
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::three_two: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_three_two_type_i_single_panel_restrict();
            n1_n2.from_string(fmt::format("{:b}", ant_restriction.n1_n2_restriction_value));
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::six_one: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_six_one_type_i_single_panel_restrict();
            n1_n2.from_number(ant_restriction.n1_n2_restriction_value.to_uint64());
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::four_two: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_four_two_type_i_single_panel_restrict();
            n1_n2.from_string(fmt::format("{:b}", ant_restriction.n1_n2_restriction_value));
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::eight_one: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_eight_one_type_i_single_panel_restrict();
            n1_n2.from_number(ant_restriction.n1_n2_restriction_value.to_uint64());
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::
              four_three: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_four_three_type_i_single_panel_restrict();
            n1_n2.from_string(fmt::format("{:b}", ant_restriction.n1_n2_restriction_value));
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::six_two: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_six_two_type_i_single_panel_restrict();
            n1_n2.from_string(fmt::format("{:b}", ant_restriction.n1_n2_restriction_value));
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::
              twelve_one: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_twelve_one_type_i_single_panel_restrict();
            n1_n2.from_number(ant_restriction.n1_n2_restriction_value.to_uint64());
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::four_four: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_four_four_type_i_single_panel_restrict();
            n1_n2.from_string(fmt::format("{:b}", ant_restriction.n1_n2_restriction_value));
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::eight_two: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_eight_two_type_i_single_panel_restrict();
            n1_n2.from_string(fmt::format("{:b}", ant_restriction.n1_n2_restriction_value));
            break;
          }
          case codebook_config::type1::single_panel::more_than_two_antenna_ports::n1_n2_restriction_type_t::
              sixteen_one: {
            auto& n1_n2 = out_ant_restriction.n1_n2.set_sixteen_one_type_i_single_panel_restrict();
            n1_n2.from_number(ant_restriction.n1_n2_restriction_value.to_uint64());
            break;
          }
          default:
            srsran_assertion_failure("Invalid n1-n2 type={}", ant_restriction.n1_n2_restriction_type);
        }
        if (ant_restriction.typei_single_panel_codebook_subset_restriction_i2.size() > 0) {
          out_ant_restriction.type_i_single_panel_codebook_subset_restrict_i2_present = true;
          out_ant_restriction.type_i_single_panel_codebook_subset_restrict_i2.from_number(
              ant_restriction.typei_single_panel_codebook_subset_restriction_i2.to_uint64());
        }
      }
      out_sp_cfg.type_i_single_panel_ri_restrict.from_number(sp_cfg_val.typei_single_panel_ri_restriction.to_uint64());
    } else if (variant_holds_alternative<codebook_config::type1::multi_panel>(tp1_cfg_val.sub_type)) {
      const auto& mp_cfg_val = variant_get<codebook_config::type1::multi_panel>(tp1_cfg_val.sub_type);
      auto&       out_mp_cfg = out_tp1_cfg.sub_type.set_type_i_multi_panel();
      switch (mp_cfg_val.ng_n1_n2_restriction_type) {
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::two_two_one: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_two_two_one_type_i_multi_panel_restrict();
          ng_n1_n2.from_number(mp_cfg_val.ng_n1_n2_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::two_four_one: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_two_four_one_type_i_multi_panel_restrict();
          ng_n1_n2.from_number(mp_cfg_val.ng_n1_n2_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::four_two_one: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_four_two_one_type_i_multi_panel_restrict();
          ng_n1_n2.from_number(mp_cfg_val.ng_n1_n2_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::two_two_two: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_two_two_two_type_i_multi_panel_restrict();
          ng_n1_n2.from_number(mp_cfg_val.ng_n1_n2_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::two_eight_one: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_two_eight_one_type_i_multi_panel_restrict();
          ng_n1_n2.from_number(mp_cfg_val.ng_n1_n2_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::four_four_one: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_four_four_one_type_i_multi_panel_restrict();
          ng_n1_n2.from_number(mp_cfg_val.ng_n1_n2_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::two_four_two: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_two_four_two_type_i_multi_panel_restrict();
          ng_n1_n2.from_string(fmt::format("{:b}", mp_cfg_val.ng_n1_n2_restriction_value));
          break;
        }
        case codebook_config::type1::multi_panel::ng_n1_n2_restriction_type_t::four_two_two: {
          auto& ng_n1_n2 = out_mp_cfg.ng_n1_n2.set_four_two_two_type_i_multi_panel_restrict();
          ng_n1_n2.from_number(mp_cfg_val.ng_n1_n2_restriction_value.to_uint64());
          break;
        }
        default:
          srsran_assertion_failure("Invalid ng-n1-n2 type={}", mp_cfg_val.ng_n1_n2_restriction_type);
      }
      out_mp_cfg.ri_restrict.from_number(mp_cfg_val.ri_restriction.to_uint64());
    }
    out_tp1_cfg.codebook_mode = tp1_cfg_val.codebook_mode;
  } else if (variant_holds_alternative<codebook_config::type2>(cfg.codebook_type)) {
    const auto& tp2_cfg_val = variant_get<codebook_config::type2>(cfg.codebook_type);
    auto&       out_tp2_cfg = out.codebook_type.set_type2();
    if (variant_holds_alternative<codebook_config::type2::typeii>(tp2_cfg_val.sub_type)) {
      const auto& typeii     = variant_get<codebook_config::type2::typeii>(tp2_cfg_val.sub_type);
      auto&       out_typeii = out_tp2_cfg.sub_type.set_type_ii();
      switch (typeii.n1_n2_codebook_subset_restriction_type) {
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::two_one: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_two_one();
          n1_n2.from_number(typeii.n1_n2_codebook_subset_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::two_two: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_two_two();
          n1_n2.from_number(typeii.n1_n2_codebook_subset_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::four_one: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_four_one();
          n1_n2.from_number(typeii.n1_n2_codebook_subset_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::three_two: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_three_two();
          n1_n2.from_number(typeii.n1_n2_codebook_subset_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::six_one: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_six_one();
          n1_n2.from_number(typeii.n1_n2_codebook_subset_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::four_two: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_four_two();
          n1_n2.from_string(fmt::format("{:b}", typeii.n1_n2_codebook_subset_restriction_value));
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::eight_one: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_eight_one();
          n1_n2.from_number(typeii.n1_n2_codebook_subset_restriction_value.to_uint64());
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::four_three: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_four_three();
          n1_n2.from_string(fmt::format("{:b}", typeii.n1_n2_codebook_subset_restriction_value));
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::six_two: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_six_two();
          n1_n2.from_string(fmt::format("{:b}", typeii.n1_n2_codebook_subset_restriction_value));
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::twelve_one: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_twelve_one();
          n1_n2.from_string(fmt::format("{:b}", typeii.n1_n2_codebook_subset_restriction_value));
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::four_four: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_four_four();
          n1_n2.from_string(fmt::format("{:b}", typeii.n1_n2_codebook_subset_restriction_value));
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::eight_two: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_eight_two();
          n1_n2.from_string(fmt::format("{:b}", typeii.n1_n2_codebook_subset_restriction_value));
          break;
        }
        case codebook_config::type2::typeii::n1_n2_codebook_subset_restriction_type_t::sixteen_one: {
          auto& n1_n2 = out_typeii.n1_n2_codebook_subset_restrict.set_sixteen_one();
          n1_n2.from_string(fmt::format("{:b}", typeii.n1_n2_codebook_subset_restriction_value));
          break;
        }
        default:
          srsran_assertion_failure("Invalid n1-n2 codebook subset restriction type={}",
                                   typeii.n1_n2_codebook_subset_restriction_type);
      }
      out_typeii.type_ii_ri_restrict.from_number(typeii.typeii_ri_restriction.to_uint64());
    } else if (variant_holds_alternative<codebook_config::type2::typeii_port_selection>(tp2_cfg_val.sub_type)) {
      const auto& typeii_ps     = variant_get<codebook_config::type2::typeii_port_selection>(tp2_cfg_val.sub_type);
      auto&       out_typeii_ps = out_tp2_cfg.sub_type.set_type_ii_port_sel();
      if (typeii_ps.port_selection_sampling_size.has_value()) {
        out_typeii_ps.port_sel_sampling_size_present = true;
        switch (typeii_ps.port_selection_sampling_size.value()) {
          case 1:
            out_typeii_ps.port_sel_sampling_size = codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::
                type_ii_port_sel_s_::port_sel_sampling_size_opts::n1;
            break;
          case 2:
            out_typeii_ps.port_sel_sampling_size = codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::
                type_ii_port_sel_s_::port_sel_sampling_size_opts::n2;
            break;
          case 3:
            out_typeii_ps.port_sel_sampling_size = codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::
                type_ii_port_sel_s_::port_sel_sampling_size_opts::n3;
            break;
          case 4:
            out_typeii_ps.port_sel_sampling_size = codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::
                type_ii_port_sel_s_::port_sel_sampling_size_opts::n4;
            break;
          default:
            srsran_assertion_failure("Invalid port selection sampling size={}",
                                     typeii_ps.port_selection_sampling_size.value());
        }
      }
      out_typeii_ps.type_ii_port_sel_ri_restrict.from_number(
          typeii_ps.typeii_port_selection_ri_restriction.to_uint64());
    }

    out_tp2_cfg.subband_amplitude = tp2_cfg_val.subband_amplitude;
    switch (tp2_cfg_val.phase_alphabet_size) {
      case 4:
        out_tp2_cfg.phase_alphabet_size = codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_opts::n4;
        break;
      case 8:
        out_tp2_cfg.phase_alphabet_size = codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_opts::n8;
        break;
      default:
        srsran_assertion_failure("Invalid phase alphabet size={}", tp2_cfg_val.phase_alphabet_size);
    }

    switch (tp2_cfg_val.nof_beams) {
      case 2:
        out_tp2_cfg.nof_beams = codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::two;
        break;
      case 3:
        out_tp2_cfg.nof_beams = codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::three;
        break;
      case 4:
        out_tp2_cfg.nof_beams = codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::four;
        break;
      default:
        srsran_assertion_failure("Invalid nof. beams={}", tp2_cfg_val.nof_beams);
    }
  }
}

static void make_asn1_port_index_each_rank(span<uint8_t> out, span<const unsigned> cfg)
{
  for (unsigned idx = 0; idx < cfg.size(); idx++) {
    out[idx] = cfg[idx];
  }
}

static asn1::rrc_nr::port_idx_for8_ranks_c
make_asn1_port_index_for_8_ranks(const csi_report_config::port_index_for_8_ranks& cfg)
{
  port_idx_for8_ranks_c out{};
  switch (cfg.port_index_type) {
    case csi_report_config::port_index_for_8_ranks::port_index_type_t::port_index_1: {
      out.set_port_idx1();
      break;
    }
    case csi_report_config::port_index_for_8_ranks::port_index_type_t::port_index_2: {
      auto& out_p_idx = out.set_port_idx2();
      if (cfg.rank1_x.has_value()) {
        out_p_idx.rank1_2_present = true;
        out_p_idx.rank1_2         = cfg.rank1_x.value();
      }
      if (not cfg.rank2_x.empty()) {
        out_p_idx.rank2_2_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank2_2, cfg.rank2_x);
      }
      break;
    }
    case csi_report_config::port_index_for_8_ranks::port_index_type_t::port_index_4: {
      auto& out_p_idx = out.set_port_idx4();
      if (cfg.rank1_x.has_value()) {
        out_p_idx.rank1_4_present = true;
        out_p_idx.rank1_4         = cfg.rank1_x.value();
      }
      if (not cfg.rank2_x.empty()) {
        out_p_idx.rank2_4_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank2_4, cfg.rank2_x);
      }
      if (not cfg.rank3_x.empty()) {
        out_p_idx.rank3_4_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank3_4, cfg.rank3_x);
      }
      if (not cfg.rank4_x.empty()) {
        out_p_idx.rank4_4_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank4_4, cfg.rank4_x);
      }
      break;
    }
    case csi_report_config::port_index_for_8_ranks::port_index_type_t::port_index_8: {
      auto& out_p_idx = out.set_port_idx8();
      if (cfg.rank1_x.has_value()) {
        out_p_idx.rank1_8_present = true;
        out_p_idx.rank1_8         = cfg.rank1_x.value();
      }
      if (not cfg.rank2_x.empty()) {
        out_p_idx.rank2_8_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank2_8, cfg.rank2_x);
      }
      if (not cfg.rank3_x.empty()) {
        out_p_idx.rank3_8_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank3_8, cfg.rank3_x);
      }
      if (not cfg.rank4_x.empty()) {
        out_p_idx.rank4_8_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank4_8, cfg.rank4_x);
      }
      if (not cfg.rank5_x.empty()) {
        out_p_idx.rank5_8_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank5_8, cfg.rank5_x);
      }
      if (not cfg.rank6_x.empty()) {
        out_p_idx.rank6_8_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank6_8, cfg.rank6_x);
      }
      if (not cfg.rank7_x.empty()) {
        out_p_idx.rank7_8_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank7_8, cfg.rank7_x);
      }
      if (not cfg.rank8_x.empty()) {
        out_p_idx.rank8_8_present = true;
        make_asn1_port_index_each_rank(out_p_idx.rank8_8, cfg.rank8_x);
      }
      break;
    }
    default:
      srsran_assertion_failure("Invalid port index type={}", cfg.port_index_type);
  }
  return out;
}

static asn1::rrc_nr::csi_report_cfg_s make_asn1_csi_report_config(const csi_report_config& cfg)
{
  csi_report_cfg_s out;
  out.report_cfg_id = cfg.report_cfg_id;
  if (cfg.carrier.has_value()) {
    out.carrier_present = true;
    out.carrier         = cfg.carrier.value();
  }
  out.res_for_ch_meas = cfg.res_for_channel_meas;
  if (cfg.csi_im_res_for_interference.has_value()) {
    out.csi_im_res_for_interference_present = true;
    out.csi_im_res_for_interference         = cfg.csi_im_res_for_interference.value();
  }
  if (cfg.nzp_csi_rs_res_for_interference.has_value()) {
    out.nzp_csi_rs_res_for_interference_present = true;
    out.nzp_csi_rs_res_for_interference         = cfg.nzp_csi_rs_res_for_interference.value();
  }

  if (variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(cfg.report_cfg_type)) {
    const auto& rep_cfg_val =
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(cfg.report_cfg_type);
    if (rep_cfg_val.report_type ==
        csi_report_config::periodic_or_semi_persistent_report_on_pucch::report_type_t::periodic) {
      auto& rep_cfg = out.report_cfg_type.set_periodic();
      make_asn1_csi_report_periodicity_and_offset(
          rep_cfg.report_slot_cfg, rep_cfg_val.report_slot_period, rep_cfg_val.report_slot_offset);
      for (const auto& pucch_csi_res : rep_cfg_val.pucch_csi_res_list) {
        rep_cfg.pucch_csi_res_list.push_back(
            {.ul_bw_part_id = pucch_csi_res.ul_bwp, .pucch_res = static_cast<uint8_t>(pucch_csi_res.pucch_res_id)});
      }
    } else if (rep_cfg_val.report_type ==
               csi_report_config::periodic_or_semi_persistent_report_on_pucch::report_type_t::semi_persistent) {
      auto& rep_cfg = out.report_cfg_type.semi_persistent_on_pucch();
      make_asn1_csi_report_periodicity_and_offset(
          rep_cfg.report_slot_cfg, rep_cfg_val.report_slot_period, rep_cfg_val.report_slot_offset);
      for (const auto& pucch_csi_res : rep_cfg_val.pucch_csi_res_list) {
        rep_cfg.pucch_csi_res_list.push_back(
            {.ul_bw_part_id = pucch_csi_res.ul_bwp, .pucch_res = static_cast<uint8_t>(pucch_csi_res.pucch_res_id)});
      }
    }
  } else if (variant_holds_alternative<csi_report_config::semi_persistent_report_on_pusch>(cfg.report_cfg_type)) {
    const auto& rep_cfg_val = variant_get<csi_report_config::semi_persistent_report_on_pusch>(cfg.report_cfg_type);
    auto&       rep_cfg     = out.report_cfg_type.semi_persistent_on_pusch();
    switch (rep_cfg_val.slot_cfg) {
      case csi_report_periodicity::slots5:
        rep_cfg.report_slot_cfg =
            csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::sl5;
        break;
      case csi_report_periodicity::slots10:
        rep_cfg.report_slot_cfg =
            csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::sl10;
        break;
      case csi_report_periodicity::slots20:
        rep_cfg.report_slot_cfg =
            csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::sl20;
        break;
      case csi_report_periodicity::slots40:
        rep_cfg.report_slot_cfg =
            csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::sl40;
        break;
      case csi_report_periodicity::slots80:
        rep_cfg.report_slot_cfg =
            csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::sl80;
        break;
      case csi_report_periodicity::slots160:
        rep_cfg.report_slot_cfg =
            csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::sl160;
        break;
      case csi_report_periodicity::slots320:
        rep_cfg.report_slot_cfg =
            csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::sl320;
        break;
      default:
        srsran_assertion_failure("Invalid CSI report periodicity={}", rep_cfg_val.slot_cfg);
    }
    for (const auto& offset : rep_cfg_val.report_slot_offset_list) {
      rep_cfg.report_slot_offset_list.push_back(offset);
    }
    rep_cfg.p0alpha = rep_cfg_val.p0_alpha;
  } else if (variant_holds_alternative<csi_report_config::aperiodic_report>(cfg.report_cfg_type)) {
    const auto& rep_cfg_val = variant_get<csi_report_config::aperiodic_report>(cfg.report_cfg_type);
    auto&       rep_cfg     = out.report_cfg_type.set_aperiodic();
    for (const auto& offset : rep_cfg_val.report_slot_offset_list) {
      rep_cfg.report_slot_offset_list.push_back(offset);
    }
  }

  switch (cfg.report_qty_type) {
    case csi_report_config::report_quantity_type_t::none:
      out.report_quant.set_none();
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_pmi_cqi:
      out.report_quant.set_cri_ri_pmi_cqi();
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_i1:
      out.report_quant.set_cri_ri_i1();
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_i1_cqi: {
      auto& qty = out.report_quant.set_cri_ri_i1_cqi();
      qty.pdsch_bundle_size_for_csi_present =
          cfg.pdsch_bundle_size_for_csi != csi_report_config::csi_ri_i1_cqi_pdsch_bundle_size_for_csi::none;
      qty.pdsch_bundle_size_for_csi =
          cfg.pdsch_bundle_size_for_csi == csi_report_config::csi_ri_i1_cqi_pdsch_bundle_size_for_csi::n2
              ? csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_opts::n2
              : csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_opts::n4;
      break;
    }
    case csi_report_config::report_quantity_type_t::cri_ri_cqi:
      out.report_quant.set_cri_ri_cqi();
      break;
    case csi_report_config::report_quantity_type_t::cri_rsrp:
      out.report_quant.set_cri_rsrp();
      break;
    case csi_report_config::report_quantity_type_t::ssb_index_rsrp:
      out.report_quant.set_ssb_idx_rsrp();
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_li_pmi_cqi:
      out.report_quant.set_cri_ri_li_pmi_cqi();
      break;
    default:
      srsran_assertion_failure("Invalid CSI report quantity={}", cfg.report_qty_type);
  }

  if (cfg.report_freq_cfg.has_value()) {
    out.report_freq_cfg_present = true;

    out.report_freq_cfg.cqi_format_ind_present =
        cfg.report_freq_cfg.value().cqi_format_ind != csi_report_config::cqi_format_indicator::none;
    out.report_freq_cfg.cqi_format_ind =
        cfg.report_freq_cfg.value().cqi_format_ind == csi_report_config::cqi_format_indicator::wideband_cqi
            ? csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi
            : csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::subband_cqi;

    out.report_freq_cfg.pmi_format_ind_present =
        cfg.report_freq_cfg.value().pmi_format_ind != csi_report_config::pmi_format_indicator::none;
    out.report_freq_cfg.pmi_format_ind =
        cfg.report_freq_cfg.value().pmi_format_ind == csi_report_config::pmi_format_indicator::wideband_pmi
            ? csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_opts::wideband_pmi
            : csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_opts::subband_pmi;

    if (cfg.report_freq_cfg.value().nof_csi_reporting_subbands.has_value()) {
      out.report_freq_cfg.csi_report_band_present = true;
      switch (cfg.report_freq_cfg.value().nof_csi_reporting_subbands.value()) {
        case 3: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands3();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 4: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands4();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 5: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands5();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 6: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands6();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 7: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands7();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 8: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands8();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 9: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands9();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 10: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands10();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 11: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands11();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 12: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands12();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 13: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands13();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 14: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands14();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 15: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands15();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 16: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands16();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 17: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands17();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 18: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands18();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
        case 19: {
          auto& sb = out.report_freq_cfg.csi_report_band.set_subbands19_v1530();
          sb.from_number(cfg.report_freq_cfg.value().csi_reporting_subbands_bitmap.to_uint64());
          break;
        }
      }
    }
  }

  out.time_restrict_for_ch_meass = cfg.is_time_restrictions_for_channel_meas_configured
                                       ? csi_report_cfg_s::time_restrict_for_ch_meass_opts::cfg
                                       : csi_report_cfg_s::time_restrict_for_ch_meass_opts::not_cfg;

  out.time_restrict_for_interference_meass = cfg.is_time_restrictions_for_interference_meas_configured
                                                 ? csi_report_cfg_s::time_restrict_for_interference_meass_opts::cfg
                                                 : csi_report_cfg_s::time_restrict_for_interference_meass_opts::not_cfg;

  if (cfg.codebook_cfg.has_value()) {
    out.codebook_cfg_present = true;
    make_asn1_codebook_config(out.codebook_cfg, cfg.codebook_cfg.value());
  }

  if (cfg.is_group_based_beam_reporting_enabled) {
    out.group_based_beam_report.set_enabled();
  } else {
    auto& grp_beam_report = out.group_based_beam_report.set_disabled();
    if (cfg.nof_reported_rs.has_value()) {
      grp_beam_report.nrof_reported_rs_present = true;
      switch (cfg.nof_reported_rs.value()) {
        case 1:
          grp_beam_report.nrof_reported_rs =
              csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::n1;
          break;
        case 2:
          grp_beam_report.nrof_reported_rs =
              csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::n2;
          break;
        case 3:
          grp_beam_report.nrof_reported_rs =
              csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::n3;
          break;
        case 4:
          grp_beam_report.nrof_reported_rs =
              csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::n4;
          break;
        default:
          srsran_assertion_failure("Invalid nof. reported RS={}", cfg.nof_reported_rs.value());
      }
    }
  }

  if (cfg.cqi_table.has_value()) {
    out.cqi_table_present = true;
    switch (cfg.cqi_table.value()) {
      case csi_report_config::cqi_table_t::table1:
        out.cqi_table = csi_report_cfg_s::cqi_table_opts::table1;
        break;
      case csi_report_config::cqi_table_t::table2:
        out.cqi_table = csi_report_cfg_s::cqi_table_opts::table2;
        break;
      case csi_report_config::cqi_table_t::table3:
        out.cqi_table = csi_report_cfg_s::cqi_table_opts::table3;
        break;
      case csi_report_config::cqi_table_t::table4:
        out.cqi_table = csi_report_cfg_s::cqi_table_opts::table4_r17;
        break;
      default:
        srsran_assertion_failure("Invalid CQI table={}", cfg.cqi_table.value());
    }
  }

  switch (cfg.subband_size) {
    case csi_report_config::subband_size_t::value1:
      out.subband_size = csi_report_cfg_s::subband_size_opts::value1;
      break;
    case csi_report_config::subband_size_t::value2:
      out.subband_size = csi_report_cfg_s::subband_size_opts::value2;
      break;
    default:
      srsran_assertion_failure("Invalid subband size={}", cfg.subband_size);
  }

  for (const auto& port_idx : cfg.non_pmi_port_indication) {
    out.non_pmi_port_ind.push_back(make_asn1_port_index_for_8_ranks(port_idx));
  }

  return out;
}

static asn1::rrc_nr::csi_aperiodic_trigger_state_s
make_asn1_aperiodic_trigger_state(const csi_aperiodic_trigger_state& cfg)
{
  csi_aperiodic_trigger_state_s out{};
  for (const auto& report_cfg_info : cfg.associated_report_cfg_info_list) {
    csi_associated_report_cfg_info_s out_report_cfg_info{};
    out_report_cfg_info.report_cfg_id = report_cfg_info.report_cfg_id;

    if (variant_holds_alternative<csi_associated_report_config_info::nzp_csi_rs>(report_cfg_info.res_for_channel)) {
      const auto& res_for_chnl =
          variant_get<csi_associated_report_config_info::nzp_csi_rs>(report_cfg_info.res_for_channel);
      auto& out_res_for_chnl   = out_report_cfg_info.res_for_ch.set_nzp_csi_rs();
      out_res_for_chnl.res_set = res_for_chnl.resource_set;
      for (const auto& qcl_info : res_for_chnl.qcl_info) {
        out_res_for_chnl.qcl_info.push_back(qcl_info);
      }
    } else if (variant_holds_alternative<csi_associated_report_config_info::csi_ssb_resource_set>(
                   report_cfg_info.res_for_channel)) {
      const auto& res_for_chnl =
          variant_get<csi_associated_report_config_info::csi_ssb_resource_set>(report_cfg_info.res_for_channel);
      auto& out_res_for_chnl = out_report_cfg_info.res_for_ch.set_csi_ssb_res_set();
      out_res_for_chnl       = res_for_chnl;
    }

    if (report_cfg_info.nzp_csi_rs_resources_for_interference.has_value()) {
      out_report_cfg_info.nzp_csi_rs_res_for_interference_present = true;
      out_report_cfg_info.nzp_csi_rs_res_for_interference =
          report_cfg_info.nzp_csi_rs_resources_for_interference.value();
    }

    if (report_cfg_info.csi_im_resources_for_interference.has_value()) {
      out_report_cfg_info.csi_im_res_for_interference_present = true;
      out_report_cfg_info.csi_im_res_for_interference = report_cfg_info.csi_im_resources_for_interference.value();
    }

    out.associated_report_cfg_info_list.push_back(out_report_cfg_info);
  }
  return out;
}

static asn1::rrc_nr::csi_semi_persistent_on_pusch_trigger_state_s
make_asn1_semi_persistent_on_pusch_trigger_state(const csi_semi_persistent_on_pusch_trigger_state& cfg)
{
  csi_semi_persistent_on_pusch_trigger_state_s out{};
  out.associated_report_cfg_info = cfg.associated_report_cfg_info;
  return out;
}

void srsran::srs_du::calculate_csi_meas_config_diff(asn1::rrc_nr::csi_meas_cfg_s& out,
                                                    const csi_meas_config&        src,
                                                    const csi_meas_config&        dest)
{
  calculate_addmodremlist_diff(
      out.nzp_csi_rs_res_to_add_mod_list,
      out.nzp_csi_rs_res_to_release_list,
      src.nzp_csi_rs_res_list,
      dest.nzp_csi_rs_res_list,
      [](const nzp_csi_rs_resource& res) { return make_asn1_nzp_csi_rs_resource(res); },
      [](const nzp_csi_rs_resource& res) { return res.res_id; });

  calculate_addmodremlist_diff(
      out.nzp_csi_rs_res_set_to_add_mod_list,
      out.nzp_csi_rs_res_set_to_release_list,
      src.nzp_csi_rs_res_set_list,
      dest.nzp_csi_rs_res_set_list,
      [](const nzp_csi_rs_resource_set& res) { return make_asn1_nzp_csi_rs_resource_set(res); },
      [](const nzp_csi_rs_resource_set& res) { return res.res_set_id; });

  calculate_addmodremlist_diff(
      out.csi_im_res_to_add_mod_list,
      out.csi_im_res_to_release_list,
      src.csi_im_res_list,
      dest.csi_im_res_list,
      [](const csi_im_resource& res) { return make_asn1_csi_im_resource(res); },
      [](const csi_im_resource& res) { return res.res_id; });

  calculate_addmodremlist_diff(
      out.csi_im_res_set_to_add_mod_list,
      out.csi_im_res_set_to_release_list,
      src.csi_im_res_set_list,
      dest.csi_im_res_set_list,
      [](const csi_im_resource_set& res) { return make_asn1_csi_im_resource_set(res); },
      [](const csi_im_resource_set& res) { return res.res_set_id; });

  calculate_addmodremlist_diff(
      out.csi_ssb_res_set_to_add_mod_list,
      out.csi_ssb_res_set_to_release_list,
      src.csi_ssb_res_set_list,
      dest.csi_ssb_res_set_list,
      [](const csi_ssb_resource_set& res) { return make_asn1_csi_ssb_resource_set(res); },
      [](const csi_ssb_resource_set& res) { return res.res_set_id; });

  calculate_addmodremlist_diff(
      out.csi_res_cfg_to_add_mod_list,
      out.csi_res_cfg_to_release_list,
      src.csi_res_cfg_list,
      dest.csi_res_cfg_list,
      [](const csi_resource_config& res) { return make_asn1_csi_resource_config(res); },
      [](const csi_resource_config& res) { return res.res_cfg_id; });

  calculate_addmodremlist_diff(
      out.csi_report_cfg_to_add_mod_list,
      out.csi_report_cfg_to_release_list,
      src.csi_report_cfg_list,
      dest.csi_report_cfg_list,
      [](const csi_report_config& res) { return make_asn1_csi_report_config(res); },
      [](const csi_report_config& res) { return res.report_cfg_id; });

  if (dest.report_trigger_size.has_value()) {
    out.report_trigger_size_present = true;
    out.report_trigger_size         = dest.report_trigger_size.value();
  }

  if ((dest.aperiodic_trigger_state_list.has_value() and not src.aperiodic_trigger_state_list.has_value()) ||
      (dest.aperiodic_trigger_state_list.has_value() and src.aperiodic_trigger_state_list.has_value() and
       dest.aperiodic_trigger_state_list != src.aperiodic_trigger_state_list)) {
    out.aperiodic_trigger_state_list_present = true;
    auto& ap_trigger_state_list              = out.aperiodic_trigger_state_list.set_setup();
    for (const auto& trigger_state : dest.aperiodic_trigger_state_list.value()) {
      ap_trigger_state_list.push_back(make_asn1_aperiodic_trigger_state(trigger_state));
    }
  } else if (src.aperiodic_trigger_state_list.has_value() and not dest.aperiodic_trigger_state_list.has_value()) {
    out.aperiodic_trigger_state_list_present = true;
    out.aperiodic_trigger_state_list.set_release();
  }

  if ((dest.semi_persistent_on_pusch_trigger_state_list.has_value() and
       not src.semi_persistent_on_pusch_trigger_state_list.has_value()) ||
      (dest.semi_persistent_on_pusch_trigger_state_list.has_value() and
       src.semi_persistent_on_pusch_trigger_state_list.has_value() and
       dest.semi_persistent_on_pusch_trigger_state_list != src.semi_persistent_on_pusch_trigger_state_list)) {
    out.semi_persistent_on_pusch_trigger_state_list_present = true;
    auto& sp_trigger_state_list = out.semi_persistent_on_pusch_trigger_state_list.set_setup();
    for (const auto& trigger_state : dest.semi_persistent_on_pusch_trigger_state_list.value()) {
      sp_trigger_state_list.push_back(make_asn1_semi_persistent_on_pusch_trigger_state(trigger_state));
    }
  } else if (src.semi_persistent_on_pusch_trigger_state_list.has_value() and
             not dest.semi_persistent_on_pusch_trigger_state_list.has_value()) {
    out.semi_persistent_on_pusch_trigger_state_list_present = true;
    out.semi_persistent_on_pusch_trigger_state_list.set_release();
  }
}
