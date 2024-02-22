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

#include "srsran/fapi/message_validators.h"
#include "dl_csi_pdu.h"
#include "dl_pdcch_pdu.h"
#include "dl_pdsch_pdu.h"
#include "dl_ssb_pdu.h"
#include "field_checkers.h"
#include "uci_pdus.h"
#include "ul_prach_pdu.h"
#include "ul_pucch_pdu.h"
#include "ul_pusch_pdu.h"
#include "srsran/support/format_utils.h"

using namespace srsran;
using namespace fapi;

/// Validates the SFN property of a message.
static bool validate_sfn(unsigned value, message_type_id msg_type, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "sfn", msg_type, report);
}

/// Validates the slot property of a message.
static bool validate_slot(unsigned value, message_type_id msg_type, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 159;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "slot", msg_type, report);
}

/// Validates the number of PDU Groups property of a DL_TTI.request or UL_TTI.request, as per SCF-222 v4.0 section 3.4.2
/// and 3.4.3.
static bool validate_num_groups(unsigned value, message_type_id msg_type, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3822;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of PDU groups", msg_type, report);
}

error_type<validator_report> srsran::fapi::validate_dl_tti_request(const dl_tti_request_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, message_type_id::dl_tti_request, report);
  success &= validate_slot(msg.slot, message_type_id::dl_tti_request, report);
  success &= validate_num_groups(msg.num_groups, message_type_id::dl_tti_request, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case dl_pdu_type::SSB:
        success &= validate_dl_ssb_pdu(pdu.ssb_pdu, report);
        break;
      case dl_pdu_type::PDCCH:
        success &= validate_dl_pdcch_pdu(message_type_id::dl_tti_request, pdu.pdcch_pdu, report);
        break;
      case dl_pdu_type::PDSCH:
        success &= validate_dl_pdsch_pdu(pdu.pdsch_pdu, report);
        break;
      case dl_pdu_type::CSI_RS:
        success &= validate_dl_csi_pdu(pdu.csi_rs_pdu, report);
        break;
      default:
        srsran_assert(0, "Invalid pdu_type");
        break;
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

error_type<validator_report> srsran::fapi::validate_ul_dci_request(const ul_dci_request_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, message_type_id::ul_dci_request, report);
  success &= validate_slot(msg.slot, message_type_id::ul_dci_request, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case ul_dci_pdu_type::PDCCH:
        success &= validate_dl_pdcch_pdu(message_type_id::ul_dci_request, pdu.pdu, report);
        break;
      default:
        srsran_assert(0, "Invalid pdu_type");
        break;
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

/// Validates the CW index property of a Tx_Data.request message, as per SCF-222 v4.0 section 3.4.6.
static bool validate_pdu_cw_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CW index", message_type_id::tx_data_request, report);
}

/// Validates the payload property of a custom Tx_Data.request TBS-TLV, as per SCF-222 v4.0 section 3.4.6.
static bool validate_tlv_custom_payload(const uint8_t* payload, validator_report& report)
{
  if (payload != nullptr) {
    return true;
  }

  report.append(0U, "TLV payload custom", message_type_id::tx_data_request);
  return false;
}

error_type<validator_report> srsran::fapi::validate_tx_data_request(const tx_data_request_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, message_type_id::tx_data_request, report);
  success &= validate_slot(msg.slot, message_type_id::tx_data_request, report);

  for (const auto& pdu : msg.pdus) {
    success &= validate_pdu_cw_index(pdu.cw_index, report);
    success &= validate_tlv_custom_payload(pdu.tlv_custom.payload, report);
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

/// Validates the RNTI property of a CRC.indication PDU or Rx_Data.indication PDU, as per  SCF-222 v4.0 section 3.4.8
/// and 3.4.7.
static bool validate_rnti(unsigned value, message_type_id msg_id, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 65535;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RNTI", msg_id, report);
}

/// Validates the RAPID property of a CRC.indication PDU or Rx_Data.indication PDU, as per  SCF-222 v4.0 section 3.4.8
/// and 3.4.7.
static bool validate_rapid(unsigned value, message_type_id msg_id, validator_report& report)
{
  static constexpr unsigned OTHERWISE = 255;
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;

  if (value == OTHERWISE) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RAPID", msg_id, report);
}

/// Validates the HARQ ID property of a CRC.indication PDU or Rx_Data.indication PDU, as per  SCF-222 v4.0 section 3.4.8
/// and 3.4.7.
static bool validate_harq_id(unsigned value, message_type_id msg_id, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 15;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "HARQ ID", msg_id, report);
}

/// Validates the timing advance offset property of a CRC.indication PDU, as per SCF-222 v4.0 section 3.4.8.
static bool validate_timing_advance_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned INVALID   = 65535;
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Timing advance offset", message_type_id::crc_indication, report);
}

