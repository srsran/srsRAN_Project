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
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/uci_status.h"
#include "srsran/phy/upper/channel_state_information.h"
#include "srsran/ran/uci/uci_constants.h"

namespace srsran {

/// Collects the results of UCI field decoding.
struct pusch_uci_field {
  static_vector<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> payload;
  uci_status                                                  status;

  /// Clears the contents of the field.
  void clear()
  {
    payload.clear();
    status = uci_status::unknown;
  }
};

/// Groups the PUSCH processor UL-SCH data decode results.
struct pusch_processor_result_data {
  /// SCH decoding information.
  pusch_decoder_result data;
  /// Channel state information.
  channel_state_information csi;
};

/// Groups the PUSCH processor UL-SCH control decode results.
struct pusch_processor_result_control {
  /// \brief Decoded HARQ-ACK information bits.
  /// \remark Leave empty if no HARQ-ACK information bits are multiplexed with the UL-SCH.
  pusch_uci_field harq_ack;
  /// \brief Decoded CSI Part 1 information bits.
  /// \remark Leave empty if no CSI Part 1 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part1;
  /// \brief Decoded CSI Part 2 information bits.
  /// \remark Leave empty if no CSI Part 2 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part2;
  /// Channel state information.
  channel_state_information csi;
};

/// \brief PUSCH processor result notifier.
///
/// The PUSCH processor uses this interface for notifying results.
class pusch_processor_result_notifier
{
public:
  /// Default destructor.
  virtual ~pusch_processor_result_notifier() = default;

  /// \brief Notifies the detection of Uplink Control Information (UCI) on PUSCH.
  ///
  /// The UCI result is always notified after on_csi(), if the \ref pusch_processor::uci_description contains any
  /// payload bits. Otherwise, it is not notified.
  ///
  /// \param[in] uci Actual UCI decoding results.
  virtual void on_uci(const pusch_processor_result_control& uci) = 0;

  /// \brief Notifies the shared channel decoding.
  ///
  /// The SCH result is always notified after on_csi(), if the \ref pusch_processor::pdu_t::codeword is
  /// available.
  ///
  /// \param[in] sch Actual SCH decoding results.
  virtual void on_sch(const pusch_processor_result_data& sch) = 0;
};

} // namespace srsran