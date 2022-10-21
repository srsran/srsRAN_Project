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
class pucch_uci_message
{
public:
  /// Default constructor. It sets the status to unknown all number of bits to zero.
  pucch_uci_message() = default;

  /// \brief Create and initialise PUCCH UCI message from the number of bits of each of the fields.
  /// \param[in] nof_sr        Number of payload bits for Scheduling Request (SR).
  /// \param[in] nof_harq_ack  Number of payload bits for HARQ-ACK feedback.
  /// \param[in] nof_csi_part1 Number of payload bits for CSI-Part1 report.
  /// \param[in] nof_csi_part2 Number of payload bits for CSI-Part2 report.
  /// \remark An assertion is triggered if the total number of payload bits exceed \ref
  /// uci_constants::MAX_NOF_PAYLOAD_BITS.
  pucch_uci_message(unsigned nof_sr, unsigned nof_harq_ack, unsigned nof_csi_part1, unsigned nof_csi_part2) :
    nof_sr_bits(nof_sr),
    nof_harq_ack_bits(nof_harq_ack),
    nof_csi_part1_bits(nof_csi_part1),
    nof_csi_part2_bits(nof_csi_part2)
  {
    srsgnb_assert(nof_sr + nof_harq_ack + nof_csi_part1 + nof_csi_part2 < uci_constants::MAX_NOF_PAYLOAD_BITS,
                  "The total number of payload bits (i.e. {}) shall not exceed {}.",
                  nof_sr + nof_harq_ack + nof_csi_part1 + nof_csi_part2,
                  uci_constants::MAX_NOF_PAYLOAD_BITS);
  }

  /// Sets the message status.
  void set_status(uci_status status_) { status = status_; }

  /// Gets the message status.
  uci_status get_status() const { return status; }

  /// Get a read-write view of the full payload.
  span<uint8_t> get_full_payload()
  {
    return span<uint8_t>(data).first(nof_sr_bits + nof_harq_ack_bits + nof_csi_part1_bits + nof_csi_part2_bits);
  }

  /// Get a read-only view of the full payload.
  span<const uint8_t> get_full_payload() const
  {
    return span<const uint8_t>(data).first(nof_sr_bits + nof_harq_ack_bits + nof_csi_part1_bits + nof_csi_part2_bits);
  }

  /// Get a read-write view of the SR bits.
  span<uint8_t> get_sr_bits() { return span<uint8_t>(data).subspan(0, nof_sr_bits); }

  /// Get a read-only view of the SR bits.
  span<const uint8_t> get_sr_bits() const { return span<const uint8_t>(data).subspan(0, nof_sr_bits); }

  /// Get a read-write view of the HARQ-ACK bits.
  span<uint8_t> get_harq_ack_bits() { return span<uint8_t>(data).subspan(nof_sr_bits, nof_harq_ack_bits); }

  /// Get a read-only view of the HARQ-ACK bits.
  span<const uint8_t> get_harq_ack_bits() const
  {
    return span<const uint8_t>(data).subspan(nof_sr_bits, nof_harq_ack_bits);
  }

  /// Get a read-write view of the CSI-Part1 bits.
  span<uint8_t> get_csi_part1_bits()
  {
    return span<uint8_t>(data).subspan(nof_sr_bits + nof_harq_ack_bits, nof_csi_part1_bits);
  }

  /// Get a read-only view of the CSI-Part1 bits.
  span<const uint8_t> get_csi_part1_bits() const
  {
    return span<const uint8_t>(data).subspan(nof_sr_bits + nof_harq_ack_bits, nof_csi_part1_bits);
  }

  /// Get a read-write view of the CSI-Part1 bits.
  span<uint8_t> get_csi_part2_bits()
  {
    return span<uint8_t>(data).subspan(nof_sr_bits + nof_harq_ack_bits + nof_csi_part1_bits, nof_csi_part2_bits);
  }

  /// Get a read-only view of the CSI-Part1 bits.
  span<const uint8_t> get_csi_part2_bits() const
  {
    return span<const uint8_t>(data).subspan(nof_sr_bits + nof_harq_ack_bits + nof_csi_part1_bits, nof_csi_part2_bits);
  }

private:
  /// Payload data storage.
  std::array<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> data = {};
  /// Number of SR bits.
  unsigned nof_sr_bits = 0;
  /// Number of HARQ-ACK bits.
  unsigned nof_harq_ack_bits = 0;
  /// Number of CSI-Part1 bits.
  unsigned nof_csi_part1_bits = 0;
  /// Number of CSI-Part2 bits.
  unsigned nof_csi_part2_bits = 0;
  /// Message decode or detection status.
  uci_status status = uci_status::invalid;
};

} // namespace srsgnb