/// Validates the timing advance offset in nanoseconds property of a CRC.indication PDU, as per SCF-222 v4.0
/// section 3.4.8.
static bool validate_timing_advance_offset_ns(int value, validator_report& report)
{
  static constexpr int INVALID   = -32768;
  static constexpr int MIN_VALUE = -16800;
  static constexpr int MAX_VALUE = 16800;

  if (value == INVALID) {
    return true;
  }

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Timing advance offset in nanoseconds", message_type_id::crc_indication, report);
}

/// Validates the RSSI property of a CRC.indication PDU, as per SCF-222 v4.0 section 3.4.8.
static bool validate_rssi(unsigned value, validator_report& report)
{
  static constexpr unsigned INVALID   = 65535;
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1280;

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RSSI", message_type_id::crc_indication, report);
}

/// Validates the RSRP property of a CRC.indication PDU, as per SCF-222 v4.0 section 3.4.8.
static bool validate_rsrp(unsigned value, validator_report& report)
{
  static constexpr unsigned INVALID   = 65535;
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1280;

  if (value == INVALID) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RSRP", message_type_id::crc_indication, report);
}

error_type<validator_report> srsran::fapi::validate_crc_indication(const crc_indication_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, message_type_id::crc_indication, report);
  success &= validate_slot(msg.slot, message_type_id::crc_indication, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    // NOTE: Handle property will not be validated as the values are not specified in the document.
    success &= validate_rnti(to_value(pdu.rnti), message_type_id::crc_indication, report);
    success &= validate_rapid(pdu.rapid, message_type_id::crc_indication, report);
    success &= validate_harq_id(pdu.harq_id, message_type_id::crc_indication, report);
    // NOTE: CB CRC status bitmap property will not be validated.
    // NOTE: SINR property uses the whole variable range, so it will not be tested.
    success &= validate_timing_advance_offset(pdu.timing_advance_offset, report);
    success &= validate_timing_advance_offset_ns(pdu.timing_advance_offset_ns, report);
    success &= validate_rssi(pdu.rssi, report);
    success &= validate_rsrp(pdu.rsrp, report);
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

/// Validates the symbol index property of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_symbol_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Symbol index", message_type_id::rach_indication, report);
}

/// Validates the slot index property of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_slot_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 79;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Slot index", message_type_id::rach_indication, report);
}

/// Validates the RACH index property of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_ra_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 7;

  return validate_field(MIN_VALUE,
                        MAX_VALUE,
                        value,
                        "Index of the received PRACH frequency domain occasion",
                        message_type_id::rach_indication,
                        report);
}

/// Validates the AVG RSSI property of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_avg_rssi(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 170000;

  if (value == std::numeric_limits<uint32_t>::max()) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "AVG RSSI", message_type_id::rach_indication, report);
}

/// Validates the RSRP property of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_rach_rsrp(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1280;

  if (value == std::numeric_limits<uint16_t>::max()) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RSRP", message_type_id::rach_indication, report);
}

/// Validates the preamble index property of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_preamble_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Preamble index", message_type_id::rach_indication, report);
}

/// Validates the timing advance offset of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_rach_timing_advance_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3846;

  if (value == std::numeric_limits<uint16_t>::max()) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Timing advance offset", message_type_id::rach_indication, report);
}

