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
#include "procedures/ng_setup_procedure.h"
#include "srsgnb/support/async/event_signal.h"

using namespace srsgnb;
using namespace asn1::ngap;
using namespace srs_cu_cp;

ngc_impl::ngc_impl(timer_manager& timers_, ngc_message_notifier& ngc_notifier_, ngc_ue_manager& ue_manager_) :
  logger(srslog::fetch_basic_logger("NGC")),
  timers(timers_),
  ngc_notifier(ngc_notifier_),
  ue_manager(ue_manager_),
  events(std::make_unique<ngc_event_manager>())
{
  ng_setup_timer = timers.create_unique_timer();
  ng_setup_timer.set(1000, [](uint32_t tid) {
    // TODO
  });
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
  std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint = cu_cp_ue_id_to_uint(msg.cu_cp_ue_id);

  auto* ue = ue_manager.find_ue(cu_cp_ue_id_uint);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping Initial UE Message", cu_cp_ue_id_uint);
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
  std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint = cu_cp_ue_id_to_uint(msg.cu_cp_ue_id);

  auto* ue = ue_manager.find_ue(cu_cp_ue_id_uint);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping UL NAS Transport Message", cu_cp_ue_id_uint);
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
    logger.error("UE AMF ID for cu_cp_ue_id={} not found!", cu_cp_ue_id_uint);
    return;
  }
  ul_nas_transport_msg->amf_ue_ngap_id.value.value = amf_ue_id_to_uint(amf_ue_id);

  ul_nas_transport_msg->nas_pdu.value.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), ul_nas_transport_msg->nas_pdu.value.begin());

  auto& user_loc_info_nr       = ul_nas_transport_msg->user_location_info.value.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = msg.nr_cgi;
  user_loc_info_nr.tai.plmn_id = msg.nr_cgi.plmn_id;
  // TODO: Set tAC

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    ngc_msg.pdu.to_json(js);
    logger.debug("Containerized UL NAS Transport Message: {}", js.to_string());
  }

  // Forward message to AMF
  ngc_notifier.on_new_message(ngc_msg);
}

