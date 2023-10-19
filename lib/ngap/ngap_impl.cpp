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

#include "ngap_impl.h"
#include "ngap_asn1_helpers.h"
#include "ngap_asn1_utils.h"
#include "procedures/ng_setup_procedure.h"
#include "procedures/ngap_dl_nas_message_transfer_procedure.h"
#include "procedures/ngap_handover_preparation_procedure.h"
#include "procedures/ngap_handover_resource_allocation_procedure.h"
#include "procedures/ngap_initial_context_setup_procedure.h"
#include "procedures/ngap_pdu_session_resource_modify_procedure.h"
#include "procedures/ngap_pdu_session_resource_release_procedure.h"
#include "procedures/ngap_pdu_session_resource_setup_procedure.h"
#include "procedures/ngap_ue_context_release_procedure.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cause.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace asn1::ngap;
using namespace srs_cu_cp;

ngap_impl::ngap_impl(ngap_configuration&                ngap_cfg_,
                     ngap_cu_cp_du_repository_notifier& cu_cp_du_repository_notifier_,
                     ngap_ue_task_scheduler&            task_sched_,
                     ngap_ue_manager&                   ue_manager_,
                     ngap_message_notifier&             ngap_notifier_,
                     task_executor&                     ctrl_exec_) :
  logger(srslog::fetch_basic_logger("NGAP")),
  ue_ctxt_list(logger),
  cu_cp_du_repository_notifier(cu_cp_du_repository_notifier_),
  task_sched(task_sched_),
  ue_manager(ue_manager_),
  ngap_notifier(ngap_notifier_),
  ctrl_exec(ctrl_exec_),
  ev_mng(timer_factory{task_sched.get_timer_manager(), ctrl_exec})
{
  context.gnb_id                     = ngap_cfg_.gnb_id;
  context.ran_node_name              = ngap_cfg_.ran_node_name;
  context.plmn                       = ngap_cfg_.plmn;
  context.tac                        = ngap_cfg_.tac;
  context.ue_context_setup_timeout_s = ngap_cfg_.ue_context_setup_timeout_s;
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
ngap_impl::~ngap_impl() {}

void ngap_impl::create_ngap_ue(ue_index_t                          ue_index,
                               ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                               ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                               ngap_du_processor_control_notifier& du_processor_ctrl_notifier)
{
  ran_ue_id_t ran_ue_id = ue_ctxt_list.get_next_ran_ue_id();
  if (ran_ue_id == ran_ue_id_t::invalid) {
    logger.error("ue={}: No RAN-UE-ID available", ue_index);
    return;
  }

  // Create UE context and store it
  ue_ctxt_list.add_ue(ue_index, ran_ue_id, task_sched.get_timer_manager(), ctrl_exec);

  // Add NGAP UE to UE manager
  ngap_ue* ue = ue_manager.add_ue(ue_index, rrc_ue_pdu_notifier, rrc_ue_ctrl_notifier, du_processor_ctrl_notifier);

  if (ue == nullptr) {
    logger.error("ue={}: Failed to create UE", ue_index);
    return;
  }

  logger.debug("ue={} ran_ue_id={}: Created UE", ue_index, ran_ue_id);
}

bool ngap_impl::update_ue_index(ue_index_t new_ue_index, ue_index_t old_ue_index)
{
  if (!ue_ctxt_list.contains(old_ue_index)) {
    logger.warning("Failed to transfer NGAP UE context from ue={} to ue={}. Old UE context does not exist",
                   old_ue_index,
                   new_ue_index);
    return false;
  }

  logger.debug("Transferring NGAP UE context from ue={} to ue={}", old_ue_index, new_ue_index);

  ue_ctxt_list.update_ue_index(new_ue_index, old_ue_index);

  return true;
}

async_task<ng_setup_response> ngap_impl::handle_ng_setup_request(const ng_setup_request& request)
{
  logger.info("Sending NgSetupRequest");
  return launch_async<ng_setup_procedure>(
      context, request, ngap_notifier, ev_mng, timer_factory{task_sched.get_timer_manager(), ctrl_exec}, logger);
}

void ngap_impl::handle_initial_ue_message(const cu_cp_initial_ue_message& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping InitialUeMessage. UE context does not exist", msg.ue_index);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_UE_MSG);

  auto& init_ue_msg           = ngap_msg.pdu.init_msg().value.init_ue_msg();
  init_ue_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);

  fill_asn1_initial_ue_message(init_ue_msg, msg, context);

  // Start UE context setup timer
  ue_ctxt.ue_context_setup_timer.set(context.ue_context_setup_timeout_s, [this, msg](timer_id_t /*tid*/) {
    on_ue_context_setup_timer_expired(msg.ue_index);
  });
  ue_ctxt.ue_context_setup_timer.run();

  logger.info("ue={} ran_ue_id={}: Sending InitialUeMessage (timeout={}s)",
              msg.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_context_setup_timer.duration().count());

  // Forward message to AMF
  ngap_notifier.on_new_message(ngap_msg);
}

