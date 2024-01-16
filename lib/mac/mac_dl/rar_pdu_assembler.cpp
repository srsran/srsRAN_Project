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

#include "rar_pdu_assembler.h"

using namespace srsran;

namespace {

/// Encoder of RAR PDUs.
class rar_pdu_encoder
{
public:
  explicit rar_pdu_encoder(const rar_information& rar_info_) : rar_info(rar_info_) {}

  void encode(span<uint8_t> output_buf);

private:
  /// Encodes RAR UL Grant into provided payload as per TS38.321 6.1.5 and 6.2.
  void encode_rar_subpdu(const rar_ul_grant& grant, bool is_last_subpdu);

  /// \brief Encodes RAR subPDU subheader as per TS38.321 6.1.5 and 6.2.2.
  void encode_rapid_subheader(uint16_t rapid, bool is_last_subpdu);

  /// Encodes RAR UL Grant into provided payload as per TS38.321 6.2.3. and TS38.213 8.2.
  void encode_rar_grant_payload(const rar_ul_grant& grant);

  const rar_information& rar_info;
  uint8_t*               ptr = nullptr;
};

void rar_pdu_encoder::encode(span<uint8_t> output_buf)
{
  // See TS38.321, Section 6.2.3.
  static constexpr unsigned MAC_RAR_SUBHEADER_AND_PAYLOAD_LENGTH = 8;
  srsran_assert(output_buf.size() >= MAC_RAR_SUBHEADER_AND_PAYLOAD_LENGTH * rar_info.grants.size(),
                "Output buffer is too small to fit encoded RAR");
  ptr = output_buf.data();

  for (unsigned i = 0; i != rar_info.grants.size(); ++i) {
    encode_rar_subpdu(rar_info.grants[i], i == rar_info.grants.size() - 1);
  }
  srsran_sanity_check(ptr <= output_buf.data() + output_buf.size(), "Encoded RAR PDU length differs from expected");

  // Pad with zeros.
  std::fill(ptr, output_buf.data() + output_buf.size(), 0);
}

void rar_pdu_encoder::encode_rar_subpdu(const rar_ul_grant& grant, bool is_last_subpdu)
{
  // Encode subheader.
  encode_rapid_subheader(grant.rapid, is_last_subpdu);

  // Encode payload.
  encode_rar_grant_payload(grant);
}

void rar_pdu_encoder::encode_rapid_subheader(uint16_t rapid, bool is_last_subpdu)
{
  static const unsigned RAPID_FLAG = 1;

  // write E/T/RAPID MAC subheader.
  *ptr = (uint8_t)((not is_last_subpdu ? 1U : 0U) << 7U) | (RAPID_FLAG << 6U) | ((uint8_t)rapid & 0x3fU);
  ptr++;
}

void rar_pdu_encoder::encode_rar_grant_payload(const rar_ul_grant& grant)
{
  // Encode TA (12 bits).
  // high 7 bits of TA go into first octet.
  *ptr = ((grant.ta >> 5U) & 0x7fU);
  ptr++;
  *ptr = ((grant.ta & 0x1fU) << 3U);

  // Encode UL grant (27 bits) as per TS38.213 Table 8.2-1.
  // encode Frequency hopping flag (1 bit).
  *ptr |= ((grant.freq_hop_flag ? 1U : 0U) << 2U);
  // encode PUSCH frequency resource allocation (14 bits).
  *ptr |= (grant.freq_resource_assignment >> (14U - 2U) & 0x3U); // first 2 bits.
  ++ptr;
  *ptr = ((grant.freq_resource_assignment >> (14U - 2U - 8U)) & 0xffU); // middle 8 bits.
  ++ptr;
  *ptr = (grant.freq_resource_assignment & 0xfU) << 4U; // last 4 bits.
  // encode PUSCH time resource allocation (4 bits).
  *ptr |= (grant.time_resource_assignment & 0xfU);
  ++ptr;
  // encode MCS (4 bits).
  *ptr = (grant.mcs.to_uint() & 0xfU) << 4U;
  // encode TPC command for PUSCH (3 bits).
  *ptr |= (grant.tpc & 0x7U) << 1U;
  // encode CSI request (1 bit).
  *ptr |= grant.csi_req ? 1U : 0U;
  ++ptr;

  // Encode Temporary C-RNTI (2 Octets).
  *ptr = (to_value(grant.temp_crnti) >> 8U) & 0xffU;
  ++ptr;
  *ptr = to_value(grant.temp_crnti) & 0xffU;
  ++ptr;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rar_pdu_assembler::rar_pdu_assembler(ticking_ring_buffer_pool& pdu_pool_) : pdu_pool(pdu_pool_) {}

span<const uint8_t> rar_pdu_assembler::encode_rar_pdu(const rar_information& rar)
{
  srsran_assert(not rar.grants.empty(), "Cannot encode RAR without UL grants");

  // Fetch PDU buffer where RAR grant payload is going to be encoded.
  span<uint8_t> pdu_bytes = pdu_pool.allocate_buffer(rar.pdsch_cfg.codewords[0].tb_size_bytes);

  // Encode RAR PDU.
  rar_pdu_encoder pdu_encoder{rar};
  pdu_encoder.encode(pdu_bytes);

  return pdu_bytes;
}
