/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/support/error_handling.h"

namespace srsran {

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
} // namespace srsran
