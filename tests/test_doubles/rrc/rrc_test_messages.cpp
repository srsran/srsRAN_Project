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

#include "rrc_test_messages.h"
#include "srsran/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"

using namespace srsran;
using namespace asn1::rrc_nr;

ul_ccch_msg_s srsran::create_rrc_setup_request()
{
  ul_ccch_msg_s msg;

  rrc_setup_request_s& req = msg.msg.set_c1().set_rrc_setup_request();

  req.rrc_setup_request.establishment_cause.value = establishment_cause_opts::mo_sig;
  req.rrc_setup_request.ue_id.set_random_value().from_string("111000010100010001111111000010001100111");

  return msg;
}

ul_ccch_msg_s
srsran::create_rrc_reestablishment_request(rnti_t old_crnti, pci_t old_pci, const std::string& short_mac_i)
{
  ul_ccch_msg_s msg;

  rrc_reest_request_s& req = msg.msg.set_c1().set_rrc_reest_request();

  req.rrc_reest_request.ue_id.c_rnti = (uint32_t)old_crnti;
  req.rrc_reest_request.ue_id.pci    = (uint16_t)old_pci;
  req.rrc_reest_request.ue_id.short_mac_i.from_string(short_mac_i);
  req.rrc_reest_request.reest_cause.value = reest_cause_opts::other_fail;

  return msg;
}

ul_dcch_msg_s srsran::create_rrc_setup_complete()
{
  ul_dcch_msg_s msg;

  rrc_setup_complete_s& req = msg.msg.set_c1().set_rrc_setup_complete();

  req.rrc_transaction_id        = 0;
  rrc_setup_complete_ies_s& ies = req.crit_exts.set_rrc_setup_complete();

  ies.sel_plmn_id = 1;
  ies.ded_nas_msg.from_string("7e004179000d0100f110f0ff000010325476092e02e0e0");

  return msg;
}

asn1::rrc_nr::ul_dcch_msg_s srsran::create_rrc_reestablishment_complete()
{
  ul_dcch_msg_s msg;

  rrc_reest_complete_s& req = msg.msg.set_c1().set_rrc_reest_complete();
  req.rrc_transaction_id    = 0;
  req.crit_exts.set_rrc_reest_complete();

  return msg;
}

asn1::rrc_nr::ul_dcch_msg_s srsran::create_rrc_reconfiguration_complete(uint8_t transaction_id)
{
  ul_dcch_msg_s msg;

  rrc_recfg_complete_s& req = msg.msg.set_c1().set_rrc_recfg_complete();
  req.rrc_transaction_id    = transaction_id;
  req.crit_exts.set_rrc_recfg_complete();

  return msg;
}

byte_buffer srsran::pack_ul_ccch_msg(const ul_ccch_msg_s& msg)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}

byte_buffer srsran::pack_ul_dcch_msg(const ul_dcch_msg_s& msg)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}
