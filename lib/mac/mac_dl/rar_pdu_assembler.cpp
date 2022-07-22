/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rar_pdu_assembler.h"

using namespace srsgnb;

/// Encoder of RAR PDUs.
class rar_pdu_encoder
{
public:
  explicit rar_pdu_encoder(const rar_information& rar_info_) : rar_info(rar_info_) {}

  void encode(std::vector<uint8_t>& output_buf);

private:
  /// Encodes RAR UL Grant into provided payload as per TS38.321 6.1.5 and 6.2.
  void encode_rar_subpdu(const rar_ul_grant& grant, bool is_last_subpdu);

  /// \brief Encodes RAR subPDU subheader as per TS38.321 6.1.5 and 6.2.2.
  void encode_rapid_subheader(uint16_t rapid, bool is_last_subpdu);

  /// Encodes RAR UL Grant into provided payload as per TS38.321 6.2.3. and as per TS38.213 8.2.
  void encode_rar_grant_payload(const rar_ul_grant& grant);

  const rar_information& rar_info;
  uint8_t*               ptr = nullptr;
};

void rar_pdu_encoder::encode(std::vector<uint8_t>& output_buf)
{
  // See TS38.321, Section 6.2.3.
  static constexpr unsigned MAC_RAR_SUBHEADER_AND_PAYLOAD_LENGTH = 8;
  output_buf.clear();
  output_buf.resize(MAC_RAR_SUBHEADER_AND_PAYLOAD_LENGTH * rar_info.grants.size());
  ptr = output_buf.data();

  for (unsigned i = 0; i != rar_info.grants.size(); ++i) {
    encode_rar_subpdu(rar_info.grants[i], i == rar_info.grants.size() - 1);
  }
  srsran_sanity_check(ptr == output_buf.data() + output_buf.size(), "Encoded RAR PDU length differs from expected");
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
  *ptr = (grant.mcs & 0xfU) << 4U;
  // encode TPC command for PUSCH (3 bits).
  *ptr |= (grant.tpc & 0x7U) << 1U;
  // encode CSI request (1 bit).
  *ptr |= grant.csi_req ? 1U : 0U;
  ++ptr;

  // Encode Temporary C-RNTI (2 Octets).
  *ptr = (grant.temp_crnti >> 8U) & 0xffU;
  ++ptr;
  *ptr = grant.temp_crnti & 0xffU;
  ++ptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rar_pdu_assembler::rar_pdu_assembler(const mac_cell_creation_request& cell_cfg_) : cell_cfg(cell_cfg_)
{
  // Pre-reserve a pool of byte vectors, where RAR PDUs will be encoded in a round-robin fashion.
  // Note: The ring has to be large enough to accommodate all the RARs in a given slot, for a sufficiently large
  // number of slots.
  static const size_t GRID_NOF_SUBFRAMES = 20;
  rar_payload_ring_buffer.resize(GRID_NOF_SUBFRAMES * get_nof_slots_per_subframe(cell_cfg.scs_common) * MAX_GRANTS);
}

span<const uint8_t> rar_pdu_assembler::encode_rar_pdu(const rar_information& rar)
{
  srsran_assert(not rar.grants.empty(), "Cannot encode RAR without UL grants");

  // Fetch output vector where RAR grant payload is going to be encoded.
  std::vector<uint8_t>& payload = rar_payload_ring_buffer[next_index++];
  // Pre-reserve space for RAR and padding bits.
  payload.reserve(rar.pdsch_cfg.codewords[0].tb_size_bytes);

  // Encode RAR PDU.
  rar_pdu_encoder pdu_encoder{rar};
  pdu_encoder.encode(payload);

  // Encode Padding.
  if (payload.size() < rar.pdsch_cfg.codewords[0].tb_size_bytes) {
    payload.resize(rar.pdsch_cfg.codewords[0].tb_size_bytes, 0);
  }

  return {payload};
}
