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
#include "dl_pdcch_pdu.h"
#include "dl_pdsch_pdu.h"
#include "dl_ssb_pdu.h"
#include "helpers.h"

using namespace srsgnb;
using namespace fapi;

/// Validates the SFN property of a message.
static bool validate_sfn(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "sfn", report);
}

/// Validates the slot property of any message.
static bool validate_slot(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 159;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "slot", report);
}

error_type<validator_report> srsgnb::fapi::validate_dl_tti_request(const dl_tti_request_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, report);
  success &= validate_slot(msg.slot, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case dl_pdu_type::SSB:
        success &= validate_dl_ssb_pdu(pdu.ssb_pdu, report);
        break;
      case dl_pdu_type::PDCCH:
        success &= validate_dl_pdcch_pdu(pdu.pdcch_pdu, report);
        break;
      case dl_pdu_type::PDSCH:
        success &= validate_dl_pdsch_pdu(pdu.pdsch_pdu, report);
        break;
        // :TODO: Implement the rest of the PDUs when their validators exist.
      default:
        srsran_assert(0, "Invalid pdu_type");
        report.append(static_cast<unsigned>(pdu.pdu_type), "DL_TTI.request PDU type");
        break;
    }
  }

  // Build the result.
  if (!success) {
    return error_type<validator_report>(std::move(report));
  }

  return {};
}

error_type<validator_report> srsgnb::fapi::validate_ul_dci_request(const ul_dci_request_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, report);
  success &= validate_slot(msg.slot, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case ul_dci_pdu_type::PDCCH:
        success &= validate_dl_pdcch_pdu(pdu.pdu, report);
        break;
        // :TODO: Implement the rest of the PDUs when their validators exist.
      default:
        srsran_assert(0, "Invalid pdu_type");
        report.append(static_cast<unsigned>(pdu.pdu_type), "UL_DCI.request PDU type");
        break;
    }
  }

  // Build the result.
  if (!success) {
    return error_type<validator_report>(std::move(report));
  }

  return {};
}
