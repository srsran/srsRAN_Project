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
#include "rrc_ue_entity.h"

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

void rrc_ue_entity::handle_ul_ccch_pdu(byte_buffer_slice pdu)
{
  // Parse UL-CCCH
  ul_ccch_msg_s ul_ccch_msg;
  {
    asn1::cbit_ref bref({pdu.begin(), pdu.end()});
    if (ul_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_ccch_msg.msg.type().value != ul_ccch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(ctxt.c_rnti, LCID_SRB0, pdu.view(), "Failed to unpack UL-CCCH message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}, SRB0", ctxt.c_rnti);
  fmt::format_to(fmtbuf2, "UL-CCCH.{}", ul_ccch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Rx, pdu.view(), ul_ccch_msg, to_c_str(fmtbuf2));

  // Handle message
  switch (ul_ccch_msg.msg.c1().type().value) {
    case ul_ccch_msg_type_c::c1_c_::types_opts::rrc_setup_request:
      handle_rrc_setup_request(ul_ccch_msg.msg.c1().rrc_setup_request());
      break;
    case ul_ccch_msg_type_c::c1_c_::types_opts::rrc_reest_request:
      handle_rrc_reest_request(ul_ccch_msg.msg.c1().rrc_reest_request());
      break;
    default:
      log_rx_pdu_fail(ctxt.c_rnti, LCID_SRB0, pdu.view(), "Unsupported UL-CCCH message type");
      // TODO Remove user
  }
}

void rrc_ue_entity::handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg)
{
  const uint8_t max_wait_time_secs = 16;
  if (not parent.is_rrc_connect_allowed()) {
    cfg.logger.error("RRC connections not allowed. Sending Connection Reject");
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  // Allocate PUCCH resources and reject if not available
  if (not init_pucch()) {
    cfg.logger.warning("Could not allocate PUCCH resources for rnti=0x{}. Sending Connection Reject", ctxt.c_rnti);
    send_rrc_reject(max_wait_time_secs);
    return;
  }

  const rrc_setup_request_ies_s& ies = msg.rrc_setup_request;

  switch (ies.ue_id.type().value) {
    case init_ue_id_c::types_opts::ng_minus5_g_s_tmsi_part1:
      rrc_ctxt.setup_ue_id = ies.ue_id.ng_minus5_g_s_tmsi_part1().to_number();
      break;
    case asn1::rrc_nr::init_ue_id_c::types_opts::random_value:
      rrc_ctxt.setup_ue_id = ies.ue_id.random_value().to_number();
      // TODO: communicate with NGAP
      break;
    default:
      cfg.logger.error("Unsupported RRCSetupRequest");
      send_rrc_reject(max_wait_time_secs);
      return;
  }
  rrc_ctxt.connection_cause.value = ies.establishment_cause.value;

  send_rrc_setup();
}

void rrc_ue_entity::handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg)
{
  // TODO: handle RRC reestablishment request
}