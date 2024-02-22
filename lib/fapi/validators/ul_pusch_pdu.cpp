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

#include "ul_pusch_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/validator_report.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::ul_tti_request;

/// This validator checks the PRACH PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(ul_pdu_type::PUSCH);

/// Validates the RNTI property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_rnti(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 65535;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RNTI", msg_type, pdu_type, report);
}

/// Validates the BWP size property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_bwp_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP size", msg_type, pdu_type, report);
}

/// Validates the BWP start property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_bwp_start(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP start", msg_type, pdu_type, report);
}

/// Validates the subcarrier spacing property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_scs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the cyclic prefix property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_cyclic_prefix(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic prefix", msg_type, pdu_type, report);
}

/// Validates the QAM modulation order property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_qam_mod_order(unsigned value, bool transform_precoding, validator_report& report)
{
  if (value == 2U || value == 4U || value == 6U || value == 8U) {
    return true;
  }

  if (transform_precoding && value == 1U) {
    return true;
  }

  report.append(value, "QAM modulation order", msg_type, pdu_type);

  return false;
}

/// Validates the MCS index property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_mcs_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 31;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "MCS index", msg_type, pdu_type, report);
}

/// Validates the MCS table property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_mcs_table(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "MCS table", msg_type, pdu_type, report);
}

/// Validates the nID PUSCH property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_nid_pusch(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "NID PUSCH", msg_type, pdu_type, report);
}

/// Validates the number of layers property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_num_layers(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of layers", msg_type, pdu_type, report);
}

/// Validates the DMRS config type property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_dmrs_config_type(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "DMRS config type", msg_type, pdu_type, report);
}

/// Validates the Low PAPR DMRS property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_low_papr_dmrs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Low PAPR DMRS", msg_type, pdu_type, report);
}

/// Validates the PUSCH DMRS identity property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_pusch_dmrs_identity(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1007;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PUSCH DMRS identity", msg_type, pdu_type, report);
}

/// Validates the nSCID property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_nscid(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "NSCID", msg_type, pdu_type, report);
}

/// Validates the number of DMRS CDM groups without data property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_num_dmrs_cdm_grps_no_data(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Number of DMRS CDM groups without data", msg_type, pdu_type, report);
}

/// Validates the resource allocation property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_resource_allocation(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Resource allocation type", msg_type, pdu_type, report);
}

/// Validates the RB start property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_rb_start(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RB start", msg_type, pdu_type, report);
}

/// Validates the RB size property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_rb_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RB size", msg_type, pdu_type, report);
}

/// Validates the VRB-to-PRB mapping property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_vrb_to_prb_mapping(unsigned value, validator_report& report)
{
  if (value == 0) {
    return true;
  }

  report.append(value, "VRB-to-PRB mapping", msg_type, pdu_type);

  return false;
}

/// Validates the tx direct current location property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_tx_direct_current_location(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4095;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Tx direct current location", msg_type, pdu_type, report);
}

/// Validates the start symbol index property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_start_symbol_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Start symbol index", msg_type, pdu_type, report);
}

/// Validates the number of symbols property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_num_symbols(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 14;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of symbols", msg_type, pdu_type, report);
}

/// Validates the RV index property of the optional PUSCH data information, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_rv_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RV index", msg_type, pdu_type, report);
}

/// Validates the HARQ process id property of the optional PUSCH data information, as per SCF-222 v4.0 Section 3.4.3.2.
static bool validate_harq_process_id(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 15;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "HARQ process id", msg_type, pdu_type, report);
}

/// Validates the HARQ ACK bit length property of the optional PUSCH UCI information, as per SCF-222 v4.0
/// Section 3.4.3.2.
static bool validate_harq_ack_bit_len(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "HARQ ACK bit length", msg_type, pdu_type, report);
}

/// Validates the CSI Part 1 bit length property of the optional PUSCH UCI information, as per SCF-222 v4.0
/// Section 3.4.3.2.
static bool validate_csi_part1_bit_len(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1706;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CSI part1 bit length", msg_type, pdu_type, report);
}

