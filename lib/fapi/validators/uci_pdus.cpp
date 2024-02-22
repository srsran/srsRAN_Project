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

#include "uci_pdus.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a UCI.indication message.
static constexpr message_type_id msg_type = message_type_id::uci_indication;

/// Validates the RNTI property of the UCI PUSCH PDU, UCI PUCCH format 0/1/2/3/4 PDU as per SCF-222 v4.0 section 3.4.9.1
/// to 3.9.4.3.
static bool validate_rnti(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 65535;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RNTI", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the timing advance offset property of the UCI PUSCH PDU, UCI PUCCH format 0/1/2/3/4 PDU as per SCF-222
/// v4.0 section 3.4.9.1 to 3.9.4.3.
static bool validate_timing_advance_offset(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;
  static constexpr unsigned INVALID   = std::numeric_limits<uint16_t>::max();

  if (value == INVALID) {
    return true;
  }

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Timing advance offset", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the timing advance offset in nanoseconds property of the UCI PUSCH PDU, UCI PUCCH format 0/1/2/3/4 PDU as
/// per SCF-222 v4.0 section 3.4.9.1 to 3.9.4.3.
static bool validate_timing_advance_offset_in_ns(uci_pdu_type pdu_type, int value, validator_report& report)
{
  static constexpr int MIN_VALUE = -16800;
  static constexpr int MAX_VALUE = 16800;
  static constexpr int INVALID   = std::numeric_limits<int16_t>::min();

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "Timing advance offset in nanoseconds",
                        msg_type,
                        static_cast<unsigned>(pdu_type),
                        report);
}

/// Validates the RSSI property of the UCI PUSCH PDU, UCI PUCCH format 0/1/2/3/4 PDU as per SCF-222 v4.0 section 3.4.9.1
/// to 3.9.4.3.
static bool validate_rssi(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1280;
  static constexpr unsigned INVALID   = std::numeric_limits<uint16_t>::max();

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RSSI", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the RSRP property of the UCI PUSCH PDU, UCI PUCCH format 0/1/2/3/4 PDU as per SCF-222 v4.0 section 3.4.9.1
/// to 3.9.4.3.
static bool validate_rsrp(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1280;
  static constexpr unsigned INVALID   = std::numeric_limits<uint16_t>::max();

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RSRP", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the HARQ detection status property of the HARQ PDU for format 2/3/4 or for PUSCH, as per SCF-222 v4.0
/// section 3.4.9.4.
static bool validate_harq_detection_status(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 5;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "HARQ detection status", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected HARQ bit length property of the HARQ PDU for format 2, 3 or 4 or for PUSCH, as per SCF-222
/// v4.0 section 3.4.9.4.
static bool validate_harq_expected_bit_length(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Expected HARQ bit length", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected HARQ payload property of the HARQ PDU for format 2, 3 or 4 or for PUSCH, as per SCF-222 v4.0
/// section 3.4.9.4.
static bool validate_harq_payload_size(uci_pdu_type                               pdu_type,
                                       unsigned                                   expected_payload_bit_length,
                                       unsigned                                   actual_payload_bit_length,
                                       uci_pusch_or_pucch_f2_3_4_detection_status status,
                                       validator_report&                          report)
{
  if (status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
      status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
    static const unsigned EXPECTED_BIT_LENGTH_VALUE_ON_FAILURE = 0;

    return validate_field_eq(EXPECTED_BIT_LENGTH_VALUE_ON_FAILURE,
                             actual_payload_bit_length,
                             "Expected HARQ payload",
                             msg_type,
                             static_cast<unsigned>(pdu_type),
                             report);
  }

  return validate_field_eq(expected_payload_bit_length,
                           actual_payload_bit_length,
                           "Expected HARQ payload",
                           msg_type,
                           static_cast<unsigned>(pdu_type),
                           report);
}

/// Validates the CSI Part 1 detection status property of the CSI Part 1 PDU, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi_part1_detection_status(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 5;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "CSI-Part1 detection status", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected CSI Part 1 bit length property of the CSI Part 1 PDU, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi_part1_expected_bit_length(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Expected CSI-Part1 bit length", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected CSI Part 1 payload property of the CSI Part 1 PDU for format 2, 3 or 4 or for PUSCH, as per
/// SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi1_payload_size(uci_pdu_type                               pdu_type,
                                       unsigned                                   expected_payload_bit_length,
                                       unsigned                                   actual_payload_bit_length,
                                       uci_pusch_or_pucch_f2_3_4_detection_status status,
                                       validator_report&                          report)
{
  if (status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
      status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
    static const unsigned EXPECTED_BIT_LENGTH_VALUE_ON_FAILURE = 0;

    return validate_field_eq(EXPECTED_BIT_LENGTH_VALUE_ON_FAILURE,
                             actual_payload_bit_length,
                             "Expected CSI-Part1 payload",
                             msg_type,
                             static_cast<unsigned>(pdu_type),
                             report);
  }

  return validate_field_eq(expected_payload_bit_length,
                           actual_payload_bit_length,
                           "Expected CSI-Part1 payload",
                           msg_type,
                           static_cast<unsigned>(pdu_type),
                           report);
}

/// Validates the CSI Part 2 detection status property of the CSI Part 2 PDU, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi_part2_detection_status(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 5;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "CSI-Part2 detection status", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected CSI Part 2 bit length property of the CSI Part 2 PDU, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi_part2_expected_bit_length(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Expected CSI-Part2 bit length", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected CSI Part 2 payload property of the CSI Part 2 PDU for format 2, 3 or 4 or for PUSCH, as per
/// SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi2_payload_size(uci_pdu_type                               pdu_type,
                                       unsigned                                   expected_payload_bit_length,
                                       unsigned                                   actual_payload_bit_length,
                                       uci_pusch_or_pucch_f2_3_4_detection_status status,
                                       validator_report&                          report)
{
  if (status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
      status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
    static const unsigned EXPECTED_BIT_LENGTH_VALUE_ON_FAILURE = 0;

    return validate_field_eq(EXPECTED_BIT_LENGTH_VALUE_ON_FAILURE,
                             actual_payload_bit_length,
                             "Expected CSI-Part2 payload",
                             msg_type,
                             static_cast<unsigned>(pdu_type),
                             report);
  }

  return validate_field_eq(expected_payload_bit_length,
                           actual_payload_bit_length,
                           "Expected CSI-Part2 payload",
                           msg_type,
                           static_cast<unsigned>(pdu_type),
                           report);
}

bool srsran::fapi::validate_uci_pusch_pdu(const uci_pusch_pdu& pdu, validator_report& report)
{
  static constexpr uci_pdu_type pdu_type = uci_pdu_type::PUSCH;
  bool                          result   = true;

  // NOTE: PDU bitmap property will not be validated.
  // NOTE: Handle property will not be validated.
  result &= validate_rnti(pdu_type, static_cast<unsigned>(pdu.rnti), report);

  // NOTE: UL SINR metric property uses the whole range of the variable, so it will not be validated.
  result &= validate_timing_advance_offset(pdu_type, pdu.timing_advance_offset, report);
  result &= validate_timing_advance_offset_in_ns(pdu_type, pdu.timing_advance_offset_ns, report);
  result &= validate_rssi(pdu_type, pdu.rssi, report);
  result &= validate_rsrp(pdu_type, pdu.rsrp, report);

  // Validate HARQ when it is present.
  if (pdu.pdu_bitmap[uci_pusch_pdu::HARQ_BIT]) {
    result &= validate_harq_detection_status(pdu_type, static_cast<unsigned>(pdu.harq.detection_status), report);
    result &= validate_harq_expected_bit_length(pdu_type, pdu.harq.expected_bit_length, report);
    result &= validate_harq_payload_size(
        pdu_type, pdu.harq.expected_bit_length, pdu.harq.payload.size(), pdu.harq.detection_status, report);
  }

  // Validate CSI Part 1 when it is present.
  if (pdu.pdu_bitmap[uci_pusch_pdu::CSI_PART1_BIT]) {
    result &=
        validate_csi_part1_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part1.detection_status), report);
    result &= validate_csi_part1_expected_bit_length(pdu_type, pdu.csi_part1.expected_bit_length, report);
    result &= validate_csi1_payload_size(pdu_type,
                                         pdu.csi_part1.expected_bit_length,
                                         pdu.csi_part1.payload.size(),
                                         pdu.csi_part1.detection_status,
                                         report);
  }

  // Validate CSI Part 2 when it is present.
  if (pdu.pdu_bitmap[uci_pusch_pdu::CSI_PART2_BIT]) {
    result &=
        validate_csi_part2_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part2.detection_status), report);
    result &= validate_csi_part2_expected_bit_length(pdu_type, pdu.csi_part2.expected_bit_length, report);
    result &= validate_csi2_payload_size(pdu_type,
                                         pdu.csi_part2.expected_bit_length,
                                         pdu.csi_part2.payload.size(),
                                         pdu.csi_part2.detection_status,
                                         report);
  }

  return result;
}

