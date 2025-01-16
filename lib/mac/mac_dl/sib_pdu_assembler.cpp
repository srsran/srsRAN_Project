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
#include "srsran/srslog/srslog.h"

using namespace srsran;

// Number of padding bytes to pre-reserve. This value is implementation-defined.
static constexpr unsigned MAX_PADDING_BYTES_LEN = 64;

// Max SI Message PDU size. This value is implementation-defined.
static constexpr unsigned MAX_BCCH_DL_SCH_PDU_SIZE = 2048;

// Payload of zeros sent to when an error occurs.
static const std::vector<uint8_t> zeros_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0);

sib_pdu_assembler::sib_pdu_assembler(const std::vector<byte_buffer>& bcch_dl_sch_payloads) :
  logger(srslog::fetch_basic_logger("MAC"))
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

unsigned sib_pdu_assembler::handle_new_sib1_payload(byte_buffer sib1_pdu)
{
  unsigned version_id = bcch_payloads[0].info.version + 1;

  // Create new BCCH entry with a larger version.
  bcch_info new_bcch;
  new_bcch.version      = version_id;
  new_bcch.payload_size = units::bytes(sib1_pdu.length());
  new_bcch.payload_and_padding.resize(sib1_pdu.length() + MAX_PADDING_BYTES_LEN, 0);
  std::copy(sib1_pdu.begin(), sib1_pdu.end(), new_bcch.payload_and_padding.begin());

  // Mark old BCCH-DL-SCH message for deferred destruction and insert new BCCH-DL-SCH message in its place.
  bcch_payloads[0].old  = std::move(bcch_payloads[0].info);
  bcch_payloads[0].info = std::move(new_bcch);

  return version_id;
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
  srsran_assert(tbs_bytes.value() <= MAX_BCCH_DL_SCH_PDU_SIZE, "Invalid TBS size for an BCCH-DL-SCH message");

  auto& bcch = bcch_payloads[idx];

  if (bcch.info.version == si_version) {
    // In case there is no pending reconfig of the SI.

    // In case the old version is pending for removal.
    bcch.info.nof_tx++;
    if (bcch.info.nof_tx == TX_COUNT_BEFORE_OLD_VERSION_REMOVAL and bcch.old.has_value()) {
      // It is safe to remove old version at this point.
      bcch.old.reset();
    }

    return encode_bcch_pdu(idx, bcch.info, tbs_bytes);
  }

  if (bcch.old.has_value() and bcch.old.value().version == si_version) {
    // We need to send the old BCCH version instead.
    return encode_bcch_pdu(idx, bcch.old.value(), tbs_bytes);
  }

  // No SI-message with matching index and version was found. Return empty.
  return span<const uint8_t>(zeros_payload.data(), tbs_bytes.value());
}

span<const uint8_t> sib_pdu_assembler::encode_bcch_pdu(unsigned msg_idx, const bcch_info& bcch, units::bytes tbs) const
{
  if (tbs < bcch.payload_size) {
    logger.error("Failed to encode BCCH-DL-SCH Transport Block for SI{}. Cause: TBS cannot be smaller than the "
                 "respective message payload",
                 msg_idx == 0 ? fmt::format("B1") : fmt::format("-message {}", msg_idx + 1));
    return span<const uint8_t>(zeros_payload.data(), tbs.value());
  }
  if (tbs.value() > bcch.payload_and_padding.size()) {
    logger.error("Failed to encode BCCH-DL-SCH Transport Block for SI{}. Cause: Memory reallocations for payload are "
                 "not allowed. Consider reserving more bytes for PADDING",
                 msg_idx == 0 ? fmt::format("B1") : fmt::format("-message {}", msg_idx + 1));
    return span<const uint8_t>(zeros_payload.data(), tbs.value());
  }

  // Generation of TB was successful.
  return span<const uint8_t>(bcch.payload_and_padding.data(), tbs.value());
}
