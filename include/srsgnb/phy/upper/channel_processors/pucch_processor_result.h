/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/phy/upper/channel_processors/pucch_uci_message.h"
#include "srsgnb/phy/upper/channel_processors/pusch_decoder_result.h"
#include "srsgnb/phy/upper/channel_state_information.h"

namespace srsgnb {

/// Collects PUCCH processor results.
struct pucch_processor_result {
  /// Channel state information.
  channel_state_information csi;
  /// UCI message.
  pucch_uci_message message;
  /// Detection metric normalized with respect to the detection threshold (if applicable).
  optional<float> detection_metric;
};

} // namespace srsgnb
