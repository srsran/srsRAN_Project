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

#include "rrc_ue_test_messages.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/security/security.h"

using namespace srsran;
using namespace srs_cu_cp;

security::sec_key srsran::srs_cu_cp::make_sec_key(std::string hex_str)
{
  byte_buffer       key_buf = make_byte_buffer(hex_str);
  security::sec_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

security::sec_128_key srsran::srs_cu_cp::make_sec_128_key(std::string hex_str)
{
  byte_buffer           key_buf = make_byte_buffer(hex_str);
  security::sec_128_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

rrc_meas_cfg srsran::srs_cu_cp::generate_dummy_meas_config()
{
  rrc_meas_cfg meas_cfg;

  // meas obj to add mod
  rrc_meas_obj_to_add_mod meas_obj_to_add_mod;

  meas_obj_to_add_mod.meas_obj_id = uint_to_meas_obj_id(1);

  rrc_meas_obj_nr meas_obj_nr;
  meas_obj_nr.ssb_freq               = 627242;
  meas_obj_nr.ssb_subcarrier_spacing = subcarrier_spacing::kHz15;

  rrc_ssb_mtc smtc1;

  smtc1.periodicity_and_offset.sf20 = 0;
  smtc1.dur                         = 2;
  meas_obj_nr.smtc1                 = smtc1;

  rrc_ssb_cfg_mob ssb_cfg_mob;
  ssb_cfg_mob.derive_ssb_idx_from_cell = true;
  meas_obj_nr.ref_sig_cfg.ssb_cfg_mob  = ssb_cfg_mob;

  rrc_thres_nr thres;
  thres.thres_rsrp                               = 36;
  meas_obj_nr.abs_thresh_ss_blocks_consolidation = thres;

  meas_obj_nr.nrof_ss_blocks_to_average = 8;
  meas_obj_nr.quant_cfg_idx             = 1;
  meas_obj_nr.freq_band_ind_nr          = 78;

  meas_obj_to_add_mod.meas_obj_nr = meas_obj_nr;

  meas_cfg.meas_obj_to_add_mod_list.push_back(meas_obj_to_add_mod);

  // report config to add mod
  rrc_report_cfg_to_add_mod report_cfg_to_add_mod;

  report_cfg_to_add_mod.report_cfg_id = uint_to_report_cfg_id(1);

  rrc_report_cfg_nr         report_cfg_nr;
  rrc_periodical_report_cfg periodical;

  periodical.rs_type                = rrc_nr_rs_type::ssb;
  periodical.report_interv          = 1024;
  periodical.report_amount          = -1;
  periodical.report_quant_cell.rsrp = true;
  periodical.report_quant_cell.rsrq = true;
  periodical.report_quant_cell.sinr = true;
  periodical.max_report_cells       = 4;

  rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp       = true;
  report_quant_rs_idxes.rsrq       = true;
  report_quant_rs_idxes.sinr       = true;
  periodical.report_quant_rs_idxes = report_quant_rs_idxes;

  periodical.max_nrof_rs_idxes_to_report = 4;
  periodical.include_beam_meass          = true;
  periodical.use_allowed_cell_list       = false;

  report_cfg_nr.periodical         = periodical;
  report_cfg_to_add_mod.report_cfg = report_cfg_nr;

  meas_cfg.report_cfg_to_add_mod_list.push_back(report_cfg_to_add_mod);

  // meas id to add mod list
  rrc_meas_id_to_add_mod meas_id_to_add_mod;

  meas_id_to_add_mod.meas_id       = uint_to_meas_id(1);
  meas_id_to_add_mod.meas_obj_id   = uint_to_meas_obj_id(1);
  meas_id_to_add_mod.report_cfg_id = uint_to_report_cfg_id(1);

  meas_cfg.meas_id_to_add_mod_list.push_back(meas_id_to_add_mod);

  // quant config
  rrc_quant_cfg quant_cfg;

  rrc_quant_cfg_nr quant_cfg_nr;
  quant_cfg_nr.quant_cfg_cell.ssb_filt_cfg.filt_coef_rsrp    = 6;
  quant_cfg_nr.quant_cfg_cell.csi_rs_filt_cfg.filt_coef_rsrp = 6;

  quant_cfg.quant_cfg_nr_list.push_back(quant_cfg_nr);

  meas_cfg.quant_cfg = quant_cfg;

  return meas_cfg;
}

rrc_reconfiguration_procedure_request srsran::srs_cu_cp::generate_rrc_reconfiguration_procedure_request()
{
  rrc_reconfiguration_procedure_request args;

  // add dummy radio bearer config
  rrc_radio_bearer_config rb_cfg;

  rrc_drb_to_add_mod drb_item;

  drb_item.drb_id = uint_to_drb_id(1);

  rrc_cn_assoc cn_assoc;
  cn_assoc.eps_bearer_id = 5;
  drb_item.cn_assoc      = cn_assoc;

  pdcp_config pdcp_config = {};

  pdcp_config.rb_type  = pdcp_rb_type::drb;
  pdcp_config.rlc_mode = pdcp_rlc_mode::am;

  pdcp_config.tx.sn_size       = pdcp_sn_size::size18bits;
  pdcp_config.rx.sn_size       = pdcp_sn_size::size18bits;
  pdcp_config.tx.discard_timer = pdcp_discard_timer::ms100;

  pdcp_config.rx.t_reordering    = pdcp_t_reordering::ms0;
  pdcp_config.ciphering_required = true;
  drb_item.pdcp_cfg              = pdcp_config;

  rrc_security_config security_config;

  security_config.key_to_use = "secondary";

  rrc_security_algorithm_config security_algorithm_config;
  security_algorithm_config.ciphering_algorithm = security::ciphering_algorithm::nea2;
  security_config.security_algorithm_cfg        = security_algorithm_config;

  rb_cfg.security_cfg = security_config;

  rb_cfg.drb_to_add_mod_list.emplace(drb_item.drb_id, drb_item);

  args.radio_bearer_cfg = rb_cfg;

  // add dummy meas config
  args.meas_cfg = generate_dummy_meas_config();

  rrc_recfg_v1530_ies non_crit_ext;
  // add dummy NAS PDU
  non_crit_ext.ded_nas_msg_list.push_back(make_byte_buffer("aabbcc"));

  // add dummy master cell group config
  non_crit_ext.master_cell_group = make_byte_buffer("deadbeef");

  args.non_crit_ext = non_crit_ext;

  return args;
}

byte_buffer srsran::srs_cu_cp::generate_invalid_rrc_reestablishment_request_pdu(pci_t pci, rnti_t c_rnti)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};

  asn1::rrc_nr::ul_ccch_msg_s ul_ccch_msg{};
  auto&                       ccch_c1          = ul_ccch_msg.msg.set_c1();
  auto&                       rrc_reest_req    = ccch_c1.set_rrc_reest_request();
  rrc_reest_req.rrc_reest_request.ue_id.c_rnti = to_value(c_rnti);
  rrc_reest_req.rrc_reest_request.ue_id.pci    = pci;
  rrc_reest_req.rrc_reest_request.ue_id.short_mac_i.from_number(0);
  rrc_reest_req.rrc_reest_request.reest_cause = asn1::rrc_nr::reest_cause_opts::options::other_fail;
  rrc_reest_req.rrc_reest_request.spare.from_number(0);

  const asn1::SRSASN_CODE ret = ul_ccch_msg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack RRC PDU.");

  return pdu;
}

