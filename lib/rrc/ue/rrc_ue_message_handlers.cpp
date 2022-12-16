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
#include "procedures/rrc_setup_procedure.h"
#include "routines/rrc_pdu_session_resource_setup_routine.h"
#include "rrc_asn1_helpers.h"
#include "rrc_ue_impl.h"

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

void rrc_ue_impl::handle_ul_ccch_pdu(byte_buffer_slice pdu)
{
  // Parse UL-CCCH
  ul_ccch_msg_s ul_ccch_msg;
  {
    asn1::cbit_ref bref({pdu.begin(), pdu.end()});
    if (ul_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_ccch_msg.msg.type().value != ul_ccch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(context.c_rnti, "UL-CCCH", pdu.view(), "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}", context.c_rnti);
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
      log_rx_pdu_fail(context.c_rnti, "UL-CCCH", pdu.view(), "Unsupported message type");
      // TODO Remove user
  }
}

void rrc_ue_impl::handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& request_msg)
{
  // 1. Perform various checks to make sure we can serve the RRC Setup Request
  if (not rrc_du.is_rrc_connect_allowed()) {
    logger.error("RRC connections not allowed. Sending Connection Reject");
    send_rrc_reject(rrc_reject_max_wait_time_s);
    on_ue_delete_request();
    return;
  }

  // 1.1 Check and allocate PUCCH resources
  if (not rrc_du.get_pucch_resources()) {
    logger.warning("Could not allocate PUCCH resources for rnti=0x{}. Sending Connection Reject", context.c_rnti);
    send_rrc_reject(rrc_reject_max_wait_time_s);
    on_ue_delete_request();
    return;
  }

  // 1.2 Extract the setup ID and cause
  const rrc_setup_request_ies_s& request_ies = request_msg.rrc_setup_request;
  switch (request_ies.ue_id.type().value) {
    case init_ue_id_c::types_opts::ng_minus5_g_s_tmsi_part1:
      context.setup_ue_id = request_ies.ue_id.ng_minus5_g_s_tmsi_part1().to_number();
      break;
    case asn1::rrc_nr::init_ue_id_c::types_opts::random_value:
      context.setup_ue_id = request_ies.ue_id.random_value().to_number();
      // TODO: communicate with NGC
      break;
    default:
      logger.error("Unsupported RRCSetupRequest");
      send_rrc_reject(rrc_reject_max_wait_time_s);
      on_ue_delete_request();
      return;
  }
  context.connection_cause.value = request_ies.establishment_cause.value;

  // Launch RRC setup procedure
  task_sched.schedule_async_task(launch_async<rrc_setup_procedure>(
      context, request_msg, du_to_cu_container, *this, du_processor_notifier, nas_notifier, *event_mng, logger));
}

void rrc_ue_impl::handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg)
{
  // TODO: handle RRC reestablishment request
}

void rrc_ue_impl::handle_ul_dcch_pdu(byte_buffer_slice pdu)
{
  // Parse UL-CCCH
  ul_dcch_msg_s ul_dcch_msg;
  {
    asn1::cbit_ref bref({pdu.begin(), pdu.end()});
    if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(context.c_rnti, "UL-DCCH", pdu.view(), "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "rnti=0x{:x}, SRB1", context.c_rnti);
  fmt::format_to(fmtbuf2, "UL-DCCH.{}", ul_dcch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Rx, pdu.view(), ul_dcch_msg, to_c_str(fmtbuf2));

  switch (ul_dcch_msg.msg.c1().type().value) {
    case ul_dcch_msg_type_c::c1_c_::types_opts::options::ul_info_transfer:
      handle_ul_info_transfer(ul_dcch_msg.msg.c1().ul_info_transfer().crit_exts.ul_info_transfer());
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_setup_complete:
      handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().rrc_setup_complete().rrc_transaction_id);
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::security_mode_complete:
      handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().security_mode_complete().rrc_transaction_id);
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg_complete:
      handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().rrc_recfg_complete().rrc_transaction_id);
      break;
    default:
      log_rx_pdu_fail(context.c_rnti, "UL-DCCH", pdu.view(), "Unsupported message type");
      break;
  }
  // TODO: Handle message
}

void rrc_ue_impl::handle_ul_info_transfer(const ul_info_transfer_ies_s& ul_info_transfer)
{
  ul_nas_transport_message ul_nas_msg = {};
  ul_nas_msg.ue_index                 = context.ue_index;
  ul_nas_msg.cell                     = context.cell;

  ul_nas_msg.nas_pdu.resize(ul_info_transfer.ded_nas_msg.size());
  std::copy(ul_info_transfer.ded_nas_msg.begin(), ul_info_transfer.ded_nas_msg.end(), ul_nas_msg.nas_pdu.begin());

  nas_notifier.on_ul_nas_transport_message(ul_nas_msg);
}

void rrc_ue_impl::handle_dl_nas_transport_message(const dl_nas_transport_message& msg)
{
  logger.info("Received DL NAS Transport message");

  dl_dcch_msg_s           dl_dcch_msg;
  dl_info_transfer_ies_s& dl_info_transfer =
      dl_dcch_msg.msg.set_c1().set_dl_info_transfer().crit_exts.set_dl_info_transfer();
  dl_info_transfer.ded_nas_msg.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), dl_info_transfer.ded_nas_msg.begin());

  send_dl_dcch(dl_dcch_msg);
}

void rrc_ue_impl::handle_rrc_transaction_complete(const ul_dcch_msg_s& msg, uint8_t transaction_id_)
{
  expected<uint8_t> transaction_id = transaction_id_;

  // Set transaction result and resume suspended procedure.
  event_mng->transactions.set(transaction_id.value(), msg);
}

void rrc_ue_impl::handle_new_guami(const guami& msg)
{
  context.current_guami = msg;
}

async_task<cu_cp_pdu_session_resource_setup_response_message>
rrc_ue_impl::handle_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_message& msg)
{
  return launch_async<rrc_pdu_session_resource_setup_routine>(msg, context, *this, *event_mng, logger);
}