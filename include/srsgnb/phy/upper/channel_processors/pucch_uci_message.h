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

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_processors/uci_status.h"
#include "srsgnb/ran/uci/uci_constants.h"

namespace srsgnb {

/// \brief Collects the uplink control information message.
///
/// As the total number of bits is limited to \ref uci_constants::MAX_NOF_PAYLOAD_BITS, the UCI payload storage is
/// common for all fields and each of the fields point to a view of the common storage \c data.
///
/// The payload size of each field is determined by the field view size. A field is not present if it is empty. The
/// total payload size does not exceed \c uci_constants::MAX_NOF_PAYLOAD_BITS.
struct pucch_uci_message {
  /// Message decode or detection status.
  uci_status status;
  /// Payload data storage.
  std::array<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> data;
  /// View of the full payload.
  span<uint8_t> full_payload;
  /// View of the Scheduling Request payload.
  span<uint8_t> sr;
  /// View of the HARQ-ACK payload.
  span<uint8_t> harq_ack;
  /// View of the CSI part 1 payload.
  span<uint8_t> csi_part1;
};

} // namespace srsgnb
