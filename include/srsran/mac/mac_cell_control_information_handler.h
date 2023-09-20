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

/// CRC indication for a given PDU.
struct mac_crc_pdu {
  rnti_t  rnti;
  uint8_t rapid;
  uint8_t harq_id;
  bool    tb_crc_success;
  /// Codeblocks CRC status.
  bounded_bitset<MAX_CBS_PER_PDU> cb_crc_status;
  /// PUSCH SINR, in dB.
  optional<float> ul_sinr_metric;
  phy_time_unit   time_advance_offset;
  uint16_t        rssi;
  uint16_t        rsrp;
};

/// \brief List of UL CRC indications for a given slot and cell.
struct mac_crc_indication_message {
  slot_point                                          sl_rx;
  static_vector<mac_crc_pdu, MAX_PUSCH_PDUS_PER_SLOT> crcs;
};

struct mac_uci_pdu {
  struct pusch_type {
    struct harq_information {
      /// HARQ Detection Status values for UCI multiplexed in the PUSCH.
      using harq_detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

      /// Creates an HARQ information object when the HARQ was not detected by the underlying layers.
      static harq_information create_undetected_harq_info(harq_detection_status harq_status, unsigned expected_nof_bits)
      {
        harq_information info;
        info.harq_status = harq_status;
        info.payload.resize(expected_nof_bits);

        return info;
      }

      /// Creates an HARQ information object when the HARQ was successfully detected by the underlying layers.
      static harq_information create_detected_harq_info(harq_detection_status harq_status,
                                                        const bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>& payload)
      {
        harq_information info;
        info.harq_status = harq_status;
        info.payload     = payload;

        return info;
      }

      /// Indicates the CRC result on UCI.
      harq_detection_status harq_status;
      /// Contents of HARQ, excluding any CRC.
      /// NOTE: If \c harq_status == crc_pass, then all HARQs bits set in \c payload should be interpreted as an ACK.
      /// \n Example: If the number of HARQ bits is 20, then it is represented as:
      /// [ HARQ_bit_19 ... HARQ_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload;
    };

    struct csi_information {
      /// CSI Part 1/CSI Part 2 Detection Status values for UCI multiplexed in the PUSCH.
      using csi_part1_or_part2_detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

      /// Indicates the detection outcome on UCI/CSI.
      csi_part1_or_part2_detection_status csi_status;
      /// Contents of UCI/CSI, excluding any CRC.
      /// Example: If the number of CSI bits is 20, then it is represented as:
      /// [ CSI_bit_19 ... CSI_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
    };

    /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// \brief Timing Advance Offset measured for the UE.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs.
    optional<float> rssi;
    /// RSRP report in dBs.
    optional<float>            rsrp;
    optional<harq_information> harq_info;
    optional<csi_information>  csi_part1_info;
    optional<csi_information>  csi_part2_info;
    // Note: Temporary for UE in test mode. Later, they will be decoded from the CSI bits.
    optional<uint8_t> ri;
    optional<uint8_t> pmi;
  };

  struct pucch_f0_or_f1_type {
    struct sr_information {
      bool sr_detected;
    };
    struct harq_information {
      constexpr static size_t                                          NOF_HARQS_PER_UCI = 2;
      static_vector<uci_pucch_f0_or_f1_harq_values, NOF_HARQS_PER_UCI> harqs;
    };

    bool is_f1;
    /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// \brief Timing Advance Offset measured for the UE.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs.
    optional<float> rssi;
    /// RSRP report in dBs.
    optional<float>            rsrp;
    optional<sr_information>   sr_info;
    optional<harq_information> harq_info;
  };

  struct pucch_f2_or_f3_or_f4_type {
    /// Maximum number of SR bits expected on the PUCCH transmission.
    static const unsigned MAX_SR_PAYLOAD_SIZE_BITS = 4;

    using sr_information = bounded_bitset<MAX_SR_PAYLOAD_SIZE_BITS>;

    struct harq_information {
      /// HARQ Detection Status values for UCI PUCCH Format 2, 3, 4.
      using harq_detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

      /// Creates an HARQ information object when the HARQ was not detected by the underlying layers.
      static harq_information create_undetected_harq_info(harq_detection_status harq_status, unsigned expected_nof_bits)
      {
        harq_information info;
        info.harq_status = harq_status;
        info.payload.resize(expected_nof_bits);

        return info;
      }

      /// Creates an HARQ information object when the HARQ was successfully detected by the underlying layers.
      static harq_information create_detected_harq_info(harq_detection_status harq_status,
                                                        const bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>& payload)
      {
        harq_information info;
        info.harq_status = harq_status;
        info.payload     = payload;

        return info;
      }

      /// Indicates CRC result on UCI.
      harq_detection_status harq_status;
      /// Contents of HARQ, excluding any CRC.
      /// NOTE: If \c harq_status == crc_pass, then all HARQs bits set in \c payload should be interpreted as an ACK.
      /// \n Example: If the number of HARQ bits is 20, then it is represented as:
      /// [ HARQ_bit_19 ... HARQ_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload;
    };

    struct uci_payload_or_csi_information {
      /// \brief Indicates whether the object includes UCI parts payload or CSI parts payload.
      enum class payload_type_t { uci_part_payload, csi_part_payload };

      /// UCI payload Part 1/Part 2 or CSI Part 1/CSI Part 2 Detection Status values for UCI PUCCH Format 2, 3, 4.
      using part1_or_part2_detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

      payload_type_t payload_type;
      /// Indicates the detection outcome on UCI/CSI.
      part1_or_part2_detection_status status;
      /// Contents of UCI/CSI, excluding any CRC.
      /// Example: If the number of CSI bits is 20, then it is represented as:
      /// [ CSI_bit_19 ... CSI_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
    };

    /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// \brief Timing Advance Offset measured for the UE.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs.
    optional<float> rssi;
    /// RSRP report in dBs.
    optional<float>                          rsrp;
    optional<sr_information>                 sr_info;
    optional<harq_information>               harq_info;
    optional<uci_payload_or_csi_information> uci_part1_or_csi_part1_info;
    optional<uci_payload_or_csi_information> uci_part2_or_csi_part2_info;
  };

  rnti_t                                                              rnti;
  variant<pusch_type, pucch_f0_or_f1_type, pucch_f2_or_f3_or_f4_type> pdu;
};

/// \brief UCI indication that may contain multiple UCI PDUs.
struct mac_uci_indication_message {
  slot_point                                           sl_rx;
  static_vector<mac_uci_pdu, MAX_UCI_PDUS_PER_UCI_IND> ucis;
};

/// Interface to handle feedback information from the PHY.
class mac_cell_control_information_handler
{
public:
  virtual ~mac_cell_control_information_handler() = default;

  /// Handles CRC indication. There can be more than one CRC indication per slot.
  virtual void handle_crc(const mac_crc_indication_message& msg) = 0;

  /// \brief Handles UCI indication. The UCI indication can be received on both PUSCH and PUCCH. There can be more than
  /// one UCI indication per slot.
  virtual void handle_uci(const mac_uci_indication_message& msg) = 0;
};

} // namespace srsran