void ngap_impl::handle_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping UlNasTransportMessage. UE context does not exist", msg.ue_index);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UL_NAS_TRANSPORT);

  auto& ul_nas_transport_msg = ngap_msg.pdu.init_msg().value.ul_nas_transport();

  ul_nas_transport_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);

  amf_ue_id_t amf_ue_id = ue_ctxt.ue_ids.amf_ue_id;
  if (amf_ue_id == amf_ue_id_t::invalid) {
    logger.warning("ue={}: Dropping UL NAS transport. UE AMF ID not found", msg.ue_index);
    return;
  }
  ul_nas_transport_msg->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);

  fill_asn1_ul_nas_transport(ul_nas_transport_msg, msg);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Sending UlNasTransportMessage",
              msg.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);

  // Forward message to AMF
  ngap_notifier.on_new_message(ngap_msg);
}

void ngap_impl::handle_message(const ngap_message& msg)
{
  logger.debug("Received PDU of type \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    msg.pdu.to_json(js);
    logger.debug("Rx NGAP PDU: {}", js.to_string());
  }

  // Run NGAP protocols in Control executor.
  ctrl_exec.execute([this, msg]() {
    switch (msg.pdu.type().value) {
      case ngap_pdu_c::types_opts::init_msg:
        handle_initiating_message(msg.pdu.init_msg());
        break;
      case ngap_pdu_c::types_opts::successful_outcome:
        handle_successful_outcome(msg.pdu.successful_outcome());
        break;
      case ngap_pdu_c::types_opts::unsuccessful_outcome:
        handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
        break;
      default:
        logger.error("Invalid PDU type");
        break;
    }
  });
}

