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

#include "srsgnb/phy/upper/channel_processors/pusch_decoder_result.h"
#include "srsgnb/phy/upper/channel_state_information.h"

namespace srsgnb {

/// Groups the PUSCH processor results.
struct pusch_processor_result {
  /// Describes the UCI information.
  struct uci_indication {
    // TBD.
  };

  /// Channel state information.
  channel_state_information csi;
  /// SCH decoding information. Available only if the PUSCH transmission carried data.
  optional<pusch_decoder_result> data;
  /// Uplink Control Information. Available only if the PUSCH transmission carried UCI.
  optional<uci_indication> uci;
};

} // namespace srsgnb