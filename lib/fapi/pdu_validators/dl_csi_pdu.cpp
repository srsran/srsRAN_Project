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

#include "dl_csi_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/validator_report.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::dl_tti_request;

/// This validator checks the CSI-RS PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(dl_pdu_type::CSI_RS);

/// Validates the subcarrier spacing property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_scs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the cyclic prefix property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_cp(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic prefix", msg_type, pdu_type, report);
}

/// Validates the start RB property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_start_rb(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Start RB", msg_type, pdu_type, report);
}

/// Validates the number of RBs property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_nof_rb(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 24;
  static constexpr unsigned MAX_VALUE = 276;

  if (value % 4 != 0) {
    report.append(value, std::make_pair(MIN_VALUE, MAX_VALUE), "Number of RBs", msg_type, pdu_type);
    return false;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of RBs", msg_type, pdu_type, report);
}

/// Validates the CSI type property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_csi_type(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CSI type", msg_type, pdu_type, report);
}

/// Validates the row property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_row(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 18;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Row", msg_type, pdu_type, report);
}

/// Validates the symbol L0 property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_symbol_l0(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Symbol L0", msg_type, pdu_type, report);
}

/// Validates the symbol L1 property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_symbol_l1(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 2;
  static constexpr unsigned MAX_VALUE = 12;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Symbol L1", msg_type, pdu_type, report);
}

/// Validates the CDM type property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_cdm_type(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CDM type", msg_type, pdu_type, report);
}

/// Validates the freq density property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_freq_density(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Freq density", msg_type, pdu_type, report);
}

/// Validates the scrambling id property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_scrambling_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Scrambling ID", msg_type, pdu_type, report);
}

/// Validates the power control offset SS profile NR and the CSI-RS power offset profile SSS properties of the CSI PDU,
/// as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_power_control_offset_profile_nr(unsigned          power_control_offset_ss_profile_nr,
                                                     int               profile_sss,
                                                     validator_report& report)
{
  static constexpr unsigned USE_PROFILE_SSS       = 255;
  static constexpr unsigned MIN_VALUE_PROFILE_NR  = 0;
  static constexpr unsigned MAX_VALUE_PROFILE_NR  = 23;
  static constexpr int      USE_PROFILE_NR        = -32768;
  static constexpr int      MIN_VALUE_PROFILE_SSS = -32767;
  static constexpr int      MAX_VALUE_PROFILE_SSS = 32767;

  if (power_control_offset_ss_profile_nr == USE_PROFILE_SSS && MIN_VALUE_PROFILE_SSS <= profile_sss &&
      profile_sss <= MAX_VALUE_PROFILE_SSS) {
    return true;
  }

  if (profile_sss == USE_PROFILE_NR && MIN_VALUE_PROFILE_NR <= power_control_offset_ss_profile_nr &&
      power_control_offset_ss_profile_nr <= MAX_VALUE_PROFILE_NR) {
    return true;
  }

  report.append(power_control_offset_ss_profile_nr, "Power control offset", msg_type, pdu_type);
  return false;
}

/// Validates the power control offset SS profile NR property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_power_control_offset_ss_profile_nr(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  if (value == 255) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Power control offset SS profile NR", msg_type, pdu_type, report);
}

/// Validates the power offset profile SSS property of the CSI PDU, as per SCF-222 v4.0 section 3.4.2.3.
static bool validate_power_offset_profile_sss(int value, validator_report& report)
{
  static constexpr int USE_PROFILE_NR = -32768;
  static constexpr int MIN_VALUE      = -32767;
  static constexpr int MAX_VALUE      = 32767;

  if (value == USE_PROFILE_NR) {
    return true;
  }

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Power offset profile SSS", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_dl_csi_pdu(const dl_csi_rs_pdu& pdu, validator_report& report)
{
  bool result = true;

  result &= validate_scs(static_cast<unsigned>(pdu.scs), report);
  result &= validate_cp(static_cast<unsigned>(pdu.cp), report);
  result &= validate_start_rb(pdu.start_rb, report);
  result &= validate_nof_rb(pdu.num_rbs, report);
  result &= validate_csi_type(static_cast<unsigned>(pdu.type), report);
  result &= validate_row(pdu.row, report);
  // NOTE: Freq domain bitmap property will not be validated.
  result &= validate_symbol_l0(pdu.symb_L0, report);
  result &= validate_symbol_l1(pdu.symb_L1, report);
  result &= validate_cdm_type(static_cast<unsigned>(pdu.cdm_type), report);
  result &= validate_freq_density(static_cast<unsigned>(pdu.freq_density), report);
  result &= validate_scrambling_id(pdu.scramb_id, report);
  result &= validate_power_control_offset_profile_nr(
      pdu.power_control_offset_profile_nr, pdu.csi_rs_maintenance_v3.csi_rs_power_offset_profile_sss, report);
  result &= validate_power_control_offset_ss_profile_nr(static_cast<unsigned>(pdu.power_control_offset_ss_profile_nr),
                                                        report);
  // NOTE: CSI-RS PDU index will not be validated.
  result &= validate_power_offset_profile_sss(pdu.csi_rs_maintenance_v3.csi_rs_power_offset_profile_sss, report);

  return result;
}
