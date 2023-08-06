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
#include "procedures/ngap_handover_preparation_procedure.h"
#include "procedures/ngap_handover_resource_allocation_procedure.h"
#include "procedures/ngap_initial_context_setup_procedure.h"
#include "procedures/ngap_pdu_session_resource_modify_procedure.h"
#include "procedures/ngap_pdu_session_resource_release_procedure.h"
#include "procedures/ngap_pdu_session_resource_setup_procedure.h"
#include "procedures/ngap_procedure_helpers.h"

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
  cu_cp_du_repository_notifier(cu_cp_du_repository_notifier_),
  task_sched(task_sched_),
  ue_manager(ue_manager_),
  ngap_notifier(ngap_notifier_),
  ctrl_exec(ctrl_exec_),
  ev_mng(timer_factory{task_sched.get_timer_manager(), ctrl_exec})
{
  context.gnb_id        = ngap_cfg_.gnb_id;
  context.ran_node_name = ngap_cfg_.ran_node_name;
  context.plmn          = ngap_cfg_.plmn;
  context.tac           = ngap_cfg_.tac;
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
ngap_impl::~ngap_impl() {}

void ngap_impl::create_ngap_ue(ue_index_t                          ue_index,
                               ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                               ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                               ngap_du_processor_control_notifier& du_processor_ctrl_notifier)
{
  // Create UE context and store it
  ngap_ue* ue = ue_manager.add_ue(ue_index, rrc_ue_pdu_notifier, rrc_ue_ctrl_notifier, du_processor_ctrl_notifier);

  if (ue == nullptr) {
    logger.error("ue={} Failed to create UE", ue_index);
    return;
  }

  logger.debug("ue={} Created UE (ran_ue_id={})", ue_index, ue->get_ran_ue_id());
}

async_task<ng_setup_response> ngap_impl::handle_ng_setup_request(const ng_setup_request& request)
{
  logger.info("Sending NgSetupRequest");
  return launch_async<ng_setup_procedure>(
      context, request, ngap_notifier, ev_mng, timer_factory{task_sched.get_timer_manager(), ctrl_exec}, logger);
}

void ngap_impl::handle_initial_ue_message(const ngap_initial_ue_message& msg)
{
  auto* ue = ue_manager.find_ngap_ue(msg.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping InitialUeMessage", msg.ue_index);
    return;
  }

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_UE_MSG);

  auto& init_ue_msg           = ngap_msg.pdu.init_msg().value.init_ue_msg();
  init_ue_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue->get_ran_ue_id());

  init_ue_msg->nas_pdu.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), init_ue_msg->nas_pdu.begin());

  init_ue_msg->rrc_establishment_cause.value = msg.establishment_cause.value;

  auto& user_loc_info_nr       = init_ue_msg->user_location_info.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = msg.nr_cgi;
  user_loc_info_nr.tai.plmn_id = msg.nr_cgi.plmn_id;
  user_loc_info_nr.tai.tac.from_number(msg.tac);

  init_ue_msg->ue_context_request_present = true;
  init_ue_msg->ue_context_request.value   = asn1::ngap::ue_context_request_opts::options::requested;

  if (msg.five_g_s_tmsi.has_value()) {
    init_ue_msg->five_g_s_tmsi_present = true;
    init_ue_msg->five_g_s_tmsi.amf_set_id.from_number(context.current_guami.amf_set_id);
    init_ue_msg->five_g_s_tmsi.amf_pointer.from_number(context.current_guami.amf_pointer);
    init_ue_msg->five_g_s_tmsi.five_g_tmsi.from_number(msg.five_g_s_tmsi.value().five_g_tmsi);
  }

  // TODO: Add missing optional values

  logger.info("ue={} Sending InitialUeMessage (ran_ue_id={})", msg.ue_index, ue->get_ran_ue_id());

  // Forward message to AMF
  ngap_notifier.on_new_message(ngap_msg);
}

