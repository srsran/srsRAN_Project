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

#include "cell_sys_info_configurator.h"
#include "srsran/mac/cell_configuration.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Entity responsible for fetching encoded SIB1 and SI messages based on scheduled SI grants.
class sib_pdu_assembler : public cell_sys_info_configurator
{
  static constexpr size_t MAX_SI_MESSAGES = 32;

public:
  class message_handler
  {
  public:
    virtual ~message_handler() = default;

    virtual si_version_type update(si_version_type si_version, const byte_buffer& pdu) = 0;

    /// Retrieve encoded SI bytes for a given SI scheduling opportunity.
    virtual span<const uint8_t> get_pdu(slot_point sl_tx, const sib_information& si_info) = 0;
  };

  sib_pdu_assembler(const mac_cell_sys_info_config& req);

  /// Update the SIB1 and SI messages.
  si_change_result handle_si_change_request(const mac_cell_sys_info_config& req) override;

  /// \brief Retrieve the encoded SI message.
  span<const uint8_t> encode_si_pdu(slot_point sl_tx, const sib_information& si_info);

private:
  void update_encoders(si_version_type version, const mac_cell_sys_info_config& req);

  srslog::basic_logger& logger;

  /// SIB1 message.
  std::unique_ptr<message_handler> sib1_encoder;

  /// SI message handlers.
  std::array<std::unique_ptr<message_handler>, MAX_SI_MESSAGES> si_encoders;

  /// Counter of SI PDU updates.
  si_version_type next_version = 0;
};

} // namespace srsran
