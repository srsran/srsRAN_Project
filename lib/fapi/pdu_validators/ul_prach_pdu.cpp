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

#include "ul_prach_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/validator_report.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::ul_tti_request;

/// This validator checks the PRACH PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(ul_pdu_type::PRACH);

/// Validates the physical cell ID property of the PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1 in table PRACH PDU.
static bool validate_phy_cell_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1007;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Physical cell ID", msg_type, pdu_type, report);
}

/// Validates the number of time-domain PRACH occasions property of the PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1
/// in table PRACH PDU.
static bool validate_num_prach_ocas(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 7;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of PRACH occasions", msg_type, pdu_type, report);
}

/// Validates the PRACH format property of the PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1 in table PRACH PDU.
static bool validate_prach_format(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PRACH format", msg_type, pdu_type, report);
}

/// Validates the frequency domain occasion index property of the PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1 in
/// table PRACH PDU.
static bool validate_index_fd_ra(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 7;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Frequency domain occasion index", msg_type, pdu_type, report);
}

/// Validates the PRACH start symbol property of the PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1 in table PRACH PDU.
static bool validate_prach_start_symbol(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PRACH start symbol", msg_type, pdu_type, report);
}

/// Validates the zero-correlation zone configuration number property of the PRACH PDU, as per SCF-222 v4.0
/// section 3.4.3.1 in table PRACH PDU.
static bool validate_num_cs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 419;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Zero-correlation zone configuration number", msg_type, pdu_type, report);
}

/// Validates the is msgA-PRACH property of the PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1 in table PRACH PDU.
static bool validate_is_msg_a_prach(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Is msgA-PRACH", msg_type, pdu_type, report);
}

/// Validates the PRACH config scope property of the maintenance v3 PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1 in
/// table PRACH maintenance FAPIv3.
static bool validate_prach_config_scope(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PRACH config scope", msg_type, pdu_type, report);
}

/// Validates the number of frequency domain occasions property of the maintenance v3 PRACH PDU, as per SCF-222 v4.0
/// section 3.4.3.1 in table PRACH maintenance FAPIv3.
static bool validate_num_fd_ra(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 8;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Number of frequency domain occasions", msg_type, pdu_type, report);
}

/// Validates the start preamble index property of the maintenance v3 PRACH PDU, as per SCF-222 v4.0 section 3.4.3.1 in
/// table PRACH maintenance FAPIv3.
static bool validate_start_preamble_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE                        = 0;
  static constexpr unsigned MAX_VALUE                        = 63;
  static constexpr unsigned LINKED_BY_PRACH_RES_CONFIG_INDEX = 255;

  if (value == LINKED_BY_PRACH_RES_CONFIG_INDEX) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Start preamble index", msg_type, pdu_type, report);
}

/// Validates the number of preamble logical indices property of the maintenance v3 PRACH PDU, as per SCF-222 v4.0
/// section 3.4.3.1 in table PRACH maintenance FAPIv3.
static bool validate_num_preambles_indices(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 64;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of preambles logical indices", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_ul_prach_pdu(const ul_prach_pdu& pdu, validator_report& report)
{
  bool result = true;

  result &= validate_phy_cell_id(pdu.phys_cell_id, report);
  result &= validate_num_prach_ocas(pdu.num_prach_ocas, report);
  result &= validate_prach_format(static_cast<unsigned>(pdu.prach_format), report);
  result &= validate_index_fd_ra(pdu.index_fd_ra, report);
  result &= validate_prach_start_symbol(pdu.prach_start_symbol, report);
  result &= validate_num_cs(pdu.num_cs, report);
  result &= validate_is_msg_a_prach(pdu.is_msg_a_prach, report);
  // NOTE: Has msgA-PUSCH beamforming property uses the whole range of the variable, so it will not be validated.

  // Maintenance v3 parameters.
  // NOTE: Handle property will not be validated.
  const auto& v3 = pdu.maintenance_v3;
  result &= validate_prach_config_scope(static_cast<unsigned>(v3.prach_config_scope), report);
  // NOTE: PRACH res config index will not be validated.
  result &= validate_num_fd_ra(v3.num_fd_ra, report);
  result &= validate_start_preamble_index(v3.start_preamble_index, report);
  result &= validate_num_preambles_indices(v3.num_preamble_indices, report);

  return result;
}
