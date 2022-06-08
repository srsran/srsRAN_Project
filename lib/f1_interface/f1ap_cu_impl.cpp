/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_cu_impl.h"
#include "../ran/gnb_format.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/support/async/event_signal.h"

using namespace srsgnb;
using namespace asn1::f1ap;
using namespace srs_cu_cp;

class srsgnb::srs_cu_cp::f1ap_cu_impl::f1ap_event_manager
{
public:
  /// F1SetupRequest receive.
  event_signal<asn1::f1ap::f1_setup_request_s> f1ap_setup_request;
};

f1ap_cu_impl::f1ap_cu_impl(f1ap_message_notifier& event_notifier_) :
  logger(srslog::fetch_basic_logger("F1AP")), event_notifier(event_notifier_)
{}

// Note: For fwd declaration of member types, dtor cannot be trivial.
f1ap_cu_impl::~f1ap_cu_impl() {}

void f1ap_cu_impl::handle_f1ap_setup_response(const f1_setup_response_message& msg)
{
  if (msg.success) {
    // TODO send response
    logger.info("Transmitting F1SetupResponse message");
  } else {
    // TODO send failure
    logger.info("Transmitting F1SetupFailure message");
  }
}

void f1ap_cu_impl::handle_init_ul_rrc_message_transfer(const asn1::f1ap::init_ulrrc_msg_transfer_s& msg)
{
  logger.info("Received F1AP initial UL RRC msg.");

  // TODO: handle init ul rrc message
}

void f1ap_cu_impl::handle_ul_rrc_message_transfer(const asn1::f1ap::ulrrc_msg_transfer_s& msg)
{
  logger.info("Received F1AP UL RRC msg.");

  // TODO: handle ul rrc message
}

void f1ap_cu_impl::handle_message(const asn1::f1ap::f1_ap_pdu_c& pdu)
{
  logger.info("Handling F1AP PDU of type {}", pdu.type().to_string());

  switch (pdu.type().value) {
    case asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg:
      handle_initiating_message(pdu.init_msg());
      break;
    default:
      logger.error("Invalid PDU type");
      break;
  }
}

void f1ap_cu_impl::handle_initiating_message(const asn1::f1ap::init_msg_s& msg)
{
  f1_setup_request_message req_msg = {};
  switch (msg.value.type().value) {
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::options::f1_setup_request:
      req_msg.request = msg.value.f1_setup_request();
      event_notifier.on_f1_setup_request_received(req_msg);
      break;
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::init_ulrrc_msg_transfer:
      handle_init_ul_rrc_message_transfer(msg.value.init_ulrrc_msg_transfer());
      break;
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ulrrc_msg_transfer:
      handle_ul_rrc_message_transfer(msg.value.ulrrc_msg_transfer());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}
