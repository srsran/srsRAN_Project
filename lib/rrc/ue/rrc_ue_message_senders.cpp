/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rrc_asn1_helpers.h"
#include "rrc_ue_entity.h"

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

void rrc_ue_entity::send_rrc_setup()
{
  cfg.logger.info("Sending RRC Setup");

  // fill rrc setup
  asn1::rrc_nr::dl_ccch_msg_s dl_ccch_msg = {};
  dl_ccch_msg.msg.set_c1().set_rrc_setup();
  asn1::rrc_nr::rrc_setup_s& rrc_setup = dl_ccch_msg.msg.c1().rrc_setup();
  fill_asn1_rrc_setup_msg(rrc_setup, ctxt.du_to_cu_rrc_container);

  if (cfg.logger.debug.enabled()) {
    asn1::json_writer js;
    rrc_setup.to_json(js);
    cfg.logger.debug("Containerized RRCSetup: {}", js.to_string());
  }

  // pack DL CCCH msg
  byte_buffer pdu = pack_into_pdu(dl_ccch_msg);

  // send down the stack
  send_srb0_pdu(std::move(pdu));
}

void rrc_ue_entity::send_srb0_pdu(byte_buffer pdu)
{
  if (ctxt.srbs.contains(LCID_SRB0)) {
    ctxt.srbs[LCID_SRB0].tx_notifier->on_new_pdu(shared_byte_buffer_view{pdu});
  } else {
    cfg.logger.error("Can't send RRC PDU - no SRB0 configured");
  }
}