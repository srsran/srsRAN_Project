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

/// PRACH results structure.
struct ul_prach_results {
  /// PRACH buffer context associated with this uplink PRACH results.
  prach_buffer_context context;
  /// PRACH detection result.
  prach_detection_result result;
};

/// PUSCH results and metadata.
struct ul_pusch_results {
  /// PUSCH data processing results, metadata and payload.
  struct pusch_data {
    /// Parameter \f$n_{RNTI}\f$ from TS38.211 Section 6.3.1.1.
    rnti_t rnti;
    /// HARQ process ID.
    unsigned harq_id;
    /// PUSCH decoder result.
    pusch_decoder_result decoder_result;
    /// \brief Data payload of the PUSCH PDU.
    ///
    /// \note The size of the span is zero if the PDU does not contain PUSCH data.
    span<const uint8_t> payload;
  };

  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Channel state information.
  channel_state_information csi;
  /// \brief PUSCH data
  ///
  ///
  /// \note This parameter is valid if the PUSCH PDU contains data, otherwise it is not.
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
