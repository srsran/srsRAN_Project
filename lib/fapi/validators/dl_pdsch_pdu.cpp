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

#include "dl_pdsch_pdu.h"
#include "field_checkers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/validator_report.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::dl_tti_request;

/// This validator checks the PDSCH PDU.
static constexpr unsigned pdu_type = static_cast<unsigned>(dl_pdu_type::PDSCH);

/// Validates the RNTI property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_rnti(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 65535;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RNTI", msg_type, pdu_type, report);
}

/// Validates the BWP size property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_bwp_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP Size", msg_type, pdu_type, report);
}

/// Validates the BWP start property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_bwp_start(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "BWP Start", msg_type, pdu_type, report);
}

/// Validates the subcarrier spacing property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_scs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 4;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Subcarrier spacing", msg_type, pdu_type, report);
}

/// Validates the cyclic prefix property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_cp(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Cyclic prefix", msg_type, pdu_type, report);
}

/// Validates the QAM modulation order property of the PDSCH codeword, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_codeword_qam_mod_order(unsigned value, validator_report& report)
{
  if (value == 2U || value == 4U || value == 6U || value == 8U) {
    return true;
  }

  report.append(static_cast<int32_t>(value), "QAM modulation order", msg_type, pdu_type);
  return false;
}

/// Validates the MCS index property of the PDSCH codeword, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_codeword_mcs_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 31;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "MCS Index", msg_type, pdu_type, report);
}

/// Validates the MCS table property of the PDSCH codeword, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_codeword_mcs_table(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "MCS Table", msg_type, pdu_type, report);
}

/// Validates the RV index property of the PDSCH codeword, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_codeword_rv_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RV Index", msg_type, pdu_type, report);
}

/// Validates the n ID PDSCH property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_nid_pdsch(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "nID PDSCH", msg_type, pdu_type, report);
}

/// Validates the number of layers property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_nof_layers(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 8;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of layers", msg_type, pdu_type, report);
}

/// Validates the QAM modulation order property of the PDSCH codeword, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_transmission_scheme(unsigned value, validator_report& report)
{
  if (value == 0U) {
    return true;
  }

  report.append(static_cast<int32_t>(value), "Trasnmission scheme", msg_type, pdu_type);
  return false;
}

/// Validates the reference point property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_ref_point(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Reference point", msg_type, pdu_type, report);
}

/// Validates the DMRS config type property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_dmrs_config_type(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "DMRS config type", msg_type, pdu_type, report);
}

/// Validates the low PAPR DMRS property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_low_papr_dmrs(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Low PAPR DMRS", msg_type, pdu_type, report);
}

/// Validates the DMRS sequence initialization property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_nscid(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "NSCID", msg_type, pdu_type, report);
}

/// Validates the number of DMRS CDM groups without data property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_num_dmrs_cdm_grps_no_data(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 3;

  return validate_field(
      MIN_VALUE, MAX_VALUE, value, "Number of DMRS CDM groups without data", msg_type, pdu_type, report);
}

/// Validates the resource allocation property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_resource_allocation(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Resource allocation", msg_type, pdu_type, report);
}

/// Validates the RB size property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_rb_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RB Size", msg_type, pdu_type, report);
}

/// Validates the RB start property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_rb_start(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "RB Start", msg_type, pdu_type, report);
}

/// Validates the VRB-to-PRB-mapping property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_vrb_to_prb_mapping(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 2;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "VRB-to-PRB-mapping", msg_type, pdu_type, report);
}

/// Validates the start symbol index property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_start_symbol_index(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 13;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Start symbol index", msg_type, pdu_type, report);
}

/// Validates the PDSCH duration in symbols property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_nr_of_symbols(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 14;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PDSCH duration in symbols", msg_type, pdu_type, report);
}

/// Validates the ratio of PDSCH EPRE to NZP CSI-RSEPRE and the PDSCH DMRS power offset profile SSS properties of the
/// PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_power_control_offset_profile_nr(unsigned          power_control_offset_profile_nr,
                                                     int               dmrs_power_profile_sss,
                                                     validator_report& report)
{
  static constexpr unsigned USE_PROFILE_SSS       = 255;
  static constexpr unsigned MIN_VALUE_PROFILE_NR  = 0;
  static constexpr unsigned MAX_VALUE_PROFILE_NR  = 23;
  static constexpr int      USE_PROFILE_NR        = -32768;
  static constexpr int      MIN_VALUE_PROFILE_SSS = -32767;
  static constexpr int      MAX_VALUE_PROFILE_SSS = 32767;

  if (power_control_offset_profile_nr == USE_PROFILE_SSS && MIN_VALUE_PROFILE_SSS <= dmrs_power_profile_sss &&
      dmrs_power_profile_sss <= MAX_VALUE_PROFILE_SSS) {
    return true;
  }

  if (dmrs_power_profile_sss == USE_PROFILE_NR && MIN_VALUE_PROFILE_NR <= power_control_offset_profile_nr &&
      power_control_offset_profile_nr <= MAX_VALUE_PROFILE_NR) {
    return true;
  }

  report.append(dmrs_power_profile_sss, "Ratio PDSCH EPRE to NZP CSI-RSEPRE", msg_type, pdu_type);
  return false;
}