/// Validates the PUCCH format property of the PUCCH format 0/1 PDU, as per SCF-222 v4.0 section 3.4.9.2.
static bool validate_pucch_format01(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "PUCCH format",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_0_1),
                        report);
}

/// Validates the SR confidence level property of the SR PDU for format 0 or 1, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_sr_confidence_level(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;
  static constexpr unsigned INVALID   = std::numeric_limits<uint8_t>::max();

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "SR confidence level",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_0_1),
                        report);
}

/// Validates the HARQ confidence level property of the HARQ PDU for format 0/1, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_harq_format01_confidence_level(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;
  static constexpr unsigned INVALID   = std::numeric_limits<uint8_t>::max();

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "HARQ confidence level",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_0_1),
                        report);
}

/// Validates the number of HARQ bits property of the HARQ PDU for format 0/1, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_harq_format01_nof_harq(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "Number of HARQ bits",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_0_1),
                        report);
}

/// Validates the HARQ value property of the HARQ PDU for format 0/1, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_harq_format01_value(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "HARQ value",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_0_1),
                        report);
}

bool srsran::fapi::validate_uci_pucch_format01_pdu(const uci_pucch_pdu_format_0_1& pdu, validator_report& report)
{
  static constexpr uci_pdu_type pdu_type = uci_pdu_type::PUCCH_format_0_1;
  bool                          result   = true;

  // NOTE: PDU bitmap property will not be validated.
  // NOTE: Handle property will not be validated.
  result &= validate_rnti(pdu_type, static_cast<unsigned>(pdu.rnti), report);
  result &= validate_pucch_format01(static_cast<unsigned>(pdu.pucch_format), report);

  // NOTE: UL SINR metric property uses the whole range of the variable, so it will not be validated.
  result &= validate_timing_advance_offset(pdu_type, pdu.timing_advance_offset, report);
  result &= validate_timing_advance_offset_in_ns(pdu_type, pdu.timing_advance_offset_ns, report);
  result &= validate_rssi(pdu_type, pdu.rssi, report);
  result &= validate_rsrp(pdu_type, pdu.rsrp, report);

  // Validate SR information when it is present.
  if (pdu.pdu_bitmap[uci_pucch_pdu_format_0_1::SR_BIT]) {
    // NOTE: SR indication property is a bool, so it is not needed to be validated.
    result &= validate_sr_confidence_level(pdu.sr.sr_confidence_level, report);
  }

  // Validate HARQ information when it is present.
  if (pdu.pdu_bitmap[uci_pucch_pdu_format_0_1::HARQ_BIT]) {
    result &= validate_harq_format01_nof_harq(pdu.harq.harq_values.size(), report);
    result &= validate_harq_format01_confidence_level(pdu.harq.harq_confidence_level, report);
    for (const auto& harq : pdu.harq.harq_values) {
      result &= validate_harq_format01_value(static_cast<unsigned>(harq), report);
    }
  }

  return result;
}