/// Validates the timing advance offset in nanoseconds property of the RACH.indication PDU, as per SCF-222 v4.0
/// section 3.4.11 in table RACH.indication message body.
static bool validate_timing_advance_offset_ns(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2005000;

  if (value == std::numeric_limits<uint32_t>::max()) {
    return true;
  }

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Timing advance offset in nanoseconds", message_type_id::rach_indication, report);
}

/// Validates the preamble power property of the RACH.indication PDU, as per SCF-222 v4.0 section 3.4.11 in table
/// RACH.indication message body.
static bool validate_preamble_power(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 170000;

  if (value == std::numeric_limits<uint32_t>::max()) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Preamble power", message_type_id::rach_indication, report);
}

error_type<validator_report> srsran::fapi::validate_rach_indication(const rach_indication_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, message_type_id::rach_indication, report);
  success &= validate_slot(msg.slot, message_type_id::rach_indication, report);

  for (const auto& pdu : msg.pdus) {
    success &= validate_symbol_index(pdu.symbol_index, report);
    success &= validate_slot_index(pdu.slot_index, report);
    success &= validate_ra_index(pdu.ra_index, report);
    success &= validate_avg_rssi(pdu.avg_rssi, report);
    success &= validate_rach_rsrp(pdu.rsrp, report);
    // NOTE: AVG SNR property uses the whole range of the property, so it will not be validated.
    for (const auto& preamble : pdu.preambles) {
      success &= validate_preamble_index(preamble.preamble_index, report);
      success &= validate_rach_timing_advance_offset(preamble.timing_advance_offset, report);
      success &= validate_timing_advance_offset_ns(preamble.timing_advance_offset_ns, report);
      success &= validate_preamble_power(preamble.preamble_pwr, report);
      // NOTE: Preamble SNR property uses the whole range of the property, so it will not be validated.
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

error_type<validator_report> srsran::fapi::validate_uci_indication(const uci_indication_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, message_type_id::uci_indication, report);
  success &= validate_slot(msg.slot, message_type_id::uci_indication, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case uci_pdu_type::PUSCH:
        success &= validate_uci_pusch_pdu(pdu.pusch_pdu, report);
        break;
      case uci_pdu_type::PUCCH_format_0_1:
        success &= validate_uci_pucch_format01_pdu(pdu.pucch_pdu_f01, report);
        break;
      case uci_pdu_type::PUCCH_format_2_3_4:
        success &= validate_uci_pucch_format234_pdu(pdu.pucch_pdu_f234, report);
        break;
      default:
        srsran_assert(0, "Invalid pdu_type");
        break;
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

/// Validates the message id property of the ERROR.indication PDU, as per SCF-222 v4.0 section 3.3.6.1 in table
/// ERROR.indication message body.
static bool validate_message_id(unsigned value, validator_report& report)
{
  // Check first range.
  static constexpr unsigned MIN_VALUE_FIRST_RANGE = 0U;
  static constexpr unsigned MAX_VALUE_FIRST_RANGE = 7U;

  if (MIN_VALUE_FIRST_RANGE <= value && value <= MAX_VALUE_FIRST_RANGE) {
    return true;
  }

  // Check second range.
  static constexpr unsigned MIN_VALUE_SECOND_RANGE = 0x80U;
  static constexpr unsigned MAX_VALUE_SECOND_RANGE = 0x8aU;

  if (MIN_VALUE_SECOND_RANGE <= value && value <= MAX_VALUE_SECOND_RANGE) {
    return true;
  }

  // Check third range.
  static constexpr unsigned MIN_VALUE_THIRD_RANGE = 0x180U;
  static constexpr unsigned MAX_VALUE_THIRD_RANGE = 0x182U;

  if (MIN_VALUE_THIRD_RANGE <= value && value <= MAX_VALUE_THIRD_RANGE) {
    return true;
  }

  report.append(static_cast<int>(value), "Message ID", message_type_id::error_indication);

  return false;
}

/// Validates the error code property of the ERROR.indication PDU, as per SCF-222 v4.0 section 3.3.6.1 in table
/// ERROR.indication message body.
static bool validate_error_code(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0U;
  static constexpr unsigned MAX_VALUE = 0xcU;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Error code", message_type_id::error_indication, report);
}

/// Validates the expected SFN property of the ERROR.indication PDU, as per SCF-222 v4.0 section 3.3.6.1 in table
/// ERROR.indication message body.
static bool validate_expected_sfn(unsigned value, error_code_id error_id, validator_report& report)
{
  static constexpr unsigned MAX_VALUE = 1023U;
  static constexpr unsigned UNUSED    = std::numeric_limits<uint16_t>::max();

  if (error_id == error_code_id::out_of_sync && value <= MAX_VALUE) {
    return true;
  }

  if (error_id != error_code_id::out_of_sync && value == UNUSED) {
    return true;
  }

  report.append(static_cast<int>(value), "Expected SFN", message_type_id::error_indication);

  return false;
}

/// Validates the expected SFN property of the ERROR.indication PDU, as per SCF-222 v4.0 section 3.3.6.1 in table
/// ERROR.indication message body.
static bool validate_expected_slot(unsigned value, error_code_id error_id, validator_report& report)
{
  static constexpr unsigned MAX_VALUE = 159U;
  static constexpr unsigned UNUSED    = std::numeric_limits<uint16_t>::max();

  if (error_id == error_code_id::out_of_sync && value <= MAX_VALUE) {
    return true;
  }

  if (error_id != error_code_id::out_of_sync && value == UNUSED) {
    return true;
  }

  report.append(static_cast<int>(value), "Expected slot", message_type_id::error_indication);

  return false;
}

error_type<validator_report> srsran::fapi::validate_error_indication(const error_indication_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  static constexpr message_type_id msg_id = message_type_id::error_indication;

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, msg_id, report);
  success &= validate_slot(msg.slot, msg_id, report);
  success &= validate_message_id(static_cast<unsigned>(msg.message_id), report);
  success &= validate_error_code(static_cast<unsigned>(msg.error_code), report);
  success &= validate_expected_sfn(msg.expected_sfn, msg.error_code, report);
  success &= validate_expected_slot(msg.expected_slot, msg.error_code, report);

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

/// Validates the PDU tag property of the Rx_Data.indication PDU, as per SCF-222 v4.0 section 3.4.7 in table
/// Rx_Data.indication message body.
/// \note This validator only accepts the custom tag.
static bool validate_pdu_tag(rx_data_indication_pdu::pdu_tag_type value, validator_report& report)
{
  if (value == rx_data_indication_pdu::pdu_tag_type::custom) {
    return true;
  }

  report.append(static_cast<int>(value), "PDU tag", message_type_id::rx_data_indication);

  return false;
}

/// Validates the PDU value property of the Rx_Data.indication PDU, as per SCF-222 v4.0 section 3.4.7 in table
/// Rx_Data.indication message body.
static bool validate_pdu_value(const uint8_t* value, validator_report& report)
{
  if (value != nullptr) {
    return true;
  }

  report.append(0, "PDU tag", message_type_id::rx_data_indication);

  return false;
}

error_type<validator_report> srsran::fapi::validate_rx_data_indication(const rx_data_indication_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  static constexpr message_type_id msg_id = message_type_id::rx_data_indication;

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, msg_id, report);
  success &= validate_slot(msg.slot, msg_id, report);
  // NOTE: Control length property will not be validated.

  for (const auto& pdu : msg.pdus) {
    // NOTE: Handle property will not be validated.
    success &= validate_rnti(to_value(pdu.rnti), msg_id, report);
    success &= validate_rapid(pdu.rapid, msg_id, report);
    success &= validate_harq_id(pdu.harq_id, msg_id, report);
    // NOTE: PDU length property will not be validated.
    success &= validate_pdu_tag(pdu.pdu_tag, report);
    // PDUs that were not decoded successfully do not carry data.
    if (pdu.pdu_length) {
      success &= validate_pdu_value(pdu.data, report);
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

error_type<validator_report> srsran::fapi::validate_slot_indication(const slot_indication_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  static constexpr message_type_id msg_id = message_type_id::slot_indication;

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, msg_id, report);
  success &= validate_slot(msg.slot, msg_id, report);

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

error_type<validator_report> srsran::fapi::validate_ul_tti_request(const ul_tti_request_message& msg)
{
  validator_report                 report(msg.sfn, msg.slot);
  static constexpr message_type_id msg_type = message_type_id::ul_tti_request;

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, msg_type, report);
  success &= validate_slot(msg.slot, msg_type, report);
  success &= validate_num_groups(msg.num_groups, msg_type, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case ul_pdu_type::PRACH:
        success &= validate_ul_prach_pdu(pdu.prach_pdu, report);
        break;
      case ul_pdu_type::PUCCH:
        success &= validate_ul_pucch_pdu(pdu.pucch_pdu, report);
        break;
      case ul_pdu_type::PUSCH:
        success &= validate_ul_pusch_pdu(pdu.pusch_pdu, report);
        break;
      default:
        srsran_assert(0, "Invalid pdu_type");
        break;
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

/// Returns a string identifier for the given UCI.indication PDU.
static const char* get_uci_pdu_type_string(uci_pdu_type pdu_id)
{
  switch (pdu_id) {
    case uci_pdu_type::PUSCH:
      return "PUSCH";
    case uci_pdu_type::PUCCH_format_0_1:
      return "PUCCH Format 0/1";
    case uci_pdu_type::PUCCH_format_2_3_4:
      return "PUCCH Format 2/3/4";
    default:
      srsran_assert(0, "Invalid UCI.indication PDU={}", pdu_id);
      break;
  }
  return "";
}

/// Returns a string identifier for the given UL_TTI.request PDU.
static const char* get_ul_tti_pdu_type_string(ul_pdu_type pdu_id)
{
  switch (pdu_id) {
    case ul_pdu_type::PUSCH:
      return "PUSCH";
    case ul_pdu_type::PRACH:
      return "PRACH";
    case ul_pdu_type::PUCCH:
      return "PUCCH";
    case ul_pdu_type::SRS:
      return "SRS";
    default:
      srsran_assert(0, "Invalid UL_TTI.request PDU={}", pdu_id);
      break;
  }
  return "";
}

/// Returns a string identifier for the given DL_TTI.request PDU.
static const char* get_dl_tti_pdu_type_string(dl_pdu_type pdu_id)
{
  switch (pdu_id) {
    case dl_pdu_type::CSI_RS:
      return "CSI-RS";
    case dl_pdu_type::PDCCH:
      return "PDCCH";
    case dl_pdu_type::PDSCH:
      return "PDSCH";
    case dl_pdu_type::SSB:
      return "SSB";
    default:
      srsran_assert(0, "Invalid DL_TTI.request PDU={}", pdu_id);
      break;
  }
  return "";
}

/// Returns a string identifier for the given message and PDU.
static const char* get_pdu_type_string(message_type_id msg_id, unsigned pdu_id)
{
  switch (msg_id) {
    case message_type_id::rach_indication:
    case message_type_id::crc_indication:
    case message_type_id::error_indication:
    case message_type_id::tx_data_request:
    case message_type_id::ul_dci_request:
    case message_type_id::rx_data_indication:
    case message_type_id::slot_indication:
    case message_type_id::config_request:
    case message_type_id::config_response:
    case message_type_id::dl_tti_response:
    case message_type_id::param_request:
    case message_type_id::param_response:
    case message_type_id::srs_indication:
    case message_type_id::start_request:
    case message_type_id::stop_indication:
    case message_type_id::stop_request:
      return "";
    case message_type_id::uci_indication:
      return get_uci_pdu_type_string(static_cast<uci_pdu_type>(pdu_id));
    case message_type_id::dl_tti_request:
      return get_dl_tti_pdu_type_string(static_cast<dl_pdu_type>(pdu_id));
    case message_type_id::ul_tti_request:
      return get_ul_tti_pdu_type_string(static_cast<ul_pdu_type>(pdu_id));
    default:
      srsran_assert(0, "Invalid FAPI message type={}", msg_id);
      break;
  }
  return "";
}

/// Returns a string representation for the given message identifier.
static const char* get_message_type_string(message_type_id msg_id)
{
  switch (msg_id) {
    case message_type_id::rach_indication:
      return "RACH.indication";
    case message_type_id::crc_indication:
      return "CRC.indication";
    case message_type_id::error_indication:
      return "ERROR.indication";
    case message_type_id::dl_tti_request:
      return "DL_TTI.request";
    case message_type_id::tx_data_request:
      return "Tx_Data.request";
    case message_type_id::ul_dci_request:
      return "UL_DCI.request";
    case message_type_id::uci_indication:
      return "UCI.indication";
    case message_type_id::rx_data_indication:
      return "Rx_Data.indication";
    case message_type_id::slot_indication:
      return "SLOT.indication";
    case message_type_id::config_request:
      return "CONFIG.request";
    case message_type_id::config_response:
      return "CONFIG.response";
    case message_type_id::dl_tti_response:
      return "DL_TTI.response";
    case message_type_id::param_request:
      return "PARAM.request";
    case message_type_id::param_response:
      return "PARAM.response";
    case message_type_id::srs_indication:
      return "SRS.indication";
    case message_type_id::start_request:
      return "START.request";
    case message_type_id::stop_indication:
      return "STOP.indication";
    case message_type_id::stop_request:
      return "STOP.request";
    case message_type_id::ul_tti_request:
      return "UL_TTI.request";
    default:
      srsran_assert(0, "Invalid FAPI message type={}", msg_id);
      break;
  }
  return "";
}

static void log_pdu_and_range_report(fmt::memory_buffer& buffer, const validator_report::error_report& report)
{
  fmt::format_to(buffer,
                 "\t- PDU type={}, property={}, value={}, expected value=[{}-{}]\n",
                 get_pdu_type_string(report.message_type, report.pdu_type.value()),
                 report.property_name,
                 report.value,
                 report.expected_value_range.value().first,
                 report.expected_value_range.value().second);
}

static void log_pdu_report(fmt::memory_buffer& buffer, const validator_report::error_report& report)
{
  fmt::format_to(buffer,
                 "\t- PDU type={}, property={}, value={}\n",
                 get_pdu_type_string(report.message_type, report.pdu_type.value()),
                 report.property_name,
                 report.value);
}

static void log_range_report(fmt::memory_buffer& buffer, const validator_report::error_report& report)
{
  fmt::format_to(buffer,
                 "\t- Property={}, value={}, expected value=[{}-{}]\n",
                 report.property_name,
                 report.value,
                 report.expected_value_range.value().first,
                 report.expected_value_range.value().second);
}

static void log_basic_report(fmt::memory_buffer& buffer, const validator_report::error_report& report)
{
  fmt::format_to(buffer, "\t- Property={}, value={}\n", report.property_name, report.value);
}

void srsran::fapi::log_validator_report(const validator_report& report, srslog::basic_logger& logger)
{
  fmt::memory_buffer str_buffer;
  fmt::format_to(str_buffer,
                 "Detected {} error(s) in message type '{}' in slot={}.{}:\n",
                 report.reports.size(),
                 get_message_type_string(report.reports.front().message_type),
                 report.sfn,
                 report.slot);

  for (const auto& error : report.reports) {
    // Basic + PDU + Range log.
    if (error.pdu_type.has_value() && error.expected_value_range.has_value()) {
      log_pdu_and_range_report(str_buffer, error);
      continue;
    }

    // Basic + PDU log.
    if (error.pdu_type.has_value()) {
      log_pdu_report(str_buffer, error);
      continue;
    }

    // Basic + Range log.
    if (error.expected_value_range.has_value()) {
      log_range_report(str_buffer, error);
      continue;
    }

    // Basic log.
    log_basic_report(str_buffer, error);
  }

  logger.warning("{}", to_c_str(str_buffer));
}
