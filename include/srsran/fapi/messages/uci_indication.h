/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/adt/static_vector.h"
#include "srsran/fapi/messages/base_message.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/uci/uci_constants.h"
#include "srsran/ran/uci/uci_mapping.h"
#include <bitset>

namespace srsran {
namespace fapi {

/// UCI CSI Part 1 information.
struct uci_csi_part1 {
  uci_pusch_or_pucch_f2_3_4_detection_status                     detection_status;
  uint16_t                                                       expected_bit_length;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
};

/// UCI CSI Part 2 information.
struct uci_csi_part2 {
  uci_pusch_or_pucch_f2_3_4_detection_status                     detection_status;
  uint16_t                                                       expected_bit_length;
  bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS> payload;
};

/// UCI HARQ PDU information.
struct uci_harq_pdu {
  uci_pusch_or_pucch_f2_3_4_detection_status       detection_status;
  uint16_t                                         expected_bit_length;
  bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS> payload;
};

/// PUSCH UCI PDU information.
struct uci_pusch_pdu {
  static constexpr unsigned BITMAP_SIZE   = 4U;
  static constexpr unsigned HARQ_BIT      = 1U;
  static constexpr unsigned CSI_PART1_BIT = 2U;
  static constexpr unsigned CSI_PART2_BIT = 3U;

  std::bitset<BITMAP_SIZE> pdu_bitmap;
  uint32_t                 handle;
  uint16_t                 rnti;
  int16_t                  ul_sinr_metric;
  uint16_t                 timing_advance_offset;
  int16_t                  timing_advance_offset_ns;
  uint16_t                 rssi;
  uint16_t                 rsrp;
  uci_harq_pdu             harq;
  uci_csi_part1            csi_part1;
  uci_csi_part2            csi_part2;
};

/// SR PDU for format 0 or 1.
struct sr_pdu_format_0_1 {
  bool    sr_indication;
  uint8_t sr_confidence_level;
};

/// UCI HARQ PDU for format 0 or 1.
struct uci_harq_format_0_1 {
  /// Maximum number of HARQs.
  static constexpr unsigned MAX_NUM_HARQ = 2U;

  uint8_t                                                     harq_confidence_level;
  static_vector<uci_pucch_f0_or_f1_harq_values, MAX_NUM_HARQ> harq_values;
};

/// UCI PUCCH for format 0 or 1.
struct uci_pucch_pdu_format_0_1 {
  static constexpr unsigned BITMAP_SIZE = 2U;

  static constexpr unsigned SR_BIT   = 0U;
  static constexpr unsigned HARQ_BIT = 1U;

  enum class format_type : uint8_t { format_0, format_1 };

  std::bitset<BITMAP_SIZE> pdu_bitmap;
  uint32_t                 handle;
  uint16_t                 rnti;
  format_type              pucch_format;
  int16_t                  ul_sinr_metric;
  uint16_t                 timing_advance_offset;
  int16_t                  timing_advance_offset_ns;
  uint16_t                 rssi;
  uint16_t                 rsrp;
  sr_pdu_format_0_1        sr;
  uci_harq_format_0_1      harq;
};

/// SR PDU for format 2, 3, or 4.
struct sr_pdu_format_2_3_4 {
  /// Maximum number of supported bytes in this message.
  static constexpr unsigned MAX_SR_PAYLOAD_SIZE_BITS = 4;

  uint16_t                                 sr_bitlen;
  bounded_bitset<MAX_SR_PAYLOAD_SIZE_BITS> sr_payload;
};

/// UCI PUCCH for format 2, 3, or 4.
struct uci_pucch_pdu_format_2_3_4 {
  static constexpr unsigned BITMAP_SIZE   = 4U;
  static constexpr unsigned SR_BIT        = 0U;
  static constexpr unsigned HARQ_BIT      = 1U;
  static constexpr unsigned CSI_PART1_BIT = 2U;
  static constexpr unsigned CSI_PART2_BIT = 3U;

  enum class format_type : uint8_t { format_2, format_3, format_4 };

  std::bitset<BITMAP_SIZE> pdu_bitmap;
  uint32_t                 handle;
  uint16_t                 rnti;
  format_type              pucch_format;
  int16_t                  ul_sinr_metric;
  uint16_t                 timing_advance_offset;
  int16_t                  timing_advance_offset_ns;
  uint16_t                 rssi;
  uint16_t                 rsrp;
  sr_pdu_format_2_3_4      sr;
  uci_harq_pdu             harq;
  uci_csi_part1            csi_part1;
  uci_csi_part2            csi_part2;
};

enum class uci_pdu_type : uint16_t { PUSCH, PUCCH_format_0_1, PUCCH_format_2_3_4 };

/// Reception data indication PDU information.
struct uci_indication_pdu {
  uci_pdu_type pdu_type;
  uint16_t     pdu_size;

  // :TODO: add a variant for this fields below.
  uci_pusch_pdu              pusch_pdu;
  uci_pucch_pdu_format_0_1   pucch_pdu_f01;
  uci_pucch_pdu_format_2_3_4 pucch_pdu_f234;
};

/// UCI indication message.
struct uci_indication_message : public base_message {
  uint16_t                                                    sfn;
  uint16_t                                                    slot;
  static_vector<uci_indication_pdu, MAX_UCI_PDUS_PER_UCI_IND> pdus;
};

} // namespace fapi
} // namespace srsran
