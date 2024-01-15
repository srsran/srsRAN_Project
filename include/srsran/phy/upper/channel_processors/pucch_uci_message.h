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

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_processors/uci/uci_status.h"
#include "srsran/ran/uci/uci_constants.h"

namespace srsran {

/// \brief Collects the uplink control information message.
///
/// As the total number of bits is limited to \ref uci_constants::MAX_NOF_PAYLOAD_BITS, the UCI payload storage is
/// common for all fields and each of the fields point to a view of the common storage \c data.
///
/// The payload size of each field is determined by the field view size. A field is not present if it is empty. The
/// total payload size does not exceed \c uci_constants::MAX_NOF_PAYLOAD_BITS.
class pucch_uci_message
{
public:
  /// Default constructor: sets the status to unknown and all number of bits to zero.
  pucch_uci_message() = default;

  /// Collects the number of information bits for each of the PUCCH message fields.
  struct configuration {
    /// Number of Scheduling Request (SR) information bits.
    unsigned nof_sr;
    /// Number of HARQ-ACK information bits.
    unsigned nof_harq_ack;
    /// Number of CSI Part 1 information bits.
    unsigned nof_csi_part1;
    /// Number of CSI Part 2 information bits.
    unsigned nof_csi_part2;
  };

  /// \brief Creates and initializes a PUCCH UCI message from the number of bits of each of the fields.
  /// \param[in] config Configuration parameters.
  /// \remark An assertion is triggered if the total number of payload bits exceed \ref
  /// uci_constants::MAX_NOF_PAYLOAD_BITS.
  explicit pucch_uci_message(const configuration& config) :
    nof_sr_bits(config.nof_sr),
    nof_harq_ack_bits(config.nof_harq_ack),
    nof_csi_part1_bits(config.nof_csi_part1),
    nof_csi_part2_bits(config.nof_csi_part2)
  {
    srsran_assert(config.nof_sr + config.nof_harq_ack + config.nof_csi_part1 + config.nof_csi_part2 <=
                      uci_constants::MAX_NOF_PAYLOAD_BITS,
                  "The total number of payload bits (i.e. {}) shall not exceed {}.",
                  config.nof_sr + config.nof_harq_ack + config.nof_csi_part1 + config.nof_csi_part2,
                  uci_constants::MAX_NOF_PAYLOAD_BITS);
  }

  /// Sets the message status.
  void set_status(uci_status status_) { status = status_; }

  /// Gets the message status.
  uci_status get_status() const { return status; }

  /// \brief Gets a read-write view of the full payload.
  /// \note The UCI payload fields are arranged following TS38.212 Section 6.3.1.1.3.
  span<uint8_t> get_full_payload()
  {
    return span<uint8_t>(data).first(nof_sr_bits + nof_harq_ack_bits + nof_csi_part1_bits + nof_csi_part2_bits);
  }

  /// \brief Gets a read-only view of the full payload.
  /// \note The UCI payload fields are arranged following TS38.212 Section 6.3.1.1.3.
  span<const uint8_t> get_full_payload() const
  {
    return span<const uint8_t>(data).first(nof_sr_bits + nof_harq_ack_bits + nof_csi_part1_bits + nof_csi_part2_bits);
  }

  /// Returns the number of expected Scheduling Request (SR) information bits.
  unsigned get_expected_nof_sr_bits() const { return nof_sr_bits; }

  /// Returns the number of expected HARQ-ACK information bits.
  unsigned get_expected_nof_harq_ack_bits() const { return nof_harq_ack_bits; }

  /// Returns the number of expected of CSI Part 1 information bits.
  unsigned get_expected_nof_csi_part1_bits() const { return nof_csi_part1_bits; }

  /// Returns the number of expected of CSI Part 2 information bits.
  unsigned get_expected_nof_csi_part2_bits() const { return nof_csi_part2_bits; }

  /// Gets a read-write view of the SR bits.
  span<uint8_t> get_sr_bits() { return span<uint8_t>(data).subspan(nof_harq_ack_bits, nof_sr_bits); }

  /// Gets a read-only view of the SR bits.
  span<const uint8_t> get_sr_bits() const { return span<const uint8_t>(data).subspan(nof_harq_ack_bits, nof_sr_bits); }

  /// Gets a read-write view of the HARQ-ACK bits.
  span<uint8_t> get_harq_ack_bits() { return span<uint8_t>(data).subspan(0, nof_harq_ack_bits); }

  /// Gets a read-only view of the HARQ-ACK bits.
  span<const uint8_t> get_harq_ack_bits() const { return span<const uint8_t>(data).subspan(0, nof_harq_ack_bits); }

  /// Gets a read-write view of the CSI Part 1 bits.
  span<uint8_t> get_csi_part1_bits()
  {
    return span<uint8_t>(data).subspan(nof_sr_bits + nof_harq_ack_bits, nof_csi_part1_bits);
  }

  /// Gets a read-only view of the CSI Part 1 bits.
  span<const uint8_t> get_csi_part1_bits() const
  {
    return span<const uint8_t>(data).subspan(nof_harq_ack_bits + nof_sr_bits, nof_csi_part1_bits);
  }

  /// Gets a read-write view of the CSI Part 1 bits.
  span<uint8_t> get_csi_part2_bits()
  {
    return span<uint8_t>(data).subspan(nof_harq_ack_bits + nof_sr_bits + nof_csi_part1_bits, nof_csi_part2_bits);
  }

  /// Gets a read-only view of the CSI Part 1 bits.
  span<const uint8_t> get_csi_part2_bits() const
  {
    return span<const uint8_t>(data).subspan(nof_harq_ack_bits + nof_sr_bits + nof_csi_part1_bits, nof_csi_part2_bits);
  }

private:
  /// Payload data storage.
  std::array<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> data;
  /// Number of SR bits.
  unsigned nof_sr_bits = 0;
  /// Number of HARQ-ACK bits.
  unsigned nof_harq_ack_bits = 0;
  /// Number of CSI Part 1 bits.
  unsigned nof_csi_part1_bits = 0;
  /// Number of CSI Part 2 bits.
  unsigned nof_csi_part2_bits = 0;
  /// Message decode or detection status.
  uci_status status = uci_status::invalid;
};

} // namespace srsran
