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

#include "f1ap_configuration_helpers.h"
#include "asn1_rrc_config_helpers.h"
#include "asn1_sys_info_packer.h"
#include "srsran/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "srsran/asn1/rrc_nr/meas_timing_cfg.h"
#include "srsran/ran/band_helper.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srs_du;

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
      report_fatal_error("Invalid SSB periodicity {}.", fmt::underlying(du_cfg.ssb_cfg.ssb_period));
  }
  meas_item.pci_present = true;
  meas_item.pci         = du_cfg.pci;

  asn1::SRSASN_CODE ret = cfg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack meas_time_cfg");
  return buf;
}

du_served_cell_info srsran::srs_du::make_f1ap_du_cell_info(const du_cell_config& du_cfg)
{
  du_served_cell_info serv_cell;

  serv_cell.nr_cgi     = du_cfg.nr_cgi;
  serv_cell.pci        = du_cfg.pci;
  serv_cell.tac        = du_cfg.tac;
  serv_cell.duplx_mode = du_cfg.tdd_ul_dl_cfg_common.has_value() ? duplex_mode::TDD : duplex_mode::FDD;
  serv_cell.scs_common = du_cfg.scs_common;
  serv_cell.dl_carrier = du_cfg.dl_carrier;
  if (serv_cell.duplx_mode == duplex_mode::FDD) {
    serv_cell.ul_carrier = du_cfg.ul_carrier;
  }
  serv_cell.packed_meas_time_cfg = make_asn1_meas_time_cfg_buffer(du_cfg);
  serv_cell.ntn_link_rtt         = std::chrono::milliseconds(du_cfg.ntn_cs_koffset);

  return serv_cell;
}

gnb_du_sys_info srsran::srs_du::make_f1ap_du_sys_info(const du_cell_config& du_cfg, std::string* js_str)
{
  gnb_du_sys_info sys_info;

  sys_info.packed_mib  = asn1_packer::pack_mib(du_cfg);
  sys_info.packed_sib1 = asn1_packer::pack_sib1(du_cfg, js_str);

  return sys_info;
}

void srsran::srs_du::fill_f1_setup_request(f1_setup_request_message&            req,
                                           const du_manager_params::ran_params& ran_params)
{
  req.gnb_du_id   = ran_params.gnb_du_id;
  req.gnb_du_name = ran_params.gnb_du_name;
  req.rrc_version = ran_params.rrc_version;

  req.served_cells.resize(ran_params.cells.size());

  for (unsigned i = 0; i != ran_params.cells.size(); ++i) {
    const du_cell_config& cell_cfg  = ran_params.cells[i];
    f1_cell_setup_params& serv_cell = req.served_cells[i];

    // Fill serving cell info.
    serv_cell.cell_index = to_du_cell_index(i);
    serv_cell.cell_info  = make_f1ap_du_cell_info(cell_cfg);
    for (const auto& slice : cell_cfg.rrm_policy_members) {
      serv_cell.slices.push_back(slice.rrc_member.s_nssai);
    }

    // Pack RRC ASN.1 Serving Cell system info.
    serv_cell.du_sys_info = make_f1ap_du_sys_info(cell_cfg, nullptr);
  }
}