/// Validates the flag CSI Part 2 property of the optional PUSCH UCI information, as per SCF-222 v4.0
/// Section 3.4.3.2.
static bool validate_flag_csi_part2(unsigned value, validator_report& report)
{
  if (value == 0U || value == 65535) {
    return true;
  }

  report.append(value, "Flag CSI part2", msg_type, pdu_type);
  return false;
}

/// Validates the alpha scaling property of the optional PUSCH UCI information, as per SCF-222 v4.0
/// Section 3.4.3.2.
static bool validate_alpha_scaling(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Alpha scaling", msg_type, pdu_type, report);
}

/// Validates the beta offset HARQ ACK property of the optional PUSCH UCI information, as per SCF-222 v4.0
/// Section 3.4.3.2.
static bool validate_beta_offset_harq_ack(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 15;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Beta offset HARQ ACK", msg_type, pdu_type, report);
}

/// Validates the beta offset CSI Part 1 property of the optional PUSCH UCI information, as per SCF-222 v4.0
/// Section 3.4.3.2.
static bool validate_beta_offset_csi1(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 18;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Beta offset CSI Part1", msg_type, pdu_type, report);
}

/// Validates the beta offset CSI Part 2 property of the optional PUSCH UCI information, as per SCF-222 v4.0
/// Section 3.4.3.2.
static bool validate_beta_offset_csi2(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 18;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Beta offset CSI Part2", msg_type, pdu_type, report);
}

/// Validates the part2 size map scope property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2, in table UCI
/// information for determining UCI Part1 to Part2 correspondence.
static bool validate_part2_size_map_scope(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Part2 size map scope", msg_type, pdu_type, report);
}

/// Validates the PTRS DMRS port property of the optional PUSCH PTRS, as per SCF-222 v4.0 Section 3.4.3.2, in table
/// optional PUSCH PTRS information.
static bool validate_ptrs_dmrs_port(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 11;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PTRS DMRS port", msg_type, pdu_type, report);
}

/// Validates the PTRS RE offset property of the optional PUSCH PTRS, as per SCF-222 v4.0 Section 3.4.3.2, in table
/// optional PUSCH PTRS information.
static bool validate_ptrs_re_offset(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 11;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PTRS RE offset", msg_type, pdu_type, report);
}

/// Validates the PTRS time density property of the optional PUSCH PTRS, as per SCF-222 v4.0 Section 3.4.3.2, in table
/// optional PUSCH PTRS information.
static bool validate_ptrs_time_density(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PTRS time density", msg_type, pdu_type, report);
}

/// Validates the PTRS frequency density property of the optional PUSCH PTRS, as per SCF-222 v4.0 Section 3.4.3.2, in
/// table optional PUSCH PTRS information.
static bool validate_ptrs_freq_density(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PTRS frequency density", msg_type, pdu_type, report);
}

/// Validates the UL PTRS power property of the optional PUSCH PTRS, as per SCF-222 v4.0 Section 3.4.3.2, in table
/// optional PUSCH PTRS information.
static bool validate_ul_ptrs_power(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "UL PTRS power", msg_type, pdu_type, report);
}

/// Validates the low PAPR group number property of the optional PUSCH DFTS OFDM, as per SCF-222 v4.0 Section 3.4.3.2,
/// in table optional DFTS OFDM information.
static bool validate_low_papr_group_number(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 29;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Low PAPR group number", msg_type, pdu_type, report);
}

/// Validates the UL PTRS sample density property of the optional DFTS OFDM, as per SCF-222 v4.0 Section 3.4.3.2, in
/// table optional DFTS OFDM information.
static bool validate_ul_ptrs_sample_density(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 8;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "UL PTRS sample density", msg_type, pdu_type, report);
}

