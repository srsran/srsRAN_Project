/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_test_messages.h"
#include "srsran/asn1/rrc_nr/ul_ccch_msg_ies.h"

using namespace srsran;

asn1::rrc_nr::ul_ccch_msg_s srsran::create_rrc_setup_request()
{
  asn1::rrc_nr::ul_ccch_msg_s msg;

  asn1::rrc_nr::rrc_setup_request_s& req = msg.msg.set_c1().set_rrc_setup_request();

  req.rrc_setup_request.establishment_cause.value = asn1::rrc_nr::establishment_cause_opts::mo_sig;
  req.rrc_setup_request.ue_id.set_random_value().from_string("111000010100010001111111000010001100111");

  return msg;
}

asn1::rrc_nr::ul_ccch_msg_s srsran::create_rrc_reestablishment_request(rnti_t old_crnti, pci_t old_pci)
{
  asn1::rrc_nr::ul_ccch_msg_s msg;

  asn1::rrc_nr::rrc_reest_request_s& req = msg.msg.set_c1().set_rrc_reest_request();

  req.rrc_reest_request.ue_id.c_rnti = (uint32_t)old_crnti;
  req.rrc_reest_request.ue_id.pci    = (uint16_t)old_pci;
  req.rrc_reest_request.ue_id.short_mac_i.from_string("1110101011111111");
  req.rrc_reest_request.reest_cause.value = asn1::rrc_nr::reest_cause_opts::other_fail;

  return msg;
}

byte_buffer srsran::pack_ul_ccch_msg(const asn1::rrc_nr::ul_ccch_msg_s& msg)
{
  byte_buffer   pdu;
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_SUCCESS) {
    return pdu;
  }
  return byte_buffer{};
}
