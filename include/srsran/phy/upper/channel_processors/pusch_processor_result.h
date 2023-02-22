/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/phy/upper/channel_processors/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/uci_status.h"
#include "srsran/phy/upper/channel_state_information.h"
#include "srsran/ran/uci/uci_constants.h"

namespace srsran {

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
  /// \brief SCH decoding information.
  /// \remark Available only if the PUSCH transmission carries data.
  optional<pusch_decoder_result> data;
  /// \brief Decoded HARQ-ACK information bits.
  /// \remark Leave empty if no HARQ-ACK information bits are multiplexed with the UL-SCH.
  pusch_uci_field harq_ack;
  /// \brief Decoded CSI Part 1 information bits.
  /// \remark Leave empty if no CSI Part 1 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part1;
  /// \brief Decoded CSI Part 2 information bits.
  /// \remark Leave empty if no CSI Part 2 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part2;
};

} // namespace srsran