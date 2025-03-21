/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/units.h"

namespace srsran {

using si_version_type       = unsigned;
using si_message_index_type = unsigned;

/// Handler of BCCH-DL-SCH PDU encoding.
class bcch_dl_sch_message_encoder
{
public:
  virtual ~bcch_dl_sch_message_encoder() = default;

  /// Retrieve encoded SI bytes for a given SI scheduling opportunity.
  virtual span<const uint8_t> get_pdu(slot_point sl_tx, const sib_information& si_info) = 0;
};

/// SI message update request.
struct si_change_request {
  /// \brief New SIB1 payload.
  /// \remark It contains the si-SchedulingInfo for the UE to acquire other SI messages.
  byte_buffer sib1;
  /// \brief SI messages that are part of the si-SchedulingInfo broadcast by SIB1 that have been added or modified.
  std::vector<std::pair<si_message_index_type, byte_buffer>> si_messages_to_addmod;
  /// \brief SI messages that are to be removed.
  std::vector<si_message_index_type> si_messages_to_rem;
};

/// SI message update outcome.
struct si_change_response {
  /// Current SI version.
  si_version_type version = 0;
  /// Length of SIB1.
  units::bytes sib1_len{0};
  /// SI message lengths.
  slotted_id_vector<si_message_index_type, units::bytes> si_msg_lens;
};

class bcch_dl_sch_message_broker final : public bcch_dl_sch_message_encoder
{
  static constexpr size_t MAX_SI_MESSAGES = 32;

public:
  class message_handler : public bcch_dl_sch_message_encoder
  {
  public:
    virtual si_version_type update(si_version_type si_version, const byte_buffer& pdu) = 0;
  };

  bcch_dl_sch_message_broker();

  /// Update the SIB1 and SI messages.
  si_change_response handle_si_change_request(const si_change_request& req);

  /// \brief Retrieve the encoded SI message.
  span<const uint8_t> get_pdu(slot_point sl_tx, const sib_information& si_info) override;

private:
  srslog::basic_logger& logger;

  /// SIB1 message.
  std::unique_ptr<message_handler> sib1_encoder;

  /// SI message handlers.
  std::array<std::unique_ptr<message_handler>, MAX_SI_MESSAGES> si_encoders;

  si_change_response last_resp;
};

} // namespace srsran
