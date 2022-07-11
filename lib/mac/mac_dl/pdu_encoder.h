/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_MAC_MAC_DL_PDU_ENCODER_H
#define SRSGNB_LIB_MAC_MAC_DL_PDU_ENCODER_H

#include "srsgnb/mac/mac_cell_manager.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// Class that manages the encoding of BCCH-DL-SCH messages to be fit in a Transport Block.
class sib_pdu_encoder
{
public:
  explicit sib_pdu_encoder(const byte_buffer& bcch_dl_sch_payload) :
    bcch_payload(bcch_dl_sch_payload.copy()), min_payload_size(bcch_payload.length())
  {
  }

  const byte_buffer& encode_sib_pdu(unsigned tbs_bytes)
  {
    srsran_assert(tbs_bytes >= min_payload_size, "The TBS for SIB1 cannot be smaller than the SIB1 payload");
    int diff_bytes = tbs_bytes - bcch_payload.length();
    // The allocated TBS is different from the stored BCCH-DL-SCH message length. We need to set padding.
    if (diff_bytes != 0) {
      // Reassign new byte_buffer (Copy-on-write), to avoid concurrency with any other place that is sharing
      // this byte_buffer.
      bcch_payload = bcch_payload.deep_copy();
      if (diff_bytes > 0) {
        // Append more padding bytes.
        for (int i = 0; i < diff_bytes; ++i) {
          bcch_payload.append(0);
        }
      } else {
        // Remove some of the existing padding bytes.
        bcch_payload.trim_tail(-diff_bytes);
      }
    }
    return bcch_payload;
  }

private:
  /// Holds the original BCCH-DL-SCH message, defined in the MAC cell configuration, plus extra padding bytes.
  byte_buffer bcch_payload;
  /// Length of the original BCCH-DL-SCH message, without padding, defined in the MAC cell configuration.
  unsigned min_payload_size;
};

inline void encode_rar_pdu(const mac_cell_creation_request& cell_cfg, const rar_information& rar, byte_buffer& pdu)
{
  // TODO
}

} // namespace srsgnb

#endif // SRSGNB_LIB_MAC_MAC_DL_PDU_ENCODER_H
