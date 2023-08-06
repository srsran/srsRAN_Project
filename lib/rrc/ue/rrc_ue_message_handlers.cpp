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

#include "../../ran/gnb_format.h"
#include "procedures/rrc_reestablishment_procedure.h"
#include "procedures/rrc_setup_procedure.h"
#include "procedures/rrc_ue_capability_transfer_procedure.h"
#include "rrc_asn1_helpers.h"
#include "rrc_ue_impl.h"
#include "ue/rrc_measurement_types_asn1_converters.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/ran/lcid.h"

using namespace srsran;
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
      log_rx_pdu_fail(context.ue_index, "CCCH UL", pdu.view(), "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "ue={}", context.ue_index);
  fmt::format_to(fmtbuf2, "CCCH UL {}", ul_ccch_msg.msg.c1().type().to_string());
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
      log_rx_pdu_fail(context.ue_index, "CCCH UL", pdu.view(), "Unsupported message type");
      // TODO Remove user
  }
}

void rrc_ue_impl::handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& request_msg)
{
  // Perform various checks to make sure we can serve the RRC Setup Request
  if (reject_users) {
    logger.error("RRC connections not allowed. Sending Connection Reject");
    send_rrc_reject(rrc_reject_max_wait_time_s);
    on_ue_delete_request(cause_t::radio_network);
    return;
  }

  // Extract the setup ID and cause
  const rrc_setup_request_ies_s& request_ies = request_msg.rrc_setup_request;
  switch (request_ies.ue_id.type().value) {
    case init_ue_id_c::types_opts::ng_5_g_s_tmsi_part1: {
      context.setup_ue_id = request_ies.ue_id.ng_5_g_s_tmsi_part1().to_number();

      // As per TS 23.003 section 2.10.1 the last 32Bits of the 5G-S-TMSI are the 5G-TMSI
      unsigned shift_bits =
          request_ies.ue_id.ng_5_g_s_tmsi_part1().length() - 32; // calculate the number of bits to shift
      context.five_g_tmsi = ((request_ies.ue_id.ng_5_g_s_tmsi_part1().to_number() << shift_bits) >> shift_bits);

      break;
    }
    case asn1::rrc_nr::init_ue_id_c::types_opts::random_value:
      context.setup_ue_id = request_ies.ue_id.random_value().to_number();
      // TODO: communicate with NGAP
      break;
    default:
      logger.error("Unsupported RRCSetupRequest");
      send_rrc_reject(rrc_reject_max_wait_time_s);
      on_ue_delete_request(cause_t::protocol);
      return;
  }
  context.connection_cause.value = request_ies.establishment_cause.value;

  // Launch RRC setup procedure
  task_sched.schedule_async_task(launch_async<rrc_setup_procedure>(context,
                                                                   request_ies.establishment_cause.value,
                                                                   du_to_cu_container,
                                                                   *this,
                                                                   du_processor_notifier,
                                                                   nas_notifier,
                                                                   *event_mng,
                                                                   logger));
}

void rrc_ue_impl::handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg)
{
  task_sched.schedule_async_task(launch_async<rrc_reestablishment_procedure>(msg,
                                                                             context,
                                                                             du_to_cu_container,
                                                                             up_resource_mng,
                                                                             *this,
                                                                             *this,
                                                                             du_processor_notifier,
                                                                             cu_cp_notifier,
                                                                             ngap_ctrl_notifier,
                                                                             nas_notifier,
                                                                             *event_mng,
                                                                             logger));
}

void rrc_ue_impl::handle_ul_dcch_pdu(byte_buffer_slice pdu)
{
  // Parse UL-CCCH
  ul_dcch_msg_s ul_dcch_msg;
  {
    asn1::cbit_ref bref({pdu.begin(), pdu.end()});
    if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(context.ue_index, "DCCH UL", pdu.view(), "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "ue={} SRB1", context.ue_index);
  fmt::format_to(fmtbuf2, "DCCH UL {}", ul_dcch_msg.msg.c1().type().to_string());
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
    case ul_dcch_msg_type_c::c1_c_::types_opts::ue_cap_info:
      handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().ue_cap_info().rrc_transaction_id);
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_recfg_complete:
      handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().rrc_recfg_complete().rrc_transaction_id);
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_reest_complete:
      handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().rrc_reest_complete().rrc_transaction_id);
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::meas_report:
      handle_measurement_report(ul_dcch_msg.msg.c1().meas_report());
      break;
    default:
      log_rx_pdu_fail(context.ue_index, "DCCH UL", pdu.view(), "Unsupported message type");
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

void rrc_ue_impl::handle_measurement_report(const asn1::rrc_nr::meas_report_s& msg)
{
  // convert asn1 to common type
  rrc_meas_results meas_results = asn1_to_measurement_results(msg.crit_exts.meas_report().meas_results);
  // send measurement results to cell measurement manager
  cell_meas_mng.report_measurement(context.ue_index, meas_results);
}

