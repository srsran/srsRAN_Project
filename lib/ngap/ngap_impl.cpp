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
#include "procedures/ngap_initial_context_setup_procedure.h"
#include "procedures/ngap_pdu_session_resource_setup_procedure.h"
#include "procedures/ngap_procedure_helpers.h"

using namespace srsran;
using namespace asn1::ngap;
using namespace srs_cu_cp;

ngap_impl::ngap_impl(ngap_configuration&     ngap_cfg_,
                     ngap_ue_task_scheduler& task_sched_,
                     ngap_ue_manager&        ue_manager_,
                     ngap_message_notifier&  ngap_notifier_,
                     task_executor&          ctrl_exec_) :
  logger(srslog::fetch_basic_logger("NGAP")),
  ngap_cfg(ngap_cfg_),
  task_sched(task_sched_),
  ue_manager(ue_manager_),
  ngap_notifier(ngap_notifier_),
  ctrl_exec(ctrl_exec_),
  ev_mng(task_sched.get_timer_manager())
{
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
  return launch_async<ng_setup_procedure>(request, ngap_notifier, ev_mng, task_sched.get_timer_manager(), logger);
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

  auto& init_ue_msg                       = ngap_msg.pdu.init_msg().value.init_ue_msg();
  init_ue_msg->ran_ue_ngap_id.value.value = ran_ue_id_to_uint(ue->get_ran_ue_id());

  init_ue_msg->nas_pdu.value.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), init_ue_msg->nas_pdu.value.begin());

  init_ue_msg->rrc_establishment_cause.value.value = msg.establishment_cause.value;

  auto& user_loc_info_nr       = init_ue_msg->user_location_info.value.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = msg.nr_cgi;
  user_loc_info_nr.tai.plmn_id = msg.nr_cgi.plmn_id;
  user_loc_info_nr.tai.tac.from_number(msg.tac);

  init_ue_msg->ue_context_request_present = true;
  init_ue_msg->ue_context_request.value   = asn1::ngap::ue_context_request_opts::options::requested;

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

  ul_nas_transport_msg->ran_ue_ngap_id.value.value = ran_ue_id_to_uint(ue->get_ran_ue_id());

  amf_ue_id_t amf_ue_id = ue->get_amf_ue_id();
  if (amf_ue_id == amf_ue_id_t::invalid) {
    logger.warning("ue={} UE AMF ID not found!", msg.ue_index);
    return;
  }
  ul_nas_transport_msg->amf_ue_ngap_id.value.value = amf_ue_id_to_uint(amf_ue_id);

  ul_nas_transport_msg->nas_pdu.value.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), ul_nas_transport_msg->nas_pdu.value.begin());

  auto& user_loc_info_nr       = ul_nas_transport_msg->user_location_info.value.set_user_location_info_nr();
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
    case ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd:
      handle_ue_context_release_command(msg.value.ue_context_release_cmd());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngap_impl::handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg)
{
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(msg->ran_ue_ngap_id.value.value));
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
    logger.debug("ue={} Received AMF UE ID={}", ue_index, uint_to_amf_ue_id(msg->amf_ue_ngap_id.value.value));
    ue_manager.set_amf_ue_id(ue_index, uint_to_amf_ue_id(msg->amf_ue_ngap_id.value.value));
  }

  byte_buffer nas_pdu;
  nas_pdu.resize(msg->nas_pdu.value.size());
  std::copy(msg->nas_pdu.value.begin(), msg->nas_pdu.value.end(), nas_pdu.begin());
  logger.debug(nas_pdu.begin(), nas_pdu.end(), "DlNasTransport PDU ({} B)", nas_pdu.length());

  ue->get_rrc_ue_pdu_notifier().on_new_pdu(std::move(nas_pdu));
}

void ngap_impl::handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request)
{
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(request->ran_ue_ngap_id.value));
  auto*      ue       = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping InitialContextSetupRequest", ue_index);
    return;
  }

  logger.info("ue={} Received InitialContextSetupRequest (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Update AMF ID and use the one from this Context Setup as per TS 38.413 v16.2 page 38
  ue_manager.set_amf_ue_id(ue_index, uint_to_amf_ue_id(request->amf_ue_ngap_id.value.value));

  // start routine
  task_sched.schedule_async_task(
      ue_index,
      launch_async<ngap_initial_context_setup_procedure>(ue_index, request, ue_manager, ngap_notifier, logger));
}

