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

#include "ul_pucch_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/validator_report.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::ul_tti_request;

/// This validator checks the PRACH PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(ul_pdu_type::PUCCH);

/// Validates the RNTI property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_rnti(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 65535;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RNTI", msg_type, pdu_type, report);
}

/// Validates the BWP size property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_bwp_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP size", msg_type, pdu_type, report);
}

/// Validates the BWP start property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_bwp_start(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP start", msg_type, pdu_type, report);
}

/// Validates the subcarrier spacing property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_scs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the cyclic prefix property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_cyclic_prefix(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic prefix", msg_type, pdu_type, report);
}

/// Validates the format type property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_format_type(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Format type", msg_type, pdu_type, report);
}

/// Validates the multi slot tx indicator property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_multi_slot_tx_indicator(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Multi slot tx indicator", msg_type, pdu_type, report);
}

/// Validates the PRB start property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_prb_start(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PRB start", msg_type, pdu_type, report);
}

/// Validates the PRB size property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_prb_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 16;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PRB size", msg_type, pdu_type, report);
}

/// Validates the start symbol index property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_start_symbol_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Start symbol index", msg_type, pdu_type, report);
}

/// Validates the duration symbols property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_duration_symbols(unsigned value, pucch_format format_type, validator_report& report)
{
  if (format_type == pucch_format::FORMAT_0 || format_type == pucch_format::FORMAT_2) {
    static constexpr unsigned MIN_VALUE = 1;
    static constexpr unsigned MAX_VALUE = 2;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "Duration in symbols", msg_type, pdu_type, report);
  }

  static constexpr unsigned MIN_VALUE = 4;
  static constexpr unsigned MAX_VALUE = 14;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Duration in symbols", msg_type, pdu_type, report);
}

/// Validates the second hop PRB property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_second_hop_prb(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Second hop PRB", msg_type, pdu_type, report);
}

/// Validates the PUCCH group hopping property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_pucch_group_hopping(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PUCCH group hopping", msg_type, pdu_type, report);
}

/// Validates the nID PUCCH hopping property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_nid_pucch_hopping(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "NID PUCCH hopping", msg_type, pdu_type, report);
}

/// Validates the initial cyclic shift property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_initial_cyclic_shift(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 11;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Initial cyclic shift", msg_type, pdu_type, report);
}

/// Validates the nID PUCCH scrambling property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_nid_pucch_scrambling(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "NID PUCCH scrambling", msg_type, pdu_type, report);
}

/// Validates the time domain occ index property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_time_domain_occ_idx(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 6;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Time domain occ index", msg_type, pdu_type, report);
}

/// Validates the pre DFT occ index property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_pre_dft_occ_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Pre DFT occ index", msg_type, pdu_type, report);
}

/// Validates the Pre DFT occ len property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_pre_dft_occ_len(unsigned value, validator_report& report)
{
  if (value == 2U || value == 4U) {
    return true;
  }

  report.append(value, "Pre DFT occ len", msg_type, pdu_type);
  return false;
}

/// Validates the m0 PUCCH DMRS cyclic shift property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_m0_pucch_dmrs_cyclic_shift(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 9;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "M0 PUCCH DMRS cyclic shift", msg_type, pdu_type, report);
}

/// Validates the SR bit len property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_sr_bit_len(unsigned value, pucch_format format_type, validator_report& report)
{
  if (format_type == pucch_format::FORMAT_0 || format_type == pucch_format::FORMAT_1) {
    static constexpr unsigned MIN_VALUE = 0;
    static constexpr unsigned MAX_VALUE = 1;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "SR bit len", msg_type, pdu_type, report);
  }

  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "SR bit len", msg_type, pdu_type, report);
}

/// Validates the HARQ bit len property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_harq_bit_len(unsigned value, pucch_format format_type, validator_report& report)
{
  if (value == 0) {
    return true;
  }

  if (format_type == pucch_format::FORMAT_0 || format_type == pucch_format::FORMAT_1) {
    static constexpr unsigned MIN_VALUE = 1;
    static constexpr unsigned MAX_VALUE = 2;

    return validate_field(MIN_VALUE, MAX_VALUE, value, "HARQ bit len", msg_type, pdu_type, report);
  }

  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "HARQ bit len", msg_type, pdu_type, report);
}

/// Validates the CSI Part 1 bit length property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_csi_part1_bit_len(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CSI part1 bit length", msg_type, pdu_type, report);
}

/// Validates the max code rate property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_max_code_rate(unsigned value, pucch_format format_type, validator_report& report)
{
  if (format_type == pucch_format::FORMAT_0 || format_type == pucch_format::FORMAT_1) {
    static constexpr unsigned INVALID = 255;

    if (value == INVALID) {
      return true;
    }

    report.append(value, "Max code rate", msg_type, pdu_type);

    return false;
  }

  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 7;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Max code rate", msg_type, pdu_type, report);
}

