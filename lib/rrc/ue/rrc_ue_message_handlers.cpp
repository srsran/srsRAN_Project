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
      log_rx_pdu_fail(c_rnti, "UL-CCCH", pdu.view(), "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}, ", c_rnti);
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
      log_rx_pdu_fail(c_rnti, "UL-CCCH", pdu.view(), "Unsupported message type");
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
    cfg.logger.warning("Could not allocate PUCCH resources for rnti=0x{}. Sending Connection Reject", c_rnti);
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

  // create SRB1
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index = ue_index;
  srb1_msg.srb_id   = srb_id_t::srb1;
  srb1_msg.pdcp_cfg = cfg.srb1_pdcp_cfg;
  du_processor_notifier.on_create_srb(srb1_msg);

  send_rrc_setup();
}

void rrc_ue_entity::handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg)
{
  // TODO: handle RRC reestablishment request
}

void rrc_ue_entity::handle_ul_dcch_pdu(byte_buffer_slice pdu)
{
  // Parse UL-CCCH
  ul_dcch_msg_s ul_dcch_msg;
  {
    asn1::cbit_ref bref({pdu.begin(), pdu.end()});
    if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(c_rnti, "UL-DCCH", pdu.view(), "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}, SRB1", c_rnti);
  fmt::format_to(fmtbuf2, "UL-DCCH.{}", ul_dcch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Rx, pdu.view(), ul_dcch_msg, to_c_str(fmtbuf2));

  switch (ul_dcch_msg.msg.c1().type()) {
    case ul_dcch_msg_type_c::c1_c_::types_opts::options::ul_info_transfer: {
      handle_ul_info_transfer(ul_dcch_msg.msg.c1().ul_info_transfer().crit_exts.ul_info_transfer());
    } break;

    default:
      cfg.logger.error("Unsupported UL DCCH Message type: {}", ul_dcch_msg.msg.c1().type().to_string());
      break;
  }
  // TODO: Handle message
}

void rrc_ue_entity::handle_ul_info_transfer(const ul_info_transfer_ies_s& ul_info_transfer)
{
  ul_nas_transport_message ul_nas_msg = {};
  ul_nas_msg.ded_nas_msg.resize(ul_info_transfer.ded_nas_msg.size());
  ul_nas_msg.ded_nas_msg = ul_info_transfer.ded_nas_msg;
  // TODO: add cgi

  ngap_notifier.on_ul_nas_transport_message(ul_nas_msg);
}

void rrc_ue_entity::handle_dl_nas_transport_message(const dl_nas_transport_message& msg)
{
  cfg.logger.info("Received DL NAS Transport message");

  dl_dcch_msg_s           dl_dcch_msg;
  dl_info_transfer_ies_s& dl_info_transfer =
      dl_dcch_msg.msg.set_c1().set_dl_info_transfer().crit_exts.set_dl_info_transfer();
  dl_info_transfer.ded_nas_msg.resize(msg.ded_nas_msg.size());
  dl_info_transfer.ded_nas_msg = msg.ded_nas_msg;

  send_dl_dcch(dl_dcch_msg);
}
