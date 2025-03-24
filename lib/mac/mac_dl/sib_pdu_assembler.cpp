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
#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/units.h"

using namespace srsran;

// Max SI Message PDU size. This value is implementation-defined.
static constexpr unsigned MAX_BCCH_DL_SCH_PDU_SIZE = 2048;

// Payload of zeros sent to when an error occurs.
static const std::vector<uint8_t> zeros_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0);

namespace {

struct bcch_message_buffer {
  unsigned             version = 0;
  units::bytes         length{0};
  std::vector<uint8_t> payload;

  bcch_message_buffer() = default;

  void set(si_version_type version_, const byte_buffer& pdu)
  {
    srsran_assert(pdu.length() <= MAX_BCCH_DL_SCH_PDU_SIZE, "Invalid SI PDU length");
    // Note: Resizing the bcch_payload after this point onwards is forbidden to avoid vector memory relocations and
    // invalidation of pointers passed to the lower layers. For this reason, we pre-reserve space for any potential
    // padding bytes.
    payload.resize(MAX_BCCH_DL_SCH_PDU_SIZE, 0);
    unsigned len = copy_segments(pdu, payload);
    if (len < length.value()) {
      // Zero-fill previously set bytes.
      std::fill(payload.begin() + len, payload.begin() + length.value(), 0);
    }
    length  = units::bytes{len};
    version = version_;
  }
};

class versioned_bcch_dl_sch_message_encoder final : public sib_pdu_assembler::message_handler
{
public:
  versioned_bcch_dl_sch_message_encoder(std::optional<si_message_index_type> si_msg_index_,
                                        si_version_type                      si_version,
                                        const byte_buffer&                   pdu,
                                        srslog::basic_logger&                logger_) :
    si_msg_index(si_msg_index_), logger(logger_)
  {
    current.set(si_version, pdu);
  }

  span<const uint8_t> get_pdu(slot_point sl_tx, const sib_information& si_info) override
  {
    unsigned tbs = si_info.pdsch_cfg.codewords[0].tb_size_bytes;
    srsran_assert(tbs <= MAX_BCCH_DL_SCH_PDU_SIZE, "BCCH-DL-SCH is too long. Revisit constant");

    if (current.version != si_info.version) {
      // Current SI message version is too old. Fetch new version from shared buffer.
      bcch_message_buffer& next = pending.read();
      if (next.version == current.version) {
        // This SI message, in particular, has not been updated. Bump its version.
        next.version    = si_info.version;
        current.version = si_info.version;
      } else {
        // The SI message was updated. Take PDU from the shared buffer.
        if (next.version != si_info.version) {
          // PDU in shared buffer does not match in version requested. This should not happen.
          logger.warning("SI message version mismatch. Expected: {}, got: {}", si_info.version, next.version);
          next.version = si_info.version;
        }
        current.version = next.version;
        current.length  = next.length;
        std::swap(next.payload, current.payload);
      }
    }

    srsran_sanity_check(current.payload.size() >= tbs, "Invalidation of SI message buffer detected");
    if (current.length.value() > tbs) {
      logger.error("Failed to encode BCCH-DL-SCH Transport Block for SI{}. Cause: TBS cannot be smaller than the "
                   "respective message payload",
                   not si_msg_index.has_value() ? fmt::format("B1") : fmt::format("-message {}", si_msg_index.value()));
    }
    return span<const uint8_t>(current.payload).first(tbs);
  }

  si_version_type update(si_version_type si_version, const byte_buffer& pdu) override
  {
    bcch_message_buffer& next = pending.write();
    next.set(si_version, pdu);
    pending.commit();
    return si_version;
  }

private:
  std::optional<si_message_index_type> si_msg_index;
  srslog::basic_logger&                logger;

  bcch_message_buffer current;

  lockfree_triple_buffer<bcch_message_buffer> pending;
};

} // namespace

sib_pdu_assembler::sib_pdu_assembler() : logger(srslog::fetch_basic_logger("MAC")) {}

span<const uint8_t> sib_pdu_assembler::encode_si_pdu(slot_point sl_tx, const sib_information& si_info)
{
  if (si_info.si_indicator == sib_information::si_indicator_type::sib1) {
    return sib1_encoder->get_pdu(sl_tx, si_info);
  }

  srsran_assert(si_info.si_msg_index.has_value(), "Invalid SI message index");
  if (si_encoders[si_info.si_msg_index.value()] == nullptr) {
    logger.error("Invalid SI message index {}", si_info.si_msg_index.value());
    return span<const uint8_t>{zeros_payload}.first(si_info.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  return si_encoders[si_info.si_msg_index.value()]->get_pdu(sl_tx, si_info);
}

si_change_result sib_pdu_assembler::handle_si_change_request(const si_change_request& req)
{
  // Prepare response.
  last_resp.version++;
  last_resp.sib1_len = units::bytes{static_cast<unsigned>(req.sib1.length())};
  for (const auto& rem : req.si_messages_to_rem) {
    auto it = std::find_if(
        last_resp.si_msg_lens.begin(), last_resp.si_msg_lens.end(), [rem](const auto& s) { return s.first == rem; });
    if (it != last_resp.si_msg_lens.end()) {
      last_resp.si_msg_lens.erase(it);
    }
  }
  for (const auto& addmod : req.si_messages_to_addmod) {
    const units::bytes si_len{static_cast<unsigned>(addmod.second.length())};
    auto               it = std::find_if(last_resp.si_msg_lens.begin(),
                           last_resp.si_msg_lens.end(),
                           [si_idx = addmod.first](const auto& s) { return s.first == si_idx; });
    if (it != last_resp.si_msg_lens.end()) {
      it->second = si_len;
    } else {
      last_resp.si_msg_lens.emplace_back(addmod.first, si_len);
    }
  }
  std::sort(last_resp.si_msg_lens.begin(), last_resp.si_msg_lens.end());

  // Update SIB1.
  si_version_type version = last_resp.version;
  if (sib1_encoder == nullptr) {
    sib1_encoder = std::make_unique<versioned_bcch_dl_sch_message_encoder>(std::nullopt, version, req.sib1, logger);
  } else {
    sib1_encoder->update(version, req.sib1);
  }

  // TODO: dealloc SI messages to remove
  // Note: This is tricky because it can only be done when the PHY has finished using the SI message.

  // Update SI message contents.
  for (const auto& si_msg : req.si_messages_to_addmod) {
    if (si_encoders[si_msg.first] == nullptr) {
      si_encoders[si_msg.first] =
          std::make_unique<versioned_bcch_dl_sch_message_encoder>(si_msg.first, version, si_msg.second, logger);
    } else {
      si_encoders[si_msg.first]->update(version, si_msg.second);
    }
  }

  return last_resp;
}