void ngap_impl::handle_ul_nas_transport_message(const ngap_ul_nas_transport_message& msg)
{
  auto* ue = ue_manager.find_ngap_ue(msg.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping UlNasTransportMessage", msg.ue_index);
    return;
  }

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UL_NAS_TRANSPORT);

  auto& ul_nas_transport_msg = ngap_msg.pdu.init_msg().value.ul_nas_transport();

  ul_nas_transport_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue->get_ran_ue_id());

  amf_ue_id_t amf_ue_id = ue->get_amf_ue_id();
  if (amf_ue_id == amf_ue_id_t::invalid) {
    logger.warning("ue={} UE AMF ID not found!", msg.ue_index);
    return;
  }
  ul_nas_transport_msg->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);

  ul_nas_transport_msg->nas_pdu.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), ul_nas_transport_msg->nas_pdu.begin());

  auto& user_loc_info_nr       = ul_nas_transport_msg->user_location_info.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = msg.nr_cgi;
  user_loc_info_nr.tai.plmn_id = msg.nr_cgi.plmn_id;
  user_loc_info_nr.tai.tac.from_number(msg.tac);

  logger.info("ue={} Sending UlNasTransportMessage (ran_ue_id={})", msg.ue_index, ue->get_ran_ue_id());

  // Forward message to AMF
  ngap_notifier.on_new_message(ngap_msg);
}

void ngap_impl::handle_message(const ngap_message& msg)
{
  logger.debug("Handling PDU of type \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));

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
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(msg->ran_ue_ngap_id));
  if (ue_index == ue_index_t::invalid) {
    logger.warning("ue={} does not exist - dropping PDU", ue_index);
    return;
  }

  auto* ue = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping PDU", ue_index);
    return;
  }

  logger.info("ue={} Received DlNasTransportMessage (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Add AMF UE ID to ue ngap context if it is not set (this is the first DL NAS Transport message)
  if (ue->get_amf_ue_id() == amf_ue_id_t::invalid) {
    // Set AMF UE ID in the UE context and also in the lookup
    logger.debug("ue={} Received AMF UE ID={}", ue_index, uint_to_amf_ue_id(msg->amf_ue_ngap_id));
    ue_manager.set_amf_ue_id(ue_index, uint_to_amf_ue_id(msg->amf_ue_ngap_id));
  }

  byte_buffer nas_pdu;
  nas_pdu.resize(msg->nas_pdu.size());
  std::copy(msg->nas_pdu.begin(), msg->nas_pdu.end(), nas_pdu.begin());
  logger.debug(nas_pdu.begin(), nas_pdu.end(), "DlNasTransport PDU ({} B)", nas_pdu.length());

  ue->get_rrc_ue_pdu_notifier().on_new_pdu(std::move(nas_pdu));
}

void ngap_impl::handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request)
{
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(request->ran_ue_ngap_id));
  auto*      ue       = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping InitialContextSetupRequest", ue_index);
    return;
  }

  logger.info("ue={} Received InitialContextSetupRequest (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Update AMF ID and use the one from this Context Setup as per TS 38.413 v16.2 page 38
  ue_manager.set_amf_ue_id(ue_index, uint_to_amf_ue_id(request->amf_ue_ngap_id));

  // start routine
  task_sched.schedule_async_task(ue_index,
                                 launch_async<ngap_initial_context_setup_procedure>(
                                     context, ue_index, request, ue_manager, ngap_notifier, logger));
}

void ngap_impl::handle_pdu_session_resource_setup_request(const asn1::ngap::pdu_session_res_setup_request_s& request)
{
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(request->ran_ue_ngap_id));
  ngap_ue*   ue       = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping PduSessionResourceSetupRequest (ran_ue_id={}, amf_ue_id={})",
                   ue_index,
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    return;
  }

  logger.info("ue={} Received PduSessionResourceSetupRequest (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Store information in UE context
  if (request->ue_aggr_max_bit_rate_present) {
    ue->set_aggregate_maximum_bit_rate_dl(request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl);
  }

  // Convert to common type
  cu_cp_pdu_session_resource_setup_request msg;
  msg.ue_index     = ue_index;
  msg.serving_plmn = context.plmn;
  if (!fill_cu_cp_pdu_session_resource_setup_request(msg, request->pdu_session_res_setup_list_su_req)) {
    logger.error("ue={} Conversion of PDU Session Resource Setup Request failed.", ue_index);
    return;
  }
  msg.ue_aggregate_maximum_bit_rate_dl = ue->get_aggregate_maximum_bit_rate_dl();

  // start routine
  task_sched.schedule_async_task(ue_index,
                                 launch_async<ngap_pdu_session_resource_setup_procedure>(
                                     msg, *ue, ue->get_du_processor_control_notifier(), ngap_notifier, logger));

  // Handle optional parameters
  if (request->nas_pdu_present) {
    handle_nas_pdu(logger, request->nas_pdu, *ue);
  }
}

