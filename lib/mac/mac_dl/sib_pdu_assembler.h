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

namespace srsran {

/// Class that manages the encoding of BCCH-DL-SCH messages to be fit in a Transport Block.
class sib_pdu_assembler
{
public:
  explicit sib_pdu_assembler(const std::vector<byte_buffer>& bcch_dl_sch_payloads);

  void handle_new_sib1_payload(byte_buffer sib1_pdu);

  span<const uint8_t> encode_sib1_pdu(unsigned si_version, units::bytes tbs_bytes);

  span<const uint8_t> encode_si_message_pdu(unsigned si_msg_idx, unsigned si_version, units::bytes tbs_bytes);

private:
  struct bcch_info {
    unsigned version = 0;
    unsigned nof_tx  = 0;
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

  // Holds the original BCCH-DL-SCH messages, defined in the MAC cell configuration.
  std::vector<bcch_context> bcch_payloads;
};

} // namespace srsran
