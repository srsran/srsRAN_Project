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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/adt/variant.h"
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_pdu_capacity_constants.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/uci/uci_constants.h"
#include "srsgnb/ran/uci/uci_mapping.h"

namespace srsgnb {

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
  phy_time_unit   ta;
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
      /// HARQ Detection Status values for UCI PUSCH.
      using harq_detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

      /// Indicates CRC result on UCI.
      harq_detection_status harq_status;
      /// Contents of HARQ, excluding any CRC.
      /// NOTE: Wherever bit is set 1 the \c harq_status applies. i.e. If \c harq_status == crc_pass, then all HARQs
      /// with bit set 1 in \c payload has its status as crc_pass. \n
      /// Example: If number of HARQ bits are 20 bits, then its represented as follows in \c payload.
      /// [ HARQ_bit_19 ... HARQ_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload;
    };

    struct csi_information {
      /// CSI Part1/CSI Part2 Detection Status values for UCI PUSCH.
      using csi_part1_or_part2_detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

      /// Indicates detection outcome on UCI/CSI.
      csi_part1_or_part2_detection_status csi_status;
      /// Contents of UCI/CSI, excluding any CRC.
      /// NOTE1: Wherever bit is set 1 the \c csi_status applies. i.e. If \c csi_status == crc_pass, then all CSIs
      /// with bit set 1 in \c payload has its status as crc_pass. \n
      /// Example: If number of CSI bits are 20 bits, then its represented as follows in \c payload.
      /// [ CSI_bit_19 ... CSI_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
    };

    /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// \brief Timing Advance measured for the UE. Values: {0, 63}.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs;
    optional<float> rssi;
    /// RSRP report in dBs;
    optional<float>            rsrp;
    optional<harq_information> harq_info;
    optional<csi_information>  csi_part1_info;
    optional<csi_information>  csi_part2_info;
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
    /// \brief Timing Advance measured for the UE. Values: {0, 63}.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs;
    optional<float> rssi;
    /// RSRP report in dBs;
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

      /// Indicates CRC result on UCI.
      harq_detection_status harq_status;
      /// Contents of HARQ, excluding any CRC.
      /// NOTE: Wherever bit is set 1 the \c harq_status applies. i.e. If \c harq_status == crc_pass, then all HARQs
      /// with bit set 1 in \c payload has its status as crc_pass. \n
      /// Example: If number of HARQ bits are 20 bits, then its represented as follows in \c payload.
      /// [ HARQ_bit_19 ... HARQ_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload;
    };

    struct uci_payload_or_csi_information {
      /// \brief Indicates whether information include UCI parts payload or CSI parts payload.
      enum class payload_type_t { uci_part_payload, csi_part_payload };

      /// UCI payload Part 1/ Part 2 or CSI Part1/CSI Part2 Detection Status values for UCI PUCCH Format 2, 3, 4.
      using part1_or_part2_detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

      payload_type_t payload_type;
      /// Indicates detection outcome on UCI/CSI.
      part1_or_part2_detection_status status;
      /// Contents of UCI/CSI, excluding any CRC.
      /// NOTE1: Wherever bit is set 1 the \c csi_status applies. i.e. If \c csi_status == crc_pass, then all CSIs
      /// with bit set 1 in \c payload has its status as crc_pass. \n
      /// Example: If number of CSI bits are 20 bits, then its represented as follows in \c payload.
      /// [ CSI_bit_19 ... CSI_bit_0 ] => [ MSB ... LSB ].
      bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
    };

    /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBs.
    optional<float> ul_sinr;
    /// \brief Timing Advance measured for the UE. Values: {0, 63}.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBs;
    optional<float> rssi;
    /// RSRP report in dBs;
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

} // namespace srsgnb
