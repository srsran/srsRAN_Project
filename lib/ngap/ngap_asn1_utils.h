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
#include "srsran/asn1/asn1_utils.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/security/security.h"
#include "srsran/support/error_handling.h"

namespace asn1 {
namespace ngap {

struct cause_c;
struct init_msg_s;
struct successful_outcome_s;
struct unsuccessful_outcome_s;
struct ngap_pdu_c;

} // namespace ngap
} // namespace asn1

namespace srsran {
namespace srs_cu_cp {

/// Get string with NGAP error cause.
const char* get_cause_str(const asn1::ngap::cause_c& cause);

/// Extracts message type.
const char* get_message_type_str(const asn1::ngap::ngap_pdu_c& pdu);

/// Extracts RAN-UE-NGAP-ID from NGAP PDU
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::init_msg_s& init_msg);
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::successful_outcome_s& success_outcome);
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome);
std::optional<ran_ue_id_t> get_ran_ue_id(const asn1::ngap::ngap_pdu_c& pdu);

std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::init_msg_s& init_msg);
std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::successful_outcome_s& success_outcome);
std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::unsuccessful_outcome_s& unsuccessful_outcome);
std::optional<amf_ue_id_t> get_amf_ue_id(const asn1::ngap::ngap_pdu_c& pdu);

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
