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
#include "srsgnb/ran/phy_time_unit.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_pdu_capacity_constants.h"
#include "srsgnb/ran/slot_point.h"
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
  double        ul_sinr_metric;
  phy_time_unit ta;
  uint16_t      rssi;
  uint16_t      rsrp;
};

/// \brief List of UL CRC indications for a given slot and cell.
struct mac_crc_indication_message {
  slot_point                                          sl_rx;
  static_vector<mac_crc_pdu, MAX_PUSCH_PDUS_PER_SLOT> crcs;
};

struct mac_uci_pdu {
  enum class pdu_type : uint16_t { pusch, pucch_f0_or_f1, pucch_other };
  struct pusch_type {
    struct harq_information {
      /// HARQ Detection Status values for UCI PUSCH.
      using harq_detection_status = uci_pusch_detection_status;

      /// Maximum number of supported bytes in this message.
      static constexpr unsigned MAX_UCI_HARQ_LEN = 214;

      /// Indicates CRC result on UCI.
      harq_detection_status harq_status;
      /// Length of HARQ payload in bits. Values {1,...,1706}.
      uint16_t payload_bits;
      /// Contents of HARQ, excluding any CRC.
      static_vector<uint8_t, MAX_UCI_HARQ_LEN> payload;
    };

    struct csi_information {
      /// CSI Part1/CSI Part2 Detection Status values for UCI PUSCH.
      using csi_part1_or_part2_detection_status = uci_pusch_detection_status;

      /// Maximum number of supported bytes in this message.
      static constexpr unsigned MAX_CSI_PART1_OR_PART2_LEN = 214;

      /// Indicates detection outcome on UCI/CSI.
      csi_part1_or_part2_detection_status csi_status;
      /// Length of CSI Part1/CSI Part2 payload in bits. Values {1,...,1706}.
      uint16_t payload_bits;
      /// Contents of UCI/CSI, excluding any CRC.
      static_vector<uint8_t, MAX_CSI_PART1_OR_PART2_LEN> payload;
    };

    /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBFs.
    float ul_sinr;
    /// \brief Timing Advance measured for the UE. Values: {0, 63}.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBFs;
    optional<float> rssi;
    /// RSRP report in dBFs;
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
    /// \brief Metric of channel quality that ranges from -65.534 to 65.534 dBFs.
    float ul_sinr;
    /// \brief Timing Advance measured for the UE. Values: {0, 63}.
    optional<phy_time_unit> time_advance_offset;
    /// RSSI report in dBFs;
    optional<float> rssi;
    /// RSRP report in dBFs;
    optional<float>            rsrp;
    optional<sr_information>   sr_info;
    optional<harq_information> harq_info;
  };
  struct pucch_other_type {};

  pdu_type type;
  rnti_t   rnti;
  // :TODO: add variant here.
  pusch_type          pusch;
  pucch_f0_or_f1_type pucch_f0_or_f1;
  pucch_other_type    pucch_other;
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
