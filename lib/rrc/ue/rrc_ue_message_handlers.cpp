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
#include "rrc_ue_helpers.h"
#include "rrc_ue_impl.h"
#include "ue/rrc_measurement_types_asn1_converters.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/ran/lcid.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

void rrc_ue_impl::handle_ul_ccch_pdu(byte_buffer pdu)
{
  // Parse UL-CCCH
  ul_ccch_msg_s ul_ccch_msg;
  {
    asn1::cbit_ref bref(pdu);
    if (ul_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_ccch_msg.msg.type().value != ul_ccch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(context.ue_index, "CCCH UL", pdu, "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "ue={}", context.ue_index);
  fmt::format_to(fmtbuf2, "CCCH UL {}", ul_ccch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Rx, pdu, ul_ccch_msg, to_c_str(fmtbuf2));

  // Handle message
  switch (ul_ccch_msg.msg.c1().type().value) {
    case ul_ccch_msg_type_c::c1_c_::types_opts::rrc_setup_request:
      handle_rrc_setup_request(ul_ccch_msg.msg.c1().rrc_setup_request());
      break;
    case ul_ccch_msg_type_c::c1_c_::types_opts::rrc_reest_request:
      handle_rrc_reest_request(ul_ccch_msg.msg.c1().rrc_reest_request());
      break;
    default:
      log_rx_pdu_fail(context.ue_index, "CCCH UL", pdu, "Unsupported message type");
      // TODO Remove user
  }
}

void rrc_ue_impl::handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& request_msg)
{
  // Perform various checks to make sure we can serve the RRC Setup Request
  if (reject_users) {
    logger.error("RRC connections not allowed. Sending Connection Reject");
    send_rrc_reject(rrc_reject_max_wait_time_s);
    on_ue_release_required(cause_radio_network_t::unspecified);
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
      on_ue_release_required(cause_protocol_t::unspecified);
      return;
  }
  context.connection_cause.value = request_ies.establishment_cause.value;

  // Launch RRC setup procedure
  task_sched.schedule_async_task(launch_async<rrc_setup_procedure>(context,
                                                                   request_ies.establishment_cause.value,
                                                                   du_to_cu_container,
                                                                   *this,
                                                                   get_rrc_ue_srb_handler(),
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
                                                                             get_rrc_ue_srb_handler(),
                                                                             du_processor_notifier,
                                                                             cu_cp_notifier,
                                                                             ngap_ctrl_notifier,
                                                                             nas_notifier,
                                                                             *event_mng,
                                                                             logger));
}

void rrc_ue_impl::handle_pdu(const srb_id_t srb_id, byte_buffer rrc_pdu)
{
  // Parse UL-DCCH
  ul_dcch_msg_s ul_dcch_msg;
  {
    asn1::cbit_ref bref(rrc_pdu);
    if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
      log_rx_pdu_fail(context.ue_index, "DCCH UL", rrc_pdu, "Failed to unpack message", true);
      return;
    }
  }

  // Log Rx message
  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "ue={} {}", context.ue_index, srb_id);
  fmt::format_to(fmtbuf2, "DCCH UL {}", ul_dcch_msg.msg.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Rx, rrc_pdu, ul_dcch_msg, to_c_str(fmtbuf2));

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
      if (context.is_inter_cu_handover) {
        logger.debug("ue={} Received a RRC Reconfiguration Complete during inter CU handover - notifying NGAP.",
                     context.ue_index);
        ngap_ctrl_notifier.on_inter_cu_ho_rrc_recfg_complete_received(
            context.ue_index, context.cell.cgi, context.cell.tac);
        context.is_inter_cu_handover = false;
      } else {
        handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().rrc_recfg_complete().rrc_transaction_id);
      }
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::rrc_reest_complete:
      handle_rrc_transaction_complete(ul_dcch_msg, ul_dcch_msg.msg.c1().rrc_reest_complete().rrc_transaction_id);
      break;
    case ul_dcch_msg_type_c::c1_c_::types_opts::meas_report:
      handle_measurement_report(ul_dcch_msg.msg.c1().meas_report());
      break;
    default:
      log_rx_pdu_fail(context.ue_index, "DCCH UL", rrc_pdu, "Unsupported message type");
      break;
  }
  // TODO: Handle message
}

