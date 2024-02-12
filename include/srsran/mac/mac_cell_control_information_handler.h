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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/uci/uci_constants.h"
#include "srsran/ran/uci/uci_mapping.h"

namespace srsran {

/// \brief CRC indication for a given PDU.
struct mac_crc_pdu {
  /// RNTI value corresponding to the UE that generated this PDU.
  rnti_t rnti;
  /// HARQ process ID.
  uint8_t harq_id;
  /// True if transport block is successfully decoded, otherwise false.
  bool tb_crc_success;
  /// PUSCH SINR, in dBs.
  optional<float> ul_sinr_metric;
  /// Time alignment.
  phy_time_unit time_advance_offset;
};

/// \brief List of Uplink CRC indications for a given slot.
struct mac_crc_indication_message {
  slot_point                                          sl_rx;
  static_vector<mac_crc_pdu, MAX_PUSCH_PDUS_PER_SLOT> crcs;
};

/// \brief UCI indication for a given PDU.
struct mac_uci_pdu {
  /// \brief UCI multiplexed in the PUSCH.
  struct pusch_type {
    /// \brief HARQ related information.
    struct harq_information {
      /// Creates an HARQ information object when the HARQ was not detected by the underlying layers.
      static harq_information create_undetected_harq_info(unsigned expected_nof_bits)
      {
        harq_information info;
        info.is_valid = false;
        info.payload.resize(expected_nof_bits);

        return info;
      }

      /// Creates an HARQ information object when the HARQ was successfully detected by the underlying layers.
      static harq_information create_detected_harq_info(const bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>& payload)
      {
        harq_information info;
        info.is_valid = true;
        info.payload  = payload;

        return info;
      }

      /// Indicates detection outcome.
      bool is_valid;
      /// \brief Contents of HARQ, excluding any CRC.
      ///
      /// \n Example: If the number of HARQ bits is 20, then it is represented as:
      /// [ HARQ_bit_19 ... HARQ_bit_0 ] => [ MSB ... LSB ].
      /// NOTE: If \c is_valid == true, then the HARQs bits set in \c payload should be interpreted as an ACK (if the
      /// bit is 1) or NACK (if the bit is 0).
      /// If \c is_valid == false, then all HARQs bits set in \c payload should be interpreted as not-detected; note
      /// that these bits cannot be ignored when \c is_valid == false, as the "not-detected" outcome will be used to
      /// acknowledge (negatively) the HARQ processes corresponding to each single bit.
      bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload;
    };

    /// \brief CSI report related information.
    struct csi_information {
      /// Indicates detection outcome.
      bool is_valid;
      /// Contents of CSI, excluding any CRC.
      /// Example: If the number of CSI bits is 20, then it is represented as:
      /// [ CSI_bit_19 ... CSI_bit_0 ] => [ MSB ... LSB ].
      /// NOTE: if \c is_valid == false, then the CSI payload will be ignored.
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
    };

    /// Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// \brief Timing Advance Offset measured for the UE.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs.
    optional<float> rssi;
    /// RSRP report in dBs.
    optional<float> rsrp;
    /// HARQ information.
    optional<harq_information> harq_info;
    /// CSI Part 1 report information.
    optional<csi_information> csi_part1_info;
    /// CSI Part 2 report information.
    optional<csi_information> csi_part2_info;
  };

  /// \brief UCI carried in PUCCH Format0 or Format1.
  struct pucch_f0_or_f1_type {
    /// \brief Scheduling Request related information.
    struct sr_information {
      /// Set to true if a SR is detected, otherwise false.
      bool detected;
    };

    /// \brief HARQ related information.
    struct harq_information {
      static constexpr size_t NOF_HARQS_PER_UCI = 2;
      /// HARQ bits.
      static_vector<uci_pucch_f0_or_f1_harq_values, NOF_HARQS_PER_UCI> harqs;
    };