void ngap_impl::handle_initiating_message(const init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case ngap_elem_procs_o::init_msg_c::types_opts::dl_nas_transport:
      handle_dl_nas_transport_message(msg.value.dl_nas_transport());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::init_context_setup_request:
      handle_initial_context_setup_request(msg.value.init_context_setup_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_setup_request:
      handle_pdu_session_resource_setup_request(msg.value.pdu_session_res_setup_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_modify_request:
      handle_pdu_session_resource_modify_request(msg.value.pdu_session_res_modify_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_release_cmd:
      handle_pdu_session_resource_release_command(msg.value.pdu_session_res_release_cmd());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd:
      handle_ue_context_release_command(msg.value.ue_context_release_cmd());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::paging:
      handle_paging(msg.value.paging());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::ho_request:
      handle_ho_request(msg.value.ho_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::error_ind:
      handle_error_indication(msg.value.error_ind());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngap_impl::handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(msg->ran_ue_ngap_id))) {
    logger.warning("ran_ue_id={} amf_ue_id={}: Dropping DlNasTransportMessage. UE context does not exist",
                   msg->ran_ue_ngap_id,
                   msg->amf_ue_ngap_id);
    send_error_indication(ue_index_t::invalid, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(msg->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    logger.info("ran_ue_id={} amf_ue_id={}: Dropping DlNasTransportMessage. UE is already scheduled for release",
                msg->ran_ue_ngap_id,
                msg->amf_ue_ngap_id);
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  auto* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
  srsran_assert(ue != nullptr,
                "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Received DlNasTransportMessage",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              msg->amf_ue_ngap_id);

  // Add AMF UE ID to ue ngap context if it is not set (this is the first DL NAS Transport message)
  if (ue_ctxt.ue_ids.amf_ue_id == amf_ue_id_t::invalid) {
    // Set AMF UE ID in the UE context and also in the lookup
    ue_ctxt_list.add_amf_ue_id(ue_ctxt.ue_ids.ran_ue_id, uint_to_amf_ue_id(msg->amf_ue_ngap_id));
  }

  // start routine
  task_sched.schedule_async_task(ue_ctxt.ue_ids.ue_index,
                                 launch_async<ngap_dl_nas_message_transfer_procedure>(
                                     msg->nas_pdu.copy(), ue_ctxt.ue_ids, ue->get_rrc_ue_pdu_notifier(), logger));
}

void ngap_impl::handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(request->ran_ue_ngap_id))) {
    logger.warning("ran_ue_id={} amf_ue_id={}: Dropping InitialContextSetupRequest. UE context does not exist",
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    send_error_indication(ue_index_t::invalid, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(request->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    logger.info("ran_ue_id={} amf_ue_id={}: Dropping InitialContextSetup. UE is already scheduled for release",
                request->ran_ue_ngap_id,
                request->amf_ue_ngap_id);
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  auto* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
  srsran_assert(ue != nullptr,
                "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  // Stop UE context setup timer
  ue_ctxt.ue_context_setup_timer.stop();

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Received InitialContextSetupRequest",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              request->amf_ue_ngap_id);

  // Update AMF ID and use the one from this Context Setup as per TS 38.413 v16.2 page 38
  ue_ctxt_list.add_amf_ue_id(ue_ctxt.ue_ids.ran_ue_id, uint_to_amf_ue_id(request->amf_ue_ngap_id));

  // Convert to common type
  ngap_init_context_setup_request init_ctxt_setup_req;
  init_ctxt_setup_req.ue_index = ue_ctxt.ue_ids.ue_index;
  if (!fill_ngap_initial_context_setup_request(init_ctxt_setup_req, request)) {
    logger.error("ue={} ran_ue_id={} amf_ue_id={}: Conversion of PDU Session Resource Setup Request failed.",
                 ue_ctxt.ue_ids.ue_index,
                 ue_ctxt.ue_ids.ran_ue_id,
                 ue_ctxt.ue_ids.amf_ue_id);
    send_error_indication(ue_ctxt.ue_ids.ue_index, {}, ue_ctxt.ue_ids.amf_ue_id);
    return;
  }

  // Store guami
  context.current_guami = init_ctxt_setup_req.guami;

  // Store UE Aggregate Maximum Bitrate
  if (init_ctxt_setup_req.ue_aggr_max_bit_rate.has_value()) {
    ue_ctxt.aggregate_maximum_bit_rate_dl = init_ctxt_setup_req.ue_aggr_max_bit_rate.value().ue_aggr_max_bit_rate_dl;
  }

  // Log security context
  logger.debug(request->security_key.data(), 32, "K_gnb");
  logger.debug("Supported integrity algorithms: {}", init_ctxt_setup_req.security_context.supported_int_algos);
  logger.debug("Supported ciphering algorithms: {}", init_ctxt_setup_req.security_context.supported_enc_algos);

  // start routine
  task_sched.schedule_async_task(
      ue_ctxt.ue_ids.ue_index,
      launch_async<ngap_initial_context_setup_procedure>(init_ctxt_setup_req,
                                                         ue_ctxt,
                                                         ue->get_rrc_ue_control_notifier(),
                                                         ue->get_rrc_ue_pdu_notifier(),
                                                         ue->get_du_processor_control_notifier(),
                                                         ngap_notifier,
                                                         logger));
}

void ngap_impl::handle_pdu_session_resource_setup_request(const asn1::ngap::pdu_session_res_setup_request_s& request)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(request->ran_ue_ngap_id))) {
    logger.warning("ran_ue_id={} amf_ue_id={}: Dropping PduSessionResourceSetupRequest. UE context does not exist",
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    send_error_indication(ue_index_t::invalid, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(request->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    logger.info(
        "ran_ue_id={} amf_ue_id={}: Dropping PduSessionResourceSetupRequest. UE is already scheduled for release",
        request->ran_ue_ngap_id,
        request->amf_ue_ngap_id);
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
  srsran_assert(ue != nullptr,
                "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  if (!ue->get_rrc_ue_control_notifier().on_security_enabled()) {
    logger.warning(
        "ue={} ran_ue_id={} amf_ue_id={}: Dropping PduSessionResourceSetupRequest. Security context does not exist",
        ue_ctxt.ue_ids.ue_index,
        request->ran_ue_ngap_id,
        request->amf_ue_ngap_id);
    send_error_indication(ue_ctxt.ue_ids.ue_index);
    return;
  }

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Received PduSessionResourceSetupRequest",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);

  // Store information in UE context
  if (request->ue_aggr_max_bit_rate_present) {
    ue_ctxt.aggregate_maximum_bit_rate_dl = request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl;
  }

  // Convert to common type
  cu_cp_pdu_session_resource_setup_request msg;
  msg.ue_index     = ue_ctxt.ue_ids.ue_index;
  msg.serving_plmn = context.plmn;
  if (!fill_cu_cp_pdu_session_resource_setup_request(msg, request->pdu_session_res_setup_list_su_req)) {
    logger.error("ue={} ran_ue_id={} amf_ue_id={}: Conversion of PDU Session Resource Setup Request failed",
                 ue_ctxt.ue_ids.ue_index,
                 ue_ctxt.ue_ids.ran_ue_id,
                 ue_ctxt.ue_ids.amf_ue_id);
    send_error_indication(ue_ctxt.ue_ids.ue_index);
    return;
  }
  msg.ue_aggregate_maximum_bit_rate_dl = ue_ctxt.aggregate_maximum_bit_rate_dl;

  // start routine
  task_sched.schedule_async_task(
      ue_ctxt.ue_ids.ue_index,
      launch_async<ngap_pdu_session_resource_setup_procedure>(msg,
                                                              request->nas_pdu.copy(),
                                                              ue_ctxt.ue_ids,
                                                              ue->get_rrc_ue_pdu_notifier(),
                                                              ue->get_du_processor_control_notifier(),
                                                              ngap_notifier,
                                                              logger));
}

void ngap_impl::handle_pdu_session_resource_modify_request(const asn1::ngap::pdu_session_res_modify_request_s& request)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(request->ran_ue_ngap_id))) {
    logger.warning("ran_ue_id={} amf_ue_id={}: Dropping PduSessionResourceModifyRequest. UE context does not exist",
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    send_error_indication(ue_index_t::invalid, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(request->ran_ue_ngap_id)];
  if (ue_ctxt.release_scheduled) {
    logger.info(
        "ran_ue_id={} amf_ue_id={}: Dropping PduSessionResourceModifyRequest. UE is already scheduled for release",
        request->ran_ue_ngap_id,
        request->amf_ue_ngap_id);
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
  srsran_assert(ue != nullptr,
                "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Received PduSessionResourceModifyRequest",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);

  if (request->ran_paging_prio_present) {
    logger.debug("Not handling RAN paging prio");
  }

  // Convert to common type
  cu_cp_pdu_session_resource_modify_request msg;
  msg.ue_index = ue_ctxt.ue_ids.ue_index;
  fill_cu_cp_pdu_session_resource_modify_request(msg, request->pdu_session_res_modify_list_mod_req);

  // start routine
  task_sched.schedule_async_task(
      ue_ctxt.ue_ids.ue_index,
      launch_async<ngap_pdu_session_resource_modify_procedure>(
          msg, ue_ctxt.ue_ids, ue->get_du_processor_control_notifier(), ngap_notifier, logger));
}

void ngap_impl::handle_pdu_session_resource_release_command(const asn1::ngap::pdu_session_res_release_cmd_s& command)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(command->ran_ue_ngap_id))) {
    logger.warning("ran_ue_id={} amf_ue_id={}: Dropping PduSessionResourceReleaseCommand. UE context does not exist",
                   command->ran_ue_ngap_id,
                   command->amf_ue_ngap_id);
    send_error_indication(ue_index_t::invalid, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(command->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    logger.info(
        "ran_ue_id={} amf_ue_id={}: Dropping PduSessionResourceReleaseCommand. UE is already scheduled for release",
        command->ran_ue_ngap_id,
        command->amf_ue_ngap_id);
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
  srsran_assert(ue != nullptr,
                "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Received PduSessionResourceReleaseCommand",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);

  // Handle optional NAS PDU
  if (command->nas_pdu_present) {
    byte_buffer nas_pdu;
    nas_pdu.resize(command->nas_pdu.size());
    std::copy(command->nas_pdu.begin(), command->nas_pdu.end(), nas_pdu.begin());
    logger.debug(nas_pdu.begin(), nas_pdu.end(), "DlNasTransport PDU ({} B)", nas_pdu.length());

    ue->get_rrc_ue_pdu_notifier().on_new_pdu(std::move(nas_pdu));
  }

  // Convert to common type
  cu_cp_pdu_session_resource_release_command msg;
  msg.ue_index = ue_ctxt.ue_ids.ue_index;
  fill_cu_cp_pdu_session_resource_release_command(msg, command);

  // start routine
  task_sched.schedule_async_task(
      ue_ctxt.ue_ids.ue_index,
      launch_async<ngap_pdu_session_resource_release_procedure>(
          msg, ue_ctxt.ue_ids, ue->get_du_processor_control_notifier(), ngap_notifier, logger));
}

void ngap_impl::handle_ue_context_release_command(const asn1::ngap::ue_context_release_cmd_s& cmd)
{
  amf_ue_id_t amf_ue_id = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id = ran_ue_id_t::invalid;
  if (cmd->ue_ngap_ids.type() == asn1::ngap::ue_ngap_ids_c::types_opts::amf_ue_ngap_id) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids.amf_ue_ngap_id());
  } else if (cmd->ue_ngap_ids.type() == asn1::ngap::ue_ngap_ids_c::types_opts::ue_ngap_id_pair) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids.ue_ngap_id_pair().amf_ue_ngap_id);
    ran_ue_id = uint_to_ran_ue_id(cmd->ue_ngap_ids.ue_ngap_id_pair().ran_ue_ngap_id);
  }

  if (!ue_ctxt_list.contains(amf_ue_id)) {
    // TS 38.413 section 8.3.3 doesn't specify abnormal conditions, so we just drop the message and send an error
    // indication
    logger.error("{}amf_ue_id={}: Dropping UeContextReleaseCommand. UE does not exist",
                 ran_ue_id == ran_ue_id_t::invalid ? "" : fmt::format("ran_ue_id={} ", ran_ue_id),
                 amf_ue_id);
    send_error_indication(ue_index_t::invalid, cause_radio_network_t::unknown_local_ue_ngap_id, amf_ue_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[amf_ue_id];

  if (ue_ctxt.release_scheduled) {
    logger.info("{}amf_ue_id={}: Dropping UeContextReleaseCommand. UE is already scheduled for release",
                ran_ue_id == ran_ue_id_t::invalid ? "" : fmt::format("ran_ue_id={} ", ran_ue_id),
                amf_ue_id);
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, cause_radio_network_t::unknown_local_ue_ngap_id, amf_ue_id);
    return;
  } else {
    ue_ctxt.release_scheduled = true;
  }

  if (ran_ue_id == ran_ue_id_t::invalid) {
    ran_ue_id = ue_ctxt.ue_ids.ran_ue_id;
  }

  // Add AMF UE ID to UE, if its not set
  if (ue_ctxt.ue_ids.amf_ue_id == amf_ue_id_t::invalid) {
    ue_ctxt_list.add_amf_ue_id(ran_ue_id, amf_ue_id);
  }

  ngap_ue* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
  srsran_assert(ue != nullptr,
                "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Received UeContextReleaseCommand",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);

  // Convert to common type
  cu_cp_ngap_ue_context_release_command msg;
  msg.ue_index = ue_ctxt.ue_ids.ue_index;
  fill_cu_cp_ngap_ue_context_release_command(msg, cmd);

  // start routine
  task_sched.schedule_async_task(
      ue_ctxt.ue_ids.ue_index,
      launch_async<ngap_ue_context_release_procedure>(
          msg, ue_ctxt.ue_ids, ue->get_du_processor_control_notifier(), ngap_notifier, logger));
}

void ngap_impl::handle_paging(const asn1::ngap::paging_s& msg)
{
  logger.info("Received Paging");

  if (msg->ue_paging_id.type() != asn1::ngap::ue_paging_id_c::types::five_g_s_tmsi) {
    logger.error("Dropping PDU. Unsupportet UE Paging ID");
    send_error_indication();
    return;
  }

  // Convert to common type
  cu_cp_paging_message cu_cp_paging_msg;
  fill_cu_cp_paging_message(cu_cp_paging_msg, msg);

  cu_cp_du_repository_notifier.on_paging_message(cu_cp_paging_msg);
}

// free function to generate a handover failure message
ngap_message generate_ho_fail(uint64_t amf_ue_id)
{
  ngap_message ngap_msg;
  ngap_msg.pdu.set_unsuccessful_outcome();
  ngap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_HO_RES_ALLOC);
  auto& ho_fail           = ngap_msg.pdu.unsuccessful_outcome().value.ho_fail();
  ho_fail->amf_ue_ngap_id = amf_ue_id;
  ho_fail->cause.set_protocol();

  return ngap_msg;
}

void ngap_impl::handle_ho_request(const asn1::ngap::ho_request_s& msg)
{
  // convert ho request to common type
  ngap_handover_request ho_request;
  if (!fill_ngap_handover_request(ho_request, msg)) {
    logger.error("Sending HO Failure. Received invalid HO Request");
    ngap_notifier.on_new_message(generate_ho_fail(msg->amf_ue_ngap_id));
    return;
  }

  logger.info("Handover request - extracted target cell. plmn={}, target cell_id={}",
              ho_request.source_to_target_transparent_container.target_cell_id.plmn,
              ho_request.source_to_target_transparent_container.target_cell_id.nci);

  // Create UE in target cell
  ho_request.ue_index = cu_cp_du_repository_notifier.request_new_ue_index_allocation(
      ho_request.source_to_target_transparent_container.target_cell_id);
  if (ho_request.ue_index == ue_index_t::invalid) {
    logger.error("Sending HO Failure. Couldn't allocate UE index");
    ngap_notifier.on_new_message(generate_ho_fail(msg->amf_ue_ngap_id));
    return;
  }

  logger.debug("ue={}: Received HandoverRequest", ho_request.ue_index);

  // start handover routine
  task_sched.schedule_async_task(
      ho_request.ue_index,
      launch_async<ngap_handover_resource_allocation_procedure>(ho_request,
                                                                uint_to_amf_ue_id(msg->amf_ue_ngap_id),
                                                                ue_ctxt_list,
                                                                cu_cp_du_repository_notifier,
                                                                ngap_notifier,
                                                                logger));
}

void ngap_impl::handle_error_indication(const asn1::ngap::error_ind_s& msg)
{
  amf_ue_id_t amf_ue_id = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id = ran_ue_id_t::invalid;
  ue_index_t  ue_index  = ue_index_t::invalid;
  if (msg->amf_ue_ngap_id_present) {
    amf_ue_id = uint_to_amf_ue_id(msg->amf_ue_ngap_id);
    if (!ue_ctxt_list.contains(uint_to_amf_ue_id(msg->amf_ue_ngap_id))) {
      logger.warning("amf_ue_id={}: Dropping ErrorIndication. UE context does not exist", msg->amf_ue_ngap_id);
      send_error_indication(ue_index_t::invalid, cause_radio_network_t::inconsistent_remote_ue_ngap_id);
      return;
    }
    ue_index = ue_ctxt_list[amf_ue_id].ue_ids.ue_index;
  } else if (msg->ran_ue_ngap_id_present) {
    ran_ue_id = uint_to_ran_ue_id(msg->ran_ue_ngap_id);
    if (!ue_ctxt_list.contains(uint_to_ran_ue_id(msg->ran_ue_ngap_id))) {
      logger.warning("ran_ue_id={}: Dropping ErrorIndication. UE context does not exist", msg->ran_ue_ngap_id);
      send_error_indication(ue_index_t::invalid, cause_radio_network_t::unknown_local_ue_ngap_id);
      return;
    }
    ue_index = ue_ctxt_list[ran_ue_id].ue_ids.ue_index;
  }

  std::string msg_cause = "";
  if (msg->cause_present) {
    msg_cause = asn1_cause_to_string(msg->cause);
  }

  logger.info(
      "ue={} ran_ue_id={} amf_ue_id={}: Received ErrorIndication. Cause {}", ue_index, ran_ue_id, amf_ue_id, msg_cause);

  // TODO: handle error indication
}

void ngap_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp: {
      ev_mng.ng_setup_outcome.set(outcome.value.ng_setup_resp());
    } break;
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ho_cmd: {
      ev_mng.handover_preparation_outcome.set(outcome.value.ho_cmd());
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngap_impl::handle_unsuccessful_outcome(const unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ng_setup_fail: {
      ev_mng.ng_setup_outcome.set(outcome.value.ng_setup_fail());
    } break;
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_prep_fail: {
      ev_mng.handover_preparation_outcome.set(outcome.value.ho_prep_fail());
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngap_impl::handle_ue_context_release_request(const cu_cp_ue_context_release_request& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping UeContextReleaseRequest. UE context does not exist", msg.ue_index);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  if (ue_ctxt.ue_ids.amf_ue_id == amf_ue_id_t::invalid) {
    logger.debug("ue={} ran_ue_id={}: Ignoring UeContextReleaseRequest. UE does not have an AMF UE ID",
                 ue_ctxt.ue_ids.ue_index,
                 ue_ctxt.ue_ids.ran_ue_id);
    return;
  }

  if (ue_ctxt.release_requested) {
    logger.debug("ue={} ran_ue_id={}: Ignoring UeContextReleaseRequest. Request already pending",
                 msg.ue_index,
                 ue_ctxt.ue_ids.ran_ue_id);
    return;
  }

  ngap_message ngap_msg;
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE_REQUEST);

  auto& ue_context_release_request = ngap_msg.pdu.init_msg().value.ue_context_release_request();

  ue_context_release_request->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);
  ue_context_release_request->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);

  // Add PDU Session IDs
  if (!msg.pdu_session_res_list_cxt_rel_req.empty()) {
    ue_context_release_request->pdu_session_res_list_cxt_rel_req_present = true;
    for (const auto& session_id : msg.pdu_session_res_list_cxt_rel_req) {
      asn1::ngap::pdu_session_res_item_cxt_rel_req_s pdu_session_item;
      pdu_session_item.pdu_session_id = pdu_session_id_to_uint(session_id);
      ue_context_release_request->pdu_session_res_list_cxt_rel_req.push_back(pdu_session_item);
    }
  }

  ue_context_release_request->cause.set_radio_network();
  // TODO: Add sub causes to common type
  // ue_context_release_request->cause.value.radio_network() =
  //     asn1::ngap::cause_radio_network_opts::options::user_inactivity;

  // Forward message to AMF
  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Sending UeContextReleaseRequest",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);
  ue_ctxt.release_requested = true; // Mark UE so retx of request are avoided.
  ngap_notifier.on_new_message(ngap_msg);
}

// TODO make preparation result an async task
async_task<ngap_handover_preparation_response>
ngap_impl::handle_handover_preparation_request(const ngap_handover_preparation_request& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping HandoverPreparationRequest. UE context does not exist", msg.ue_index);
    return launch_async([](coro_context<async_task<ngap_handover_preparation_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(ngap_handover_preparation_response{false});
    });
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  ngap_ue* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
  srsran_assert(ue != nullptr,
                "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Starting HO preparation",
              ue_ctxt.ue_ids.ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);

  return launch_async<ngap_handover_preparation_procedure>(msg,
                                                           context,
                                                           ue_ctxt.ue_ids,
                                                           ngap_notifier,
                                                           ue->get_rrc_ue_control_notifier(),
                                                           ue->get_up_resource_manager(),
                                                           ev_mng,
                                                           timer_factory{task_sched.get_timer_manager(), ctrl_exec},
                                                           logger);
}

void ngap_impl::handle_inter_cu_ho_rrc_recfg_complete(const ue_index_t           ue_index,
                                                      const nr_cell_global_id_t& cgi,
                                                      const unsigned             tac)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Dropping RrcReconfigurationComplete. UE context does not exist", ue_index);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_HO_NOTIF);

  auto& ho_notify           = ngap_msg.pdu.init_msg().value.ho_notify();
  ho_notify->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);
  ho_notify->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);

  auto& user_loc_info_nr  = ho_notify->user_location_info.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi = nr_cgi_to_ngap_asn1(cgi);
  user_loc_info_nr.tai.plmn_id.from_string(cgi.plmn_hex);
  user_loc_info_nr.tai.tac.from_number(tac);

  // Forward message to AMF
  logger.info("ue={} ran_ue_id={} amf_ue_id={}: Sending HandoverNotify",
              ue_index,
              ue_ctxt.ue_ids.ran_ue_id,
              ue_ctxt.ue_ids.amf_ue_id);
  ngap_notifier.on_new_message(ngap_msg);
}

