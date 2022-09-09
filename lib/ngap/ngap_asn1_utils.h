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
#include "srsgnb/asn1/ngap.h"
#include "srsgnb/ngap/ngap_types.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Get string with NGAP error cause.
inline const char* get_cause_str(const asn1::ngap::cause_c& cause)
{
  using namespace asn1::ngap;
  switch (cause.type()) {
    case cause_c::types_opts::radio_network:
      return cause.radio_network().to_string();
    case cause_c::types_opts::transport:
      return cause.transport().to_string();
    case cause_c::types_opts::nas:
      return cause.nas().to_string();
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
inline const char* get_message_type_str(const asn1::ngap::ngap_pdu_c& pdu)
{
  using namespace asn1::ngap;
  switch (pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return pdu.init_msg().value.type().to_string();
    case ngap_pdu_c::types_opts::successful_outcome:
      return pdu.successful_outcome().value.type().to_string();
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return pdu.unsuccessful_outcome().value.type().to_string();
    default:
      break;
  }
  report_fatal_error("Invalid NGAP PDU type \"{}\"", pdu.type().to_string());
}

inline expected<ue_ngap_id_t> get_ue_ngap_id(const asn1::ngap::init_msg_s& init_msg)
{
  switch (init_msg.value.type()) {
    case asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg:
      return uint_to_ue_ngap_id(init_msg.value.init_ue_msg()->ran_ue_ngap_id->value);
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<ue_ngap_id_t> get_ue_ngap_id(const asn1::ngap::successful_outcome_s& success_outcome)
{
  return {default_error_t{}};
}

inline expected<ue_ngap_id_t> get_ue_ngap_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  return {default_error_t{}};
}

inline expected<ue_ngap_id_t> get_ue_ngap_id(const asn1::ngap::ngap_pdu_c& pdu)
{
  using namespace asn1::ngap;
  switch (pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return get_ue_ngap_id(pdu.init_msg());
    case ngap_pdu_c::types_opts::successful_outcome:
      return get_ue_ngap_id(pdu.successful_outcome());
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return get_ue_ngap_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return {default_error_t{}};
}

} // namespace srs_cu_cp
} // namespace srsgnb