void rrc_ue_impl::handle_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdcp_pdu)
{
  logger.debug(
      pdcp_pdu.begin(), pdcp_pdu.end(), "ue={} C-RNTI={} RX {} PDCP PDU", context.ue_index, context.c_rnti, srb_id);

  if (context.srbs.find(srb_id) == context.srbs.end()) {
    logger.error(pdcp_pdu.begin(),
                 pdcp_pdu.end(),
                 "ue={} C-RNTI={} RX {} is not set up. Dropping PDU.",
                 context.ue_index,
                 context.c_rnti,
                 srb_id);
    return;
  }

  // Unpack PDCP PDU
  byte_buffer rrc_pdu = context.srbs.at(srb_id).unpack_pdcp_pdu(std::move(pdcp_pdu));
  if (rrc_pdu.empty()) {
    logger.warning("ue={} C-RNTI={} RX {} Dropping PDU.", context.ue_index, context.c_rnti, srb_id);
    logger.debug("original len={}, new_len={}", pdcp_pdu.length(), rrc_pdu.length());
    return;
  }

  logger.debug(rrc_pdu.begin(),
               rrc_pdu.end(),
               "ue={} C-RNTI={} RX {} RRC PDU ({} B)",
               context.ue_index,
               context.c_rnti,
               srb_id,
               rrc_pdu.length());
  handle_pdu(srb_id, std::move(rrc_pdu));
}

void rrc_ue_impl::handle_ul_info_transfer(const ul_info_transfer_ies_s& ul_info_transfer)
{
  cu_cp_ul_nas_transport ul_nas_msg         = {};
  ul_nas_msg.ue_index                       = context.ue_index;
  ul_nas_msg.nas_pdu                        = ul_info_transfer.ded_nas_msg.copy();
  ul_nas_msg.user_location_info.nr_cgi      = context.cell.cgi;
  ul_nas_msg.user_location_info.tai.plmn_id = context.cell.cgi.plmn_hex;
  ul_nas_msg.user_location_info.tai.tac     = context.cell.tac;

  nas_notifier.on_ul_nas_transport_message(ul_nas_msg);
}

void rrc_ue_impl::handle_measurement_report(const asn1::rrc_nr::meas_report_s& msg)
{
  // convert asn1 to common type
  rrc_meas_results meas_results = asn1_to_measurement_results(msg.crit_exts.meas_report().meas_results);
  // send measurement results to cell measurement manager
  cell_meas_mng.report_measurement(context.ue_index, meas_results);
}

void rrc_ue_impl::handle_dl_nas_transport_message(byte_buffer nas_pdu)
{
  logger.debug("Received DlNasTransportMessage ({} B)", nas_pdu.length());

  dl_dcch_msg_s           dl_dcch_msg;
  dl_info_transfer_ies_s& dl_info_transfer =
      dl_dcch_msg.msg.set_c1().set_dl_info_transfer().crit_exts.set_dl_info_transfer();
  dl_info_transfer.ded_nas_msg = nas_pdu.copy();

  if (context.srbs.find(srb_id_t::srb2) != context.srbs.end()) {
    send_dl_dcch(srb_id_t::srb2, dl_dcch_msg);
  } else {
    send_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
  }
}

void rrc_ue_impl::handle_rrc_transaction_complete(const ul_dcch_msg_s& msg, uint8_t transaction_id_)
{
  expected<uint8_t> transaction_id = transaction_id_;

  // Set transaction result and resume suspended procedure.
  if (not event_mng->transactions.set_response(transaction_id.value(), msg)) {
    logger.warning("ue={} Unexpected RRC transaction id={}", context.ue_index, transaction_id.value());
  }
}

async_task<bool> rrc_ue_impl::handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg)
{
  return launch_async<rrc_reconfiguration_procedure>(context, msg, *this, *event_mng, get_rrc_ue_srb_handler(), logger);
}

uint8_t rrc_ue_impl::handle_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg)
{
  // Create transaction to get transaction ID
  rrc_transaction transaction    = event_mng->transactions.create_transaction();
  unsigned        transaction_id = transaction.id();

  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_recfg();
  rrc_recfg_s& rrc_reconfig = dl_dcch_msg.msg.c1().rrc_recfg();
  fill_asn1_rrc_reconfiguration_msg(rrc_reconfig, transaction_id, msg);
  on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);

  return transaction_id;
}