void ngap_impl::handle_pdu_session_resource_setup_request(const asn1::ngap::pdu_session_res_setup_request_s& request)
{
  ue_index_t ue_index = ue_manager.get_ue_index(uint_to_ran_ue_id(request->ran_ue_ngap_id.value));
  auto*      ue       = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping PduSessionResourceSetupRequest", ue_index);
    return;
  }

  logger.info("ue={} Received PduSessionResourceSetupRequest (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Store information in UE context
  if (request->ue_aggr_max_bit_rate_present) {
    ue->set_aggregate_maximum_bit_rate_dl(request->ue_aggr_max_bit_rate.value.ue_aggr_max_bit_rate_dl);
  }

  // Convert to common type
  cu_cp_pdu_session_resource_setup_request msg;
  msg.ue_index     = ue_index;
  msg.serving_plmn = ngap_cfg.plmn;
  fill_cu_cp_pdu_session_resource_setup_request(msg, request->pdu_session_res_setup_list_su_req.value);
  msg.ue_aggregate_maximum_bit_rate_dl = ue->get_aggregate_maximum_bit_rate_dl();
  msg.qos_config                       = ngap_cfg.qos_config;

  // start routine
  task_sched.schedule_async_task(ue_index,
                                 launch_async<ngap_pdu_session_resource_setup_procedure>(
                                     *ue, msg, ue->get_du_processor_control_notifier(), ngap_notifier, logger));

  // Handle optional parameters
  if (request->nas_pdu_present) {
    handle_nas_pdu(logger, request->nas_pdu.value, *ue);
  }
}

void ngap_impl::handle_ue_context_release_command(const asn1::ngap::ue_context_release_cmd_s& cmd)
{
  amf_ue_id_t amf_ue_id = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id = ran_ue_id_t::invalid;
  if (cmd->ue_ngap_ids.value.type() == asn1::ngap::ue_ngap_ids_c::types_opts::amf_ue_ngap_id) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids->amf_ue_ngap_id());
  } else if (cmd->ue_ngap_ids.value.type() == asn1::ngap::ue_ngap_ids_c::types_opts::ue_ngap_id_pair) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids->ue_ngap_id_pair().amf_ue_ngap_id);
    ran_ue_id = uint_to_ran_ue_id(cmd->ue_ngap_ids->ue_ngap_id_pair().ran_ue_ngap_id);
  }
  ue_index_t ue_index = ue_manager.get_ue_index(amf_ue_id);
  auto*      ue       = ue_manager.find_ngap_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={} does not exist - dropping UeContextReleaseCommand", ue_index);
    return;
  }

  if (ran_ue_id == ran_ue_id_t::invalid) {
    ran_ue_id = ue->get_ran_ue_id();
  }

  logger.info("ue={} Received UeContextReleaseCommand (ran_ue_id={})", ue_index, ue->get_ran_ue_id());

  // Convert to common type
  cu_cp_ue_context_release_command msg;
  msg.ue_index = ue_index;
  fill_cu_cp_ue_context_release_command(msg, cmd);

  // Notify DU processor about UE Context Release Command
  ue->get_du_processor_control_notifier().on_new_ue_context_release_command(msg);

  // Send UE Context Release Complete
  ngap_message ngap_msg = {};

  ngap_msg.pdu.set_successful_outcome();
  ngap_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);

  // TODO: Add optional fields to UE context Release Complete
  // fill_asn1_ue_context_release_complete(ngap_msg.pdu.successful_outcome().value.ue_context_release_complete(),
  // response);

  auto& ue_context_release_complete = ngap_msg.pdu.successful_outcome().value.ue_context_release_complete();
  ue_context_release_complete->amf_ue_ngap_id.value = amf_ue_id_to_uint(amf_ue_id);
  ue_context_release_complete->ran_ue_ngap_id.value = ran_ue_id_to_uint(ran_ue_id);

  // Remove NGAP UE
  ue_manager.remove_ngap_ue(ue_index);

  // Remove NGAP UE
  ue_manager.remove_ngap_ue(ue_index);

  logger.info("Sending UeContextReleaseComplete");
  ngap_notifier.on_new_message(ngap_msg);
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

size_t ngap_impl::get_nof_ues() const
{
  return ue_manager.get_nof_ngap_ues();
}