/// Validates the UL PTRS time density transform precoding property of the optional PUSCH DFTS OFDM, as per SCF-222 v4.0
/// Section 3.4.3.2, in table optional DFTS OFDM information.
static bool validate_ul_ptrs_time_density_transform_precoding(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "UL PTRS time density transform precoding", msg_type, pdu_type, report);
}

/// Validates the PUSCH trans type property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2, in table PUSCH
/// maintenance FAPIv3.
static bool validate_pusch_trans_type(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PUSCH trans type", msg_type, pdu_type, report);
}

/// Validates the delta BWP0 start from active BWP property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2, in
/// table PUSCH maintenance FAPIv3.
static bool validate_delta_bwp0_start_from_active_bwp(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Delta BWP0 start from active BWP", msg_type, pdu_type, report);
}

/// Validates the initial UL BWP size property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2, in table PUSCH
/// maintenance FAPIv3.
static bool validate_initial_ul_bwp_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Initial UL BWP size", msg_type, pdu_type, report);
}

/// Validates the group or sequence hopping property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2, in table
/// PUSCH maintenance FAPIv3.
static bool validate_group_or_sequence_hopping(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Group or sequence hopping", msg_type, pdu_type, report);
}

/// Validates the PUSCH second hop PRB property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2, in table PUSCH
/// maintenance FAPIv3.
static bool validate_pusch_second_hop_prb(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PUSCH second hop PRB", msg_type, pdu_type, report);
}