/// Validates the PUCCH format property of the PUCCH format 2/3/4 PDU, as per SCF-222 v4.0 section 3.4.9.3.
static bool validate_pucch_format234(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "PUCCH format",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_2_3_4),
                        report);
}

/// Validates the SR bit length property of the SR format 2/3/4 PDU, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_sr_format234_bit_length(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "SR bit length",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_2_3_4),
                        report);
}

bool srsran::fapi::validate_uci_pucch_format234_pdu(const uci_pucch_pdu_format_2_3_4& pdu, validator_report& report)
{
  static constexpr uci_pdu_type pdu_type = uci_pdu_type::PUCCH_format_2_3_4;
  bool                          result   = true;

  // NOTE: PDU bitmap property will not be validated.
  // NOTE: Handle property will not be validated.
  result &= validate_rnti(pdu_type, static_cast<unsigned>(pdu.rnti), report);
  result &= validate_pucch_format234(static_cast<unsigned>(pdu.pucch_format), report);

  // NOTE: UL SINR metric property uses the whole range of the variable, so it will not be validated.
  result &= validate_timing_advance_offset(pdu_type, pdu.timing_advance_offset, report);
  result &= validate_timing_advance_offset_in_ns(pdu_type, pdu.timing_advance_offset_ns, report);
  result &= validate_rssi(pdu_type, pdu.rssi, report);
  result &= validate_rsrp(pdu_type, pdu.rsrp, report);

  // Validate SR information when it is present.
  if (pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::SR_BIT]) {
    result &= validate_sr_format234_bit_length(pdu.sr.sr_bitlen, report);
  }

  // Validate HARQ when it is present.
  if (pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::HARQ_BIT]) {
    result &= validate_harq_detection_status(pdu_type, static_cast<unsigned>(pdu.harq.detection_status), report);
    result &= validate_harq_expected_bit_length(pdu_type, pdu.harq.expected_bit_length, report);
    result &= validate_harq_payload_size(
        pdu_type, pdu.harq.expected_bit_length, pdu.harq.payload.size(), pdu.harq.detection_status, report);
  }

  // Validate CSI Part 1 when it is present.
  if (pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT]) {
    result &=
        validate_csi_part1_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part1.detection_status), report);
    result &= validate_csi_part1_expected_bit_length(pdu_type, pdu.csi_part1.expected_bit_length, report);
    result &= validate_csi1_payload_size(pdu_type,
                                         pdu.csi_part1.expected_bit_length,
                                         pdu.csi_part1.payload.size(),
                                         pdu.csi_part1.detection_status,
                                         report);
  }

  // Validate CSI Part 2 when it is present.
  if (pdu.pdu_bitmap[uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT]) {
    result &=
        validate_csi_part2_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part2.detection_status), report);
    result &= validate_csi_part2_expected_bit_length(pdu_type, pdu.csi_part2.expected_bit_length, report);
    result &= validate_csi2_payload_size(pdu_type,
                                         pdu.csi_part2.expected_bit_length,
                                         pdu.csi_part2.payload.size(),
                                         pdu.csi_part2.detection_status,
                                         report);
  }

  return result;
}
