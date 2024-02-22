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

#include "dl_pdcch_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/validator_report.h"

using namespace srsran;
using namespace fapi;

/// This validator checks the PDCCH PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(dl_pdu_type::PDCCH);

/// Validates the CORESET BWP size property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_coreset_bwp_size(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CORESET BWP size", msg_type, pdu_type, report);
}

/// Validates the CORESET BWP start property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_coreset_bwp_start(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CORESET BWP start", msg_type, pdu_type, report);
}

/// Validates the subcarrier spacing property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_scs(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the cyclic prefix property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_cp(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic prefix", msg_type, pdu_type, report);
}

/// Validates the start symbol index property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_start_symbol_index(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Start symbol index", msg_type, pdu_type, report);
}

/// Validates the duration symbols property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_duration_symbols(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Duration symbols", msg_type, pdu_type, report);
}

/// Validates the CCE to REG mapping type property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_cce_reg_mapping_type(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CCE to REG mapping type", msg_type, pdu_type, report);
}

/// Validates the REG bundle size property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_reg_bundle_size(message_type_id         msg_type,
                                     unsigned                value,
                                     cce_to_reg_mapping_type cce_type,
                                     unsigned                duration,
                                     validator_report&       report)
{
  if (cce_type == cce_to_reg_mapping_type::interleaved) {
    if (duration == 3) {
      if (value == 3 || value == 6) {
        return true;
      }
    } else {
      if (value == 2 || value == 6) {
        return true;
      }
    }
  } else {
    if (value == 6) {
      return true;
    }
  }

  report.append(value, "REG bundle size", msg_type, pdu_type);
  return false;
}

/// Validates the interleaver size property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_interleaver_size(message_type_id msg_type, unsigned value, validator_report& report)
{
  if (value == 2 || value == 3 || value == 6) {
    return true;
  }

  report.append(value, "Interleaver size", msg_type, pdu_type);
  return false;
}

/// Validates the CORESET type property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_coreset_type(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CORESET type", msg_type, pdu_type, report);
}

/// Validates the shift index property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_shift_index(message_type_id    msg_type,
                                 unsigned           value,
                                 pdcch_coreset_type coreset_type,
                                 validator_report&  report)
{
  if (coreset_type == pdcch_coreset_type::pbch_or_coreset0) {
    static constexpr unsigned MIN_VALUE = 0;
    static constexpr unsigned MAX_VALUE = 1023;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "Shift index - PBCH or SIB1", msg_type, pdu_type, report);
  }

  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Shift index - Other", msg_type, pdu_type, report);
}

/// Validates the precoder granularity property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_precoder_granularity(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Precoder granularity", msg_type, pdu_type, report);
}

/// Validates the RNTI property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_rnti(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 65535;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RNTI", msg_type, pdu_type, report);
}

/// Validates the CCE index property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_cce_index(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 135;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CCE index", msg_type, pdu_type, report);
}

/// Validates the aggregation level property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_aggregation_level(message_type_id msg_type, unsigned value, validator_report& report)
{
  if (value == 1U || value == 2U || value == 4U || value == 8U || value == 16U) {
    return true;
  }

  report.append(value, "Aggregation level", msg_type, pdu_type);
  return false;
}

