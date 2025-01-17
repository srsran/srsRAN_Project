/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/mac/mac_cell_manager.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Class that manages the encoding of BCCH-DL-SCH messages to be fit in a Transport Block.
class sib_pdu_assembler
{
public:
  explicit sib_pdu_assembler(const std::vector<byte_buffer>& bcch_dl_sch_payloads);

  /// Stores a new SIB1 PDU to be broadcast.
  /// \return Returns the version ID of the new PDU.
  unsigned handle_new_sib1_payload(const byte_buffer& sib1_pdu);

  span<const uint8_t> encode_sib1_pdu(unsigned si_version, units::bytes tbs_bytes);

  span<const uint8_t> encode_si_message_pdu(unsigned si_msg_idx, unsigned si_version, units::bytes tbs_bytes);

private:
  struct bcch_info {
    /// Current version of the BCCH-DL-SCH message.
    unsigned version = 0;
    /// Number of times this BCCH-DL-SCH message was sent to lower layers.
    unsigned nof_tx = 0;
    /// Length of the original BCCH-DL-SCH message, without padding, defined in the MAC cell configuration.
    units::bytes         payload_size;
    std::vector<uint8_t> payload_and_padding;
  };
  struct bcch_context {
    /// Holds the most recent BCCH-DL-SCH message version.
    bcch_info info;
    /// Holds old version of the BCCH-DL-SCH message that may still being used by lower layers.
    std::optional<bcch_info> old;
  };

  span<const uint8_t> encode_si_pdu(unsigned idx, unsigned si_version, units::bytes tbs_bytes);

  span<const uint8_t> encode_bcch_pdu(unsigned msg_idx, const bcch_info& bcch, units::bytes tbs) const;

  srslog::basic_logger& logger;

  // Holds the currently configured BCCH-DL-SCH messages.
  std::vector<bcch_context> bcch_payloads;
};

} // namespace srsran
