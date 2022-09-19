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
#include "procedures/ngap_setup_procedure.h"
#include "srsgnb/support/async/event_signal.h"

using namespace srsgnb;
using namespace asn1::ngap;
using namespace srs_cu_cp;

ngc_impl::ngc_impl(timer_manager& timers_, ngc_message_notifier& message_notifier_) :
  logger(srslog::fetch_basic_logger("NGAP")),
  timers(timers_),
  ngc_notifier(message_notifier_),
  events(std::make_unique<ngap_event_manager>())
{
  ngap_ue_context empty_context = {};
  std::fill(ue_ngap_id_to_ngap_ue_context.begin(), ue_ngap_id_to_ngap_ue_context.end(), empty_context);

  ng_setup_timer = timers.create_unique_timer();
  ng_setup_timer.set(1000, [](uint32_t tid) {
    // TODO
  });
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
ngc_impl::~ngc_impl() {}

async_task<ng_setup_response_message> ngc_impl::handle_ngap_setup_request(const ng_setup_request_message& request)
{
  return launch_async<ngap_setup_procedure>(request, ngc_notifier, *events, logger);
}

void ngc_impl::handle_initial_ue_message(const ngap_initial_ue_message& msg)
{
  logger.info("Handling Initial UE Message");

  // Create UE context and store it
  ngap_ue_context ue_ctxt = {};
  ue_ctxt.du_index        = get_du_index_from_ue_ngap_id(msg.ue_ngap_id);
  ue_ctxt.ue_index        = get_ue_index_from_ue_ngap_id(msg.ue_ngap_id);

  logger.debug(
      "Created NGAP UE (ue_ngap_id={}, du_index={}, ue_index={}).", msg.ue_ngap_id, ue_ctxt.du_index, ue_ctxt.ue_index);

  ngc_message ngc_msg = {};
  ngc_msg.pdu.set_init_msg();
  ngc_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_UE_MSG);

  auto& init_ue_msg = ngc_msg.pdu.init_msg().value.init_ue_msg();

  init_ue_msg->ran_ue_ngap_id.value.value = ue_ngap_id_to_uint(msg.ue_ngap_id);

  init_ue_msg->nas_pdu.value.resize(msg.nas_pdu.length());
  std::copy(msg.nas_pdu.begin(), msg.nas_pdu.end(), init_ue_msg->nas_pdu.value.begin());

  init_ue_msg->rrcestablishment_cause.value.value = msg.establishment_cause.value;

  auto& user_loc_info_nr       = init_ue_msg->user_location_info.value.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = msg.nr_cgi;
  user_loc_info_nr.tai.plmn_id = msg.nr_cgi.plmn_id;
  // TODO: Set tAC

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
  logger.info("Handling UL NAS Transport Message");

  ngc_message ngc_msg = {};
  ngc_msg.pdu.set_init_msg();
  ngc_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UL_NAS_TRANSPORT);

  auto& ul_nas_transport_msg = ngc_msg.pdu.init_msg().value.ul_nas_transport();

  std::underlying_type_t<ue_ngap_id_t> ue_ngap_id_uint = ue_ngap_id_to_uint(msg.ue_ngap_id);
  ul_nas_transport_msg->ran_ue_ngap_id.value.value     = ue_ngap_id_uint;

  if (ue_ngap_id_to_ngap_ue_context[ue_ngap_id_uint].amf_ue_id == ue_amf_id_t::invalid) {
    logger.error("UE AMF ID for ue_ngap_id={} not found!", ue_ngap_id_uint);
    return;
  }

  ue_amf_id_t ue_amf_id                            = ue_ngap_id_to_ngap_ue_context[ue_ngap_id_uint].amf_ue_id;
  ul_nas_transport_msg->amf_ue_ngap_id.value.value = ue_amf_id_to_uint(ue_amf_id);

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
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngc_impl::handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg)
{
  std::underlying_type_t<ue_ngap_id_t> ue_ngap_id_uint = msg->ran_ue_ngap_id.value.value;

  // Add AMF UE ID to ue ngap context if it is not set (this is the first DL NAS Transport message)
  if (ue_ngap_id_to_ngap_ue_context[ue_ngap_id_uint].amf_ue_id == ue_amf_id_t::invalid) {
    ue_ngap_id_to_ngap_ue_context[ue_ngap_id_uint].amf_ue_id = uint_to_ue_amf_id(msg->amf_ue_ngap_id.value.value);
  }

  // TODO: create NGAP to RRC UE adapter and forward message
}

void ngc_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp: {
      events->ngap_setup_response.set(&outcome.value.ng_setup_resp());
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngc_impl::handle_unsuccessful_outcome(const unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ng_setup_fail: {
      events->ngap_setup_response.set(&outcome.value.ng_setup_fail());
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}