byte_buffer srsran::srs_cu_cp::generate_valid_rrc_reestablishment_request_pdu(pci_t                       pci,
                                                                              rnti_t                      c_rnti,
                                                                              std::string                 short_mac_i,
                                                                              asn1::rrc_nr::reest_cause_e cause)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};

  asn1::rrc_nr::ul_ccch_msg_s ul_ccch_msg{};
  auto&                       ccch_c1          = ul_ccch_msg.msg.set_c1();
  auto&                       rrc_reest_req    = ccch_c1.set_rrc_reest_request();
  rrc_reest_req.rrc_reest_request.ue_id.c_rnti = to_value(c_rnti);
  rrc_reest_req.rrc_reest_request.ue_id.pci    = pci;
  rrc_reest_req.rrc_reest_request.ue_id.short_mac_i.from_string(short_mac_i);
  rrc_reest_req.rrc_reest_request.reest_cause = cause;
  rrc_reest_req.rrc_reest_request.spare.from_number(0);

  const asn1::SRSASN_CODE ret = ul_ccch_msg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack RRC PDU.");

  return pdu;
}

byte_buffer srsran::srs_cu_cp::generate_rrc_reestablishment_complete_pdu()
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};

  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg{};
  ul_dcch_msg.msg.set_c1().set_rrc_reest_complete();
  ul_dcch_msg.msg.c1().rrc_reest_complete().crit_exts.set_rrc_reest_complete();

  const asn1::SRSASN_CODE ret = ul_dcch_msg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack RRC PDU.");

  return pdu;
}