async_task<bool> rrc_ue_impl::handle_handover_reconfiguration_complete_expected(uint8_t transaction_id)
{
  // arbitrary timeout for RRC Reconfig procedure, UE will be removed if timer fires
  const std::chrono::milliseconds timeout_ms{1000};

  return launch_async(
      [this, timeout_ms, transaction_id, transaction = rrc_transaction{}](coro_context<async_task<bool>>& ctx) mutable {
        CORO_BEGIN(ctx);

        logger.debug("ue={} Awaiting RRC Reconfiguration Complete.", context.ue_index);
        // create new transaction for RRC Reconfiguration procedure
        transaction = event_mng->transactions.create_transaction(transaction_id, timeout_ms);

        CORO_AWAIT(transaction);

        bool procedure_result = false;
        if (transaction.has_response()) {
          logger.debug("ue={} Received RRC Reconfiguration Complete after HO.", context.ue_index);
          procedure_result = true;
        } else {
          logger.debug("ue={} Did not receive RRC Reconfiguration Complete after HO - timed out.", context.ue_index);
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
  if (context.srbs.find(srb_id_t::srb1) == context.srbs.end()) {
    logger.error("ue={} C-RNTI={} RX {} is not set up, can't create RRC Release PDU.",
                 context.ue_index,
                 context.c_rnti,
                 srb_id_t::srb1);
  } else {
    release_context.rrc_release_pdu = context.srbs.at(srb_id_t::srb1).pack_rrc_pdu(pack_into_pdu(dl_dcch_msg));
    release_context.srb_id          = srb_id_t::srb1;
  }

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

bool rrc_ue_impl::handle_new_security_context(const security::security_context& sec_context)
{
  // Copy security context to RRC UE context
  context.sec_context = sec_context;

  // Select preferred integrity algorithm.
  security::preferred_integrity_algorithms inc_algo_pref_list  = {security::integrity_algorithm::nia2,
                                                                  security::integrity_algorithm::nia1,
                                                                  security::integrity_algorithm::nia3,
                                                                  security::integrity_algorithm::nia0};
  security::preferred_ciphering_algorithms ciph_algo_pref_list = {security::ciphering_algorithm::nea0,
                                                                  security::ciphering_algorithm::nea2,
                                                                  security::ciphering_algorithm::nea1,
                                                                  security::ciphering_algorithm::nea3};
  if (not context.sec_context.select_algorithms(inc_algo_pref_list, ciph_algo_pref_list)) {
    logger.error("ue={} could not select security algorithm", context.ue_index);
    return false;
  }
  logger.debug("ue={} selected security algorithms NIA=NIA{} NEA=NEA{}",
               context.ue_index,
               context.sec_context.sel_algos.integ_algo,
               context.sec_context.sel_algos.cipher_algo);

  // Generate K_rrc_enc and K_rrc_int
  context.sec_context.generate_as_keys();

  // activate SRB1 PDCP security
  on_new_as_security_context();

  return true;
}

byte_buffer rrc_ue_impl::get_rrc_handover_command(const rrc_reconfiguration_procedure_request& request,
                                                  unsigned                                     transaction_id)
{
  // pack RRC Reconfig
  rrc_recfg_s rrc_reconfig;
  fill_asn1_rrc_reconfiguration_msg(rrc_reconfig, transaction_id, request);
  byte_buffer reconfig_pdu = pack_into_pdu(rrc_reconfig);

  ho_cmd_s ho_cmd;
  ho_cmd.crit_exts.set_c1().set_ho_cmd().ho_cmd_msg = reconfig_pdu.copy();

  // pack Handover Command
  byte_buffer ho_cmd_pdu = pack_into_pdu(ho_cmd);

  fmt::memory_buffer fmtbuf, fmtbuf2;
  fmt::format_to(fmtbuf, "ue={}", context.ue_index);
  fmt::format_to(fmtbuf2, "{}", ho_cmd.crit_exts.c1().type().to_string());
  log_rrc_message(to_c_str(fmtbuf), Tx, ho_cmd_pdu, ho_cmd, to_c_str(fmtbuf2));

  return ho_cmd_pdu;
}

bool rrc_ue_impl::handle_rrc_handover_command(byte_buffer cmd)
{
  // Unpack HGandover Command
  asn1::rrc_nr::ho_cmd_s handover_command;
  asn1::cbit_ref         bref({cmd.begin(), cmd.end()});

  if (handover_command.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Couldn't unpack Handover Command RRC container.");
    return false;
  }

  // Unpack RRC Reconfiguration to new DL DCCH Message
  asn1::cbit_ref bref2({handover_command.crit_exts.c1().ho_cmd().ho_cmd_msg.begin(),
                        handover_command.crit_exts.c1().ho_cmd().ho_cmd_msg.end()});

  dl_dcch_msg_s dl_dcch_msg;
  auto&         rrc_recfg = dl_dcch_msg.msg.set_c1().set_rrc_recfg();

  if (rrc_recfg.unpack(bref2) != asn1::SRSASN_SUCCESS) {
    logger.error("Couldn't unpack RRC Reconfiguration container.");
    return false;
  }

  // Send to UE
  on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);

  return true;
}