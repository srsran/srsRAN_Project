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

#include "dl_ssb_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/validator_report.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::dl_tti_request;

/// This validator checks the SSB PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(dl_pdu_type::SSB);

/// Validates the PCI property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_phy_cell_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1007;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Physical cell ID", msg_type, pdu_type, report);
}

/// Validates the Beta PSS profine NR property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_beta_pss_profile_nr(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Beta PSS profile NR", msg_type, pdu_type, report);
}

/// Validates the block index property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_block_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "SS/PBCH block index", msg_type, pdu_type, report);
}

/// Validates the subcarrier offset property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_subcarrier_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 31;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier offset", msg_type, pdu_type, report);
}

/// Validates the offset point A property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4.
static bool validate_offset_point_A(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2199;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Offset point A", msg_type, pdu_type, report);
}

/// Validates the DMRS type A position property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4 in table PHY
/// MIB.
static bool validate_dmrs_type_a_position(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "DMRS type A position", msg_type, pdu_type, report);
}

/// Validates the case property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4 in table SSB/PBCH PDU maintenance
/// FAPIv3.
static bool validate_case(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "SSB Case type", msg_type, pdu_type, report);
}

/// Validates the subcarrier spacing property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4 in table SSB/PBCH
/// PDU maintenance FAPIv3.
static bool validate_subcarrier_spacing(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the LMax property of the SSB PDU, as per SCF-222 v4.0 section 3.4.2.4 in table SSB/PBCH PDU  maintenance
/// FAPIv3.
static bool validate_L_max(unsigned value, validator_report& report)
{
  if (value == 4 || value == 8 || value == 64) {
    return true;
  }

  report.append(value, "L_max", msg_type, pdu_type);
  return false;
}

bool srsran::fapi::validate_dl_ssb_pdu(const dl_ssb_pdu& pdu, validator_report& report)
{
  bool result = true;

  result &= validate_phy_cell_id(pdu.phys_cell_id, report);
  result &= validate_beta_pss_profile_nr(static_cast<unsigned>(pdu.beta_pss_profile_nr), report);
  result &= validate_block_index(pdu.ssb_block_index, report);
  result &= validate_subcarrier_offset(pdu.ssb_subcarrier_offset, report);
  result &= validate_offset_point_A(pdu.ssb_offset_pointA.to_uint(), report);
  // NOTE: BCH payload property will not be validated.
  if (pdu.bch_payload_flag == bch_payload_type::phy_full) {
    result &= validate_dmrs_type_a_position(
        static_cast<std::underlying_type_t<fapi::dmrs_typeA_pos>>(pdu.bch_payload.phy_mib_pdu.dmrs_typeA_position),
        report);
    // NOTE: PDCCH config SIB1 property uses the whole range and will not be validated.
    // NOTE: Cell barred property uses the whole range and will not be validated.
    // NOTE: Intra freq reselection property uses the whole range and will not be validated.
  }

  // NOTE: SSB PDU index property will not be validated, as its range is not defined.
  result &= validate_case(static_cast<unsigned>(pdu.ssb_maintenance_v3.case_type), report);
  result &= validate_subcarrier_spacing(static_cast<unsigned>(pdu.ssb_maintenance_v3.scs), report);
  result &= validate_L_max(pdu.ssb_maintenance_v3.L_max, report);

  return result;
}
