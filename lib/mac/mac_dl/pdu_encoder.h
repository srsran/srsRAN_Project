/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/mac/mac_cell_manager.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// Class that manages the encoding of BCCH-DL-SCH messages to be fit in a Transport Block.
class sib_pdu_encoder
{
public:
  explicit sib_pdu_encoder(const byte_buffer& bcch_dl_sch_payload) : min_payload_size(bcch_dl_sch_payload.length())
  {
    // Note: Resizing the bcch_payload after the ctor is forbidden, to avoid vector memory relocations and invalidation
    // of pointers passed to the lower layers. For this reason, we pre-reserve any potential padding bytes.
    static constexpr unsigned MAX_PADDING_BYTES_LEN = 64;
    bcch_payload.resize(min_payload_size + MAX_PADDING_BYTES_LEN, 0);
    bcch_payload.assign(bcch_dl_sch_payload.begin(), bcch_dl_sch_payload.end());
  }

  span<const uint8_t> encode_sib_pdu(unsigned tbs_bytes) const
  {
    srsran_assert(tbs_bytes >= min_payload_size, "The TBS for SIB1 cannot be smaller than the SIB1 payload");
    srsran_assert(tbs_bytes <= bcch_payload.capacity(),
                  "Memory rellocations of the SIB1 payload not allowed. Consider reserving more bytes for PADDING");
    return span<const uint8_t>(bcch_payload.data(), tbs_bytes);
  }

private:
  /// Holds the original BCCH-DL-SCH message, defined in the MAC cell configuration, plus extra padding bytes.
  std::vector<uint8_t> bcch_payload;
  /// Length of the original BCCH-DL-SCH message, without padding, defined in the MAC cell configuration.
  unsigned min_payload_size;
};

inline void encode_rar_pdu(const mac_cell_creation_request& cell_cfg, const rar_information& rar, byte_buffer& pdu)
{
  // TODO
}

} // namespace srsgnb
