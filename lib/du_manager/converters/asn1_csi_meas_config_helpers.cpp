/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "asn1_csi_meas_config_helpers.h"
#include "../../asn1/asn1_diff_utils.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;
using namespace asn1::rrc_nr;

asn1::rrc_nr::csi_rs_res_map_s make_asn1_nzp_csi_rs_resource_mapping(const csi_rs_resource_mapping& cfg)
{
  csi_rs_res_map_s out{};
  if (variant_holds_alternative<csi_rs_resource_mapping::fd_alloc_row1>(cfg.fd_alloc)) {
    auto& row     = out.freq_domain_alloc.set_row1();
    auto& row_val = variant_get<csi_rs_resource_mapping::fd_alloc_row1>(cfg.fd_alloc);
    row.from_number(row_val.to_uint64());
  } else if (variant_holds_alternative<csi_rs_resource_mapping::fd_alloc_row2>(cfg.fd_alloc)) {
    auto& row     = out.freq_domain_alloc.set_row2();
    auto& row_val = variant_get<csi_rs_resource_mapping::fd_alloc_row2>(cfg.fd_alloc);
    row.from_number(row_val.to_uint64());
  } else if (variant_holds_alternative<csi_rs_resource_mapping::fd_alloc_row4>(cfg.fd_alloc)) {
    auto& row     = out.freq_domain_alloc.set_row4();
    auto& row_val = variant_get<csi_rs_resource_mapping::fd_alloc_row4>(cfg.fd_alloc);
    row.from_number(row_val.to_uint64());
  } else if (variant_holds_alternative<csi_rs_resource_mapping::fd_alloc_other>(cfg.fd_alloc)) {
    auto& row     = out.freq_domain_alloc.set_other();
    auto& row_val = variant_get<csi_rs_resource_mapping::fd_alloc_other>(cfg.fd_alloc);
    row.from_number(row_val.to_uint64());
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
      srsgnb_assertion_failure("Invalid nof. NZP CSI-RS ports={}", cfg.nof_ports);
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
      srsgnb_assertion_failure("Invalid CDM={}", cfg.cdm);
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
      srsgnb_assertion_failure("Invalid freq. density={}", cfg.freq_density);
  }

  out.freq_band.start_rb = cfg.freq_band_start_rb;
  out.freq_band.nrof_rbs = cfg.freq_band_nof_rb;

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
        srsgnb_assertion_failure("Invalid power control offset SS={}", cfg.pwr_ctrl_offset_ss_db.value());
    }
  }
  out.scrambling_id = cfg.scrambling_id;

  if (cfg.csi_res_period.has_value() and cfg.csi_res_offset.has_value()) {
    out.periodicity_and_offset_present = true;
    switch (cfg.csi_res_period.value()) {
      case csi_resource_periodicity::slots4: {
        auto& p_and_o = out.periodicity_and_offset.set_slots4();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots5: {
        auto& p_and_o = out.periodicity_and_offset.set_slots5();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots8: {
        auto& p_and_o = out.periodicity_and_offset.set_slots8();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots10: {
        auto& p_and_o = out.periodicity_and_offset.set_slots10();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots16: {
        auto& p_and_o = out.periodicity_and_offset.set_slots16();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots20: {
        auto& p_and_o = out.periodicity_and_offset.set_slots20();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots32: {
        auto& p_and_o = out.periodicity_and_offset.set_slots32();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots40: {
        auto& p_and_o = out.periodicity_and_offset.set_slots40();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots64: {
        auto& p_and_o = out.periodicity_and_offset.set_slots64();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots80: {
        auto& p_and_o = out.periodicity_and_offset.set_slots80();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots160: {
        auto& p_and_o = out.periodicity_and_offset.set_slots160();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots320: {
        auto& p_and_o = out.periodicity_and_offset.set_slots320();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      case csi_resource_periodicity::slots640: {
        auto& p_and_o = out.periodicity_and_offset.set_slots640();
        p_and_o       = cfg.csi_res_offset.value();
        break;
      }
      default:
        srsgnb_assertion_failure("Invalid CSI-RS periodicity={}", cfg.csi_res_period.value());
    }
  }

  if (cfg.tci_state_id.has_value()) {
    out.qcl_info_periodic_csi_rs_present = true;
    out.qcl_info_periodic_csi_rs         = cfg.tci_state_id.value();
  }

  return out;
}

void srsgnb::srs_du::calculate_csi_meas_config_diff(asn1::rrc_nr::csi_meas_cfg_s& out,
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
}