void ngap_impl::remove_ue_context(ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.debug("ue={}: UE context not found", ue_index);
    return;
  }

  ue_ctxt_list.remove_ue_context(ue_index);
}

void ngap_impl::send_error_indication(ue_index_t ue_index, optional<cause_t> cause, optional<amf_ue_id_t> amf_ue_id)
{
  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_ERROR_IND);
  auto& error_ind = ngap_msg.pdu.init_msg().value.error_ind();

  if (ue_index != ue_index_t::invalid) {
    if (ue_ctxt_list.contains(ue_index)) {
      ngap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

      error_ind->ran_ue_ngap_id_present = true;
      error_ind->ran_ue_ngap_id         = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);

      if (ue_ctxt.ue_ids.amf_ue_id != amf_ue_id_t::invalid) {
        error_ind->amf_ue_ngap_id_present = true;
        error_ind->amf_ue_ngap_id         = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);
      }
    }
  }

  // Set optionally provided AMF ID
  if (amf_ue_id.has_value()) {
    error_ind->amf_ue_ngap_id_present = true;
    error_ind->amf_ue_ngap_id         = amf_ue_id_to_uint(amf_ue_id.value());
  }

  if (cause.has_value()) {
    error_ind->cause_present = true;
    error_ind->cause         = cause_to_asn1(cause.value());
  }

  // TODO: Add missing values

  // Forward message to AMF
  logger.info("{}{}{}: Sending ErrorIndication",
              ue_index != ue_index_t::invalid ? fmt::format("ue={}", ue_index) : "",
              error_ind->ran_ue_ngap_id_present ? fmt::format(" ran_ue_id={}", error_ind->ran_ue_ngap_id) : "",
              error_ind->amf_ue_ngap_id_present ? fmt::format(" amf_ue_id={}", error_ind->amf_ue_ngap_id) : "");
  ngap_notifier.on_new_message(ngap_msg);
}

