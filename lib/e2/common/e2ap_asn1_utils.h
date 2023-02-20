/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/e2ap/e2ap.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

/// Extracts transaction id of Initiating message.
inline expected<uint8_t> get_transaction_id(const asn1::e2ap::init_msg_s& out)
{
  using namespace asn1::e2ap;
  switch (out.value.type().value) {
    case e2_ap_elem_procs_o::init_msg_c::types_opts::e2setup_request:
      return out.value.e2setup_request()->transaction_id.value.value;
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Successful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::e2ap::successful_outcome_s& out)
{
  using namespace asn1::e2ap;
  switch (out.value.type().value) {
    case e2_ap_elem_procs_o::successful_outcome_c::types_opts::e2setup_resp:
      return out.value.e2setup_resp()->transaction_id.value.value;
      break;
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of Unsuccessful Outcome message.
inline expected<uint8_t> get_transaction_id(const asn1::e2ap::unsuccessful_outcome_s& out)
{
  using namespace asn1::e2ap;
  switch (out.value.type().value) {
    case e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::e2setup_fail:
      return out.value.e2setup_fail()->transaction_id.value.value;
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts transaction id of E2AP PDU.
inline expected<uint8_t> get_transaction_id(const asn1::e2ap::e2_ap_pdu_c& pdu)
{
  using namespace asn1::e2ap;
  switch (pdu.type().value) {
    case e2_ap_pdu_c::types_opts::init_msg:
      return get_transaction_id(pdu.init_msg());
    case e2_ap_pdu_c::types_opts::successful_outcome:
      return get_transaction_id(pdu.successful_outcome());
    case e2_ap_pdu_c::types_opts::unsuccessful_outcome:
      return get_transaction_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return {default_error_t{}};
}

/// Extracts message type.
inline const char* get_message_type_str(const asn1::e2ap::e2_ap_pdu_c& pdu)
{
  switch (pdu.type().value) {
    case asn1::e2ap::e2_ap_pdu_c::types_opts::init_msg:
      return pdu.init_msg().value.type().to_string();
    case asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome:
      return pdu.successful_outcome().value.type().to_string();
    case asn1::e2ap::e2_ap_pdu_c::types_opts::unsuccessful_outcome:
      return pdu.unsuccessful_outcome().value.type().to_string();
    default:
      break;
  }
  report_fatal_error("Invalid E2AP PDU type \"{}\"", pdu.type().to_string());
}
} // namespace srsgnb