/// Validates the power control offset SS profile NR and the PDCCH DMRS power offset profile SSS properties of the PDCCH
/// PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_power_control_offset_profile_nr(message_type_id   msg_type,
                                                     int               power_control_offset_ss_profile_nr,
                                                     int               dmrs_power_profile_sss,
                                                     validator_report& report)
{
  static constexpr int USE_PROFILE_SSS       = -127;
  static constexpr int MIN_VALUE_PROFILE_NR  = -8;
  static constexpr int MAX_VALUE_PROFILE_NR  = 8;
  static constexpr int USE_PROFILE_NR        = -32768;
  static constexpr int MIN_VALUE_PROFILE_SSS = -32767;
  static constexpr int MAX_VALUE_PROFILE_SSS = 32767;

  if (power_control_offset_ss_profile_nr == USE_PROFILE_SSS && MIN_VALUE_PROFILE_SSS <= dmrs_power_profile_sss &&
      dmrs_power_profile_sss <= MAX_VALUE_PROFILE_SSS) {
    return true;
  }

  if (dmrs_power_profile_sss == USE_PROFILE_NR && MIN_VALUE_PROFILE_NR <= power_control_offset_ss_profile_nr &&
      power_control_offset_ss_profile_nr <= MAX_VALUE_PROFILE_NR) {
    return true;
  }

  report.append(dmrs_power_profile_sss, "DMRS Power offset", msg_type, pdu_type);
  return false;
}

/// Validates the collocated AL16 candidate property of the PDCCH PDU, as per SCF-222 v4.0 section 3.4.2.1.
static bool validate_collocated_al16_candidate(message_type_id msg_type, unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Collocated AL16 candidate", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_dl_pdcch_pdu(message_type_id msg_type, const dl_pdcch_pdu& pdu, validator_report& report)
{
  bool result = true;

  result &= validate_coreset_bwp_size(msg_type, pdu.coreset_bwp_size, report);
  result &= validate_coreset_bwp_start(msg_type, pdu.coreset_bwp_start, report);
  result &= validate_scs(msg_type, static_cast<unsigned>(pdu.scs), report);
  result &= validate_cp(msg_type, pdu.cp.value, report);
  result &= validate_start_symbol_index(msg_type, pdu.start_symbol_index, report);
  result &= validate_duration_symbols(msg_type, pdu.duration_symbols, report);
  result &= validate_cce_reg_mapping_type(msg_type, static_cast<unsigned>(pdu.cce_reg_mapping_type), report);
  result &=
      validate_reg_bundle_size(msg_type, pdu.reg_bundle_size, pdu.cce_reg_mapping_type, pdu.duration_symbols, report);
  result &= validate_coreset_type(msg_type, static_cast<unsigned>(pdu.coreset_type), report);
  if (pdu.cce_reg_mapping_type == cce_to_reg_mapping_type::interleaved) {
    result &= validate_interleaver_size(msg_type, pdu.interleaver_size, report);
    result &= validate_shift_index(msg_type, pdu.shift_index, pdu.coreset_type, report);
  }
  result &= validate_precoder_granularity(msg_type, static_cast<unsigned>(pdu.precoder_granularity), report);

  // Validate DL DCIs.
  for (unsigned i = 0, e = pdu.dl_dci.size(); i != e; ++i) {
    const auto& dci    = pdu.dl_dci[i];
    const auto& dci_v3 = pdu.maintenance_v3.info[i];

    result &= validate_rnti(msg_type, static_cast<unsigned>(dci.rnti), report);
    // NOTE: N-Id PDCCH data field uses the whole range of the variable, so it will not be checked.
    // NOTE: N-RNTI PDCCH data field uses the whole range of the variable, so it will not be checked.
    result &= validate_cce_index(msg_type, dci.cce_index, report);
    result &= validate_aggregation_level(msg_type, dci.aggregation_level, report);
    result &= validate_power_control_offset_profile_nr(
        msg_type, dci.power_control_offset_ss_profile_nr, dci_v3.pdcch_dmrs_power_offset_profile_sss, report);
    // NOTE: DCI index uses the whole range of the variable, so it will not be checked.
    result &= validate_collocated_al16_candidate(msg_type, dci_v3.collocated_AL16_candidate, report);
    // NOTE: PDCCH DMRS power offset profile NR uses the whole range of the variable, so it will not be checked.
    // NOTE: PDCCH Data power offset profile NR uses the whole range of the variable, so it will not be checked.
    // NOTE: N-ID uses the whole range of the variable, so it will not be checked.
  }

  return result;
}
