/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_impl.h"
#include "procedures/ngap_setup_procedure.h"
#include "srsgnb/support/async/event_signal.h"

using namespace srsgnb;
using namespace asn1::ngap;
using namespace srs_cu_cp;

ngap_impl::ngap_impl(timer_manager& timers_, ng_message_notifier& message_notifier_) :
  logger(srslog::fetch_basic_logger("NGAP")),
  timers(timers_),
  ng_notifier(message_notifier_),
  events(std::make_unique<ngap_event_manager>())
{
  ng_setup_timer = timers.create_unique_timer();
  ng_setup_timer.set(1000, [](uint32_t tid) {
    // TODO
  });
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
ngap_impl::~ngap_impl() {}

async_task<ng_setup_response_message> ngap_impl::handle_ngap_setup_request(const ng_setup_request_message& request)
{
  return launch_async<ngap_setup_procedure>(request, ng_notifier, *events, logger);
}

void ngap_impl::handle_initial_ue_message(const ngap_initial_ue_message& msg)
{
  logger.info("Handling Initial UE Message");

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_UE_MSG);

  auto& init_ue_msg = ngap_msg.pdu.init_msg().value.init_ue_msg();

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
    ngap_msg.pdu.to_json(js);
    logger.debug("Containerized Initial UE Message: {}", js.to_string());
  }

  // Forward message to AMF
  ng_notifier.on_new_message(ngap_msg);
}

void ngap_impl::handle_message(const ngap_message& msg)
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

void ngap_impl::handle_initiating_message(const init_msg_s& msg)
{
  switch (msg.value.type().value) {
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngap_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp: {
      events->ngap_setup_response.set(&outcome.value.ng_setup_resp());
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngap_impl::handle_unsuccessful_outcome(const unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ng_setup_fail: {
      events->ngap_setup_response.set(&outcome.value.ng_setup_fail());
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}
