/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"

namespace srsgnb {

/// Get string with F1AP error cause.
inline const char* get_cause_str(const asn1::f1ap::cause_c& cause)
{
  using namespace asn1::f1ap;
  switch (cause.type()) {
    case cause_c::types_opts::radio_network:
      return cause.radio_network().to_string();
    case cause_c::types_opts::transport:
      return cause.transport().to_string();
    case cause_c::types_opts::protocol:
      return cause.protocol().to_string();
    case cause_c::types_opts::misc:
      return cause.misc().to_string();
    default:
      break;
  }
  return "unknown";
}

/// Extracts message type.
inline const char* get_message_type_str(const asn1::f1ap::f1_ap_pdu_c& pdu)
{
  switch (pdu.type().value) {
    case asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg:
      return pdu.init_msg().value.type().to_string();
    case asn1::f1ap::f1_ap_pdu_c::types_opts::successful_outcome:
      return pdu.successful_outcome().value.type().to_string();
    case asn1::f1ap::f1_ap_pdu_c::types_opts::unsuccessful_outcome:
      return pdu.unsuccessful_outcome().value.type().to_string();
    default:
      break;
  }
  srsgnb_terminate("Invalid F1AP PDU type \"{}\"", pdu.type().to_string());
}

/// Extracts transaction id of Initiating message.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::init_msg_s& out)
{
  using namespace asn1::f1ap;
  switch (out.value.type().value) {
    case f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request:
      return out.value.f1_setup_request()->transaction_id->value;
    case f1_ap_elem_procs_o::init_msg_c::types_opts::gnbcu_cfg_upd:
      return out.value.gnbcu_cfg_upd()->transaction_id->value;
    case f1_ap_elem_procs_o::init_msg_c::types_opts::gnbdu_cfg_upd:
      return out.value.gnbdu_cfg_upd()->transaction_id->value;
    case f1_ap_elem_procs_o::init_msg_c::types_opts::f1_removal_request:
      return (*out.value.f1_removal_request())[0]->transaction_id();
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Successful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::successful_outcome_s& out)
{
  using namespace asn1::f1ap;
  switch (out.value.type().value) {
    case f1_ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp:
      return out.value.f1_setup_resp()->transaction_id->value;
    case f1_ap_elem_procs_o::successful_outcome_c::types_opts::gnbcu_cfg_upd_ack:
      return out.value.gnbcu_cfg_upd_ack()->transaction_id->value;
    case f1_ap_elem_procs_o::successful_outcome_c::types_opts::gnbdu_cfg_upd_ack:
      return out.value.gnbdu_cfg_upd_ack()->transaction_id->value;
    case f1_ap_elem_procs_o::successful_outcome_c::types_opts::f1_removal_resp:
      return out.value.f1_removal_resp()->transaction_id->value;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Unsuccessful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::unsuccessful_outcome_s& out)
{
  using namespace asn1::f1ap;
  switch (out.value.type().value) {
    case f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail:
      return out.value.f1_setup_fail()->transaction_id->value;
    case f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnbcu_cfg_upd_fail:
      return out.value.gnbcu_cfg_upd_fail()->transaction_id->value;
    case f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnbdu_cfg_upd_fail:
      return out.value.gnbdu_cfg_upd_fail()->transaction_id->value;
    case f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_removal_fail:
      return out.value.f1_removal_fail()->transaction_id->value;
      // TODO: Remaining cases.
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of F1AP PDU.
inline expected<uint8_t> get_transaction_id(const asn1::f1ap::f1_ap_pdu_c& pdu)
{
  using namespace asn1::f1ap;
  switch (pdu.type().value) {
    case f1_ap_pdu_c::types_opts::init_msg:
      return get_transaction_id(pdu.init_msg());
    case f1_ap_pdu_c::types_opts::successful_outcome:
      return get_transaction_id(pdu.successful_outcome());
    case f1_ap_pdu_c::types_opts::unsuccessful_outcome:
      return get_transaction_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return {default_error_t{}};
}

} // namespace srsgnb