/// Validates the part2 size map scope property of the PUCCH PDU, as per SCF-222 v4.0 Section 3.4.3.3.
static bool validate_part2_size_map_scope(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Part2 size map scope", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_ul_pucch_pdu(const ul_pucch_pdu& pdu, validator_report& report)
{
  bool result = true;

  result &= validate_rnti(static_cast<unsigned>(pdu.rnti), report);
  // NOTE: Handle property range is not specified.
  result &= validate_bwp_size(pdu.bwp_size, report);
  result &= validate_bwp_start(pdu.bwp_start, report);
  result &= validate_scs(static_cast<unsigned>(pdu.scs), report);
  result &= validate_cyclic_prefix(static_cast<unsigned>(pdu.cp), report);
  result &= validate_format_type(static_cast<unsigned>(pdu.format_type), report);
  result &= validate_multi_slot_tx_indicator(static_cast<unsigned>(pdu.multi_slot_tx_indicator), report);
  // NOTE: Pi to BPSK property uses the whole range of the variable, so it will not be validated.
  result &= validate_prb_start(pdu.prb_start, report);
  result &= validate_prb_size(pdu.prb_size, report);
  result &= validate_start_symbol_index(pdu.start_symbol_index, report);
  result &= validate_duration_symbols(pdu.nr_of_symbols, pdu.format_type, report);
  // NOTE: Intra slot frequency hopping property uses the whole range of the variable, so it will not be validated.
  result &= validate_second_hop_prb(pdu.second_hop_prb, report);
  // NOTE: Obsolete 8bit property is not supported in this release, so it will not be validated.
  if (pdu.format_type == pucch_format::FORMAT_0 || pdu.format_type == pucch_format::FORMAT_1 ||
      pdu.format_type == pucch_format::FORMAT_3 || pdu.format_type == pucch_format::FORMAT_4) {
    result &= validate_pucch_group_hopping(static_cast<unsigned>(pdu.pucch_grp_hopping), report);
    result &= validate_nid_pucch_hopping(pdu.nid_pucch_hopping, report);
    result &= validate_initial_cyclic_shift(pdu.initial_cyclic_shift, report);
  }
  if (pdu.format_type == pucch_format::FORMAT_2 || pdu.format_type == pucch_format::FORMAT_3 ||
      pdu.format_type == pucch_format::FORMAT_4) {
    result &= validate_nid_pucch_scrambling(pdu.nid_pucch_scrambling, report);
    result &= validate_csi_part1_bit_len(pdu.csi_part1_bit_length, report);
    result &= validate_max_code_rate(pdu.pucch_maintenance_v3.max_code_rate, pdu.format_type, report);
  }
  if (pdu.format_type == pucch_format::FORMAT_1) {
    result &= validate_time_domain_occ_idx(pdu.time_domain_occ_index, report);
  }
  if (pdu.format_type == pucch_format::FORMAT_4) {
    result &= validate_pre_dft_occ_index(pdu.pre_dft_occ_idx, report);
    result &= validate_pre_dft_occ_len(pdu.pre_dft_occ_len, report);
  }
  if (pdu.format_type == pucch_format::FORMAT_3 || pdu.format_type == pucch_format::FORMAT_4) {
    // NOTE: Add DMRS flag property uses the whole range of the variable, so it will not be validated.
    // NOTE: NID0 PUCCH DMRS scrambling property uses the whole range of the variable, so it will not be validated.
    result &= validate_m0_pucch_dmrs_cyclic_shift(pdu.m0_pucch_dmrs_cyclic_shift, report);
  }

  result &= validate_sr_bit_len(pdu.sr_bit_len, pdu.format_type, report);
  result &= validate_harq_bit_len(pdu.bit_len_harq, pdu.format_type, report);
  // NOTE: UL BWP ID property uses the whole range of the variable, so it will not be validated.

  // Validate UCI Part1 to Part2 correspondence.
  for (const auto& corr : pdu.uci_correspondence.part2) {
    // NOTE: Priority property range is not specified.
    // NOTE: Param offsets and param sizes properties range is not specified.
    if (corr.param_sizes.empty() || corr.param_offsets.empty() ||
        corr.param_sizes.size() != corr.param_offsets.size()) {
      report.append(0, "Param offset or param size", msg_type, pdu_type);
      result = false;
    }
    // NOTE: Part2 size map index property range is not specified.
    result &= validate_part2_size_map_scope(static_cast<unsigned>(corr.part2_size_map_scope), report);
  }

  return result;
}
