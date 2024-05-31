/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
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

  /// \brief Creates a data-related decoding result produced from a discarded transmission.
  /// \param rnti_   Parameter \f$n_{RNTI}\f$ from TS38.211 Section 6.3.1.1.
  /// \param slot_   Slot description (also specifies the numerology).
  /// \param harq_id HARQ process ID.
  /// \return The data-related PUSCH processing results without CSI measurements and marked as failed transmission.
  static ul_pusch_results_data create_discarded(rnti_t rnti_, slot_point slot_, unsigned harq_id_)
  {
    ul_pusch_results_data ret;
    ret.rnti           = rnti_;
    ret.slot           = slot_;
    ret.csi            = channel_state_information();
    ret.harq_id        = harq_id_;
    ret.decoder_result = pusch_decoder_result();
    ret.payload        = span<const uint8_t>();
    return ret;
  }
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
  std::optional<pusch_uci_field> harq_ack;
  /// CSI Part 1 results.
  std::optional<pusch_uci_field> csi1;
  /// CSI Part 2 results.
  std::optional<pusch_uci_field> csi2;

  /// \brief Creates a control-related result produced from a discarded transmission.
  /// \param rnti_        Parameter \f$n_{RNTI}\f$ from TS38.211 Section 6.3.1.1.
  /// \param slot_        Slot description (also specifies the numerology).
  /// \param nof_harq_ack Number of HARQ-ACK feedback bits.
  /// \return The control-related PUSCH processing results without CSI measurements, and the HARQ-ACK feedback payload
  /// status is unknown.
  static ul_pusch_results_control create_discarded(rnti_t rnti_, slot_point slot_, unsigned nof_harq_ack)
  {
    ul_pusch_results_control ret;
    ret.rnti = rnti_;
    ret.slot = slot_;
    ret.csi  = channel_state_information();
    ret.harq_ack.emplace(pusch_uci_field{uci_payload_type(nof_harq_ack), uci_status::unknown});
    ret.csi1 = std::nullopt;
    ret.csi2 = std::nullopt;
    return ret;
  }
};

/// PUCCH results structure.
struct ul_pucch_results {
  /// PUCCH context.
  ul_pucch_context context;
  /// PUCCH processor result.
  pucch_processor_result processor_result;

  /// \brief Creates a PUCCH result structure result produced from a discarded transmission.
  /// \param context_     Discarded PUCCH context.
  /// \param nof_harq_ack Number of HARQ-ACK feedback bits.
  /// \return The PUCCH processing results without CSI measurements, and the HARQ-ACK feedback payload status is
  /// unknown.
  static ul_pucch_results create_discarded(const ul_pucch_context& context_, unsigned nof_harq_ack)
  {
    ul_pucch_results ret;
    ret.context                  = context_;
    ret.processor_result.csi     = channel_state_information();
    ret.processor_result.message = pucch_uci_message({0, nof_harq_ack, 0, 0});
    ret.processor_result.message.set_status(uci_status::unknown);
    ret.processor_result.detection_metric = std::nullopt;
    return ret;
  }
};

/// Sounding Reference Signal results structure.
struct ul_srs_results {
  /// SRS context.
  ul_srs_context context;
  /// SRS channel estimation result.
  srs_estimator_result processor_result;
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

  /// \brief Notifies the results of a Sounding Reference Signals channel estimation.
  virtual void on_new_srs_results(const ul_srs_results& result) = 0;
};

} // namespace srsran