void ngap_impl::handle_pdu_session_resource_modify_request(const asn1::ngap::pdu_session_res_modify_request_s& request)
{
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(request->ran_ue_ngap_id));
  ngap_ue*   ue       = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping PduSessionResourceModifyRequest (ran_ue_id={}, amf_ue_id={})",
                   ue_index,
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    return;
  }

  logger.info("ue={} Received PduSessionResourceModifyRequest (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  if (request->ran_paging_prio_present) {
    logger.debug("Not handling RAN paging prio");
  }

  // Convert to common type
  cu_cp_pdu_session_resource_modify_request msg;
  msg.ue_index = ue_index;
  fill_cu_cp_pdu_session_resource_modify_request(msg, request->pdu_session_res_modify_list_mod_req);

  // start routine
  task_sched.schedule_async_task(ue_index,
                                 launch_async<ngap_pdu_session_resource_modify_procedure>(
                                     msg, *ue, ue->get_du_processor_control_notifier(), ngap_notifier, logger));
}

void ngap_impl::handle_pdu_session_resource_release_command(const asn1::ngap::pdu_session_res_release_cmd_s& command)
{
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(command->ran_ue_ngap_id));
  ngap_ue*   ue       = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping PduSessionResourceReleaseCommand (ran_ue_id={}, amf_ue_id={})",
                   ue_index,
                   command->ran_ue_ngap_id,
                   command->amf_ue_ngap_id);
    return;
  }

  logger.info("ue={} Received PduSessionResourceReleaseCommand (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Handle optional NAS PDU
  if (command->nas_pdu_present) {
    byte_buffer nas_pdu;
    nas_pdu.resize(command->nas_pdu.size());
    std::copy(command->nas_pdu.begin(), command->nas_pdu.end(), nas_pdu.begin());
    logger.debug(nas_pdu.begin(), nas_pdu.end(), "DlNasTransport PDU ({} B)", nas_pdu.length());

    ue->get_rrc_ue_pdu_notifier().on_new_pdu(std::move(nas_pdu));
  }

  cu_cp_pdu_session_resource_release_command msg;
  msg.ue_index = ue_index;
  fill_cu_cp_pdu_session_resource_release_command(msg, command);

  // start routine
  task_sched.schedule_async_task(ue_index,
                                 launch_async<ngap_pdu_session_resource_release_procedure>(
                                     *ue, msg, ue->get_du_processor_control_notifier(), ngap_notifier, logger));
}

void ngap_impl::handle_ue_context_release_command(const asn1::ngap::ue_context_release_cmd_s& cmd)
{
  amf_ue_id_t amf_ue_id = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id = ran_ue_id_t::invalid;
  ue_index_t  ue_index  = ue_index_t::invalid;
  if (cmd->ue_ngap_ids.type() == asn1::ngap::ue_ngap_ids_c::types_opts::amf_ue_ngap_id) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids.amf_ue_ngap_id());
    ue_index  = ue_manager.get_ue_index(amf_ue_id);
  } else if (cmd->ue_ngap_ids.type() == asn1::ngap::ue_ngap_ids_c::types_opts::ue_ngap_id_pair) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids.ue_ngap_id_pair().amf_ue_ngap_id);
    ran_ue_id = uint_to_ran_ue_id(cmd->ue_ngap_ids.ue_ngap_id_pair().ran_ue_ngap_id);
    ue_index  = ue_manager.get_ue_index(ran_ue_id);
  }

  auto* ue = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    // TS 38.413 section 8.3.3 doesn't specify abnormal conditions, so we just drop the message
    logger.warning("ue={} does not exist - dropping UeContextReleaseCommand{}{}",
                   ue_index,
                   amf_ue_id == amf_ue_id_t::invalid ? "" : fmt::format(" amf_ue_id={}", amf_ue_id),
                   ran_ue_id == ran_ue_id_t::invalid ? "" : fmt::format(" ran_ue_id={}", ran_ue_id));
    return;
  }

  if (ran_ue_id == ran_ue_id_t::invalid) {
    ran_ue_id = ue->get_ran_ue_id();
  }

  logger.info("ue={} Received UeContextReleaseCommand (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Convert to common type
  cu_cp_ngap_ue_context_release_command msg;
  msg.ue_index = ue_index;
  fill_cu_cp_ngap_ue_context_release_command(msg, cmd);

  // Notify DU processor about UE Context Release Command
  cu_cp_ue_context_release_complete ue_context_release_complete =
      ue->get_du_processor_control_notifier().on_new_ue_context_release_command(msg);

  // Send UE Context Release Complete
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);

  auto& asn1_ue_context_release_complete = ngap_msg.pdu.successful_outcome().value.ue_context_release_complete();
  asn1_ue_context_release_complete->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  asn1_ue_context_release_complete->ran_ue_ngap_id = ran_ue_id_to_uint(ran_ue_id);

  fill_asn1_ue_context_release_complete(asn1_ue_context_release_complete, ue_context_release_complete);

  // Remove NGAP UE
  ue_manager.remove_ngap_ue(ue_index);

  logger.info("ue={} Sending UeContextReleaseComplete", ue_index);
  ngap_notifier.on_new_message(ngap_msg);
}

