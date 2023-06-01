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

#include "rrc_ue_test_messages.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"

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

cu_cp_rrc_reconfiguration_procedure_request srsran::srs_cu_cp::generate_rrc_reconfiguration_procedure_request()
{
  cu_cp_rrc_reconfiguration_procedure_request args;

  // add dummy radio bearer config
  cu_cp_radio_bearer_config rb_cfg;

  cu_cp_drb_to_add_mod drb_item;

  drb_item.drb_id = uint_to_drb_id(1);

  cu_cp_cn_assoc cn_assoc;
  cn_assoc.eps_bearer_id = 5;
  drb_item.cn_assoc      = cn_assoc;

  pdcp_config pdcp_config = {};

  pdcp_config.rb_type  = pdcp_rb_type::drb;
  pdcp_config.rlc_mode = pdcp_rlc_mode::am;

  pdcp_config.tx.sn_size       = pdcp_sn_size::size18bits;
  pdcp_config.rx.sn_size       = pdcp_sn_size::size18bits;
  pdcp_config.tx.discard_timer = pdcp_discard_timer::ms100;

  pdcp_config.rx.t_reordering = pdcp_t_reordering::ms0;
  drb_item.pdcp_cfg           = pdcp_config;

  cu_cp_security_config security_config;

  security_config.key_to_use = "secondary";

  cu_cp_security_algorithm_config security_algorithm_config;
  security_algorithm_config.ciphering_algorithm = "nea2";
  security_config.security_algorithm_cfg        = security_algorithm_config;

  rb_cfg.security_cfg = security_config;

  rb_cfg.drb_to_add_mod_list.emplace(drb_item.drb_id, drb_item);

  args.radio_bearer_cfg = rb_cfg;

  cu_cp_rrc_recfg_v1530_ies non_crit_ext;
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
  rrc_reest_req.rrc_reest_request.ue_id.c_rnti = c_rnti;
  rrc_reest_req.rrc_reest_request.ue_id.pci    = pci;
  rrc_reest_req.rrc_reest_request.ue_id.short_mac_i.from_number(0);
  rrc_reest_req.rrc_reest_request.reest_cause = asn1::rrc_nr::reest_cause_opts::options::other_fail;
  rrc_reest_req.rrc_reest_request.spare.from_number(0);

  const asn1::SRSASN_CODE ret = ul_ccch_msg.pack(bref);
  srsran_assert(ret == asn1::SRSASN_SUCCESS, "Failed to pack RRC PDU.");

  return pdu;
}

byte_buffer srsran::srs_cu_cp::generate_valid_rrc_reestablishment_request_pdu(pci_t pci, rnti_t c_rnti)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};

  asn1::rrc_nr::ul_ccch_msg_s ul_ccch_msg{};
  auto&                       ccch_c1          = ul_ccch_msg.msg.set_c1();
  auto&                       rrc_reest_req    = ccch_c1.set_rrc_reest_request();
  rrc_reest_req.rrc_reest_request.ue_id.c_rnti = c_rnti;
  rrc_reest_req.rrc_reest_request.ue_id.pci    = pci;
  rrc_reest_req.rrc_reest_request.ue_id.short_mac_i.from_number(0); // TODO: Add valid short mac i
  rrc_reest_req.rrc_reest_request.reest_cause = asn1::rrc_nr::reest_cause_opts::options::other_fail;
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