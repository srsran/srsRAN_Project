/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_validators.h"
#include "dl_csi_pdu.h"
#include "dl_pdcch_pdu.h"
#include "dl_pdsch_pdu.h"
#include "dl_ssb_pdu.h"
#include "helpers.h"
#include "uci_pdus.h"
#include "ul_prach_pdu.h"
#include "ul_pucch_pdu.h"
#include "ul_pusch_pdu.h"

using namespace srsgnb;
using namespace fapi;

/// Validates the SFN property of a message.
static bool validate_sfn(unsigned value, message_type_id msg_type, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "sfn", msg_type, report);
}

/// Validates the slot property of any message.
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

error_type<validator_report> srsgnb::fapi::validate_dl_tti_request(const dl_tti_request_message& msg)
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
        srsgnb_assert(0, "Invalid pdu_type");
        report.append(static_cast<int>(pdu.pdu_type), "DL_TTI.request PDU type", message_type_id::dl_tti_request);
        break;
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

error_type<validator_report> srsgnb::fapi::validate_ul_dci_request(const ul_dci_request_message& msg)
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
        // :TODO: Implement the rest of the PDUs when their validators exist.
      default:
        srsgnb_assert(0, "Invalid pdu_type");
        report.append(static_cast<int>(pdu.pdu_type), "UL_DCI.request PDU type", message_type_id::ul_dci_request);
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

error_type<validator_report> srsgnb::fapi::validate_tx_data_request(const tx_data_request_message& msg)
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
///// and 3.4.7.
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
///// and 3.4.7.
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

error_type<validator_report> srsgnb::fapi::validate_crc_indication(const crc_indication_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, message_type_id::crc_indication, report);
  success &= validate_slot(msg.slot, message_type_id::crc_indication, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    // NOTE: Handle property will no be validated as the values are not specified in the document.
    success &= validate_rnti(pdu.rnti, message_type_id::crc_indication, report);
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

error_type<validator_report> srsgnb::fapi::validate_rach_indication(const rach_indication_message& msg)
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
      // NOTE: Preamble SNR property uses the whole range of the property,so it will not be validated.
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

error_type<validator_report> srsgnb::fapi::validate_uci_indication(const uci_indication_message& msg)
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
        srsgnb_assert(0, "Invalid pdu_type");
        report.append(static_cast<int>(pdu.pdu_type), "UCI.indication PDU type", message_type_id::uci_indication);
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

error_type<validator_report> srsgnb::fapi::validate_error_indication(const error_indication_message& msg)
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

error_type<validator_report> srsgnb::fapi::validate_rx_data_indication(const rx_data_indication_message& msg)
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
    success &= validate_rnti(pdu.rnti, msg_id, report);
    success &= validate_rapid(pdu.rapid, msg_id, report);
    success &= validate_harq_id(pdu.harq_id, msg_id, report);
    // NOTE: PDU length property will not be validated.
    success &= validate_pdu_tag(pdu.pdu_tag, report);
    success &= validate_pdu_value(pdu.data, report);
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

error_type<validator_report> srsgnb::fapi::validate_slot_indication(const slot_indication_message& msg)
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

error_type<validator_report> srsgnb::fapi::validate_ul_tti_request(const ul_tti_request_message& msg)
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
        srsgnb_assert(0, "Invalid pdu_type");
        report.append(static_cast<int>(pdu.pdu_type), "UL_TTI.request PDU type", msg_type);
        break;
    }
  }

  // Build the result.
  if (!success) {
    return {std::move(report)};
  }

  return {};
}

void srsgnb::fapi::log_validator_report(const validator_report& report)
{
  static const std::string log_name = "FAPI";

  srslog::fetch_basic_logger(log_name).error(
      "FAPI message bad-formed in SFN ({}) and slot ({}).", report.sfn, report.slot);
  for (const auto& error : report.reports) {
    srslog::fetch_basic_logger(log_name).error(
        "Error message ({}), PDU ({}),  in property ({}), value ({}), expected [{} - {}]",
        error.message_type,
        error.pdu_type.has_value() ? error.pdu_type.value() : -1,
        error.property_name,
        error.value,
        error.expected_value_range.first,
        error.expected_value_range.second);
  }
}