void ngap_impl::handle_paging(const asn1::ngap::paging_s& msg)
{
  logger.info("Received Paging");

  if (msg->ue_paging_id.type() != asn1::ngap::ue_paging_id_c::types::five_g_s_tmsi) {
    logger.error("Unsupportet UE Paging ID");
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
    logger.error("Received invalid HO Request - sending Ho Failure.");
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
    logger.error("Couldn't allocate UE index - sending Ho Failure");
    ngap_notifier.on_new_message(generate_ho_fail(msg->amf_ue_ngap_id));
    return;
  }

  logger.debug("Handover request - allocated ue_index={}.", ho_request.ue_index);

  // start handover routine
  task_sched.schedule_async_task(
      ho_request.ue_index,
      launch_async<ngap_handover_resource_allocation_procedure>(ho_request,
                                                                uint_to_amf_ue_id(msg->amf_ue_ngap_id),
                                                                cu_cp_du_repository_notifier,
                                                                ngap_notifier,
                                                                ue_manager,
                                                                logger));
}

void ngap_impl::handle_error_indication(const asn1::ngap::error_ind_s& msg)
{
  ue_index_t ue_index = ue_index_t::invalid;
  ngap_ue*   ue       = nullptr;
  if (msg->amf_ue_ngap_id_present) {
    ue_index = ue_manager.get_ue_index(uint_to_amf_ue_id(msg->amf_ue_ngap_id));
    ue       = ue_manager.find_ngap_ue(ue_index);
  } else if (msg->ran_ue_ngap_id_present) {
    ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(msg->ran_ue_ngap_id));
    ue       = ue_manager.find_ngap_ue(ue_index);
  }

  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping ErrorIndication", ue_index);
    return;
  } else {
    std::string cause = "";
    if (msg->cause_present) {
      cause = asn1_cause_to_string(msg->cause);
    }

    logger.info("ue={} Received ErrorIndication (ran_ue_id={}) - cause {}", ue_index, ue->get_ran_ue_id(), cause);
  }

  // TODO: handle error indication
}

void ngap_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp: {
      ev_mng.ng_setup_outcome.set(outcome.value.ng_setup_resp());
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
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngap_impl::handle_ue_context_release_request(const cu_cp_ue_context_release_request& msg)
{
  ngap_ue* ue = ue_manager.find_ngap_ue(msg.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist", msg.ue_index);
    return;
  }

  if (ue->get_amf_ue_id() == amf_ue_id_t::invalid) {
    logger.debug("ue={} does not have an AMF UE ID - ignoring release request", msg.ue_index);
    return;
  }

  ngap_message ngap_msg;
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE_REQUEST);

  auto& ue_context_release_request = ngap_msg.pdu.init_msg().value.ue_context_release_request();

  ue_context_release_request->ran_ue_ngap_id = ran_ue_id_to_uint(ue->get_ran_ue_id());
  ue_context_release_request->amf_ue_ngap_id = amf_ue_id_to_uint(ue->get_amf_ue_id());

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
  logger.info("ue={} Sending UeContextReleaseRequest", msg.ue_index);
  ngap_notifier.on_new_message(ngap_msg);
}

// TODO make preparation result an async task
async_task<ngap_handover_preparation_response>
ngap_impl::handle_handover_preparation_request(const ngap_handover_preparation_request& msg)
{
  logger.info("Starting HO preparation");
  return launch_async<ngap_handover_preparation_procedure>(msg,
                                                           context,
                                                           ue_manager,
                                                           ngap_notifier,
                                                           ev_mng,
                                                           timer_factory{task_sched.get_timer_manager(), ctrl_exec},
                                                           logger);
}

size_t ngap_impl::get_nof_ues() const
{
  return ue_manager.get_nof_ngap_ues();
}
