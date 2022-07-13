/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../ran/gnb_format.h"
#include "rrc_asn1_helpers.h"
#include "rrc_ue_entity.h"

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

void rrc_ue_entity::send_rrc_setup()
{
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_setup();
  rrc_setup_s& rrc_setup = dl_ccch_msg.msg.c1().rrc_setup();
  fill_asn1_rrc_setup_msg(rrc_setup, ctxt.du_to_cu_rrc_container);

  send_dl_ccch(dl_ccch_msg);
}

void rrc_ue_entity::send_dl_ccch(const dl_ccch_msg_s& dl_ccch_msg)
{
  // pack DL CCCH msg
  byte_buffer pdu = pack_into_pdu(dl_ccch_msg);

  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}, SRB0", ctxt.c_rnti);
  fmt::format_to(fmtbuf2, "DL-CCCH.{}", dl_ccch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Tx, pdu, dl_ccch_msg, to_c_str(fmtbuf2));

  // send down the stack
  send_srb0_pdu(std::move(pdu));
}

/// TS 38.331, RRCReject message
void rrc_ue_entity::send_rrc_reject(uint8_t reject_wait_time_secs)
{
  dl_ccch_msg_s     dl_ccch_msg;
  rrc_reject_ies_s& reject = dl_ccch_msg.msg.set_c1().set_rrc_reject().crit_exts.set_rrc_reject();

  // See TS 38.331, RejectWaitTime
  if (reject_wait_time_secs > 0) {
    reject.wait_time_present = true;
    reject.wait_time         = reject_wait_time_secs;
  }

  send_dl_ccch(dl_ccch_msg);

  // TODO: remove user?
}

void rrc_ue_entity::send_srb0_pdu(byte_buffer pdu)
{
  if (ctxt.srbs.contains(LCID_SRB0)) {
    ctxt.srbs[LCID_SRB0].tx_notifier->on_new_pdu(byte_buffer_slice{pdu});
  } else {
    cfg.logger.error("Can't send RRC PDU - no SRB0 configured");
  }
}