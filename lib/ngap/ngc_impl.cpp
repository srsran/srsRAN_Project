/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngc_impl.h"
#include "ngap_asn1_utils.h"
#include "procedures/ng_initial_context_setup_procedure.h"
#include "procedures/ng_setup_procedure.h"
#include "srsgnb/support/async/event_signal.h"

using namespace srsgnb;
using namespace asn1::ngap;
using namespace srs_cu_cp;

ngc_impl::ngc_impl(ngc_ue_task_scheduler& task_sched_,
                   ngc_ue_manager&        ue_manager_,
                   ngc_message_notifier&  ngc_notifier_) :
  logger(srslog::fetch_basic_logger("NGC")),
  task_sched(task_sched_),
  ue_manager(ue_manager_),
  ngc_notifier(ngc_notifier_),
  events(std::make_unique<ngc_event_manager>())
{
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
ngc_impl::~ngc_impl() {}

void ngc_impl::create_ngc_ue(du_index_t                   du_index,
                             ue_index_t                   ue_index,
                             ngc_rrc_ue_pdu_notifier&     rrc_ue_pdu_notifier,
                             ngc_rrc_ue_control_notifier& rrc_ue_ctrl_notifier)
{
  // Create UE context and store it
  cu_cp_ue_id_t cu_cp_ue_id = get_cu_cp_ue_id(du_index, ue_index);
  auto&         ue          = ue_manager.add_ue(cu_cp_ue_id, rrc_ue_pdu_notifier, rrc_ue_ctrl_notifier);

  logger.debug("Created NGAP UE (ran_ue_id={}, cu_cp_ue_id={}, du_index={}, ue_index={})",
               ue.get_ran_ue_id(),
               cu_cp_ue_id,
               du_index,
               ue_index);
}

async_task<ng_setup_response_message> ngc_impl::handle_ng_setup_request(const ng_setup_request_message& request)
{
  return launch_async<ng_setup_procedure>(request, ngc_notifier, *events, logger);
}

void ngc_impl::handle_initial_ue_message(const ngap_initial_ue_message& msg)
{
  auto* ue = ue_manager.find_ue(msg.cu_cp_ue_id);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping Initial UE Message", msg.cu_cp_ue_id);
    return;
  }

  logger.info("Handling Initial UE Message for UE with ran_ue_id={}", ue->get_ran_ue_id());

  ngc_message ngc_msg = {};
  ngc_msg.pdu.set_init_msg();
  ngc_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_UE_MSG);

  auto& init_ue_msg                       = ngc_msg.pdu.init_msg().value.init_ue_msg();
  init_ue_msg->ran_ue_ngap_id.value.value = ran_ue_id_to_uint(ue->get_ran_ue_id());

  init_ue_msg->nas_pdu.value.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), init_ue_msg->nas_pdu.value.begin());

  init_ue_msg->rrcestablishment_cause.value.value = msg.establishment_cause.value;

  auto& user_loc_info_nr       = init_ue_msg->user_location_info.value.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = msg.nr_cgi;
  user_loc_info_nr.tai.plmn_id = msg.nr_cgi.plmn_id;
  user_loc_info_nr.tai.tac.from_number(msg.tac);

  init_ue_msg->ue_context_request_present = true;
  init_ue_msg->ue_context_request.value   = asn1::ngap::ue_context_request_opts::options::requested;

  // TODO: Add missing optional values

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    ngc_msg.pdu.to_json(js);
    logger.debug("Containerized Initial UE Message: {}", js.to_string());
  }

  // Forward message to AMF
  ngc_notifier.on_new_message(ngc_msg);
}

void ngc_impl::handle_ul_nas_transport_message(const ngap_ul_nas_transport_message& msg)
{
  auto* ue = ue_manager.find_ue(msg.cu_cp_ue_id);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping UL NAS Transport Message", msg.cu_cp_ue_id);
    return;
  }

  logger.info("Handling UL NAS Transport Message for UE with ran_ue_id={}", ue->get_ran_ue_id());

  ngc_message ngc_msg = {};
  ngc_msg.pdu.set_init_msg();
  ngc_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UL_NAS_TRANSPORT);

  auto& ul_nas_transport_msg = ngc_msg.pdu.init_msg().value.ul_nas_transport();

  ul_nas_transport_msg->ran_ue_ngap_id.value.value = ran_ue_id_to_uint(ue->get_ran_ue_id());

  amf_ue_id_t amf_ue_id = ue->get_amf_ue_id();
  if (amf_ue_id == amf_ue_id_t::invalid) {
    logger.error("UE AMF ID for cu_cp_ue_id={} not found!", msg.cu_cp_ue_id);
    return;
  }
  ul_nas_transport_msg->amf_ue_ngap_id.value.value = amf_ue_id_to_uint(amf_ue_id);

  ul_nas_transport_msg->nas_pdu.value.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), ul_nas_transport_msg->nas_pdu.value.begin());

  auto& user_loc_info_nr       = ul_nas_transport_msg->user_location_info.value.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = msg.nr_cgi;
  user_loc_info_nr.tai.plmn_id = msg.nr_cgi.plmn_id;
  user_loc_info_nr.tai.tac.from_number(msg.tac);

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    ngc_msg.pdu.to_json(js);
    logger.debug("Containerized UL NAS Transport Message: {}", js.to_string());
  }

  // Forward message to AMF
  ngc_notifier.on_new_message(ngc_msg);
}