void rrc_ue_impl::handle_dl_nas_transport_message(const dl_nas_transport_message& msg)
{
  logger.debug("Received DlNasTransportMessage ({} B)", msg.nas_pdu.length());

  dl_dcch_msg_s           dl_dcch_msg;
  dl_info_transfer_ies_s& dl_info_transfer =
      dl_dcch_msg.msg.set_c1().set_dl_info_transfer().crit_exts.set_dl_info_transfer();
  dl_info_transfer.ded_nas_msg.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), dl_info_transfer.ded_nas_msg.begin());

  if (srbs[srb_id_to_uint(srb_id_t::srb2)].pdu_notifier != nullptr) {
    send_dl_dcch(srb_id_t::srb2, dl_dcch_msg);
  } else {
    send_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
  }
}

void rrc_ue_impl::handle_rrc_transaction_complete(const ul_dcch_msg_s& msg, uint8_t transaction_id_)
{
  expected<uint8_t> transaction_id = transaction_id_;

  // Set transaction result and resume suspended procedure.
  if (not event_mng->transactions.set(transaction_id.value(), msg)) {
    logger.warning("Unexpected transaction id={}", transaction_id.value());
  }
}

async_task<bool> rrc_ue_impl::handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg)
{
  return launch_async<rrc_reconfiguration_procedure>(context, msg, *this, *event_mng, du_processor_notifier, logger);
}

uint8_t rrc_ue_impl::handle_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg)
{
  // Create transaction to get transaction ID
  rrc_transaction transaction    = event_mng->transactions.create_transaction();
  unsigned        transaction_id = transaction.id();
  // abort transaction (we will receive the RRC Reconfiguration Complete on the target UE)
  if (not event_mng->transactions.set(transaction_id, RRC_PROC_TIMEOUT)) {
    logger.warning("Unexpected transaction id={}", transaction_id);
  }

  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_recfg();
  rrc_recfg_s& rrc_reconfig = dl_dcch_msg.msg.c1().rrc_recfg();
  fill_asn1_rrc_reconfiguration_msg(rrc_reconfig, transaction_id, msg);
  on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);

  return transaction_id;
}

async_task<bool> rrc_ue_impl::handle_handover_reconfiguration_complete_expected(uint8_t transaction_id)
{
  const std::chrono::milliseconds timeout_ms{
      1000}; // arbitrary timeout for RRC Reconfig procedure, UE will be removed if timer fires

  rrc_transaction transaction;

  // Schedule CU-UP removal task
  return launch_async([this, timeout_ms, transaction_id, &transaction](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    logger.debug("ue={} Awaiting RRC Reconfiguration Complete.", context.ue_index);
    // create new transaction for RRC Reconfiguration procedure
    transaction = event_mng->transactions.create_transaction(transaction_id, timeout_ms);

    CORO_AWAIT(transaction);

    auto coro_res         = transaction.result();
    bool procedure_result = false;
    if (coro_res.has_value()) {
      logger.debug("ue={} Received RRC Reconfiguration Complete.", context.ue_index);
      procedure_result = true;
    } else {
      logger.debug("ue={} Did not receive RRC Reconfiguration Complete - timed out.", context.ue_index);
      this->on_ue_delete_request(cause_t::protocol); // delete UE context if reconfig fails
    }

    CORO_RETURN(procedure_result);
  });
}

async_task<bool> rrc_ue_impl::handle_rrc_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg)
{
  //  Launch RRC UE capability transfer procedure
  return launch_async<rrc_ue_capability_transfer_procedure>(context, *this, *event_mng, logger);
}

rrc_ue_release_context rrc_ue_impl::get_rrc_ue_release_context()
{
  // prepare location info to return
  rrc_ue_release_context release_context;
  release_context.user_location_info.nr_cgi      = context.cell.cgi;
  release_context.user_location_info.tai.plmn_id = context.cell.cgi.plmn_hex;
  release_context.user_location_info.tai.tac     = context.cell.tac;

  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_release().crit_exts.set_rrc_release();

  // pack DL CCCH msg
  release_context.rrc_release_pdu = pack_into_pdu(dl_dcch_msg);
  release_context.srb_id          = srb_id_t::srb1;

  return release_context;
}

optional<rrc_meas_cfg> rrc_ue_impl::get_rrc_ue_meas_config()
{
  return cell_meas_mng.get_measurement_config(context.cell.cgi.nci);
}

rrc_reestablishment_ue_context_t rrc_ue_impl::get_context()
{
  rrc_reestablishment_ue_context_t rrc_reest_context;
  rrc_reest_context.sec_context = context.sec_context;

  if (context.capabilities.has_value()) {
    rrc_reest_context.capabilities = context.capabilities.value();
  }
  rrc_reest_context.up_ctx = up_resource_mng.get_up_context();

  return rrc_reest_context;
}
