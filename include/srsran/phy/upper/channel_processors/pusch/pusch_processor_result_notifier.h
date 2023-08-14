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

#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/uci_status.h"
#include "srsran/ran/uci/uci_constants.h"

namespace srsran {

class channel_state_information;

/// Collects the results of UCI field decoding.
struct pusch_uci_field {
  static_vector<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> payload;
  uci_status                                                  status;
};

/// Groups the PUSCH processor UL-SCH data decode results.
struct pusch_processor_result_data {
  /// Error vector Magnitude (EVM) calculated from the modulation demapper soft bits.
  optional<float> evm;
  /// SCH decoding information.
  pusch_decoder_result data;
};

/// Groups the PUSCH processor UL-SCH control decode results.
struct pusch_processor_result_control {
  /// Error vector Magnitude (EVM) calculated from the modulation demapper soft bits.
  optional<float> evm;
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

/// \brief PUSCH processor result notifier.
///
/// The PUSCH processor uses this interface for notifying results.
class pusch_processor_result_notifier
{
public:
  /// Default destructor.
  virtual ~pusch_processor_result_notifier() = default;

  /// \brief Notifies the Channel State Information (CSI) measurement.
  ///
  /// This result is always notified once.
  ///
  /// \param[in] csi Actual measurement.
  virtual void on_csi(const channel_state_information& csi) = 0;

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