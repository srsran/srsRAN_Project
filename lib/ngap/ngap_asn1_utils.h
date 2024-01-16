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
#include "srsran/asn1/ngap/ngap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/security/security.h"
#include "srsran/support/error_handling.h"

namespace srsran {
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
      return uint_to_ran_ue_id(init_msg.value.init_ue_msg()->ran_ue_ngap_id);
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

inline void copy_asn1_key(security::sec_key& key_out, const asn1::fixed_bitstring<256, false, true>& key_in)
{
  for (uint32_t i = 0; i < key_in.nof_octets(); ++i) {
    key_out[i] = key_in.data()[key_in.nof_octets() - 1 - i];
  }
}

inline void fill_supported_algorithms(security::supported_algorithms&              supported_algos_out,
                                      const asn1::fixed_bitstring<16, true, true>& supported_algos_in)
{
  uint16_t tmp = *reinterpret_cast<const uint16_t*>(supported_algos_in.data());
  for (uint16_t i = 0; i < 3; ++i) {
    supported_algos_out[i] = ((tmp >> (15 - i)) & 0x01U) == 1; // interesting bits are in the second byte
  }
}

} // namespace srs_cu_cp
} // namespace srsran
