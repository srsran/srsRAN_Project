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

/// \file
/// \brief Interface of the upper-PHY notifier in charge of messages carrying the result of uplink detection and
/// decoding.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/upper/channel_processors/prach_detection_result.h"
#include "srsran/phy/upper/channel_processors/pucch_processor_result.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/uplink_processor_context.h"

namespace srsran {

/// PRACH results structure.
struct ul_prach_results {
  /// PRACH buffer context associated with this uplink PRACH results.
  prach_buffer_context context;
  /// PRACH detection result.
  prach_detection_result result;
};

/// Data-related PUSCH decoding output.
struct ul_pusch_results_data {
  /// Parameter \f$n_{RNTI}\f$ from TS38.211 Section 6.3.1.1.
  rnti_t rnti;
  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Channel state information.
  channel_state_information csi;
  /// HARQ process ID.
  unsigned harq_id;
  /// PUSCH decoder result.
  pusch_decoder_result decoder_result;
  /// \brief Data payload of the PUSCH PDU.
  ///
  /// \note The span is empty if the PDU does not contain PUSCH data or if the PDU could not be decoded successfully
  /// (i.e., CRC is KO).
  span<const uint8_t> payload;
};

/// Control-related PUSCH decoding output.
struct ul_pusch_results_control {
  /// Parameter \f$n_{RNTI}\f$ from TS38.211 Section 6.3.1.1.
  rnti_t rnti;
  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Channel state information.
  channel_state_information csi;
  /// HARQ ACK results.
  optional<pusch_uci_field> harq_ack;
  /// CSI Part 1 results.
  optional<pusch_uci_field> csi1;
  /// CSI Part 2 results.
  optional<pusch_uci_field> csi2;
};

/// PUCCH results structure.
struct ul_pucch_results {
  /// PUCCH context.
  ul_pucch_context context;
  /// PUCCH processor result.
  pucch_processor_result processor_result;
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

  /// \brief Notifies the results of a PUSCH control decodification.
  virtual void on_new_pusch_results_control(const ul_pusch_results_control& result) = 0;

  /// \brief Notifies the results of a PUSCH data decodification.
  virtual void on_new_pusch_results_data(const ul_pusch_results_data& result) = 0;

  /// \brief Notifies the results of a PUCCH decodification.
  virtual void on_new_pucch_results(const ul_pucch_results& result) = 0;
};

} // namespace srsran