/// Validates the ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE and the PDSCH DMRS power offset profile SSS properties
/// of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_power_control_offset_ss_profile_nr(unsigned          power_control_offset_ss_profile_nr,
                                                        int               dmrs_power_profile_sss,
                                                        validator_report& report)
{
  static constexpr unsigned USE_PROFILE_SSS       = 255;
  static constexpr unsigned MIN_VALUE_PROFILE_NR  = 0;
  static constexpr unsigned MAX_VALUE_PROFILE_NR  = 3;
  static constexpr int      USE_PROFILE_NR        = -32768;
  static constexpr int      MIN_VALUE_PROFILE_SSS = -32767;
  static constexpr int      MAX_VALUE_PROFILE_SSS = 32767;

  if (power_control_offset_ss_profile_nr == USE_PROFILE_SSS && MIN_VALUE_PROFILE_SSS <= dmrs_power_profile_sss &&
      dmrs_power_profile_sss <= MAX_VALUE_PROFILE_SSS) {
    return true;
  }

  if (dmrs_power_profile_sss == USE_PROFILE_NR && MIN_VALUE_PROFILE_NR <= power_control_offset_ss_profile_nr &&
      power_control_offset_ss_profile_nr <= MAX_VALUE_PROFILE_NR) {
    return true;
  }

  report.append(dmrs_power_profile_sss, "Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE", msg_type, pdu_type);
  return false;
}

/// Validates the is inline TB CRC property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_is_inline_tb_crc(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Is inline TB CRC", msg_type, pdu_type, report);
}

/// Validates the PDSCH transmission type property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_maintenance_v3_pdsch_trans_type(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 5;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PDSCH transmission type", msg_type, pdu_type, report);
}

/// Validates the CORESET start point property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_maintenance_v3_coreset_start_point(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "CORESET start point", msg_type, pdu_type, report);
}

/// Validates the initial DL BWP size property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_maintenance_v3_initial_dl_bwp_size(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 274;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Initial DL BWP size", msg_type, pdu_type, report);
}

/// Validates the LDPC base graph property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_maintenance_v3_ldpc_base_graph(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = static_cast<unsigned>(ldpc_base_graph_type::BG1);
  static constexpr unsigned MAX_VALUE = static_cast<unsigned>(ldpc_base_graph_type::BG2);

  return validate_field(MIN_VALUE, MAX_VALUE, value, "LDPC base graph", msg_type, pdu_type, report);
}

/// Validates the maximum number CBG per TB property of the PDSCH codeword, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_maintenance_v3_max_num_cbg_per_tb(unsigned value, validator_report& report)
{
  if (value == 2U || value == 4U || value == 6U || value == 8U) {
    return true;
  }

  report.append(static_cast<int32_t>(value), "Maximum number CBG per TB", msg_type, pdu_type);
  return false;
}

