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

#include "srsgnb/adt/optional.h"
#include "srsgnb/phy/upper/channel_processors/pusch_decoder_result.h"
#include "srsgnb/phy/upper/channel_processors/uci_status.h"
#include "srsgnb/phy/upper/channel_state_information.h"
#include "srsgnb/ran/uci/uci_constants.h"

namespace srsgnb {

/// Collects the results of UCI field decoding.
struct pusch_uci_field {
  static_vector<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> payload;
  uci_status                                                  status;
};

/// Groups the PUSCH processor results.
struct pusch_processor_result {
  /// Channel state information.
  channel_state_information csi;
  /// Error vector Magnitude (EVM) calculated from the modulation demapper soft bits.
  optional<float> evm;
  /// SCH decoding information. Available only if the PUSCH transmission carried data.
  optional<pusch_decoder_result> data;
  /// Decoded HARQ-ACK information bits. Leave empty if no HARQ-ACK information bits are multiplexed with the UL-SCH.
  pusch_uci_field harq_ack;
  /// Decoded CSI-Part1 information bits. Leave empty if no CSI-Part1 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part1;
  /// Decoded CSI-Part2 information bits. Leave empty if no CSI-Part2 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part2;
};

} // namespace srsgnb