void ngc_impl::handle_message(const ngc_message& msg)
{
  logger.info("Handling NGAP PDU of type \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));

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
}

void ngc_impl::handle_initiating_message(const init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case ngap_elem_procs_o::init_msg_c::types_opts::dl_nas_transport:
      handle_dl_nas_transport_message(msg.value.dl_nas_transport());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::init_context_setup_request:
      handle_initial_context_setup_request(msg.value.init_context_setup_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd:
      handle_ue_context_release_command(msg.value.ue_context_release_cmd());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngc_impl::handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg)
{
  cu_cp_ue_id_t cu_cp_ue_id = ue_manager.get_cu_cp_ue_id(uint_to_ran_ue_id(msg->ran_ue_ngap_id.value.value));
  if (cu_cp_ue_id == cu_cp_ue_id_t::invalid) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping PDU", cu_cp_ue_id);
    return;
  }

  auto* ue = ue_manager.find_ue(cu_cp_ue_id);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping PDU", cu_cp_ue_id);
    return;
  }

  // Add AMF UE ID to ue ngap context if it is not set (this is the first DL NAS Transport message)
  if (ue->get_amf_ue_id() == amf_ue_id_t::invalid) {
    // Set AMF UE ID in the UE context and also in the lookup
    ue_manager.set_amf_ue_id(cu_cp_ue_id, uint_to_amf_ue_id(msg->amf_ue_ngap_id.value.value));
  }

  byte_buffer nas_pdu;
  nas_pdu.resize(msg->nas_pdu.value.size());
  std::copy(msg->nas_pdu.value.begin(), msg->nas_pdu.value.end(), nas_pdu.begin());

  ue->get_rrc_ue_pdu_notifier().on_new_pdu(std::move(nas_pdu));
}

void ngc_impl::handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request)
{
  cu_cp_ue_id_t cu_cp_ue_id = ue_manager.get_cu_cp_ue_id(uint_to_ran_ue_id(request->ran_ue_ngap_id.value));
  auto*         ue          = ue_manager.find_ue(cu_cp_ue_id);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping Initial Context Setup Request", cu_cp_ue_id);
    return;
  }

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    request.to_json(js);
    logger.debug("Received Initial Context Setup Request Message: {}", js.to_string());
  }

  // start procedure
  task_sched.schedule_async_task(cu_cp_ue_id,
                                 launch_async<ng_initial_context_setup_procedure>(*ue, request, ngc_notifier, logger));
}

void ngc_impl::handle_ue_context_release_command(const asn1::ngap::ue_context_release_cmd_s& cmd)
{
  std::underlying_type_t<amf_ue_id_t> amf_ue_id_uint = 0;
  if (cmd->ue_ngap_ids.value.type() == asn1::ngap::ue_ngap_ids_c::types_opts::amf_ue_ngap_id) {
    amf_ue_id_uint = cmd->ue_ngap_ids->amf_ue_ngap_id();
  } else if (cmd->ue_ngap_ids.value.type() == asn1::ngap::ue_ngap_ids_c::types_opts::ue_ngap_id_pair) {
    amf_ue_id_uint = cmd->ue_ngap_ids->ue_ngap_id_pair().amf_ue_ngap_id;
  }
  cu_cp_ue_id_t cu_cp_ue_id = ue_manager.get_cu_cp_ue_id(uint_to_amf_ue_id(amf_ue_id_uint));
  auto*         ue          = ue_manager.find_ue(cu_cp_ue_id);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping UE Context Release Command", cu_cp_ue_id);
    return;
  }

  // TODO: trigger RRC Connection Release and remove UE
}

void ngc_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp: {
      events->ng_setup_response.set(&outcome.value.ng_setup_resp());
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngc_impl::handle_unsuccessful_outcome(const unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ng_setup_fail: {
      events->ng_setup_response.set(&outcome.value.ng_setup_fail());
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

size_t ngc_impl::get_nof_ues() const
{
  return ue_manager.get_nof_ngc_ues();
}
