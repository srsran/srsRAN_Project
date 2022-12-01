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
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/ngap/ngc_types.h"
#include "srsgnb/security/security.h"
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

inline expected<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::init_msg_s& init_msg)
{
  switch (init_msg.value.type()) {
    case asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg:
      return uint_to_ran_ue_id(init_msg.value.init_ue_msg()->ran_ue_ngap_id->value);
    default:
      break;
  }
  return {default_error_t{}};
}

inline expected<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::successful_outcome_s& success_outcome)
{
  return {default_error_t{}};
}

inline expected<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  return {default_error_t{}};
}

inline expected<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::ngap_pdu_c& pdu)
{
  using namespace asn1::ngap;
  switch (pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return get_ran_ue_id(pdu.init_msg());
    case ngap_pdu_c::types_opts::successful_outcome:
      return get_ran_ue_id(pdu.successful_outcome());
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return get_ran_ue_id(pdu.unsuccessful_outcome());
    default:
      break;
  }
  return {default_error_t{}};
}

inline void copy_asn1_key(security::sec_as_key& key_out, const asn1::fixed_bitstring<256, false, true>& key_in)
{
  memcpy(key_out.data(), key_in.data(), security::sec_key_len);
}

inline void fill_supported_integrity_algorithms(security::supported_integrity_algos&         supported_algos_out,
                                                const asn1::fixed_bitstring<16, true, true>& supported_algos_in)
{
  for (unsigned i = 0; i < 3; ++i) {
    supported_algos_out[i] = supported_algos_in.get(i);
  }
}

inline void fill_supported_ciphering_algorithms(security::supported_ciphering_algos&         supported_algos_out,
                                                const asn1::fixed_bitstring<16, true, true>& supported_algos_in)
{
  for (unsigned i = 0; i < 3; ++i) {
    supported_algos_out[i] = supported_algos_in.get(i);
  }
}
} // namespace srs_cu_cp
} // namespace srsgnb
