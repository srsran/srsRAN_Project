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

#include "dl_prs_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages/dl_tti_request.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::dl_tti_request;

/// This validator checks the PRS PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(dl_pdu_type::PRS);

/// Validates the subcarrier spacing property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_scs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the cyclix prefix property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_cp(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic prefix", msg_type, pdu_type, report);
}

/// Validates the n ID property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_n_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4095;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "n ID PRS", msg_type, pdu_type, report);
}

/// Validates the comb size property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_comb_size(unsigned value, validator_report& report)
{
  if (value == 2 || value == 4 || value == 6 || value == 12) {
    return true;
  }
  report.append(value, "Comb size", msg_type, pdu_type);
  return false;
}

/// Validates the comb offset property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_comb_offset(unsigned value, unsigned comb_size, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;

  return validate_field(MIN_VALUE, comb_size - 1U, value, "Comb offset", msg_type, pdu_type, report);
}

/// Validates the number of symbols property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_nof_symbols(unsigned value, validator_report& report)
{
  if (value == 2 || value == 4 || value == 6 || value == 12) {
    return true;
  }
  report.append(value, "Number of symbols", msg_type, pdu_type);
  return false;
}

/// Validates the symbol offset property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_symbol_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 12;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Symbol offset", msg_type, pdu_type, report);
}

/// Validates the number of RBs property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_nof_rbs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 24;
  static constexpr unsigned MAX_VALUE = 272;

  if (value == 0 || value % 4) {
    report.append(value, "Number of RBs", msg_type, pdu_type);

    return false;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of RBs", msg_type, pdu_type, report);
}

/// Validates the start RB property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_start_rb(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2176;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PRB offset", msg_type, pdu_type, report);
}

/// Validates the power offset property of the PRS PDU, as per SCF-222 v8.0 section 3.4.2.4a.
static bool validate_power_offset(float value, validator_report& report)
{
  static constexpr int MIN_VALUE = -32767;
  static constexpr int MAX_VALUE = 32767;

  int power = value / 1000;

  return validate_field(MIN_VALUE, MAX_VALUE, power, "PRS power offset", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_dl_prs_pdu(const dl_prs_pdu& pdu, validator_report& report)
{
  bool result = true;

  result &= validate_scs(static_cast<unsigned>(pdu.scs), report);
  result &= validate_cp(static_cast<unsigned>(pdu.cp), report);
  result &= validate_n_id(pdu.nid_prs, report);
  result &= validate_comb_size(static_cast<unsigned>(pdu.comb_size), report);
  result &= validate_comb_offset(pdu.comb_offset, static_cast<unsigned>(pdu.comb_size), report);
  result &= validate_nof_symbols(static_cast<unsigned>(pdu.num_symbols), report);
  result &= validate_symbol_offset(pdu.first_symbol, report);
  result &= validate_nof_rbs(pdu.num_rbs, report);
  result &= validate_start_rb(pdu.start_rb, report);
  if (pdu.prs_power_offset) {
    result &= validate_power_offset(pdu.prs_power_offset.value(), report);
  }
  // PDU index uses the whole range of the uint16_t, so it will not be validated.

  return result;
}