/// Validates the LTE CRS MBSFN derivation method property of the PDSCH PDU, as per SCF-222 v4.0 section 3.4.2.2.
static bool validate_parameters_v4_lte_crs_mbsfn_derivation_method(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "LTE CRS MBSFN derivation method", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_dl_pdsch_pdu(const dl_pdsch_pdu& pdu, validator_report& report)
{
  bool result = true;

  // NOTE: PDU bitmap property will not be validated.
  result &= validate_rnti(static_cast<unsigned>(pdu.rnti), report);
  // NOTE: PDU index property will not be validated as it uses the whole range of the variable.
  result &= validate_bwp_size(pdu.bwp_size, report);
  result &= validate_bwp_start(pdu.bwp_start, report);
  result &= validate_scs(static_cast<unsigned>(pdu.scs), report);
  result &= validate_cp(static_cast<unsigned>(pdu.cp), report);

  // Validate the codewords.
  for (const auto& cw : pdu.cws) {
    // NOTE: Target code rate will not be validated, as the document doesn't specify the valid values.
    result &= validate_codeword_qam_mod_order(cw.qam_mod_order, report);
    result &= validate_codeword_mcs_index(cw.mcs_index, report);
    result &= validate_codeword_mcs_table(cw.mcs_table, report);
    result &= validate_codeword_rv_index(cw.rv_index, report);
    // NOTE: TB size property will not be validated, as the document doesn't specify the valid values.
  }

  result &= validate_nid_pdsch(pdu.nid_pdsch, report);
  result &= validate_nof_layers(pdu.num_layers, report);
  result &= validate_transmission_scheme(pdu.transmission_scheme, report);
  result &= validate_ref_point(static_cast<unsigned>(pdu.ref_point), report);

  // NOTE: DL DMRS symbol position bitmap will not be validated.
  result &= validate_dmrs_config_type(static_cast<unsigned>(pdu.dmrs_type), report);
  // NOTE: PDSCH DMRS-Scrambling-ID property will not be validated as it uses the whole range of the variable.
  // NOTE: PDSCH DMRS-Scrambling-ID complement property will not be validated as it uses the whole range of the
  // variable.
  result &= validate_low_papr_dmrs(static_cast<unsigned>(pdu.low_papr_dmrs), report);
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
  result &= validate_start_symbol_index(pdu.start_symbol_index, report);
  result &= validate_nr_of_symbols(pdu.nr_of_symbols, report);

  result &= validate_power_control_offset_profile_nr(
      pdu.power_control_offset_profile_nr, pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss, report);
  result &= validate_power_control_offset_ss_profile_nr(static_cast<unsigned>(pdu.power_control_offset_ss_profile_nr),
                                                        pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss,
                                                        report);

  if (pdu.pdu_bitmap[dl_pdsch_pdu::PDU_BITMAP_CBG_RETX_CTRL_BIT]) {
    // NOTE: Is last CB present bitmap property will not be validated.
    result &= validate_is_inline_tb_crc(static_cast<unsigned>(pdu.is_inline_tb_crc), report);
    // NOTE: DL TB CRC CW property will not be validated.
    result &= validate_maintenance_v3_max_num_cbg_per_tb(pdu.pdsch_maintenance_v3.max_num_cbg_per_tb, report);
  }

  result &=
      validate_maintenance_v3_pdsch_trans_type(static_cast<unsigned>(pdu.pdsch_maintenance_v3.trans_type), report);

  if (pdu.pdsch_maintenance_v3.trans_type == pdsch_trans_type::non_interleaved_common_ss) {
    result &= validate_maintenance_v3_coreset_start_point(pdu.pdsch_maintenance_v3.coreset_start_point, report);
  } else if (pdu.pdsch_maintenance_v3.trans_type == pdsch_trans_type::interleaved_common_any_coreset0_present ||
             pdu.pdsch_maintenance_v3.trans_type == pdsch_trans_type::interleaved_common_any_coreset0_not_present) {
    result &= validate_maintenance_v3_coreset_start_point(pdu.pdsch_maintenance_v3.coreset_start_point, report);
    result &= validate_maintenance_v3_initial_dl_bwp_size(pdu.pdsch_maintenance_v3.initial_dl_bwp_size, report);
  }

  result &=
      validate_maintenance_v3_ldpc_base_graph(static_cast<unsigned>(pdu.pdsch_maintenance_v3.ldpc_base_graph), report);

  // NOTE: TB size LBRM Bytes parameter property will not be validated.
  // NOTE: TB CRC required bitmap property will not be validated.
  // NOTE: SSB PDUs for RM property will not be validated as it uses the whole range of the variable.
  // NOTE: SSB config for RM property will not be validated as it uses the whole range of the variable..
  // NOTE: PRB symbol RM pattern bitmap by reference property will not be validated.
  // NOTE: PDCCH PDU index property will not be validated.
  // NOTE: DCI index property will not be validated.
  // NOTE: LTE CRS RM pattern bitmap property will not be validated.
  // NOTE: CSI RS for RM property will not be checked.
  // NOTE: PDSCH DMRS power offset profile SSS property will not be checked, as it uses the whole range of the variable
  // and it's checked with the power control offset profile NR property.
  // NOTE: PDSCH data power offset profile SSS property will not be checked, as it uses the whole range of the variable.
  // NOTE: PDSCH PTRS power offset profile SSS property will not be checked, as it uses the whole range of the variable.
  // NOTE: CORESET RM pattern bitmap by reference property will not be checked.

  result &= validate_parameters_v4_lte_crs_mbsfn_derivation_method(
      pdu.pdsch_parameters_v4.lte_crs_mbsfn_derivation_method, report);
  // NOTE: LTE CRS MBSFN pattern bitmap property will not be validated.

  return result;
}
