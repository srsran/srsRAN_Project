/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Interface of the upper-PHY notifier in charge of messages carrying the result of uplink detection and
/// decoding.

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/upper/channel_processors/prach_detection_result.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor_result.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

/// \brief PRACH results structure.
struct ul_prach_results {
  /// PRACH buffer context associated with this uplink PRACH results.
  prach_buffer_context context;
  /// PRACH detection result.
  prach_detection_result result;
};

/// \brief PUSCH results structure.
struct ul_pusch_results {
  struct pusch_data {
    /// Provides \f$n_{RNTI}\f$ from TS 38.211 section 6.3.1.1 Scrambling.
    rnti_t rnti;
    /// HARQ process ID.
    unsigned harq_id;
    /// PUSCH decoder result.
    pusch_decoder_result decoder_result;
    /// Data payload of the PUSCH PDU.
    ///
    /// Note: the size of the span is zero if the PDU does not contain PUSCH data.
    span<const uint8_t> payload;
  };

  /// Indicates the slot and numerology.
  slot_point slot;
  /// Channel state information.
  channel_state_information csi;
  /// Contains the results for the data PUSCH.
  optional<pusch_data> data;
};

/// \brief Interface of the upper-PHY notifier in charge of messages carrying the result of uplink detection and
/// decoding.
class upper_phy_rx_results_notifier
{
public:
  /// Default destructor.
  virtual ~upper_phy_rx_results_notifier() = default;

  /// \brief Notifies the results of a PRACH preamble detection.
  virtual void on_new_prach_results(const ul_prach_results& result) = 0;

  /// \brief Notifies the results of a PUSCH decodification.
  virtual void on_new_pusch_results(const ul_pusch_results& result) = 0;
};

} // namespace srsgnb
