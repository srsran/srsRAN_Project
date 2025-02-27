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

#include "ul_srs_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages/ul_tti_request.h"
#include "srsran/ran/srs/srs_configuration.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a UL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::ul_tti_request;

/// This validator checks the SRS PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(ul_pdu_type::SRS);

/// Validates the RNTI property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_rnti(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 65535;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RNTI", msg_type, pdu_type, report);
}

/// Validates the BWP size property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_bwp_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP size", msg_type, pdu_type, report);
}

/// Validates the BWP start property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_bwp_start(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP start", msg_type, pdu_type, report);
}

/// Validates the subcarrier spacing property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_scs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the cyclic prefix property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_cyclic_prefix(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic prefix", msg_type, pdu_type, report);
}

/// Validates the number of antenna ports property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_nof_antenna_ports(unsigned value, validator_report& report)
{
  if (value == 1 || value == 2 || value == 4) {
    return true;
  }

  report.append(value, "Number of antenna ports", msg_type, pdu_type);

  return false;
}

/// Validates the number of symbols property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_nof_symbols(unsigned value, validator_report& report)
{
  if (value == 1 || value == 2 || value == 4) {
    return true;
  }

  report.append(value, "Number of symbols", msg_type, pdu_type);

  return false;
}

/// Validates the number of repetitions property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_nof_repetitions(srs_nof_symbols value, validator_report& report)
{
  if (value == n1 || value == n2 || value == n4) {
    return true;
  }

  report.append(static_cast<unsigned>(value), "Number of repetitions", msg_type, pdu_type);

  return false;
}

/// Validates the time start position property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_time_start_position(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Time start position", msg_type, pdu_type, report);
}

/// Validates the config index property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_config_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 63;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Config index", msg_type, pdu_type, report);
}

/// Validates the sequence identifier property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_sequence_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Sequence identifier", msg_type, pdu_type, report);
}

/// Validates the bandwidth index property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_bandwidth_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Bandwidth index", msg_type, pdu_type, report);
}

/// Validates the comb size property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_comb_size(tx_comb_size value, validator_report& report)
{
  if (value == tx_comb_size::n2 || value == tx_comb_size::n4) {
    return true;
  }

  report.append(static_cast<unsigned>(value), "Comb size", msg_type, pdu_type);

  return false;
}

/// Validates the comb offset property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_comb_offset(tx_comb_size size, unsigned value, validator_report& report)
{
  if (size == tx_comb_size::n2) {
    static constexpr unsigned MIN_VALUE = 0;
    static constexpr unsigned MAX_VALUE = 1;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "Comb offset", msg_type, pdu_type, report);
  }

  if (size == tx_comb_size::n4) {
    static constexpr unsigned MIN_VALUE = 0;
    static constexpr unsigned MAX_VALUE = 3;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "Comb offset", msg_type, pdu_type, report);
  }

  report.append(static_cast<unsigned>(value), "Comb ofsset", msg_type, pdu_type);

  return false;
}

/// Validates the cyclic shift property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_cyclic_shift(tx_comb_size size, unsigned value, validator_report& report)
{
  if (size == tx_comb_size::n2) {
    static constexpr unsigned MIN_VALUE = 0;
    static constexpr unsigned MAX_VALUE = 7;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic shift", msg_type, pdu_type, report);
  }

  if (size == tx_comb_size::n4) {
    static constexpr unsigned MIN_VALUE = 0;
    static constexpr unsigned MAX_VALUE = 11;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic shift", msg_type, pdu_type, report);
  }

  report.append(static_cast<unsigned>(value), "Cyclic shift", msg_type, pdu_type);

  return false;
}

/// Validates the frequency position property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_frequency_position(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 67;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Frequency position", msg_type, pdu_type, report);
}

/// Validates the frequency shift property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_frequency_shift(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 268;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Frequency shift", msg_type, pdu_type, report);
}