void ngap_impl::schedule_error_indication(ue_index_t ue_index, cause_t cause, optional<amf_ue_id_t> amf_ue_id)
{
  logger.info("{}{}: Scheduling ErrorIndication",
              ue_index != ue_index_t::invalid ? fmt::format("ue={}", ue_index) : "",
              amf_ue_id.has_value() ? fmt::format(" amf_ue_id={}", amf_ue_id.value()) : "");
  task_sched.schedule_async_task(ue_index,
                                 launch_async([this, ue_index, cause, amf_ue_id](coro_context<async_task<void>>& ctx) {
                                   CORO_BEGIN(ctx);
                                   send_error_indication(ue_index, cause, amf_ue_id);
                                   CORO_RETURN();
                                 }));
}

void ngap_impl::on_ue_context_setup_timer_expired(ue_index_t ue_index)
{
  if (ue_ctxt_list.contains(ue_index)) {
    ngap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

    logger.warning("ue={}: UE context setup timer expired after {}s. Releasing UE from DU",
                   ue_index,
                   ue_ctxt.ue_context_setup_timer.duration().count());

    auto* ue = ue_manager.find_ngap_ue(ue_ctxt.ue_ids.ue_index);
    srsran_assert(ue != nullptr,
                  "ue={} ran_ue_id={} amf_ue_id={}: UE for UE context doesn't exist",
                  ue_ctxt.ue_ids.ue_index,
                  ue_ctxt.ue_ids.ran_ue_id,
                  ue_ctxt.ue_ids.amf_ue_id);

    task_sched.schedule_async_task(ue_index, launch_async([ue, ue_index](coro_context<async_task<void>>& ctx) {
                                     CORO_BEGIN(ctx);
                                     CORO_AWAIT(
                                         ue->get_du_processor_control_notifier().on_new_ue_context_release_command(
                                             {ue_index, cause_nas_t::unspecified}));
                                     CORO_RETURN();
                                   }));

  } else {
    logger.debug("ue={}: Ignoring expired UE context setup timer. UE context not found", ue_index);
    return;
  }
}