    /// Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// Timing Advance Offset measured for the UE.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs.
    optional<float> rssi;
    /// RSRP report in dBs.
    optional<float> rsrp;
    /// SR information.
    optional<sr_information> sr_info;
    /// HARQ information.
    optional<harq_information> harq_info;
  };

  /// \brief UCI carried in PUCCH Format2, Format3 or Format4.
  struct pucch_f2_or_f3_or_f4_type {
    /// Maximum number of SR bits expected on the PUCCH transmission.
    static constexpr size_t MAX_SR_PAYLOAD_SIZE_BITS = 4;
    using sr_information                             = bounded_bitset<MAX_SR_PAYLOAD_SIZE_BITS>;

    /// \brief HARQ related information.
    struct harq_information {
      /// Creates an HARQ information object when the HARQ was not detected by the underlying layers.
      static harq_information create_undetected_harq_info(unsigned expected_nof_bits)
      {
        harq_information info;
        info.is_valid = false;
        info.payload.resize(expected_nof_bits);

        return info;
      }

      /// Creates an HARQ information object when the HARQ was successfully detected by the underlying layers.
      static harq_information create_detected_harq_info(const bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>& payload)
      {
        harq_information info;
        info.is_valid = true;
        info.payload  = payload;

        return info;
      }

      /// Indicates detection outcome.
      bool is_valid;
      /// \brief Contents of HARQ, excluding any CRC.
      ///
      /// \n Example: If the number of HARQ bits is 20, then it is represented as:
      /// [ HARQ_bit_19 ... HARQ_bit_0 ] => [ MSB ... LSB ].
      /// NOTE: If \c is_valid == true, then the HARQs bits set in \c payload should be interpreted as an ACK (if the
      /// bit is 1) or NACK (if the bit is 0).
      /// If \c is_valid == false, then all HARQs bits set in \c payload should be interpreted as not-detected; note
      /// that these bits cannot be ignored when \c is_valid == false, as the "not-detected" outcome will be used to
      /// acknowledge (negatively) the HARQ processes corresponding to each single bit.
      bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload;
    };

    /// \brief CSI report related information.
    struct csi_information {
      /// Indicates detection outcome.
      bool is_valid;
      /// Contents of CSI, excluding any CRC.
      /// Example: If the number of CSI bits is 20, then it is represented as:
      /// [ CSI_bit_19 ... CSI_bit_0 ] => [ MSB ... LSB ].
      /// NOTE: if \c is_valid == false, then the CSI payload will be ignored.
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
    };

    /// Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// Timing Advance Offset measured for the UE.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs.
    optional<float> rssi;
    /// RSRP report in dBs.
    optional<float> rsrp;
    /// SR information.
    optional<sr_information> sr_info;
    /// HARQ information.
    optional<harq_information> harq_info;
    /// CSI Part 1 report information.
    optional<csi_information> csi_part1_info;
    /// CSI Part 2 report information.
    optional<csi_information> csi_part2_info;
  };

  /// RNTI value corresponding to the UE that generated this PDU.
  rnti_t rnti;
  /// UCI PDU multiplexed either in the PUSCH or encoded in the PUCCH.
  variant<pusch_type, pucch_f0_or_f1_type, pucch_f2_or_f3_or_f4_type> pdu;
};

/// \brief UCI indication that may contain multiple UCI PDUs.
struct mac_uci_indication_message {
  slot_point                                           sl_rx;
  static_vector<mac_uci_pdu, MAX_UCI_PDUS_PER_UCI_IND> ucis;
};

/// \brief Interface to handle feedback information from the PHY.
class mac_cell_control_information_handler
{
public:
  virtual ~mac_cell_control_information_handler() = default;

  /// \brief Handles a CRC indication.
  ///
  /// There can be more than one CRC indication per slot.
  virtual void handle_crc(const mac_crc_indication_message& msg) = 0;

  /// \brief Handles an UCI indication.
  ///
  /// The UCI indication can be received on both PUSCH and PUCCH. There can be more than one UCI indication per slot.
  virtual void handle_uci(const mac_uci_indication_message& msg) = 0;
};

} // namespace srsran