/// Validates the frequency hopping property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_frequency_hopping(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Frequency hopping", msg_type, pdu_type, report);
}

/// Validates the group or sequence hopping property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_group_or_sequence_hopping(srs_group_or_sequence_hopping value, validator_report& report)
{
  if (value == srs_group_or_sequence_hopping::group_hopping ||
      value == srs_group_or_sequence_hopping::sequence_hopping || value == srs_group_or_sequence_hopping::neither) {
    return true;
  }

  report.append(static_cast<unsigned>(value), "Group or sequence hopping", msg_type, pdu_type);

  return false;
}

/// Validates the resource type property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_resource_type(srs_resource_type value, validator_report& report)
{
  if (value == srs_resource_type::aperiodic || value == srs_resource_type::periodic ||
      value == srs_resource_type::semi_persistent) {
    return true;
  }

  report.append(static_cast<unsigned>(value), "Resource type", msg_type, pdu_type);

  return false;
}

/// Validates the periodicity property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_t_srs(srs_periodicity value, validator_report& report)
{
  if (value == srs_periodicity::sl1 || value == srs_periodicity::sl2 || value == srs_periodicity::sl4 ||
      value == srs_periodicity::sl5 || value == srs_periodicity::sl8 || value == srs_periodicity::sl10 ||
      value == srs_periodicity::sl16 || value == srs_periodicity::sl20 || value == srs_periodicity::sl32 ||
      value == srs_periodicity::sl40 || value == srs_periodicity::sl64 || value == srs_periodicity::sl80 ||
      value == srs_periodicity::sl160 || value == srs_periodicity::sl320 || value == srs_periodicity::sl640 ||
      value == srs_periodicity::sl1280 || value == srs_periodicity::sl2560) {
    return true;
  }

  report.append(static_cast<unsigned>(value), "T-SRS", msg_type, pdu_type);

  return false;
}

/// Validates the slot offset property of the SRS PDU, as per SCF-222 v4.0 Section 3.4.3.3
static bool validate_t_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2559;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Slot offset value", msg_type, pdu_type, report);
}

bool fapi::validate_ul_srs_pdu(const ul_srs_pdu& pdu, validator_report& report)
{
  bool result = true;

  // NOTE: pdu Bitmap property range is not specified and will not be validated.
  result &= validate_rnti(static_cast<unsigned>(pdu.rnti), report);
  // NOTE: Handle property range is not specified.
  result &= validate_bwp_size(pdu.bwp_size, report);
  result &= validate_bwp_start(pdu.bwp_start, report);
  result &= validate_scs(static_cast<unsigned>(pdu.scs), report);
  result &= validate_cyclic_prefix(static_cast<unsigned>(pdu.cp), report);
  result &= validate_nof_antenna_ports(pdu.num_ant_ports, report);
  result &= validate_nof_symbols(pdu.num_symbols, report);
  result &= validate_nof_repetitions(pdu.num_repetitions, report);
  result &= validate_time_start_position(pdu.time_start_position, report);
  result &= validate_config_index(pdu.config_index, report);
  result &= validate_sequence_id(pdu.sequence_id, report);
  result &= validate_bandwidth_index(pdu.bandwidth_index, report);
  result &= validate_comb_size(pdu.comb_size, report);
  result &= validate_comb_offset(pdu.comb_size, pdu.comb_offset, report);
  result &= validate_cyclic_shift(pdu.comb_size, pdu.cyclic_shift, report);
  result &= validate_frequency_position(pdu.frequency_position, report);
  result &= validate_frequency_shift(pdu.frequency_shift, report);
  result &= validate_frequency_hopping(pdu.frequency_hopping, report);
  result &= validate_group_or_sequence_hopping(pdu.group_or_sequence_hopping, report);
  result &= validate_resource_type(pdu.resource_type, report);
  result &= validate_t_srs(pdu.t_srs, report);
  result &= validate_t_offset(pdu.t_offset, report);

  return result;
}