byte_buffer srsran::srs_cu_cp::generate_measurement_report_pdu()
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};

  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg{};
  auto& meas_results = ul_dcch_msg.msg.set_c1().set_meas_report().crit_exts.set_meas_report().meas_results;

  meas_results.meas_id = 1U;
  asn1::rrc_nr::meas_result_serv_mo_s meas_result_serv_mo;
  meas_result_serv_mo.serv_cell_id = 0U;
  // pci
  meas_result_serv_mo.meas_result_serving_cell.pci_present = true;
  meas_result_serv_mo.meas_result_serving_cell.pci         = 0U;
  // meas result
  // cell results
  meas_result_serv_mo.meas_result_serving_cell.meas_result.cell_results.results_ssb_cell_present      = true;
  meas_result_serv_mo.meas_result_serving_cell.meas_result.cell_results.results_ssb_cell.rsrp_present = true;
  meas_result_serv_mo.meas_result_serving_cell.meas_result.cell_results.results_ssb_cell.rsrp         = 113U;
  meas_result_serv_mo.meas_result_serving_cell.meas_result.cell_results.results_ssb_cell.rsrq_present = true;
  meas_result_serv_mo.meas_result_serving_cell.meas_result.cell_results.results_ssb_cell.rsrq         = 65U;
  meas_result_serv_mo.meas_result_serving_cell.meas_result.cell_results.results_ssb_cell.sinr_present = true;
  meas_result_serv_mo.meas_result_serving_cell.meas_result.cell_results.results_ssb_cell.sinr         = 92U;
  // rs idx results
  meas_result_serv_mo.meas_result_serving_cell.meas_result.rs_idx_results_present = true;

  asn1::rrc_nr::results_per_ssb_idx_s results_per_ssb_idx;
  results_per_ssb_idx.ssb_idx                  = 0U;
  results_per_ssb_idx.ssb_results_present      = true;
  results_per_ssb_idx.ssb_results.rsrp_present = true;
  results_per_ssb_idx.ssb_results.rsrp         = 113U;
  results_per_ssb_idx.ssb_results.rsrq_present = true;
  results_per_ssb_idx.ssb_results.rsrq         = 66U;
  results_per_ssb_idx.ssb_results.sinr_present = true;
  results_per_ssb_idx.ssb_results.sinr         = 92U;

  meas_result_serv_mo.meas_result_serving_cell.meas_result.rs_idx_results.results_ssb_idxes.push_back(
      results_per_ssb_idx);

  meas_results.meas_result_serving_mo_list.push_back(meas_result_serv_mo);

  const asn1::SRSASN_CODE ret = ul_dcch_msg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack RRC PDU.");

  return pdu;
}
