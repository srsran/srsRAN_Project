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

void ngap_impl::handle_message(const ngap_message& msg)
{
  logger.info("Handling NGAP PDU of type \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));

  switch (msg.pdu.type().value) {
    case asn1::ngap::ngap_pdu_c::types_opts::init_msg:
      handle_initiating_message(msg.pdu.init_msg());
      break;
    case asn1::ngap::ngap_pdu_c::types_opts::successful_outcome:
      handle_successful_outcome(msg.pdu.successful_outcome());
      break;
    case asn1::ngap::ngap_pdu_c::types_opts::unsuccessful_outcome:
      handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
      break;
    default:
      logger.error("Invalid PDU type");
      break;
  }
}

void ngap_impl::handle_initiating_message(const asn1::ngap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngap_impl::handle_successful_outcome(const asn1::ngap::successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp: {
      events->ngap_setup_response.set(&outcome.value.ng_setup_resp());
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngap_impl::handle_unsuccessful_outcome(const asn1::ngap::unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::ngap::ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ng_setup_fail: {
      events->ngap_setup_response.set(&outcome.value.ng_setup_fail());
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}
