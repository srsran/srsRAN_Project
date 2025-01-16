/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sib_pdu_assembler.h"

using namespace srsran;

// Number of padding bytes to pre-reserve. This value is implementation-defined.
static constexpr unsigned MAX_PADDING_BYTES_LEN = 64;

sib_pdu_assembler::sib_pdu_assembler(const std::vector<byte_buffer>& bcch_dl_sch_payloads)
{
  bcch_payloads.resize(bcch_dl_sch_payloads.size());
  for (unsigned i = 0, e = bcch_payloads.size(); i != e; ++i) {
    bcch_payloads[i].info.version      = 0;
    bcch_payloads[i].info.payload_size = units::bytes(bcch_dl_sch_payloads[i].length());

    // Note: Resizing the bcch_payload is forbidden, to avoid vector memory relocations and invalidation of pointers
    // passed to the lower layers. For this reason, we pre-reserve any potential padding bytes.
    bcch_payloads[i].info.payload_and_padding.resize(bcch_dl_sch_payloads[i].length() + MAX_PADDING_BYTES_LEN, 0);
    std::copy(bcch_dl_sch_payloads[i].begin(),
              bcch_dl_sch_payloads[i].end(),
              bcch_payloads[i].info.payload_and_padding.begin());
  }
}

void sib_pdu_assembler::handle_new_sib1_payload(byte_buffer sib1_pdu)
{
  // Create new BCCH entry with a larger version.
  bcch_info new_bcch;
  new_bcch.version      = bcch_payloads[0].info.version + 1;
  new_bcch.payload_size = units::bytes(sib1_pdu.length());
  new_bcch.payload_and_padding.resize(sib1_pdu.length() + MAX_PADDING_BYTES_LEN, 0);
  std::copy(sib1_pdu.begin(), sib1_pdu.end(), new_bcch.payload_and_padding.begin());

  // Mark old BCCH-DL-SCH message for deferred destruction and insert new BCCH-DL-SCH message in its place.
  bcch_payloads[0].old  = std::move(bcch_payloads[0].info);
  bcch_payloads[0].info = std::move(new_bcch);
}

span<const uint8_t> sib_pdu_assembler::encode_sib1_pdu(unsigned si_version, units::bytes tbs_bytes)
{
  return encode_si_pdu(0, si_version, tbs_bytes);
}

span<const uint8_t>
sib_pdu_assembler::encode_si_message_pdu(unsigned si_msg_idx, unsigned si_version, units::bytes tbs_bytes)
{
  return encode_si_pdu(si_msg_idx + 1, si_version, tbs_bytes);
}

span<const uint8_t> sib_pdu_assembler::encode_si_pdu(unsigned idx, unsigned si_version, units::bytes tbs_bytes)
{
  static constexpr unsigned TX_COUNT_BEFORE_OLD_VERSION_REMOVAL = 4;

  auto& bcch = bcch_payloads[idx];

  if (bcch.info.version == si_version) {
    // In case there is no pending reconfig of the SI.
    srsran_assert(tbs_bytes >= bcch.info.payload_size,
                  "The allocated PDSCH TBS cannot be smaller than the respective SI{} payload",
                  idx == 0 ? fmt::format("B1") : fmt::format("-message {}", idx + 1));
    srsran_assert(tbs_bytes <= units::bytes(bcch.info.payload_and_padding.size()),
                  "Memory rellocations of the SIB1 payload not allowed. Consider reserving more bytes for PADDING");

    // In case the old version is pending for removal.
    bcch.info.nof_tx++;
    if (bcch.info.nof_tx == TX_COUNT_BEFORE_OLD_VERSION_REMOVAL and bcch.old.has_value()) {
      // It is safe to remove old version at this point.
      bcch.old.reset();
    }

    return span<const uint8_t>(bcch.info.payload_and_padding.data(), tbs_bytes.value());
  }

  if (bcch.old.has_value() and bcch.old.value().version == si_version) {
    // We need to send the old BCCH version instead.
    srsran_assert(tbs_bytes >= bcch.old->payload_size,
                  "The allocated PDSCH TBS cannot be smaller than the respective SI{} payload",
                  idx == 0 ? fmt::format("B1") : fmt::format("-message {}", idx + 1));
    srsran_assert(tbs_bytes <= units::bytes(bcch.old->payload_and_padding.size()),
                  "Memory rellocations of the SIB1 payload not allowed. Consider reserving more bytes for PADDING");

    return span<const uint8_t>(bcch.old->payload_and_padding.data(), tbs_bytes.value());
  }

  // No SI-message with matching index and version was found. Return empty.
  return span<const uint8_t>();
}
