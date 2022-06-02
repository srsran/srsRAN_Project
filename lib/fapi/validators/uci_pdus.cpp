/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uci_pdus.h"
#include "helpers.h"
#include "srsgnb/adt/bitmap_utils.h"
#include "srsgnb/fapi/messages.h"

using namespace srsgnb;
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

/// Validates the expected HARQ bit length detection status property of the HARQ PDU for format 2, 3 or 4 or for PUSCH,
/// as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_harq_expected_bit_length(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Expected HARQ bit length", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the CSI part 1 detection status property of the CSI part 1 PDU, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi_part1_detection_status(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 5;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "CSI Part 1 detection status", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected CSI part 1 bit length detection status property of the CSI part 1 PDU, as per SCF-222 v4.0
/// section 3.4.9.4.
static bool validate_csi_part1_expected_bit_length(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Expected CSI part1 bit length", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the CSI part 2 detection status property of the CSI part 2 PDU, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_csi_part2_detection_status(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 5;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "CSI Part 2 detection status", msg_type, static_cast<unsigned>(pdu_type), report);
}

/// Validates the expected CSI part 2 bit length detection status property of the CSI part 2 PDU, as per SCF-222 v4.0
/// section 3.4.9.4.
static bool validate_csi_part2_expected_bit_length(uci_pdu_type pdu_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Expected CSI part2 bit length", msg_type, static_cast<unsigned>(pdu_type), report);
}

bool srsgnb::fapi::validate_uci_pusch_pdu(const uci_pusch_pdu& pdu, validator_report& report)
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
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pusch_pdu::HARQ_BIT)) {
    result &= validate_harq_detection_status(pdu_type, static_cast<unsigned>(pdu.harq.detection_status), report);
    result &= validate_harq_expected_bit_length(pdu_type, pdu.harq.bit_length, report);
    // NOTE: HARQ payload property will not be checked.
  }

  // Validate CSI part 1 when it is present.
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pusch_pdu::CSI_PART1_BIT)) {
    result &=
        validate_csi_part1_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part1.detection_status), report);
    result &= validate_csi_part1_expected_bit_length(pdu_type, pdu.csi_part1.bit_length, report);
    // NOTE: CSI part 1 payload property will not be checked.
  }

  // Validate CSI part 2 when it is present.
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pusch_pdu::CSI_PART2_BIT)) {
    result &=
        validate_csi_part2_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part2.detection_status), report);
    result &= validate_csi_part2_expected_bit_length(pdu_type, pdu.csi_part2.bit_length, report);
    // NOTE: CSI part 2 payload property will not be checked.
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

/// Validates the SR indication property of the SR PDU for format 0/1, as per SCF-222 v4.0 section 3.4.9.4.
static bool validate_sr_indication(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "SR indication",
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

bool srsgnb::fapi::validate_uci_pucch_format01_pdu(const uci_pucch_pdu_format_0_1& pdu, validator_report& report)
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
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pucch_pdu_format_0_1::SR_BIT)) {
    result &= validate_sr_indication(pdu.sr.sr_indication, report);
    result &= validate_sr_confidence_level(pdu.sr.sr_confidence_level, report);
  }

  // Validate HARQ information when it is present.
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pucch_pdu_format_0_1::HARQ_BIT)) {
    result &= validate_harq_format01_nof_harq(pdu.harq.harq_values.size(), report);
    result &= validate_harq_format01_confidence_level(pdu.harq.harq_confidence_level, report);
    for (const auto& harq : pdu.harq.harq_values) {
      result &= validate_harq_format01_value(harq, report);
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

/// Validates the UCI detection status property of the UCI payload for PUSCH or PUCCH, as per SCF-222 v4.0
/// section 3.4.9.4.
static bool validate_uci_detection_status(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 5;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "UCI detection status",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_2_3_4),
                        report);
}

/// Validates the expected UCI payload size property of the UCI payload for PUSCH or PUCCH, as per SCF-222 v4.0
/// section 3.4.9.4.
static bool validate_uci_expected_bit_length(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "Expected UCI payload size",
                        msg_type,
                        static_cast<unsigned>(uci_pdu_type::PUCCH_format_2_3_4),
                        report);
}

bool srsgnb::fapi::validate_uci_pucch_format234_pdu(const uci_pucch_pdu_format_2_3_4& pdu, validator_report& report)
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
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pucch_pdu_format_2_3_4::SR_BIT)) {
    result &= validate_sr_format234_bit_length(pdu.sr.sr_bitlen, report);
  }

  // Validate HARQ when it is present.
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pucch_pdu_format_2_3_4::HARQ_BIT)) {
    result &= validate_harq_detection_status(pdu_type, static_cast<unsigned>(pdu.harq.detection_status), report);
    result &= validate_harq_expected_bit_length(pdu_type, pdu.harq.bit_length, report);
    // NOTE: HARQ payload property will not be checked.
  }

  // Validate CSI part 1 when it is present.
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT)) {
    result &=
        validate_csi_part1_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part1.detection_status), report);
    result &= validate_csi_part1_expected_bit_length(pdu_type, pdu.csi_part1.bit_length, report);
    // NOTE: CSI part 1 payload property will not be checked.

    // Check the UCI part 1 payload.
    result &= validate_uci_detection_status(static_cast<unsigned>(pdu.uci_part1.detection_status), report);
    result &= validate_uci_expected_bit_length(pdu.uci_part1.expected_uci_payload_size, report);
  }

  // Validate CSI part 2 when it is present.
  if (check_bitmap_bit(pdu.pdu_bitmap, uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT)) {
    result &=
        validate_csi_part2_detection_status(pdu_type, static_cast<unsigned>(pdu.csi_part2.detection_status), report);
    result &= validate_csi_part2_expected_bit_length(pdu_type, pdu.csi_part2.bit_length, report);
    // NOTE: CSI part 2 payload property will not be checked.

    // Check the UCI part 2 payload.
    result &= validate_uci_detection_status(static_cast<unsigned>(pdu.uci_part2.detection_status), report);
    result &= validate_uci_expected_bit_length(pdu.uci_part2.expected_uci_payload_size, report);
  }

  return result;
}