void ngc_impl::handle_initial_context_setup_response_message(const ngap_initial_context_setup_response_message& msg)
{
  ngc_message ngc_msg = {};

  auto* ue = ue_manager.find_ue(msg.cu_cp_ue_id);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist", msg.cu_cp_ue_id);
    return;
  }

  if (msg.success) {
    ngc_msg.pdu.set_successful_outcome();
    ngc_msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
    auto& init_ctxt_setup_resp                 = ngc_msg.pdu.successful_outcome().value.init_context_setup_resp();
    init_ctxt_setup_resp->amf_ue_ngap_id.value = amf_ue_id_to_uint(ue->get_amf_ue_id());
    init_ctxt_setup_resp->ran_ue_ngap_id.value = ran_ue_id_to_uint(ue->get_ran_ue_id());

    // Fill PDU Session Resource Setup Response List
    if (!msg.succeed_to_setup.empty()) {
      init_ctxt_setup_resp->pdu_session_res_setup_list_cxt_res_present = true;
      init_ctxt_setup_resp->pdu_session_res_setup_list_cxt_res->resize(msg.succeed_to_setup.size());
      for (auto& it : msg.succeed_to_setup) {
        asn1::ngap::pdu_session_res_setup_item_cxt_res_s res_item;
        res_item.pdu_session_id = it.pdu_session_id;
        res_item.pdu_session_res_setup_resp_transfer.resize(it.pdu_session_res.length());
        std::copy(
            it.pdu_session_res.begin(), it.pdu_session_res.end(), res_item.pdu_session_res_setup_resp_transfer.begin());

        init_ctxt_setup_resp->pdu_session_res_setup_list_cxt_res->push_back(res_item);
      }
    }

    // Fill PDU Session Resource Failed to Setup List
    if (!msg.failed_to_setup.empty()) {
      init_ctxt_setup_resp->pdu_session_res_failed_to_setup_list_cxt_res_present = true;
      init_ctxt_setup_resp->pdu_session_res_failed_to_setup_list_cxt_res->resize(msg.failed_to_setup.size());
      for (auto& it : msg.failed_to_setup) {
        asn1::ngap::pdu_session_res_failed_to_setup_item_cxt_res_s res_item;
        res_item.pdu_session_id = it.pdu_session_id;
        res_item.pdu_session_res_setup_unsuccessful_transfer.resize(it.pdu_session_res.length());
        std::copy(it.pdu_session_res.begin(),
                  it.pdu_session_res.end(),
                  res_item.pdu_session_res_setup_unsuccessful_transfer.begin());

        init_ctxt_setup_resp->pdu_session_res_failed_to_setup_list_cxt_res->push_back(res_item);
      }
    }

    // Fill Criticality Diagnostics
    if (msg.crit_diagnostics.has_value()) {
      init_ctxt_setup_resp->crit_diagnostics_present = true;
      init_ctxt_setup_resp->crit_diagnostics.value   = msg.crit_diagnostics.value();
    }
  } else {
    ngc_msg.pdu.set_unsuccessful_outcome();
    ngc_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
    auto& init_ctxt_setup_fail                 = ngc_msg.pdu.unsuccessful_outcome().value.init_context_setup_fail();
    init_ctxt_setup_fail->amf_ue_ngap_id.value = amf_ue_id_to_uint(ue->get_amf_ue_id());
    init_ctxt_setup_fail->ran_ue_ngap_id.value = ran_ue_id_to_uint(ue->get_ran_ue_id());

    // Fill Cause
    if (!msg.cause.has_value()) {
      logger.debug("Missing failure cause");
      return;
    }
    init_ctxt_setup_fail->cause.value = msg.cause.value();

    // Fill PDU Session Resource Failed to Setup List
    if (!msg.failed_to_setup.empty()) {
      init_ctxt_setup_fail->pdu_session_res_failed_to_setup_list_cxt_fail_present = true;
      init_ctxt_setup_fail->pdu_session_res_failed_to_setup_list_cxt_fail->resize(msg.failed_to_setup.size());
      for (auto& it : msg.failed_to_setup) {
        asn1::ngap::pdu_session_res_failed_to_setup_item_cxt_fail_s fail_item;
        fail_item.pdu_session_id = it.pdu_session_id;
        fail_item.pdu_session_res_setup_unsuccessful_transfer.resize(it.pdu_session_res.length());
        std::copy(it.pdu_session_res.begin(),
                  it.pdu_session_res.end(),
                  fail_item.pdu_session_res_setup_unsuccessful_transfer.begin());

        init_ctxt_setup_fail->pdu_session_res_failed_to_setup_list_cxt_fail->push_back(fail_item);
      }
    }

    // Fill Criticality Diagnostics
    if (msg.crit_diagnostics.has_value()) {
      init_ctxt_setup_fail->crit_diagnostics_present = true;
      init_ctxt_setup_fail->crit_diagnostics.value   = msg.crit_diagnostics.value();
    }
  }

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
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngc_impl::handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg)
{
  cu_cp_ue_id_t cu_cp_ue_id = ue_manager.get_cu_cp_ue_id(msg->ran_ue_ngap_id.value.value);
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
    ue->set_amf_ue_id(uint_to_amf_ue_id(msg->amf_ue_ngap_id.value.value));
  }

  byte_buffer nas_pdu;
  nas_pdu.resize(msg->nas_pdu.value.size());
  std::copy(msg->nas_pdu.value.begin(), msg->nas_pdu.value.end(), nas_pdu.begin());

  ue->get_rrc_ue_pdu_notifier().on_new_pdu(std::move(nas_pdu));
}

void ngc_impl::handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request)
{
  cu_cp_ue_id_t cu_cp_ue_id = ue_manager.get_cu_cp_ue_id(request->ran_ue_ngap_id.value);
  auto*         ue          = ue_manager.find_ue(cu_cp_ue_id);
  if (ue == nullptr) {
    logger.info("UE with cu_cp_ue_id={} does not exist. Dropping Initial Context Setup Request", cu_cp_ue_id);
    return;
  }

  ngap_initial_context_setup_request_message init_ctxt_setup_req_msg;
  init_ctxt_setup_req_msg.cu_cp_ue_id = cu_cp_ue_id;
  init_ctxt_setup_req_msg.request     = request;

  rrc_init_security_context sec_ctx = {};
  copy_asn1_key(sec_ctx.k, *request->security_key);
  fill_supported_integrity_algorithms(sec_ctx.supported_int_algos,
                                      request->ue_security_cap->nrintegrity_protection_algorithms);
  fill_supported_ciphering_algorithms(sec_ctx.supported_int_algos, request->ue_security_cap->nrencryption_algorithms);

  ue->get_rrc_ue_control_notifier().on_initial_context_setup_request_received(init_ctxt_setup_req_msg);
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