/// Validates the LDPC base graph property of the PUSCH PDU, as per SCF-222 v4.0 Section 3.4.3.2, in table  PUSCH
/// maintenance FAPIv3.
static bool validate_ldpc_base_graph(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "LDPC base graph", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_ul_pusch_pdu(const ul_pusch_pdu& pdu, validator_report& report)
{
  bool result = true;

  // NOTE: pdu Bitmap property range is not specified and will not be validated.
  result &= validate_rnti(static_cast<unsigned>(pdu.rnti), report);
  // NOTE: Handle property range is not specified.
  result &= validate_bwp_size(pdu.bwp_size, report);
  result &= validate_bwp_start(pdu.bwp_start, report);
  result &= validate_scs(static_cast<unsigned>(pdu.scs), report);
  result &= validate_cyclic_prefix(static_cast<unsigned>(pdu.cp), report);
  // NOTE: Target code rate property range is not specified, so it will not be validated.
  result &= validate_qam_mod_order(get_bits_per_symbol(pdu.qam_mod_order), pdu.transform_precoding, report);
  result &= validate_mcs_index(pdu.mcs_index, report);
  result &= validate_mcs_table(static_cast<unsigned>(pdu.mcs_table), report);
  // NOTE: Transform precoding uses the whole range of the variable, so it will not be validated.
  result &= validate_nid_pusch(pdu.nid_pusch, report);
  result &= validate_num_layers(pdu.num_layers, report);
  // NOTE: UL DMRS symbol position bitmap will not be validated.
  result &= validate_dmrs_config_type(static_cast<unsigned>(pdu.dmrs_type), report);
  // NOTE: PUSCH DMRS scrambling ID property uses the whole range, so it will not be validated.
  // NOTE: PUSCH DMRS scrambling ID complement property uses the whole range, so it will not be validated.
  result &= validate_low_papr_dmrs(static_cast<unsigned>(pdu.low_papr_dmrs), report);
  result &= validate_pusch_dmrs_identity(pdu.pusch_dmrs_identity, report);
  result &= validate_nscid(pdu.nscid, report);
  result &= validate_num_dmrs_cdm_grps_no_data(pdu.num_dmrs_cdm_grps_no_data, report);
  // NOTE: DMRS ports bitmap will not be validated.
  result &= validate_resource_allocation(static_cast<unsigned>(pdu.resource_alloc), report);
  // NOTE: RB bitmap will not be validated.
  if (pdu.resource_alloc == resource_allocation_type::type_1) {
    result &= validate_rb_start(pdu.rb_start, report);
    result &= validate_rb_size(pdu.rb_size, report);
  }

  result &= validate_vrb_to_prb_mapping(static_cast<unsigned>(pdu.vrb_to_prb_mapping), report);
  // NOTE: Intra slot frequency hopping property uses the whole range, so it will not be validated.
  result &= validate_tx_direct_current_location(pdu.tx_direct_current_location, report);
  // NOTE: Uplink frequency shift 7p5kHz property uses the whole range, so it will not be validated.
  result &= validate_start_symbol_index(pdu.start_symbol_index, report);
  result &= validate_num_symbols(pdu.nr_of_symbols, report);

  if (pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_DATA_BIT]) {
    const auto& data = pdu.pusch_data;
    result &= validate_rv_index(data.rv_index, report);
    result &= validate_harq_process_id(data.harq_process_id, report);
    // NOTE: New data property uses the whole range, so it will not be validated.
    // NOTE: TB size property range is not specified, so it will not be validated.
    // NOTE: Num CB property range is not specified, so it will not be validated.
    // NOTE: CB present and position bitmap property will not be validated.
  }

  if (pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_UCI_BIT]) {
    const auto& uci = pdu.pusch_uci;
    result &= validate_harq_ack_bit_len(uci.harq_ack_bit_length, report);
    result &= validate_csi_part1_bit_len(uci.csi_part1_bit_length, report);
    result &= validate_flag_csi_part2(uci.flags_csi_part2, report);
    result &= validate_alpha_scaling(static_cast<unsigned>(uci.alpha_scaling), report);
    result &= validate_beta_offset_harq_ack(uci.beta_offset_harq_ack, report);
    result &= validate_beta_offset_csi1(uci.beta_offset_csi1, report);
    result &= validate_beta_offset_csi2(uci.beta_offset_csi2, report);

    // Check the optional PUSCH UCI added in FAPIv3.
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
  }

  if (pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_PTRS_BIT]) {
    const auto& ptrs = pdu.pusch_ptrs;

    for (const auto& ptrs_port : ptrs.port_info) {
      // NOTE: PTRS port index bitmap will not be validated.
      result &= validate_ptrs_dmrs_port(ptrs_port.ptrs_dmrs_port, report);
      result &= validate_ptrs_re_offset(ptrs_port.ptrs_re_offset, report);
    }

    result &= validate_ptrs_time_density(ptrs.ptrs_time_density, report);
    result &= validate_ptrs_freq_density(ptrs.ptrs_freq_density, report);
    result &= validate_ul_ptrs_power(static_cast<unsigned>(ptrs.ul_ptrs_power), report);
  }

  if (pdu.pdu_bitmap[ul_pusch_pdu::DFTS_OFDM_BIT]) {
    const auto& ofdm = pdu.pusch_ofdm;

    result &= validate_low_papr_group_number(ofdm.low_papr_group_number, report);
    // NOTE: Low PAPR sequence number range is not specified, so it will not be validated.
    result &= validate_ul_ptrs_sample_density(ofdm.ul_ptrs_sample_density, report);
    result &= validate_ul_ptrs_time_density_transform_precoding(ofdm.ul_ptrs_time_density_transform_precoding, report);
  }

  // Maintenance v3 parameters.
  const auto& v3 = pdu.pusch_maintenance_v3;
  result &= validate_pusch_trans_type(v3.pusch_trans_type, report);
  result &= validate_delta_bwp0_start_from_active_bwp(v3.delta_bwp0_start_from_active_bwp, report);
  result &= validate_initial_ul_bwp_size(v3.initial_ul_bwp_size, report);
  result &= validate_group_or_sequence_hopping(v3.group_or_sequence_hopping, report);
  result &= validate_pusch_second_hop_prb(v3.pusch_second_hop_prb, report);
  result &= validate_ldpc_base_graph(static_cast<unsigned>(v3.ldpc_base_graph), report);
  // NOTE: TB size LBRM bytes property range is not specified, so it will not be validated.

  // Parameters v4.
  // NOTE: CB CRC status request property uses the whole range of the variable, so it will not be validated

  return result;
}
