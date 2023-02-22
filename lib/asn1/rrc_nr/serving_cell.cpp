/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/asn1/rrc_nr/serving_cell.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// NR-NS-PmaxValue ::= SEQUENCE
SRSASN_CODE nr_ns_pmax_value_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_pmax_present, 1));

  if (add_pmax_present) {
    HANDLE_CODE(pack_integer(bref, add_pmax, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, add_spec_emission, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_ns_pmax_value_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_pmax_present, 1));

  if (add_pmax_present) {
    HANDLE_CODE(unpack_integer(add_pmax, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(add_spec_emission, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void nr_ns_pmax_value_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_pmax_present) {
    j.write_int("additionalPmax", add_pmax);
  }
  j.write_int("additionalSpectrumEmission", add_spec_emission);
  j.end_obj();
}

// NR-MultiBandInfo ::= SEQUENCE
SRSASN_CODE nr_multi_band_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_ind_nr_present, 1));
  HANDLE_CODE(bref.pack(nr_ns_pmax_list.size() > 0, 1));

  if (freq_band_ind_nr_present) {
    HANDLE_CODE(pack_integer(bref, freq_band_ind_nr, (uint16_t)1u, (uint16_t)1024u));
  }
  if (nr_ns_pmax_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nr_ns_pmax_list, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_multi_band_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(freq_band_ind_nr_present, 1));
  bool nr_ns_pmax_list_present;
  HANDLE_CODE(bref.unpack(nr_ns_pmax_list_present, 1));

  if (freq_band_ind_nr_present) {
    HANDLE_CODE(unpack_integer(freq_band_ind_nr, bref, (uint16_t)1u, (uint16_t)1024u));
  }
  if (nr_ns_pmax_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nr_ns_pmax_list, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void nr_multi_band_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_ind_nr_present) {
    j.write_int("freqBandIndicatorNR", freq_band_ind_nr);
  }
  if (nr_ns_pmax_list.size() > 0) {
    j.start_array("nr-NS-PmaxList");
    for (const auto& e1 : nr_ns_pmax_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// PDSCH-TimeDomainResourceAllocation ::= SEQUENCE
SRSASN_CODE pdsch_time_domain_res_alloc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(k0_present, 1));

  if (k0_present) {
    HANDLE_CODE(pack_integer(bref, k0, (uint8_t)0u, (uint8_t)32u));
  }
  HANDLE_CODE(map_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, start_symbol_and_len, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_time_domain_res_alloc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(k0_present, 1));

  if (k0_present) {
    HANDLE_CODE(unpack_integer(k0, bref, (uint8_t)0u, (uint8_t)32u));
  }
  HANDLE_CODE(map_type.unpack(bref));
  HANDLE_CODE(unpack_integer(start_symbol_and_len, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void pdsch_time_domain_res_alloc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (k0_present) {
    j.write_int("k0", k0);
  }
  j.write_str("mappingType", map_type.to_string());
  j.write_int("startSymbolAndLength", start_symbol_and_len);
  j.end_obj();
}

const char* pdsch_time_domain_res_alloc_s::map_type_opts::to_string() const
{
  static const char* names[] = {"typeA", "typeB"};
  return convert_enum_idx(names, 2, value, "pdsch_time_domain_res_alloc_s::map_type_e_");
}

// PUSCH-TimeDomainResourceAllocation ::= SEQUENCE
SRSASN_CODE pusch_time_domain_res_alloc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(k2_present, 1));

  if (k2_present) {
    HANDLE_CODE(pack_integer(bref, k2, (uint8_t)0u, (uint8_t)32u));
  }
  HANDLE_CODE(map_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, start_symbol_and_len, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_time_domain_res_alloc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(k2_present, 1));

  if (k2_present) {
    HANDLE_CODE(unpack_integer(k2, bref, (uint8_t)0u, (uint8_t)32u));
  }
  HANDLE_CODE(map_type.unpack(bref));
  HANDLE_CODE(unpack_integer(start_symbol_and_len, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void pusch_time_domain_res_alloc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (k2_present) {
    j.write_int("k2", k2);
  }
  j.write_str("mappingType", map_type.to_string());
  j.write_int("startSymbolAndLength", start_symbol_and_len);
  j.end_obj();
}

const char* pusch_time_domain_res_alloc_s::map_type_opts::to_string() const
{
  static const char* names[] = {"typeA", "typeB"};
  return convert_enum_idx(names, 2, value, "pusch_time_domain_res_alloc_s::map_type_e_");
}

// ControlResourceSet ::= SEQUENCE
SRSASN_CODE coreset_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tci_states_pdcch_to_add_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(tci_states_pdcch_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(tci_present_in_dci_present, 1));
  HANDLE_CODE(bref.pack(pdcch_dmrs_scrambling_id_present, 1));

  HANDLE_CODE(pack_integer(bref, coreset_id, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(freq_domain_res.pack(bref));
  HANDLE_CODE(pack_integer(bref, dur, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(cce_reg_map_type.pack(bref));
  HANDLE_CODE(precoder_granularity.pack(bref));
  if (tci_states_pdcch_to_add_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tci_states_pdcch_to_add_list, 1, 64, integer_packer<uint8_t>(0, 127)));
  }
  if (tci_states_pdcch_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tci_states_pdcch_to_release_list, 1, 64, integer_packer<uint8_t>(0, 127)));
  }
  if (pdcch_dmrs_scrambling_id_present) {
    HANDLE_CODE(pack_integer(bref, pdcch_dmrs_scrambling_id, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE coreset_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool tci_states_pdcch_to_add_list_present;
  HANDLE_CODE(bref.unpack(tci_states_pdcch_to_add_list_present, 1));
  bool tci_states_pdcch_to_release_list_present;
  HANDLE_CODE(bref.unpack(tci_states_pdcch_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(tci_present_in_dci_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_dmrs_scrambling_id_present, 1));

  HANDLE_CODE(unpack_integer(coreset_id, bref, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(freq_domain_res.unpack(bref));
  HANDLE_CODE(unpack_integer(dur, bref, (uint8_t)1u, (uint8_t)3u));
  HANDLE_CODE(cce_reg_map_type.unpack(bref));
  HANDLE_CODE(precoder_granularity.unpack(bref));
  if (tci_states_pdcch_to_add_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tci_states_pdcch_to_add_list, bref, 1, 64, integer_packer<uint8_t>(0, 127)));
  }
  if (tci_states_pdcch_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tci_states_pdcch_to_release_list, bref, 1, 64, integer_packer<uint8_t>(0, 127)));
  }
  if (pdcch_dmrs_scrambling_id_present) {
    HANDLE_CODE(unpack_integer(pdcch_dmrs_scrambling_id, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
void coreset_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("controlResourceSetId", coreset_id);
  j.write_str("frequencyDomainResources", freq_domain_res.to_string());
  j.write_int("duration", dur);
  j.write_fieldname("cce-REG-MappingType");
  cce_reg_map_type.to_json(j);
  j.write_str("precoderGranularity", precoder_granularity.to_string());
  if (tci_states_pdcch_to_add_list.size() > 0) {
    j.start_array("tci-StatesPDCCH-ToAddList");
    for (const auto& e1 : tci_states_pdcch_to_add_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (tci_states_pdcch_to_release_list.size() > 0) {
    j.start_array("tci-StatesPDCCH-ToReleaseList");
    for (const auto& e1 : tci_states_pdcch_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (tci_present_in_dci_present) {
    j.write_str("tci-PresentInDCI", "enabled");
  }
  if (pdcch_dmrs_scrambling_id_present) {
    j.write_int("pdcch-DMRS-ScramblingID", pdcch_dmrs_scrambling_id);
  }
  j.end_obj();
}

void coreset_s::cce_reg_map_type_c_::set(types::options e)
{
  type_ = e;
}
coreset_s::cce_reg_map_type_c_::interleaved_s_& coreset_s::cce_reg_map_type_c_::set_interleaved()
{
  set(types::interleaved);
  return c;
}
void coreset_s::cce_reg_map_type_c_::set_non_interleaved()
{
  set(types::non_interleaved);
}
void coreset_s::cce_reg_map_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interleaved:
      j.write_fieldname("interleaved");
      j.start_obj();
      j.write_str("reg-BundleSize", c.reg_bundle_size.to_string());
      j.write_str("interleaverSize", c.interleaver_size.to_string());
      if (c.shift_idx_present) {
        j.write_int("shiftIndex", c.shift_idx);
      }
      j.end_obj();
      break;
    case types::non_interleaved:
      break;
    default:
      log_invalid_choice_id(type_, "coreset_s::cce_reg_map_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE coreset_s::cce_reg_map_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interleaved:
      HANDLE_CODE(bref.pack(c.shift_idx_present, 1));
      HANDLE_CODE(c.reg_bundle_size.pack(bref));
      HANDLE_CODE(c.interleaver_size.pack(bref));
      if (c.shift_idx_present) {
        HANDLE_CODE(pack_integer(bref, c.shift_idx, (uint16_t)0u, (uint16_t)274u));
      }
      break;
    case types::non_interleaved:
      break;
    default:
      log_invalid_choice_id(type_, "coreset_s::cce_reg_map_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE coreset_s::cce_reg_map_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interleaved:
      HANDLE_CODE(bref.unpack(c.shift_idx_present, 1));
      HANDLE_CODE(c.reg_bundle_size.unpack(bref));
      HANDLE_CODE(c.interleaver_size.unpack(bref));
      if (c.shift_idx_present) {
        HANDLE_CODE(unpack_integer(c.shift_idx, bref, (uint16_t)0u, (uint16_t)274u));
      }
      break;
    case types::non_interleaved:
      break;
    default:
      log_invalid_choice_id(type_, "coreset_s::cce_reg_map_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* coreset_s::cce_reg_map_type_c_::interleaved_s_::reg_bundle_size_opts::to_string() const
{
  static const char* names[] = {"n2", "n3", "n6"};
  return convert_enum_idx(names, 3, value, "coreset_s::cce_reg_map_type_c_::interleaved_s_::reg_bundle_size_e_");
}
uint8_t coreset_s::cce_reg_map_type_c_::interleaved_s_::reg_bundle_size_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 6};
  return map_enum_number(numbers, 3, value, "coreset_s::cce_reg_map_type_c_::interleaved_s_::reg_bundle_size_e_");
}

const char* coreset_s::cce_reg_map_type_c_::interleaved_s_::interleaver_size_opts::to_string() const
{
  static const char* names[] = {"n2", "n3", "n6"};
  return convert_enum_idx(names, 3, value, "coreset_s::cce_reg_map_type_c_::interleaved_s_::interleaver_size_e_");
}
uint8_t coreset_s::cce_reg_map_type_c_::interleaved_s_::interleaver_size_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 6};
  return map_enum_number(numbers, 3, value, "coreset_s::cce_reg_map_type_c_::interleaved_s_::interleaver_size_e_");
}

const char* coreset_s::precoder_granularity_opts::to_string() const
{
  static const char* names[] = {"sameAsREG-bundle", "allContiguousRBs"};
  return convert_enum_idx(names, 2, value, "coreset_s::precoder_granularity_e_");
}

// RACH-ConfigGeneric ::= SEQUENCE
SRSASN_CODE rach_cfg_generic_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, prach_cfg_idx, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(msg1_fdm.pack(bref));
  HANDLE_CODE(pack_integer(bref, msg1_freq_start, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(pack_integer(bref, zero_correlation_zone_cfg, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, preamb_rx_target_pwr, (int16_t)-202, (int16_t)-60));
  HANDLE_CODE(preamb_trans_max.pack(bref));
  HANDLE_CODE(pwr_ramp_step.pack(bref));
  HANDLE_CODE(ra_resp_win.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_generic_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(prach_cfg_idx, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(msg1_fdm.unpack(bref));
  HANDLE_CODE(unpack_integer(msg1_freq_start, bref, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(unpack_integer(zero_correlation_zone_cfg, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(preamb_rx_target_pwr, bref, (int16_t)-202, (int16_t)-60));
  HANDLE_CODE(preamb_trans_max.unpack(bref));
  HANDLE_CODE(pwr_ramp_step.unpack(bref));
  HANDLE_CODE(ra_resp_win.unpack(bref));

  return SRSASN_SUCCESS;
}
void rach_cfg_generic_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("prach-ConfigurationIndex", prach_cfg_idx);
  j.write_str("msg1-FDM", msg1_fdm.to_string());
  j.write_int("msg1-FrequencyStart", msg1_freq_start);
  j.write_int("zeroCorrelationZoneConfig", zero_correlation_zone_cfg);
  j.write_int("preambleReceivedTargetPower", preamb_rx_target_pwr);
  j.write_str("preambleTransMax", preamb_trans_max.to_string());
  j.write_str("powerRampingStep", pwr_ramp_step.to_string());
  j.write_str("ra-ResponseWindow", ra_resp_win.to_string());
  j.end_obj();
}

const char* rach_cfg_generic_s::msg1_fdm_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "eight"};
  return convert_enum_idx(names, 4, value, "rach_cfg_generic_s::msg1_fdm_e_");
}
uint8_t rach_cfg_generic_s::msg1_fdm_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "rach_cfg_generic_s::msg1_fdm_e_");
}

const char* rach_cfg_generic_s::preamb_trans_max_opts::to_string() const
{
  static const char* names[] = {"n3", "n4", "n5", "n6", "n7", "n8", "n10", "n20", "n50", "n100", "n200"};
  return convert_enum_idx(names, 11, value, "rach_cfg_generic_s::preamb_trans_max_e_");
}
uint8_t rach_cfg_generic_s::preamb_trans_max_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4, 5, 6, 7, 8, 10, 20, 50, 100, 200};
  return map_enum_number(numbers, 11, value, "rach_cfg_generic_s::preamb_trans_max_e_");
}

const char* rach_cfg_generic_s::pwr_ramp_step_opts::to_string() const
{
  static const char* names[] = {"dB0", "dB2", "dB4", "dB6"};
  return convert_enum_idx(names, 4, value, "rach_cfg_generic_s::pwr_ramp_step_e_");
}
uint8_t rach_cfg_generic_s::pwr_ramp_step_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6};
  return map_enum_number(numbers, 4, value, "rach_cfg_generic_s::pwr_ramp_step_e_");
}

const char* rach_cfg_generic_s::ra_resp_win_opts::to_string() const
{
  static const char* names[] = {"sl1", "sl2", "sl4", "sl8", "sl10", "sl20", "sl40", "sl80"};
  return convert_enum_idx(names, 8, value, "rach_cfg_generic_s::ra_resp_win_e_");
}
uint8_t rach_cfg_generic_s::ra_resp_win_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 10, 20, 40, 80};
  return map_enum_number(numbers, 8, value, "rach_cfg_generic_s::ra_resp_win_e_");
}

// SearchSpace ::= SEQUENCE
SRSASN_CODE search_space_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(coreset_id_present, 1));
  HANDLE_CODE(bref.pack(monitoring_slot_periodicity_and_offset_present, 1));
  HANDLE_CODE(bref.pack(dur_present, 1));
  HANDLE_CODE(bref.pack(monitoring_symbols_within_slot_present, 1));
  HANDLE_CODE(bref.pack(nrof_candidates_present, 1));
  HANDLE_CODE(bref.pack(search_space_type_present, 1));

  HANDLE_CODE(pack_integer(bref, search_space_id, (uint8_t)0u, (uint8_t)39u));
  if (coreset_id_present) {
    HANDLE_CODE(pack_integer(bref, coreset_id, (uint8_t)0u, (uint8_t)11u));
  }
  if (monitoring_slot_periodicity_and_offset_present) {
    HANDLE_CODE(monitoring_slot_periodicity_and_offset.pack(bref));
  }
  if (dur_present) {
    HANDLE_CODE(pack_integer(bref, dur, (uint16_t)2u, (uint16_t)2559u));
  }
  if (monitoring_symbols_within_slot_present) {
    HANDLE_CODE(monitoring_symbols_within_slot.pack(bref));
  }
  if (nrof_candidates_present) {
    HANDLE_CODE(nrof_candidates.aggregation_level1.pack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level2.pack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level4.pack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level8.pack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level16.pack(bref));
  }
  if (search_space_type_present) {
    HANDLE_CODE(search_space_type.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE search_space_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(coreset_id_present, 1));
  HANDLE_CODE(bref.unpack(monitoring_slot_periodicity_and_offset_present, 1));
  HANDLE_CODE(bref.unpack(dur_present, 1));
  HANDLE_CODE(bref.unpack(monitoring_symbols_within_slot_present, 1));
  HANDLE_CODE(bref.unpack(nrof_candidates_present, 1));
  HANDLE_CODE(bref.unpack(search_space_type_present, 1));

  HANDLE_CODE(unpack_integer(search_space_id, bref, (uint8_t)0u, (uint8_t)39u));
  if (coreset_id_present) {
    HANDLE_CODE(unpack_integer(coreset_id, bref, (uint8_t)0u, (uint8_t)11u));
  }
  if (monitoring_slot_periodicity_and_offset_present) {
    HANDLE_CODE(monitoring_slot_periodicity_and_offset.unpack(bref));
  }
  if (dur_present) {
    HANDLE_CODE(unpack_integer(dur, bref, (uint16_t)2u, (uint16_t)2559u));
  }
  if (monitoring_symbols_within_slot_present) {
    HANDLE_CODE(monitoring_symbols_within_slot.unpack(bref));
  }
  if (nrof_candidates_present) {
    HANDLE_CODE(nrof_candidates.aggregation_level1.unpack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level2.unpack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level4.unpack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level8.unpack(bref));
    HANDLE_CODE(nrof_candidates.aggregation_level16.unpack(bref));
  }
  if (search_space_type_present) {
    HANDLE_CODE(search_space_type.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void search_space_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("searchSpaceId", search_space_id);
  if (coreset_id_present) {
    j.write_int("controlResourceSetId", coreset_id);
  }
  if (monitoring_slot_periodicity_and_offset_present) {
    j.write_fieldname("monitoringSlotPeriodicityAndOffset");
    monitoring_slot_periodicity_and_offset.to_json(j);
  }
  if (dur_present) {
    j.write_int("duration", dur);
  }
  if (monitoring_symbols_within_slot_present) {
    j.write_str("monitoringSymbolsWithinSlot", monitoring_symbols_within_slot.to_string());
  }
  if (nrof_candidates_present) {
    j.write_fieldname("nrofCandidates");
    j.start_obj();
    j.write_str("aggregationLevel1", nrof_candidates.aggregation_level1.to_string());
    j.write_str("aggregationLevel2", nrof_candidates.aggregation_level2.to_string());
    j.write_str("aggregationLevel4", nrof_candidates.aggregation_level4.to_string());
    j.write_str("aggregationLevel8", nrof_candidates.aggregation_level8.to_string());
    j.write_str("aggregationLevel16", nrof_candidates.aggregation_level16.to_string());
    j.end_obj();
  }
  if (search_space_type_present) {
    j.write_fieldname("searchSpaceType");
    search_space_type.to_json(j);
  }
  j.end_obj();
}

void search_space_s::monitoring_slot_periodicity_and_offset_c_::destroy_() {}
void search_space_s::monitoring_slot_periodicity_and_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
search_space_s::monitoring_slot_periodicity_and_offset_c_::monitoring_slot_periodicity_and_offset_c_(
    const search_space_s::monitoring_slot_periodicity_and_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl1:
      break;
    case types::sl2:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl4:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl5:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl8:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl1280:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl2560:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::monitoring_slot_periodicity_and_offset_c_");
  }
}
search_space_s::monitoring_slot_periodicity_and_offset_c_&
search_space_s::monitoring_slot_periodicity_and_offset_c_::operator=(
    const search_space_s::monitoring_slot_periodicity_and_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl1:
      break;
    case types::sl2:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl4:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl5:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl8:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl1280:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl2560:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::monitoring_slot_periodicity_and_offset_c_");
  }

  return *this;
}
void search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl1()
{
  set(types::sl1);
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl2()
{
  set(types::sl2);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl4()
{
  set(types::sl4);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl5()
{
  set(types::sl5);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl8()
{
  set(types::sl8);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl10()
{
  set(types::sl10);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl16()
{
  set(types::sl16);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl20()
{
  set(types::sl20);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl40()
{
  set(types::sl40);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl80()
{
  set(types::sl80);
  return c.get<uint8_t>();
}
uint8_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl160()
{
  set(types::sl160);
  return c.get<uint8_t>();
}
uint16_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl320()
{
  set(types::sl320);
  return c.get<uint16_t>();
}
uint16_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl640()
{
  set(types::sl640);
  return c.get<uint16_t>();
}
uint16_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl1280()
{
  set(types::sl1280);
  return c.get<uint16_t>();
}
uint16_t& search_space_s::monitoring_slot_periodicity_and_offset_c_::set_sl2560()
{
  set(types::sl2560);
  return c.get<uint16_t>();
}
void search_space_s::monitoring_slot_periodicity_and_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl1:
      break;
    case types::sl2:
      j.write_int("sl2", c.get<uint8_t>());
      break;
    case types::sl4:
      j.write_int("sl4", c.get<uint8_t>());
      break;
    case types::sl5:
      j.write_int("sl5", c.get<uint8_t>());
      break;
    case types::sl8:
      j.write_int("sl8", c.get<uint8_t>());
      break;
    case types::sl10:
      j.write_int("sl10", c.get<uint8_t>());
      break;
    case types::sl16:
      j.write_int("sl16", c.get<uint8_t>());
      break;
    case types::sl20:
      j.write_int("sl20", c.get<uint8_t>());
      break;
    case types::sl40:
      j.write_int("sl40", c.get<uint8_t>());
      break;
    case types::sl80:
      j.write_int("sl80", c.get<uint8_t>());
      break;
    case types::sl160:
      j.write_int("sl160", c.get<uint8_t>());
      break;
    case types::sl320:
      j.write_int("sl320", c.get<uint16_t>());
      break;
    case types::sl640:
      j.write_int("sl640", c.get<uint16_t>());
      break;
    case types::sl1280:
      j.write_int("sl1280", c.get<uint16_t>());
      break;
    case types::sl2560:
      j.write_int("sl2560", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::monitoring_slot_periodicity_and_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE search_space_s::monitoring_slot_periodicity_and_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl1:
      break;
    case types::sl2:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::sl4:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::sl5:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sl8:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::sl10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sl16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sl20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sl40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sl160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sl320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sl640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sl1280:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::sl2560:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::monitoring_slot_periodicity_and_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE search_space_s::monitoring_slot_periodicity_and_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl1:
      break;
    case types::sl2:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::sl4:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::sl5:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sl8:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::sl10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sl16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sl20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sl40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sl160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sl320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sl640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sl1280:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::sl2560:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::monitoring_slot_periodicity_and_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* search_space_s::nrof_candidates_s_::aggregation_level1_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6", "n8"};
  return convert_enum_idx(names, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level1_e_");
}
uint8_t search_space_s::nrof_candidates_s_::aggregation_level1_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8};
  return map_enum_number(numbers, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level1_e_");
}

const char* search_space_s::nrof_candidates_s_::aggregation_level2_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6", "n8"};
  return convert_enum_idx(names, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level2_e_");
}
uint8_t search_space_s::nrof_candidates_s_::aggregation_level2_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8};
  return map_enum_number(numbers, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level2_e_");
}

const char* search_space_s::nrof_candidates_s_::aggregation_level4_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6", "n8"};
  return convert_enum_idx(names, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level4_e_");
}
uint8_t search_space_s::nrof_candidates_s_::aggregation_level4_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8};
  return map_enum_number(numbers, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level4_e_");
}

const char* search_space_s::nrof_candidates_s_::aggregation_level8_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6", "n8"};
  return convert_enum_idx(names, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level8_e_");
}
uint8_t search_space_s::nrof_candidates_s_::aggregation_level8_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8};
  return map_enum_number(numbers, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level8_e_");
}

const char* search_space_s::nrof_candidates_s_::aggregation_level16_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6", "n8"};
  return convert_enum_idx(names, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level16_e_");
}
uint8_t search_space_s::nrof_candidates_s_::aggregation_level16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8};
  return map_enum_number(numbers, 8, value, "search_space_s::nrof_candidates_s_::aggregation_level16_e_");
}

void search_space_s::search_space_type_c_::destroy_()
{
  switch (type_) {
    case types::common:
      c.destroy<common_s_>();
      break;
    case types::ue_specific:
      c.destroy<ue_specific_s_>();
      break;
    default:
      break;
  }
}
void search_space_s::search_space_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::common:
      c.init<common_s_>();
      break;
    case types::ue_specific:
      c.init<ue_specific_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::search_space_type_c_");
  }
}
search_space_s::search_space_type_c_::search_space_type_c_(const search_space_s::search_space_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::common:
      c.init(other.c.get<common_s_>());
      break;
    case types::ue_specific:
      c.init(other.c.get<ue_specific_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::search_space_type_c_");
  }
}
search_space_s::search_space_type_c_&
search_space_s::search_space_type_c_::operator=(const search_space_s::search_space_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::common:
      c.set(other.c.get<common_s_>());
      break;
    case types::ue_specific:
      c.set(other.c.get<ue_specific_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::search_space_type_c_");
  }

  return *this;
}
search_space_s::search_space_type_c_::common_s_& search_space_s::search_space_type_c_::set_common()
{
  set(types::common);
  return c.get<common_s_>();
}
search_space_s::search_space_type_c_::ue_specific_s_& search_space_s::search_space_type_c_::set_ue_specific()
{
  set(types::ue_specific);
  return c.get<ue_specific_s_>();
}
void search_space_s::search_space_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::common:
      j.write_fieldname("common");
      j.start_obj();
      if (c.get<common_s_>().dci_format0_0_and_format1_0_present) {
        j.write_fieldname("dci-Format0-0-AndFormat1-0");
        j.start_obj();
        j.end_obj();
      }
      if (c.get<common_s_>().dci_format2_0_present) {
        j.write_fieldname("dci-Format2-0");
        j.start_obj();
        j.write_fieldname("nrofCandidates-SFI");
        j.start_obj();
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1_present) {
          j.write_str("aggregationLevel1",
                      c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1.to_string());
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2_present) {
          j.write_str("aggregationLevel2",
                      c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2.to_string());
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4_present) {
          j.write_str("aggregationLevel4",
                      c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4.to_string());
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8_present) {
          j.write_str("aggregationLevel8",
                      c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8.to_string());
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16_present) {
          j.write_str("aggregationLevel16",
                      c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16.to_string());
        }
        j.end_obj();
        j.end_obj();
      }
      if (c.get<common_s_>().dci_format2_1_present) {
        j.write_fieldname("dci-Format2-1");
        j.start_obj();
        j.end_obj();
      }
      if (c.get<common_s_>().dci_format2_2_present) {
        j.write_fieldname("dci-Format2-2");
        j.start_obj();
        j.end_obj();
      }
      if (c.get<common_s_>().dci_format2_3_present) {
        j.write_fieldname("dci-Format2-3");
        j.start_obj();
        if (c.get<common_s_>().dci_format2_3.dummy1_present) {
          j.write_str("dummy1", c.get<common_s_>().dci_format2_3.dummy1.to_string());
        }
        j.write_str("dummy2", c.get<common_s_>().dci_format2_3.dummy2.to_string());
        j.end_obj();
      }
      j.end_obj();
      break;
    case types::ue_specific:
      j.write_fieldname("ue-Specific");
      j.start_obj();
      j.write_str("dci-Formats", c.get<ue_specific_s_>().dci_formats.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::search_space_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE search_space_s::search_space_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::common:
      HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format0_0_and_format1_0_present, 1));
      HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_0_present, 1));
      HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_1_present, 1));
      HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_2_present, 1));
      HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_3_present, 1));
      if (c.get<common_s_>().dci_format0_0_and_format1_0_present) {
        bref.pack(c.get<common_s_>().dci_format0_0_and_format1_0.ext, 1);
      }
      if (c.get<common_s_>().dci_format2_0_present) {
        bref.pack(c.get<common_s_>().dci_format2_0.ext, 1);
        HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1_present, 1));
        HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2_present, 1));
        HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4_present, 1));
        HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8_present, 1));
        HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16_present, 1));
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1.pack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2.pack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4.pack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8.pack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16.pack(bref));
        }
      }
      if (c.get<common_s_>().dci_format2_1_present) {
        bref.pack(c.get<common_s_>().dci_format2_1.ext, 1);
      }
      if (c.get<common_s_>().dci_format2_2_present) {
        bref.pack(c.get<common_s_>().dci_format2_2.ext, 1);
      }
      if (c.get<common_s_>().dci_format2_3_present) {
        bref.pack(c.get<common_s_>().dci_format2_3.ext, 1);
        HANDLE_CODE(bref.pack(c.get<common_s_>().dci_format2_3.dummy1_present, 1));
        if (c.get<common_s_>().dci_format2_3.dummy1_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_3.dummy1.pack(bref));
        }
        HANDLE_CODE(c.get<common_s_>().dci_format2_3.dummy2.pack(bref));
      }
      break;
    case types::ue_specific:
      bref.pack(c.get<ue_specific_s_>().ext, 1);
      HANDLE_CODE(c.get<ue_specific_s_>().dci_formats.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::search_space_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE search_space_s::search_space_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::common:
      HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format0_0_and_format1_0_present, 1));
      HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_0_present, 1));
      HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_1_present, 1));
      HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_2_present, 1));
      HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_3_present, 1));
      if (c.get<common_s_>().dci_format0_0_and_format1_0_present) {
        bref.unpack(c.get<common_s_>().dci_format0_0_and_format1_0.ext, 1);
      }
      if (c.get<common_s_>().dci_format2_0_present) {
        bref.unpack(c.get<common_s_>().dci_format2_0.ext, 1);
        HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1_present, 1));
        HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2_present, 1));
        HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4_present, 1));
        HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8_present, 1));
        HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16_present, 1));
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level1.unpack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level2.unpack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level4.unpack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level8.unpack(bref));
        }
        if (c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_0.nrof_candidates_sfi.aggregation_level16.unpack(bref));
        }
      }
      if (c.get<common_s_>().dci_format2_1_present) {
        bref.unpack(c.get<common_s_>().dci_format2_1.ext, 1);
      }
      if (c.get<common_s_>().dci_format2_2_present) {
        bref.unpack(c.get<common_s_>().dci_format2_2.ext, 1);
      }
      if (c.get<common_s_>().dci_format2_3_present) {
        bref.unpack(c.get<common_s_>().dci_format2_3.ext, 1);
        HANDLE_CODE(bref.unpack(c.get<common_s_>().dci_format2_3.dummy1_present, 1));
        if (c.get<common_s_>().dci_format2_3.dummy1_present) {
          HANDLE_CODE(c.get<common_s_>().dci_format2_3.dummy1.unpack(bref));
        }
        HANDLE_CODE(c.get<common_s_>().dci_format2_3.dummy2.unpack(bref));
      }
      break;
    case types::ue_specific:
      bref.unpack(c.get<ue_specific_s_>().ext, 1);
      HANDLE_CODE(c.get<ue_specific_s_>().dci_formats.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "search_space_s::search_space_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level1_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                          "aggregation_level1_e_");
}
uint8_t search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level1_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                         "aggregation_level1_e_");
}

const char* search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level2_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                          "aggregation_level2_e_");
}
uint8_t search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level2_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                         "aggregation_level2_e_");
}

const char* search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level4_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                          "aggregation_level4_e_");
}
uint8_t search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level4_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                         "aggregation_level4_e_");
}

const char* search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level8_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                          "aggregation_level8_e_");
}
uint8_t search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level8_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                         "aggregation_level8_e_");
}

const char* search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                          "aggregation_level16_e_");
}
uint8_t search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::
    aggregation_level16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "search_space_s::search_space_type_c_::common_s_::dci_format2_0_s_::nrof_candidates_sfi_s_::"
                         "aggregation_level16_e_");
}

const char* search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy1_opts::to_string() const
{
  static const char* names[] = {"sl1", "sl2", "sl4", "sl5", "sl8", "sl10", "sl16", "sl20"};
  return convert_enum_idx(
      names, 8, value, "search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy1_e_");
}
uint8_t search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy1_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 5, 8, 10, 16, 20};
  return map_enum_number(
      numbers, 8, value, "search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy1_e_");
}

const char* search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy2_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(
      names, 2, value, "search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy2_e_");
}
uint8_t search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy2_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "search_space_s::search_space_type_c_::common_s_::dci_format2_3_s_::dummy2_e_");
}

const char* search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::to_string() const
{
  static const char* names[] = {"formats0-0-And-1-0", "formats0-1-And-1-1"};
  return convert_enum_idx(names, 2, value, "search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_e_");
}

// BWP ::= SEQUENCE
SRSASN_CODE bwp_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cp_present, 1));

  HANDLE_CODE(pack_integer(bref, location_and_bw, (uint16_t)0u, (uint16_t)37949u));
  HANDLE_CODE(subcarrier_spacing.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cp_present, 1));

  HANDLE_CODE(unpack_integer(location_and_bw, bref, (uint16_t)0u, (uint16_t)37949u));
  HANDLE_CODE(subcarrier_spacing.unpack(bref));

  return SRSASN_SUCCESS;
}
void bwp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("locationAndBandwidth", location_and_bw);
  j.write_str("subcarrierSpacing", subcarrier_spacing.to_string());
  if (cp_present) {
    j.write_str("cyclicPrefix", "extended");
  }
  j.end_obj();
}

// PDCCH-ConfigCommon ::= SEQUENCE
SRSASN_CODE pdcch_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(coreset_zero_present, 1));
  HANDLE_CODE(bref.pack(common_coreset_present, 1));
  HANDLE_CODE(bref.pack(search_space_zero_present, 1));
  HANDLE_CODE(bref.pack(common_search_space_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(search_space_sib1_present, 1));
  HANDLE_CODE(bref.pack(search_space_other_sys_info_present, 1));
  HANDLE_CODE(bref.pack(paging_search_space_present, 1));
  HANDLE_CODE(bref.pack(ra_search_space_present, 1));

  if (coreset_zero_present) {
    HANDLE_CODE(pack_integer(bref, coreset_zero, (uint8_t)0u, (uint8_t)15u));
  }
  if (common_coreset_present) {
    HANDLE_CODE(common_coreset.pack(bref));
  }
  if (search_space_zero_present) {
    HANDLE_CODE(pack_integer(bref, search_space_zero, (uint8_t)0u, (uint8_t)15u));
  }
  if (common_search_space_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, common_search_space_list, 1, 4));
  }
  if (search_space_sib1_present) {
    HANDLE_CODE(pack_integer(bref, search_space_sib1, (uint8_t)0u, (uint8_t)39u));
  }
  if (search_space_other_sys_info_present) {
    HANDLE_CODE(pack_integer(bref, search_space_other_sys_info, (uint8_t)0u, (uint8_t)39u));
  }
  if (paging_search_space_present) {
    HANDLE_CODE(pack_integer(bref, paging_search_space, (uint8_t)0u, (uint8_t)39u));
  }
  if (ra_search_space_present) {
    HANDLE_CODE(pack_integer(bref, ra_search_space, (uint8_t)0u, (uint8_t)39u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= first_pdcch_monitoring_occasion_of_po.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(first_pdcch_monitoring_occasion_of_po.is_present(), 1));
      if (first_pdcch_monitoring_occasion_of_po.is_present()) {
        HANDLE_CODE(first_pdcch_monitoring_occasion_of_po->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(coreset_zero_present, 1));
  HANDLE_CODE(bref.unpack(common_coreset_present, 1));
  HANDLE_CODE(bref.unpack(search_space_zero_present, 1));
  bool common_search_space_list_present;
  HANDLE_CODE(bref.unpack(common_search_space_list_present, 1));
  HANDLE_CODE(bref.unpack(search_space_sib1_present, 1));
  HANDLE_CODE(bref.unpack(search_space_other_sys_info_present, 1));
  HANDLE_CODE(bref.unpack(paging_search_space_present, 1));
  HANDLE_CODE(bref.unpack(ra_search_space_present, 1));

  if (coreset_zero_present) {
    HANDLE_CODE(unpack_integer(coreset_zero, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (common_coreset_present) {
    HANDLE_CODE(common_coreset.unpack(bref));
  }
  if (search_space_zero_present) {
    HANDLE_CODE(unpack_integer(search_space_zero, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (common_search_space_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(common_search_space_list, bref, 1, 4));
  }
  if (search_space_sib1_present) {
    HANDLE_CODE(unpack_integer(search_space_sib1, bref, (uint8_t)0u, (uint8_t)39u));
  }
  if (search_space_other_sys_info_present) {
    HANDLE_CODE(unpack_integer(search_space_other_sys_info, bref, (uint8_t)0u, (uint8_t)39u));
  }
  if (paging_search_space_present) {
    HANDLE_CODE(unpack_integer(paging_search_space, bref, (uint8_t)0u, (uint8_t)39u));
  }
  if (ra_search_space_present) {
    HANDLE_CODE(unpack_integer(ra_search_space, bref, (uint8_t)0u, (uint8_t)39u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool first_pdcch_monitoring_occasion_of_po_present;
      HANDLE_CODE(bref.unpack(first_pdcch_monitoring_occasion_of_po_present, 1));
      first_pdcch_monitoring_occasion_of_po.set_present(first_pdcch_monitoring_occasion_of_po_present);
      if (first_pdcch_monitoring_occasion_of_po.is_present()) {
        HANDLE_CODE(first_pdcch_monitoring_occasion_of_po->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pdcch_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (coreset_zero_present) {
    j.write_int("controlResourceSetZero", coreset_zero);
  }
  if (common_coreset_present) {
    j.write_fieldname("commonControlResourceSet");
    common_coreset.to_json(j);
  }
  if (search_space_zero_present) {
    j.write_int("searchSpaceZero", search_space_zero);
  }
  if (common_search_space_list.size() > 0) {
    j.start_array("commonSearchSpaceList");
    for (const auto& e1 : common_search_space_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (search_space_sib1_present) {
    j.write_int("searchSpaceSIB1", search_space_sib1);
  }
  if (search_space_other_sys_info_present) {
    j.write_int("searchSpaceOtherSystemInformation", search_space_other_sys_info);
  }
  if (paging_search_space_present) {
    j.write_int("pagingSearchSpace", paging_search_space);
  }
  if (ra_search_space_present) {
    j.write_int("ra-SearchSpace", ra_search_space);
  }
  if (ext) {
    if (first_pdcch_monitoring_occasion_of_po.is_present()) {
      j.write_fieldname("firstPDCCH-MonitoringOccasionOfPO");
      first_pdcch_monitoring_occasion_of_po->to_json(j);
    }
  }
  j.end_obj();
}

void pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::destroy_()
{
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.destroy<scs15_kh_zone_t_l_>();
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.destroy<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.destroy<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.destroy<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zone_sixteenth_t_l_>();
      break;
    default:
      break;
  }
}
void pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.init<scs15_kh_zone_t_l_>();
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.init<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.init<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.init<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.init<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.init<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.init<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.init<scs120_kh_zone_sixteenth_t_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_");
  }
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::first_pdcch_monitoring_occasion_of_po_c_(
    const pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.init(other.c.get<scs15_kh_zone_t_l_>());
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.init(other.c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>());
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.init(other.c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>());
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.init(other.c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>());
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zone_sixteenth_t_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_");
  }
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_&
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::operator=(
    const pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.set(other.c.get<scs15_kh_zone_t_l_>());
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.set(other.c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>());
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.set(other.c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>());
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.set(other.c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>());
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zone_sixteenth_t_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_");
  }

  return *this;
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::scs15_kh_zone_t_l_&
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs15_kh_zone_t()
{
  set(types::scs15_kh_zone_t);
  return c.get<scs15_kh_zone_t_l_>();
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::scs30_kh_zone_t_scs15_kh_zhalf_t_l_&
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs30_kh_zone_t_scs15_kh_zhalf_t()
{
  set(types::scs30_kh_zone_t_scs15_kh_zhalf_t);
  return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
{
  set(types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t);
  return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
{
  set(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t);
  return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
}
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::scs120_kh_zone_sixteenth_t_l_&
pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs120_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zone_sixteenth_t_l_>();
}
void pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scs15_kh_zone_t:
      j.start_array("sCS15KHZoneT");
      for (const auto& e1 : c.get<scs15_kh_zone_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      j.start_array("sCS30KHZoneT-SCS15KHZhalfT");
      for (const auto& e1 : c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      j.start_array("sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT");
      for (const auto& e1 : c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      j.start_array("sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT");
      for (const auto& e1 : c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT");
      for (const auto& e1 :
           c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZoneEighthT-SCS60KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs120_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scs15_kh_zone_t:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scs15_kh_zone_t_l_>(), 1, 4, integer_packer<uint8_t>(0, 139)));
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>(), 1, 4, integer_packer<uint16_t>(0, 279)));
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>(),
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 559)));
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      HANDLE_CODE(
          pack_dyn_seq_of(bref,
                          c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>(),
                          1,
                          4,
                          integer_packer<uint16_t>(0, 1119)));
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(
          bref,
          c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>(),
          1,
          4,
          integer_packer<uint16_t>(0, 2239)));
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>(),
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 4479)));
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>(),
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 8959)));
      break;
    case types::scs120_kh_zone_sixteenth_t:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<scs120_kh_zone_sixteenth_t_l_>(), 1, 4, integer_packer<uint16_t>(0, 17919)));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scs15_kh_zone_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs15_kh_zone_t_l_>(), bref, 1, 4, integer_packer<uint8_t>(0, 139)));
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>(), bref, 1, 4, integer_packer<uint16_t>(0, 279)));
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>(),
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 559)));
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>(),
                            bref,
                            1,
                            4,
                            integer_packer<uint16_t>(0, 1119)));
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>(),
          bref,
          1,
          4,
          integer_packer<uint16_t>(0, 2239)));
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>(),
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 4479)));
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>(),
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 8959)));
      break;
    case types::scs120_kh_zone_sixteenth_t:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<scs120_kh_zone_sixteenth_t_l_>(), bref, 1, 4, integer_packer<uint16_t>(0, 17919)));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PDSCH-ConfigCommon ::= SEQUENCE
SRSASN_CODE pdsch_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdsch_time_domain_alloc_list.size() > 0, 1));

  if (pdsch_time_domain_alloc_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pdsch_time_domain_alloc_list, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool pdsch_time_domain_alloc_list_present;
  HANDLE_CODE(bref.unpack(pdsch_time_domain_alloc_list_present, 1));

  if (pdsch_time_domain_alloc_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pdsch_time_domain_alloc_list, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdsch_time_domain_alloc_list.size() > 0) {
    j.start_array("pdsch-TimeDomainAllocationList");
    for (const auto& e1 : pdsch_time_domain_alloc_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// PUCCH-ConfigCommon ::= SEQUENCE
SRSASN_CODE pucch_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pucch_res_common_present, 1));
  HANDLE_CODE(bref.pack(hop_id_present, 1));
  HANDLE_CODE(bref.pack(p0_nominal_present, 1));

  if (pucch_res_common_present) {
    HANDLE_CODE(pack_integer(bref, pucch_res_common, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(pucch_group_hop.pack(bref));
  if (hop_id_present) {
    HANDLE_CODE(pack_integer(bref, hop_id, (uint16_t)0u, (uint16_t)1023u));
  }
  if (p0_nominal_present) {
    HANDLE_CODE(pack_integer(bref, p0_nominal, (int16_t)-202, (int16_t)24));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pucch_res_common_present, 1));
  HANDLE_CODE(bref.unpack(hop_id_present, 1));
  HANDLE_CODE(bref.unpack(p0_nominal_present, 1));

  if (pucch_res_common_present) {
    HANDLE_CODE(unpack_integer(pucch_res_common, bref, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(pucch_group_hop.unpack(bref));
  if (hop_id_present) {
    HANDLE_CODE(unpack_integer(hop_id, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (p0_nominal_present) {
    HANDLE_CODE(unpack_integer(p0_nominal, bref, (int16_t)-202, (int16_t)24));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pucch_res_common_present) {
    j.write_int("pucch-ResourceCommon", pucch_res_common);
  }
  j.write_str("pucch-GroupHopping", pucch_group_hop.to_string());
  if (hop_id_present) {
    j.write_int("hoppingId", hop_id);
  }
  if (p0_nominal_present) {
    j.write_int("p0-nominal", p0_nominal);
  }
  j.end_obj();
}

const char* pucch_cfg_common_s::pucch_group_hop_opts::to_string() const
{
  static const char* names[] = {"neither", "enable", "disable"};
  return convert_enum_idx(names, 3, value, "pucch_cfg_common_s::pucch_group_hop_e_");
}

// PUSCH-ConfigCommon ::= SEQUENCE
SRSASN_CODE pusch_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(group_hop_enabled_transform_precoding_present, 1));
  HANDLE_CODE(bref.pack(pusch_time_domain_alloc_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(msg3_delta_preamb_present, 1));
  HANDLE_CODE(bref.pack(p0_nominal_with_grant_present, 1));

  if (pusch_time_domain_alloc_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pusch_time_domain_alloc_list, 1, 16));
  }
  if (msg3_delta_preamb_present) {
    HANDLE_CODE(pack_integer(bref, msg3_delta_preamb, (int8_t)-1, (int8_t)6));
  }
  if (p0_nominal_with_grant_present) {
    HANDLE_CODE(pack_integer(bref, p0_nominal_with_grant, (int16_t)-202, (int16_t)24));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(group_hop_enabled_transform_precoding_present, 1));
  bool pusch_time_domain_alloc_list_present;
  HANDLE_CODE(bref.unpack(pusch_time_domain_alloc_list_present, 1));
  HANDLE_CODE(bref.unpack(msg3_delta_preamb_present, 1));
  HANDLE_CODE(bref.unpack(p0_nominal_with_grant_present, 1));

  if (pusch_time_domain_alloc_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pusch_time_domain_alloc_list, bref, 1, 16));
  }
  if (msg3_delta_preamb_present) {
    HANDLE_CODE(unpack_integer(msg3_delta_preamb, bref, (int8_t)-1, (int8_t)6));
  }
  if (p0_nominal_with_grant_present) {
    HANDLE_CODE(unpack_integer(p0_nominal_with_grant, bref, (int16_t)-202, (int16_t)24));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (group_hop_enabled_transform_precoding_present) {
    j.write_str("groupHoppingEnabledTransformPrecoding", "enabled");
  }
  if (pusch_time_domain_alloc_list.size() > 0) {
    j.start_array("pusch-TimeDomainAllocationList");
    for (const auto& e1 : pusch_time_domain_alloc_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (msg3_delta_preamb_present) {
    j.write_int("msg3-DeltaPreamble", msg3_delta_preamb);
  }
  if (p0_nominal_with_grant_present) {
    j.write_int("p0-NominalWithGrant", p0_nominal_with_grant);
  }
  j.end_obj();
}

// PagingCycle ::= ENUMERATED
const char* paging_cycle_opts::to_string() const
{
  static const char* names[] = {"rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(names, 4, value, "paging_cycle_e");
}
uint16_t paging_cycle_opts::to_number() const
{
  static const uint16_t numbers[] = {32, 64, 128, 256};
  return map_enum_number(numbers, 4, value, "paging_cycle_e");
}

// RACH-ConfigCommon ::= SEQUENCE
SRSASN_CODE rach_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(total_nof_ra_preambs_present, 1));
  HANDLE_CODE(bref.pack(ssb_per_rach_occasion_and_cb_preambs_per_ssb_present, 1));
  HANDLE_CODE(bref.pack(group_bcfg_present, 1));
  HANDLE_CODE(bref.pack(rsrp_thres_ssb_present, 1));
  HANDLE_CODE(bref.pack(rsrp_thres_ssb_sul_present, 1));
  HANDLE_CODE(bref.pack(msg1_subcarrier_spacing_present, 1));
  HANDLE_CODE(bref.pack(msg3_transform_precoder_present, 1));

  HANDLE_CODE(rach_cfg_generic.pack(bref));
  if (total_nof_ra_preambs_present) {
    HANDLE_CODE(pack_integer(bref, total_nof_ra_preambs, (uint8_t)1u, (uint8_t)63u));
  }
  if (ssb_per_rach_occasion_and_cb_preambs_per_ssb_present) {
    HANDLE_CODE(ssb_per_rach_occasion_and_cb_preambs_per_ssb.pack(bref));
  }
  if (group_bcfg_present) {
    HANDLE_CODE(group_bcfg.ra_msg3_size_group_a.pack(bref));
    HANDLE_CODE(group_bcfg.msg_pwr_offset_group_b.pack(bref));
    HANDLE_CODE(pack_integer(bref, group_bcfg.nof_ra_preambs_group_a, (uint8_t)1u, (uint8_t)64u));
  }
  HANDLE_CODE(ra_contention_resolution_timer.pack(bref));
  if (rsrp_thres_ssb_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_thres_ssb, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrp_thres_ssb_sul_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_thres_ssb_sul, (uint8_t)0u, (uint8_t)127u));
  }
  HANDLE_CODE(prach_root_seq_idx.pack(bref));
  if (msg1_subcarrier_spacing_present) {
    HANDLE_CODE(msg1_subcarrier_spacing.pack(bref));
  }
  HANDLE_CODE(restricted_set_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(total_nof_ra_preambs_present, 1));
  HANDLE_CODE(bref.unpack(ssb_per_rach_occasion_and_cb_preambs_per_ssb_present, 1));
  HANDLE_CODE(bref.unpack(group_bcfg_present, 1));
  HANDLE_CODE(bref.unpack(rsrp_thres_ssb_present, 1));
  HANDLE_CODE(bref.unpack(rsrp_thres_ssb_sul_present, 1));
  HANDLE_CODE(bref.unpack(msg1_subcarrier_spacing_present, 1));
  HANDLE_CODE(bref.unpack(msg3_transform_precoder_present, 1));

  HANDLE_CODE(rach_cfg_generic.unpack(bref));
  if (total_nof_ra_preambs_present) {
    HANDLE_CODE(unpack_integer(total_nof_ra_preambs, bref, (uint8_t)1u, (uint8_t)63u));
  }
  if (ssb_per_rach_occasion_and_cb_preambs_per_ssb_present) {
    HANDLE_CODE(ssb_per_rach_occasion_and_cb_preambs_per_ssb.unpack(bref));
  }
  if (group_bcfg_present) {
    HANDLE_CODE(group_bcfg.ra_msg3_size_group_a.unpack(bref));
    HANDLE_CODE(group_bcfg.msg_pwr_offset_group_b.unpack(bref));
    HANDLE_CODE(unpack_integer(group_bcfg.nof_ra_preambs_group_a, bref, (uint8_t)1u, (uint8_t)64u));
  }
  HANDLE_CODE(ra_contention_resolution_timer.unpack(bref));
  if (rsrp_thres_ssb_present) {
    HANDLE_CODE(unpack_integer(rsrp_thres_ssb, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (rsrp_thres_ssb_sul_present) {
    HANDLE_CODE(unpack_integer(rsrp_thres_ssb_sul, bref, (uint8_t)0u, (uint8_t)127u));
  }
  HANDLE_CODE(prach_root_seq_idx.unpack(bref));
  if (msg1_subcarrier_spacing_present) {
    HANDLE_CODE(msg1_subcarrier_spacing.unpack(bref));
  }
  HANDLE_CODE(restricted_set_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void rach_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rach-ConfigGeneric");
  rach_cfg_generic.to_json(j);
  if (total_nof_ra_preambs_present) {
    j.write_int("totalNumberOfRA-Preambles", total_nof_ra_preambs);
  }
  if (ssb_per_rach_occasion_and_cb_preambs_per_ssb_present) {
    j.write_fieldname("ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
    ssb_per_rach_occasion_and_cb_preambs_per_ssb.to_json(j);
  }
  if (group_bcfg_present) {
    j.write_fieldname("groupBconfigured");
    j.start_obj();
    j.write_str("ra-Msg3SizeGroupA", group_bcfg.ra_msg3_size_group_a.to_string());
    j.write_str("messagePowerOffsetGroupB", group_bcfg.msg_pwr_offset_group_b.to_string());
    j.write_int("numberOfRA-PreamblesGroupA", group_bcfg.nof_ra_preambs_group_a);
    j.end_obj();
  }
  j.write_str("ra-ContentionResolutionTimer", ra_contention_resolution_timer.to_string());
  if (rsrp_thres_ssb_present) {
    j.write_int("rsrp-ThresholdSSB", rsrp_thres_ssb);
  }
  if (rsrp_thres_ssb_sul_present) {
    j.write_int("rsrp-ThresholdSSB-SUL", rsrp_thres_ssb_sul);
  }
  j.write_fieldname("prach-RootSequenceIndex");
  prach_root_seq_idx.to_json(j);
  if (msg1_subcarrier_spacing_present) {
    j.write_str("msg1-SubcarrierSpacing", msg1_subcarrier_spacing.to_string());
  }
  j.write_str("restrictedSetConfig", restricted_set_cfg.to_string());
  if (msg3_transform_precoder_present) {
    j.write_str("msg3-transformPrecoder", "enabled");
  }
  j.end_obj();
}

void rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::destroy_() {}
void rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_(
    const rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_eighth:
      c.init(other.c.get<one_eighth_e_>());
      break;
    case types::one_fourth:
      c.init(other.c.get<one_fourth_e_>());
      break;
    case types::one_half:
      c.init(other.c.get<one_half_e_>());
      break;
    case types::one:
      c.init(other.c.get<one_e_>());
      break;
    case types::two:
      c.init(other.c.get<two_e_>());
      break;
    case types::four:
      c.init(other.c.get<uint8_t>());
      break;
    case types::eight:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sixteen:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_");
  }
}
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_&
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::operator=(
    const rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_eighth:
      c.set(other.c.get<one_eighth_e_>());
      break;
    case types::one_fourth:
      c.set(other.c.get<one_fourth_e_>());
      break;
    case types::one_half:
      c.set(other.c.get<one_half_e_>());
      break;
    case types::one:
      c.set(other.c.get<one_e_>());
      break;
    case types::two:
      c.set(other.c.get<two_e_>());
      break;
    case types::four:
      c.set(other.c.get<uint8_t>());
      break;
    case types::eight:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sixteen:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_");
  }

  return *this;
}
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_eighth_e_&
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_one_eighth()
{
  set(types::one_eighth);
  return c.get<one_eighth_e_>();
}
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_fourth_e_&
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_one_fourth()
{
  set(types::one_fourth);
  return c.get<one_fourth_e_>();
}
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_half_e_&
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_one_half()
{
  set(types::one_half);
  return c.get<one_half_e_>();
}
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_e_&
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_one()
{
  set(types::one);
  return c.get<one_e_>();
}
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::two_e_&
rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_two()
{
  set(types::two);
  return c.get<two_e_>();
}
uint8_t& rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_four()
{
  set(types::four);
  return c.get<uint8_t>();
}
uint8_t& rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_eight()
{
  set(types::eight);
  return c.get<uint8_t>();
}
uint8_t& rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::set_sixteen()
{
  set(types::sixteen);
  return c.get<uint8_t>();
}
void rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_eighth:
      j.write_str("oneEighth", c.get<one_eighth_e_>().to_string());
      break;
    case types::one_fourth:
      j.write_str("oneFourth", c.get<one_fourth_e_>().to_string());
      break;
    case types::one_half:
      j.write_str("oneHalf", c.get<one_half_e_>().to_string());
      break;
    case types::one:
      j.write_str("one", c.get<one_e_>().to_string());
      break;
    case types::two:
      j.write_str("two", c.get<two_e_>().to_string());
      break;
    case types::four:
      j.write_int("four", c.get<uint8_t>());
      break;
    case types::eight:
      j.write_int("eight", c.get<uint8_t>());
      break;
    case types::sixteen:
      j.write_int("sixteen", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_");
  }
  j.end_obj();
}
SRSASN_CODE rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_eighth:
      HANDLE_CODE(c.get<one_eighth_e_>().pack(bref));
      break;
    case types::one_fourth:
      HANDLE_CODE(c.get<one_fourth_e_>().pack(bref));
      break;
    case types::one_half:
      HANDLE_CODE(c.get<one_half_e_>().pack(bref));
      break;
    case types::one:
      HANDLE_CODE(c.get<one_e_>().pack(bref));
      break;
    case types::two:
      HANDLE_CODE(c.get<two_e_>().pack(bref));
      break;
    case types::four:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)16u));
      break;
    case types::eight:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)8u));
      break;
    case types::sixteen:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_eighth:
      HANDLE_CODE(c.get<one_eighth_e_>().unpack(bref));
      break;
    case types::one_fourth:
      HANDLE_CODE(c.get<one_fourth_e_>().unpack(bref));
      break;
    case types::one_half:
      HANDLE_CODE(c.get<one_half_e_>().unpack(bref));
      break;
    case types::one:
      HANDLE_CODE(c.get<one_e_>().unpack(bref));
      break;
    case types::two:
      HANDLE_CODE(c.get<two_e_>().unpack(bref));
      break;
    case types::four:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)16u));
      break;
    case types::eight:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)8u));
      break;
    case types::sixteen:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_eighth_opts::to_string() const
{
  static const char* names[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60", "n64"};
  return convert_enum_idx(
      names, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_eighth_e_");
}
uint8_t rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_eighth_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
  return map_enum_number(
      numbers, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_eighth_e_");
}

const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_fourth_opts::to_string() const
{
  static const char* names[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60", "n64"};
  return convert_enum_idx(
      names, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_fourth_e_");
}
uint8_t rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_fourth_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
  return map_enum_number(
      numbers, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_fourth_e_");
}

const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_half_opts::to_string() const
{
  static const char* names[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60", "n64"};
  return convert_enum_idx(
      names, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_half_e_");
}
uint8_t rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_half_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
  return map_enum_number(
      numbers, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_half_e_");
}

const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_opts::to_string() const
{
  static const char* names[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60", "n64"};
  return convert_enum_idx(
      names, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_e_");
}
uint8_t rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
  return map_enum_number(
      numbers, 16, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::one_e_");
}

const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::two_opts::to_string() const
{
  static const char* names[] = {"n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32"};
  return convert_enum_idx(
      names, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::two_e_");
}
uint8_t rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::two_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 12, 16, 20, 24, 28, 32};
  return map_enum_number(
      numbers, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::two_e_");
}

const char* rach_cfg_common_s::group_bcfg_s_::ra_msg3_size_group_a_opts::to_string() const
{
  static const char* names[] = {"b56",
                                "b144",
                                "b208",
                                "b256",
                                "b282",
                                "b480",
                                "b640",
                                "b800",
                                "b1000",
                                "b72",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "rach_cfg_common_s::group_bcfg_s_::ra_msg3_size_group_a_e_");
}
uint16_t rach_cfg_common_s::group_bcfg_s_::ra_msg3_size_group_a_opts::to_number() const
{
  static const uint16_t numbers[] = {56, 144, 208, 256, 282, 480, 640, 800, 1000, 72};
  return map_enum_number(numbers, 10, value, "rach_cfg_common_s::group_bcfg_s_::ra_msg3_size_group_a_e_");
}

const char* rach_cfg_common_s::group_bcfg_s_::msg_pwr_offset_group_b_opts::to_string() const
{
  static const char* names[] = {"minusinfinity", "dB0", "dB5", "dB8", "dB10", "dB12", "dB15", "dB18"};
  return convert_enum_idx(names, 8, value, "rach_cfg_common_s::group_bcfg_s_::msg_pwr_offset_group_b_e_");
}
int8_t rach_cfg_common_s::group_bcfg_s_::msg_pwr_offset_group_b_opts::to_number() const
{
  static const int8_t numbers[] = {-1, 0, 5, 8, 10, 12, 15, 18};
  return map_enum_number(numbers, 8, value, "rach_cfg_common_s::group_bcfg_s_::msg_pwr_offset_group_b_e_");
}

const char* rach_cfg_common_s::ra_contention_resolution_timer_opts::to_string() const
{
  static const char* names[] = {"sf8", "sf16", "sf24", "sf32", "sf40", "sf48", "sf56", "sf64"};
  return convert_enum_idx(names, 8, value, "rach_cfg_common_s::ra_contention_resolution_timer_e_");
}
uint8_t rach_cfg_common_s::ra_contention_resolution_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 16, 24, 32, 40, 48, 56, 64};
  return map_enum_number(numbers, 8, value, "rach_cfg_common_s::ra_contention_resolution_timer_e_");
}

void rach_cfg_common_s::prach_root_seq_idx_c_::destroy_() {}
void rach_cfg_common_s::prach_root_seq_idx_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rach_cfg_common_s::prach_root_seq_idx_c_::prach_root_seq_idx_c_(const rach_cfg_common_s::prach_root_seq_idx_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::l839:
      c.init(other.c.get<uint16_t>());
      break;
    case types::l139:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::prach_root_seq_idx_c_");
  }
}
rach_cfg_common_s::prach_root_seq_idx_c_&
rach_cfg_common_s::prach_root_seq_idx_c_::operator=(const rach_cfg_common_s::prach_root_seq_idx_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::l839:
      c.set(other.c.get<uint16_t>());
      break;
    case types::l139:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::prach_root_seq_idx_c_");
  }

  return *this;
}
uint16_t& rach_cfg_common_s::prach_root_seq_idx_c_::set_l839()
{
  set(types::l839);
  return c.get<uint16_t>();
}
uint8_t& rach_cfg_common_s::prach_root_seq_idx_c_::set_l139()
{
  set(types::l139);
  return c.get<uint8_t>();
}
void rach_cfg_common_s::prach_root_seq_idx_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::l839:
      j.write_int("l839", c.get<uint16_t>());
      break;
    case types::l139:
      j.write_int("l139", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::prach_root_seq_idx_c_");
  }
  j.end_obj();
}
SRSASN_CODE rach_cfg_common_s::prach_root_seq_idx_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::l839:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)837u));
      break;
    case types::l139:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)137u));
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::prach_root_seq_idx_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_common_s::prach_root_seq_idx_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::l839:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)837u));
      break;
    case types::l139:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)137u));
      break;
    default:
      log_invalid_choice_id(type_, "rach_cfg_common_s::prach_root_seq_idx_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rach_cfg_common_s::restricted_set_cfg_opts::to_string() const
{
  static const char* names[] = {"unrestrictedSet", "restrictedSetTypeA", "restrictedSetTypeB"};
  return convert_enum_idx(names, 3, value, "rach_cfg_common_s::restricted_set_cfg_e_");
}

// SCS-SpecificCarrier ::= SEQUENCE
SRSASN_CODE scs_specific_carrier_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, offset_to_carrier, (uint16_t)0u, (uint16_t)2199u));
  HANDLE_CODE(subcarrier_spacing.pack(bref));
  HANDLE_CODE(pack_integer(bref, carrier_bw, (uint16_t)1u, (uint16_t)275u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= tx_direct_current_location_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tx_direct_current_location_present, 1));
      if (tx_direct_current_location_present) {
        HANDLE_CODE(pack_integer(bref, tx_direct_current_location, (uint16_t)0u, (uint16_t)4095u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scs_specific_carrier_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(offset_to_carrier, bref, (uint16_t)0u, (uint16_t)2199u));
  HANDLE_CODE(subcarrier_spacing.unpack(bref));
  HANDLE_CODE(unpack_integer(carrier_bw, bref, (uint16_t)1u, (uint16_t)275u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(tx_direct_current_location_present, 1));
      if (tx_direct_current_location_present) {
        HANDLE_CODE(unpack_integer(tx_direct_current_location, bref, (uint16_t)0u, (uint16_t)4095u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void scs_specific_carrier_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("offsetToCarrier", offset_to_carrier);
  j.write_str("subcarrierSpacing", subcarrier_spacing.to_string());
  j.write_int("carrierBandwidth", carrier_bw);
  if (ext) {
    if (tx_direct_current_location_present) {
      j.write_int("txDirectCurrentLocation", tx_direct_current_location);
    }
  }
  j.end_obj();
}

// BCCH-Config ::= SEQUENCE
SRSASN_CODE bcch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(mod_period_coeff.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(mod_period_coeff.unpack(bref));

  return SRSASN_SUCCESS;
}
void bcch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("modificationPeriodCoeff", mod_period_coeff.to_string());
  j.end_obj();
}

const char* bcch_cfg_s::mod_period_coeff_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8", "n16"};
  return convert_enum_idx(names, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}
uint8_t bcch_cfg_s::mod_period_coeff_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 16};
  return map_enum_number(numbers, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}

// BWP-DownlinkCommon ::= SEQUENCE
SRSASN_CODE bwp_dl_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdcch_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_common_present, 1));

  HANDLE_CODE(generic_params.pack(bref));
  if (pdcch_cfg_common_present) {
    HANDLE_CODE(pdcch_cfg_common.pack(bref));
  }
  if (pdsch_cfg_common_present) {
    HANDLE_CODE(pdsch_cfg_common.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_dl_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdcch_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_common_present, 1));

  HANDLE_CODE(generic_params.unpack(bref));
  if (pdcch_cfg_common_present) {
    HANDLE_CODE(pdcch_cfg_common.unpack(bref));
  }
  if (pdsch_cfg_common_present) {
    HANDLE_CODE(pdsch_cfg_common.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bwp_dl_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("genericParameters");
  generic_params.to_json(j);
  if (pdcch_cfg_common_present) {
    j.write_fieldname("pdcch-ConfigCommon");
    pdcch_cfg_common.to_json(j);
  }
  if (pdsch_cfg_common_present) {
    j.write_fieldname("pdsch-ConfigCommon");
    pdsch_cfg_common.to_json(j);
  }
  j.end_obj();
}

// BWP-UplinkCommon ::= SEQUENCE
SRSASN_CODE bwp_ul_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rach_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(pusch_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(pucch_cfg_common_present, 1));

  HANDLE_CODE(generic_params.pack(bref));
  if (rach_cfg_common_present) {
    HANDLE_CODE(rach_cfg_common.pack(bref));
  }
  if (pusch_cfg_common_present) {
    HANDLE_CODE(pusch_cfg_common.pack(bref));
  }
  if (pucch_cfg_common_present) {
    HANDLE_CODE(pucch_cfg_common.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_ul_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rach_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(pusch_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(pucch_cfg_common_present, 1));

  HANDLE_CODE(generic_params.unpack(bref));
  if (rach_cfg_common_present) {
    HANDLE_CODE(rach_cfg_common.unpack(bref));
  }
  if (pusch_cfg_common_present) {
    HANDLE_CODE(pusch_cfg_common.unpack(bref));
  }
  if (pucch_cfg_common_present) {
    HANDLE_CODE(pucch_cfg_common.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bwp_ul_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("genericParameters");
  generic_params.to_json(j);
  if (rach_cfg_common_present) {
    j.write_fieldname("rach-ConfigCommon");
    rach_cfg_common.to_json(j);
  }
  if (pusch_cfg_common_present) {
    j.write_fieldname("pusch-ConfigCommon");
    pusch_cfg_common.to_json(j);
  }
  if (pucch_cfg_common_present) {
    j.write_fieldname("pucch-ConfigCommon");
    pucch_cfg_common.to_json(j);
  }
  j.end_obj();
}

// FrequencyInfoDL-SIB ::= SEQUENCE
SRSASN_CODE freq_info_dl_sib_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8));
  HANDLE_CODE(pack_integer(bref, offset_to_point_a, (uint16_t)0u, (uint16_t)2199u));
  HANDLE_CODE(pack_dyn_seq_of(bref, scs_specific_carrier_list, 1, 5));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_info_dl_sib_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8));
  HANDLE_CODE(unpack_integer(offset_to_point_a, bref, (uint16_t)0u, (uint16_t)2199u));
  HANDLE_CODE(unpack_dyn_seq_of(scs_specific_carrier_list, bref, 1, 5));

  return SRSASN_SUCCESS;
}
void freq_info_dl_sib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("frequencyBandList");
  for (const auto& e1 : freq_band_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("offsetToPointA", offset_to_point_a);
  j.start_array("scs-SpecificCarrierList");
  for (const auto& e1 : scs_specific_carrier_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// FrequencyInfoUL-SIB ::= SEQUENCE
SRSASN_CODE freq_info_ul_sib_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(absolute_freq_point_a_present, 1));
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(freq_shift7p5khz_present, 1));

  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8));
  }
  if (absolute_freq_point_a_present) {
    HANDLE_CODE(pack_integer(bref, absolute_freq_point_a, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, scs_specific_carrier_list, 1, 5));
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_info_ul_sib_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  HANDLE_CODE(bref.unpack(absolute_freq_point_a_present, 1));
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(freq_shift7p5khz_present, 1));

  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8));
  }
  if (absolute_freq_point_a_present) {
    HANDLE_CODE(unpack_integer(absolute_freq_point_a, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(scs_specific_carrier_list, bref, 1, 5));
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }

  return SRSASN_SUCCESS;
}
void freq_info_ul_sib_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (absolute_freq_point_a_present) {
    j.write_int("absoluteFrequencyPointA", absolute_freq_point_a);
  }
  j.start_array("scs-SpecificCarrierList");
  for (const auto& e1 : scs_specific_carrier_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  if (freq_shift7p5khz_present) {
    j.write_str("frequencyShift7p5khz", "true");
  }
  j.end_obj();
}

// PCCH-Config ::= SEQUENCE
SRSASN_CODE pcch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(first_pdcch_monitoring_occasion_of_po_present, 1));

  HANDLE_CODE(default_paging_cycle.pack(bref));
  HANDLE_CODE(nand_paging_frame_offset.pack(bref));
  HANDLE_CODE(ns.pack(bref));
  if (first_pdcch_monitoring_occasion_of_po_present) {
    HANDLE_CODE(first_pdcch_monitoring_occasion_of_po.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(first_pdcch_monitoring_occasion_of_po_present, 1));

  HANDLE_CODE(default_paging_cycle.unpack(bref));
  HANDLE_CODE(nand_paging_frame_offset.unpack(bref));
  HANDLE_CODE(ns.unpack(bref));
  if (first_pdcch_monitoring_occasion_of_po_present) {
    HANDLE_CODE(first_pdcch_monitoring_occasion_of_po.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pcch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("defaultPagingCycle", default_paging_cycle.to_string());
  j.write_fieldname("nAndPagingFrameOffset");
  nand_paging_frame_offset.to_json(j);
  j.write_str("ns", ns.to_string());
  if (first_pdcch_monitoring_occasion_of_po_present) {
    j.write_fieldname("firstPDCCH-MonitoringOccasionOfPO");
    first_pdcch_monitoring_occasion_of_po.to_json(j);
  }
  j.end_obj();
}

void pcch_cfg_s::nand_paging_frame_offset_c_::destroy_() {}
void pcch_cfg_s::nand_paging_frame_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
pcch_cfg_s::nand_paging_frame_offset_c_::nand_paging_frame_offset_c_(
    const pcch_cfg_s::nand_paging_frame_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_t:
      break;
    case types::half_t:
      c.init(other.c.get<uint8_t>());
      break;
    case types::quarter_t:
      c.init(other.c.get<uint8_t>());
      break;
    case types::one_eighth_t:
      c.init(other.c.get<uint8_t>());
      break;
    case types::one_sixteenth_t:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::nand_paging_frame_offset_c_");
  }
}
pcch_cfg_s::nand_paging_frame_offset_c_&
pcch_cfg_s::nand_paging_frame_offset_c_::operator=(const pcch_cfg_s::nand_paging_frame_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_t:
      break;
    case types::half_t:
      c.set(other.c.get<uint8_t>());
      break;
    case types::quarter_t:
      c.set(other.c.get<uint8_t>());
      break;
    case types::one_eighth_t:
      c.set(other.c.get<uint8_t>());
      break;
    case types::one_sixteenth_t:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::nand_paging_frame_offset_c_");
  }

  return *this;
}
void pcch_cfg_s::nand_paging_frame_offset_c_::set_one_t()
{
  set(types::one_t);
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_half_t()
{
  set(types::half_t);
  return c.get<uint8_t>();
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_quarter_t()
{
  set(types::quarter_t);
  return c.get<uint8_t>();
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_one_eighth_t()
{
  set(types::one_eighth_t);
  return c.get<uint8_t>();
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_one_sixteenth_t()
{
  set(types::one_sixteenth_t);
  return c.get<uint8_t>();
}
void pcch_cfg_s::nand_paging_frame_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_t:
      break;
    case types::half_t:
      j.write_int("halfT", c.get<uint8_t>());
      break;
    case types::quarter_t:
      j.write_int("quarterT", c.get<uint8_t>());
      break;
    case types::one_eighth_t:
      j.write_int("oneEighthT", c.get<uint8_t>());
      break;
    case types::one_sixteenth_t:
      j.write_int("oneSixteenthT", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::nand_paging_frame_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE pcch_cfg_s::nand_paging_frame_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_t:
      break;
    case types::half_t:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::quarter_t:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::one_eighth_t:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::one_sixteenth_t:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::nand_paging_frame_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_s::nand_paging_frame_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_t:
      break;
    case types::half_t:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::quarter_t:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::one_eighth_t:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::one_sixteenth_t:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::nand_paging_frame_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pcch_cfg_s::ns_opts::to_string() const
{
  static const char* names[] = {"four", "two", "one"};
  return convert_enum_idx(names, 3, value, "pcch_cfg_s::ns_e_");
}
uint8_t pcch_cfg_s::ns_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 2, 1};
  return map_enum_number(numbers, 3, value, "pcch_cfg_s::ns_e_");
}

void pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::destroy_()
{
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.destroy<scs15_kh_zone_t_l_>();
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.destroy<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.destroy<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.destroy<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.destroy<scs120_kh_zone_sixteenth_t_l_>();
      break;
    default:
      break;
  }
}
void pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.init<scs15_kh_zone_t_l_>();
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.init<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.init<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.init<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.init<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.init<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.init<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.init<scs120_kh_zone_sixteenth_t_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_");
  }
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::first_pdcch_monitoring_occasion_of_po_c_(
    const pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.init(other.c.get<scs15_kh_zone_t_l_>());
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.init(other.c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>());
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.init(other.c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>());
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.init(other.c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>());
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.init(other.c.get<scs120_kh_zone_sixteenth_t_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_");
  }
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_& pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::operator=(
    const pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::scs15_kh_zone_t:
      c.set(other.c.get<scs15_kh_zone_t_l_>());
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      c.set(other.c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>());
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      c.set(other.c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>());
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      c.set(other.c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>());
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs120_kh_zone_sixteenth_t:
      c.set(other.c.get<scs120_kh_zone_sixteenth_t_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_");
  }

  return *this;
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs15_kh_zone_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs15_kh_zone_t()
{
  set(types::scs15_kh_zone_t);
  return c.get<scs15_kh_zone_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs30_kh_zone_t_scs15_kh_zhalf_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs30_kh_zone_t_scs15_kh_zhalf_t()
{
  set(types::scs30_kh_zone_t_scs15_kh_zhalf_t);
  return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
{
  set(types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t);
  return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
{
  set(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t);
  return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs120_kh_zone_sixteenth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs120_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zone_sixteenth_t_l_>();
}
void pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scs15_kh_zone_t:
      j.start_array("sCS15KHZoneT");
      for (const auto& e1 : c.get<scs15_kh_zone_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      j.start_array("sCS30KHZoneT-SCS15KHZhalfT");
      for (const auto& e1 : c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      j.start_array("sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT");
      for (const auto& e1 : c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      j.start_array("sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT");
      for (const auto& e1 : c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT");
      for (const auto& e1 :
           c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZoneEighthT-SCS60KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs120_kh_zone_sixteenth_t:
      j.start_array("sCS120KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs120_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_");
  }
  j.end_obj();
}
SRSASN_CODE pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scs15_kh_zone_t:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scs15_kh_zone_t_l_>(), 1, 4, integer_packer<uint8_t>(0, 139)));
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>(), 1, 4, integer_packer<uint16_t>(0, 279)));
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>(),
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 559)));
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      HANDLE_CODE(
          pack_dyn_seq_of(bref,
                          c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>(),
                          1,
                          4,
                          integer_packer<uint16_t>(0, 1119)));
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(
          bref,
          c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>(),
          1,
          4,
          integer_packer<uint16_t>(0, 2239)));
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>(),
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 4479)));
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>(),
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 8959)));
      break;
    case types::scs120_kh_zone_sixteenth_t:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<scs120_kh_zone_sixteenth_t_l_>(), 1, 4, integer_packer<uint16_t>(0, 17919)));
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scs15_kh_zone_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs15_kh_zone_t_l_>(), bref, 1, 4, integer_packer<uint8_t>(0, 139)));
      break;
    case types::scs30_kh_zone_t_scs15_kh_zhalf_t:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>(), bref, 1, 4, integer_packer<uint16_t>(0, 279)));
      break;
    case types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>(),
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 559)));
      break;
    case types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>(),
                            bref,
                            1,
                            4,
                            integer_packer<uint16_t>(0, 1119)));
      break;
    case types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>(),
          bref,
          1,
          4,
          integer_packer<uint16_t>(0, 2239)));
      break;
    case types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>(),
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 4479)));
      break;
    case types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>(),
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 8959)));
      break;
    case types::scs120_kh_zone_sixteenth_t:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<scs120_kh_zone_sixteenth_t_l_>(), bref, 1, 4, integer_packer<uint16_t>(0, 17919)));
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// TDD-UL-DL-Pattern ::= SEQUENCE
SRSASN_CODE tdd_ul_dl_pattern_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(dl_ul_tx_periodicity.pack(bref));
  HANDLE_CODE(pack_integer(bref, nrof_dl_slots, (uint16_t)0u, (uint16_t)320u));
  HANDLE_CODE(pack_integer(bref, nrof_dl_symbols, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(pack_integer(bref, nrof_ul_slots, (uint16_t)0u, (uint16_t)320u));
  HANDLE_CODE(pack_integer(bref, nrof_ul_symbols, (uint8_t)0u, (uint8_t)13u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= dl_ul_tx_periodicity_v1530_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_ul_tx_periodicity_v1530_present, 1));
      if (dl_ul_tx_periodicity_v1530_present) {
        HANDLE_CODE(dl_ul_tx_periodicity_v1530.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_pattern_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(dl_ul_tx_periodicity.unpack(bref));
  HANDLE_CODE(unpack_integer(nrof_dl_slots, bref, (uint16_t)0u, (uint16_t)320u));
  HANDLE_CODE(unpack_integer(nrof_dl_symbols, bref, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(unpack_integer(nrof_ul_slots, bref, (uint16_t)0u, (uint16_t)320u));
  HANDLE_CODE(unpack_integer(nrof_ul_symbols, bref, (uint8_t)0u, (uint8_t)13u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(dl_ul_tx_periodicity_v1530_present, 1));
      if (dl_ul_tx_periodicity_v1530_present) {
        HANDLE_CODE(dl_ul_tx_periodicity_v1530.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void tdd_ul_dl_pattern_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dl-UL-TransmissionPeriodicity", dl_ul_tx_periodicity.to_string());
  j.write_int("nrofDownlinkSlots", nrof_dl_slots);
  j.write_int("nrofDownlinkSymbols", nrof_dl_symbols);
  j.write_int("nrofUplinkSlots", nrof_ul_slots);
  j.write_int("nrofUplinkSymbols", nrof_ul_symbols);
  if (ext) {
    if (dl_ul_tx_periodicity_v1530_present) {
      j.write_str("dl-UL-TransmissionPeriodicity-v1530", dl_ul_tx_periodicity_v1530.to_string());
    }
  }
  j.end_obj();
}

const char* tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::to_string() const
{
  static const char* names[] = {"ms0p5", "ms0p625", "ms1", "ms1p25", "ms2", "ms2p5", "ms5", "ms10"};
  return convert_enum_idx(names, 8, value, "tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_e_");
}

const char* tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_opts::to_string() const
{
  static const char* names[] = {"ms3", "ms4"};
  return convert_enum_idx(names, 2, value, "tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_e_");
}
uint8_t tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4};
  return map_enum_number(numbers, 2, value, "tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_e_");
}

// TimeAlignmentTimer ::= ENUMERATED
const char* time_align_timer_opts::to_string() const
{
  static const char* names[] = {"ms500", "ms750", "ms1280", "ms1920", "ms2560", "ms5120", "ms10240", "infinity"};
  return convert_enum_idx(names, 8, value, "time_align_timer_e");
}
int16_t time_align_timer_opts::to_number() const
{
  static const int16_t numbers[] = {500, 750, 1280, 1920, 2560, 5120, 10240, -1};
  return map_enum_number(numbers, 8, value, "time_align_timer_e");
}

// DownlinkConfigCommonSIB ::= SEQUENCE
SRSASN_CODE dl_cfg_common_sib_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(freq_info_dl.pack(bref));
  HANDLE_CODE(init_dl_bwp.pack(bref));
  HANDLE_CODE(bcch_cfg.pack(bref));
  HANDLE_CODE(pcch_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_cfg_common_sib_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(freq_info_dl.unpack(bref));
  HANDLE_CODE(init_dl_bwp.unpack(bref));
  HANDLE_CODE(bcch_cfg.unpack(bref));
  HANDLE_CODE(pcch_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void dl_cfg_common_sib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("frequencyInfoDL");
  freq_info_dl.to_json(j);
  j.write_fieldname("initialDownlinkBWP");
  init_dl_bwp.to_json(j);
  j.write_fieldname("bcch-Config");
  bcch_cfg.to_json(j);
  j.write_fieldname("pcch-Config");
  pcch_cfg.to_json(j);
  j.end_obj();
}

// TDD-UL-DL-ConfigCommon ::= SEQUENCE
SRSASN_CODE tdd_ul_dl_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pattern2_present, 1));

  HANDLE_CODE(ref_subcarrier_spacing.pack(bref));
  HANDLE_CODE(pattern1.pack(bref));
  if (pattern2_present) {
    HANDLE_CODE(pattern2.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pattern2_present, 1));

  HANDLE_CODE(ref_subcarrier_spacing.unpack(bref));
  HANDLE_CODE(pattern1.unpack(bref));
  if (pattern2_present) {
    HANDLE_CODE(pattern2.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tdd_ul_dl_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("referenceSubcarrierSpacing", ref_subcarrier_spacing.to_string());
  j.write_fieldname("pattern1");
  pattern1.to_json(j);
  if (pattern2_present) {
    j.write_fieldname("pattern2");
    pattern2.to_json(j);
  }
  j.end_obj();
}

// UplinkConfigCommonSIB ::= SEQUENCE
SRSASN_CODE ul_cfg_common_sib_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(freq_info_ul.pack(bref));
  HANDLE_CODE(init_ul_bwp.pack(bref));
  HANDLE_CODE(time_align_timer_common.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_common_sib_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(freq_info_ul.unpack(bref));
  HANDLE_CODE(init_ul_bwp.unpack(bref));
  HANDLE_CODE(time_align_timer_common.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_cfg_common_sib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("frequencyInfoUL");
  freq_info_ul.to_json(j);
  j.write_fieldname("initialUplinkBWP");
  init_ul_bwp.to_json(j);
  j.write_str("timeAlignmentTimerCommon", time_align_timer_common.to_string());
  j.end_obj();
}

// ServingCellConfigCommonSIB ::= SEQUENCE
SRSASN_CODE serving_cell_cfg_common_sib_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(supplementary_ul_present, 1));
  HANDLE_CODE(bref.pack(n_timing_advance_offset_present, 1));
  HANDLE_CODE(bref.pack(tdd_ul_dl_cfg_common_present, 1));

  HANDLE_CODE(dl_cfg_common.pack(bref));
  if (ul_cfg_common_present) {
    HANDLE_CODE(ul_cfg_common.pack(bref));
  }
  if (supplementary_ul_present) {
    HANDLE_CODE(supplementary_ul.pack(bref));
  }
  if (n_timing_advance_offset_present) {
    HANDLE_CODE(n_timing_advance_offset.pack(bref));
  }
  HANDLE_CODE(bref.pack(ssb_positions_in_burst.group_presence_present, 1));
  HANDLE_CODE(ssb_positions_in_burst.in_one_group.pack(bref));
  if (ssb_positions_in_burst.group_presence_present) {
    HANDLE_CODE(ssb_positions_in_burst.group_presence.pack(bref));
  }
  HANDLE_CODE(ssb_periodicity_serving_cell.pack(bref));
  if (tdd_ul_dl_cfg_common_present) {
    HANDLE_CODE(tdd_ul_dl_cfg_common.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, ss_pbch_block_pwr, (int8_t)-60, (int8_t)50));

  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_cfg_common_sib_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(supplementary_ul_present, 1));
  HANDLE_CODE(bref.unpack(n_timing_advance_offset_present, 1));
  HANDLE_CODE(bref.unpack(tdd_ul_dl_cfg_common_present, 1));

  HANDLE_CODE(dl_cfg_common.unpack(bref));
  if (ul_cfg_common_present) {
    HANDLE_CODE(ul_cfg_common.unpack(bref));
  }
  if (supplementary_ul_present) {
    HANDLE_CODE(supplementary_ul.unpack(bref));
  }
  if (n_timing_advance_offset_present) {
    HANDLE_CODE(n_timing_advance_offset.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(ssb_positions_in_burst.group_presence_present, 1));
  HANDLE_CODE(ssb_positions_in_burst.in_one_group.unpack(bref));
  if (ssb_positions_in_burst.group_presence_present) {
    HANDLE_CODE(ssb_positions_in_burst.group_presence.unpack(bref));
  }
  HANDLE_CODE(ssb_periodicity_serving_cell.unpack(bref));
  if (tdd_ul_dl_cfg_common_present) {
    HANDLE_CODE(tdd_ul_dl_cfg_common.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(ss_pbch_block_pwr, bref, (int8_t)-60, (int8_t)50));

  return SRSASN_SUCCESS;
}
void serving_cell_cfg_common_sib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("downlinkConfigCommon");
  dl_cfg_common.to_json(j);
  if (ul_cfg_common_present) {
    j.write_fieldname("uplinkConfigCommon");
    ul_cfg_common.to_json(j);
  }
  if (supplementary_ul_present) {
    j.write_fieldname("supplementaryUplink");
    supplementary_ul.to_json(j);
  }
  if (n_timing_advance_offset_present) {
    j.write_str("n-TimingAdvanceOffset", n_timing_advance_offset.to_string());
  }
  j.write_fieldname("ssb-PositionsInBurst");
  j.start_obj();
  j.write_str("inOneGroup", ssb_positions_in_burst.in_one_group.to_string());
  if (ssb_positions_in_burst.group_presence_present) {
    j.write_str("groupPresence", ssb_positions_in_burst.group_presence.to_string());
  }
  j.end_obj();
  j.write_str("ssb-PeriodicityServingCell", ssb_periodicity_serving_cell.to_string());
  if (tdd_ul_dl_cfg_common_present) {
    j.write_fieldname("tdd-UL-DL-ConfigurationCommon");
    tdd_ul_dl_cfg_common.to_json(j);
  }
  j.write_int("ss-PBCH-BlockPower", ss_pbch_block_pwr);
  j.end_obj();
}

const char* serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::to_string() const
{
  static const char* names[] = {"n0", "n25600", "n39936"};
  return convert_enum_idx(names, 3, value, "serving_cell_cfg_common_sib_s::n_timing_advance_offset_e_");
}
uint16_t serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 25600, 39936};
  return map_enum_number(numbers, 3, value, "serving_cell_cfg_common_sib_s::n_timing_advance_offset_e_");
}

const char* serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::to_string() const
{
  static const char* names[] = {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160"};
  return convert_enum_idx(names, 6, value, "serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_e_");
}
uint8_t serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_e_");
}

// BFR-CSIRS-Resource ::= SEQUENCE
SRSASN_CODE bfr_csi_rs_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ra_occasion_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ra_preamb_idx_present, 1));

  HANDLE_CODE(pack_integer(bref, csi_rs, (uint8_t)0u, (uint8_t)191u));
  if (ra_occasion_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ra_occasion_list, 1, 64, integer_packer<uint16_t>(0, 511)));
  }
  if (ra_preamb_idx_present) {
    HANDLE_CODE(pack_integer(bref, ra_preamb_idx, (uint8_t)0u, (uint8_t)63u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bfr_csi_rs_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ra_occasion_list_present;
  HANDLE_CODE(bref.unpack(ra_occasion_list_present, 1));
  HANDLE_CODE(bref.unpack(ra_preamb_idx_present, 1));

  HANDLE_CODE(unpack_integer(csi_rs, bref, (uint8_t)0u, (uint8_t)191u));
  if (ra_occasion_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ra_occasion_list, bref, 1, 64, integer_packer<uint16_t>(0, 511)));
  }
  if (ra_preamb_idx_present) {
    HANDLE_CODE(unpack_integer(ra_preamb_idx, bref, (uint8_t)0u, (uint8_t)63u));
  }

  return SRSASN_SUCCESS;
}
void bfr_csi_rs_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-RS", csi_rs);
  if (ra_occasion_list.size() > 0) {
    j.start_array("ra-OccasionList");
    for (const auto& e1 : ra_occasion_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (ra_preamb_idx_present) {
    j.write_int("ra-PreambleIndex", ra_preamb_idx);
  }
  j.end_obj();
}

// BFR-SSB-Resource ::= SEQUENCE
SRSASN_CODE bfr_ssb_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ssb, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, ra_preamb_idx, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bfr_ssb_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ssb, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(ra_preamb_idx, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void bfr_ssb_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb", ssb);
  j.write_int("ra-PreambleIndex", ra_preamb_idx);
  j.end_obj();
}

// CSI-FrequencyOccupation ::= SEQUENCE
SRSASN_CODE csi_freq_occupation_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, start_rb, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(pack_integer(bref, nrof_rbs, (uint16_t)24u, (uint16_t)276u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_freq_occupation_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(start_rb, bref, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(unpack_integer(nrof_rbs, bref, (uint16_t)24u, (uint16_t)276u));

  return SRSASN_SUCCESS;
}
void csi_freq_occupation_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("startingRB", start_rb);
  j.write_int("nrofRBs", nrof_rbs);
  j.end_obj();
}

// CSI-RS-ResourceMapping ::= SEQUENCE
SRSASN_CODE csi_rs_res_map_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(first_ofdm_symbol_in_time_domain2_present, 1));

  HANDLE_CODE(freq_domain_alloc.pack(bref));
  HANDLE_CODE(nrof_ports.pack(bref));
  HANDLE_CODE(pack_integer(bref, first_ofdm_symbol_in_time_domain, (uint8_t)0u, (uint8_t)13u));
  if (first_ofdm_symbol_in_time_domain2_present) {
    HANDLE_CODE(pack_integer(bref, first_ofdm_symbol_in_time_domain2, (uint8_t)2u, (uint8_t)12u));
  }
  HANDLE_CODE(cdm_type.pack(bref));
  HANDLE_CODE(density.pack(bref));
  HANDLE_CODE(freq_band.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_map_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(first_ofdm_symbol_in_time_domain2_present, 1));

  HANDLE_CODE(freq_domain_alloc.unpack(bref));
  HANDLE_CODE(nrof_ports.unpack(bref));
  HANDLE_CODE(unpack_integer(first_ofdm_symbol_in_time_domain, bref, (uint8_t)0u, (uint8_t)13u));
  if (first_ofdm_symbol_in_time_domain2_present) {
    HANDLE_CODE(unpack_integer(first_ofdm_symbol_in_time_domain2, bref, (uint8_t)2u, (uint8_t)12u));
  }
  HANDLE_CODE(cdm_type.unpack(bref));
  HANDLE_CODE(density.unpack(bref));
  HANDLE_CODE(freq_band.unpack(bref));

  return SRSASN_SUCCESS;
}
void csi_rs_res_map_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("frequencyDomainAllocation");
  freq_domain_alloc.to_json(j);
  j.write_str("nrofPorts", nrof_ports.to_string());
  j.write_int("firstOFDMSymbolInTimeDomain", first_ofdm_symbol_in_time_domain);
  if (first_ofdm_symbol_in_time_domain2_present) {
    j.write_int("firstOFDMSymbolInTimeDomain2", first_ofdm_symbol_in_time_domain2);
  }
  j.write_str("cdm-Type", cdm_type.to_string());
  j.write_fieldname("density");
  density.to_json(j);
  j.write_fieldname("freqBand");
  freq_band.to_json(j);
  j.end_obj();
}

void csi_rs_res_map_s::freq_domain_alloc_c_::destroy_()
{
  switch (type_) {
    case types::row1:
      c.destroy<fixed_bitstring<4>>();
      break;
    case types::row2:
      c.destroy<fixed_bitstring<12>>();
      break;
    case types::row4:
      c.destroy<fixed_bitstring<3>>();
      break;
    case types::other:
      c.destroy<fixed_bitstring<6>>();
      break;
    default:
      break;
  }
}
void csi_rs_res_map_s::freq_domain_alloc_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::row1:
      c.init<fixed_bitstring<4>>();
      break;
    case types::row2:
      c.init<fixed_bitstring<12>>();
      break;
    case types::row4:
      c.init<fixed_bitstring<3>>();
      break;
    case types::other:
      c.init<fixed_bitstring<6>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::freq_domain_alloc_c_");
  }
}
csi_rs_res_map_s::freq_domain_alloc_c_::freq_domain_alloc_c_(const csi_rs_res_map_s::freq_domain_alloc_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::row1:
      c.init(other.c.get<fixed_bitstring<4>>());
      break;
    case types::row2:
      c.init(other.c.get<fixed_bitstring<12>>());
      break;
    case types::row4:
      c.init(other.c.get<fixed_bitstring<3>>());
      break;
    case types::other:
      c.init(other.c.get<fixed_bitstring<6>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::freq_domain_alloc_c_");
  }
}
csi_rs_res_map_s::freq_domain_alloc_c_&
csi_rs_res_map_s::freq_domain_alloc_c_::operator=(const csi_rs_res_map_s::freq_domain_alloc_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::row1:
      c.set(other.c.get<fixed_bitstring<4>>());
      break;
    case types::row2:
      c.set(other.c.get<fixed_bitstring<12>>());
      break;
    case types::row4:
      c.set(other.c.get<fixed_bitstring<3>>());
      break;
    case types::other:
      c.set(other.c.get<fixed_bitstring<6>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::freq_domain_alloc_c_");
  }

  return *this;
}
fixed_bitstring<4>& csi_rs_res_map_s::freq_domain_alloc_c_::set_row1()
{
  set(types::row1);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<12>& csi_rs_res_map_s::freq_domain_alloc_c_::set_row2()
{
  set(types::row2);
  return c.get<fixed_bitstring<12>>();
}
fixed_bitstring<3>& csi_rs_res_map_s::freq_domain_alloc_c_::set_row4()
{
  set(types::row4);
  return c.get<fixed_bitstring<3>>();
}
fixed_bitstring<6>& csi_rs_res_map_s::freq_domain_alloc_c_::set_other()
{
  set(types::other);
  return c.get<fixed_bitstring<6>>();
}
void csi_rs_res_map_s::freq_domain_alloc_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::row1:
      j.write_str("row1", c.get<fixed_bitstring<4>>().to_string());
      break;
    case types::row2:
      j.write_str("row2", c.get<fixed_bitstring<12>>().to_string());
      break;
    case types::row4:
      j.write_str("row4", c.get<fixed_bitstring<3>>().to_string());
      break;
    case types::other:
      j.write_str("other", c.get<fixed_bitstring<6>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::freq_domain_alloc_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_res_map_s::freq_domain_alloc_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::row1:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().pack(bref));
      break;
    case types::row2:
      HANDLE_CODE(c.get<fixed_bitstring<12>>().pack(bref));
      break;
    case types::row4:
      HANDLE_CODE(c.get<fixed_bitstring<3>>().pack(bref));
      break;
    case types::other:
      HANDLE_CODE(c.get<fixed_bitstring<6>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::freq_domain_alloc_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_map_s::freq_domain_alloc_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::row1:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().unpack(bref));
      break;
    case types::row2:
      HANDLE_CODE(c.get<fixed_bitstring<12>>().unpack(bref));
      break;
    case types::row4:
      HANDLE_CODE(c.get<fixed_bitstring<3>>().unpack(bref));
      break;
    case types::other:
      HANDLE_CODE(c.get<fixed_bitstring<6>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::freq_domain_alloc_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_rs_res_map_s::nrof_ports_opts::to_string() const
{
  static const char* names[] = {"p1", "p2", "p4", "p8", "p12", "p16", "p24", "p32"};
  return convert_enum_idx(names, 8, value, "csi_rs_res_map_s::nrof_ports_e_");
}
uint8_t csi_rs_res_map_s::nrof_ports_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 12, 16, 24, 32};
  return map_enum_number(numbers, 8, value, "csi_rs_res_map_s::nrof_ports_e_");
}

const char* csi_rs_res_map_s::cdm_type_opts::to_string() const
{
  static const char* names[] = {"noCDM", "fd-CDM2", "cdm4-FD2-TD2", "cdm8-FD2-TD4"};
  return convert_enum_idx(names, 4, value, "csi_rs_res_map_s::cdm_type_e_");
}
uint8_t csi_rs_res_map_s::cdm_type_opts::to_number() const
{
  switch (value) {
    case fd_cdm2:
      return 2;
    case cdm4_fd2_td2:
      return 4;
    case cdm8_fd2_td4:
      return 8;
    default:
      invalid_enum_number(value, "csi_rs_res_map_s::cdm_type_e_");
  }
  return 0;
}

void csi_rs_res_map_s::density_c_::set(types::options e)
{
  type_ = e;
}
csi_rs_res_map_s::density_c_::dot5_e_& csi_rs_res_map_s::density_c_::set_dot5()
{
  set(types::dot5);
  return c;
}
void csi_rs_res_map_s::density_c_::set_one()
{
  set(types::one);
}
void csi_rs_res_map_s::density_c_::set_three()
{
  set(types::three);
}
void csi_rs_res_map_s::density_c_::set_spare()
{
  set(types::spare);
}
void csi_rs_res_map_s::density_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dot5:
      j.write_str("dot5", c.to_string());
      break;
    case types::one:
      break;
    case types::three:
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::density_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_rs_res_map_s::density_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dot5:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::one:
      break;
    case types::three:
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::density_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_rs_res_map_s::density_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dot5:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::one:
      break;
    case types::three:
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "csi_rs_res_map_s::density_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_rs_res_map_s::density_c_::dot5_opts::to_string() const
{
  static const char* names[] = {"evenPRBs", "oddPRBs"};
  return convert_enum_idx(names, 2, value, "csi_rs_res_map_s::density_c_::dot5_e_");
}

// CSI-ResourcePeriodicityAndOffset ::= CHOICE
void csi_res_periodicity_and_offset_c::destroy_() {}
void csi_res_periodicity_and_offset_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
csi_res_periodicity_and_offset_c::csi_res_periodicity_and_offset_c(const csi_res_periodicity_and_offset_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slots4:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots5:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots8:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::slots640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_periodicity_and_offset_c");
  }
}
csi_res_periodicity_and_offset_c&
csi_res_periodicity_and_offset_c::operator=(const csi_res_periodicity_and_offset_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slots4:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots5:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots8:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::slots640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_periodicity_and_offset_c");
  }

  return *this;
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots4()
{
  set(types::slots4);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots5()
{
  set(types::slots5);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots8()
{
  set(types::slots8);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots10()
{
  set(types::slots10);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots16()
{
  set(types::slots16);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots20()
{
  set(types::slots20);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots32()
{
  set(types::slots32);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots40()
{
  set(types::slots40);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots64()
{
  set(types::slots64);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots80()
{
  set(types::slots80);
  return c.get<uint8_t>();
}
uint8_t& csi_res_periodicity_and_offset_c::set_slots160()
{
  set(types::slots160);
  return c.get<uint8_t>();
}
uint16_t& csi_res_periodicity_and_offset_c::set_slots320()
{
  set(types::slots320);
  return c.get<uint16_t>();
}
uint16_t& csi_res_periodicity_and_offset_c::set_slots640()
{
  set(types::slots640);
  return c.get<uint16_t>();
}
void csi_res_periodicity_and_offset_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slots4:
      j.write_int("slots4", c.get<uint8_t>());
      break;
    case types::slots5:
      j.write_int("slots5", c.get<uint8_t>());
      break;
    case types::slots8:
      j.write_int("slots8", c.get<uint8_t>());
      break;
    case types::slots10:
      j.write_int("slots10", c.get<uint8_t>());
      break;
    case types::slots16:
      j.write_int("slots16", c.get<uint8_t>());
      break;
    case types::slots20:
      j.write_int("slots20", c.get<uint8_t>());
      break;
    case types::slots32:
      j.write_int("slots32", c.get<uint8_t>());
      break;
    case types::slots40:
      j.write_int("slots40", c.get<uint8_t>());
      break;
    case types::slots64:
      j.write_int("slots64", c.get<uint8_t>());
      break;
    case types::slots80:
      j.write_int("slots80", c.get<uint8_t>());
      break;
    case types::slots160:
      j.write_int("slots160", c.get<uint8_t>());
      break;
    case types::slots320:
      j.write_int("slots320", c.get<uint16_t>());
      break;
    case types::slots640:
      j.write_int("slots640", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_periodicity_and_offset_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_res_periodicity_and_offset_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slots4:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::slots5:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::slots8:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::slots10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::slots16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::slots20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::slots32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::slots40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::slots64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::slots80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::slots160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::slots320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::slots640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_periodicity_and_offset_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_res_periodicity_and_offset_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slots4:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::slots5:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::slots8:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::slots10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::slots16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::slots20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::slots32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::slots40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::slots64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::slots80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::slots160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::slots320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::slots640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_periodicity_and_offset_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// INT-ConfigurationPerServingCell ::= SEQUENCE
SRSASN_CODE int_cfg_per_serving_cell_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serving_cell_id, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, position_in_dci, (uint8_t)0u, (uint8_t)125u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE int_cfg_per_serving_cell_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serving_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(position_in_dci, bref, (uint8_t)0u, (uint8_t)125u));

  return SRSASN_SUCCESS;
}
void int_cfg_per_serving_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servingCellId", serving_cell_id);
  j.write_int("positionInDCI", position_in_dci);
  j.end_obj();
}

// PTRS-DownlinkConfig ::= SEQUENCE
SRSASN_CODE ptrs_dl_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_density_present, 1));
  HANDLE_CODE(bref.pack(time_density_present, 1));
  HANDLE_CODE(bref.pack(epre_ratio_present, 1));
  HANDLE_CODE(bref.pack(res_elem_offset_present, 1));

  if (freq_density_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, &(freq_density)[0], freq_density.size(), integer_packer<uint16_t>(1, 276)));
  }
  if (time_density_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, &(time_density)[0], time_density.size(), integer_packer<uint8_t>(0, 29)));
  }
  if (epre_ratio_present) {
    HANDLE_CODE(pack_integer(bref, epre_ratio, (uint8_t)0u, (uint8_t)3u));
  }
  if (res_elem_offset_present) {
    HANDLE_CODE(res_elem_offset.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ptrs_dl_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_density_present, 1));
  HANDLE_CODE(bref.unpack(time_density_present, 1));
  HANDLE_CODE(bref.unpack(epre_ratio_present, 1));
  HANDLE_CODE(bref.unpack(res_elem_offset_present, 1));

  if (freq_density_present) {
    HANDLE_CODE(unpack_fixed_seq_of(&(freq_density)[0], bref, freq_density.size(), integer_packer<uint16_t>(1, 276)));
  }
  if (time_density_present) {
    HANDLE_CODE(unpack_fixed_seq_of(&(time_density)[0], bref, time_density.size(), integer_packer<uint8_t>(0, 29)));
  }
  if (epre_ratio_present) {
    HANDLE_CODE(unpack_integer(epre_ratio, bref, (uint8_t)0u, (uint8_t)3u));
  }
  if (res_elem_offset_present) {
    HANDLE_CODE(res_elem_offset.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ptrs_dl_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_density_present) {
    j.start_array("frequencyDensity");
    for (const auto& e1 : freq_density) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (time_density_present) {
    j.start_array("timeDensity");
    for (const auto& e1 : time_density) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (epre_ratio_present) {
    j.write_int("epre-Ratio", epre_ratio);
  }
  if (res_elem_offset_present) {
    j.write_str("resourceElementOffset", res_elem_offset.to_string());
  }
  j.end_obj();
}

const char* ptrs_dl_cfg_s::res_elem_offset_opts::to_string() const
{
  static const char* names[] = {"offset01", "offset10", "offset11"};
  return convert_enum_idx(names, 3, value, "ptrs_dl_cfg_s::res_elem_offset_e_");
}
float ptrs_dl_cfg_s::res_elem_offset_opts::to_number() const
{
  static const float numbers[] = {0.1, 1.0, 1.1};
  return map_enum_number(numbers, 3, value, "ptrs_dl_cfg_s::res_elem_offset_e_");
}
const char* ptrs_dl_cfg_s::res_elem_offset_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.1", "1.0", "1.1"};
  return convert_enum_idx(number_strs, 3, value, "ptrs_dl_cfg_s::res_elem_offset_e_");
}

// QCL-Info ::= SEQUENCE
SRSASN_CODE qcl_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_present, 1));
  HANDLE_CODE(bref.pack(bwp_id_present, 1));

  if (cell_present) {
    HANDLE_CODE(pack_integer(bref, cell, (uint8_t)0u, (uint8_t)31u));
  }
  if (bwp_id_present) {
    HANDLE_CODE(pack_integer(bref, bwp_id, (uint8_t)0u, (uint8_t)4u));
  }
  HANDLE_CODE(ref_sig.pack(bref));
  HANDLE_CODE(qcl_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE qcl_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_present, 1));
  HANDLE_CODE(bref.unpack(bwp_id_present, 1));

  if (cell_present) {
    HANDLE_CODE(unpack_integer(cell, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (bwp_id_present) {
    HANDLE_CODE(unpack_integer(bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  }
  HANDLE_CODE(ref_sig.unpack(bref));
  HANDLE_CODE(qcl_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void qcl_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_present) {
    j.write_int("cell", cell);
  }
  if (bwp_id_present) {
    j.write_int("bwp-Id", bwp_id);
  }
  j.write_fieldname("referenceSignal");
  ref_sig.to_json(j);
  j.write_str("qcl-Type", qcl_type.to_string());
  j.end_obj();
}

void qcl_info_s::ref_sig_c_::destroy_() {}
void qcl_info_s::ref_sig_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
qcl_info_s::ref_sig_c_::ref_sig_c_(const qcl_info_s::ref_sig_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::csi_rs:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ssb:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qcl_info_s::ref_sig_c_");
  }
}
qcl_info_s::ref_sig_c_& qcl_info_s::ref_sig_c_::operator=(const qcl_info_s::ref_sig_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::csi_rs:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ssb:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qcl_info_s::ref_sig_c_");
  }

  return *this;
}
uint8_t& qcl_info_s::ref_sig_c_::set_csi_rs()
{
  set(types::csi_rs);
  return c.get<uint8_t>();
}
uint8_t& qcl_info_s::ref_sig_c_::set_ssb()
{
  set(types::ssb);
  return c.get<uint8_t>();
}
void qcl_info_s::ref_sig_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::csi_rs:
      j.write_int("csi-rs", c.get<uint8_t>());
      break;
    case types::ssb:
      j.write_int("ssb", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "qcl_info_s::ref_sig_c_");
  }
  j.end_obj();
}
SRSASN_CODE qcl_info_s::ref_sig_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::csi_rs:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    case types::ssb:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_, "qcl_info_s::ref_sig_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE qcl_info_s::ref_sig_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::csi_rs:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)191u));
      break;
    case types::ssb:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_, "qcl_info_s::ref_sig_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* qcl_info_s::qcl_type_opts::to_string() const
{
  static const char* names[] = {"typeA", "typeB", "typeC", "typeD"};
  return convert_enum_idx(names, 4, value, "qcl_info_s::qcl_type_e_");
}

// DMRS-DownlinkConfig ::= SEQUENCE
SRSASN_CODE dmrs_dl_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dmrs_type_present, 1));
  HANDLE_CODE(bref.pack(dmrs_add_position_present, 1));
  HANDLE_CODE(bref.pack(max_len_present, 1));
  HANDLE_CODE(bref.pack(scrambling_id0_present, 1));
  HANDLE_CODE(bref.pack(scrambling_id1_present, 1));
  HANDLE_CODE(bref.pack(phase_tracking_rs_present, 1));

  if (dmrs_add_position_present) {
    HANDLE_CODE(dmrs_add_position.pack(bref));
  }
  if (scrambling_id0_present) {
    HANDLE_CODE(pack_integer(bref, scrambling_id0, (uint32_t)0u, (uint32_t)65535u));
  }
  if (scrambling_id1_present) {
    HANDLE_CODE(pack_integer(bref, scrambling_id1, (uint32_t)0u, (uint32_t)65535u));
  }
  if (phase_tracking_rs_present) {
    HANDLE_CODE(phase_tracking_rs.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dmrs_dl_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dmrs_type_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_add_position_present, 1));
  HANDLE_CODE(bref.unpack(max_len_present, 1));
  HANDLE_CODE(bref.unpack(scrambling_id0_present, 1));
  HANDLE_CODE(bref.unpack(scrambling_id1_present, 1));
  HANDLE_CODE(bref.unpack(phase_tracking_rs_present, 1));

  if (dmrs_add_position_present) {
    HANDLE_CODE(dmrs_add_position.unpack(bref));
  }
  if (scrambling_id0_present) {
    HANDLE_CODE(unpack_integer(scrambling_id0, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (scrambling_id1_present) {
    HANDLE_CODE(unpack_integer(scrambling_id1, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (phase_tracking_rs_present) {
    HANDLE_CODE(phase_tracking_rs.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dmrs_dl_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dmrs_type_present) {
    j.write_str("dmrs-Type", "type2");
  }
  if (dmrs_add_position_present) {
    j.write_str("dmrs-AdditionalPosition", dmrs_add_position.to_string());
  }
  if (max_len_present) {
    j.write_str("maxLength", "len2");
  }
  if (scrambling_id0_present) {
    j.write_int("scramblingID0", scrambling_id0);
  }
  if (scrambling_id1_present) {
    j.write_int("scramblingID1", scrambling_id1);
  }
  if (phase_tracking_rs_present) {
    j.write_fieldname("phaseTrackingRS");
    phase_tracking_rs.to_json(j);
  }
  j.end_obj();
}

const char* dmrs_dl_cfg_s::dmrs_add_position_opts::to_string() const
{
  static const char* names[] = {"pos0", "pos1", "pos3"};
  return convert_enum_idx(names, 3, value, "dmrs_dl_cfg_s::dmrs_add_position_e_");
}
uint8_t dmrs_dl_cfg_s::dmrs_add_position_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 3};
  return map_enum_number(numbers, 3, value, "dmrs_dl_cfg_s::dmrs_add_position_e_");
}

// DownlinkPreemption ::= SEQUENCE
SRSASN_CODE dl_preemption_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, int_rnti, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(time_freq_set.pack(bref));
  HANDLE_CODE(pack_integer(bref, dci_payload_size, (uint8_t)0u, (uint8_t)126u));
  HANDLE_CODE(pack_dyn_seq_of(bref, int_cfg_per_serving_cell, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_preemption_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(int_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(time_freq_set.unpack(bref));
  HANDLE_CODE(unpack_integer(dci_payload_size, bref, (uint8_t)0u, (uint8_t)126u));
  HANDLE_CODE(unpack_dyn_seq_of(int_cfg_per_serving_cell, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void dl_preemption_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("int-RNTI", int_rnti);
  j.write_str("timeFrequencySet", time_freq_set.to_string());
  j.write_int("dci-PayloadSize", dci_payload_size);
  j.start_array("int-ConfigurationPerServingCell");
  for (const auto& e1 : int_cfg_per_serving_cell) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* dl_preemption_s::time_freq_set_opts::to_string() const
{
  static const char* names[] = {"set0", "set1"};
  return convert_enum_idx(names, 2, value, "dl_preemption_s::time_freq_set_e_");
}
uint8_t dl_preemption_s::time_freq_set_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "dl_preemption_s::time_freq_set_e_");
}

// PUCCH-TPC-CommandConfig ::= SEQUENCE
SRSASN_CODE pucch_tpc_cmd_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tpc_idx_pcell_present, 1));
  HANDLE_CODE(bref.pack(tpc_idx_pucch_scell_present, 1));

  if (tpc_idx_pcell_present) {
    HANDLE_CODE(pack_integer(bref, tpc_idx_pcell, (uint8_t)1u, (uint8_t)15u));
  }
  if (tpc_idx_pucch_scell_present) {
    HANDLE_CODE(pack_integer(bref, tpc_idx_pucch_scell, (uint8_t)1u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_tpc_cmd_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tpc_idx_pcell_present, 1));
  HANDLE_CODE(bref.unpack(tpc_idx_pucch_scell_present, 1));

  if (tpc_idx_pcell_present) {
    HANDLE_CODE(unpack_integer(tpc_idx_pcell, bref, (uint8_t)1u, (uint8_t)15u));
  }
  if (tpc_idx_pucch_scell_present) {
    HANDLE_CODE(unpack_integer(tpc_idx_pucch_scell, bref, (uint8_t)1u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void pucch_tpc_cmd_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tpc_idx_pcell_present) {
    j.write_int("tpc-IndexPCell", tpc_idx_pcell);
  }
  if (tpc_idx_pucch_scell_present) {
    j.write_int("tpc-IndexPUCCH-SCell", tpc_idx_pucch_scell);
  }
  j.end_obj();
}

// PUSCH-TPC-CommandConfig ::= SEQUENCE
SRSASN_CODE pusch_tpc_cmd_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tpc_idx_present, 1));
  HANDLE_CODE(bref.pack(tpc_idx_sul_present, 1));
  HANDLE_CODE(bref.pack(target_cell_present, 1));

  if (tpc_idx_present) {
    HANDLE_CODE(pack_integer(bref, tpc_idx, (uint8_t)1u, (uint8_t)15u));
  }
  if (tpc_idx_sul_present) {
    HANDLE_CODE(pack_integer(bref, tpc_idx_sul, (uint8_t)1u, (uint8_t)15u));
  }
  if (target_cell_present) {
    HANDLE_CODE(pack_integer(bref, target_cell, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_tpc_cmd_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tpc_idx_present, 1));
  HANDLE_CODE(bref.unpack(tpc_idx_sul_present, 1));
  HANDLE_CODE(bref.unpack(target_cell_present, 1));

  if (tpc_idx_present) {
    HANDLE_CODE(unpack_integer(tpc_idx, bref, (uint8_t)1u, (uint8_t)15u));
  }
  if (tpc_idx_sul_present) {
    HANDLE_CODE(unpack_integer(tpc_idx_sul, bref, (uint8_t)1u, (uint8_t)15u));
  }
  if (target_cell_present) {
    HANDLE_CODE(unpack_integer(target_cell, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void pusch_tpc_cmd_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tpc_idx_present) {
    j.write_int("tpc-Index", tpc_idx);
  }
  if (tpc_idx_sul_present) {
    j.write_int("tpc-IndexSUL", tpc_idx_sul);
  }
  if (target_cell_present) {
    j.write_int("targetCell", target_cell);
  }
  j.end_obj();
}

// RadioLinkMonitoringRS ::= SEQUENCE
SRSASN_CODE radio_link_monitoring_rs_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, radio_link_monitoring_rs_id, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(purpose.pack(bref));
  HANDLE_CODE(detection_res.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE radio_link_monitoring_rs_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(radio_link_monitoring_rs_id, bref, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(purpose.unpack(bref));
  HANDLE_CODE(detection_res.unpack(bref));

  return SRSASN_SUCCESS;
}
void radio_link_monitoring_rs_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("radioLinkMonitoringRS-Id", radio_link_monitoring_rs_id);
  j.write_str("purpose", purpose.to_string());
  j.write_fieldname("detectionResource");
  detection_res.to_json(j);
  j.end_obj();
}

const char* radio_link_monitoring_rs_s::purpose_opts::to_string() const
{
  static const char* names[] = {"beamFailure", "rlf", "both"};
  return convert_enum_idx(names, 3, value, "radio_link_monitoring_rs_s::purpose_e_");
}

void radio_link_monitoring_rs_s::detection_res_c_::destroy_() {}
void radio_link_monitoring_rs_s::detection_res_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
radio_link_monitoring_rs_s::detection_res_c_::detection_res_c_(
    const radio_link_monitoring_rs_s::detection_res_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "radio_link_monitoring_rs_s::detection_res_c_");
  }
}
radio_link_monitoring_rs_s::detection_res_c_&
radio_link_monitoring_rs_s::detection_res_c_::operator=(const radio_link_monitoring_rs_s::detection_res_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "radio_link_monitoring_rs_s::detection_res_c_");
  }

  return *this;
}
uint8_t& radio_link_monitoring_rs_s::detection_res_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& radio_link_monitoring_rs_s::detection_res_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
void radio_link_monitoring_rs_s::detection_res_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_idx:
      j.write_int("ssb-Index", c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      j.write_int("csi-RS-Index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "radio_link_monitoring_rs_s::detection_res_c_");
  }
  j.end_obj();
}
SRSASN_CODE radio_link_monitoring_rs_s::detection_res_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "radio_link_monitoring_rs_s::detection_res_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE radio_link_monitoring_rs_s::detection_res_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "radio_link_monitoring_rs_s::detection_res_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RateMatchPattern ::= SEQUENCE
SRSASN_CODE rate_match_pattern_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(subcarrier_spacing_present, 1));

  HANDLE_CODE(pack_integer(bref, rate_match_pattern_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pattern_type.pack(bref));
  if (subcarrier_spacing_present) {
    HANDLE_CODE(subcarrier_spacing.pack(bref));
  }
  HANDLE_CODE(dummy.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rate_match_pattern_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(subcarrier_spacing_present, 1));

  HANDLE_CODE(unpack_integer(rate_match_pattern_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pattern_type.unpack(bref));
  if (subcarrier_spacing_present) {
    HANDLE_CODE(subcarrier_spacing.unpack(bref));
  }
  HANDLE_CODE(dummy.unpack(bref));

  return SRSASN_SUCCESS;
}
void rate_match_pattern_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rateMatchPatternId", rate_match_pattern_id);
  j.write_fieldname("patternType");
  pattern_type.to_json(j);
  if (subcarrier_spacing_present) {
    j.write_str("subcarrierSpacing", subcarrier_spacing.to_string());
  }
  j.write_str("dummy", dummy.to_string());
  j.end_obj();
}

void rate_match_pattern_s::pattern_type_c_::destroy_()
{
  switch (type_) {
    case types::bitmaps:
      c.destroy<bitmaps_s_>();
      break;
    default:
      break;
  }
}
void rate_match_pattern_s::pattern_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::bitmaps:
      c.init<bitmaps_s_>();
      break;
    case types::coreset:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_");
  }
}
rate_match_pattern_s::pattern_type_c_::pattern_type_c_(const rate_match_pattern_s::pattern_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::bitmaps:
      c.init(other.c.get<bitmaps_s_>());
      break;
    case types::coreset:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_");
  }
}
rate_match_pattern_s::pattern_type_c_&
rate_match_pattern_s::pattern_type_c_::operator=(const rate_match_pattern_s::pattern_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::bitmaps:
      c.set(other.c.get<bitmaps_s_>());
      break;
    case types::coreset:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_");
  }

  return *this;
}
rate_match_pattern_s::pattern_type_c_::bitmaps_s_& rate_match_pattern_s::pattern_type_c_::set_bitmaps()
{
  set(types::bitmaps);
  return c.get<bitmaps_s_>();
}
uint8_t& rate_match_pattern_s::pattern_type_c_::set_coreset()
{
  set(types::coreset);
  return c.get<uint8_t>();
}
void rate_match_pattern_s::pattern_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::bitmaps:
      j.write_fieldname("bitmaps");
      j.start_obj();
      j.write_str("resourceBlocks", c.get<bitmaps_s_>().res_blocks.to_string());
      j.write_fieldname("symbolsInResourceBlock");
      c.get<bitmaps_s_>().symbols_in_res_block.to_json(j);
      if (c.get<bitmaps_s_>().periodicity_and_pattern_present) {
        j.write_fieldname("periodicityAndPattern");
        c.get<bitmaps_s_>().periodicity_and_pattern.to_json(j);
      }
      j.end_obj();
      break;
    case types::coreset:
      j.write_int("controlResourceSet", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE rate_match_pattern_s::pattern_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::bitmaps:
      bref.pack(c.get<bitmaps_s_>().ext, 1);
      HANDLE_CODE(bref.pack(c.get<bitmaps_s_>().periodicity_and_pattern_present, 1));
      HANDLE_CODE(c.get<bitmaps_s_>().res_blocks.pack(bref));
      HANDLE_CODE(c.get<bitmaps_s_>().symbols_in_res_block.pack(bref));
      if (c.get<bitmaps_s_>().periodicity_and_pattern_present) {
        HANDLE_CODE(c.get<bitmaps_s_>().periodicity_and_pattern.pack(bref));
      }
      break;
    case types::coreset:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)11u));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rate_match_pattern_s::pattern_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::bitmaps:
      bref.unpack(c.get<bitmaps_s_>().ext, 1);
      HANDLE_CODE(bref.unpack(c.get<bitmaps_s_>().periodicity_and_pattern_present, 1));
      HANDLE_CODE(c.get<bitmaps_s_>().res_blocks.unpack(bref));
      HANDLE_CODE(c.get<bitmaps_s_>().symbols_in_res_block.unpack(bref));
      if (c.get<bitmaps_s_>().periodicity_and_pattern_present) {
        HANDLE_CODE(c.get<bitmaps_s_>().periodicity_and_pattern.unpack(bref));
      }
      break;
    case types::coreset:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)11u));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::destroy_()
{
  switch (type_) {
    case types::one_slot:
      c.destroy<fixed_bitstring<14>>();
      break;
    case types::two_slots:
      c.destroy<fixed_bitstring<28>>();
      break;
    default:
      break;
  }
}
void rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::one_slot:
      c.init<fixed_bitstring<14>>();
      break;
    case types::two_slots:
      c.init<fixed_bitstring<28>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_");
  }
}
rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::symbols_in_res_block_c_(
    const rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_slot:
      c.init(other.c.get<fixed_bitstring<14>>());
      break;
    case types::two_slots:
      c.init(other.c.get<fixed_bitstring<28>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_");
  }
}
rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_&
rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::operator=(
    const rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_slot:
      c.set(other.c.get<fixed_bitstring<14>>());
      break;
    case types::two_slots:
      c.set(other.c.get<fixed_bitstring<28>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_");
  }

  return *this;
}
fixed_bitstring<14>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::set_one_slot()
{
  set(types::one_slot);
  return c.get<fixed_bitstring<14>>();
}
fixed_bitstring<28>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::set_two_slots()
{
  set(types::two_slots);
  return c.get<fixed_bitstring<28>>();
}
void rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_slot:
      j.write_str("oneSlot", c.get<fixed_bitstring<14>>().to_string());
      break;
    case types::two_slots:
      j.write_str("twoSlots", c.get<fixed_bitstring<28>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_");
  }
  j.end_obj();
}
SRSASN_CODE rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_slot:
      HANDLE_CODE(c.get<fixed_bitstring<14>>().pack(bref));
      break;
    case types::two_slots:
      HANDLE_CODE(c.get<fixed_bitstring<28>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_slot:
      HANDLE_CODE(c.get<fixed_bitstring<14>>().unpack(bref));
      break;
    case types::two_slots:
      HANDLE_CODE(c.get<fixed_bitstring<28>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::destroy_()
{
  switch (type_) {
    case types::n2:
      c.destroy<fixed_bitstring<2>>();
      break;
    case types::n4:
      c.destroy<fixed_bitstring<4>>();
      break;
    case types::n5:
      c.destroy<fixed_bitstring<5>>();
      break;
    case types::n8:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::n10:
      c.destroy<fixed_bitstring<10>>();
      break;
    case types::n20:
      c.destroy<fixed_bitstring<20>>();
      break;
    case types::n40:
      c.destroy<fixed_bitstring<40>>();
      break;
    default:
      break;
  }
}
void rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::n2:
      c.init<fixed_bitstring<2>>();
      break;
    case types::n4:
      c.init<fixed_bitstring<4>>();
      break;
    case types::n5:
      c.init<fixed_bitstring<5>>();
      break;
    case types::n8:
      c.init<fixed_bitstring<8>>();
      break;
    case types::n10:
      c.init<fixed_bitstring<10>>();
      break;
    case types::n20:
      c.init<fixed_bitstring<20>>();
      break;
    case types::n40:
      c.init<fixed_bitstring<40>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_");
  }
}
rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::periodicity_and_pattern_c_(
    const rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n2:
      c.init(other.c.get<fixed_bitstring<2>>());
      break;
    case types::n4:
      c.init(other.c.get<fixed_bitstring<4>>());
      break;
    case types::n5:
      c.init(other.c.get<fixed_bitstring<5>>());
      break;
    case types::n8:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::n10:
      c.init(other.c.get<fixed_bitstring<10>>());
      break;
    case types::n20:
      c.init(other.c.get<fixed_bitstring<20>>());
      break;
    case types::n40:
      c.init(other.c.get<fixed_bitstring<40>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_");
  }
}
rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_&
rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::operator=(
    const rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n2:
      c.set(other.c.get<fixed_bitstring<2>>());
      break;
    case types::n4:
      c.set(other.c.get<fixed_bitstring<4>>());
      break;
    case types::n5:
      c.set(other.c.get<fixed_bitstring<5>>());
      break;
    case types::n8:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::n10:
      c.set(other.c.get<fixed_bitstring<10>>());
      break;
    case types::n20:
      c.set(other.c.get<fixed_bitstring<20>>());
      break;
    case types::n40:
      c.set(other.c.get<fixed_bitstring<40>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_");
  }

  return *this;
}
fixed_bitstring<2>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set_n2()
{
  set(types::n2);
  return c.get<fixed_bitstring<2>>();
}
fixed_bitstring<4>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set_n4()
{
  set(types::n4);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<5>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set_n5()
{
  set(types::n5);
  return c.get<fixed_bitstring<5>>();
}
fixed_bitstring<8>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set_n8()
{
  set(types::n8);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<10>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set_n10()
{
  set(types::n10);
  return c.get<fixed_bitstring<10>>();
}
fixed_bitstring<20>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set_n20()
{
  set(types::n20);
  return c.get<fixed_bitstring<20>>();
}
fixed_bitstring<40>& rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::set_n40()
{
  set(types::n40);
  return c.get<fixed_bitstring<40>>();
}
void rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n2:
      j.write_str("n2", c.get<fixed_bitstring<2>>().to_string());
      break;
    case types::n4:
      j.write_str("n4", c.get<fixed_bitstring<4>>().to_string());
      break;
    case types::n5:
      j.write_str("n5", c.get<fixed_bitstring<5>>().to_string());
      break;
    case types::n8:
      j.write_str("n8", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::n10:
      j.write_str("n10", c.get<fixed_bitstring<10>>().to_string());
      break;
    case types::n20:
      j.write_str("n20", c.get<fixed_bitstring<20>>().to_string());
      break;
    case types::n40:
      j.write_str("n40", c.get<fixed_bitstring<40>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_");
  }
  j.end_obj();
}
SRSASN_CODE rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n2:
      HANDLE_CODE(c.get<fixed_bitstring<2>>().pack(bref));
      break;
    case types::n4:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().pack(bref));
      break;
    case types::n5:
      HANDLE_CODE(c.get<fixed_bitstring<5>>().pack(bref));
      break;
    case types::n8:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::n10:
      HANDLE_CODE(c.get<fixed_bitstring<10>>().pack(bref));
      break;
    case types::n20:
      HANDLE_CODE(c.get<fixed_bitstring<20>>().pack(bref));
      break;
    case types::n40:
      HANDLE_CODE(c.get<fixed_bitstring<40>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n2:
      HANDLE_CODE(c.get<fixed_bitstring<2>>().unpack(bref));
      break;
    case types::n4:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().unpack(bref));
      break;
    case types::n5:
      HANDLE_CODE(c.get<fixed_bitstring<5>>().unpack(bref));
      break;
    case types::n8:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::n10:
      HANDLE_CODE(c.get<fixed_bitstring<10>>().unpack(bref));
      break;
    case types::n20:
      HANDLE_CODE(c.get<fixed_bitstring<20>>().unpack(bref));
      break;
    case types::n40:
      HANDLE_CODE(c.get<fixed_bitstring<40>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rate_match_pattern_s::dummy_opts::to_string() const
{
  static const char* names[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(names, 2, value, "rate_match_pattern_s::dummy_e_");
}

void rate_match_pattern_group_item_c_::destroy_() {}
void rate_match_pattern_group_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rate_match_pattern_group_item_c_::rate_match_pattern_group_item_c_(const rate_match_pattern_group_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_level:
      c.init(other.c.get<uint8_t>());
      break;
    case types::bwp_level:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_group_item_c_");
  }
}
rate_match_pattern_group_item_c_&
rate_match_pattern_group_item_c_::operator=(const rate_match_pattern_group_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_level:
      c.set(other.c.get<uint8_t>());
      break;
    case types::bwp_level:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_group_item_c_");
  }

  return *this;
}
uint8_t& rate_match_pattern_group_item_c_::set_cell_level()
{
  set(types::cell_level);
  return c.get<uint8_t>();
}
uint8_t& rate_match_pattern_group_item_c_::set_bwp_level()
{
  set(types::bwp_level);
  return c.get<uint8_t>();
}
void rate_match_pattern_group_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_level:
      j.write_int("cellLevel", c.get<uint8_t>());
      break;
    case types::bwp_level:
      j.write_int("bwpLevel", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_group_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE rate_match_pattern_group_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_level:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::bwp_level:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_group_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rate_match_pattern_group_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_level:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::bwp_level:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "rate_match_pattern_group_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRS-TPC-CommandConfig ::= SEQUENCE
SRSASN_CODE srs_tpc_cmd_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(start_bit_of_format2_3_present, 1));
  HANDLE_CODE(bref.pack(field_type_format2_3_present, 1));

  if (start_bit_of_format2_3_present) {
    HANDLE_CODE(pack_integer(bref, start_bit_of_format2_3, (uint8_t)1u, (uint8_t)31u));
  }
  if (field_type_format2_3_present) {
    HANDLE_CODE(pack_integer(bref, field_type_format2_3, (uint8_t)0u, (uint8_t)1u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= start_bit_of_format2_3_sul_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(start_bit_of_format2_3_sul_present, 1));
      if (start_bit_of_format2_3_sul_present) {
        HANDLE_CODE(pack_integer(bref, start_bit_of_format2_3_sul, (uint8_t)1u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_tpc_cmd_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(start_bit_of_format2_3_present, 1));
  HANDLE_CODE(bref.unpack(field_type_format2_3_present, 1));

  if (start_bit_of_format2_3_present) {
    HANDLE_CODE(unpack_integer(start_bit_of_format2_3, bref, (uint8_t)1u, (uint8_t)31u));
  }
  if (field_type_format2_3_present) {
    HANDLE_CODE(unpack_integer(field_type_format2_3, bref, (uint8_t)0u, (uint8_t)1u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(start_bit_of_format2_3_sul_present, 1));
      if (start_bit_of_format2_3_sul_present) {
        HANDLE_CODE(unpack_integer(start_bit_of_format2_3_sul, bref, (uint8_t)1u, (uint8_t)31u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srs_tpc_cmd_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (start_bit_of_format2_3_present) {
    j.write_int("startingBitOfFormat2-3", start_bit_of_format2_3);
  }
  if (field_type_format2_3_present) {
    j.write_int("fieldTypeFormat2-3", field_type_format2_3);
  }
  if (ext) {
    if (start_bit_of_format2_3_sul_present) {
      j.write_int("startingBitOfFormat2-3SUL", start_bit_of_format2_3_sul);
    }
  }
  j.end_obj();
}

// TCI-State ::= SEQUENCE
SRSASN_CODE tci_state_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(qcl_type2_present, 1));

  HANDLE_CODE(pack_integer(bref, tci_state_id, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(qcl_type1.pack(bref));
  if (qcl_type2_present) {
    HANDLE_CODE(qcl_type2.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tci_state_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(qcl_type2_present, 1));

  HANDLE_CODE(unpack_integer(tci_state_id, bref, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(qcl_type1.unpack(bref));
  if (qcl_type2_present) {
    HANDLE_CODE(qcl_type2.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tci_state_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("tci-StateId", tci_state_id);
  j.write_fieldname("qcl-Type1");
  qcl_type1.to_json(j);
  if (qcl_type2_present) {
    j.write_fieldname("qcl-Type2");
    qcl_type2.to_json(j);
  }
  j.end_obj();
}

// ZP-CSI-RS-Resource ::= SEQUENCE
SRSASN_CODE zp_csi_rs_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(periodicity_and_offset_present, 1));

  HANDLE_CODE(pack_integer(bref, zp_csi_rs_res_id, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(res_map.pack(bref));
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE zp_csi_rs_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(periodicity_and_offset_present, 1));

  HANDLE_CODE(unpack_integer(zp_csi_rs_res_id, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(res_map.unpack(bref));
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void zp_csi_rs_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("zp-CSI-RS-ResourceId", zp_csi_rs_res_id);
  j.write_fieldname("resourceMapping");
  res_map.to_json(j);
  if (periodicity_and_offset_present) {
    j.write_fieldname("periodicityAndOffset");
    periodicity_and_offset.to_json(j);
  }
  j.end_obj();
}

// ZP-CSI-RS-ResourceSet ::= SEQUENCE
SRSASN_CODE zp_csi_rs_res_set_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, zp_csi_rs_res_set_id, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_dyn_seq_of(bref, zp_csi_rs_res_id_list, 1, 16, integer_packer<uint8_t>(0, 31)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE zp_csi_rs_res_set_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(zp_csi_rs_res_set_id, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_dyn_seq_of(zp_csi_rs_res_id_list, bref, 1, 16, integer_packer<uint8_t>(0, 31)));

  return SRSASN_SUCCESS;
}
void zp_csi_rs_res_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("zp-CSI-RS-ResourceSetId", zp_csi_rs_res_set_id);
  j.start_array("zp-CSI-RS-ResourceIdList");
  for (const auto& e1 : zp_csi_rs_res_id_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// PDCCH-Config ::= SEQUENCE
SRSASN_CODE pdcch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(coreset_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(coreset_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(search_spaces_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(search_spaces_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(dl_preemption_present, 1));
  HANDLE_CODE(bref.pack(tpc_pusch_present, 1));
  HANDLE_CODE(bref.pack(tpc_pucch_present, 1));
  HANDLE_CODE(bref.pack(tpc_srs_present, 1));

  if (coreset_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, coreset_to_add_mod_list, 1, 3));
  }
  if (coreset_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, coreset_to_release_list, 1, 3, integer_packer<uint8_t>(0, 11)));
  }
  if (search_spaces_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, search_spaces_to_add_mod_list, 1, 10));
  }
  if (search_spaces_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, search_spaces_to_release_list, 1, 10, integer_packer<uint8_t>(0, 39)));
  }
  if (dl_preemption_present) {
    HANDLE_CODE(dl_preemption.pack(bref));
  }
  if (tpc_pusch_present) {
    HANDLE_CODE(tpc_pusch.pack(bref));
  }
  if (tpc_pucch_present) {
    HANDLE_CODE(tpc_pucch.pack(bref));
  }
  if (tpc_srs_present) {
    HANDLE_CODE(tpc_srs.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool coreset_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(coreset_to_add_mod_list_present, 1));
  bool coreset_to_release_list_present;
  HANDLE_CODE(bref.unpack(coreset_to_release_list_present, 1));
  bool search_spaces_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(search_spaces_to_add_mod_list_present, 1));
  bool search_spaces_to_release_list_present;
  HANDLE_CODE(bref.unpack(search_spaces_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(dl_preemption_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pusch_present, 1));
  HANDLE_CODE(bref.unpack(tpc_pucch_present, 1));
  HANDLE_CODE(bref.unpack(tpc_srs_present, 1));

  if (coreset_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(coreset_to_add_mod_list, bref, 1, 3));
  }
  if (coreset_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(coreset_to_release_list, bref, 1, 3, integer_packer<uint8_t>(0, 11)));
  }
  if (search_spaces_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(search_spaces_to_add_mod_list, bref, 1, 10));
  }
  if (search_spaces_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(search_spaces_to_release_list, bref, 1, 10, integer_packer<uint8_t>(0, 39)));
  }
  if (dl_preemption_present) {
    HANDLE_CODE(dl_preemption.unpack(bref));
  }
  if (tpc_pusch_present) {
    HANDLE_CODE(tpc_pusch.unpack(bref));
  }
  if (tpc_pucch_present) {
    HANDLE_CODE(tpc_pucch.unpack(bref));
  }
  if (tpc_srs_present) {
    HANDLE_CODE(tpc_srs.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (coreset_to_add_mod_list.size() > 0) {
    j.start_array("controlResourceSetToAddModList");
    for (const auto& e1 : coreset_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (coreset_to_release_list.size() > 0) {
    j.start_array("controlResourceSetToReleaseList");
    for (const auto& e1 : coreset_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (search_spaces_to_add_mod_list.size() > 0) {
    j.start_array("searchSpacesToAddModList");
    for (const auto& e1 : search_spaces_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (search_spaces_to_release_list.size() > 0) {
    j.start_array("searchSpacesToReleaseList");
    for (const auto& e1 : search_spaces_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (dl_preemption_present) {
    j.write_fieldname("downlinkPreemption");
    dl_preemption.to_json(j);
  }
  if (tpc_pusch_present) {
    j.write_fieldname("tpc-PUSCH");
    tpc_pusch.to_json(j);
  }
  if (tpc_pucch_present) {
    j.write_fieldname("tpc-PUCCH");
    tpc_pucch.to_json(j);
  }
  if (tpc_srs_present) {
    j.write_fieldname("tpc-SRS");
    tpc_srs.to_json(j);
  }
  j.end_obj();
}

// PDSCH-Config ::= SEQUENCE
SRSASN_CODE pdsch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_scrambling_id_pdsch_present, 1));
  HANDLE_CODE(bref.pack(dmrs_dl_for_pdsch_map_type_a_present, 1));
  HANDLE_CODE(bref.pack(dmrs_dl_for_pdsch_map_type_b_present, 1));
  HANDLE_CODE(bref.pack(tci_states_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(tci_states_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(vrb_to_prb_interleaver_present, 1));
  HANDLE_CODE(bref.pack(pdsch_time_domain_alloc_list_present, 1));
  HANDLE_CODE(bref.pack(pdsch_aggregation_factor_present, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_group1.size() > 0, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_group2.size() > 0, 1));
  HANDLE_CODE(bref.pack(mcs_table_present, 1));
  HANDLE_CODE(bref.pack(max_nrof_code_words_sched_by_dci_present, 1));
  HANDLE_CODE(bref.pack(zp_csi_rs_res_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(zp_csi_rs_res_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(aperiodic_zp_csi_rs_res_sets_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(aperiodic_zp_csi_rs_res_sets_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(sp_zp_csi_rs_res_sets_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(sp_zp_csi_rs_res_sets_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(p_zp_csi_rs_res_set_present, 1));

  if (data_scrambling_id_pdsch_present) {
    HANDLE_CODE(pack_integer(bref, data_scrambling_id_pdsch, (uint16_t)0u, (uint16_t)1023u));
  }
  if (dmrs_dl_for_pdsch_map_type_a_present) {
    HANDLE_CODE(dmrs_dl_for_pdsch_map_type_a.pack(bref));
  }
  if (dmrs_dl_for_pdsch_map_type_b_present) {
    HANDLE_CODE(dmrs_dl_for_pdsch_map_type_b.pack(bref));
  }
  if (tci_states_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tci_states_to_add_mod_list, 1, 128));
  }
  if (tci_states_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, tci_states_to_release_list, 1, 128, integer_packer<uint8_t>(0, 127)));
  }
  if (vrb_to_prb_interleaver_present) {
    HANDLE_CODE(vrb_to_prb_interleaver.pack(bref));
  }
  HANDLE_CODE(res_alloc.pack(bref));
  if (pdsch_time_domain_alloc_list_present) {
    HANDLE_CODE(pdsch_time_domain_alloc_list.pack(bref));
  }
  if (pdsch_aggregation_factor_present) {
    HANDLE_CODE(pdsch_aggregation_factor.pack(bref));
  }
  if (rate_match_pattern_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_to_add_mod_list, 1, 4));
  }
  if (rate_match_pattern_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_to_release_list, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (rate_match_pattern_group1.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_group1, 1, 8));
  }
  if (rate_match_pattern_group2.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_group2, 1, 8));
  }
  HANDLE_CODE(rbg_size.pack(bref));
  if (mcs_table_present) {
    HANDLE_CODE(mcs_table.pack(bref));
  }
  if (max_nrof_code_words_sched_by_dci_present) {
    HANDLE_CODE(max_nrof_code_words_sched_by_dci.pack(bref));
  }
  HANDLE_CODE(prb_bundling_type.pack(bref));
  if (zp_csi_rs_res_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, zp_csi_rs_res_to_add_mod_list, 1, 32));
  }
  if (zp_csi_rs_res_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, zp_csi_rs_res_to_release_list, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (aperiodic_zp_csi_rs_res_sets_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, aperiodic_zp_csi_rs_res_sets_to_add_mod_list, 1, 16));
  }
  if (aperiodic_zp_csi_rs_res_sets_to_release_list.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, aperiodic_zp_csi_rs_res_sets_to_release_list, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (sp_zp_csi_rs_res_sets_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sp_zp_csi_rs_res_sets_to_add_mod_list, 1, 16));
  }
  if (sp_zp_csi_rs_res_sets_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sp_zp_csi_rs_res_sets_to_release_list, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (p_zp_csi_rs_res_set_present) {
    HANDLE_CODE(p_zp_csi_rs_res_set.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_scrambling_id_pdsch_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_dl_for_pdsch_map_type_a_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_dl_for_pdsch_map_type_b_present, 1));
  bool tci_states_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(tci_states_to_add_mod_list_present, 1));
  bool tci_states_to_release_list_present;
  HANDLE_CODE(bref.unpack(tci_states_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(vrb_to_prb_interleaver_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_time_domain_alloc_list_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_aggregation_factor_present, 1));
  bool rate_match_pattern_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_to_add_mod_list_present, 1));
  bool rate_match_pattern_to_release_list_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_to_release_list_present, 1));
  bool rate_match_pattern_group1_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_group1_present, 1));
  bool rate_match_pattern_group2_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_group2_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_present, 1));
  HANDLE_CODE(bref.unpack(max_nrof_code_words_sched_by_dci_present, 1));
  bool zp_csi_rs_res_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(zp_csi_rs_res_to_add_mod_list_present, 1));
  bool zp_csi_rs_res_to_release_list_present;
  HANDLE_CODE(bref.unpack(zp_csi_rs_res_to_release_list_present, 1));
  bool aperiodic_zp_csi_rs_res_sets_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(aperiodic_zp_csi_rs_res_sets_to_add_mod_list_present, 1));
  bool aperiodic_zp_csi_rs_res_sets_to_release_list_present;
  HANDLE_CODE(bref.unpack(aperiodic_zp_csi_rs_res_sets_to_release_list_present, 1));
  bool sp_zp_csi_rs_res_sets_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(sp_zp_csi_rs_res_sets_to_add_mod_list_present, 1));
  bool sp_zp_csi_rs_res_sets_to_release_list_present;
  HANDLE_CODE(bref.unpack(sp_zp_csi_rs_res_sets_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(p_zp_csi_rs_res_set_present, 1));

  if (data_scrambling_id_pdsch_present) {
    HANDLE_CODE(unpack_integer(data_scrambling_id_pdsch, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (dmrs_dl_for_pdsch_map_type_a_present) {
    HANDLE_CODE(dmrs_dl_for_pdsch_map_type_a.unpack(bref));
  }
  if (dmrs_dl_for_pdsch_map_type_b_present) {
    HANDLE_CODE(dmrs_dl_for_pdsch_map_type_b.unpack(bref));
  }
  if (tci_states_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tci_states_to_add_mod_list, bref, 1, 128));
  }
  if (tci_states_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(tci_states_to_release_list, bref, 1, 128, integer_packer<uint8_t>(0, 127)));
  }
  if (vrb_to_prb_interleaver_present) {
    HANDLE_CODE(vrb_to_prb_interleaver.unpack(bref));
  }
  HANDLE_CODE(res_alloc.unpack(bref));
  if (pdsch_time_domain_alloc_list_present) {
    HANDLE_CODE(pdsch_time_domain_alloc_list.unpack(bref));
  }
  if (pdsch_aggregation_factor_present) {
    HANDLE_CODE(pdsch_aggregation_factor.unpack(bref));
  }
  if (rate_match_pattern_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_to_add_mod_list, bref, 1, 4));
  }
  if (rate_match_pattern_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (rate_match_pattern_group1_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_group1, bref, 1, 8));
  }
  if (rate_match_pattern_group2_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_group2, bref, 1, 8));
  }
  HANDLE_CODE(rbg_size.unpack(bref));
  if (mcs_table_present) {
    HANDLE_CODE(mcs_table.unpack(bref));
  }
  if (max_nrof_code_words_sched_by_dci_present) {
    HANDLE_CODE(max_nrof_code_words_sched_by_dci.unpack(bref));
  }
  HANDLE_CODE(prb_bundling_type.unpack(bref));
  if (zp_csi_rs_res_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(zp_csi_rs_res_to_add_mod_list, bref, 1, 32));
  }
  if (zp_csi_rs_res_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(zp_csi_rs_res_to_release_list, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (aperiodic_zp_csi_rs_res_sets_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(aperiodic_zp_csi_rs_res_sets_to_add_mod_list, bref, 1, 16));
  }
  if (aperiodic_zp_csi_rs_res_sets_to_release_list_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(aperiodic_zp_csi_rs_res_sets_to_release_list, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (sp_zp_csi_rs_res_sets_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sp_zp_csi_rs_res_sets_to_add_mod_list, bref, 1, 16));
  }
  if (sp_zp_csi_rs_res_sets_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sp_zp_csi_rs_res_sets_to_release_list, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (p_zp_csi_rs_res_set_present) {
    HANDLE_CODE(p_zp_csi_rs_res_set.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (data_scrambling_id_pdsch_present) {
    j.write_int("dataScramblingIdentityPDSCH", data_scrambling_id_pdsch);
  }
  if (dmrs_dl_for_pdsch_map_type_a_present) {
    j.write_fieldname("dmrs-DownlinkForPDSCH-MappingTypeA");
    dmrs_dl_for_pdsch_map_type_a.to_json(j);
  }
  if (dmrs_dl_for_pdsch_map_type_b_present) {
    j.write_fieldname("dmrs-DownlinkForPDSCH-MappingTypeB");
    dmrs_dl_for_pdsch_map_type_b.to_json(j);
  }
  if (tci_states_to_add_mod_list.size() > 0) {
    j.start_array("tci-StatesToAddModList");
    for (const auto& e1 : tci_states_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (tci_states_to_release_list.size() > 0) {
    j.start_array("tci-StatesToReleaseList");
    for (const auto& e1 : tci_states_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (vrb_to_prb_interleaver_present) {
    j.write_str("vrb-ToPRB-Interleaver", vrb_to_prb_interleaver.to_string());
  }
  j.write_str("resourceAllocation", res_alloc.to_string());
  if (pdsch_time_domain_alloc_list_present) {
    j.write_fieldname("pdsch-TimeDomainAllocationList");
    pdsch_time_domain_alloc_list.to_json(j);
  }
  if (pdsch_aggregation_factor_present) {
    j.write_str("pdsch-AggregationFactor", pdsch_aggregation_factor.to_string());
  }
  if (rate_match_pattern_to_add_mod_list.size() > 0) {
    j.start_array("rateMatchPatternToAddModList");
    for (const auto& e1 : rate_match_pattern_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rate_match_pattern_to_release_list.size() > 0) {
    j.start_array("rateMatchPatternToReleaseList");
    for (const auto& e1 : rate_match_pattern_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (rate_match_pattern_group1.size() > 0) {
    j.start_array("rateMatchPatternGroup1");
    for (const auto& e1 : rate_match_pattern_group1) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rate_match_pattern_group2.size() > 0) {
    j.start_array("rateMatchPatternGroup2");
    for (const auto& e1 : rate_match_pattern_group2) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("rbg-Size", rbg_size.to_string());
  if (mcs_table_present) {
    j.write_str("mcs-Table", mcs_table.to_string());
  }
  if (max_nrof_code_words_sched_by_dci_present) {
    j.write_str("maxNrofCodeWordsScheduledByDCI", max_nrof_code_words_sched_by_dci.to_string());
  }
  j.write_fieldname("prb-BundlingType");
  prb_bundling_type.to_json(j);
  if (zp_csi_rs_res_to_add_mod_list.size() > 0) {
    j.start_array("zp-CSI-RS-ResourceToAddModList");
    for (const auto& e1 : zp_csi_rs_res_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (zp_csi_rs_res_to_release_list.size() > 0) {
    j.start_array("zp-CSI-RS-ResourceToReleaseList");
    for (const auto& e1 : zp_csi_rs_res_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (aperiodic_zp_csi_rs_res_sets_to_add_mod_list.size() > 0) {
    j.start_array("aperiodic-ZP-CSI-RS-ResourceSetsToAddModList");
    for (const auto& e1 : aperiodic_zp_csi_rs_res_sets_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (aperiodic_zp_csi_rs_res_sets_to_release_list.size() > 0) {
    j.start_array("aperiodic-ZP-CSI-RS-ResourceSetsToReleaseList");
    for (const auto& e1 : aperiodic_zp_csi_rs_res_sets_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sp_zp_csi_rs_res_sets_to_add_mod_list.size() > 0) {
    j.start_array("sp-ZP-CSI-RS-ResourceSetsToAddModList");
    for (const auto& e1 : sp_zp_csi_rs_res_sets_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sp_zp_csi_rs_res_sets_to_release_list.size() > 0) {
    j.start_array("sp-ZP-CSI-RS-ResourceSetsToReleaseList");
    for (const auto& e1 : sp_zp_csi_rs_res_sets_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (p_zp_csi_rs_res_set_present) {
    j.write_fieldname("p-ZP-CSI-RS-ResourceSet");
    p_zp_csi_rs_res_set.to_json(j);
  }
  j.end_obj();
}

const char* pdsch_cfg_s::vrb_to_prb_interleaver_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::vrb_to_prb_interleaver_e_");
}
uint8_t pdsch_cfg_s::vrb_to_prb_interleaver_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_s::vrb_to_prb_interleaver_e_");
}

const char* pdsch_cfg_s::res_alloc_opts::to_string() const
{
  static const char* names[] = {"resourceAllocationType0", "resourceAllocationType1", "dynamicSwitch"};
  return convert_enum_idx(names, 3, value, "pdsch_cfg_s::res_alloc_e_");
}
uint8_t pdsch_cfg_s::res_alloc_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_s::res_alloc_e_");
}

const char* pdsch_cfg_s::pdsch_aggregation_factor_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8"};
  return convert_enum_idx(names, 3, value, "pdsch_cfg_s::pdsch_aggregation_factor_e_");
}
uint8_t pdsch_cfg_s::pdsch_aggregation_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "pdsch_cfg_s::pdsch_aggregation_factor_e_");
}

const char* pdsch_cfg_s::rbg_size_opts::to_string() const
{
  static const char* names[] = {"config1", "config2"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::rbg_size_e_");
}
uint8_t pdsch_cfg_s::rbg_size_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_s::rbg_size_e_");
}

const char* pdsch_cfg_s::mcs_table_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::mcs_table_e_");
}
uint16_t pdsch_cfg_s::mcs_table_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_s::mcs_table_e_");
}

const char* pdsch_cfg_s::max_nrof_code_words_sched_by_dci_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::max_nrof_code_words_sched_by_dci_e_");
}
uint8_t pdsch_cfg_s::max_nrof_code_words_sched_by_dci_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_s::max_nrof_code_words_sched_by_dci_e_");
}

void pdsch_cfg_s::prb_bundling_type_c_::destroy_()
{
  switch (type_) {
    case types::static_bundling:
      c.destroy<static_bundling_s_>();
      break;
    case types::dyn_bundling:
      c.destroy<dyn_bundling_s_>();
      break;
    default:
      break;
  }
}
void pdsch_cfg_s::prb_bundling_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::static_bundling:
      c.init<static_bundling_s_>();
      break;
    case types::dyn_bundling:
      c.init<dyn_bundling_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_cfg_s::prb_bundling_type_c_");
  }
}
pdsch_cfg_s::prb_bundling_type_c_::prb_bundling_type_c_(const pdsch_cfg_s::prb_bundling_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::static_bundling:
      c.init(other.c.get<static_bundling_s_>());
      break;
    case types::dyn_bundling:
      c.init(other.c.get<dyn_bundling_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_cfg_s::prb_bundling_type_c_");
  }
}
pdsch_cfg_s::prb_bundling_type_c_&
pdsch_cfg_s::prb_bundling_type_c_::operator=(const pdsch_cfg_s::prb_bundling_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::static_bundling:
      c.set(other.c.get<static_bundling_s_>());
      break;
    case types::dyn_bundling:
      c.set(other.c.get<dyn_bundling_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_cfg_s::prb_bundling_type_c_");
  }

  return *this;
}
pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_& pdsch_cfg_s::prb_bundling_type_c_::set_static_bundling()
{
  set(types::static_bundling);
  return c.get<static_bundling_s_>();
}
pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_& pdsch_cfg_s::prb_bundling_type_c_::set_dyn_bundling()
{
  set(types::dyn_bundling);
  return c.get<dyn_bundling_s_>();
}
void pdsch_cfg_s::prb_bundling_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::static_bundling:
      j.write_fieldname("staticBundling");
      j.start_obj();
      if (c.get<static_bundling_s_>().bundle_size_present) {
        j.write_str("bundleSize", c.get<static_bundling_s_>().bundle_size.to_string());
      }
      j.end_obj();
      break;
    case types::dyn_bundling:
      j.write_fieldname("dynamicBundling");
      j.start_obj();
      if (c.get<dyn_bundling_s_>().bundle_size_set1_present) {
        j.write_str("bundleSizeSet1", c.get<dyn_bundling_s_>().bundle_size_set1.to_string());
      }
      if (c.get<dyn_bundling_s_>().bundle_size_set2_present) {
        j.write_str("bundleSizeSet2", c.get<dyn_bundling_s_>().bundle_size_set2.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_cfg_s::prb_bundling_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdsch_cfg_s::prb_bundling_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::static_bundling:
      HANDLE_CODE(bref.pack(c.get<static_bundling_s_>().bundle_size_present, 1));
      if (c.get<static_bundling_s_>().bundle_size_present) {
        HANDLE_CODE(c.get<static_bundling_s_>().bundle_size.pack(bref));
      }
      break;
    case types::dyn_bundling:
      HANDLE_CODE(bref.pack(c.get<dyn_bundling_s_>().bundle_size_set1_present, 1));
      HANDLE_CODE(bref.pack(c.get<dyn_bundling_s_>().bundle_size_set2_present, 1));
      if (c.get<dyn_bundling_s_>().bundle_size_set1_present) {
        HANDLE_CODE(c.get<dyn_bundling_s_>().bundle_size_set1.pack(bref));
      }
      if (c.get<dyn_bundling_s_>().bundle_size_set2_present) {
        HANDLE_CODE(c.get<dyn_bundling_s_>().bundle_size_set2.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_cfg_s::prb_bundling_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_s::prb_bundling_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::static_bundling:
      HANDLE_CODE(bref.unpack(c.get<static_bundling_s_>().bundle_size_present, 1));
      if (c.get<static_bundling_s_>().bundle_size_present) {
        HANDLE_CODE(c.get<static_bundling_s_>().bundle_size.unpack(bref));
      }
      break;
    case types::dyn_bundling:
      HANDLE_CODE(bref.unpack(c.get<dyn_bundling_s_>().bundle_size_set1_present, 1));
      HANDLE_CODE(bref.unpack(c.get<dyn_bundling_s_>().bundle_size_set2_present, 1));
      if (c.get<dyn_bundling_s_>().bundle_size_set1_present) {
        HANDLE_CODE(c.get<dyn_bundling_s_>().bundle_size_set1.unpack(bref));
      }
      if (c.get<dyn_bundling_s_>().bundle_size_set2_present) {
        HANDLE_CODE(c.get<dyn_bundling_s_>().bundle_size_set2.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "pdsch_cfg_s::prb_bundling_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::to_string() const
{
  static const char* names[] = {"n4", "wideband"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_e_");
}
uint8_t pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::to_number() const
{
  static const uint8_t numbers[] = {4};
  return map_enum_number(numbers, 1, value, "pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_e_");
}

const char* pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set1_opts::to_string() const
{
  static const char* names[] = {"n4", "wideband", "n2-wideband", "n4-wideband"};
  return convert_enum_idx(names, 4, value, "pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set1_e_");
}

const char* pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set2_opts::to_string() const
{
  static const char* names[] = {"n4", "wideband"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set2_e_");
}
uint8_t pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set2_opts::to_number() const
{
  static const uint8_t numbers[] = {4};
  return map_enum_number(numbers, 1, value, "pdsch_cfg_s::prb_bundling_type_c_::dyn_bundling_s_::bundle_size_set2_e_");
}

// RadioLinkMonitoringConfig ::= SEQUENCE
SRSASN_CODE radio_link_monitoring_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(fail_detection_res_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(fail_detection_res_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(beam_fail_instance_max_count_present, 1));
  HANDLE_CODE(bref.pack(beam_fail_detection_timer_present, 1));

  if (fail_detection_res_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, fail_detection_res_to_add_mod_list, 1, 10));
  }
  if (fail_detection_res_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, fail_detection_res_to_release_list, 1, 10, integer_packer<uint8_t>(0, 9)));
  }
  if (beam_fail_instance_max_count_present) {
    HANDLE_CODE(beam_fail_instance_max_count.pack(bref));
  }
  if (beam_fail_detection_timer_present) {
    HANDLE_CODE(beam_fail_detection_timer.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE radio_link_monitoring_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool fail_detection_res_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(fail_detection_res_to_add_mod_list_present, 1));
  bool fail_detection_res_to_release_list_present;
  HANDLE_CODE(bref.unpack(fail_detection_res_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(beam_fail_instance_max_count_present, 1));
  HANDLE_CODE(bref.unpack(beam_fail_detection_timer_present, 1));

  if (fail_detection_res_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(fail_detection_res_to_add_mod_list, bref, 1, 10));
  }
  if (fail_detection_res_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(fail_detection_res_to_release_list, bref, 1, 10, integer_packer<uint8_t>(0, 9)));
  }
  if (beam_fail_instance_max_count_present) {
    HANDLE_CODE(beam_fail_instance_max_count.unpack(bref));
  }
  if (beam_fail_detection_timer_present) {
    HANDLE_CODE(beam_fail_detection_timer.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void radio_link_monitoring_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_detection_res_to_add_mod_list.size() > 0) {
    j.start_array("failureDetectionResourcesToAddModList");
    for (const auto& e1 : fail_detection_res_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (fail_detection_res_to_release_list.size() > 0) {
    j.start_array("failureDetectionResourcesToReleaseList");
    for (const auto& e1 : fail_detection_res_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (beam_fail_instance_max_count_present) {
    j.write_str("beamFailureInstanceMaxCount", beam_fail_instance_max_count.to_string());
  }
  if (beam_fail_detection_timer_present) {
    j.write_str("beamFailureDetectionTimer", beam_fail_detection_timer.to_string());
  }
  j.end_obj();
}

const char* radio_link_monitoring_cfg_s::beam_fail_instance_max_count_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(names, 8, value, "radio_link_monitoring_cfg_s::beam_fail_instance_max_count_e_");
}
uint8_t radio_link_monitoring_cfg_s::beam_fail_instance_max_count_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(numbers, 8, value, "radio_link_monitoring_cfg_s::beam_fail_instance_max_count_e_");
}

const char* radio_link_monitoring_cfg_s::beam_fail_detection_timer_opts::to_string() const
{
  static const char* names[] = {"pbfd1", "pbfd2", "pbfd3", "pbfd4", "pbfd5", "pbfd6", "pbfd8", "pbfd10"};
  return convert_enum_idx(names, 8, value, "radio_link_monitoring_cfg_s::beam_fail_detection_timer_e_");
}
uint8_t radio_link_monitoring_cfg_s::beam_fail_detection_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(numbers, 8, value, "radio_link_monitoring_cfg_s::beam_fail_detection_timer_e_");
}

// SPS-Config ::= SEQUENCE
SRSASN_CODE sps_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(n1_pucch_an_present, 1));
  HANDLE_CODE(bref.pack(mcs_table_present, 1));

  HANDLE_CODE(periodicity.pack(bref));
  HANDLE_CODE(pack_integer(bref, nrof_harq_processes, (uint8_t)1u, (uint8_t)8u));
  if (n1_pucch_an_present) {
    HANDLE_CODE(pack_integer(bref, n1_pucch_an, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sps_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(n1_pucch_an_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_present, 1));

  HANDLE_CODE(periodicity.unpack(bref));
  HANDLE_CODE(unpack_integer(nrof_harq_processes, bref, (uint8_t)1u, (uint8_t)8u));
  if (n1_pucch_an_present) {
    HANDLE_CODE(unpack_integer(n1_pucch_an, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void sps_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("periodicity", periodicity.to_string());
  j.write_int("nrofHARQ-Processes", nrof_harq_processes);
  if (n1_pucch_an_present) {
    j.write_int("n1PUCCH-AN", n1_pucch_an);
  }
  if (mcs_table_present) {
    j.write_str("mcs-Table", "qam64LowSE");
  }
  j.end_obj();
}

const char* sps_cfg_s::periodicity_opts::to_string() const
{
  static const char* names[] = {"ms10",
                                "ms20",
                                "ms32",
                                "ms40",
                                "ms64",
                                "ms80",
                                "ms128",
                                "ms160",
                                "ms320",
                                "ms640",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sps_cfg_s::periodicity_e_");
}
uint16_t sps_cfg_s::periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 32, 40, 64, 80, 128, 160, 320, 640};
  return map_enum_number(numbers, 10, value, "sps_cfg_s::periodicity_e_");
}

// BWP-DownlinkDedicated ::= SEQUENCE
SRSASN_CODE bwp_dl_ded_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdcch_cfg_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_present, 1));
  HANDLE_CODE(bref.pack(sps_cfg_present, 1));
  HANDLE_CODE(bref.pack(radio_link_monitoring_cfg_present, 1));

  if (pdcch_cfg_present) {
    HANDLE_CODE(pdcch_cfg.pack(bref));
  }
  if (pdsch_cfg_present) {
    HANDLE_CODE(pdsch_cfg.pack(bref));
  }
  if (sps_cfg_present) {
    HANDLE_CODE(sps_cfg.pack(bref));
  }
  if (radio_link_monitoring_cfg_present) {
    HANDLE_CODE(radio_link_monitoring_cfg.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_dl_ded_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdcch_cfg_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_present, 1));
  HANDLE_CODE(bref.unpack(sps_cfg_present, 1));
  HANDLE_CODE(bref.unpack(radio_link_monitoring_cfg_present, 1));

  if (pdcch_cfg_present) {
    HANDLE_CODE(pdcch_cfg.unpack(bref));
  }
  if (pdsch_cfg_present) {
    HANDLE_CODE(pdsch_cfg.unpack(bref));
  }
  if (sps_cfg_present) {
    HANDLE_CODE(sps_cfg.unpack(bref));
  }
  if (radio_link_monitoring_cfg_present) {
    HANDLE_CODE(radio_link_monitoring_cfg.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bwp_dl_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcch_cfg_present) {
    j.write_fieldname("pdcch-Config");
    pdcch_cfg.to_json(j);
  }
  if (pdsch_cfg_present) {
    j.write_fieldname("pdsch-Config");
    pdsch_cfg.to_json(j);
  }
  if (sps_cfg_present) {
    j.write_fieldname("sps-Config");
    sps_cfg.to_json(j);
  }
  if (radio_link_monitoring_cfg_present) {
    j.write_fieldname("radioLinkMonitoringConfig");
    radio_link_monitoring_cfg.to_json(j);
  }
  j.end_obj();
}

// BWP-Downlink ::= SEQUENCE
SRSASN_CODE bwp_dl_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(bwp_common_present, 1));
  HANDLE_CODE(bref.pack(bwp_ded_present, 1));

  HANDLE_CODE(pack_integer(bref, bwp_id, (uint8_t)0u, (uint8_t)4u));
  if (bwp_common_present) {
    HANDLE_CODE(bwp_common.pack(bref));
  }
  if (bwp_ded_present) {
    HANDLE_CODE(bwp_ded.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_dl_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(bwp_common_present, 1));
  HANDLE_CODE(bref.unpack(bwp_ded_present, 1));

  HANDLE_CODE(unpack_integer(bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  if (bwp_common_present) {
    HANDLE_CODE(bwp_common.unpack(bref));
  }
  if (bwp_ded_present) {
    HANDLE_CODE(bwp_ded.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bwp_dl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bwp-Id", bwp_id);
  if (bwp_common_present) {
    j.write_fieldname("bwp-Common");
    bwp_common.to_json(j);
  }
  if (bwp_ded_present) {
    j.write_fieldname("bwp-Dedicated");
    bwp_ded.to_json(j);
  }
  j.end_obj();
}

// Alpha ::= ENUMERATED
const char* alpha_opts::to_string() const
{
  static const char* names[] = {"alpha0", "alpha04", "alpha05", "alpha06", "alpha07", "alpha08", "alpha09", "alpha1"};
  return convert_enum_idx(names, 8, value, "alpha_e");
}
float alpha_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(numbers, 8, value, "alpha_e");
}
const char* alpha_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(number_strs, 8, value, "alpha_e");
}

// BetaOffsets ::= SEQUENCE
SRSASN_CODE beta_offsets_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(beta_offset_ack_idx1_present, 1));
  HANDLE_CODE(bref.pack(beta_offset_ack_idx2_present, 1));
  HANDLE_CODE(bref.pack(beta_offset_ack_idx3_present, 1));
  HANDLE_CODE(bref.pack(beta_offset_csi_part1_idx1_present, 1));
  HANDLE_CODE(bref.pack(beta_offset_csi_part1_idx2_present, 1));
  HANDLE_CODE(bref.pack(beta_offset_csi_part2_idx1_present, 1));
  HANDLE_CODE(bref.pack(beta_offset_csi_part2_idx2_present, 1));

  if (beta_offset_ack_idx1_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_ack_idx1, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_ack_idx2_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_ack_idx2, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_ack_idx3_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_ack_idx3, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part1_idx1_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_csi_part1_idx1, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part1_idx2_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_csi_part1_idx2, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part2_idx1_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_csi_part2_idx1, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part2_idx2_present) {
    HANDLE_CODE(pack_integer(bref, beta_offset_csi_part2_idx2, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE beta_offsets_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(beta_offset_ack_idx1_present, 1));
  HANDLE_CODE(bref.unpack(beta_offset_ack_idx2_present, 1));
  HANDLE_CODE(bref.unpack(beta_offset_ack_idx3_present, 1));
  HANDLE_CODE(bref.unpack(beta_offset_csi_part1_idx1_present, 1));
  HANDLE_CODE(bref.unpack(beta_offset_csi_part1_idx2_present, 1));
  HANDLE_CODE(bref.unpack(beta_offset_csi_part2_idx1_present, 1));
  HANDLE_CODE(bref.unpack(beta_offset_csi_part2_idx2_present, 1));

  if (beta_offset_ack_idx1_present) {
    HANDLE_CODE(unpack_integer(beta_offset_ack_idx1, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_ack_idx2_present) {
    HANDLE_CODE(unpack_integer(beta_offset_ack_idx2, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_ack_idx3_present) {
    HANDLE_CODE(unpack_integer(beta_offset_ack_idx3, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part1_idx1_present) {
    HANDLE_CODE(unpack_integer(beta_offset_csi_part1_idx1, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part1_idx2_present) {
    HANDLE_CODE(unpack_integer(beta_offset_csi_part1_idx2, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part2_idx1_present) {
    HANDLE_CODE(unpack_integer(beta_offset_csi_part2_idx1, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (beta_offset_csi_part2_idx2_present) {
    HANDLE_CODE(unpack_integer(beta_offset_csi_part2_idx2, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void beta_offsets_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (beta_offset_ack_idx1_present) {
    j.write_int("betaOffsetACK-Index1", beta_offset_ack_idx1);
  }
  if (beta_offset_ack_idx2_present) {
    j.write_int("betaOffsetACK-Index2", beta_offset_ack_idx2);
  }
  if (beta_offset_ack_idx3_present) {
    j.write_int("betaOffsetACK-Index3", beta_offset_ack_idx3);
  }
  if (beta_offset_csi_part1_idx1_present) {
    j.write_int("betaOffsetCSI-Part1-Index1", beta_offset_csi_part1_idx1);
  }
  if (beta_offset_csi_part1_idx2_present) {
    j.write_int("betaOffsetCSI-Part1-Index2", beta_offset_csi_part1_idx2);
  }
  if (beta_offset_csi_part2_idx1_present) {
    j.write_int("betaOffsetCSI-Part2-Index1", beta_offset_csi_part2_idx1);
  }
  if (beta_offset_csi_part2_idx2_present) {
    j.write_int("betaOffsetCSI-Part2-Index2", beta_offset_csi_part2_idx2);
  }
  j.end_obj();
}

// P0-PUCCH ::= SEQUENCE
SRSASN_CODE p0_pucch_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p0_pucch_id, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, p0_pucch_value, (int8_t)-16, (int8_t)15));

  return SRSASN_SUCCESS;
}
SRSASN_CODE p0_pucch_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p0_pucch_id, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(p0_pucch_value, bref, (int8_t)-16, (int8_t)15));

  return SRSASN_SUCCESS;
}
void p0_pucch_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-PUCCH-Id", p0_pucch_id);
  j.write_int("p0-PUCCH-Value", p0_pucch_value);
  j.end_obj();
}

// P0-PUSCH-AlphaSet ::= SEQUENCE
SRSASN_CODE p0_pusch_alpha_set_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p0_present, 1));
  HANDLE_CODE(bref.pack(alpha_present, 1));

  HANDLE_CODE(pack_integer(bref, p0_pusch_alpha_set_id, (uint8_t)0u, (uint8_t)29u));
  if (p0_present) {
    HANDLE_CODE(pack_integer(bref, p0, (int8_t)-16, (int8_t)15));
  }
  if (alpha_present) {
    HANDLE_CODE(alpha.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE p0_pusch_alpha_set_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p0_present, 1));
  HANDLE_CODE(bref.unpack(alpha_present, 1));

  HANDLE_CODE(unpack_integer(p0_pusch_alpha_set_id, bref, (uint8_t)0u, (uint8_t)29u));
  if (p0_present) {
    HANDLE_CODE(unpack_integer(p0, bref, (int8_t)-16, (int8_t)15));
  }
  if (alpha_present) {
    HANDLE_CODE(alpha.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void p0_pusch_alpha_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-PUSCH-AlphaSetId", p0_pusch_alpha_set_id);
  if (p0_present) {
    j.write_int("p0", p0);
  }
  if (alpha_present) {
    j.write_str("alpha", alpha.to_string());
  }
  j.end_obj();
}

// PTRS-UplinkConfig ::= SEQUENCE
SRSASN_CODE ptrs_ul_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(transform_precoder_disabled_present, 1));
  HANDLE_CODE(bref.pack(transform_precoder_enabled_present, 1));

  if (transform_precoder_disabled_present) {
    HANDLE_CODE(bref.pack(transform_precoder_disabled.freq_density_present, 1));
    HANDLE_CODE(bref.pack(transform_precoder_disabled.time_density_present, 1));
    HANDLE_CODE(bref.pack(transform_precoder_disabled.res_elem_offset_present, 1));
    if (transform_precoder_disabled.freq_density_present) {
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    &(transform_precoder_disabled.freq_density)[0],
                                    transform_precoder_disabled.freq_density.size(),
                                    integer_packer<uint16_t>(1, 276)));
    }
    if (transform_precoder_disabled.time_density_present) {
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    &(transform_precoder_disabled.time_density)[0],
                                    transform_precoder_disabled.time_density.size(),
                                    integer_packer<uint8_t>(0, 29)));
    }
    HANDLE_CODE(transform_precoder_disabled.max_nrof_ports.pack(bref));
    if (transform_precoder_disabled.res_elem_offset_present) {
      HANDLE_CODE(transform_precoder_disabled.res_elem_offset.pack(bref));
    }
    HANDLE_CODE(transform_precoder_disabled.ptrs_pwr.pack(bref));
  }
  if (transform_precoder_enabled_present) {
    HANDLE_CODE(bref.pack(transform_precoder_enabled.time_density_transform_precoding_present, 1));
    HANDLE_CODE(pack_fixed_seq_of(bref,
                                  &(transform_precoder_enabled.sample_density)[0],
                                  transform_precoder_enabled.sample_density.size(),
                                  integer_packer<uint16_t>(1, 276)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ptrs_ul_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(transform_precoder_disabled_present, 1));
  HANDLE_CODE(bref.unpack(transform_precoder_enabled_present, 1));

  if (transform_precoder_disabled_present) {
    HANDLE_CODE(bref.unpack(transform_precoder_disabled.freq_density_present, 1));
    HANDLE_CODE(bref.unpack(transform_precoder_disabled.time_density_present, 1));
    HANDLE_CODE(bref.unpack(transform_precoder_disabled.res_elem_offset_present, 1));
    if (transform_precoder_disabled.freq_density_present) {
      HANDLE_CODE(unpack_fixed_seq_of(&(transform_precoder_disabled.freq_density)[0],
                                      bref,
                                      transform_precoder_disabled.freq_density.size(),
                                      integer_packer<uint16_t>(1, 276)));
    }
    if (transform_precoder_disabled.time_density_present) {
      HANDLE_CODE(unpack_fixed_seq_of(&(transform_precoder_disabled.time_density)[0],
                                      bref,
                                      transform_precoder_disabled.time_density.size(),
                                      integer_packer<uint8_t>(0, 29)));
    }
    HANDLE_CODE(transform_precoder_disabled.max_nrof_ports.unpack(bref));
    if (transform_precoder_disabled.res_elem_offset_present) {
      HANDLE_CODE(transform_precoder_disabled.res_elem_offset.unpack(bref));
    }
    HANDLE_CODE(transform_precoder_disabled.ptrs_pwr.unpack(bref));
  }
  if (transform_precoder_enabled_present) {
    HANDLE_CODE(bref.unpack(transform_precoder_enabled.time_density_transform_precoding_present, 1));
    HANDLE_CODE(unpack_fixed_seq_of(&(transform_precoder_enabled.sample_density)[0],
                                    bref,
                                    transform_precoder_enabled.sample_density.size(),
                                    integer_packer<uint16_t>(1, 276)));
  }

  return SRSASN_SUCCESS;
}
void ptrs_ul_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (transform_precoder_disabled_present) {
    j.write_fieldname("transformPrecoderDisabled");
    j.start_obj();
    if (transform_precoder_disabled.freq_density_present) {
      j.start_array("frequencyDensity");
      for (const auto& e1 : transform_precoder_disabled.freq_density) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (transform_precoder_disabled.time_density_present) {
      j.start_array("timeDensity");
      for (const auto& e1 : transform_precoder_disabled.time_density) {
        j.write_int(e1);
      }
      j.end_array();
    }
    j.write_str("maxNrofPorts", transform_precoder_disabled.max_nrof_ports.to_string());
    if (transform_precoder_disabled.res_elem_offset_present) {
      j.write_str("resourceElementOffset", transform_precoder_disabled.res_elem_offset.to_string());
    }
    j.write_str("ptrs-Power", transform_precoder_disabled.ptrs_pwr.to_string());
    j.end_obj();
  }
  if (transform_precoder_enabled_present) {
    j.write_fieldname("transformPrecoderEnabled");
    j.start_obj();
    j.start_array("sampleDensity");
    for (const auto& e1 : transform_precoder_enabled.sample_density) {
      j.write_int(e1);
    }
    j.end_array();
    if (transform_precoder_enabled.time_density_transform_precoding_present) {
      j.write_str("timeDensityTransformPrecoding", "d2");
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* ptrs_ul_cfg_s::transform_precoder_disabled_s_::max_nrof_ports_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names, 2, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::max_nrof_ports_e_");
}
uint8_t ptrs_ul_cfg_s::transform_precoder_disabled_s_::max_nrof_ports_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::max_nrof_ports_e_");
}

const char* ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_opts::to_string() const
{
  static const char* names[] = {"offset01", "offset10", "offset11"};
  return convert_enum_idx(names, 3, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_e_");
}
float ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_opts::to_number() const
{
  static const float numbers[] = {0.1, 1.0, 1.1};
  return map_enum_number(numbers, 3, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_e_");
}
const char* ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.1", "1.0", "1.1"};
  return convert_enum_idx(number_strs, 3, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::res_elem_offset_e_");
}

const char* ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_opts::to_string() const
{
  static const char* names[] = {"p00", "p01", "p10", "p11"};
  return convert_enum_idx(names, 4, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_e_");
}
float ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.1, 1.0, 1.1};
  return map_enum_number(numbers, 4, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_e_");
}
const char* ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.0", "0.1", "1.0", "1.1"};
  return convert_enum_idx(number_strs, 4, value, "ptrs_ul_cfg_s::transform_precoder_disabled_s_::ptrs_pwr_e_");
}

// PUCCH-MaxCodeRate ::= ENUMERATED
const char* pucch_max_code_rate_opts::to_string() const
{
  static const char* names[] = {
      "zeroDot08", "zeroDot15", "zeroDot25", "zeroDot35", "zeroDot45", "zeroDot60", "zeroDot80"};
  return convert_enum_idx(names, 7, value, "pucch_max_code_rate_e");
}
float pucch_max_code_rate_opts::to_number() const
{
  static const float numbers[] = {0.08, 0.15, 0.25, 0.35, 0.45, 0.6, 0.8};
  return map_enum_number(numbers, 7, value, "pucch_max_code_rate_e");
}
const char* pucch_max_code_rate_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.08", "0.15", "0.25", "0.35", "0.45", "0.60", "0.80"};
  return convert_enum_idx(number_strs, 7, value, "pucch_max_code_rate_e");
}

// PUCCH-PathlossReferenceRS ::= SEQUENCE
SRSASN_CODE pucch_pathloss_ref_rs_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pucch_pathloss_ref_rs_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(ref_sig.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_pathloss_ref_rs_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pucch_pathloss_ref_rs_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(ref_sig.unpack(bref));

  return SRSASN_SUCCESS;
}
void pucch_pathloss_ref_rs_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pucch-PathlossReferenceRS-Id", pucch_pathloss_ref_rs_id);
  j.write_fieldname("referenceSignal");
  ref_sig.to_json(j);
  j.end_obj();
}

void pucch_pathloss_ref_rs_s::ref_sig_c_::destroy_() {}
void pucch_pathloss_ref_rs_s::ref_sig_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
pucch_pathloss_ref_rs_s::ref_sig_c_::ref_sig_c_(const pucch_pathloss_ref_rs_s::ref_sig_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_pathloss_ref_rs_s::ref_sig_c_");
  }
}
pucch_pathloss_ref_rs_s::ref_sig_c_&
pucch_pathloss_ref_rs_s::ref_sig_c_::operator=(const pucch_pathloss_ref_rs_s::ref_sig_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_pathloss_ref_rs_s::ref_sig_c_");
  }

  return *this;
}
uint8_t& pucch_pathloss_ref_rs_s::ref_sig_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& pucch_pathloss_ref_rs_s::ref_sig_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
void pucch_pathloss_ref_rs_s::ref_sig_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_idx:
      j.write_int("ssb-Index", c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      j.write_int("csi-RS-Index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "pucch_pathloss_ref_rs_s::ref_sig_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_pathloss_ref_rs_s::ref_sig_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_pathloss_ref_rs_s::ref_sig_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_pathloss_ref_rs_s::ref_sig_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_pathloss_ref_rs_s::ref_sig_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUCCH-format0 ::= SEQUENCE
SRSASN_CODE pucch_format0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, init_cyclic_shift, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(pack_integer(bref, nrof_symbols, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(pack_integer(bref, start_symbol_idx, (uint8_t)0u, (uint8_t)13u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(init_cyclic_shift, bref, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(unpack_integer(nrof_symbols, bref, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(unpack_integer(start_symbol_idx, bref, (uint8_t)0u, (uint8_t)13u));

  return SRSASN_SUCCESS;
}
void pucch_format0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("initialCyclicShift", init_cyclic_shift);
  j.write_int("nrofSymbols", nrof_symbols);
  j.write_int("startingSymbolIndex", start_symbol_idx);
  j.end_obj();
}

// PUCCH-format1 ::= SEQUENCE
SRSASN_CODE pucch_format1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, init_cyclic_shift, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(pack_integer(bref, nrof_symbols, (uint8_t)4u, (uint8_t)14u));
  HANDLE_CODE(pack_integer(bref, start_symbol_idx, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(pack_integer(bref, time_domain_occ, (uint8_t)0u, (uint8_t)6u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(init_cyclic_shift, bref, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(unpack_integer(nrof_symbols, bref, (uint8_t)4u, (uint8_t)14u));
  HANDLE_CODE(unpack_integer(start_symbol_idx, bref, (uint8_t)0u, (uint8_t)10u));
  HANDLE_CODE(unpack_integer(time_domain_occ, bref, (uint8_t)0u, (uint8_t)6u));

  return SRSASN_SUCCESS;
}
void pucch_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("initialCyclicShift", init_cyclic_shift);
  j.write_int("nrofSymbols", nrof_symbols);
  j.write_int("startingSymbolIndex", start_symbol_idx);
  j.write_int("timeDomainOCC", time_domain_occ);
  j.end_obj();
}

// PUCCH-format2 ::= SEQUENCE
SRSASN_CODE pucch_format2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, nrof_prbs, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, nrof_symbols, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(pack_integer(bref, start_symbol_idx, (uint8_t)0u, (uint8_t)13u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(nrof_prbs, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(nrof_symbols, bref, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(unpack_integer(start_symbol_idx, bref, (uint8_t)0u, (uint8_t)13u));

  return SRSASN_SUCCESS;
}
void pucch_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nrofPRBs", nrof_prbs);
  j.write_int("nrofSymbols", nrof_symbols);
  j.write_int("startingSymbolIndex", start_symbol_idx);
  j.end_obj();
}

// PUCCH-format3 ::= SEQUENCE
SRSASN_CODE pucch_format3_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, nrof_prbs, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, nrof_symbols, (uint8_t)4u, (uint8_t)14u));
  HANDLE_CODE(pack_integer(bref, start_symbol_idx, (uint8_t)0u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format3_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(nrof_prbs, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(nrof_symbols, bref, (uint8_t)4u, (uint8_t)14u));
  HANDLE_CODE(unpack_integer(start_symbol_idx, bref, (uint8_t)0u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
void pucch_format3_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nrofPRBs", nrof_prbs);
  j.write_int("nrofSymbols", nrof_symbols);
  j.write_int("startingSymbolIndex", start_symbol_idx);
  j.end_obj();
}

// PUCCH-format4 ::= SEQUENCE
SRSASN_CODE pucch_format4_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, nrof_symbols, (uint8_t)4u, (uint8_t)14u));
  HANDLE_CODE(occ_len.pack(bref));
  HANDLE_CODE(occ_idx.pack(bref));
  HANDLE_CODE(pack_integer(bref, start_symbol_idx, (uint8_t)0u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format4_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(nrof_symbols, bref, (uint8_t)4u, (uint8_t)14u));
  HANDLE_CODE(occ_len.unpack(bref));
  HANDLE_CODE(occ_idx.unpack(bref));
  HANDLE_CODE(unpack_integer(start_symbol_idx, bref, (uint8_t)0u, (uint8_t)10u));

  return SRSASN_SUCCESS;
}
void pucch_format4_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nrofSymbols", nrof_symbols);
  j.write_str("occ-Length", occ_len.to_string());
  j.write_str("occ-Index", occ_idx.to_string());
  j.write_int("startingSymbolIndex", start_symbol_idx);
  j.end_obj();
}

const char* pucch_format4_s::occ_len_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(names, 2, value, "pucch_format4_s::occ_len_e_");
}
uint8_t pucch_format4_s::occ_len_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "pucch_format4_s::occ_len_e_");
}

const char* pucch_format4_s::occ_idx_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3"};
  return convert_enum_idx(names, 4, value, "pucch_format4_s::occ_idx_e_");
}
uint8_t pucch_format4_s::occ_idx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "pucch_format4_s::occ_idx_e_");
}

// PUSCH-PathlossReferenceRS ::= SEQUENCE
SRSASN_CODE pusch_pathloss_ref_rs_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pusch_pathloss_ref_rs_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(ref_sig.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_pathloss_ref_rs_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pusch_pathloss_ref_rs_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(ref_sig.unpack(bref));

  return SRSASN_SUCCESS;
}
void pusch_pathloss_ref_rs_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pusch-PathlossReferenceRS-Id", pusch_pathloss_ref_rs_id);
  j.write_fieldname("referenceSignal");
  ref_sig.to_json(j);
  j.end_obj();
}

void pusch_pathloss_ref_rs_s::ref_sig_c_::destroy_() {}
void pusch_pathloss_ref_rs_s::ref_sig_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
pusch_pathloss_ref_rs_s::ref_sig_c_::ref_sig_c_(const pusch_pathloss_ref_rs_s::ref_sig_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pusch_pathloss_ref_rs_s::ref_sig_c_");
  }
}
pusch_pathloss_ref_rs_s::ref_sig_c_&
pusch_pathloss_ref_rs_s::ref_sig_c_::operator=(const pusch_pathloss_ref_rs_s::ref_sig_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pusch_pathloss_ref_rs_s::ref_sig_c_");
  }

  return *this;
}
uint8_t& pusch_pathloss_ref_rs_s::ref_sig_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& pusch_pathloss_ref_rs_s::ref_sig_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
void pusch_pathloss_ref_rs_s::ref_sig_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_idx:
      j.write_int("ssb-Index", c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      j.write_int("csi-RS-Index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "pusch_pathloss_ref_rs_s::ref_sig_c_");
  }
  j.end_obj();
}
SRSASN_CODE pusch_pathloss_ref_rs_s::ref_sig_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_pathloss_ref_rs_s::ref_sig_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_pathloss_ref_rs_s::ref_sig_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "pusch_pathloss_ref_rs_s::ref_sig_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRI-PUSCH-PowerControl ::= SEQUENCE
SRSASN_CODE sri_pusch_pwr_ctrl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sri_pusch_pwr_ctrl_id, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, sri_pusch_pathloss_ref_rs_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, sri_p0_pusch_alpha_set_id, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(sri_pusch_closed_loop_idx.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sri_pusch_pwr_ctrl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sri_pusch_pwr_ctrl_id, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(sri_pusch_pathloss_ref_rs_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(sri_p0_pusch_alpha_set_id, bref, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(sri_pusch_closed_loop_idx.unpack(bref));

  return SRSASN_SUCCESS;
}
void sri_pusch_pwr_ctrl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sri-PUSCH-PowerControlId", sri_pusch_pwr_ctrl_id);
  j.write_int("sri-PUSCH-PathlossReferenceRS-Id", sri_pusch_pathloss_ref_rs_id);
  j.write_int("sri-P0-PUSCH-AlphaSetId", sri_p0_pusch_alpha_set_id);
  j.write_str("sri-PUSCH-ClosedLoopIndex", sri_pusch_closed_loop_idx.to_string());
  j.end_obj();
}

const char* sri_pusch_pwr_ctrl_s::sri_pusch_closed_loop_idx_opts::to_string() const
{
  static const char* names[] = {"i0", "i1"};
  return convert_enum_idx(names, 2, value, "sri_pusch_pwr_ctrl_s::sri_pusch_closed_loop_idx_e_");
}
uint8_t sri_pusch_pwr_ctrl_s::sri_pusch_closed_loop_idx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "sri_pusch_pwr_ctrl_s::sri_pusch_closed_loop_idx_e_");
}

// CG-UCI-OnPUSCH ::= CHOICE
void cg_uci_on_pusch_c::destroy_()
{
  switch (type_) {
    case types::dyn:
      c.destroy<dyn_l_>();
      break;
    case types::semi_static:
      c.destroy<beta_offsets_s>();
      break;
    default:
      break;
  }
}
void cg_uci_on_pusch_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::dyn:
      c.init<dyn_l_>();
      break;
    case types::semi_static:
      c.init<beta_offsets_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cg_uci_on_pusch_c");
  }
}
cg_uci_on_pusch_c::cg_uci_on_pusch_c(const cg_uci_on_pusch_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::dyn:
      c.init(other.c.get<dyn_l_>());
      break;
    case types::semi_static:
      c.init(other.c.get<beta_offsets_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cg_uci_on_pusch_c");
  }
}
cg_uci_on_pusch_c& cg_uci_on_pusch_c::operator=(const cg_uci_on_pusch_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::dyn:
      c.set(other.c.get<dyn_l_>());
      break;
    case types::semi_static:
      c.set(other.c.get<beta_offsets_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cg_uci_on_pusch_c");
  }

  return *this;
}
cg_uci_on_pusch_c::dyn_l_& cg_uci_on_pusch_c::set_dyn()
{
  set(types::dyn);
  return c.get<dyn_l_>();
}
beta_offsets_s& cg_uci_on_pusch_c::set_semi_static()
{
  set(types::semi_static);
  return c.get<beta_offsets_s>();
}
void cg_uci_on_pusch_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dyn:
      j.start_array("dynamic");
      for (const auto& e1 : c.get<dyn_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::semi_static:
      j.write_fieldname("semiStatic");
      c.get<beta_offsets_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "cg_uci_on_pusch_c");
  }
  j.end_obj();
}
SRSASN_CODE cg_uci_on_pusch_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dyn:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<dyn_l_>(), 1, 4));
      break;
    case types::semi_static:
      HANDLE_CODE(c.get<beta_offsets_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cg_uci_on_pusch_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_uci_on_pusch_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dyn:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<dyn_l_>(), bref, 1, 4));
      break;
    case types::semi_static:
      HANDLE_CODE(c.get<beta_offsets_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cg_uci_on_pusch_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// DMRS-UplinkConfig ::= SEQUENCE
SRSASN_CODE dmrs_ul_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dmrs_type_present, 1));
  HANDLE_CODE(bref.pack(dmrs_add_position_present, 1));
  HANDLE_CODE(bref.pack(phase_tracking_rs_present, 1));
  HANDLE_CODE(bref.pack(max_len_present, 1));
  HANDLE_CODE(bref.pack(transform_precoding_disabled_present, 1));
  HANDLE_CODE(bref.pack(transform_precoding_enabled_present, 1));

  if (dmrs_add_position_present) {
    HANDLE_CODE(dmrs_add_position.pack(bref));
  }
  if (phase_tracking_rs_present) {
    HANDLE_CODE(phase_tracking_rs.pack(bref));
  }
  if (transform_precoding_disabled_present) {
    bref.pack(transform_precoding_disabled.ext, 1);
    HANDLE_CODE(bref.pack(transform_precoding_disabled.scrambling_id0_present, 1));
    HANDLE_CODE(bref.pack(transform_precoding_disabled.scrambling_id1_present, 1));
    if (transform_precoding_disabled.scrambling_id0_present) {
      HANDLE_CODE(pack_integer(bref, transform_precoding_disabled.scrambling_id0, (uint32_t)0u, (uint32_t)65535u));
    }
    if (transform_precoding_disabled.scrambling_id1_present) {
      HANDLE_CODE(pack_integer(bref, transform_precoding_disabled.scrambling_id1, (uint32_t)0u, (uint32_t)65535u));
    }
  }
  if (transform_precoding_enabled_present) {
    bref.pack(transform_precoding_enabled.ext, 1);
    HANDLE_CODE(bref.pack(transform_precoding_enabled.npusch_id_present, 1));
    HANDLE_CODE(bref.pack(transform_precoding_enabled.seq_group_hop_present, 1));
    HANDLE_CODE(bref.pack(transform_precoding_enabled.seq_hop_present, 1));
    if (transform_precoding_enabled.npusch_id_present) {
      HANDLE_CODE(pack_integer(bref, transform_precoding_enabled.npusch_id, (uint16_t)0u, (uint16_t)1007u));
    }
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dmrs_ul_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dmrs_type_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_add_position_present, 1));
  HANDLE_CODE(bref.unpack(phase_tracking_rs_present, 1));
  HANDLE_CODE(bref.unpack(max_len_present, 1));
  HANDLE_CODE(bref.unpack(transform_precoding_disabled_present, 1));
  HANDLE_CODE(bref.unpack(transform_precoding_enabled_present, 1));

  if (dmrs_add_position_present) {
    HANDLE_CODE(dmrs_add_position.unpack(bref));
  }
  if (phase_tracking_rs_present) {
    HANDLE_CODE(phase_tracking_rs.unpack(bref));
  }
  if (transform_precoding_disabled_present) {
    bref.unpack(transform_precoding_disabled.ext, 1);
    HANDLE_CODE(bref.unpack(transform_precoding_disabled.scrambling_id0_present, 1));
    HANDLE_CODE(bref.unpack(transform_precoding_disabled.scrambling_id1_present, 1));
    if (transform_precoding_disabled.scrambling_id0_present) {
      HANDLE_CODE(unpack_integer(transform_precoding_disabled.scrambling_id0, bref, (uint32_t)0u, (uint32_t)65535u));
    }
    if (transform_precoding_disabled.scrambling_id1_present) {
      HANDLE_CODE(unpack_integer(transform_precoding_disabled.scrambling_id1, bref, (uint32_t)0u, (uint32_t)65535u));
    }
  }
  if (transform_precoding_enabled_present) {
    bref.unpack(transform_precoding_enabled.ext, 1);
    HANDLE_CODE(bref.unpack(transform_precoding_enabled.npusch_id_present, 1));
    HANDLE_CODE(bref.unpack(transform_precoding_enabled.seq_group_hop_present, 1));
    HANDLE_CODE(bref.unpack(transform_precoding_enabled.seq_hop_present, 1));
    if (transform_precoding_enabled.npusch_id_present) {
      HANDLE_CODE(unpack_integer(transform_precoding_enabled.npusch_id, bref, (uint16_t)0u, (uint16_t)1007u));
    }
  }

  return SRSASN_SUCCESS;
}
void dmrs_ul_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dmrs_type_present) {
    j.write_str("dmrs-Type", "type2");
  }
  if (dmrs_add_position_present) {
    j.write_str("dmrs-AdditionalPosition", dmrs_add_position.to_string());
  }
  if (phase_tracking_rs_present) {
    j.write_fieldname("phaseTrackingRS");
    phase_tracking_rs.to_json(j);
  }
  if (max_len_present) {
    j.write_str("maxLength", "len2");
  }
  if (transform_precoding_disabled_present) {
    j.write_fieldname("transformPrecodingDisabled");
    j.start_obj();
    if (transform_precoding_disabled.scrambling_id0_present) {
      j.write_int("scramblingID0", transform_precoding_disabled.scrambling_id0);
    }
    if (transform_precoding_disabled.scrambling_id1_present) {
      j.write_int("scramblingID1", transform_precoding_disabled.scrambling_id1);
    }
    j.end_obj();
  }
  if (transform_precoding_enabled_present) {
    j.write_fieldname("transformPrecodingEnabled");
    j.start_obj();
    if (transform_precoding_enabled.npusch_id_present) {
      j.write_int("nPUSCH-Identity", transform_precoding_enabled.npusch_id);
    }
    if (transform_precoding_enabled.seq_group_hop_present) {
      j.write_str("sequenceGroupHopping", "disabled");
    }
    if (transform_precoding_enabled.seq_hop_present) {
      j.write_str("sequenceHopping", "enabled");
    }
    j.end_obj();
  }
  j.end_obj();
}

const char* dmrs_ul_cfg_s::dmrs_add_position_opts::to_string() const
{
  static const char* names[] = {"pos0", "pos1", "pos3"};
  return convert_enum_idx(names, 3, value, "dmrs_ul_cfg_s::dmrs_add_position_e_");
}
uint8_t dmrs_ul_cfg_s::dmrs_add_position_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 3};
  return map_enum_number(numbers, 3, value, "dmrs_ul_cfg_s::dmrs_add_position_e_");
}

// PRACH-ResourceDedicatedBFR ::= CHOICE
void prach_res_ded_bfr_c::destroy_()
{
  switch (type_) {
    case types::ssb:
      c.destroy<bfr_ssb_res_s>();
      break;
    case types::csi_rs:
      c.destroy<bfr_csi_rs_res_s>();
      break;
    default:
      break;
  }
}
void prach_res_ded_bfr_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ssb:
      c.init<bfr_ssb_res_s>();
      break;
    case types::csi_rs:
      c.init<bfr_csi_rs_res_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "prach_res_ded_bfr_c");
  }
}
prach_res_ded_bfr_c::prach_res_ded_bfr_c(const prach_res_ded_bfr_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb:
      c.init(other.c.get<bfr_ssb_res_s>());
      break;
    case types::csi_rs:
      c.init(other.c.get<bfr_csi_rs_res_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "prach_res_ded_bfr_c");
  }
}
prach_res_ded_bfr_c& prach_res_ded_bfr_c::operator=(const prach_res_ded_bfr_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb:
      c.set(other.c.get<bfr_ssb_res_s>());
      break;
    case types::csi_rs:
      c.set(other.c.get<bfr_csi_rs_res_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "prach_res_ded_bfr_c");
  }

  return *this;
}
bfr_ssb_res_s& prach_res_ded_bfr_c::set_ssb()
{
  set(types::ssb);
  return c.get<bfr_ssb_res_s>();
}
bfr_csi_rs_res_s& prach_res_ded_bfr_c::set_csi_rs()
{
  set(types::csi_rs);
  return c.get<bfr_csi_rs_res_s>();
}
void prach_res_ded_bfr_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb:
      j.write_fieldname("ssb");
      c.get<bfr_ssb_res_s>().to_json(j);
      break;
    case types::csi_rs:
      j.write_fieldname("csi-RS");
      c.get<bfr_csi_rs_res_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "prach_res_ded_bfr_c");
  }
  j.end_obj();
}
SRSASN_CODE prach_res_ded_bfr_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb:
      HANDLE_CODE(c.get<bfr_ssb_res_s>().pack(bref));
      break;
    case types::csi_rs:
      HANDLE_CODE(c.get<bfr_csi_rs_res_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "prach_res_ded_bfr_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_res_ded_bfr_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb:
      HANDLE_CODE(c.get<bfr_ssb_res_s>().unpack(bref));
      break;
    case types::csi_rs:
      HANDLE_CODE(c.get<bfr_csi_rs_res_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "prach_res_ded_bfr_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUCCH-FormatConfig ::= SEQUENCE
SRSASN_CODE pucch_format_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(interslot_freq_hop_present, 1));
  HANDLE_CODE(bref.pack(add_dmrs_present, 1));
  HANDLE_CODE(bref.pack(max_code_rate_present, 1));
  HANDLE_CODE(bref.pack(nrof_slots_present, 1));
  HANDLE_CODE(bref.pack(pi2_bpsk_present, 1));
  HANDLE_CODE(bref.pack(simul_harq_ack_csi_present, 1));

  if (max_code_rate_present) {
    HANDLE_CODE(max_code_rate.pack(bref));
  }
  if (nrof_slots_present) {
    HANDLE_CODE(nrof_slots.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_format_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(interslot_freq_hop_present, 1));
  HANDLE_CODE(bref.unpack(add_dmrs_present, 1));
  HANDLE_CODE(bref.unpack(max_code_rate_present, 1));
  HANDLE_CODE(bref.unpack(nrof_slots_present, 1));
  HANDLE_CODE(bref.unpack(pi2_bpsk_present, 1));
  HANDLE_CODE(bref.unpack(simul_harq_ack_csi_present, 1));

  if (max_code_rate_present) {
    HANDLE_CODE(max_code_rate.unpack(bref));
  }
  if (nrof_slots_present) {
    HANDLE_CODE(nrof_slots.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_format_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (interslot_freq_hop_present) {
    j.write_str("interslotFrequencyHopping", "enabled");
  }
  if (add_dmrs_present) {
    j.write_str("additionalDMRS", "true");
  }
  if (max_code_rate_present) {
    j.write_str("maxCodeRate", max_code_rate.to_string());
  }
  if (nrof_slots_present) {
    j.write_str("nrofSlots", nrof_slots.to_string());
  }
  if (pi2_bpsk_present) {
    j.write_str("pi2BPSK", "enabled");
  }
  if (simul_harq_ack_csi_present) {
    j.write_str("simultaneousHARQ-ACK-CSI", "true");
  }
  j.end_obj();
}

const char* pucch_format_cfg_s::nrof_slots_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8"};
  return convert_enum_idx(names, 3, value, "pucch_format_cfg_s::nrof_slots_e_");
}
uint8_t pucch_format_cfg_s::nrof_slots_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "pucch_format_cfg_s::nrof_slots_e_");
}

// PUCCH-PowerControl ::= SEQUENCE
SRSASN_CODE pucch_pwr_ctrl_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(delta_f_pucch_f0_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_f1_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_f2_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_f3_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_f4_present, 1));
  HANDLE_CODE(bref.pack(p0_set.size() > 0, 1));
  HANDLE_CODE(bref.pack(pathloss_ref_rss.size() > 0, 1));
  HANDLE_CODE(bref.pack(two_pucch_pc_adjustment_states_present, 1));

  if (delta_f_pucch_f0_present) {
    HANDLE_CODE(pack_integer(bref, delta_f_pucch_f0, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f1_present) {
    HANDLE_CODE(pack_integer(bref, delta_f_pucch_f1, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f2_present) {
    HANDLE_CODE(pack_integer(bref, delta_f_pucch_f2, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f3_present) {
    HANDLE_CODE(pack_integer(bref, delta_f_pucch_f3, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f4_present) {
    HANDLE_CODE(pack_integer(bref, delta_f_pucch_f4, (int8_t)-16, (int8_t)15));
  }
  if (p0_set.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p0_set, 1, 8));
  }
  if (pathloss_ref_rss.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pathloss_ref_rss, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_pwr_ctrl_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(delta_f_pucch_f0_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_f1_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_f2_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_f3_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_f4_present, 1));
  bool p0_set_present;
  HANDLE_CODE(bref.unpack(p0_set_present, 1));
  bool pathloss_ref_rss_present;
  HANDLE_CODE(bref.unpack(pathloss_ref_rss_present, 1));
  HANDLE_CODE(bref.unpack(two_pucch_pc_adjustment_states_present, 1));

  if (delta_f_pucch_f0_present) {
    HANDLE_CODE(unpack_integer(delta_f_pucch_f0, bref, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f1_present) {
    HANDLE_CODE(unpack_integer(delta_f_pucch_f1, bref, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f2_present) {
    HANDLE_CODE(unpack_integer(delta_f_pucch_f2, bref, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f3_present) {
    HANDLE_CODE(unpack_integer(delta_f_pucch_f3, bref, (int8_t)-16, (int8_t)15));
  }
  if (delta_f_pucch_f4_present) {
    HANDLE_CODE(unpack_integer(delta_f_pucch_f4, bref, (int8_t)-16, (int8_t)15));
  }
  if (p0_set_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p0_set, bref, 1, 8));
  }
  if (pathloss_ref_rss_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pathloss_ref_rss, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void pucch_pwr_ctrl_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (delta_f_pucch_f0_present) {
    j.write_int("deltaF-PUCCH-f0", delta_f_pucch_f0);
  }
  if (delta_f_pucch_f1_present) {
    j.write_int("deltaF-PUCCH-f1", delta_f_pucch_f1);
  }
  if (delta_f_pucch_f2_present) {
    j.write_int("deltaF-PUCCH-f2", delta_f_pucch_f2);
  }
  if (delta_f_pucch_f3_present) {
    j.write_int("deltaF-PUCCH-f3", delta_f_pucch_f3);
  }
  if (delta_f_pucch_f4_present) {
    j.write_int("deltaF-PUCCH-f4", delta_f_pucch_f4);
  }
  if (p0_set.size() > 0) {
    j.start_array("p0-Set");
    for (const auto& e1 : p0_set) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pathloss_ref_rss.size() > 0) {
    j.start_array("pathlossReferenceRSs");
    for (const auto& e1 : pathloss_ref_rss) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (two_pucch_pc_adjustment_states_present) {
    j.write_str("twoPUCCH-PC-AdjustmentStates", "twoStates");
  }
  j.end_obj();
}

// PUCCH-Resource ::= SEQUENCE
SRSASN_CODE pucch_res_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(intra_slot_freq_hop_present, 1));
  HANDLE_CODE(bref.pack(second_hop_prb_present, 1));

  HANDLE_CODE(pack_integer(bref, pucch_res_id, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(pack_integer(bref, start_prb, (uint16_t)0u, (uint16_t)274u));
  if (second_hop_prb_present) {
    HANDLE_CODE(pack_integer(bref, second_hop_prb, (uint16_t)0u, (uint16_t)274u));
  }
  HANDLE_CODE(format.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_res_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(intra_slot_freq_hop_present, 1));
  HANDLE_CODE(bref.unpack(second_hop_prb_present, 1));

  HANDLE_CODE(unpack_integer(pucch_res_id, bref, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(unpack_integer(start_prb, bref, (uint16_t)0u, (uint16_t)274u));
  if (second_hop_prb_present) {
    HANDLE_CODE(unpack_integer(second_hop_prb, bref, (uint16_t)0u, (uint16_t)274u));
  }
  HANDLE_CODE(format.unpack(bref));

  return SRSASN_SUCCESS;
}
void pucch_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pucch-ResourceId", pucch_res_id);
  j.write_int("startingPRB", start_prb);
  if (intra_slot_freq_hop_present) {
    j.write_str("intraSlotFrequencyHopping", "enabled");
  }
  if (second_hop_prb_present) {
    j.write_int("secondHopPRB", second_hop_prb);
  }
  j.write_fieldname("format");
  format.to_json(j);
  j.end_obj();
}

void pucch_res_s::format_c_::destroy_()
{
  switch (type_) {
    case types::format0:
      c.destroy<pucch_format0_s>();
      break;
    case types::format1:
      c.destroy<pucch_format1_s>();
      break;
    case types::format2:
      c.destroy<pucch_format2_s>();
      break;
    case types::format3:
      c.destroy<pucch_format3_s>();
      break;
    case types::format4:
      c.destroy<pucch_format4_s>();
      break;
    default:
      break;
  }
}
void pucch_res_s::format_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::format0:
      c.init<pucch_format0_s>();
      break;
    case types::format1:
      c.init<pucch_format1_s>();
      break;
    case types::format2:
      c.init<pucch_format2_s>();
      break;
    case types::format3:
      c.init<pucch_format3_s>();
      break;
    case types::format4:
      c.init<pucch_format4_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_res_s::format_c_");
  }
}
pucch_res_s::format_c_::format_c_(const pucch_res_s::format_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::format0:
      c.init(other.c.get<pucch_format0_s>());
      break;
    case types::format1:
      c.init(other.c.get<pucch_format1_s>());
      break;
    case types::format2:
      c.init(other.c.get<pucch_format2_s>());
      break;
    case types::format3:
      c.init(other.c.get<pucch_format3_s>());
      break;
    case types::format4:
      c.init(other.c.get<pucch_format4_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_res_s::format_c_");
  }
}
pucch_res_s::format_c_& pucch_res_s::format_c_::operator=(const pucch_res_s::format_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::format0:
      c.set(other.c.get<pucch_format0_s>());
      break;
    case types::format1:
      c.set(other.c.get<pucch_format1_s>());
      break;
    case types::format2:
      c.set(other.c.get<pucch_format2_s>());
      break;
    case types::format3:
      c.set(other.c.get<pucch_format3_s>());
      break;
    case types::format4:
      c.set(other.c.get<pucch_format4_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_res_s::format_c_");
  }

  return *this;
}
pucch_format0_s& pucch_res_s::format_c_::set_format0()
{
  set(types::format0);
  return c.get<pucch_format0_s>();
}
pucch_format1_s& pucch_res_s::format_c_::set_format1()
{
  set(types::format1);
  return c.get<pucch_format1_s>();
}
pucch_format2_s& pucch_res_s::format_c_::set_format2()
{
  set(types::format2);
  return c.get<pucch_format2_s>();
}
pucch_format3_s& pucch_res_s::format_c_::set_format3()
{
  set(types::format3);
  return c.get<pucch_format3_s>();
}
pucch_format4_s& pucch_res_s::format_c_::set_format4()
{
  set(types::format4);
  return c.get<pucch_format4_s>();
}
void pucch_res_s::format_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::format0:
      j.write_fieldname("format0");
      c.get<pucch_format0_s>().to_json(j);
      break;
    case types::format1:
      j.write_fieldname("format1");
      c.get<pucch_format1_s>().to_json(j);
      break;
    case types::format2:
      j.write_fieldname("format2");
      c.get<pucch_format2_s>().to_json(j);
      break;
    case types::format3:
      j.write_fieldname("format3");
      c.get<pucch_format3_s>().to_json(j);
      break;
    case types::format4:
      j.write_fieldname("format4");
      c.get<pucch_format4_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pucch_res_s::format_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_res_s::format_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::format0:
      HANDLE_CODE(c.get<pucch_format0_s>().pack(bref));
      break;
    case types::format1:
      HANDLE_CODE(c.get<pucch_format1_s>().pack(bref));
      break;
    case types::format2:
      HANDLE_CODE(c.get<pucch_format2_s>().pack(bref));
      break;
    case types::format3:
      HANDLE_CODE(c.get<pucch_format3_s>().pack(bref));
      break;
    case types::format4:
      HANDLE_CODE(c.get<pucch_format4_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_res_s::format_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_res_s::format_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::format0:
      HANDLE_CODE(c.get<pucch_format0_s>().unpack(bref));
      break;
    case types::format1:
      HANDLE_CODE(c.get<pucch_format1_s>().unpack(bref));
      break;
    case types::format2:
      HANDLE_CODE(c.get<pucch_format2_s>().unpack(bref));
      break;
    case types::format3:
      HANDLE_CODE(c.get<pucch_format3_s>().unpack(bref));
      break;
    case types::format4:
      HANDLE_CODE(c.get<pucch_format4_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_res_s::format_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PUCCH-ResourceSet ::= SEQUENCE
SRSASN_CODE pucch_res_set_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(max_payload_size_present, 1));

  HANDLE_CODE(pack_integer(bref, pucch_res_set_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_dyn_seq_of(bref, res_list, 1, 32, integer_packer<uint8_t>(0, 127)));
  if (max_payload_size_present) {
    HANDLE_CODE(pack_integer(bref, max_payload_size, (uint16_t)4u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_res_set_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(max_payload_size_present, 1));

  HANDLE_CODE(unpack_integer(pucch_res_set_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_dyn_seq_of(res_list, bref, 1, 32, integer_packer<uint8_t>(0, 127)));
  if (max_payload_size_present) {
    HANDLE_CODE(unpack_integer(max_payload_size, bref, (uint16_t)4u, (uint16_t)256u));
  }

  return SRSASN_SUCCESS;
}
void pucch_res_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pucch-ResourceSetId", pucch_res_set_id);
  j.start_array("resourceList");
  for (const auto& e1 : res_list) {
    j.write_int(e1);
  }
  j.end_array();
  if (max_payload_size_present) {
    j.write_int("maxPayloadSize", max_payload_size);
  }
  j.end_obj();
}

// PUCCH-SpatialRelationInfo ::= SEQUENCE
SRSASN_CODE pucch_spatial_relation_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serving_cell_id_present, 1));

  HANDLE_CODE(pack_integer(bref, pucch_spatial_relation_info_id, (uint8_t)1u, (uint8_t)8u));
  if (serving_cell_id_present) {
    HANDLE_CODE(pack_integer(bref, serving_cell_id, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(ref_sig.pack(bref));
  HANDLE_CODE(pack_integer(bref, pucch_pathloss_ref_rs_id, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, p0_pucch_id, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(closed_loop_idx.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_spatial_relation_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(serving_cell_id_present, 1));

  HANDLE_CODE(unpack_integer(pucch_spatial_relation_info_id, bref, (uint8_t)1u, (uint8_t)8u));
  if (serving_cell_id_present) {
    HANDLE_CODE(unpack_integer(serving_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(ref_sig.unpack(bref));
  HANDLE_CODE(unpack_integer(pucch_pathloss_ref_rs_id, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(p0_pucch_id, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(closed_loop_idx.unpack(bref));

  return SRSASN_SUCCESS;
}
void pucch_spatial_relation_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pucch-SpatialRelationInfoId", pucch_spatial_relation_info_id);
  if (serving_cell_id_present) {
    j.write_int("servingCellId", serving_cell_id);
  }
  j.write_fieldname("referenceSignal");
  ref_sig.to_json(j);
  j.write_int("pucch-PathlossReferenceRS-Id", pucch_pathloss_ref_rs_id);
  j.write_int("p0-PUCCH-Id", p0_pucch_id);
  j.write_str("closedLoopIndex", closed_loop_idx.to_string());
  j.end_obj();
}

void pucch_spatial_relation_info_s::ref_sig_c_::destroy_()
{
  switch (type_) {
    case types::srs:
      c.destroy<srs_s_>();
      break;
    default:
      break;
  }
}
void pucch_spatial_relation_info_s::ref_sig_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ssb_idx:
      break;
    case types::csi_rs_idx:
      break;
    case types::srs:
      c.init<srs_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_spatial_relation_info_s::ref_sig_c_");
  }
}
pucch_spatial_relation_info_s::ref_sig_c_::ref_sig_c_(const pucch_spatial_relation_info_s::ref_sig_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::srs:
      c.init(other.c.get<srs_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_spatial_relation_info_s::ref_sig_c_");
  }
}
pucch_spatial_relation_info_s::ref_sig_c_&
pucch_spatial_relation_info_s::ref_sig_c_::operator=(const pucch_spatial_relation_info_s::ref_sig_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::srs:
      c.set(other.c.get<srs_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pucch_spatial_relation_info_s::ref_sig_c_");
  }

  return *this;
}
uint8_t& pucch_spatial_relation_info_s::ref_sig_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& pucch_spatial_relation_info_s::ref_sig_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
pucch_spatial_relation_info_s::ref_sig_c_::srs_s_& pucch_spatial_relation_info_s::ref_sig_c_::set_srs()
{
  set(types::srs);
  return c.get<srs_s_>();
}
void pucch_spatial_relation_info_s::ref_sig_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_idx:
      j.write_int("ssb-Index", c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      j.write_int("csi-RS-Index", c.get<uint8_t>());
      break;
    case types::srs:
      j.write_fieldname("srs");
      j.start_obj();
      j.write_int("resource", c.get<srs_s_>().res);
      j.write_int("uplinkBWP", c.get<srs_s_>().ul_bwp);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "pucch_spatial_relation_info_s::ref_sig_c_");
  }
  j.end_obj();
}
SRSASN_CODE pucch_spatial_relation_info_s::ref_sig_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    case types::srs:
      HANDLE_CODE(pack_integer(bref, c.get<srs_s_>().res, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(pack_integer(bref, c.get<srs_s_>().ul_bwp, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_spatial_relation_info_s::ref_sig_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_spatial_relation_info_s::ref_sig_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)191u));
      break;
    case types::srs:
      HANDLE_CODE(unpack_integer(c.get<srs_s_>().res, bref, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(unpack_integer(c.get<srs_s_>().ul_bwp, bref, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "pucch_spatial_relation_info_s::ref_sig_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pucch_spatial_relation_info_s::closed_loop_idx_opts::to_string() const
{
  static const char* names[] = {"i0", "i1"};
  return convert_enum_idx(names, 2, value, "pucch_spatial_relation_info_s::closed_loop_idx_e_");
}
uint8_t pucch_spatial_relation_info_s::closed_loop_idx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "pucch_spatial_relation_info_s::closed_loop_idx_e_");
}

// PUSCH-PowerControl ::= SEQUENCE
SRSASN_CODE pusch_pwr_ctrl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tpc_accumulation_present, 1));
  HANDLE_CODE(bref.pack(msg3_alpha_present, 1));
  HANDLE_CODE(bref.pack(p0_nominal_without_grant_present, 1));
  HANDLE_CODE(bref.pack(p0_alpha_sets.size() > 0, 1));
  HANDLE_CODE(bref.pack(pathloss_ref_rs_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(pathloss_ref_rs_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(two_pusch_pc_adjustment_states_present, 1));
  HANDLE_CODE(bref.pack(delta_mcs_present, 1));
  HANDLE_CODE(bref.pack(sri_pusch_map_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(sri_pusch_map_to_release_list.size() > 0, 1));

  if (msg3_alpha_present) {
    HANDLE_CODE(msg3_alpha.pack(bref));
  }
  if (p0_nominal_without_grant_present) {
    HANDLE_CODE(pack_integer(bref, p0_nominal_without_grant, (int16_t)-202, (int16_t)24));
  }
  if (p0_alpha_sets.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, p0_alpha_sets, 1, 30));
  }
  if (pathloss_ref_rs_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pathloss_ref_rs_to_add_mod_list, 1, 4));
  }
  if (pathloss_ref_rs_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pathloss_ref_rs_to_release_list, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (sri_pusch_map_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sri_pusch_map_to_add_mod_list, 1, 16));
  }
  if (sri_pusch_map_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sri_pusch_map_to_release_list, 1, 16, integer_packer<uint8_t>(0, 15)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_pwr_ctrl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tpc_accumulation_present, 1));
  HANDLE_CODE(bref.unpack(msg3_alpha_present, 1));
  HANDLE_CODE(bref.unpack(p0_nominal_without_grant_present, 1));
  bool p0_alpha_sets_present;
  HANDLE_CODE(bref.unpack(p0_alpha_sets_present, 1));
  bool pathloss_ref_rs_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(pathloss_ref_rs_to_add_mod_list_present, 1));
  bool pathloss_ref_rs_to_release_list_present;
  HANDLE_CODE(bref.unpack(pathloss_ref_rs_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(two_pusch_pc_adjustment_states_present, 1));
  HANDLE_CODE(bref.unpack(delta_mcs_present, 1));
  bool sri_pusch_map_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(sri_pusch_map_to_add_mod_list_present, 1));
  bool sri_pusch_map_to_release_list_present;
  HANDLE_CODE(bref.unpack(sri_pusch_map_to_release_list_present, 1));

  if (msg3_alpha_present) {
    HANDLE_CODE(msg3_alpha.unpack(bref));
  }
  if (p0_nominal_without_grant_present) {
    HANDLE_CODE(unpack_integer(p0_nominal_without_grant, bref, (int16_t)-202, (int16_t)24));
  }
  if (p0_alpha_sets_present) {
    HANDLE_CODE(unpack_dyn_seq_of(p0_alpha_sets, bref, 1, 30));
  }
  if (pathloss_ref_rs_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pathloss_ref_rs_to_add_mod_list, bref, 1, 4));
  }
  if (pathloss_ref_rs_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pathloss_ref_rs_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (sri_pusch_map_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sri_pusch_map_to_add_mod_list, bref, 1, 16));
  }
  if (sri_pusch_map_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sri_pusch_map_to_release_list, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
  }

  return SRSASN_SUCCESS;
}
void pusch_pwr_ctrl_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tpc_accumulation_present) {
    j.write_str("tpc-Accumulation", "disabled");
  }
  if (msg3_alpha_present) {
    j.write_str("msg3-Alpha", msg3_alpha.to_string());
  }
  if (p0_nominal_without_grant_present) {
    j.write_int("p0-NominalWithoutGrant", p0_nominal_without_grant);
  }
  if (p0_alpha_sets.size() > 0) {
    j.start_array("p0-AlphaSets");
    for (const auto& e1 : p0_alpha_sets) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pathloss_ref_rs_to_add_mod_list.size() > 0) {
    j.start_array("pathlossReferenceRSToAddModList");
    for (const auto& e1 : pathloss_ref_rs_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pathloss_ref_rs_to_release_list.size() > 0) {
    j.start_array("pathlossReferenceRSToReleaseList");
    for (const auto& e1 : pathloss_ref_rs_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (two_pusch_pc_adjustment_states_present) {
    j.write_str("twoPUSCH-PC-AdjustmentStates", "twoStates");
  }
  if (delta_mcs_present) {
    j.write_str("deltaMCS", "enabled");
  }
  if (sri_pusch_map_to_add_mod_list.size() > 0) {
    j.start_array("sri-PUSCH-MappingToAddModList");
    for (const auto& e1 : sri_pusch_map_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sri_pusch_map_to_release_list.size() > 0) {
    j.start_array("sri-PUSCH-MappingToReleaseList");
    for (const auto& e1 : sri_pusch_map_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// RA-Prioritization ::= SEQUENCE
SRSASN_CODE ra_prioritization_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(scaling_factor_bi_present, 1));

  HANDLE_CODE(pwr_ramp_step_high_prio.pack(bref));
  if (scaling_factor_bi_present) {
    HANDLE_CODE(scaling_factor_bi.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_prioritization_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(scaling_factor_bi_present, 1));

  HANDLE_CODE(pwr_ramp_step_high_prio.unpack(bref));
  if (scaling_factor_bi_present) {
    HANDLE_CODE(scaling_factor_bi.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ra_prioritization_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("powerRampingStepHighPriority", pwr_ramp_step_high_prio.to_string());
  if (scaling_factor_bi_present) {
    j.write_str("scalingFactorBI", scaling_factor_bi.to_string());
  }
  j.end_obj();
}

const char* ra_prioritization_s::pwr_ramp_step_high_prio_opts::to_string() const
{
  static const char* names[] = {"dB0", "dB2", "dB4", "dB6"};
  return convert_enum_idx(names, 4, value, "ra_prioritization_s::pwr_ramp_step_high_prio_e_");
}
uint8_t ra_prioritization_s::pwr_ramp_step_high_prio_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6};
  return map_enum_number(numbers, 4, value, "ra_prioritization_s::pwr_ramp_step_high_prio_e_");
}

const char* ra_prioritization_s::scaling_factor_bi_opts::to_string() const
{
  static const char* names[] = {"zero", "dot25", "dot5", "dot75"};
  return convert_enum_idx(names, 4, value, "ra_prioritization_s::scaling_factor_bi_e_");
}
uint8_t ra_prioritization_s::scaling_factor_bi_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 25, 5, 75};
  return map_enum_number(numbers, 4, value, "ra_prioritization_s::scaling_factor_bi_e_");
}

// SRS-ResourceSet ::= SEQUENCE
SRSASN_CODE srs_res_set_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srs_res_id_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(alpha_present, 1));
  HANDLE_CODE(bref.pack(p0_present, 1));
  HANDLE_CODE(bref.pack(pathloss_ref_rs_present, 1));
  HANDLE_CODE(bref.pack(srs_pwr_ctrl_adjustment_states_present, 1));

  HANDLE_CODE(pack_integer(bref, srs_res_set_id, (uint8_t)0u, (uint8_t)15u));
  if (srs_res_id_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_res_id_list, 1, 16, integer_packer<uint8_t>(0, 63)));
  }
  HANDLE_CODE(res_type.pack(bref));
  HANDLE_CODE(usage.pack(bref));
  if (alpha_present) {
    HANDLE_CODE(alpha.pack(bref));
  }
  if (p0_present) {
    HANDLE_CODE(pack_integer(bref, p0, (int16_t)-202, (int16_t)24));
  }
  if (pathloss_ref_rs_present) {
    HANDLE_CODE(pathloss_ref_rs.pack(bref));
  }
  if (srs_pwr_ctrl_adjustment_states_present) {
    HANDLE_CODE(srs_pwr_ctrl_adjustment_states.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_set_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool srs_res_id_list_present;
  HANDLE_CODE(bref.unpack(srs_res_id_list_present, 1));
  HANDLE_CODE(bref.unpack(alpha_present, 1));
  HANDLE_CODE(bref.unpack(p0_present, 1));
  HANDLE_CODE(bref.unpack(pathloss_ref_rs_present, 1));
  HANDLE_CODE(bref.unpack(srs_pwr_ctrl_adjustment_states_present, 1));

  HANDLE_CODE(unpack_integer(srs_res_set_id, bref, (uint8_t)0u, (uint8_t)15u));
  if (srs_res_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_res_id_list, bref, 1, 16, integer_packer<uint8_t>(0, 63)));
  }
  HANDLE_CODE(res_type.unpack(bref));
  HANDLE_CODE(usage.unpack(bref));
  if (alpha_present) {
    HANDLE_CODE(alpha.unpack(bref));
  }
  if (p0_present) {
    HANDLE_CODE(unpack_integer(p0, bref, (int16_t)-202, (int16_t)24));
  }
  if (pathloss_ref_rs_present) {
    HANDLE_CODE(pathloss_ref_rs.unpack(bref));
  }
  if (srs_pwr_ctrl_adjustment_states_present) {
    HANDLE_CODE(srs_pwr_ctrl_adjustment_states.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_res_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srs-ResourceSetId", srs_res_set_id);
  if (srs_res_id_list.size() > 0) {
    j.start_array("srs-ResourceIdList");
    for (const auto& e1 : srs_res_id_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_fieldname("resourceType");
  res_type.to_json(j);
  j.write_str("usage", usage.to_string());
  if (alpha_present) {
    j.write_str("alpha", alpha.to_string());
  }
  if (p0_present) {
    j.write_int("p0", p0);
  }
  if (pathloss_ref_rs_present) {
    j.write_fieldname("pathlossReferenceRS");
    pathloss_ref_rs.to_json(j);
  }
  if (srs_pwr_ctrl_adjustment_states_present) {
    j.write_str("srs-PowerControlAdjustmentStates", srs_pwr_ctrl_adjustment_states.to_string());
  }
  j.end_obj();
}

void srs_res_set_s::res_type_c_::destroy_()
{
  switch (type_) {
    case types::aperiodic:
      c.destroy<aperiodic_s_>();
      break;
    case types::semi_persistent:
      c.destroy<semi_persistent_s_>();
      break;
    case types::periodic:
      c.destroy<periodic_s_>();
      break;
    default:
      break;
  }
}
void srs_res_set_s::res_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::aperiodic:
      c.init<aperiodic_s_>();
      break;
    case types::semi_persistent:
      c.init<semi_persistent_s_>();
      break;
    case types::periodic:
      c.init<periodic_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::res_type_c_");
  }
}
srs_res_set_s::res_type_c_::res_type_c_(const srs_res_set_s::res_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::aperiodic:
      c.init(other.c.get<aperiodic_s_>());
      break;
    case types::semi_persistent:
      c.init(other.c.get<semi_persistent_s_>());
      break;
    case types::periodic:
      c.init(other.c.get<periodic_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::res_type_c_");
  }
}
srs_res_set_s::res_type_c_& srs_res_set_s::res_type_c_::operator=(const srs_res_set_s::res_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::aperiodic:
      c.set(other.c.get<aperiodic_s_>());
      break;
    case types::semi_persistent:
      c.set(other.c.get<semi_persistent_s_>());
      break;
    case types::periodic:
      c.set(other.c.get<periodic_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::res_type_c_");
  }

  return *this;
}
srs_res_set_s::res_type_c_::aperiodic_s_& srs_res_set_s::res_type_c_::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<aperiodic_s_>();
}
srs_res_set_s::res_type_c_::semi_persistent_s_& srs_res_set_s::res_type_c_::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<semi_persistent_s_>();
}
srs_res_set_s::res_type_c_::periodic_s_& srs_res_set_s::res_type_c_::set_periodic()
{
  set(types::periodic);
  return c.get<periodic_s_>();
}
void srs_res_set_s::res_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::aperiodic:
      j.write_fieldname("aperiodic");
      c.get<aperiodic_s_>().to_json(j);
      break;
    case types::semi_persistent:
      j.write_fieldname("semi-persistent");
      j.start_obj();
      if (c.get<semi_persistent_s_>().associated_csi_rs_present) {
        j.write_int("associatedCSI-RS", c.get<semi_persistent_s_>().associated_csi_rs);
      }
      j.end_obj();
      break;
    case types::periodic:
      j.write_fieldname("periodic");
      j.start_obj();
      if (c.get<periodic_s_>().associated_csi_rs_present) {
        j.write_int("associatedCSI-RS", c.get<periodic_s_>().associated_csi_rs);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::res_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_res_set_s::res_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::aperiodic:
      HANDLE_CODE(c.get<aperiodic_s_>().pack(bref));
      break;
    case types::semi_persistent:
      bref.pack(c.get<semi_persistent_s_>().ext, 1);
      HANDLE_CODE(bref.pack(c.get<semi_persistent_s_>().associated_csi_rs_present, 1));
      if (c.get<semi_persistent_s_>().associated_csi_rs_present) {
        HANDLE_CODE(pack_integer(bref, c.get<semi_persistent_s_>().associated_csi_rs, (uint8_t)0u, (uint8_t)191u));
      }
      break;
    case types::periodic:
      bref.pack(c.get<periodic_s_>().ext, 1);
      HANDLE_CODE(bref.pack(c.get<periodic_s_>().associated_csi_rs_present, 1));
      if (c.get<periodic_s_>().associated_csi_rs_present) {
        HANDLE_CODE(pack_integer(bref, c.get<periodic_s_>().associated_csi_rs, (uint8_t)0u, (uint8_t)191u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::res_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_set_s::res_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::aperiodic:
      HANDLE_CODE(c.get<aperiodic_s_>().unpack(bref));
      break;
    case types::semi_persistent:
      bref.unpack(c.get<semi_persistent_s_>().ext, 1);
      HANDLE_CODE(bref.unpack(c.get<semi_persistent_s_>().associated_csi_rs_present, 1));
      if (c.get<semi_persistent_s_>().associated_csi_rs_present) {
        HANDLE_CODE(unpack_integer(c.get<semi_persistent_s_>().associated_csi_rs, bref, (uint8_t)0u, (uint8_t)191u));
      }
      break;
    case types::periodic:
      bref.unpack(c.get<periodic_s_>().ext, 1);
      HANDLE_CODE(bref.unpack(c.get<periodic_s_>().associated_csi_rs_present, 1));
      if (c.get<periodic_s_>().associated_csi_rs_present) {
        HANDLE_CODE(unpack_integer(c.get<periodic_s_>().associated_csi_rs, bref, (uint8_t)0u, (uint8_t)191u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::res_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE srs_res_set_s::res_type_c_::aperiodic_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(csi_rs_present, 1));
  HANDLE_CODE(bref.pack(slot_offset_present, 1));

  HANDLE_CODE(pack_integer(bref, aperiodic_srs_res_trigger, (uint8_t)1u, (uint8_t)3u));
  if (csi_rs_present) {
    HANDLE_CODE(pack_integer(bref, csi_rs, (uint8_t)0u, (uint8_t)191u));
  }
  if (slot_offset_present) {
    HANDLE_CODE(pack_integer(bref, slot_offset, (uint8_t)1u, (uint8_t)32u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= aperiodic_srs_res_trigger_list.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(aperiodic_srs_res_trigger_list.is_present(), 1));
      if (aperiodic_srs_res_trigger_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *aperiodic_srs_res_trigger_list, 1, 2, integer_packer<uint8_t>(1, 3)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_set_s::res_type_c_::aperiodic_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(csi_rs_present, 1));
  HANDLE_CODE(bref.unpack(slot_offset_present, 1));

  HANDLE_CODE(unpack_integer(aperiodic_srs_res_trigger, bref, (uint8_t)1u, (uint8_t)3u));
  if (csi_rs_present) {
    HANDLE_CODE(unpack_integer(csi_rs, bref, (uint8_t)0u, (uint8_t)191u));
  }
  if (slot_offset_present) {
    HANDLE_CODE(unpack_integer(slot_offset, bref, (uint8_t)1u, (uint8_t)32u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool aperiodic_srs_res_trigger_list_present;
      HANDLE_CODE(bref.unpack(aperiodic_srs_res_trigger_list_present, 1));
      aperiodic_srs_res_trigger_list.set_present(aperiodic_srs_res_trigger_list_present);
      if (aperiodic_srs_res_trigger_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*aperiodic_srs_res_trigger_list, bref, 1, 2, integer_packer<uint8_t>(1, 3)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srs_res_set_s::res_type_c_::aperiodic_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("aperiodicSRS-ResourceTrigger", aperiodic_srs_res_trigger);
  if (csi_rs_present) {
    j.write_int("csi-RS", csi_rs);
  }
  if (slot_offset_present) {
    j.write_int("slotOffset", slot_offset);
  }
  if (ext) {
    if (aperiodic_srs_res_trigger_list.is_present()) {
      j.start_array("aperiodicSRS-ResourceTriggerList");
      for (const auto& e1 : *aperiodic_srs_res_trigger_list) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* srs_res_set_s::usage_opts::to_string() const
{
  static const char* names[] = {"beamManagement", "codebook", "nonCodebook", "antennaSwitching"};
  return convert_enum_idx(names, 4, value, "srs_res_set_s::usage_e_");
}

void srs_res_set_s::pathloss_ref_rs_c_::destroy_() {}
void srs_res_set_s::pathloss_ref_rs_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
srs_res_set_s::pathloss_ref_rs_c_::pathloss_ref_rs_c_(const srs_res_set_s::pathloss_ref_rs_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::pathloss_ref_rs_c_");
  }
}
srs_res_set_s::pathloss_ref_rs_c_&
srs_res_set_s::pathloss_ref_rs_c_::operator=(const srs_res_set_s::pathloss_ref_rs_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::pathloss_ref_rs_c_");
  }

  return *this;
}
uint8_t& srs_res_set_s::pathloss_ref_rs_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& srs_res_set_s::pathloss_ref_rs_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
void srs_res_set_s::pathloss_ref_rs_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_idx:
      j.write_int("ssb-Index", c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      j.write_int("csi-RS-Index", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::pathloss_ref_rs_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_res_set_s::pathloss_ref_rs_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::pathloss_ref_rs_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_set_s::pathloss_ref_rs_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)191u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_set_s::pathloss_ref_rs_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* srs_res_set_s::srs_pwr_ctrl_adjustment_states_opts::to_string() const
{
  static const char* names[] = {"sameAsFci2", "separateClosedLoop"};
  return convert_enum_idx(names, 2, value, "srs_res_set_s::srs_pwr_ctrl_adjustment_states_e_");
}
uint8_t srs_res_set_s::srs_pwr_ctrl_adjustment_states_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "srs_res_set_s::srs_pwr_ctrl_adjustment_states_e_");
}

// SchedulingRequestResourceConfig ::= SEQUENCE
SRSASN_CODE sched_request_res_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(periodicity_and_offset_present, 1));
  HANDLE_CODE(bref.pack(res_present, 1));

  HANDLE_CODE(pack_integer(bref, sched_request_res_id, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, sched_request_id, (uint8_t)0u, (uint8_t)7u));
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.pack(bref));
  }
  if (res_present) {
    HANDLE_CODE(pack_integer(bref, res, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_res_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(periodicity_and_offset_present, 1));
  HANDLE_CODE(bref.unpack(res_present, 1));

  HANDLE_CODE(unpack_integer(sched_request_res_id, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(sched_request_id, bref, (uint8_t)0u, (uint8_t)7u));
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.unpack(bref));
  }
  if (res_present) {
    HANDLE_CODE(unpack_integer(res, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void sched_request_res_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("schedulingRequestResourceId", sched_request_res_id);
  j.write_int("schedulingRequestID", sched_request_id);
  if (periodicity_and_offset_present) {
    j.write_fieldname("periodicityAndOffset");
    periodicity_and_offset.to_json(j);
  }
  if (res_present) {
    j.write_int("resource", res);
  }
  j.end_obj();
}

void sched_request_res_cfg_s::periodicity_and_offset_c_::destroy_() {}
void sched_request_res_cfg_s::periodicity_and_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sched_request_res_cfg_s::periodicity_and_offset_c_::periodicity_and_offset_c_(
    const sched_request_res_cfg_s::periodicity_and_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sym2:
      break;
    case types::sym6or7:
      break;
    case types::sl1:
      break;
    case types::sl2:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl4:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl5:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl8:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_res_cfg_s::periodicity_and_offset_c_");
  }
}
sched_request_res_cfg_s::periodicity_and_offset_c_& sched_request_res_cfg_s::periodicity_and_offset_c_::operator=(
    const sched_request_res_cfg_s::periodicity_and_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sym2:
      break;
    case types::sym6or7:
      break;
    case types::sl1:
      break;
    case types::sl2:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl4:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl5:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl8:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_res_cfg_s::periodicity_and_offset_c_");
  }

  return *this;
}
void sched_request_res_cfg_s::periodicity_and_offset_c_::set_sym2()
{
  set(types::sym2);
}
void sched_request_res_cfg_s::periodicity_and_offset_c_::set_sym6or7()
{
  set(types::sym6or7);
}
void sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl1()
{
  set(types::sl1);
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl2()
{
  set(types::sl2);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl4()
{
  set(types::sl4);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl5()
{
  set(types::sl5);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl8()
{
  set(types::sl8);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl10()
{
  set(types::sl10);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl16()
{
  set(types::sl16);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl20()
{
  set(types::sl20);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl40()
{
  set(types::sl40);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl80()
{
  set(types::sl80);
  return c.get<uint8_t>();
}
uint8_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl160()
{
  set(types::sl160);
  return c.get<uint8_t>();
}
uint16_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl320()
{
  set(types::sl320);
  return c.get<uint16_t>();
}
uint16_t& sched_request_res_cfg_s::periodicity_and_offset_c_::set_sl640()
{
  set(types::sl640);
  return c.get<uint16_t>();
}
void sched_request_res_cfg_s::periodicity_and_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sym2:
      break;
    case types::sym6or7:
      break;
    case types::sl1:
      break;
    case types::sl2:
      j.write_int("sl2", c.get<uint8_t>());
      break;
    case types::sl4:
      j.write_int("sl4", c.get<uint8_t>());
      break;
    case types::sl5:
      j.write_int("sl5", c.get<uint8_t>());
      break;
    case types::sl8:
      j.write_int("sl8", c.get<uint8_t>());
      break;
    case types::sl10:
      j.write_int("sl10", c.get<uint8_t>());
      break;
    case types::sl16:
      j.write_int("sl16", c.get<uint8_t>());
      break;
    case types::sl20:
      j.write_int("sl20", c.get<uint8_t>());
      break;
    case types::sl40:
      j.write_int("sl40", c.get<uint8_t>());
      break;
    case types::sl80:
      j.write_int("sl80", c.get<uint8_t>());
      break;
    case types::sl160:
      j.write_int("sl160", c.get<uint8_t>());
      break;
    case types::sl320:
      j.write_int("sl320", c.get<uint16_t>());
      break;
    case types::sl640:
      j.write_int("sl640", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_res_cfg_s::periodicity_and_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE sched_request_res_cfg_s::periodicity_and_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sym2:
      break;
    case types::sym6or7:
      break;
    case types::sl1:
      break;
    case types::sl2:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::sl4:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::sl5:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sl8:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::sl10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sl16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sl20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sl40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sl160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sl320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sl640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_res_cfg_s::periodicity_and_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_request_res_cfg_s::periodicity_and_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sym2:
      break;
    case types::sym6or7:
      break;
    case types::sl1:
      break;
    case types::sl2:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::sl4:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::sl5:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::sl8:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::sl10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sl16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::sl20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sl40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sl160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sl320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sl640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    default:
      log_invalid_choice_id(type_, "sched_request_res_cfg_s::periodicity_and_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UCI-OnPUSCH ::= SEQUENCE
SRSASN_CODE uci_on_pusch_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(beta_offsets_present, 1));

  if (beta_offsets_present) {
    HANDLE_CODE(beta_offsets.pack(bref));
  }
  HANDLE_CODE(scaling.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE uci_on_pusch_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(beta_offsets_present, 1));

  if (beta_offsets_present) {
    HANDLE_CODE(beta_offsets.unpack(bref));
  }
  HANDLE_CODE(scaling.unpack(bref));

  return SRSASN_SUCCESS;
}
void uci_on_pusch_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (beta_offsets_present) {
    j.write_fieldname("betaOffsets");
    beta_offsets.to_json(j);
  }
  j.write_str("scaling", scaling.to_string());
  j.end_obj();
}

void uci_on_pusch_s::beta_offsets_c_::destroy_()
{
  switch (type_) {
    case types::dyn:
      c.destroy<dyn_l_>();
      break;
    case types::semi_static:
      c.destroy<beta_offsets_s>();
      break;
    default:
      break;
  }
}
void uci_on_pusch_s::beta_offsets_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::dyn:
      c.init<dyn_l_>();
      break;
    case types::semi_static:
      c.init<beta_offsets_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uci_on_pusch_s::beta_offsets_c_");
  }
}
uci_on_pusch_s::beta_offsets_c_::beta_offsets_c_(const uci_on_pusch_s::beta_offsets_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::dyn:
      c.init(other.c.get<dyn_l_>());
      break;
    case types::semi_static:
      c.init(other.c.get<beta_offsets_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uci_on_pusch_s::beta_offsets_c_");
  }
}
uci_on_pusch_s::beta_offsets_c_&
uci_on_pusch_s::beta_offsets_c_::operator=(const uci_on_pusch_s::beta_offsets_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::dyn:
      c.set(other.c.get<dyn_l_>());
      break;
    case types::semi_static:
      c.set(other.c.get<beta_offsets_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uci_on_pusch_s::beta_offsets_c_");
  }

  return *this;
}
uci_on_pusch_s::beta_offsets_c_::dyn_l_& uci_on_pusch_s::beta_offsets_c_::set_dyn()
{
  set(types::dyn);
  return c.get<dyn_l_>();
}
beta_offsets_s& uci_on_pusch_s::beta_offsets_c_::set_semi_static()
{
  set(types::semi_static);
  return c.get<beta_offsets_s>();
}
void uci_on_pusch_s::beta_offsets_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dyn:
      j.start_array("dynamic");
      for (const auto& e1 : c.get<dyn_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::semi_static:
      j.write_fieldname("semiStatic");
      c.get<beta_offsets_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "uci_on_pusch_s::beta_offsets_c_");
  }
  j.end_obj();
}
SRSASN_CODE uci_on_pusch_s::beta_offsets_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dyn:
      HANDLE_CODE(pack_fixed_seq_of(bref, &(c.get<dyn_l_>())[0], c.get<dyn_l_>().size()));
      break;
    case types::semi_static:
      HANDLE_CODE(c.get<beta_offsets_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "uci_on_pusch_s::beta_offsets_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE uci_on_pusch_s::beta_offsets_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dyn:
      HANDLE_CODE(unpack_fixed_seq_of(&(c.get<dyn_l_>())[0], bref, c.get<dyn_l_>().size()));
      break;
    case types::semi_static:
      HANDLE_CODE(c.get<beta_offsets_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "uci_on_pusch_s::beta_offsets_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* uci_on_pusch_s::scaling_opts::to_string() const
{
  static const char* names[] = {"f0p5", "f0p65", "f0p8", "f1"};
  return convert_enum_idx(names, 4, value, "uci_on_pusch_s::scaling_e_");
}

// BeamFailureRecoveryConfig ::= SEQUENCE
SRSASN_CODE beam_fail_recovery_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(root_seq_idx_bfr_present, 1));
  HANDLE_CODE(bref.pack(rach_cfg_bfr_present, 1));
  HANDLE_CODE(bref.pack(rsrp_thres_ssb_present, 1));
  HANDLE_CODE(bref.pack(candidate_beam_rs_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ssb_per_rach_occasion_present, 1));
  HANDLE_CODE(bref.pack(ra_ssb_occasion_mask_idx_present, 1));
  HANDLE_CODE(bref.pack(recovery_search_space_id_present, 1));
  HANDLE_CODE(bref.pack(ra_prioritization_present, 1));
  HANDLE_CODE(bref.pack(beam_fail_recovery_timer_present, 1));

  if (root_seq_idx_bfr_present) {
    HANDLE_CODE(pack_integer(bref, root_seq_idx_bfr, (uint8_t)0u, (uint8_t)137u));
  }
  if (rach_cfg_bfr_present) {
    HANDLE_CODE(rach_cfg_bfr.pack(bref));
  }
  if (rsrp_thres_ssb_present) {
    HANDLE_CODE(pack_integer(bref, rsrp_thres_ssb, (uint8_t)0u, (uint8_t)127u));
  }
  if (candidate_beam_rs_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_beam_rs_list, 1, 16));
  }
  if (ssb_per_rach_occasion_present) {
    HANDLE_CODE(ssb_per_rach_occasion.pack(bref));
  }
  if (ra_ssb_occasion_mask_idx_present) {
    HANDLE_CODE(pack_integer(bref, ra_ssb_occasion_mask_idx, (uint8_t)0u, (uint8_t)15u));
  }
  if (recovery_search_space_id_present) {
    HANDLE_CODE(pack_integer(bref, recovery_search_space_id, (uint8_t)0u, (uint8_t)39u));
  }
  if (ra_prioritization_present) {
    HANDLE_CODE(ra_prioritization.pack(bref));
  }
  if (beam_fail_recovery_timer_present) {
    HANDLE_CODE(beam_fail_recovery_timer.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= msg1_subcarrier_spacing_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(msg1_subcarrier_spacing_present, 1));
      if (msg1_subcarrier_spacing_present) {
        HANDLE_CODE(msg1_subcarrier_spacing.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE beam_fail_recovery_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(root_seq_idx_bfr_present, 1));
  HANDLE_CODE(bref.unpack(rach_cfg_bfr_present, 1));
  HANDLE_CODE(bref.unpack(rsrp_thres_ssb_present, 1));
  bool candidate_beam_rs_list_present;
  HANDLE_CODE(bref.unpack(candidate_beam_rs_list_present, 1));
  HANDLE_CODE(bref.unpack(ssb_per_rach_occasion_present, 1));
  HANDLE_CODE(bref.unpack(ra_ssb_occasion_mask_idx_present, 1));
  HANDLE_CODE(bref.unpack(recovery_search_space_id_present, 1));
  HANDLE_CODE(bref.unpack(ra_prioritization_present, 1));
  HANDLE_CODE(bref.unpack(beam_fail_recovery_timer_present, 1));

  if (root_seq_idx_bfr_present) {
    HANDLE_CODE(unpack_integer(root_seq_idx_bfr, bref, (uint8_t)0u, (uint8_t)137u));
  }
  if (rach_cfg_bfr_present) {
    HANDLE_CODE(rach_cfg_bfr.unpack(bref));
  }
  if (rsrp_thres_ssb_present) {
    HANDLE_CODE(unpack_integer(rsrp_thres_ssb, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (candidate_beam_rs_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_beam_rs_list, bref, 1, 16));
  }
  if (ssb_per_rach_occasion_present) {
    HANDLE_CODE(ssb_per_rach_occasion.unpack(bref));
  }
  if (ra_ssb_occasion_mask_idx_present) {
    HANDLE_CODE(unpack_integer(ra_ssb_occasion_mask_idx, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (recovery_search_space_id_present) {
    HANDLE_CODE(unpack_integer(recovery_search_space_id, bref, (uint8_t)0u, (uint8_t)39u));
  }
  if (ra_prioritization_present) {
    HANDLE_CODE(ra_prioritization.unpack(bref));
  }
  if (beam_fail_recovery_timer_present) {
    HANDLE_CODE(beam_fail_recovery_timer.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(msg1_subcarrier_spacing_present, 1));
      if (msg1_subcarrier_spacing_present) {
        HANDLE_CODE(msg1_subcarrier_spacing.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void beam_fail_recovery_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (root_seq_idx_bfr_present) {
    j.write_int("rootSequenceIndex-BFR", root_seq_idx_bfr);
  }
  if (rach_cfg_bfr_present) {
    j.write_fieldname("rach-ConfigBFR");
    rach_cfg_bfr.to_json(j);
  }
  if (rsrp_thres_ssb_present) {
    j.write_int("rsrp-ThresholdSSB", rsrp_thres_ssb);
  }
  if (candidate_beam_rs_list.size() > 0) {
    j.start_array("candidateBeamRSList");
    for (const auto& e1 : candidate_beam_rs_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ssb_per_rach_occasion_present) {
    j.write_str("ssb-perRACH-Occasion", ssb_per_rach_occasion.to_string());
  }
  if (ra_ssb_occasion_mask_idx_present) {
    j.write_int("ra-ssb-OccasionMaskIndex", ra_ssb_occasion_mask_idx);
  }
  if (recovery_search_space_id_present) {
    j.write_int("recoverySearchSpaceId", recovery_search_space_id);
  }
  if (ra_prioritization_present) {
    j.write_fieldname("ra-Prioritization");
    ra_prioritization.to_json(j);
  }
  if (beam_fail_recovery_timer_present) {
    j.write_str("beamFailureRecoveryTimer", beam_fail_recovery_timer.to_string());
  }
  if (ext) {
    if (msg1_subcarrier_spacing_present) {
      j.write_str("msg1-SubcarrierSpacing", msg1_subcarrier_spacing.to_string());
    }
  }
  j.end_obj();
}

const char* beam_fail_recovery_cfg_s::ssb_per_rach_occasion_opts::to_string() const
{
  static const char* names[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(names, 8, value, "beam_fail_recovery_cfg_s::ssb_per_rach_occasion_e_");
}
float beam_fail_recovery_cfg_s::ssb_per_rach_occasion_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(numbers, 8, value, "beam_fail_recovery_cfg_s::ssb_per_rach_occasion_e_");
}
const char* beam_fail_recovery_cfg_s::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(number_strs, 8, value, "beam_fail_recovery_cfg_s::ssb_per_rach_occasion_e_");
}

const char* beam_fail_recovery_cfg_s::beam_fail_recovery_timer_opts::to_string() const
{
  static const char* names[] = {"ms10", "ms20", "ms40", "ms60", "ms80", "ms100", "ms150", "ms200"};
  return convert_enum_idx(names, 8, value, "beam_fail_recovery_cfg_s::beam_fail_recovery_timer_e_");
}
uint8_t beam_fail_recovery_cfg_s::beam_fail_recovery_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 60, 80, 100, 150, 200};
  return map_enum_number(numbers, 8, value, "beam_fail_recovery_cfg_s::beam_fail_recovery_timer_e_");
}

// ConfiguredGrantConfig ::= SEQUENCE
SRSASN_CODE cfg_grant_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_hop_present, 1));
  HANDLE_CODE(bref.pack(mcs_table_present, 1));
  HANDLE_CODE(bref.pack(mcs_table_transform_precoder_present, 1));
  HANDLE_CODE(bref.pack(uci_on_pusch_present, 1));
  HANDLE_CODE(bref.pack(rbg_size_present, 1));
  HANDLE_CODE(bref.pack(transform_precoder_present, 1));
  HANDLE_CODE(bref.pack(rep_k_rv_present, 1));
  HANDLE_CODE(bref.pack(cfg_grant_timer_present, 1));
  HANDLE_CODE(bref.pack(rrc_cfg_ul_grant_present, 1));

  if (freq_hop_present) {
    HANDLE_CODE(freq_hop.pack(bref));
  }
  HANDLE_CODE(cg_dmrs_cfg.pack(bref));
  if (mcs_table_present) {
    HANDLE_CODE(mcs_table.pack(bref));
  }
  if (mcs_table_transform_precoder_present) {
    HANDLE_CODE(mcs_table_transform_precoder.pack(bref));
  }
  if (uci_on_pusch_present) {
    HANDLE_CODE(uci_on_pusch.pack(bref));
  }
  HANDLE_CODE(res_alloc.pack(bref));
  HANDLE_CODE(pwr_ctrl_loop_to_use.pack(bref));
  HANDLE_CODE(pack_integer(bref, p0_pusch_alpha, (uint8_t)0u, (uint8_t)29u));
  if (transform_precoder_present) {
    HANDLE_CODE(transform_precoder.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, nrof_harq_processes, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(rep_k.pack(bref));
  if (rep_k_rv_present) {
    HANDLE_CODE(rep_k_rv.pack(bref));
  }
  HANDLE_CODE(periodicity.pack(bref));
  if (cfg_grant_timer_present) {
    HANDLE_CODE(pack_integer(bref, cfg_grant_timer, (uint8_t)1u, (uint8_t)64u));
  }
  if (rrc_cfg_ul_grant_present) {
    bref.pack(rrc_cfg_ul_grant.ext, 1);
    HANDLE_CODE(bref.pack(rrc_cfg_ul_grant.dmrs_seq_initization_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_ul_grant.srs_res_ind_present, 1));
    HANDLE_CODE(bref.pack(rrc_cfg_ul_grant.freq_hop_offset_present, 1));
    HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.time_domain_offset, (uint16_t)0u, (uint16_t)5119u));
    HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.time_domain_alloc, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(rrc_cfg_ul_grant.freq_domain_alloc.pack(bref));
    HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.ant_port, (uint8_t)0u, (uint8_t)31u));
    if (rrc_cfg_ul_grant.dmrs_seq_initization_present) {
      HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.dmrs_seq_initization, (uint8_t)0u, (uint8_t)1u));
    }
    HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.precoding_and_nof_layers, (uint8_t)0u, (uint8_t)63u));
    if (rrc_cfg_ul_grant.srs_res_ind_present) {
      HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.srs_res_ind, (uint8_t)0u, (uint8_t)15u));
    }
    HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.mcs_and_tbs, (uint8_t)0u, (uint8_t)31u));
    if (rrc_cfg_ul_grant.freq_hop_offset_present) {
      HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.freq_hop_offset, (uint16_t)1u, (uint16_t)274u));
    }
    HANDLE_CODE(pack_integer(bref, rrc_cfg_ul_grant.pathloss_ref_idx, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_grant_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_hop_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_transform_precoder_present, 1));
  HANDLE_CODE(bref.unpack(uci_on_pusch_present, 1));
  HANDLE_CODE(bref.unpack(rbg_size_present, 1));
  HANDLE_CODE(bref.unpack(transform_precoder_present, 1));
  HANDLE_CODE(bref.unpack(rep_k_rv_present, 1));
  HANDLE_CODE(bref.unpack(cfg_grant_timer_present, 1));
  HANDLE_CODE(bref.unpack(rrc_cfg_ul_grant_present, 1));

  if (freq_hop_present) {
    HANDLE_CODE(freq_hop.unpack(bref));
  }
  HANDLE_CODE(cg_dmrs_cfg.unpack(bref));
  if (mcs_table_present) {
    HANDLE_CODE(mcs_table.unpack(bref));
  }
  if (mcs_table_transform_precoder_present) {
    HANDLE_CODE(mcs_table_transform_precoder.unpack(bref));
  }
  if (uci_on_pusch_present) {
    HANDLE_CODE(uci_on_pusch.unpack(bref));
  }
  HANDLE_CODE(res_alloc.unpack(bref));
  HANDLE_CODE(pwr_ctrl_loop_to_use.unpack(bref));
  HANDLE_CODE(unpack_integer(p0_pusch_alpha, bref, (uint8_t)0u, (uint8_t)29u));
  if (transform_precoder_present) {
    HANDLE_CODE(transform_precoder.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(nrof_harq_processes, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(rep_k.unpack(bref));
  if (rep_k_rv_present) {
    HANDLE_CODE(rep_k_rv.unpack(bref));
  }
  HANDLE_CODE(periodicity.unpack(bref));
  if (cfg_grant_timer_present) {
    HANDLE_CODE(unpack_integer(cfg_grant_timer, bref, (uint8_t)1u, (uint8_t)64u));
  }
  if (rrc_cfg_ul_grant_present) {
    bref.unpack(rrc_cfg_ul_grant.ext, 1);
    HANDLE_CODE(bref.unpack(rrc_cfg_ul_grant.dmrs_seq_initization_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_ul_grant.srs_res_ind_present, 1));
    HANDLE_CODE(bref.unpack(rrc_cfg_ul_grant.freq_hop_offset_present, 1));
    HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.time_domain_offset, bref, (uint16_t)0u, (uint16_t)5119u));
    HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.time_domain_alloc, bref, (uint8_t)0u, (uint8_t)15u));
    HANDLE_CODE(rrc_cfg_ul_grant.freq_domain_alloc.unpack(bref));
    HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.ant_port, bref, (uint8_t)0u, (uint8_t)31u));
    if (rrc_cfg_ul_grant.dmrs_seq_initization_present) {
      HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.dmrs_seq_initization, bref, (uint8_t)0u, (uint8_t)1u));
    }
    HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.precoding_and_nof_layers, bref, (uint8_t)0u, (uint8_t)63u));
    if (rrc_cfg_ul_grant.srs_res_ind_present) {
      HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.srs_res_ind, bref, (uint8_t)0u, (uint8_t)15u));
    }
    HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.mcs_and_tbs, bref, (uint8_t)0u, (uint8_t)31u));
    if (rrc_cfg_ul_grant.freq_hop_offset_present) {
      HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.freq_hop_offset, bref, (uint16_t)1u, (uint16_t)274u));
    }
    HANDLE_CODE(unpack_integer(rrc_cfg_ul_grant.pathloss_ref_idx, bref, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
void cfg_grant_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_hop_present) {
    j.write_str("frequencyHopping", freq_hop.to_string());
  }
  j.write_fieldname("cg-DMRS-Configuration");
  cg_dmrs_cfg.to_json(j);
  if (mcs_table_present) {
    j.write_str("mcs-Table", mcs_table.to_string());
  }
  if (mcs_table_transform_precoder_present) {
    j.write_str("mcs-TableTransformPrecoder", mcs_table_transform_precoder.to_string());
  }
  if (uci_on_pusch_present) {
    j.write_fieldname("uci-OnPUSCH");
    uci_on_pusch.to_json(j);
  }
  j.write_str("resourceAllocation", res_alloc.to_string());
  if (rbg_size_present) {
    j.write_str("rbg-Size", "config2");
  }
  j.write_str("powerControlLoopToUse", pwr_ctrl_loop_to_use.to_string());
  j.write_int("p0-PUSCH-Alpha", p0_pusch_alpha);
  if (transform_precoder_present) {
    j.write_str("transformPrecoder", transform_precoder.to_string());
  }
  j.write_int("nrofHARQ-Processes", nrof_harq_processes);
  j.write_str("repK", rep_k.to_string());
  if (rep_k_rv_present) {
    j.write_str("repK-RV", rep_k_rv.to_string());
  }
  j.write_str("periodicity", periodicity.to_string());
  if (cfg_grant_timer_present) {
    j.write_int("configuredGrantTimer", cfg_grant_timer);
  }
  if (rrc_cfg_ul_grant_present) {
    j.write_fieldname("rrc-ConfiguredUplinkGrant");
    j.start_obj();
    j.write_int("timeDomainOffset", rrc_cfg_ul_grant.time_domain_offset);
    j.write_int("timeDomainAllocation", rrc_cfg_ul_grant.time_domain_alloc);
    j.write_str("frequencyDomainAllocation", rrc_cfg_ul_grant.freq_domain_alloc.to_string());
    j.write_int("antennaPort", rrc_cfg_ul_grant.ant_port);
    if (rrc_cfg_ul_grant.dmrs_seq_initization_present) {
      j.write_int("dmrs-SeqInitialization", rrc_cfg_ul_grant.dmrs_seq_initization);
    }
    j.write_int("precodingAndNumberOfLayers", rrc_cfg_ul_grant.precoding_and_nof_layers);
    if (rrc_cfg_ul_grant.srs_res_ind_present) {
      j.write_int("srs-ResourceIndicator", rrc_cfg_ul_grant.srs_res_ind);
    }
    j.write_int("mcsAndTBS", rrc_cfg_ul_grant.mcs_and_tbs);
    if (rrc_cfg_ul_grant.freq_hop_offset_present) {
      j.write_int("frequencyHoppingOffset", rrc_cfg_ul_grant.freq_hop_offset);
    }
    j.write_int("pathlossReferenceIndex", rrc_cfg_ul_grant.pathloss_ref_idx);
    j.end_obj();
  }
  j.end_obj();
}

const char* cfg_grant_cfg_s::freq_hop_opts::to_string() const
{
  static const char* names[] = {"intraSlot", "interSlot"};
  return convert_enum_idx(names, 2, value, "cfg_grant_cfg_s::freq_hop_e_");
}

const char* cfg_grant_cfg_s::mcs_table_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE"};
  return convert_enum_idx(names, 2, value, "cfg_grant_cfg_s::mcs_table_e_");
}
uint16_t cfg_grant_cfg_s::mcs_table_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "cfg_grant_cfg_s::mcs_table_e_");
}

const char* cfg_grant_cfg_s::mcs_table_transform_precoder_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE"};
  return convert_enum_idx(names, 2, value, "cfg_grant_cfg_s::mcs_table_transform_precoder_e_");
}
uint16_t cfg_grant_cfg_s::mcs_table_transform_precoder_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "cfg_grant_cfg_s::mcs_table_transform_precoder_e_");
}

const char* cfg_grant_cfg_s::res_alloc_opts::to_string() const
{
  static const char* names[] = {"resourceAllocationType0", "resourceAllocationType1", "dynamicSwitch"};
  return convert_enum_idx(names, 3, value, "cfg_grant_cfg_s::res_alloc_e_");
}
uint8_t cfg_grant_cfg_s::res_alloc_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "cfg_grant_cfg_s::res_alloc_e_");
}

const char* cfg_grant_cfg_s::pwr_ctrl_loop_to_use_opts::to_string() const
{
  static const char* names[] = {"n0", "n1"};
  return convert_enum_idx(names, 2, value, "cfg_grant_cfg_s::pwr_ctrl_loop_to_use_e_");
}
uint8_t cfg_grant_cfg_s::pwr_ctrl_loop_to_use_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "cfg_grant_cfg_s::pwr_ctrl_loop_to_use_e_");
}

const char* cfg_grant_cfg_s::transform_precoder_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "cfg_grant_cfg_s::transform_precoder_e_");
}

const char* cfg_grant_cfg_s::rep_k_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(names, 4, value, "cfg_grant_cfg_s::rep_k_e_");
}
uint8_t cfg_grant_cfg_s::rep_k_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "cfg_grant_cfg_s::rep_k_e_");
}

const char* cfg_grant_cfg_s::rep_k_rv_opts::to_string() const
{
  static const char* names[] = {"s1-0231", "s2-0303", "s3-0000"};
  return convert_enum_idx(names, 3, value, "cfg_grant_cfg_s::rep_k_rv_e_");
}
uint8_t cfg_grant_cfg_s::rep_k_rv_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "cfg_grant_cfg_s::rep_k_rv_e_");
}

const char* cfg_grant_cfg_s::periodicity_opts::to_string() const
{
  static const char* names[] = {
      "sym2",      "sym7",      "sym1x14",   "sym2x14",    "sym4x14",    "sym5x14",    "sym8x14",    "sym10x14",
      "sym16x14",  "sym20x14",  "sym32x14",  "sym40x14",   "sym64x14",   "sym80x14",   "sym128x14",  "sym160x14",
      "sym256x14", "sym320x14", "sym512x14", "sym640x14",  "sym1024x14", "sym1280x14", "sym2560x14", "sym5120x14",
      "sym6",      "sym1x12",   "sym2x12",   "sym4x12",    "sym5x12",    "sym8x12",    "sym10x12",   "sym16x12",
      "sym20x12",  "sym32x12",  "sym40x12",  "sym64x12",   "sym80x12",   "sym128x12",  "sym160x12",  "sym256x12",
      "sym320x12", "sym512x12", "sym640x12", "sym1280x12", "sym2560x12"};
  return convert_enum_idx(names, 45, value, "cfg_grant_cfg_s::periodicity_e_");
}

// PUCCH-Config ::= SEQUENCE
SRSASN_CODE pucch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(res_set_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(res_set_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(res_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(res_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(format1_present, 1));
  HANDLE_CODE(bref.pack(format2_present, 1));
  HANDLE_CODE(bref.pack(format3_present, 1));
  HANDLE_CODE(bref.pack(format4_present, 1));
  HANDLE_CODE(bref.pack(sched_request_res_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(sched_request_res_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(multi_csi_pucch_res_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(dl_data_to_ul_ack.size() > 0, 1));
  HANDLE_CODE(bref.pack(spatial_relation_info_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(spatial_relation_info_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(pucch_pwr_ctrl_present, 1));

  if (res_set_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, res_set_to_add_mod_list, 1, 4));
  }
  if (res_set_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, res_set_to_release_list, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (res_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, res_to_add_mod_list, 1, 128));
  }
  if (res_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, res_to_release_list, 1, 128, integer_packer<uint8_t>(0, 127)));
  }
  if (format1_present) {
    HANDLE_CODE(format1.pack(bref));
  }
  if (format2_present) {
    HANDLE_CODE(format2.pack(bref));
  }
  if (format3_present) {
    HANDLE_CODE(format3.pack(bref));
  }
  if (format4_present) {
    HANDLE_CODE(format4.pack(bref));
  }
  if (sched_request_res_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_request_res_to_add_mod_list, 1, 8));
  }
  if (sched_request_res_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sched_request_res_to_release_list, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (multi_csi_pucch_res_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, multi_csi_pucch_res_list, 1, 2, integer_packer<uint8_t>(0, 127)));
  }
  if (dl_data_to_ul_ack.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_data_to_ul_ack, 1, 8, integer_packer<uint8_t>(0, 15)));
  }
  if (spatial_relation_info_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, spatial_relation_info_to_add_mod_list, 1, 8));
  }
  if (spatial_relation_info_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, spatial_relation_info_to_release_list, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (pucch_pwr_ctrl_present) {
    HANDLE_CODE(pucch_pwr_ctrl.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool res_set_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(res_set_to_add_mod_list_present, 1));
  bool res_set_to_release_list_present;
  HANDLE_CODE(bref.unpack(res_set_to_release_list_present, 1));
  bool res_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(res_to_add_mod_list_present, 1));
  bool res_to_release_list_present;
  HANDLE_CODE(bref.unpack(res_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(format1_present, 1));
  HANDLE_CODE(bref.unpack(format2_present, 1));
  HANDLE_CODE(bref.unpack(format3_present, 1));
  HANDLE_CODE(bref.unpack(format4_present, 1));
  bool sched_request_res_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(sched_request_res_to_add_mod_list_present, 1));
  bool sched_request_res_to_release_list_present;
  HANDLE_CODE(bref.unpack(sched_request_res_to_release_list_present, 1));
  bool multi_csi_pucch_res_list_present;
  HANDLE_CODE(bref.unpack(multi_csi_pucch_res_list_present, 1));
  bool dl_data_to_ul_ack_present;
  HANDLE_CODE(bref.unpack(dl_data_to_ul_ack_present, 1));
  bool spatial_relation_info_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(spatial_relation_info_to_add_mod_list_present, 1));
  bool spatial_relation_info_to_release_list_present;
  HANDLE_CODE(bref.unpack(spatial_relation_info_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(pucch_pwr_ctrl_present, 1));

  if (res_set_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(res_set_to_add_mod_list, bref, 1, 4));
  }
  if (res_set_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(res_set_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 3)));
  }
  if (res_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(res_to_add_mod_list, bref, 1, 128));
  }
  if (res_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(res_to_release_list, bref, 1, 128, integer_packer<uint8_t>(0, 127)));
  }
  if (format1_present) {
    HANDLE_CODE(format1.unpack(bref));
  }
  if (format2_present) {
    HANDLE_CODE(format2.unpack(bref));
  }
  if (format3_present) {
    HANDLE_CODE(format3.unpack(bref));
  }
  if (format4_present) {
    HANDLE_CODE(format4.unpack(bref));
  }
  if (sched_request_res_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_request_res_to_add_mod_list, bref, 1, 8));
  }
  if (sched_request_res_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sched_request_res_to_release_list, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (multi_csi_pucch_res_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(multi_csi_pucch_res_list, bref, 1, 2, integer_packer<uint8_t>(0, 127)));
  }
  if (dl_data_to_ul_ack_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_data_to_ul_ack, bref, 1, 8, integer_packer<uint8_t>(0, 15)));
  }
  if (spatial_relation_info_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(spatial_relation_info_to_add_mod_list, bref, 1, 8));
  }
  if (spatial_relation_info_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(spatial_relation_info_to_release_list, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
  }
  if (pucch_pwr_ctrl_present) {
    HANDLE_CODE(pucch_pwr_ctrl.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (res_set_to_add_mod_list.size() > 0) {
    j.start_array("resourceSetToAddModList");
    for (const auto& e1 : res_set_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (res_set_to_release_list.size() > 0) {
    j.start_array("resourceSetToReleaseList");
    for (const auto& e1 : res_set_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (res_to_add_mod_list.size() > 0) {
    j.start_array("resourceToAddModList");
    for (const auto& e1 : res_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (res_to_release_list.size() > 0) {
    j.start_array("resourceToReleaseList");
    for (const auto& e1 : res_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (format1_present) {
    j.write_fieldname("format1");
    format1.to_json(j);
  }
  if (format2_present) {
    j.write_fieldname("format2");
    format2.to_json(j);
  }
  if (format3_present) {
    j.write_fieldname("format3");
    format3.to_json(j);
  }
  if (format4_present) {
    j.write_fieldname("format4");
    format4.to_json(j);
  }
  if (sched_request_res_to_add_mod_list.size() > 0) {
    j.start_array("schedulingRequestResourceToAddModList");
    for (const auto& e1 : sched_request_res_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sched_request_res_to_release_list.size() > 0) {
    j.start_array("schedulingRequestResourceToReleaseList");
    for (const auto& e1 : sched_request_res_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (multi_csi_pucch_res_list.size() > 0) {
    j.start_array("multi-CSI-PUCCH-ResourceList");
    for (const auto& e1 : multi_csi_pucch_res_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (dl_data_to_ul_ack.size() > 0) {
    j.start_array("dl-DataToUL-ACK");
    for (const auto& e1 : dl_data_to_ul_ack) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (spatial_relation_info_to_add_mod_list.size() > 0) {
    j.start_array("spatialRelationInfoToAddModList");
    for (const auto& e1 : spatial_relation_info_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (spatial_relation_info_to_release_list.size() > 0) {
    j.start_array("spatialRelationInfoToReleaseList");
    for (const auto& e1 : spatial_relation_info_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (pucch_pwr_ctrl_present) {
    j.write_fieldname("pucch-PowerControl");
    pucch_pwr_ctrl.to_json(j);
  }
  j.end_obj();
}

// PUSCH-Config ::= SEQUENCE
SRSASN_CODE pusch_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_scrambling_id_pusch_present, 1));
  HANDLE_CODE(bref.pack(tx_cfg_present, 1));
  HANDLE_CODE(bref.pack(dmrs_ul_for_pusch_map_type_a_present, 1));
  HANDLE_CODE(bref.pack(dmrs_ul_for_pusch_map_type_b_present, 1));
  HANDLE_CODE(bref.pack(pusch_pwr_ctrl_present, 1));
  HANDLE_CODE(bref.pack(freq_hop_present, 1));
  HANDLE_CODE(bref.pack(freq_hop_offset_lists.size() > 0, 1));
  HANDLE_CODE(bref.pack(pusch_time_domain_alloc_list_present, 1));
  HANDLE_CODE(bref.pack(pusch_aggregation_factor_present, 1));
  HANDLE_CODE(bref.pack(mcs_table_present, 1));
  HANDLE_CODE(bref.pack(mcs_table_transform_precoder_present, 1));
  HANDLE_CODE(bref.pack(transform_precoder_present, 1));
  HANDLE_CODE(bref.pack(codebook_subset_present, 1));
  HANDLE_CODE(bref.pack(max_rank_present, 1));
  HANDLE_CODE(bref.pack(rbg_size_present, 1));
  HANDLE_CODE(bref.pack(uci_on_pusch_present, 1));
  HANDLE_CODE(bref.pack(tp_pi2_bpsk_present, 1));

  if (data_scrambling_id_pusch_present) {
    HANDLE_CODE(pack_integer(bref, data_scrambling_id_pusch, (uint16_t)0u, (uint16_t)1023u));
  }
  if (tx_cfg_present) {
    HANDLE_CODE(tx_cfg.pack(bref));
  }
  if (dmrs_ul_for_pusch_map_type_a_present) {
    HANDLE_CODE(dmrs_ul_for_pusch_map_type_a.pack(bref));
  }
  if (dmrs_ul_for_pusch_map_type_b_present) {
    HANDLE_CODE(dmrs_ul_for_pusch_map_type_b.pack(bref));
  }
  if (pusch_pwr_ctrl_present) {
    HANDLE_CODE(pusch_pwr_ctrl.pack(bref));
  }
  if (freq_hop_present) {
    HANDLE_CODE(freq_hop.pack(bref));
  }
  if (freq_hop_offset_lists.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_hop_offset_lists, 1, 4, integer_packer<uint16_t>(1, 274)));
  }
  HANDLE_CODE(res_alloc.pack(bref));
  if (pusch_time_domain_alloc_list_present) {
    HANDLE_CODE(pusch_time_domain_alloc_list.pack(bref));
  }
  if (pusch_aggregation_factor_present) {
    HANDLE_CODE(pusch_aggregation_factor.pack(bref));
  }
  if (mcs_table_present) {
    HANDLE_CODE(mcs_table.pack(bref));
  }
  if (mcs_table_transform_precoder_present) {
    HANDLE_CODE(mcs_table_transform_precoder.pack(bref));
  }
  if (transform_precoder_present) {
    HANDLE_CODE(transform_precoder.pack(bref));
  }
  if (codebook_subset_present) {
    HANDLE_CODE(codebook_subset.pack(bref));
  }
  if (max_rank_present) {
    HANDLE_CODE(pack_integer(bref, max_rank, (uint8_t)1u, (uint8_t)4u));
  }
  if (uci_on_pusch_present) {
    HANDLE_CODE(uci_on_pusch.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_scrambling_id_pusch_present, 1));
  HANDLE_CODE(bref.unpack(tx_cfg_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_ul_for_pusch_map_type_a_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_ul_for_pusch_map_type_b_present, 1));
  HANDLE_CODE(bref.unpack(pusch_pwr_ctrl_present, 1));
  HANDLE_CODE(bref.unpack(freq_hop_present, 1));
  bool freq_hop_offset_lists_present;
  HANDLE_CODE(bref.unpack(freq_hop_offset_lists_present, 1));
  HANDLE_CODE(bref.unpack(pusch_time_domain_alloc_list_present, 1));
  HANDLE_CODE(bref.unpack(pusch_aggregation_factor_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_transform_precoder_present, 1));
  HANDLE_CODE(bref.unpack(transform_precoder_present, 1));
  HANDLE_CODE(bref.unpack(codebook_subset_present, 1));
  HANDLE_CODE(bref.unpack(max_rank_present, 1));
  HANDLE_CODE(bref.unpack(rbg_size_present, 1));
  HANDLE_CODE(bref.unpack(uci_on_pusch_present, 1));
  HANDLE_CODE(bref.unpack(tp_pi2_bpsk_present, 1));

  if (data_scrambling_id_pusch_present) {
    HANDLE_CODE(unpack_integer(data_scrambling_id_pusch, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (tx_cfg_present) {
    HANDLE_CODE(tx_cfg.unpack(bref));
  }
  if (dmrs_ul_for_pusch_map_type_a_present) {
    HANDLE_CODE(dmrs_ul_for_pusch_map_type_a.unpack(bref));
  }
  if (dmrs_ul_for_pusch_map_type_b_present) {
    HANDLE_CODE(dmrs_ul_for_pusch_map_type_b.unpack(bref));
  }
  if (pusch_pwr_ctrl_present) {
    HANDLE_CODE(pusch_pwr_ctrl.unpack(bref));
  }
  if (freq_hop_present) {
    HANDLE_CODE(freq_hop.unpack(bref));
  }
  if (freq_hop_offset_lists_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_hop_offset_lists, bref, 1, 4, integer_packer<uint16_t>(1, 274)));
  }
  HANDLE_CODE(res_alloc.unpack(bref));
  if (pusch_time_domain_alloc_list_present) {
    HANDLE_CODE(pusch_time_domain_alloc_list.unpack(bref));
  }
  if (pusch_aggregation_factor_present) {
    HANDLE_CODE(pusch_aggregation_factor.unpack(bref));
  }
  if (mcs_table_present) {
    HANDLE_CODE(mcs_table.unpack(bref));
  }
  if (mcs_table_transform_precoder_present) {
    HANDLE_CODE(mcs_table_transform_precoder.unpack(bref));
  }
  if (transform_precoder_present) {
    HANDLE_CODE(transform_precoder.unpack(bref));
  }
  if (codebook_subset_present) {
    HANDLE_CODE(codebook_subset.unpack(bref));
  }
  if (max_rank_present) {
    HANDLE_CODE(unpack_integer(max_rank, bref, (uint8_t)1u, (uint8_t)4u));
  }
  if (uci_on_pusch_present) {
    HANDLE_CODE(uci_on_pusch.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (data_scrambling_id_pusch_present) {
    j.write_int("dataScramblingIdentityPUSCH", data_scrambling_id_pusch);
  }
  if (tx_cfg_present) {
    j.write_str("txConfig", tx_cfg.to_string());
  }
  if (dmrs_ul_for_pusch_map_type_a_present) {
    j.write_fieldname("dmrs-UplinkForPUSCH-MappingTypeA");
    dmrs_ul_for_pusch_map_type_a.to_json(j);
  }
  if (dmrs_ul_for_pusch_map_type_b_present) {
    j.write_fieldname("dmrs-UplinkForPUSCH-MappingTypeB");
    dmrs_ul_for_pusch_map_type_b.to_json(j);
  }
  if (pusch_pwr_ctrl_present) {
    j.write_fieldname("pusch-PowerControl");
    pusch_pwr_ctrl.to_json(j);
  }
  if (freq_hop_present) {
    j.write_str("frequencyHopping", freq_hop.to_string());
  }
  if (freq_hop_offset_lists.size() > 0) {
    j.start_array("frequencyHoppingOffsetLists");
    for (const auto& e1 : freq_hop_offset_lists) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_str("resourceAllocation", res_alloc.to_string());
  if (pusch_time_domain_alloc_list_present) {
    j.write_fieldname("pusch-TimeDomainAllocationList");
    pusch_time_domain_alloc_list.to_json(j);
  }
  if (pusch_aggregation_factor_present) {
    j.write_str("pusch-AggregationFactor", pusch_aggregation_factor.to_string());
  }
  if (mcs_table_present) {
    j.write_str("mcs-Table", mcs_table.to_string());
  }
  if (mcs_table_transform_precoder_present) {
    j.write_str("mcs-TableTransformPrecoder", mcs_table_transform_precoder.to_string());
  }
  if (transform_precoder_present) {
    j.write_str("transformPrecoder", transform_precoder.to_string());
  }
  if (codebook_subset_present) {
    j.write_str("codebookSubset", codebook_subset.to_string());
  }
  if (max_rank_present) {
    j.write_int("maxRank", max_rank);
  }
  if (rbg_size_present) {
    j.write_str("rbg-Size", "config2");
  }
  if (uci_on_pusch_present) {
    j.write_fieldname("uci-OnPUSCH");
    uci_on_pusch.to_json(j);
  }
  if (tp_pi2_bpsk_present) {
    j.write_str("tp-pi2BPSK", "enabled");
  }
  j.end_obj();
}

const char* pusch_cfg_s::tx_cfg_opts::to_string() const
{
  static const char* names[] = {"codebook", "nonCodebook"};
  return convert_enum_idx(names, 2, value, "pusch_cfg_s::tx_cfg_e_");
}

const char* pusch_cfg_s::freq_hop_opts::to_string() const
{
  static const char* names[] = {"intraSlot", "interSlot"};
  return convert_enum_idx(names, 2, value, "pusch_cfg_s::freq_hop_e_");
}

const char* pusch_cfg_s::res_alloc_opts::to_string() const
{
  static const char* names[] = {"resourceAllocationType0", "resourceAllocationType1", "dynamicSwitch"};
  return convert_enum_idx(names, 3, value, "pusch_cfg_s::res_alloc_e_");
}
uint8_t pusch_cfg_s::res_alloc_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "pusch_cfg_s::res_alloc_e_");
}

const char* pusch_cfg_s::pusch_aggregation_factor_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8"};
  return convert_enum_idx(names, 3, value, "pusch_cfg_s::pusch_aggregation_factor_e_");
}
uint8_t pusch_cfg_s::pusch_aggregation_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "pusch_cfg_s::pusch_aggregation_factor_e_");
}

const char* pusch_cfg_s::mcs_table_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE"};
  return convert_enum_idx(names, 2, value, "pusch_cfg_s::mcs_table_e_");
}
uint16_t pusch_cfg_s::mcs_table_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "pusch_cfg_s::mcs_table_e_");
}

const char* pusch_cfg_s::mcs_table_transform_precoder_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE"};
  return convert_enum_idx(names, 2, value, "pusch_cfg_s::mcs_table_transform_precoder_e_");
}
uint16_t pusch_cfg_s::mcs_table_transform_precoder_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "pusch_cfg_s::mcs_table_transform_precoder_e_");
}

const char* pusch_cfg_s::transform_precoder_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "pusch_cfg_s::transform_precoder_e_");
}

const char* pusch_cfg_s::codebook_subset_opts::to_string() const
{
  static const char* names[] = {"fullyAndPartialAndNonCoherent", "partialAndNonCoherent", "nonCoherent"};
  return convert_enum_idx(names, 3, value, "pusch_cfg_s::codebook_subset_e_");
}

// SRS-Config ::= SEQUENCE
SRSASN_CODE srs_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srs_res_set_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_res_set_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_res_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_res_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(tpc_accumulation_present, 1));

  if (srs_res_set_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_res_set_to_release_list, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (srs_res_set_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_res_set_to_add_mod_list, 1, 16));
  }
  if (srs_res_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_res_to_release_list, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (srs_res_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_res_to_add_mod_list, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool srs_res_set_to_release_list_present;
  HANDLE_CODE(bref.unpack(srs_res_set_to_release_list_present, 1));
  bool srs_res_set_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(srs_res_set_to_add_mod_list_present, 1));
  bool srs_res_to_release_list_present;
  HANDLE_CODE(bref.unpack(srs_res_to_release_list_present, 1));
  bool srs_res_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(srs_res_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(tpc_accumulation_present, 1));

  if (srs_res_set_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_res_set_to_release_list, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (srs_res_set_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_res_set_to_add_mod_list, bref, 1, 16));
  }
  if (srs_res_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_res_to_release_list, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (srs_res_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_res_to_add_mod_list, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void srs_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_res_set_to_release_list.size() > 0) {
    j.start_array("srs-ResourceSetToReleaseList");
    for (const auto& e1 : srs_res_set_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (srs_res_set_to_add_mod_list.size() > 0) {
    j.start_array("srs-ResourceSetToAddModList");
    for (const auto& e1 : srs_res_set_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srs_res_to_release_list.size() > 0) {
    j.start_array("srs-ResourceToReleaseList");
    for (const auto& e1 : srs_res_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (srs_res_to_add_mod_list.size() > 0) {
    j.start_array("srs-ResourceToAddModList");
    for (const auto& e1 : srs_res_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (tpc_accumulation_present) {
    j.write_str("tpc-Accumulation", "disabled");
  }
  j.end_obj();
}

// BWP-UplinkDedicated ::= SEQUENCE
SRSASN_CODE bwp_ul_ded_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pucch_cfg_present, 1));
  HANDLE_CODE(bref.pack(pusch_cfg_present, 1));
  HANDLE_CODE(bref.pack(cfg_grant_cfg_present, 1));
  HANDLE_CODE(bref.pack(srs_cfg_present, 1));
  HANDLE_CODE(bref.pack(beam_fail_recovery_cfg_present, 1));

  if (pucch_cfg_present) {
    HANDLE_CODE(pucch_cfg.pack(bref));
  }
  if (pusch_cfg_present) {
    HANDLE_CODE(pusch_cfg.pack(bref));
  }
  if (cfg_grant_cfg_present) {
    HANDLE_CODE(cfg_grant_cfg.pack(bref));
  }
  if (srs_cfg_present) {
    HANDLE_CODE(srs_cfg.pack(bref));
  }
  if (beam_fail_recovery_cfg_present) {
    HANDLE_CODE(beam_fail_recovery_cfg.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_ul_ded_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pucch_cfg_present, 1));
  HANDLE_CODE(bref.unpack(pusch_cfg_present, 1));
  HANDLE_CODE(bref.unpack(cfg_grant_cfg_present, 1));
  HANDLE_CODE(bref.unpack(srs_cfg_present, 1));
  HANDLE_CODE(bref.unpack(beam_fail_recovery_cfg_present, 1));

  if (pucch_cfg_present) {
    HANDLE_CODE(pucch_cfg.unpack(bref));
  }
  if (pusch_cfg_present) {
    HANDLE_CODE(pusch_cfg.unpack(bref));
  }
  if (cfg_grant_cfg_present) {
    HANDLE_CODE(cfg_grant_cfg.unpack(bref));
  }
  if (srs_cfg_present) {
    HANDLE_CODE(srs_cfg.unpack(bref));
  }
  if (beam_fail_recovery_cfg_present) {
    HANDLE_CODE(beam_fail_recovery_cfg.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bwp_ul_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pucch_cfg_present) {
    j.write_fieldname("pucch-Config");
    pucch_cfg.to_json(j);
  }
  if (pusch_cfg_present) {
    j.write_fieldname("pusch-Config");
    pusch_cfg.to_json(j);
  }
  if (cfg_grant_cfg_present) {
    j.write_fieldname("configuredGrantConfig");
    cfg_grant_cfg.to_json(j);
  }
  if (srs_cfg_present) {
    j.write_fieldname("srs-Config");
    srs_cfg.to_json(j);
  }
  if (beam_fail_recovery_cfg_present) {
    j.write_fieldname("beamFailureRecoveryConfig");
    beam_fail_recovery_cfg.to_json(j);
  }
  j.end_obj();
}

// BWP-Uplink ::= SEQUENCE
SRSASN_CODE bwp_ul_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(bwp_common_present, 1));
  HANDLE_CODE(bref.pack(bwp_ded_present, 1));

  HANDLE_CODE(pack_integer(bref, bwp_id, (uint8_t)0u, (uint8_t)4u));
  if (bwp_common_present) {
    HANDLE_CODE(bwp_common.pack(bref));
  }
  if (bwp_ded_present) {
    HANDLE_CODE(bwp_ded.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_ul_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(bwp_common_present, 1));
  HANDLE_CODE(bref.unpack(bwp_ded_present, 1));

  HANDLE_CODE(unpack_integer(bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  if (bwp_common_present) {
    HANDLE_CODE(bwp_common.unpack(bref));
  }
  if (bwp_ded_present) {
    HANDLE_CODE(bwp_ded.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bwp_ul_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bwp-Id", bwp_id);
  if (bwp_common_present) {
    j.write_fieldname("bwp-Common");
    bwp_common.to_json(j);
  }
  if (bwp_ded_present) {
    j.write_fieldname("bwp-Dedicated");
    bwp_ded.to_json(j);
  }
  j.end_obj();
}

// CSI-AssociatedReportConfigInfo ::= SEQUENCE
SRSASN_CODE csi_associated_report_cfg_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(csi_im_res_for_interference_present, 1));
  HANDLE_CODE(bref.pack(nzp_csi_rs_res_for_interference_present, 1));

  HANDLE_CODE(pack_integer(bref, report_cfg_id, (uint8_t)0u, (uint8_t)47u));
  HANDLE_CODE(res_for_ch.pack(bref));
  if (csi_im_res_for_interference_present) {
    HANDLE_CODE(pack_integer(bref, csi_im_res_for_interference, (uint8_t)1u, (uint8_t)16u));
  }
  if (nzp_csi_rs_res_for_interference_present) {
    HANDLE_CODE(pack_integer(bref, nzp_csi_rs_res_for_interference, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_associated_report_cfg_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(csi_im_res_for_interference_present, 1));
  HANDLE_CODE(bref.unpack(nzp_csi_rs_res_for_interference_present, 1));

  HANDLE_CODE(unpack_integer(report_cfg_id, bref, (uint8_t)0u, (uint8_t)47u));
  HANDLE_CODE(res_for_ch.unpack(bref));
  if (csi_im_res_for_interference_present) {
    HANDLE_CODE(unpack_integer(csi_im_res_for_interference, bref, (uint8_t)1u, (uint8_t)16u));
  }
  if (nzp_csi_rs_res_for_interference_present) {
    HANDLE_CODE(unpack_integer(nzp_csi_rs_res_for_interference, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void csi_associated_report_cfg_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reportConfigId", report_cfg_id);
  j.write_fieldname("resourcesForChannel");
  res_for_ch.to_json(j);
  if (csi_im_res_for_interference_present) {
    j.write_int("csi-IM-ResourcesForInterference", csi_im_res_for_interference);
  }
  if (nzp_csi_rs_res_for_interference_present) {
    j.write_int("nzp-CSI-RS-ResourcesForInterference", nzp_csi_rs_res_for_interference);
  }
  j.end_obj();
}

void csi_associated_report_cfg_info_s::res_for_ch_c_::destroy_()
{
  switch (type_) {
    case types::nzp_csi_rs:
      c.destroy<nzp_csi_rs_s_>();
      break;
    default:
      break;
  }
}
void csi_associated_report_cfg_info_s::res_for_ch_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nzp_csi_rs:
      c.init<nzp_csi_rs_s_>();
      break;
    case types::csi_ssb_res_set:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch_c_");
  }
}
csi_associated_report_cfg_info_s::res_for_ch_c_::res_for_ch_c_(
    const csi_associated_report_cfg_info_s::res_for_ch_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nzp_csi_rs:
      c.init(other.c.get<nzp_csi_rs_s_>());
      break;
    case types::csi_ssb_res_set:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch_c_");
  }
}
csi_associated_report_cfg_info_s::res_for_ch_c_&
csi_associated_report_cfg_info_s::res_for_ch_c_::operator=(const csi_associated_report_cfg_info_s::res_for_ch_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nzp_csi_rs:
      c.set(other.c.get<nzp_csi_rs_s_>());
      break;
    case types::csi_ssb_res_set:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch_c_");
  }

  return *this;
}
csi_associated_report_cfg_info_s::res_for_ch_c_::nzp_csi_rs_s_&
csi_associated_report_cfg_info_s::res_for_ch_c_::set_nzp_csi_rs()
{
  set(types::nzp_csi_rs);
  return c.get<nzp_csi_rs_s_>();
}
uint8_t& csi_associated_report_cfg_info_s::res_for_ch_c_::set_csi_ssb_res_set()
{
  set(types::csi_ssb_res_set);
  return c.get<uint8_t>();
}
void csi_associated_report_cfg_info_s::res_for_ch_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nzp_csi_rs:
      j.write_fieldname("nzp-CSI-RS");
      j.start_obj();
      j.write_int("resourceSet", c.get<nzp_csi_rs_s_>().res_set);
      if (c.get<nzp_csi_rs_s_>().qcl_info.size() > 0) {
        j.start_array("qcl-info");
        for (const auto& e1 : c.get<nzp_csi_rs_s_>().qcl_info) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    case types::csi_ssb_res_set:
      j.write_int("csi-SSB-ResourceSet", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_associated_report_cfg_info_s::res_for_ch_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nzp_csi_rs:
      HANDLE_CODE(bref.pack(c.get<nzp_csi_rs_s_>().qcl_info.size() > 0, 1));
      HANDLE_CODE(pack_integer(bref, c.get<nzp_csi_rs_s_>().res_set, (uint8_t)1u, (uint8_t)16u));
      if (c.get<nzp_csi_rs_s_>().qcl_info.size() > 0) {
        HANDLE_CODE(pack_dyn_seq_of(bref, c.get<nzp_csi_rs_s_>().qcl_info, 1, 16, integer_packer<uint8_t>(0, 127)));
      }
      break;
    case types::csi_ssb_res_set:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)1u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_associated_report_cfg_info_s::res_for_ch_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nzp_csi_rs:
      bool qcl_info_present;
      HANDLE_CODE(bref.unpack(qcl_info_present, 1));
      HANDLE_CODE(unpack_integer(c.get<nzp_csi_rs_s_>().res_set, bref, (uint8_t)1u, (uint8_t)16u));
      if (qcl_info_present) {
        HANDLE_CODE(unpack_dyn_seq_of(c.get<nzp_csi_rs_s_>().qcl_info, bref, 1, 16, integer_packer<uint8_t>(0, 127)));
      }
      break;
    case types::csi_ssb_res_set:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)1u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CSI-AperiodicTriggerState ::= SEQUENCE
SRSASN_CODE csi_aperiodic_trigger_state_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, associated_report_cfg_info_list, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_aperiodic_trigger_state_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(associated_report_cfg_info_list, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void csi_aperiodic_trigger_state_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("associatedReportConfigInfoList");
  for (const auto& e1 : associated_report_cfg_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CSI-IM-Resource ::= SEQUENCE
SRSASN_CODE csi_im_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(csi_im_res_elem_pattern_present, 1));
  HANDLE_CODE(bref.pack(freq_band_present, 1));
  HANDLE_CODE(bref.pack(periodicity_and_offset_present, 1));

  HANDLE_CODE(pack_integer(bref, csi_im_res_id, (uint8_t)0u, (uint8_t)31u));
  if (csi_im_res_elem_pattern_present) {
    HANDLE_CODE(csi_im_res_elem_pattern.pack(bref));
  }
  if (freq_band_present) {
    HANDLE_CODE(freq_band.pack(bref));
  }
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_im_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(csi_im_res_elem_pattern_present, 1));
  HANDLE_CODE(bref.unpack(freq_band_present, 1));
  HANDLE_CODE(bref.unpack(periodicity_and_offset_present, 1));

  HANDLE_CODE(unpack_integer(csi_im_res_id, bref, (uint8_t)0u, (uint8_t)31u));
  if (csi_im_res_elem_pattern_present) {
    HANDLE_CODE(csi_im_res_elem_pattern.unpack(bref));
  }
  if (freq_band_present) {
    HANDLE_CODE(freq_band.unpack(bref));
  }
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_im_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-IM-ResourceId", csi_im_res_id);
  if (csi_im_res_elem_pattern_present) {
    j.write_fieldname("csi-IM-ResourceElementPattern");
    csi_im_res_elem_pattern.to_json(j);
  }
  if (freq_band_present) {
    j.write_fieldname("freqBand");
    freq_band.to_json(j);
  }
  if (periodicity_and_offset_present) {
    j.write_fieldname("periodicityAndOffset");
    periodicity_and_offset.to_json(j);
  }
  j.end_obj();
}

void csi_im_res_s::csi_im_res_elem_pattern_c_::destroy_()
{
  switch (type_) {
    case types::pattern0:
      c.destroy<pattern0_s_>();
      break;
    case types::pattern1:
      c.destroy<pattern1_s_>();
      break;
    default:
      break;
  }
}
void csi_im_res_s::csi_im_res_elem_pattern_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pattern0:
      c.init<pattern0_s_>();
      break;
    case types::pattern1:
      c.init<pattern1_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_im_res_s::csi_im_res_elem_pattern_c_");
  }
}
csi_im_res_s::csi_im_res_elem_pattern_c_::csi_im_res_elem_pattern_c_(
    const csi_im_res_s::csi_im_res_elem_pattern_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pattern0:
      c.init(other.c.get<pattern0_s_>());
      break;
    case types::pattern1:
      c.init(other.c.get<pattern1_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_im_res_s::csi_im_res_elem_pattern_c_");
  }
}
csi_im_res_s::csi_im_res_elem_pattern_c_&
csi_im_res_s::csi_im_res_elem_pattern_c_::operator=(const csi_im_res_s::csi_im_res_elem_pattern_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pattern0:
      c.set(other.c.get<pattern0_s_>());
      break;
    case types::pattern1:
      c.set(other.c.get<pattern1_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_im_res_s::csi_im_res_elem_pattern_c_");
  }

  return *this;
}
csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_& csi_im_res_s::csi_im_res_elem_pattern_c_::set_pattern0()
{
  set(types::pattern0);
  return c.get<pattern0_s_>();
}
csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_& csi_im_res_s::csi_im_res_elem_pattern_c_::set_pattern1()
{
  set(types::pattern1);
  return c.get<pattern1_s_>();
}
void csi_im_res_s::csi_im_res_elem_pattern_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pattern0:
      j.write_fieldname("pattern0");
      j.start_obj();
      j.write_str("subcarrierLocation-p0", c.get<pattern0_s_>().subcarrier_location_p0.to_string());
      j.write_int("symbolLocation-p0", c.get<pattern0_s_>().symbol_location_p0);
      j.end_obj();
      break;
    case types::pattern1:
      j.write_fieldname("pattern1");
      j.start_obj();
      j.write_str("subcarrierLocation-p1", c.get<pattern1_s_>().subcarrier_location_p1.to_string());
      j.write_int("symbolLocation-p1", c.get<pattern1_s_>().symbol_location_p1);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_im_res_s::csi_im_res_elem_pattern_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_im_res_s::csi_im_res_elem_pattern_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pattern0:
      HANDLE_CODE(c.get<pattern0_s_>().subcarrier_location_p0.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<pattern0_s_>().symbol_location_p0, (uint8_t)0u, (uint8_t)12u));
      break;
    case types::pattern1:
      HANDLE_CODE(c.get<pattern1_s_>().subcarrier_location_p1.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<pattern1_s_>().symbol_location_p1, (uint8_t)0u, (uint8_t)13u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_im_res_s::csi_im_res_elem_pattern_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_im_res_s::csi_im_res_elem_pattern_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pattern0:
      HANDLE_CODE(c.get<pattern0_s_>().subcarrier_location_p0.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<pattern0_s_>().symbol_location_p0, bref, (uint8_t)0u, (uint8_t)12u));
      break;
    case types::pattern1:
      HANDLE_CODE(c.get<pattern1_s_>().subcarrier_location_p1.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<pattern1_s_>().symbol_location_p1, bref, (uint8_t)0u, (uint8_t)13u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_im_res_s::csi_im_res_elem_pattern_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::to_string() const
{
  static const char* names[] = {"s0", "s2", "s4", "s6", "s8", "s10"};
  return convert_enum_idx(
      names, 6, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_e_");
}
uint8_t csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6, 8, 10};
  return map_enum_number(
      numbers, 6, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_e_");
}

const char* csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::to_string() const
{
  static const char* names[] = {"s0", "s4", "s8"};
  return convert_enum_idx(
      names, 3, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_e_");
}
uint8_t csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 4, 8};
  return map_enum_number(
      numbers, 3, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_e_");
}

// CSI-IM-ResourceSet ::= SEQUENCE
SRSASN_CODE csi_im_res_set_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_im_res_set_id, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_res, 1, 8, integer_packer<uint8_t>(0, 31)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_im_res_set_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_im_res_set_id, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_dyn_seq_of(csi_im_res, bref, 1, 8, integer_packer<uint8_t>(0, 31)));

  return SRSASN_SUCCESS;
}
void csi_im_res_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-IM-ResourceSetId", csi_im_res_set_id);
  j.start_array("csi-IM-Resources");
  for (const auto& e1 : csi_im_res) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// CSI-ReportPeriodicityAndOffset ::= CHOICE
void csi_report_periodicity_and_offset_c::destroy_() {}
void csi_report_periodicity_and_offset_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
csi_report_periodicity_and_offset_c::csi_report_periodicity_and_offset_c(
    const csi_report_periodicity_and_offset_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slots4:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots5:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots8:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::slots320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_periodicity_and_offset_c");
  }
}
csi_report_periodicity_and_offset_c&
csi_report_periodicity_and_offset_c::operator=(const csi_report_periodicity_and_offset_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slots4:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots5:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots8:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::slots320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_periodicity_and_offset_c");
  }

  return *this;
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots4()
{
  set(types::slots4);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots5()
{
  set(types::slots5);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots8()
{
  set(types::slots8);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots10()
{
  set(types::slots10);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots16()
{
  set(types::slots16);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots20()
{
  set(types::slots20);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots40()
{
  set(types::slots40);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots80()
{
  set(types::slots80);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots160()
{
  set(types::slots160);
  return c.get<uint8_t>();
}
uint16_t& csi_report_periodicity_and_offset_c::set_slots320()
{
  set(types::slots320);
  return c.get<uint16_t>();
}
void csi_report_periodicity_and_offset_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slots4:
      j.write_int("slots4", c.get<uint8_t>());
      break;
    case types::slots5:
      j.write_int("slots5", c.get<uint8_t>());
      break;
    case types::slots8:
      j.write_int("slots8", c.get<uint8_t>());
      break;
    case types::slots10:
      j.write_int("slots10", c.get<uint8_t>());
      break;
    case types::slots16:
      j.write_int("slots16", c.get<uint8_t>());
      break;
    case types::slots20:
      j.write_int("slots20", c.get<uint8_t>());
      break;
    case types::slots40:
      j.write_int("slots40", c.get<uint8_t>());
      break;
    case types::slots80:
      j.write_int("slots80", c.get<uint8_t>());
      break;
    case types::slots160:
      j.write_int("slots160", c.get<uint8_t>());
      break;
    case types::slots320:
      j.write_int("slots320", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_periodicity_and_offset_c");
  }
  j.end_obj();
}
SRSASN_CODE csi_report_periodicity_and_offset_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slots4:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::slots5:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::slots8:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::slots10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::slots16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::slots20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::slots40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::slots80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::slots160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::slots320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_periodicity_and_offset_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_periodicity_and_offset_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slots4:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::slots5:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::slots8:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::slots10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::slots16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::slots20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::slots40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::slots80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::slots160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::slots320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_periodicity_and_offset_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CSI-SemiPersistentOnPUSCH-TriggerState ::= SEQUENCE
SRSASN_CODE csi_semi_persistent_on_pusch_trigger_state_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, associated_report_cfg_info, (uint8_t)0u, (uint8_t)47u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_semi_persistent_on_pusch_trigger_state_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(associated_report_cfg_info, bref, (uint8_t)0u, (uint8_t)47u));

  return SRSASN_SUCCESS;
}
void csi_semi_persistent_on_pusch_trigger_state_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("associatedReportConfigInfo", associated_report_cfg_info);
  j.end_obj();
}

// CodebookConfig ::= SEQUENCE
SRSASN_CODE codebook_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(codebook_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(codebook_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void codebook_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("codebookType");
  codebook_type.to_json(j);
  j.end_obj();
}

void codebook_cfg_s::codebook_type_c_::destroy_()
{
  switch (type_) {
    case types::type1:
      c.destroy<type1_s_>();
      break;
    case types::type2:
      c.destroy<type2_s_>();
      break;
    default:
      break;
  }
}
void codebook_cfg_s::codebook_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type1:
      c.init<type1_s_>();
      break;
    case types::type2:
      c.init<type2_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_");
  }
}
codebook_cfg_s::codebook_type_c_::codebook_type_c_(const codebook_cfg_s::codebook_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type1:
      c.init(other.c.get<type1_s_>());
      break;
    case types::type2:
      c.init(other.c.get<type2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_");
  }
}
codebook_cfg_s::codebook_type_c_&
codebook_cfg_s::codebook_type_c_::operator=(const codebook_cfg_s::codebook_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type1:
      c.set(other.c.get<type1_s_>());
      break;
    case types::type2:
      c.set(other.c.get<type2_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_");
  }

  return *this;
}
codebook_cfg_s::codebook_type_c_::type1_s_& codebook_cfg_s::codebook_type_c_::set_type1()
{
  set(types::type1);
  return c.get<type1_s_>();
}
codebook_cfg_s::codebook_type_c_::type2_s_& codebook_cfg_s::codebook_type_c_::set_type2()
{
  set(types::type2);
  return c.get<type2_s_>();
}
void codebook_cfg_s::codebook_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type1:
      j.write_fieldname("type1");
      j.start_obj();
      j.write_fieldname("subType");
      c.get<type1_s_>().sub_type.to_json(j);
      j.write_int("codebookMode", c.get<type1_s_>().codebook_mode);
      j.end_obj();
      break;
    case types::type2:
      j.write_fieldname("type2");
      j.start_obj();
      j.write_fieldname("subType");
      c.get<type2_s_>().sub_type.to_json(j);
      j.write_str("phaseAlphabetSize", c.get<type2_s_>().phase_alphabet_size.to_string());
      j.write_bool("subbandAmplitude", c.get<type2_s_>().subband_amplitude);
      j.write_str("numberOfBeams", c.get<type2_s_>().nof_beams.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type1:
      HANDLE_CODE(c.get<type1_s_>().sub_type.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<type1_s_>().codebook_mode, (uint8_t)1u, (uint8_t)2u));
      break;
    case types::type2:
      HANDLE_CODE(c.get<type2_s_>().sub_type.pack(bref));
      HANDLE_CODE(c.get<type2_s_>().phase_alphabet_size.pack(bref));
      HANDLE_CODE(bref.pack(c.get<type2_s_>().subband_amplitude, 1));
      HANDLE_CODE(c.get<type2_s_>().nof_beams.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type1:
      HANDLE_CODE(c.get<type1_s_>().sub_type.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<type1_s_>().codebook_mode, bref, (uint8_t)1u, (uint8_t)2u));
      break;
    case types::type2:
      HANDLE_CODE(c.get<type2_s_>().sub_type.unpack(bref));
      HANDLE_CODE(c.get<type2_s_>().phase_alphabet_size.unpack(bref));
      HANDLE_CODE(bref.unpack(c.get<type2_s_>().subband_amplitude, 1));
      HANDLE_CODE(c.get<type2_s_>().nof_beams.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::destroy_()
{
  switch (type_) {
    case types::type_i_single_panel:
      c.destroy<type_i_single_panel_s_>();
      break;
    case types::type_i_multi_panel:
      c.destroy<type_i_multi_panel_s_>();
      break;
    default:
      break;
  }
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type_i_single_panel:
      c.init<type_i_single_panel_s_>();
      break;
    case types::type_i_multi_panel:
      c.init<type_i_multi_panel_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::sub_type_c_(
    const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type_i_single_panel:
      c.init(other.c.get<type_i_single_panel_s_>());
      break;
    case types::type_i_multi_panel:
      c.init(other.c.get<type_i_multi_panel_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::operator=(
    const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type_i_single_panel:
      c.set(other.c.get<type_i_single_panel_s_>());
      break;
    case types::type_i_multi_panel:
      c.set(other.c.get<type_i_multi_panel_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_");
  }

  return *this;
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::set_type_i_single_panel()
{
  set(types::type_i_single_panel);
  return c.get<type_i_single_panel_s_>();
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::set_type_i_multi_panel()
{
  set(types::type_i_multi_panel);
  return c.get<type_i_multi_panel_s_>();
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type_i_single_panel:
      j.write_fieldname("typeI-SinglePanel");
      j.start_obj();
      j.write_fieldname("nrOfAntennaPorts");
      c.get<type_i_single_panel_s_>().nr_of_ant_ports.to_json(j);
      j.write_str("typeI-SinglePanel-ri-Restriction",
                  c.get<type_i_single_panel_s_>().type_i_single_panel_ri_restrict.to_string());
      j.end_obj();
      break;
    case types::type_i_multi_panel:
      j.write_fieldname("typeI-MultiPanel");
      j.start_obj();
      j.write_fieldname("ng-n1-n2");
      c.get<type_i_multi_panel_s_>().ng_n1_n2.to_json(j);
      j.write_str("ri-Restriction", c.get<type_i_multi_panel_s_>().ri_restrict.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type_i_single_panel:
      HANDLE_CODE(c.get<type_i_single_panel_s_>().nr_of_ant_ports.pack(bref));
      HANDLE_CODE(c.get<type_i_single_panel_s_>().type_i_single_panel_ri_restrict.pack(bref));
      break;
    case types::type_i_multi_panel:
      HANDLE_CODE(c.get<type_i_multi_panel_s_>().ng_n1_n2.pack(bref));
      HANDLE_CODE(c.get<type_i_multi_panel_s_>().ri_restrict.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type_i_single_panel:
      HANDLE_CODE(c.get<type_i_single_panel_s_>().nr_of_ant_ports.unpack(bref));
      HANDLE_CODE(c.get<type_i_single_panel_s_>().type_i_single_panel_ri_restrict.unpack(bref));
      break;
    case types::type_i_multi_panel:
      HANDLE_CODE(c.get<type_i_multi_panel_s_>().ng_n1_n2.unpack(bref));
      HANDLE_CODE(c.get<type_i_multi_panel_s_>().ri_restrict.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::destroy_()
{
  switch (type_) {
    case types::two:
      c.destroy<two_s_>();
      break;
    case types::more_than_two:
      c.destroy<more_than_two_s_>();
      break;
    default:
      break;
  }
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::set(
    types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::two:
      c.init<two_s_>();
      break;
    case types::more_than_two:
      c.init<more_than_two_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::nr_of_ant_ports_c_(
    const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::two:
      c.init(other.c.get<two_s_>());
      break;
    case types::more_than_two:
      c.init(other.c.get<more_than_two_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::operator=(
    const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::two:
      c.set(other.c.get<two_s_>());
      break;
    case types::more_than_two:
      c.set(other.c.get<more_than_two_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_");
  }

  return *this;
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::two_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::set_two()
{
  set(types::two);
  return c.get<two_s_>();
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::more_than_two_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::set_more_than_two()
{
  set(types::more_than_two);
  return c.get<more_than_two_s_>();
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two:
      j.write_fieldname("two");
      j.start_obj();
      j.write_str("twoTX-CodebookSubsetRestriction", c.get<two_s_>().two_tx_codebook_subset_restrict.to_string());
      j.end_obj();
      break;
    case types::more_than_two:
      j.write_fieldname("moreThanTwo");
      j.start_obj();
      j.write_fieldname("n1-n2");
      c.get<more_than_two_s_>().n1_n2.to_json(j);
      if (c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2_present) {
        j.write_str("typeI-SinglePanel-codebookSubsetRestriction-i2",
                    c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::pack(
    bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two:
      HANDLE_CODE(c.get<two_s_>().two_tx_codebook_subset_restrict.pack(bref));
      break;
    case types::more_than_two:
      HANDLE_CODE(bref.pack(c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2_present, 1));
      HANDLE_CODE(c.get<more_than_two_s_>().n1_n2.pack(bref));
      if (c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2_present) {
        HANDLE_CODE(c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::unpack(
    cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two:
      HANDLE_CODE(c.get<two_s_>().two_tx_codebook_subset_restrict.unpack(bref));
      break;
    case types::more_than_two:
      HANDLE_CODE(bref.unpack(c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2_present, 1));
      HANDLE_CODE(c.get<more_than_two_s_>().n1_n2.unpack(bref));
      if (c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2_present) {
        HANDLE_CODE(c.get<more_than_two_s_>().type_i_single_panel_codebook_subset_restrict_i2.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::destroy_()
{
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::two_two_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<64>>();
      break;
    case types::four_one_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<16>>();
      break;
    case types::three_two_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<96>>();
      break;
    case types::six_one_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<24>>();
      break;
    case types::four_two_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<128>>();
      break;
    case types::eight_one_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<32>>();
      break;
    case types::four_three_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<192>>();
      break;
    case types::six_two_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<192>>();
      break;
    case types::twelve_one_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<48>>();
      break;
    case types::four_four_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<256>>();
      break;
    case types::eight_two_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<256>>();
      break;
    case types::sixteen_one_type_i_single_panel_restrict:
      c.destroy<fixed_bitstring<64>>();
      break;
    default:
      break;
  }
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict:
      c.init<fixed_bitstring<8>>();
      break;
    case types::two_two_type_i_single_panel_restrict:
      c.init<fixed_bitstring<64>>();
      break;
    case types::four_one_type_i_single_panel_restrict:
      c.init<fixed_bitstring<16>>();
      break;
    case types::three_two_type_i_single_panel_restrict:
      c.init<fixed_bitstring<96>>();
      break;
    case types::six_one_type_i_single_panel_restrict:
      c.init<fixed_bitstring<24>>();
      break;
    case types::four_two_type_i_single_panel_restrict:
      c.init<fixed_bitstring<128>>();
      break;
    case types::eight_one_type_i_single_panel_restrict:
      c.init<fixed_bitstring<32>>();
      break;
    case types::four_three_type_i_single_panel_restrict:
      c.init<fixed_bitstring<192>>();
      break;
    case types::six_two_type_i_single_panel_restrict:
      c.init<fixed_bitstring<192>>();
      break;
    case types::twelve_one_type_i_single_panel_restrict:
      c.init<fixed_bitstring<48>>();
      break;
    case types::four_four_type_i_single_panel_restrict:
      c.init<fixed_bitstring<256>>();
      break;
    case types::eight_two_type_i_single_panel_restrict:
      c.init<fixed_bitstring<256>>();
      break;
    case types::sixteen_one_type_i_single_panel_restrict:
      c.init<fixed_bitstring<64>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::more_than_two_s_::
    n1_n2_c_::n1_n2_c_(const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                           nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_one_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<16>>());
      break;
    case types::three_two_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<96>>());
      break;
    case types::six_one_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<24>>());
      break;
    case types::four_two_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<128>>());
      break;
    case types::eight_one_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_three_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<192>>());
      break;
    case types::six_two_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<192>>());
      break;
    case types::twelve_one_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_four_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<256>>());
      break;
    case types::eight_two_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<256>>());
      break;
    case types::sixteen_one_type_i_single_panel_restrict:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::more_than_two_s_::
    n1_n2_c_&
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
        more_than_two_s_::n1_n2_c_::operator=(
            const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
                more_than_two_s_::n1_n2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_one_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<16>>());
      break;
    case types::three_two_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<96>>());
      break;
    case types::six_one_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<24>>());
      break;
    case types::four_two_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<128>>());
      break;
    case types::eight_one_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_three_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<192>>());
      break;
    case types::six_two_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<192>>());
      break;
    case types::twelve_one_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_four_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<256>>());
      break;
    case types::eight_two_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<256>>());
      break;
    case types::sixteen_one_type_i_single_panel_restrict:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }

  return *this;
}
fixed_bitstring<8>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_one_type_i_single_panel_restrict()
{
  set(types::two_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_two_type_i_single_panel_restrict()
{
  set(types::two_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<16>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_one_type_i_single_panel_restrict()
{
  set(types::four_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<96>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_three_two_type_i_single_panel_restrict()
{
  set(types::three_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<24>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_one_type_i_single_panel_restrict()
{
  set(types::six_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<24>>();
}
fixed_bitstring<128>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_two_type_i_single_panel_restrict()
{
  set(types::four_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<32>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_one_type_i_single_panel_restrict()
{
  set(types::eight_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<192>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_three_type_i_single_panel_restrict()
{
  set(types::four_three_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<192>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_two_type_i_single_panel_restrict()
{
  set(types::six_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<48>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_twelve_one_type_i_single_panel_restrict()
{
  set(types::twelve_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<256>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_four_type_i_single_panel_restrict()
{
  set(types::four_four_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<256>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_two_type_i_single_panel_restrict()
{
  set(types::eight_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_sixteen_one_type_i_single_panel_restrict()
{
  set(types::sixteen_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict:
      j.write_str("two-one-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::two_two_type_i_single_panel_restrict:
      j.write_str("two-two-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<64>>().to_string());
      break;
    case types::four_one_type_i_single_panel_restrict:
      j.write_str("four-one-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<16>>().to_string());
      break;
    case types::three_two_type_i_single_panel_restrict:
      j.write_str("three-two-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<96>>().to_string());
      break;
    case types::six_one_type_i_single_panel_restrict:
      j.write_str("six-one-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<24>>().to_string());
      break;
    case types::four_two_type_i_single_panel_restrict:
      j.write_str("four-two-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<128>>().to_string());
      break;
    case types::eight_one_type_i_single_panel_restrict:
      j.write_str("eight-one-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<32>>().to_string());
      break;
    case types::four_three_type_i_single_panel_restrict:
      j.write_str("four-three-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<192>>().to_string());
      break;
    case types::six_two_type_i_single_panel_restrict:
      j.write_str("six-two-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<192>>().to_string());
      break;
    case types::twelve_one_type_i_single_panel_restrict:
      j.write_str("twelve-one-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<48>>().to_string());
      break;
    case types::four_four_type_i_single_panel_restrict:
      j.write_str("four-four-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<256>>().to_string());
      break;
    case types::eight_two_type_i_single_panel_restrict:
      j.write_str("eight-two-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<256>>().to_string());
      break;
    case types::sixteen_one_type_i_single_panel_restrict:
      j.write_str("sixteen-one-TypeI-SinglePanel-Restriction", c.get<fixed_bitstring<64>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::two_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    case types::four_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().pack(bref));
      break;
    case types::three_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().pack(bref));
      break;
    case types::six_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().pack(bref));
      break;
    case types::four_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().pack(bref));
      break;
    case types::eight_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().pack(bref));
      break;
    case types::four_three_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().pack(bref));
      break;
    case types::six_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().pack(bref));
      break;
    case types::twelve_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().pack(bref));
      break;
    case types::four_four_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().pack(bref));
      break;
    case types::eight_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().pack(bref));
      break;
    case types::sixteen_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::two_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    case types::four_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().unpack(bref));
      break;
    case types::three_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().unpack(bref));
      break;
    case types::six_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().unpack(bref));
      break;
    case types::four_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().unpack(bref));
      break;
    case types::eight_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().unpack(bref));
      break;
    case types::four_three_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().unpack(bref));
      break;
    case types::six_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().unpack(bref));
      break;
    case types::twelve_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().unpack(bref));
      break;
    case types::four_four_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().unpack(bref));
      break;
    case types::eight_two_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().unpack(bref));
      break;
    case types::sixteen_one_type_i_single_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::destroy_()
{
  switch (type_) {
    case types::two_two_one_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::two_four_one_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<16>>();
      break;
    case types::four_two_one_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::two_two_two_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<64>>();
      break;
    case types::two_eight_one_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<32>>();
      break;
    case types::four_four_one_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<16>>();
      break;
    case types::two_four_two_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<128>>();
      break;
    case types::four_two_two_type_i_multi_panel_restrict:
      c.destroy<fixed_bitstring<64>>();
      break;
    default:
      break;
  }
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::two_two_one_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<8>>();
      break;
    case types::two_four_one_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<16>>();
      break;
    case types::four_two_one_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<8>>();
      break;
    case types::two_two_two_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<64>>();
      break;
    case types::two_eight_one_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<32>>();
      break;
    case types::four_four_one_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<16>>();
      break;
    case types::two_four_two_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<128>>();
      break;
    case types::four_two_two_type_i_multi_panel_restrict:
      c.init<fixed_bitstring<64>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::ng_n1_n2_c_(
    const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::two_two_one_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_four_one_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<16>>());
      break;
    case types::four_two_one_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_two_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::two_eight_one_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_four_one_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<16>>());
      break;
    case types::two_four_two_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<128>>());
      break;
    case types::four_two_two_type_i_multi_panel_restrict:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::operator=(
    const codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::two_two_one_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_four_one_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<16>>());
      break;
    case types::four_two_one_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_two_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::two_eight_one_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_four_one_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<16>>());
      break;
    case types::two_four_two_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<128>>());
      break;
    case types::four_two_two_type_i_multi_panel_restrict:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_");
  }

  return *this;
}
fixed_bitstring<8>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_two_one_type_i_multi_panel_restrict()
{
  set(types::two_two_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<16>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_four_one_type_i_multi_panel_restrict()
{
  set(types::two_four_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<8>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_four_two_one_type_i_multi_panel_restrict()
{
  set(types::four_two_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_two_two_type_i_multi_panel_restrict()
{
  set(types::two_two_two_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<32>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_eight_one_type_i_multi_panel_restrict()
{
  set(types::two_eight_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<16>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_four_four_one_type_i_multi_panel_restrict()
{
  set(types::four_four_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<128>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_four_two_type_i_multi_panel_restrict()
{
  set(types::two_four_two_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_four_two_two_type_i_multi_panel_restrict()
{
  set(types::four_two_two_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
void codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two_two_one_type_i_multi_panel_restrict:
      j.write_str("two-two-one-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::two_four_one_type_i_multi_panel_restrict:
      j.write_str("two-four-one-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<16>>().to_string());
      break;
    case types::four_two_one_type_i_multi_panel_restrict:
      j.write_str("four-two-one-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::two_two_two_type_i_multi_panel_restrict:
      j.write_str("two-two-two-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<64>>().to_string());
      break;
    case types::two_eight_one_type_i_multi_panel_restrict:
      j.write_str("two-eight-one-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<32>>().to_string());
      break;
    case types::four_four_one_type_i_multi_panel_restrict:
      j.write_str("four-four-one-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<16>>().to_string());
      break;
    case types::two_four_two_type_i_multi_panel_restrict:
      j.write_str("two-four-two-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<128>>().to_string());
      break;
    case types::four_two_two_type_i_multi_panel_restrict:
      j.write_str("four-two-two-TypeI-MultiPanel-Restriction", c.get<fixed_bitstring<64>>().to_string());
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_");
  }
  j.end_obj();
}
SRSASN_CODE
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two_two_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::two_four_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().pack(bref));
      break;
    case types::four_two_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::two_two_two_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    case types::two_eight_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().pack(bref));
      break;
    case types::four_four_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().pack(bref));
      break;
    case types::two_four_two_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().pack(bref));
      break;
    case types::four_two_two_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two_two_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::two_four_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().unpack(bref));
      break;
    case types::four_two_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::two_two_two_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    case types::two_eight_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().unpack(bref));
      break;
    case types::four_four_one_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().unpack(bref));
      break;
    case types::two_four_two_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().unpack(bref));
      break;
    case types::four_two_two_type_i_multi_panel_restrict:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::destroy_()
{
  switch (type_) {
    case types::type_ii:
      c.destroy<type_ii_s_>();
      break;
    case types::type_ii_port_sel:
      c.destroy<type_ii_port_sel_s_>();
      break;
    default:
      break;
  }
}
void codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type_ii:
      c.init<type_ii_s_>();
      break;
    case types::type_ii_port_sel:
      c.init<type_ii_port_sel_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::sub_type_c_(
    const codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type_ii:
      c.init(other.c.get<type_ii_s_>());
      break;
    case types::type_ii_port_sel:
      c.init(other.c.get<type_ii_port_sel_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::operator=(
    const codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type_ii:
      c.set(other.c.get<type_ii_s_>());
      break;
    case types::type_ii_port_sel:
      c.set(other.c.get<type_ii_port_sel_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_");
  }

  return *this;
}
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii()
{
  set(types::type_ii);
  return c.get<type_ii_s_>();
}
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii_port_sel()
{
  set(types::type_ii_port_sel);
  return c.get<type_ii_port_sel_s_>();
}
void codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type_ii:
      j.write_fieldname("typeII");
      j.start_obj();
      j.write_fieldname("n1-n2-codebookSubsetRestriction");
      c.get<type_ii_s_>().n1_n2_codebook_subset_restrict.to_json(j);
      j.write_str("typeII-RI-Restriction", c.get<type_ii_s_>().type_ii_ri_restrict.to_string());
      j.end_obj();
      break;
    case types::type_ii_port_sel:
      j.write_fieldname("typeII-PortSelection");
      j.start_obj();
      if (c.get<type_ii_port_sel_s_>().port_sel_sampling_size_present) {
        j.write_str("portSelectionSamplingSize", c.get<type_ii_port_sel_s_>().port_sel_sampling_size.to_string());
      }
      j.write_str("typeII-PortSelectionRI-Restriction",
                  c.get<type_ii_port_sel_s_>().type_ii_port_sel_ri_restrict.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type_ii:
      HANDLE_CODE(c.get<type_ii_s_>().n1_n2_codebook_subset_restrict.pack(bref));
      HANDLE_CODE(c.get<type_ii_s_>().type_ii_ri_restrict.pack(bref));
      break;
    case types::type_ii_port_sel:
      HANDLE_CODE(bref.pack(c.get<type_ii_port_sel_s_>().port_sel_sampling_size_present, 1));
      if (c.get<type_ii_port_sel_s_>().port_sel_sampling_size_present) {
        HANDLE_CODE(c.get<type_ii_port_sel_s_>().port_sel_sampling_size.pack(bref));
      }
      HANDLE_CODE(c.get<type_ii_port_sel_s_>().type_ii_port_sel_ri_restrict.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type_ii:
      HANDLE_CODE(c.get<type_ii_s_>().n1_n2_codebook_subset_restrict.unpack(bref));
      HANDLE_CODE(c.get<type_ii_s_>().type_ii_ri_restrict.unpack(bref));
      break;
    case types::type_ii_port_sel:
      HANDLE_CODE(bref.unpack(c.get<type_ii_port_sel_s_>().port_sel_sampling_size_present, 1));
      if (c.get<type_ii_port_sel_s_>().port_sel_sampling_size_present) {
        HANDLE_CODE(c.get<type_ii_port_sel_s_>().port_sel_sampling_size.unpack(bref));
      }
      HANDLE_CODE(c.get<type_ii_port_sel_s_>().type_ii_port_sel_ri_restrict.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::destroy_()
{
  switch (type_) {
    case types::two_one:
      c.destroy<fixed_bitstring<16>>();
      break;
    case types::two_two:
      c.destroy<fixed_bitstring<43>>();
      break;
    case types::four_one:
      c.destroy<fixed_bitstring<32>>();
      break;
    case types::three_two:
      c.destroy<fixed_bitstring<59>>();
      break;
    case types::six_one:
      c.destroy<fixed_bitstring<48>>();
      break;
    case types::four_two:
      c.destroy<fixed_bitstring<75>>();
      break;
    case types::eight_one:
      c.destroy<fixed_bitstring<64>>();
      break;
    case types::four_three:
      c.destroy<fixed_bitstring<107>>();
      break;
    case types::six_two:
      c.destroy<fixed_bitstring<107>>();
      break;
    case types::twelve_one:
      c.destroy<fixed_bitstring<96>>();
      break;
    case types::four_four:
      c.destroy<fixed_bitstring<139>>();
      break;
    case types::eight_two:
      c.destroy<fixed_bitstring<139>>();
      break;
    case types::sixteen_one:
      c.destroy<fixed_bitstring<128>>();
      break;
    default:
      break;
  }
}
void codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set(
    types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::two_one:
      c.init<fixed_bitstring<16>>();
      break;
    case types::two_two:
      c.init<fixed_bitstring<43>>();
      break;
    case types::four_one:
      c.init<fixed_bitstring<32>>();
      break;
    case types::three_two:
      c.init<fixed_bitstring<59>>();
      break;
    case types::six_one:
      c.init<fixed_bitstring<48>>();
      break;
    case types::four_two:
      c.init<fixed_bitstring<75>>();
      break;
    case types::eight_one:
      c.init<fixed_bitstring<64>>();
      break;
    case types::four_three:
      c.init<fixed_bitstring<107>>();
      break;
    case types::six_two:
      c.init<fixed_bitstring<107>>();
      break;
    case types::twelve_one:
      c.init<fixed_bitstring<96>>();
      break;
    case types::four_four:
      c.init<fixed_bitstring<139>>();
      break;
    case types::eight_two:
      c.init<fixed_bitstring<139>>();
      break;
    case types::sixteen_one:
      c.init<fixed_bitstring<128>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::
    n1_n2_codebook_subset_restrict_c_(
        const codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_&
            other)
{
  type_ = other.type();
  switch (type_) {
    case types::two_one:
      c.init(other.c.get<fixed_bitstring<16>>());
      break;
    case types::two_two:
      c.init(other.c.get<fixed_bitstring<43>>());
      break;
    case types::four_one:
      c.init(other.c.get<fixed_bitstring<32>>());
      break;
    case types::three_two:
      c.init(other.c.get<fixed_bitstring<59>>());
      break;
    case types::six_one:
      c.init(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_two:
      c.init(other.c.get<fixed_bitstring<75>>());
      break;
    case types::eight_one:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_three:
      c.init(other.c.get<fixed_bitstring<107>>());
      break;
    case types::six_two:
      c.init(other.c.get<fixed_bitstring<107>>());
      break;
    case types::twelve_one:
      c.init(other.c.get<fixed_bitstring<96>>());
      break;
    case types::four_four:
      c.init(other.c.get<fixed_bitstring<139>>());
      break;
    case types::eight_two:
      c.init(other.c.get<fixed_bitstring<139>>());
      break;
    case types::sixteen_one:
      c.init(other.c.get<fixed_bitstring<128>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_");
  }
}
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::operator=(
    const codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::two_one:
      c.set(other.c.get<fixed_bitstring<16>>());
      break;
    case types::two_two:
      c.set(other.c.get<fixed_bitstring<43>>());
      break;
    case types::four_one:
      c.set(other.c.get<fixed_bitstring<32>>());
      break;
    case types::three_two:
      c.set(other.c.get<fixed_bitstring<59>>());
      break;
    case types::six_one:
      c.set(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_two:
      c.set(other.c.get<fixed_bitstring<75>>());
      break;
    case types::eight_one:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_three:
      c.set(other.c.get<fixed_bitstring<107>>());
      break;
    case types::six_two:
      c.set(other.c.get<fixed_bitstring<107>>());
      break;
    case types::twelve_one:
      c.set(other.c.get<fixed_bitstring<96>>());
      break;
    case types::four_four:
      c.set(other.c.get<fixed_bitstring<139>>());
      break;
    case types::eight_two:
      c.set(other.c.get<fixed_bitstring<139>>());
      break;
    case types::sixteen_one:
      c.set(other.c.get<fixed_bitstring<128>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_");
  }

  return *this;
}
fixed_bitstring<16>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_two_one()
{
  set(types::two_one);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<43>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_two_two()
{
  set(types::two_two);
  return c.get<fixed_bitstring<43>>();
}
fixed_bitstring<32>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_one()
{
  set(types::four_one);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<59>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_three_two()
{
  set(types::three_two);
  return c.get<fixed_bitstring<59>>();
}
fixed_bitstring<48>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_six_one()
{
  set(types::six_one);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<75>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_two()
{
  set(types::four_two);
  return c.get<fixed_bitstring<75>>();
}
fixed_bitstring<64>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_eight_one()
{
  set(types::eight_one);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<107>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_three()
{
  set(types::four_three);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<107>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_six_two()
{
  set(types::six_two);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<96>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_twelve_one()
{
  set(types::twelve_one);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<139>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_four()
{
  set(types::four_four);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<139>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_eight_two()
{
  set(types::eight_two);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<128>& codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::
    n1_n2_codebook_subset_restrict_c_::set_sixteen_one()
{
  set(types::sixteen_one);
  return c.get<fixed_bitstring<128>>();
}
void codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two_one:
      j.write_str("two-one", c.get<fixed_bitstring<16>>().to_string());
      break;
    case types::two_two:
      j.write_str("two-two", c.get<fixed_bitstring<43>>().to_string());
      break;
    case types::four_one:
      j.write_str("four-one", c.get<fixed_bitstring<32>>().to_string());
      break;
    case types::three_two:
      j.write_str("three-two", c.get<fixed_bitstring<59>>().to_string());
      break;
    case types::six_one:
      j.write_str("six-one", c.get<fixed_bitstring<48>>().to_string());
      break;
    case types::four_two:
      j.write_str("four-two", c.get<fixed_bitstring<75>>().to_string());
      break;
    case types::eight_one:
      j.write_str("eight-one", c.get<fixed_bitstring<64>>().to_string());
      break;
    case types::four_three:
      j.write_str("four-three", c.get<fixed_bitstring<107>>().to_string());
      break;
    case types::six_two:
      j.write_str("six-two", c.get<fixed_bitstring<107>>().to_string());
      break;
    case types::twelve_one:
      j.write_str("twelve-one", c.get<fixed_bitstring<96>>().to_string());
      break;
    case types::four_four:
      j.write_str("four-four", c.get<fixed_bitstring<139>>().to_string());
      break;
    case types::eight_two:
      j.write_str("eight-two", c.get<fixed_bitstring<139>>().to_string());
      break;
    case types::sixteen_one:
      j.write_str("sixteen-one", c.get<fixed_bitstring<128>>().to_string());
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_");
  }
  j.end_obj();
}
SRSASN_CODE
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::pack(
    bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two_one:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().pack(bref));
      break;
    case types::two_two:
      HANDLE_CODE(c.get<fixed_bitstring<43>>().pack(bref));
      break;
    case types::four_one:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().pack(bref));
      break;
    case types::three_two:
      HANDLE_CODE(c.get<fixed_bitstring<59>>().pack(bref));
      break;
    case types::six_one:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().pack(bref));
      break;
    case types::four_two:
      HANDLE_CODE(c.get<fixed_bitstring<75>>().pack(bref));
      break;
    case types::eight_one:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    case types::four_three:
      HANDLE_CODE(c.get<fixed_bitstring<107>>().pack(bref));
      break;
    case types::six_two:
      HANDLE_CODE(c.get<fixed_bitstring<107>>().pack(bref));
      break;
    case types::twelve_one:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().pack(bref));
      break;
    case types::four_four:
      HANDLE_CODE(c.get<fixed_bitstring<139>>().pack(bref));
      break;
    case types::eight_two:
      HANDLE_CODE(c.get<fixed_bitstring<139>>().pack(bref));
      break;
    case types::sixteen_one:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::unpack(
    cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two_one:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().unpack(bref));
      break;
    case types::two_two:
      HANDLE_CODE(c.get<fixed_bitstring<43>>().unpack(bref));
      break;
    case types::four_one:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().unpack(bref));
      break;
    case types::three_two:
      HANDLE_CODE(c.get<fixed_bitstring<59>>().unpack(bref));
      break;
    case types::six_one:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().unpack(bref));
      break;
    case types::four_two:
      HANDLE_CODE(c.get<fixed_bitstring<75>>().unpack(bref));
      break;
    case types::eight_one:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    case types::four_three:
      HANDLE_CODE(c.get<fixed_bitstring<107>>().unpack(bref));
      break;
    case types::six_two:
      HANDLE_CODE(c.get<fixed_bitstring<107>>().unpack(bref));
      break;
    case types::twelve_one:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().unpack(bref));
      break;
    case types::four_four:
      HANDLE_CODE(c.get<fixed_bitstring<139>>().unpack(bref));
      break;
    case types::eight_two:
      HANDLE_CODE(c.get<fixed_bitstring<139>>().unpack(bref));
      break;
    case types::sixteen_one:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_opts::to_string()
    const
{
  static const char* names[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(
      names,
      4,
      value,
      "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_e_");
}
uint8_t
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_opts::to_number()
    const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(
      numbers,
      4,
      value,
      "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_e_");
}

const char* codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_opts::to_string() const
{
  static const char* names[] = {"n4", "n8"};
  return convert_enum_idx(names, 2, value, "codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_e_");
}
uint8_t codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8};
  return map_enum_number(numbers, 2, value, "codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_e_");
}

const char* codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::to_string() const
{
  static const char* names[] = {"two", "three", "four"};
  return convert_enum_idx(names, 3, value, "codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_e_");
}
uint8_t codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4};
  return map_enum_number(numbers, 3, value, "codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_e_");
}

// PUCCH-CSI-Resource ::= SEQUENCE
SRSASN_CODE pucch_csi_res_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ul_bw_part_id, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(pack_integer(bref, pucch_res, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_csi_res_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ul_bw_part_id, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(unpack_integer(pucch_res, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void pucch_csi_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("uplinkBandwidthPartId", ul_bw_part_id);
  j.write_int("pucch-Resource", pucch_res);
  j.end_obj();
}

// PortIndexFor8Ranks ::= CHOICE
void port_idx_for8_ranks_c::destroy_()
{
  switch (type_) {
    case types::port_idx8:
      c.destroy<port_idx8_s_>();
      break;
    case types::port_idx4:
      c.destroy<port_idx4_s_>();
      break;
    case types::port_idx2:
      c.destroy<port_idx2_s_>();
      break;
    default:
      break;
  }
}
void port_idx_for8_ranks_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::port_idx8:
      c.init<port_idx8_s_>();
      break;
    case types::port_idx4:
      c.init<port_idx4_s_>();
      break;
    case types::port_idx2:
      c.init<port_idx2_s_>();
      break;
    case types::port_idx1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "port_idx_for8_ranks_c");
  }
}
port_idx_for8_ranks_c::port_idx_for8_ranks_c(const port_idx_for8_ranks_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::port_idx8:
      c.init(other.c.get<port_idx8_s_>());
      break;
    case types::port_idx4:
      c.init(other.c.get<port_idx4_s_>());
      break;
    case types::port_idx2:
      c.init(other.c.get<port_idx2_s_>());
      break;
    case types::port_idx1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "port_idx_for8_ranks_c");
  }
}
port_idx_for8_ranks_c& port_idx_for8_ranks_c::operator=(const port_idx_for8_ranks_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::port_idx8:
      c.set(other.c.get<port_idx8_s_>());
      break;
    case types::port_idx4:
      c.set(other.c.get<port_idx4_s_>());
      break;
    case types::port_idx2:
      c.set(other.c.get<port_idx2_s_>());
      break;
    case types::port_idx1:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "port_idx_for8_ranks_c");
  }

  return *this;
}
port_idx_for8_ranks_c::port_idx8_s_& port_idx_for8_ranks_c::set_port_idx8()
{
  set(types::port_idx8);
  return c.get<port_idx8_s_>();
}
port_idx_for8_ranks_c::port_idx4_s_& port_idx_for8_ranks_c::set_port_idx4()
{
  set(types::port_idx4);
  return c.get<port_idx4_s_>();
}
port_idx_for8_ranks_c::port_idx2_s_& port_idx_for8_ranks_c::set_port_idx2()
{
  set(types::port_idx2);
  return c.get<port_idx2_s_>();
}
void port_idx_for8_ranks_c::set_port_idx1()
{
  set(types::port_idx1);
}
void port_idx_for8_ranks_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::port_idx8:
      j.write_fieldname("portIndex8");
      j.start_obj();
      if (c.get<port_idx8_s_>().rank1_8_present) {
        j.write_int("rank1-8", c.get<port_idx8_s_>().rank1_8);
      }
      if (c.get<port_idx8_s_>().rank2_8_present) {
        j.start_array("rank2-8");
        for (const auto& e1 : c.get<port_idx8_s_>().rank2_8) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx8_s_>().rank3_8_present) {
        j.start_array("rank3-8");
        for (const auto& e1 : c.get<port_idx8_s_>().rank3_8) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx8_s_>().rank4_8_present) {
        j.start_array("rank4-8");
        for (const auto& e1 : c.get<port_idx8_s_>().rank4_8) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx8_s_>().rank5_8_present) {
        j.start_array("rank5-8");
        for (const auto& e1 : c.get<port_idx8_s_>().rank5_8) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx8_s_>().rank6_8_present) {
        j.start_array("rank6-8");
        for (const auto& e1 : c.get<port_idx8_s_>().rank6_8) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx8_s_>().rank7_8_present) {
        j.start_array("rank7-8");
        for (const auto& e1 : c.get<port_idx8_s_>().rank7_8) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx8_s_>().rank8_8_present) {
        j.start_array("rank8-8");
        for (const auto& e1 : c.get<port_idx8_s_>().rank8_8) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    case types::port_idx4:
      j.write_fieldname("portIndex4");
      j.start_obj();
      if (c.get<port_idx4_s_>().rank1_4_present) {
        j.write_int("rank1-4", c.get<port_idx4_s_>().rank1_4);
      }
      if (c.get<port_idx4_s_>().rank2_4_present) {
        j.start_array("rank2-4");
        for (const auto& e1 : c.get<port_idx4_s_>().rank2_4) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx4_s_>().rank3_4_present) {
        j.start_array("rank3-4");
        for (const auto& e1 : c.get<port_idx4_s_>().rank3_4) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<port_idx4_s_>().rank4_4_present) {
        j.start_array("rank4-4");
        for (const auto& e1 : c.get<port_idx4_s_>().rank4_4) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    case types::port_idx2:
      j.write_fieldname("portIndex2");
      j.start_obj();
      if (c.get<port_idx2_s_>().rank1_2_present) {
        j.write_int("rank1-2", c.get<port_idx2_s_>().rank1_2);
      }
      if (c.get<port_idx2_s_>().rank2_2_present) {
        j.start_array("rank2-2");
        for (const auto& e1 : c.get<port_idx2_s_>().rank2_2) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    case types::port_idx1:
      break;
    default:
      log_invalid_choice_id(type_, "port_idx_for8_ranks_c");
  }
  j.end_obj();
}
SRSASN_CODE port_idx_for8_ranks_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::port_idx8:
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank1_8_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank2_8_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank3_8_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank4_8_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank5_8_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank6_8_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank7_8_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx8_s_>().rank8_8_present, 1));
      if (c.get<port_idx8_s_>().rank1_8_present) {
        HANDLE_CODE(pack_integer(bref, c.get<port_idx8_s_>().rank1_8, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.get<port_idx8_s_>().rank2_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx8_s_>().rank2_8)[0],
                                      c.get<port_idx8_s_>().rank2_8.size(),
                                      integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank3_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx8_s_>().rank3_8)[0],
                                      c.get<port_idx8_s_>().rank3_8.size(),
                                      integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank4_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx8_s_>().rank4_8)[0],
                                      c.get<port_idx8_s_>().rank4_8.size(),
                                      integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank5_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx8_s_>().rank5_8)[0],
                                      c.get<port_idx8_s_>().rank5_8.size(),
                                      integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank6_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx8_s_>().rank6_8)[0],
                                      c.get<port_idx8_s_>().rank6_8.size(),
                                      integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank7_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx8_s_>().rank7_8)[0],
                                      c.get<port_idx8_s_>().rank7_8.size(),
                                      integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank8_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx8_s_>().rank8_8)[0],
                                      c.get<port_idx8_s_>().rank8_8.size(),
                                      integer_packer<uint8_t>(0, 7)));
      }
      break;
    case types::port_idx4:
      HANDLE_CODE(bref.pack(c.get<port_idx4_s_>().rank1_4_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx4_s_>().rank2_4_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx4_s_>().rank3_4_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx4_s_>().rank4_4_present, 1));
      if (c.get<port_idx4_s_>().rank1_4_present) {
        HANDLE_CODE(pack_integer(bref, c.get<port_idx4_s_>().rank1_4, (uint8_t)0u, (uint8_t)3u));
      }
      if (c.get<port_idx4_s_>().rank2_4_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx4_s_>().rank2_4)[0],
                                      c.get<port_idx4_s_>().rank2_4.size(),
                                      integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank3_4_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx4_s_>().rank3_4)[0],
                                      c.get<port_idx4_s_>().rank3_4.size(),
                                      integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank4_4_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx4_s_>().rank4_4)[0],
                                      c.get<port_idx4_s_>().rank4_4.size(),
                                      integer_packer<uint8_t>(0, 3)));
      }
      break;
    case types::port_idx2:
      HANDLE_CODE(bref.pack(c.get<port_idx2_s_>().rank1_2_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx2_s_>().rank2_2_present, 1));
      if (c.get<port_idx2_s_>().rank1_2_present) {
        HANDLE_CODE(pack_integer(bref, c.get<port_idx2_s_>().rank1_2, (uint8_t)0u, (uint8_t)1u));
      }
      if (c.get<port_idx2_s_>().rank2_2_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<port_idx2_s_>().rank2_2)[0],
                                      c.get<port_idx2_s_>().rank2_2.size(),
                                      integer_packer<uint8_t>(0, 1)));
      }
      break;
    case types::port_idx1:
      break;
    default:
      log_invalid_choice_id(type_, "port_idx_for8_ranks_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE port_idx_for8_ranks_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::port_idx8:
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank1_8_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank2_8_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank3_8_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank4_8_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank5_8_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank6_8_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank7_8_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx8_s_>().rank8_8_present, 1));
      if (c.get<port_idx8_s_>().rank1_8_present) {
        HANDLE_CODE(unpack_integer(c.get<port_idx8_s_>().rank1_8, bref, (uint8_t)0u, (uint8_t)7u));
      }
      if (c.get<port_idx8_s_>().rank2_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx8_s_>().rank2_8)[0],
                                        bref,
                                        c.get<port_idx8_s_>().rank2_8.size(),
                                        integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank3_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx8_s_>().rank3_8)[0],
                                        bref,
                                        c.get<port_idx8_s_>().rank3_8.size(),
                                        integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank4_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx8_s_>().rank4_8)[0],
                                        bref,
                                        c.get<port_idx8_s_>().rank4_8.size(),
                                        integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank5_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx8_s_>().rank5_8)[0],
                                        bref,
                                        c.get<port_idx8_s_>().rank5_8.size(),
                                        integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank6_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx8_s_>().rank6_8)[0],
                                        bref,
                                        c.get<port_idx8_s_>().rank6_8.size(),
                                        integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank7_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx8_s_>().rank7_8)[0],
                                        bref,
                                        c.get<port_idx8_s_>().rank7_8.size(),
                                        integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank8_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx8_s_>().rank8_8)[0],
                                        bref,
                                        c.get<port_idx8_s_>().rank8_8.size(),
                                        integer_packer<uint8_t>(0, 7)));
      }
      break;
    case types::port_idx4:
      HANDLE_CODE(bref.unpack(c.get<port_idx4_s_>().rank1_4_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx4_s_>().rank2_4_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx4_s_>().rank3_4_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx4_s_>().rank4_4_present, 1));
      if (c.get<port_idx4_s_>().rank1_4_present) {
        HANDLE_CODE(unpack_integer(c.get<port_idx4_s_>().rank1_4, bref, (uint8_t)0u, (uint8_t)3u));
      }
      if (c.get<port_idx4_s_>().rank2_4_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx4_s_>().rank2_4)[0],
                                        bref,
                                        c.get<port_idx4_s_>().rank2_4.size(),
                                        integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank3_4_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx4_s_>().rank3_4)[0],
                                        bref,
                                        c.get<port_idx4_s_>().rank3_4.size(),
                                        integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank4_4_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx4_s_>().rank4_4)[0],
                                        bref,
                                        c.get<port_idx4_s_>().rank4_4.size(),
                                        integer_packer<uint8_t>(0, 3)));
      }
      break;
    case types::port_idx2:
      HANDLE_CODE(bref.unpack(c.get<port_idx2_s_>().rank1_2_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx2_s_>().rank2_2_present, 1));
      if (c.get<port_idx2_s_>().rank1_2_present) {
        HANDLE_CODE(unpack_integer(c.get<port_idx2_s_>().rank1_2, bref, (uint8_t)0u, (uint8_t)1u));
      }
      if (c.get<port_idx2_s_>().rank2_2_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<port_idx2_s_>().rank2_2)[0],
                                        bref,
                                        c.get<port_idx2_s_>().rank2_2.size(),
                                        integer_packer<uint8_t>(0, 1)));
      }
      break;
    case types::port_idx1:
      break;
    default:
      log_invalid_choice_id(type_, "port_idx_for8_ranks_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CSI-ReportConfig ::= SEQUENCE
SRSASN_CODE csi_report_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_present, 1));
  HANDLE_CODE(bref.pack(csi_im_res_for_interference_present, 1));
  HANDLE_CODE(bref.pack(nzp_csi_rs_res_for_interference_present, 1));
  HANDLE_CODE(bref.pack(report_freq_cfg_present, 1));
  HANDLE_CODE(bref.pack(codebook_cfg_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(cqi_table_present, 1));
  HANDLE_CODE(bref.pack(non_pmi_port_ind.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, report_cfg_id, (uint8_t)0u, (uint8_t)47u));
  if (carrier_present) {
    HANDLE_CODE(pack_integer(bref, carrier, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, res_for_ch_meas, (uint8_t)0u, (uint8_t)111u));
  if (csi_im_res_for_interference_present) {
    HANDLE_CODE(pack_integer(bref, csi_im_res_for_interference, (uint8_t)0u, (uint8_t)111u));
  }
  if (nzp_csi_rs_res_for_interference_present) {
    HANDLE_CODE(pack_integer(bref, nzp_csi_rs_res_for_interference, (uint8_t)0u, (uint8_t)111u));
  }
  HANDLE_CODE(report_cfg_type.pack(bref));
  HANDLE_CODE(report_quant.pack(bref));
  if (report_freq_cfg_present) {
    HANDLE_CODE(bref.pack(report_freq_cfg.cqi_format_ind_present, 1));
    HANDLE_CODE(bref.pack(report_freq_cfg.pmi_format_ind_present, 1));
    HANDLE_CODE(bref.pack(report_freq_cfg.csi_report_band_present, 1));
    if (report_freq_cfg.cqi_format_ind_present) {
      HANDLE_CODE(report_freq_cfg.cqi_format_ind.pack(bref));
    }
    if (report_freq_cfg.pmi_format_ind_present) {
      HANDLE_CODE(report_freq_cfg.pmi_format_ind.pack(bref));
    }
    if (report_freq_cfg.csi_report_band_present) {
      HANDLE_CODE(report_freq_cfg.csi_report_band.pack(bref));
    }
  }
  HANDLE_CODE(time_restrict_for_ch_meass.pack(bref));
  HANDLE_CODE(time_restrict_for_interference_meass.pack(bref));
  if (codebook_cfg_present) {
    HANDLE_CODE(codebook_cfg.pack(bref));
  }
  if (dummy_present) {
    HANDLE_CODE(dummy.pack(bref));
  }
  HANDLE_CODE(group_based_beam_report.pack(bref));
  if (cqi_table_present) {
    HANDLE_CODE(cqi_table.pack(bref));
  }
  HANDLE_CODE(subband_size.pack(bref));
  if (non_pmi_port_ind.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, non_pmi_port_ind, 1, 128));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= semi_persistent_on_pusch_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1530.is_present(), 1));
      if (semi_persistent_on_pusch_v1530.is_present()) {
        HANDLE_CODE(semi_persistent_on_pusch_v1530->report_slot_cfg_v1530.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(carrier_present, 1));
  HANDLE_CODE(bref.unpack(csi_im_res_for_interference_present, 1));
  HANDLE_CODE(bref.unpack(nzp_csi_rs_res_for_interference_present, 1));
  HANDLE_CODE(bref.unpack(report_freq_cfg_present, 1));
  HANDLE_CODE(bref.unpack(codebook_cfg_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(cqi_table_present, 1));
  bool non_pmi_port_ind_present;
  HANDLE_CODE(bref.unpack(non_pmi_port_ind_present, 1));

  HANDLE_CODE(unpack_integer(report_cfg_id, bref, (uint8_t)0u, (uint8_t)47u));
  if (carrier_present) {
    HANDLE_CODE(unpack_integer(carrier, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(res_for_ch_meas, bref, (uint8_t)0u, (uint8_t)111u));
  if (csi_im_res_for_interference_present) {
    HANDLE_CODE(unpack_integer(csi_im_res_for_interference, bref, (uint8_t)0u, (uint8_t)111u));
  }
  if (nzp_csi_rs_res_for_interference_present) {
    HANDLE_CODE(unpack_integer(nzp_csi_rs_res_for_interference, bref, (uint8_t)0u, (uint8_t)111u));
  }
  HANDLE_CODE(report_cfg_type.unpack(bref));
  HANDLE_CODE(report_quant.unpack(bref));
  if (report_freq_cfg_present) {
    HANDLE_CODE(bref.unpack(report_freq_cfg.cqi_format_ind_present, 1));
    HANDLE_CODE(bref.unpack(report_freq_cfg.pmi_format_ind_present, 1));
    HANDLE_CODE(bref.unpack(report_freq_cfg.csi_report_band_present, 1));
    if (report_freq_cfg.cqi_format_ind_present) {
      HANDLE_CODE(report_freq_cfg.cqi_format_ind.unpack(bref));
    }
    if (report_freq_cfg.pmi_format_ind_present) {
      HANDLE_CODE(report_freq_cfg.pmi_format_ind.unpack(bref));
    }
    if (report_freq_cfg.csi_report_band_present) {
      HANDLE_CODE(report_freq_cfg.csi_report_band.unpack(bref));
    }
  }
  HANDLE_CODE(time_restrict_for_ch_meass.unpack(bref));
  HANDLE_CODE(time_restrict_for_interference_meass.unpack(bref));
  if (codebook_cfg_present) {
    HANDLE_CODE(codebook_cfg.unpack(bref));
  }
  if (dummy_present) {
    HANDLE_CODE(dummy.unpack(bref));
  }
  HANDLE_CODE(group_based_beam_report.unpack(bref));
  if (cqi_table_present) {
    HANDLE_CODE(cqi_table.unpack(bref));
  }
  HANDLE_CODE(subband_size.unpack(bref));
  if (non_pmi_port_ind_present) {
    HANDLE_CODE(unpack_dyn_seq_of(non_pmi_port_ind, bref, 1, 128));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool semi_persistent_on_pusch_v1530_present;
      HANDLE_CODE(bref.unpack(semi_persistent_on_pusch_v1530_present, 1));
      semi_persistent_on_pusch_v1530.set_present(semi_persistent_on_pusch_v1530_present);
      if (semi_persistent_on_pusch_v1530.is_present()) {
        HANDLE_CODE(semi_persistent_on_pusch_v1530->report_slot_cfg_v1530.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void csi_report_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reportConfigId", report_cfg_id);
  if (carrier_present) {
    j.write_int("carrier", carrier);
  }
  j.write_int("resourcesForChannelMeasurement", res_for_ch_meas);
  if (csi_im_res_for_interference_present) {
    j.write_int("csi-IM-ResourcesForInterference", csi_im_res_for_interference);
  }
  if (nzp_csi_rs_res_for_interference_present) {
    j.write_int("nzp-CSI-RS-ResourcesForInterference", nzp_csi_rs_res_for_interference);
  }
  j.write_fieldname("reportConfigType");
  report_cfg_type.to_json(j);
  j.write_fieldname("reportQuantity");
  report_quant.to_json(j);
  if (report_freq_cfg_present) {
    j.write_fieldname("reportFreqConfiguration");
    j.start_obj();
    if (report_freq_cfg.cqi_format_ind_present) {
      j.write_str("cqi-FormatIndicator", report_freq_cfg.cqi_format_ind.to_string());
    }
    if (report_freq_cfg.pmi_format_ind_present) {
      j.write_str("pmi-FormatIndicator", report_freq_cfg.pmi_format_ind.to_string());
    }
    if (report_freq_cfg.csi_report_band_present) {
      j.write_fieldname("csi-ReportingBand");
      report_freq_cfg.csi_report_band.to_json(j);
    }
    j.end_obj();
  }
  j.write_str("timeRestrictionForChannelMeasurements", time_restrict_for_ch_meass.to_string());
  j.write_str("timeRestrictionForInterferenceMeasurements", time_restrict_for_interference_meass.to_string());
  if (codebook_cfg_present) {
    j.write_fieldname("codebookConfig");
    codebook_cfg.to_json(j);
  }
  if (dummy_present) {
    j.write_str("dummy", dummy.to_string());
  }
  j.write_fieldname("groupBasedBeamReporting");
  group_based_beam_report.to_json(j);
  if (cqi_table_present) {
    j.write_str("cqi-Table", cqi_table.to_string());
  }
  j.write_str("subbandSize", subband_size.to_string());
  if (non_pmi_port_ind.size() > 0) {
    j.start_array("non-PMI-PortIndication");
    for (const auto& e1 : non_pmi_port_ind) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (semi_persistent_on_pusch_v1530.is_present()) {
      j.write_fieldname("semiPersistentOnPUSCH-v1530");
      j.start_obj();
      j.write_str("reportSlotConfig-v1530", semi_persistent_on_pusch_v1530->report_slot_cfg_v1530.to_string());
      j.end_obj();
    }
  }
  j.end_obj();
}

void csi_report_cfg_s::report_cfg_type_c_::destroy_()
{
  switch (type_) {
    case types::periodic:
      c.destroy<periodic_s_>();
      break;
    case types::semi_persistent_on_pucch:
      c.destroy<semi_persistent_on_pucch_s_>();
      break;
    case types::semi_persistent_on_pusch:
      c.destroy<semi_persistent_on_pusch_s_>();
      break;
    case types::aperiodic:
      c.destroy<aperiodic_s_>();
      break;
    default:
      break;
  }
}
void csi_report_cfg_s::report_cfg_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::periodic:
      c.init<periodic_s_>();
      break;
    case types::semi_persistent_on_pucch:
      c.init<semi_persistent_on_pucch_s_>();
      break;
    case types::semi_persistent_on_pusch:
      c.init<semi_persistent_on_pusch_s_>();
      break;
    case types::aperiodic:
      c.init<aperiodic_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_cfg_type_c_");
  }
}
csi_report_cfg_s::report_cfg_type_c_::report_cfg_type_c_(const csi_report_cfg_s::report_cfg_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::periodic:
      c.init(other.c.get<periodic_s_>());
      break;
    case types::semi_persistent_on_pucch:
      c.init(other.c.get<semi_persistent_on_pucch_s_>());
      break;
    case types::semi_persistent_on_pusch:
      c.init(other.c.get<semi_persistent_on_pusch_s_>());
      break;
    case types::aperiodic:
      c.init(other.c.get<aperiodic_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_cfg_type_c_");
  }
}
csi_report_cfg_s::report_cfg_type_c_&
csi_report_cfg_s::report_cfg_type_c_::operator=(const csi_report_cfg_s::report_cfg_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::periodic:
      c.set(other.c.get<periodic_s_>());
      break;
    case types::semi_persistent_on_pucch:
      c.set(other.c.get<semi_persistent_on_pucch_s_>());
      break;
    case types::semi_persistent_on_pusch:
      c.set(other.c.get<semi_persistent_on_pusch_s_>());
      break;
    case types::aperiodic:
      c.set(other.c.get<aperiodic_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_cfg_type_c_");
  }

  return *this;
}
csi_report_cfg_s::report_cfg_type_c_::periodic_s_& csi_report_cfg_s::report_cfg_type_c_::set_periodic()
{
  set(types::periodic);
  return c.get<periodic_s_>();
}
csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pucch_s_&
csi_report_cfg_s::report_cfg_type_c_::set_semi_persistent_on_pucch()
{
  set(types::semi_persistent_on_pucch);
  return c.get<semi_persistent_on_pucch_s_>();
}
csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_&
csi_report_cfg_s::report_cfg_type_c_::set_semi_persistent_on_pusch()
{
  set(types::semi_persistent_on_pusch);
  return c.get<semi_persistent_on_pusch_s_>();
}
csi_report_cfg_s::report_cfg_type_c_::aperiodic_s_& csi_report_cfg_s::report_cfg_type_c_::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<aperiodic_s_>();
}
void csi_report_cfg_s::report_cfg_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::periodic:
      j.write_fieldname("periodic");
      j.start_obj();
      j.write_fieldname("reportSlotConfig");
      c.get<periodic_s_>().report_slot_cfg.to_json(j);
      j.start_array("pucch-CSI-ResourceList");
      for (const auto& e1 : c.get<periodic_s_>().pucch_csi_res_list) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    case types::semi_persistent_on_pucch:
      j.write_fieldname("semiPersistentOnPUCCH");
      j.start_obj();
      j.write_fieldname("reportSlotConfig");
      c.get<semi_persistent_on_pucch_s_>().report_slot_cfg.to_json(j);
      j.start_array("pucch-CSI-ResourceList");
      for (const auto& e1 : c.get<semi_persistent_on_pucch_s_>().pucch_csi_res_list) {
        e1.to_json(j);
      }
      j.end_array();
      j.end_obj();
      break;
    case types::semi_persistent_on_pusch:
      j.write_fieldname("semiPersistentOnPUSCH");
      j.start_obj();
      j.write_str("reportSlotConfig", c.get<semi_persistent_on_pusch_s_>().report_slot_cfg.to_string());
      j.start_array("reportSlotOffsetList");
      for (const auto& e1 : c.get<semi_persistent_on_pusch_s_>().report_slot_offset_list) {
        j.write_int(e1);
      }
      j.end_array();
      j.write_int("p0alpha", c.get<semi_persistent_on_pusch_s_>().p0alpha);
      j.end_obj();
      break;
    case types::aperiodic:
      j.write_fieldname("aperiodic");
      j.start_obj();
      j.start_array("reportSlotOffsetList");
      for (const auto& e1 : c.get<aperiodic_s_>().report_slot_offset_list) {
        j.write_int(e1);
      }
      j.end_array();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_cfg_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_report_cfg_s::report_cfg_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::periodic:
      HANDLE_CODE(c.get<periodic_s_>().report_slot_cfg.pack(bref));
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<periodic_s_>().pucch_csi_res_list, 1, 4));
      break;
    case types::semi_persistent_on_pucch:
      HANDLE_CODE(c.get<semi_persistent_on_pucch_s_>().report_slot_cfg.pack(bref));
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<semi_persistent_on_pucch_s_>().pucch_csi_res_list, 1, 4));
      break;
    case types::semi_persistent_on_pusch:
      HANDLE_CODE(c.get<semi_persistent_on_pusch_s_>().report_slot_cfg.pack(bref));
      HANDLE_CODE(pack_dyn_seq_of(
          bref, c.get<semi_persistent_on_pusch_s_>().report_slot_offset_list, 1, 16, integer_packer<uint8_t>(0, 32)));
      HANDLE_CODE(pack_integer(bref, c.get<semi_persistent_on_pusch_s_>().p0alpha, (uint8_t)0u, (uint8_t)29u));
      break;
    case types::aperiodic:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<aperiodic_s_>().report_slot_offset_list, 1, 16, integer_packer<uint8_t>(0, 32)));
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_cfg_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_cfg_s::report_cfg_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::periodic:
      HANDLE_CODE(c.get<periodic_s_>().report_slot_cfg.unpack(bref));
      HANDLE_CODE(unpack_dyn_seq_of(c.get<periodic_s_>().pucch_csi_res_list, bref, 1, 4));
      break;
    case types::semi_persistent_on_pucch:
      HANDLE_CODE(c.get<semi_persistent_on_pucch_s_>().report_slot_cfg.unpack(bref));
      HANDLE_CODE(unpack_dyn_seq_of(c.get<semi_persistent_on_pucch_s_>().pucch_csi_res_list, bref, 1, 4));
      break;
    case types::semi_persistent_on_pusch:
      HANDLE_CODE(c.get<semi_persistent_on_pusch_s_>().report_slot_cfg.unpack(bref));
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<semi_persistent_on_pusch_s_>().report_slot_offset_list, bref, 1, 16, integer_packer<uint8_t>(0, 32)));
      HANDLE_CODE(unpack_integer(c.get<semi_persistent_on_pusch_s_>().p0alpha, bref, (uint8_t)0u, (uint8_t)29u));
      break;
    case types::aperiodic:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<aperiodic_s_>().report_slot_offset_list, bref, 1, 16, integer_packer<uint8_t>(0, 32)));
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_cfg_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::to_string() const
{
  static const char* names[] = {"sl5", "sl10", "sl20", "sl40", "sl80", "sl160", "sl320"};
  return convert_enum_idx(
      names, 7, value, "csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_e_");
}
uint16_t csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 40, 80, 160, 320};
  return map_enum_number(
      numbers, 7, value, "csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_e_");
}

void csi_report_cfg_s::report_quant_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::report_quant_c_::set_none()
{
  set(types::none);
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_pmi_cqi()
{
  set(types::cri_ri_pmi_cqi);
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_i1()
{
  set(types::cri_ri_i1);
}
csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_& csi_report_cfg_s::report_quant_c_::set_cri_ri_i1_cqi()
{
  set(types::cri_ri_i1_cqi);
  return c;
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_cqi()
{
  set(types::cri_ri_cqi);
}
void csi_report_cfg_s::report_quant_c_::set_cri_rsrp()
{
  set(types::cri_rsrp);
}
void csi_report_cfg_s::report_quant_c_::set_ssb_idx_rsrp()
{
  set(types::ssb_idx_rsrp);
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_li_pmi_cqi()
{
  set(types::cri_ri_li_pmi_cqi);
}
void csi_report_cfg_s::report_quant_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::none:
      break;
    case types::cri_ri_pmi_cqi:
      break;
    case types::cri_ri_i1:
      break;
    case types::cri_ri_i1_cqi:
      j.write_fieldname("cri-RI-i1-CQI");
      j.start_obj();
      if (c.pdsch_bundle_size_for_csi_present) {
        j.write_str("pdsch-BundleSizeForCSI", c.pdsch_bundle_size_for_csi.to_string());
      }
      j.end_obj();
      break;
    case types::cri_ri_cqi:
      break;
    case types::cri_rsrp:
      break;
    case types::ssb_idx_rsrp:
      break;
    case types::cri_ri_li_pmi_cqi:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_quant_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_report_cfg_s::report_quant_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::none:
      break;
    case types::cri_ri_pmi_cqi:
      break;
    case types::cri_ri_i1:
      break;
    case types::cri_ri_i1_cqi:
      HANDLE_CODE(bref.pack(c.pdsch_bundle_size_for_csi_present, 1));
      if (c.pdsch_bundle_size_for_csi_present) {
        HANDLE_CODE(c.pdsch_bundle_size_for_csi.pack(bref));
      }
      break;
    case types::cri_ri_cqi:
      break;
    case types::cri_rsrp:
      break;
    case types::ssb_idx_rsrp:
      break;
    case types::cri_ri_li_pmi_cqi:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_quant_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_cfg_s::report_quant_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::none:
      break;
    case types::cri_ri_pmi_cqi:
      break;
    case types::cri_ri_i1:
      break;
    case types::cri_ri_i1_cqi:
      HANDLE_CODE(bref.unpack(c.pdsch_bundle_size_for_csi_present, 1));
      if (c.pdsch_bundle_size_for_csi_present) {
        HANDLE_CODE(c.pdsch_bundle_size_for_csi.unpack(bref));
      }
      break;
    case types::cri_ri_cqi:
      break;
    case types::cri_rsrp:
      break;
    case types::ssb_idx_rsrp:
      break;
    case types::cri_ri_li_pmi_cqi:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_quant_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(
      names, 2, value, "csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_e_");
}
uint8_t csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(
      numbers, 2, value, "csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_e_");
}

const char* csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::to_string() const
{
  static const char* names[] = {"widebandCQI", "subbandCQI"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_e_");
}

const char* csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_opts::to_string() const
{
  static const char* names[] = {"widebandPMI", "subbandPMI"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_e_");
}

void csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::destroy_()
{
  switch (type_) {
    case types::subbands3:
      c.destroy<fixed_bitstring<3>>();
      break;
    case types::subbands4:
      c.destroy<fixed_bitstring<4>>();
      break;
    case types::subbands5:
      c.destroy<fixed_bitstring<5>>();
      break;
    case types::subbands6:
      c.destroy<fixed_bitstring<6>>();
      break;
    case types::subbands7:
      c.destroy<fixed_bitstring<7>>();
      break;
    case types::subbands8:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::subbands9:
      c.destroy<fixed_bitstring<9>>();
      break;
    case types::subbands10:
      c.destroy<fixed_bitstring<10>>();
      break;
    case types::subbands11:
      c.destroy<fixed_bitstring<11>>();
      break;
    case types::subbands12:
      c.destroy<fixed_bitstring<12>>();
      break;
    case types::subbands13:
      c.destroy<fixed_bitstring<13>>();
      break;
    case types::subbands14:
      c.destroy<fixed_bitstring<14>>();
      break;
    case types::subbands15:
      c.destroy<fixed_bitstring<15>>();
      break;
    case types::subbands16:
      c.destroy<fixed_bitstring<16>>();
      break;
    case types::subbands17:
      c.destroy<fixed_bitstring<17>>();
      break;
    case types::subbands18:
      c.destroy<fixed_bitstring<18>>();
      break;
    case types::subbands19_v1530:
      c.destroy<fixed_bitstring<19>>();
      break;
    default:
      break;
  }
}
void csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::subbands3:
      c.init<fixed_bitstring<3>>();
      break;
    case types::subbands4:
      c.init<fixed_bitstring<4>>();
      break;
    case types::subbands5:
      c.init<fixed_bitstring<5>>();
      break;
    case types::subbands6:
      c.init<fixed_bitstring<6>>();
      break;
    case types::subbands7:
      c.init<fixed_bitstring<7>>();
      break;
    case types::subbands8:
      c.init<fixed_bitstring<8>>();
      break;
    case types::subbands9:
      c.init<fixed_bitstring<9>>();
      break;
    case types::subbands10:
      c.init<fixed_bitstring<10>>();
      break;
    case types::subbands11:
      c.init<fixed_bitstring<11>>();
      break;
    case types::subbands12:
      c.init<fixed_bitstring<12>>();
      break;
    case types::subbands13:
      c.init<fixed_bitstring<13>>();
      break;
    case types::subbands14:
      c.init<fixed_bitstring<14>>();
      break;
    case types::subbands15:
      c.init<fixed_bitstring<15>>();
      break;
    case types::subbands16:
      c.init<fixed_bitstring<16>>();
      break;
    case types::subbands17:
      c.init<fixed_bitstring<17>>();
      break;
    case types::subbands18:
      c.init<fixed_bitstring<18>>();
      break;
    case types::subbands19_v1530:
      c.init<fixed_bitstring<19>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_");
  }
}
csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::csi_report_band_c_(
    const csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::subbands3:
      c.init(other.c.get<fixed_bitstring<3>>());
      break;
    case types::subbands4:
      c.init(other.c.get<fixed_bitstring<4>>());
      break;
    case types::subbands5:
      c.init(other.c.get<fixed_bitstring<5>>());
      break;
    case types::subbands6:
      c.init(other.c.get<fixed_bitstring<6>>());
      break;
    case types::subbands7:
      c.init(other.c.get<fixed_bitstring<7>>());
      break;
    case types::subbands8:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::subbands9:
      c.init(other.c.get<fixed_bitstring<9>>());
      break;
    case types::subbands10:
      c.init(other.c.get<fixed_bitstring<10>>());
      break;
    case types::subbands11:
      c.init(other.c.get<fixed_bitstring<11>>());
      break;
    case types::subbands12:
      c.init(other.c.get<fixed_bitstring<12>>());
      break;
    case types::subbands13:
      c.init(other.c.get<fixed_bitstring<13>>());
      break;
    case types::subbands14:
      c.init(other.c.get<fixed_bitstring<14>>());
      break;
    case types::subbands15:
      c.init(other.c.get<fixed_bitstring<15>>());
      break;
    case types::subbands16:
      c.init(other.c.get<fixed_bitstring<16>>());
      break;
    case types::subbands17:
      c.init(other.c.get<fixed_bitstring<17>>());
      break;
    case types::subbands18:
      c.init(other.c.get<fixed_bitstring<18>>());
      break;
    case types::subbands19_v1530:
      c.init(other.c.get<fixed_bitstring<19>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_");
  }
}
csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_&
csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::operator=(
    const csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::subbands3:
      c.set(other.c.get<fixed_bitstring<3>>());
      break;
    case types::subbands4:
      c.set(other.c.get<fixed_bitstring<4>>());
      break;
    case types::subbands5:
      c.set(other.c.get<fixed_bitstring<5>>());
      break;
    case types::subbands6:
      c.set(other.c.get<fixed_bitstring<6>>());
      break;
    case types::subbands7:
      c.set(other.c.get<fixed_bitstring<7>>());
      break;
    case types::subbands8:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::subbands9:
      c.set(other.c.get<fixed_bitstring<9>>());
      break;
    case types::subbands10:
      c.set(other.c.get<fixed_bitstring<10>>());
      break;
    case types::subbands11:
      c.set(other.c.get<fixed_bitstring<11>>());
      break;
    case types::subbands12:
      c.set(other.c.get<fixed_bitstring<12>>());
      break;
    case types::subbands13:
      c.set(other.c.get<fixed_bitstring<13>>());
      break;
    case types::subbands14:
      c.set(other.c.get<fixed_bitstring<14>>());
      break;
    case types::subbands15:
      c.set(other.c.get<fixed_bitstring<15>>());
      break;
    case types::subbands16:
      c.set(other.c.get<fixed_bitstring<16>>());
      break;
    case types::subbands17:
      c.set(other.c.get<fixed_bitstring<17>>());
      break;
    case types::subbands18:
      c.set(other.c.get<fixed_bitstring<18>>());
      break;
    case types::subbands19_v1530:
      c.set(other.c.get<fixed_bitstring<19>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_");
  }

  return *this;
}
fixed_bitstring<3>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands3()
{
  set(types::subbands3);
  return c.get<fixed_bitstring<3>>();
}
fixed_bitstring<4>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands4()
{
  set(types::subbands4);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<5>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands5()
{
  set(types::subbands5);
  return c.get<fixed_bitstring<5>>();
}
fixed_bitstring<6>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands6()
{
  set(types::subbands6);
  return c.get<fixed_bitstring<6>>();
}
fixed_bitstring<7>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands7()
{
  set(types::subbands7);
  return c.get<fixed_bitstring<7>>();
}
fixed_bitstring<8>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands8()
{
  set(types::subbands8);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<9>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands9()
{
  set(types::subbands9);
  return c.get<fixed_bitstring<9>>();
}
fixed_bitstring<10>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands10()
{
  set(types::subbands10);
  return c.get<fixed_bitstring<10>>();
}
fixed_bitstring<11>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands11()
{
  set(types::subbands11);
  return c.get<fixed_bitstring<11>>();
}
fixed_bitstring<12>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands12()
{
  set(types::subbands12);
  return c.get<fixed_bitstring<12>>();
}
fixed_bitstring<13>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands13()
{
  set(types::subbands13);
  return c.get<fixed_bitstring<13>>();
}
fixed_bitstring<14>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands14()
{
  set(types::subbands14);
  return c.get<fixed_bitstring<14>>();
}
fixed_bitstring<15>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands15()
{
  set(types::subbands15);
  return c.get<fixed_bitstring<15>>();
}
fixed_bitstring<16>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands16()
{
  set(types::subbands16);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<17>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands17()
{
  set(types::subbands17);
  return c.get<fixed_bitstring<17>>();
}
fixed_bitstring<18>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands18()
{
  set(types::subbands18);
  return c.get<fixed_bitstring<18>>();
}
fixed_bitstring<19>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands19_v1530()
{
  set(types::subbands19_v1530);
  return c.get<fixed_bitstring<19>>();
}
void csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::subbands3:
      j.write_str("subbands3", c.get<fixed_bitstring<3>>().to_string());
      break;
    case types::subbands4:
      j.write_str("subbands4", c.get<fixed_bitstring<4>>().to_string());
      break;
    case types::subbands5:
      j.write_str("subbands5", c.get<fixed_bitstring<5>>().to_string());
      break;
    case types::subbands6:
      j.write_str("subbands6", c.get<fixed_bitstring<6>>().to_string());
      break;
    case types::subbands7:
      j.write_str("subbands7", c.get<fixed_bitstring<7>>().to_string());
      break;
    case types::subbands8:
      j.write_str("subbands8", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::subbands9:
      j.write_str("subbands9", c.get<fixed_bitstring<9>>().to_string());
      break;
    case types::subbands10:
      j.write_str("subbands10", c.get<fixed_bitstring<10>>().to_string());
      break;
    case types::subbands11:
      j.write_str("subbands11", c.get<fixed_bitstring<11>>().to_string());
      break;
    case types::subbands12:
      j.write_str("subbands12", c.get<fixed_bitstring<12>>().to_string());
      break;
    case types::subbands13:
      j.write_str("subbands13", c.get<fixed_bitstring<13>>().to_string());
      break;
    case types::subbands14:
      j.write_str("subbands14", c.get<fixed_bitstring<14>>().to_string());
      break;
    case types::subbands15:
      j.write_str("subbands15", c.get<fixed_bitstring<15>>().to_string());
      break;
    case types::subbands16:
      j.write_str("subbands16", c.get<fixed_bitstring<16>>().to_string());
      break;
    case types::subbands17:
      j.write_str("subbands17", c.get<fixed_bitstring<17>>().to_string());
      break;
    case types::subbands18:
      j.write_str("subbands18", c.get<fixed_bitstring<18>>().to_string());
      break;
    case types::subbands19_v1530:
      j.write_str("subbands19-v1530", c.get<fixed_bitstring<19>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::subbands3:
      HANDLE_CODE(c.get<fixed_bitstring<3>>().pack(bref));
      break;
    case types::subbands4:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().pack(bref));
      break;
    case types::subbands5:
      HANDLE_CODE(c.get<fixed_bitstring<5>>().pack(bref));
      break;
    case types::subbands6:
      HANDLE_CODE(c.get<fixed_bitstring<6>>().pack(bref));
      break;
    case types::subbands7:
      HANDLE_CODE(c.get<fixed_bitstring<7>>().pack(bref));
      break;
    case types::subbands8:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::subbands9:
      HANDLE_CODE(c.get<fixed_bitstring<9>>().pack(bref));
      break;
    case types::subbands10:
      HANDLE_CODE(c.get<fixed_bitstring<10>>().pack(bref));
      break;
    case types::subbands11:
      HANDLE_CODE(c.get<fixed_bitstring<11>>().pack(bref));
      break;
    case types::subbands12:
      HANDLE_CODE(c.get<fixed_bitstring<12>>().pack(bref));
      break;
    case types::subbands13:
      HANDLE_CODE(c.get<fixed_bitstring<13>>().pack(bref));
      break;
    case types::subbands14:
      HANDLE_CODE(c.get<fixed_bitstring<14>>().pack(bref));
      break;
    case types::subbands15:
      HANDLE_CODE(c.get<fixed_bitstring<15>>().pack(bref));
      break;
    case types::subbands16:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().pack(bref));
      break;
    case types::subbands17:
      HANDLE_CODE(c.get<fixed_bitstring<17>>().pack(bref));
      break;
    case types::subbands18:
      HANDLE_CODE(c.get<fixed_bitstring<18>>().pack(bref));
      break;
    case types::subbands19_v1530: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<fixed_bitstring<19>>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::subbands3:
      HANDLE_CODE(c.get<fixed_bitstring<3>>().unpack(bref));
      break;
    case types::subbands4:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().unpack(bref));
      break;
    case types::subbands5:
      HANDLE_CODE(c.get<fixed_bitstring<5>>().unpack(bref));
      break;
    case types::subbands6:
      HANDLE_CODE(c.get<fixed_bitstring<6>>().unpack(bref));
      break;
    case types::subbands7:
      HANDLE_CODE(c.get<fixed_bitstring<7>>().unpack(bref));
      break;
    case types::subbands8:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::subbands9:
      HANDLE_CODE(c.get<fixed_bitstring<9>>().unpack(bref));
      break;
    case types::subbands10:
      HANDLE_CODE(c.get<fixed_bitstring<10>>().unpack(bref));
      break;
    case types::subbands11:
      HANDLE_CODE(c.get<fixed_bitstring<11>>().unpack(bref));
      break;
    case types::subbands12:
      HANDLE_CODE(c.get<fixed_bitstring<12>>().unpack(bref));
      break;
    case types::subbands13:
      HANDLE_CODE(c.get<fixed_bitstring<13>>().unpack(bref));
      break;
    case types::subbands14:
      HANDLE_CODE(c.get<fixed_bitstring<14>>().unpack(bref));
      break;
    case types::subbands15:
      HANDLE_CODE(c.get<fixed_bitstring<15>>().unpack(bref));
      break;
    case types::subbands16:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().unpack(bref));
      break;
    case types::subbands17:
      HANDLE_CODE(c.get<fixed_bitstring<17>>().unpack(bref));
      break;
    case types::subbands18:
      HANDLE_CODE(c.get<fixed_bitstring<18>>().unpack(bref));
      break;
    case types::subbands19_v1530: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<fixed_bitstring<19>>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_report_cfg_s::time_restrict_for_ch_meass_opts::to_string() const
{
  static const char* names[] = {"configured", "notConfigured"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::time_restrict_for_ch_meass_e_");
}

const char* csi_report_cfg_s::time_restrict_for_interference_meass_opts::to_string() const
{
  static const char* names[] = {"configured", "notConfigured"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::time_restrict_for_interference_meass_e_");
}

const char* csi_report_cfg_s::dummy_opts::to_string() const
{
  static const char* names[] = {"n1", "n2"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::dummy_e_");
}
uint8_t csi_report_cfg_s::dummy_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_report_cfg_s::dummy_e_");
}

void csi_report_cfg_s::group_based_beam_report_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::group_based_beam_report_c_::set_enabled()
{
  set(types::enabled);
}
csi_report_cfg_s::group_based_beam_report_c_::disabled_s_& csi_report_cfg_s::group_based_beam_report_c_::set_disabled()
{
  set(types::disabled);
  return c;
}
void csi_report_cfg_s::group_based_beam_report_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::enabled:
      break;
    case types::disabled:
      j.write_fieldname("disabled");
      j.start_obj();
      if (c.nrof_reported_rs_present) {
        j.write_str("nrofReportedRS", c.nrof_reported_rs.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::group_based_beam_report_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_report_cfg_s::group_based_beam_report_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::enabled:
      break;
    case types::disabled:
      HANDLE_CODE(bref.pack(c.nrof_reported_rs_present, 1));
      if (c.nrof_reported_rs_present) {
        HANDLE_CODE(c.nrof_reported_rs.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::group_based_beam_report_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_cfg_s::group_based_beam_report_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::enabled:
      break;
    case types::disabled:
      HANDLE_CODE(bref.unpack(c.nrof_reported_rs_present, 1));
      if (c.nrof_reported_rs_present) {
        HANDLE_CODE(c.nrof_reported_rs.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "csi_report_cfg_s::group_based_beam_report_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(
      names, 4, value, "csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_e_");
}
uint8_t csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(
      numbers, 4, value, "csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_e_");
}

const char* csi_report_cfg_s::cqi_table_opts::to_string() const
{
  static const char* names[] = {"table1", "table2", "table3", "spare1"};
  return convert_enum_idx(names, 4, value, "csi_report_cfg_s::cqi_table_e_");
}
uint8_t csi_report_cfg_s::cqi_table_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "csi_report_cfg_s::cqi_table_e_");
}

const char* csi_report_cfg_s::subband_size_opts::to_string() const
{
  static const char* names[] = {"value1", "value2"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::subband_size_e_");
}
uint8_t csi_report_cfg_s::subband_size_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_report_cfg_s::subband_size_e_");
}

const char* csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_opts::to_string() const
{
  static const char* names[] = {"sl4", "sl8", "sl16"};
  return convert_enum_idx(
      names, 3, value, "csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_e_");
}
uint8_t csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16};
  return map_enum_number(
      numbers, 3, value, "csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_e_");
}

// CSI-ResourceConfig ::= SEQUENCE
SRSASN_CODE csi_res_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_res_cfg_id, (uint8_t)0u, (uint8_t)111u));
  HANDLE_CODE(csi_rs_res_set_list.pack(bref));
  HANDLE_CODE(pack_integer(bref, bwp_id, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(res_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_res_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_res_cfg_id, bref, (uint8_t)0u, (uint8_t)111u));
  HANDLE_CODE(csi_rs_res_set_list.unpack(bref));
  HANDLE_CODE(unpack_integer(bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(res_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void csi_res_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-ResourceConfigId", csi_res_cfg_id);
  j.write_fieldname("csi-RS-ResourceSetList");
  csi_rs_res_set_list.to_json(j);
  j.write_int("bwp-Id", bwp_id);
  j.write_str("resourceType", res_type.to_string());
  j.end_obj();
}

void csi_res_cfg_s::csi_rs_res_set_list_c_::destroy_()
{
  switch (type_) {
    case types::nzp_csi_rs_ssb:
      c.destroy<nzp_csi_rs_ssb_s_>();
      break;
    case types::csi_im_res_set_list:
      c.destroy<csi_im_res_set_list_l_>();
      break;
    default:
      break;
  }
}
void csi_res_cfg_s::csi_rs_res_set_list_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nzp_csi_rs_ssb:
      c.init<nzp_csi_rs_ssb_s_>();
      break;
    case types::csi_im_res_set_list:
      c.init<csi_im_res_set_list_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_cfg_s::csi_rs_res_set_list_c_");
  }
}
csi_res_cfg_s::csi_rs_res_set_list_c_::csi_rs_res_set_list_c_(const csi_res_cfg_s::csi_rs_res_set_list_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nzp_csi_rs_ssb:
      c.init(other.c.get<nzp_csi_rs_ssb_s_>());
      break;
    case types::csi_im_res_set_list:
      c.init(other.c.get<csi_im_res_set_list_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_cfg_s::csi_rs_res_set_list_c_");
  }
}
csi_res_cfg_s::csi_rs_res_set_list_c_&
csi_res_cfg_s::csi_rs_res_set_list_c_::operator=(const csi_res_cfg_s::csi_rs_res_set_list_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nzp_csi_rs_ssb:
      c.set(other.c.get<nzp_csi_rs_ssb_s_>());
      break;
    case types::csi_im_res_set_list:
      c.set(other.c.get<csi_im_res_set_list_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_cfg_s::csi_rs_res_set_list_c_");
  }

  return *this;
}
csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_& csi_res_cfg_s::csi_rs_res_set_list_c_::set_nzp_csi_rs_ssb()
{
  set(types::nzp_csi_rs_ssb);
  return c.get<nzp_csi_rs_ssb_s_>();
}
csi_res_cfg_s::csi_rs_res_set_list_c_::csi_im_res_set_list_l_&
csi_res_cfg_s::csi_rs_res_set_list_c_::set_csi_im_res_set_list()
{
  set(types::csi_im_res_set_list);
  return c.get<csi_im_res_set_list_l_>();
}
void csi_res_cfg_s::csi_rs_res_set_list_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nzp_csi_rs_ssb:
      j.write_fieldname("nzp-CSI-RS-SSB");
      j.start_obj();
      if (c.get<nzp_csi_rs_ssb_s_>().nzp_csi_rs_res_set_list.size() > 0) {
        j.start_array("nzp-CSI-RS-ResourceSetList");
        for (const auto& e1 : c.get<nzp_csi_rs_ssb_s_>().nzp_csi_rs_res_set_list) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list_present) {
        j.start_array("csi-SSB-ResourceSetList");
        for (const auto& e1 : c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    case types::csi_im_res_set_list:
      j.start_array("csi-IM-ResourceSetList");
      for (const auto& e1 : c.get<csi_im_res_set_list_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_cfg_s::csi_rs_res_set_list_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_res_cfg_s::csi_rs_res_set_list_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nzp_csi_rs_ssb:
      HANDLE_CODE(bref.pack(c.get<nzp_csi_rs_ssb_s_>().nzp_csi_rs_res_set_list.size() > 0, 1));
      HANDLE_CODE(bref.pack(c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list_present, 1));
      if (c.get<nzp_csi_rs_ssb_s_>().nzp_csi_rs_res_set_list.size() > 0) {
        HANDLE_CODE(pack_dyn_seq_of(
            bref, c.get<nzp_csi_rs_ssb_s_>().nzp_csi_rs_res_set_list, 1, 16, integer_packer<uint8_t>(0, 63)));
      }
      if (c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list_present) {
        HANDLE_CODE(pack_fixed_seq_of(bref,
                                      &(c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list)[0],
                                      c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list.size(),
                                      integer_packer<uint8_t>(0, 63)));
      }
      break;
    case types::csi_im_res_set_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<csi_im_res_set_list_l_>(), 1, 16, integer_packer<uint8_t>(0, 63)));
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_cfg_s::csi_rs_res_set_list_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_res_cfg_s::csi_rs_res_set_list_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nzp_csi_rs_ssb:
      bool nzp_csi_rs_res_set_list_present;
      HANDLE_CODE(bref.unpack(nzp_csi_rs_res_set_list_present, 1));
      HANDLE_CODE(bref.unpack(c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list_present, 1));
      if (nzp_csi_rs_res_set_list_present) {
        HANDLE_CODE(unpack_dyn_seq_of(
            c.get<nzp_csi_rs_ssb_s_>().nzp_csi_rs_res_set_list, bref, 1, 16, integer_packer<uint8_t>(0, 63)));
      }
      if (c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list_present) {
        HANDLE_CODE(unpack_fixed_seq_of(&(c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list)[0],
                                        bref,
                                        c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list.size(),
                                        integer_packer<uint8_t>(0, 63)));
      }
      break;
    case types::csi_im_res_set_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<csi_im_res_set_list_l_>(), bref, 1, 16, integer_packer<uint8_t>(0, 63)));
      break;
    default:
      log_invalid_choice_id(type_, "csi_res_cfg_s::csi_rs_res_set_list_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_res_cfg_s::res_type_opts::to_string() const
{
  static const char* names[] = {"aperiodic", "semiPersistent", "periodic"};
  return convert_enum_idx(names, 3, value, "csi_res_cfg_s::res_type_e_");
}

// CSI-SSB-ResourceSet ::= SEQUENCE
SRSASN_CODE csi_ssb_res_set_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_ssb_res_set_id, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_dyn_seq_of(bref, csi_ssb_res_list, 1, 64, integer_packer<uint8_t>(0, 63)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_ssb_res_set_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_ssb_res_set_id, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_dyn_seq_of(csi_ssb_res_list, bref, 1, 64, integer_packer<uint8_t>(0, 63)));

  return SRSASN_SUCCESS;
}
void csi_ssb_res_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("csi-SSB-ResourceSetId", csi_ssb_res_set_id);
  j.start_array("csi-SSB-ResourceList");
  for (const auto& e1 : csi_ssb_res_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// NZP-CSI-RS-Resource ::= SEQUENCE
SRSASN_CODE nzp_csi_rs_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pwr_ctrl_offset_ss_present, 1));
  HANDLE_CODE(bref.pack(periodicity_and_offset_present, 1));
  HANDLE_CODE(bref.pack(qcl_info_periodic_csi_rs_present, 1));

  HANDLE_CODE(pack_integer(bref, nzp_csi_rs_res_id, (uint8_t)0u, (uint8_t)191u));
  HANDLE_CODE(res_map.pack(bref));
  HANDLE_CODE(pack_integer(bref, pwr_ctrl_offset, (int8_t)-8, (int8_t)15));
  if (pwr_ctrl_offset_ss_present) {
    HANDLE_CODE(pwr_ctrl_offset_ss.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, scrambling_id, (uint16_t)0u, (uint16_t)1023u));
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.pack(bref));
  }
  if (qcl_info_periodic_csi_rs_present) {
    HANDLE_CODE(pack_integer(bref, qcl_info_periodic_csi_rs, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nzp_csi_rs_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pwr_ctrl_offset_ss_present, 1));
  HANDLE_CODE(bref.unpack(periodicity_and_offset_present, 1));
  HANDLE_CODE(bref.unpack(qcl_info_periodic_csi_rs_present, 1));

  HANDLE_CODE(unpack_integer(nzp_csi_rs_res_id, bref, (uint8_t)0u, (uint8_t)191u));
  HANDLE_CODE(res_map.unpack(bref));
  HANDLE_CODE(unpack_integer(pwr_ctrl_offset, bref, (int8_t)-8, (int8_t)15));
  if (pwr_ctrl_offset_ss_present) {
    HANDLE_CODE(pwr_ctrl_offset_ss.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(scrambling_id, bref, (uint16_t)0u, (uint16_t)1023u));
  if (periodicity_and_offset_present) {
    HANDLE_CODE(periodicity_and_offset.unpack(bref));
  }
  if (qcl_info_periodic_csi_rs_present) {
    HANDLE_CODE(unpack_integer(qcl_info_periodic_csi_rs, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void nzp_csi_rs_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nzp-CSI-RS-ResourceId", nzp_csi_rs_res_id);
  j.write_fieldname("resourceMapping");
  res_map.to_json(j);
  j.write_int("powerControlOffset", pwr_ctrl_offset);
  if (pwr_ctrl_offset_ss_present) {
    j.write_str("powerControlOffsetSS", pwr_ctrl_offset_ss.to_string());
  }
  j.write_int("scramblingID", scrambling_id);
  if (periodicity_and_offset_present) {
    j.write_fieldname("periodicityAndOffset");
    periodicity_and_offset.to_json(j);
  }
  if (qcl_info_periodic_csi_rs_present) {
    j.write_int("qcl-InfoPeriodicCSI-RS", qcl_info_periodic_csi_rs);
  }
  j.end_obj();
}

const char* nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::to_string() const
{
  static const char* names[] = {"db-3", "db0", "db3", "db6"};
  return convert_enum_idx(names, 4, value, "nzp_csi_rs_res_s::pwr_ctrl_offset_ss_e_");
}
int8_t nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::to_number() const
{
  static const int8_t numbers[] = {-3, 0, 3, 6};
  return map_enum_number(numbers, 4, value, "nzp_csi_rs_res_s::pwr_ctrl_offset_ss_e_");
}

// NZP-CSI-RS-ResourceSet ::= SEQUENCE
SRSASN_CODE nzp_csi_rs_res_set_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(repeat_present, 1));
  HANDLE_CODE(bref.pack(aperiodic_trigger_offset_present, 1));
  HANDLE_CODE(bref.pack(trs_info_present, 1));

  HANDLE_CODE(pack_integer(bref, nzp_csi_res_set_id, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_dyn_seq_of(bref, nzp_csi_rs_res, 1, 64, integer_packer<uint8_t>(0, 191)));
  if (repeat_present) {
    HANDLE_CODE(repeat.pack(bref));
  }
  if (aperiodic_trigger_offset_present) {
    HANDLE_CODE(pack_integer(bref, aperiodic_trigger_offset, (uint8_t)0u, (uint8_t)6u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nzp_csi_rs_res_set_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(repeat_present, 1));
  HANDLE_CODE(bref.unpack(aperiodic_trigger_offset_present, 1));
  HANDLE_CODE(bref.unpack(trs_info_present, 1));

  HANDLE_CODE(unpack_integer(nzp_csi_res_set_id, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_dyn_seq_of(nzp_csi_rs_res, bref, 1, 64, integer_packer<uint8_t>(0, 191)));
  if (repeat_present) {
    HANDLE_CODE(repeat.unpack(bref));
  }
  if (aperiodic_trigger_offset_present) {
    HANDLE_CODE(unpack_integer(aperiodic_trigger_offset, bref, (uint8_t)0u, (uint8_t)6u));
  }

  return SRSASN_SUCCESS;
}
void nzp_csi_rs_res_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nzp-CSI-ResourceSetId", nzp_csi_res_set_id);
  j.start_array("nzp-CSI-RS-Resources");
  for (const auto& e1 : nzp_csi_rs_res) {
    j.write_int(e1);
  }
  j.end_array();
  if (repeat_present) {
    j.write_str("repetition", repeat.to_string());
  }
  if (aperiodic_trigger_offset_present) {
    j.write_int("aperiodicTriggeringOffset", aperiodic_trigger_offset);
  }
  if (trs_info_present) {
    j.write_str("trs-Info", "true");
  }
  j.end_obj();
}

const char* nzp_csi_rs_res_set_s::repeat_opts::to_string() const
{
  static const char* names[] = {"on", "off"};
  return convert_enum_idx(names, 2, value, "nzp_csi_rs_res_set_s::repeat_e_");
}

// CSI-MeasConfig ::= SEQUENCE
SRSASN_CODE csi_meas_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(nzp_csi_rs_res_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(nzp_csi_rs_res_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(nzp_csi_rs_res_set_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(nzp_csi_rs_res_set_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_im_res_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_im_res_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_im_res_set_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_im_res_set_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_ssb_res_set_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_ssb_res_set_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_res_cfg_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_res_cfg_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_report_cfg_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(csi_report_cfg_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(report_trigger_size_present, 1));
  HANDLE_CODE(bref.pack(aperiodic_trigger_state_list_present, 1));
  HANDLE_CODE(bref.pack(semi_persistent_on_pusch_trigger_state_list_present, 1));

  if (nzp_csi_rs_res_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nzp_csi_rs_res_to_add_mod_list, 1, 192));
  }
  if (nzp_csi_rs_res_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nzp_csi_rs_res_to_release_list, 1, 192, integer_packer<uint8_t>(0, 191)));
  }
  if (nzp_csi_rs_res_set_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nzp_csi_rs_res_set_to_add_mod_list, 1, 64));
  }
  if (nzp_csi_rs_res_set_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nzp_csi_rs_res_set_to_release_list, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (csi_im_res_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_res_to_add_mod_list, 1, 32));
  }
  if (csi_im_res_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_res_to_release_list, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (csi_im_res_set_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_res_set_to_add_mod_list, 1, 64));
  }
  if (csi_im_res_set_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_im_res_set_to_release_list, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (csi_ssb_res_set_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_ssb_res_set_to_add_mod_list, 1, 64));
  }
  if (csi_ssb_res_set_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_ssb_res_set_to_release_list, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (csi_res_cfg_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_res_cfg_to_add_mod_list, 1, 112));
  }
  if (csi_res_cfg_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_res_cfg_to_release_list, 1, 112, integer_packer<uint8_t>(0, 111)));
  }
  if (csi_report_cfg_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_report_cfg_to_add_mod_list, 1, 48));
  }
  if (csi_report_cfg_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_report_cfg_to_release_list, 1, 48, integer_packer<uint8_t>(0, 47)));
  }
  if (report_trigger_size_present) {
    HANDLE_CODE(pack_integer(bref, report_trigger_size, (uint8_t)0u, (uint8_t)6u));
  }
  if (aperiodic_trigger_state_list_present) {
    HANDLE_CODE(aperiodic_trigger_state_list.pack(bref));
  }
  if (semi_persistent_on_pusch_trigger_state_list_present) {
    HANDLE_CODE(semi_persistent_on_pusch_trigger_state_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_meas_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool nzp_csi_rs_res_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(nzp_csi_rs_res_to_add_mod_list_present, 1));
  bool nzp_csi_rs_res_to_release_list_present;
  HANDLE_CODE(bref.unpack(nzp_csi_rs_res_to_release_list_present, 1));
  bool nzp_csi_rs_res_set_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(nzp_csi_rs_res_set_to_add_mod_list_present, 1));
  bool nzp_csi_rs_res_set_to_release_list_present;
  HANDLE_CODE(bref.unpack(nzp_csi_rs_res_set_to_release_list_present, 1));
  bool csi_im_res_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(csi_im_res_to_add_mod_list_present, 1));
  bool csi_im_res_to_release_list_present;
  HANDLE_CODE(bref.unpack(csi_im_res_to_release_list_present, 1));
  bool csi_im_res_set_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(csi_im_res_set_to_add_mod_list_present, 1));
  bool csi_im_res_set_to_release_list_present;
  HANDLE_CODE(bref.unpack(csi_im_res_set_to_release_list_present, 1));
  bool csi_ssb_res_set_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(csi_ssb_res_set_to_add_mod_list_present, 1));
  bool csi_ssb_res_set_to_release_list_present;
  HANDLE_CODE(bref.unpack(csi_ssb_res_set_to_release_list_present, 1));
  bool csi_res_cfg_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(csi_res_cfg_to_add_mod_list_present, 1));
  bool csi_res_cfg_to_release_list_present;
  HANDLE_CODE(bref.unpack(csi_res_cfg_to_release_list_present, 1));
  bool csi_report_cfg_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(csi_report_cfg_to_add_mod_list_present, 1));
  bool csi_report_cfg_to_release_list_present;
  HANDLE_CODE(bref.unpack(csi_report_cfg_to_release_list_present, 1));
  HANDLE_CODE(bref.unpack(report_trigger_size_present, 1));
  HANDLE_CODE(bref.unpack(aperiodic_trigger_state_list_present, 1));
  HANDLE_CODE(bref.unpack(semi_persistent_on_pusch_trigger_state_list_present, 1));

  if (nzp_csi_rs_res_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nzp_csi_rs_res_to_add_mod_list, bref, 1, 192));
  }
  if (nzp_csi_rs_res_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nzp_csi_rs_res_to_release_list, bref, 1, 192, integer_packer<uint8_t>(0, 191)));
  }
  if (nzp_csi_rs_res_set_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nzp_csi_rs_res_set_to_add_mod_list, bref, 1, 64));
  }
  if (nzp_csi_rs_res_set_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nzp_csi_rs_res_set_to_release_list, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (csi_im_res_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_res_to_add_mod_list, bref, 1, 32));
  }
  if (csi_im_res_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_res_to_release_list, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
  }
  if (csi_im_res_set_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_res_set_to_add_mod_list, bref, 1, 64));
  }
  if (csi_im_res_set_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_im_res_set_to_release_list, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (csi_ssb_res_set_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_ssb_res_set_to_add_mod_list, bref, 1, 64));
  }
  if (csi_ssb_res_set_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_ssb_res_set_to_release_list, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (csi_res_cfg_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_res_cfg_to_add_mod_list, bref, 1, 112));
  }
  if (csi_res_cfg_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_res_cfg_to_release_list, bref, 1, 112, integer_packer<uint8_t>(0, 111)));
  }
  if (csi_report_cfg_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_report_cfg_to_add_mod_list, bref, 1, 48));
  }
  if (csi_report_cfg_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_report_cfg_to_release_list, bref, 1, 48, integer_packer<uint8_t>(0, 47)));
  }
  if (report_trigger_size_present) {
    HANDLE_CODE(unpack_integer(report_trigger_size, bref, (uint8_t)0u, (uint8_t)6u));
  }
  if (aperiodic_trigger_state_list_present) {
    HANDLE_CODE(aperiodic_trigger_state_list.unpack(bref));
  }
  if (semi_persistent_on_pusch_trigger_state_list_present) {
    HANDLE_CODE(semi_persistent_on_pusch_trigger_state_list.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void csi_meas_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nzp_csi_rs_res_to_add_mod_list.size() > 0) {
    j.start_array("nzp-CSI-RS-ResourceToAddModList");
    for (const auto& e1 : nzp_csi_rs_res_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nzp_csi_rs_res_to_release_list.size() > 0) {
    j.start_array("nzp-CSI-RS-ResourceToReleaseList");
    for (const auto& e1 : nzp_csi_rs_res_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (nzp_csi_rs_res_set_to_add_mod_list.size() > 0) {
    j.start_array("nzp-CSI-RS-ResourceSetToAddModList");
    for (const auto& e1 : nzp_csi_rs_res_set_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nzp_csi_rs_res_set_to_release_list.size() > 0) {
    j.start_array("nzp-CSI-RS-ResourceSetToReleaseList");
    for (const auto& e1 : nzp_csi_rs_res_set_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_im_res_to_add_mod_list.size() > 0) {
    j.start_array("csi-IM-ResourceToAddModList");
    for (const auto& e1 : csi_im_res_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csi_im_res_to_release_list.size() > 0) {
    j.start_array("csi-IM-ResourceToReleaseList");
    for (const auto& e1 : csi_im_res_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_im_res_set_to_add_mod_list.size() > 0) {
    j.start_array("csi-IM-ResourceSetToAddModList");
    for (const auto& e1 : csi_im_res_set_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csi_im_res_set_to_release_list.size() > 0) {
    j.start_array("csi-IM-ResourceSetToReleaseList");
    for (const auto& e1 : csi_im_res_set_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_ssb_res_set_to_add_mod_list.size() > 0) {
    j.start_array("csi-SSB-ResourceSetToAddModList");
    for (const auto& e1 : csi_ssb_res_set_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csi_ssb_res_set_to_release_list.size() > 0) {
    j.start_array("csi-SSB-ResourceSetToReleaseList");
    for (const auto& e1 : csi_ssb_res_set_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_res_cfg_to_add_mod_list.size() > 0) {
    j.start_array("csi-ResourceConfigToAddModList");
    for (const auto& e1 : csi_res_cfg_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csi_res_cfg_to_release_list.size() > 0) {
    j.start_array("csi-ResourceConfigToReleaseList");
    for (const auto& e1 : csi_res_cfg_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (csi_report_cfg_to_add_mod_list.size() > 0) {
    j.start_array("csi-ReportConfigToAddModList");
    for (const auto& e1 : csi_report_cfg_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csi_report_cfg_to_release_list.size() > 0) {
    j.start_array("csi-ReportConfigToReleaseList");
    for (const auto& e1 : csi_report_cfg_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (report_trigger_size_present) {
    j.write_int("reportTriggerSize", report_trigger_size);
  }
  if (aperiodic_trigger_state_list_present) {
    j.write_fieldname("aperiodicTriggerStateList");
    aperiodic_trigger_state_list.to_json(j);
  }
  if (semi_persistent_on_pusch_trigger_state_list_present) {
    j.write_fieldname("semiPersistentOnPUSCH-TriggerStateList");
    semi_persistent_on_pusch_trigger_state_list.to_json(j);
  }
  j.end_obj();
}

// EUTRA-MBSFN-SubframeConfig ::= SEQUENCE
SRSASN_CODE eutra_mbsfn_sf_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sf_alloc2_present, 1));

  HANDLE_CODE(radioframe_alloc_period.pack(bref));
  HANDLE_CODE(pack_integer(bref, radioframe_alloc_offset, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(sf_alloc1.pack(bref));
  if (sf_alloc2_present) {
    HANDLE_CODE(sf_alloc2.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_mbsfn_sf_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sf_alloc2_present, 1));

  HANDLE_CODE(radioframe_alloc_period.unpack(bref));
  HANDLE_CODE(unpack_integer(radioframe_alloc_offset, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(sf_alloc1.unpack(bref));
  if (sf_alloc2_present) {
    HANDLE_CODE(sf_alloc2.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void eutra_mbsfn_sf_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("radioframeAllocationPeriod", radioframe_alloc_period.to_string());
  j.write_int("radioframeAllocationOffset", radioframe_alloc_offset);
  j.write_fieldname("subframeAllocation1");
  sf_alloc1.to_json(j);
  if (sf_alloc2_present) {
    j.write_fieldname("subframeAllocation2");
    sf_alloc2.to_json(j);
  }
  j.end_obj();
}

const char* eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n16", "n32"};
  return convert_enum_idx(names, 6, value, "eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}
uint8_t eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}

void eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::destroy_()
{
  switch (type_) {
    case types::one_frame:
      c.destroy<fixed_bitstring<6>>();
      break;
    case types::four_frames:
      c.destroy<fixed_bitstring<24>>();
      break;
    default:
      break;
  }
}
void eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::one_frame:
      c.init<fixed_bitstring<6>>();
      break;
    case types::four_frames:
      c.init<fixed_bitstring<24>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_");
  }
}
eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::sf_alloc1_c_(const eutra_mbsfn_sf_cfg_s::sf_alloc1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_frame:
      c.init(other.c.get<fixed_bitstring<6>>());
      break;
    case types::four_frames:
      c.init(other.c.get<fixed_bitstring<24>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_");
  }
}
eutra_mbsfn_sf_cfg_s::sf_alloc1_c_&
eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::operator=(const eutra_mbsfn_sf_cfg_s::sf_alloc1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_frame:
      c.set(other.c.get<fixed_bitstring<6>>());
      break;
    case types::four_frames:
      c.set(other.c.get<fixed_bitstring<24>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_");
  }

  return *this;
}
fixed_bitstring<6>& eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::set_one_frame()
{
  set(types::one_frame);
  return c.get<fixed_bitstring<6>>();
}
fixed_bitstring<24>& eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::set_four_frames()
{
  set(types::four_frames);
  return c.get<fixed_bitstring<24>>();
}
void eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_frame:
      j.write_str("oneFrame", c.get<fixed_bitstring<6>>().to_string());
      break;
    case types::four_frames:
      j.write_str("fourFrames", c.get<fixed_bitstring<24>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_");
  }
  j.end_obj();
}
SRSASN_CODE eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_frame:
      HANDLE_CODE(c.get<fixed_bitstring<6>>().pack(bref));
      break;
    case types::four_frames:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_frame:
      HANDLE_CODE(c.get<fixed_bitstring<6>>().unpack(bref));
      break;
    case types::four_frames:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::destroy_()
{
  switch (type_) {
    case types::one_frame:
      c.destroy<fixed_bitstring<2>>();
      break;
    case types::four_frames:
      c.destroy<fixed_bitstring<8>>();
      break;
    default:
      break;
  }
}
void eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::one_frame:
      c.init<fixed_bitstring<2>>();
      break;
    case types::four_frames:
      c.init<fixed_bitstring<8>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_");
  }
}
eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::sf_alloc2_c_(const eutra_mbsfn_sf_cfg_s::sf_alloc2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_frame:
      c.init(other.c.get<fixed_bitstring<2>>());
      break;
    case types::four_frames:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_");
  }
}
eutra_mbsfn_sf_cfg_s::sf_alloc2_c_&
eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::operator=(const eutra_mbsfn_sf_cfg_s::sf_alloc2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_frame:
      c.set(other.c.get<fixed_bitstring<2>>());
      break;
    case types::four_frames:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_");
  }

  return *this;
}
fixed_bitstring<2>& eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::set_one_frame()
{
  set(types::one_frame);
  return c.get<fixed_bitstring<2>>();
}
fixed_bitstring<8>& eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::set_four_frames()
{
  set(types::four_frames);
  return c.get<fixed_bitstring<8>>();
}
void eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_frame:
      j.write_str("oneFrame", c.get<fixed_bitstring<2>>().to_string());
      break;
    case types::four_frames:
      j.write_str("fourFrames", c.get<fixed_bitstring<8>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_");
  }
  j.end_obj();
}
SRSASN_CODE eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_frame:
      HANDLE_CODE(c.get<fixed_bitstring<2>>().pack(bref));
      break;
    case types::four_frames:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_frame:
      HANDLE_CODE(c.get<fixed_bitstring<2>>().unpack(bref));
      break;
    case types::four_frames:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRS-CC-SetIndex ::= SEQUENCE
SRSASN_CODE srs_cc_set_idx_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cc_set_idx_present, 1));
  HANDLE_CODE(bref.pack(cc_idx_in_one_cc_set_present, 1));

  if (cc_set_idx_present) {
    HANDLE_CODE(pack_integer(bref, cc_set_idx, (uint8_t)0u, (uint8_t)3u));
  }
  if (cc_idx_in_one_cc_set_present) {
    HANDLE_CODE(pack_integer(bref, cc_idx_in_one_cc_set, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_cc_set_idx_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cc_set_idx_present, 1));
  HANDLE_CODE(bref.unpack(cc_idx_in_one_cc_set_present, 1));

  if (cc_set_idx_present) {
    HANDLE_CODE(unpack_integer(cc_set_idx, bref, (uint8_t)0u, (uint8_t)3u));
  }
  if (cc_idx_in_one_cc_set_present) {
    HANDLE_CODE(unpack_integer(cc_idx_in_one_cc_set, bref, (uint8_t)0u, (uint8_t)7u));
  }

  return SRSASN_SUCCESS;
}
void srs_cc_set_idx_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cc_set_idx_present) {
    j.write_int("cc-SetIndex", cc_set_idx);
  }
  if (cc_idx_in_one_cc_set_present) {
    j.write_int("cc-IndexInOneCC-Set", cc_idx_in_one_cc_set);
  }
  j.end_obj();
}

// SlotFormatCombination ::= SEQUENCE
SRSASN_CODE slot_format_combination_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, slot_format_combination_id, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(pack_dyn_seq_of(bref, slot_formats, 1, 256, integer_packer<uint16_t>(0, 255)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE slot_format_combination_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(slot_format_combination_id, bref, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(unpack_dyn_seq_of(slot_formats, bref, 1, 256, integer_packer<uint16_t>(0, 255)));

  return SRSASN_SUCCESS;
}
void slot_format_combination_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("slotFormatCombinationId", slot_format_combination_id);
  j.start_array("slotFormats");
  for (const auto& e1 : slot_formats) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// PUSCH-CodeBlockGroupTransmission ::= SEQUENCE
SRSASN_CODE pusch_code_block_group_tx_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(max_code_block_groups_per_transport_block.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_code_block_group_tx_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(max_code_block_groups_per_transport_block.unpack(bref));

  return SRSASN_SUCCESS;
}
void pusch_code_block_group_tx_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxCodeBlockGroupsPerTransportBlock", max_code_block_groups_per_transport_block.to_string());
  j.end_obj();
}

const char* pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n6", "n8"};
  return convert_enum_idx(names, 4, value, "pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_");
}
uint8_t pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8};
  return map_enum_number(
      numbers, 4, value, "pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_");
}

// SRS-TPC-PDCCH-Config ::= SEQUENCE
SRSASN_CODE srs_tpc_pdcch_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_cc_set_idxlist.size() > 0, 1));

  if (srs_cc_set_idxlist.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_cc_set_idxlist, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_tpc_pdcch_cfg_s::unpack(cbit_ref& bref)
{
  bool srs_cc_set_idxlist_present;
  HANDLE_CODE(bref.unpack(srs_cc_set_idxlist_present, 1));

  if (srs_cc_set_idxlist_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_cc_set_idxlist, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void srs_tpc_pdcch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_cc_set_idxlist.size() > 0) {
    j.start_array("srs-CC-SetIndexlist");
    for (const auto& e1 : srs_cc_set_idxlist) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SlotFormatCombinationsPerCell ::= SEQUENCE
SRSASN_CODE slot_format_combinations_per_cell_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(subcarrier_spacing2_present, 1));
  HANDLE_CODE(bref.pack(slot_format_combinations.size() > 0, 1));
  HANDLE_CODE(bref.pack(position_in_dci_present, 1));

  HANDLE_CODE(pack_integer(bref, serving_cell_id, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(subcarrier_spacing.pack(bref));
  if (subcarrier_spacing2_present) {
    HANDLE_CODE(subcarrier_spacing2.pack(bref));
  }
  if (slot_format_combinations.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slot_format_combinations, 1, 512));
  }
  if (position_in_dci_present) {
    HANDLE_CODE(pack_integer(bref, position_in_dci, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE slot_format_combinations_per_cell_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(subcarrier_spacing2_present, 1));
  bool slot_format_combinations_present;
  HANDLE_CODE(bref.unpack(slot_format_combinations_present, 1));
  HANDLE_CODE(bref.unpack(position_in_dci_present, 1));

  HANDLE_CODE(unpack_integer(serving_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(subcarrier_spacing.unpack(bref));
  if (subcarrier_spacing2_present) {
    HANDLE_CODE(subcarrier_spacing2.unpack(bref));
  }
  if (slot_format_combinations_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slot_format_combinations, bref, 1, 512));
  }
  if (position_in_dci_present) {
    HANDLE_CODE(unpack_integer(position_in_dci, bref, (uint8_t)0u, (uint8_t)127u));
  }

  return SRSASN_SUCCESS;
}
void slot_format_combinations_per_cell_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servingCellId", serving_cell_id);
  j.write_str("subcarrierSpacing", subcarrier_spacing.to_string());
  if (subcarrier_spacing2_present) {
    j.write_str("subcarrierSpacing2", subcarrier_spacing2.to_string());
  }
  if (slot_format_combinations.size() > 0) {
    j.start_array("slotFormatCombinations");
    for (const auto& e1 : slot_format_combinations) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (position_in_dci_present) {
    j.write_int("positionInDCI", position_in_dci);
  }
  j.end_obj();
}

// PDSCH-CodeBlockGroupTransmission ::= SEQUENCE
SRSASN_CODE pdsch_code_block_group_tx_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(max_code_block_groups_per_transport_block.pack(bref));
  HANDLE_CODE(bref.pack(code_block_group_flush_ind, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_code_block_group_tx_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(max_code_block_groups_per_transport_block.unpack(bref));
  HANDLE_CODE(bref.unpack(code_block_group_flush_ind, 1));

  return SRSASN_SUCCESS;
}
void pdsch_code_block_group_tx_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxCodeBlockGroupsPerTransportBlock", max_code_block_groups_per_transport_block.to_string());
  j.write_bool("codeBlockGroupFlushIndicator", code_block_group_flush_ind);
  j.end_obj();
}

const char* pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n6", "n8"};
  return convert_enum_idx(names, 4, value, "pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_");
}
uint8_t pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8};
  return map_enum_number(
      numbers, 4, value, "pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_");
}

// PUSCH-ServingCellConfig ::= SEQUENCE
SRSASN_CODE pusch_serving_cell_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(code_block_group_tx_present, 1));
  HANDLE_CODE(bref.pack(rate_matching_present, 1));
  HANDLE_CODE(bref.pack(xoverhead_present, 1));

  if (code_block_group_tx_present) {
    HANDLE_CODE(code_block_group_tx.pack(bref));
  }
  if (xoverhead_present) {
    HANDLE_CODE(xoverhead.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= max_mimo_layers_present;
    group_flags[0] |= processing_type2_enabled_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_mimo_layers_present, 1));
      HANDLE_CODE(bref.pack(processing_type2_enabled_present, 1));
      if (max_mimo_layers_present) {
        HANDLE_CODE(pack_integer(bref, max_mimo_layers, (uint8_t)1u, (uint8_t)4u));
      }
      if (processing_type2_enabled_present) {
        HANDLE_CODE(bref.pack(processing_type2_enabled, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_serving_cell_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(code_block_group_tx_present, 1));
  HANDLE_CODE(bref.unpack(rate_matching_present, 1));
  HANDLE_CODE(bref.unpack(xoverhead_present, 1));

  if (code_block_group_tx_present) {
    HANDLE_CODE(code_block_group_tx.unpack(bref));
  }
  if (xoverhead_present) {
    HANDLE_CODE(xoverhead.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_mimo_layers_present, 1));
      HANDLE_CODE(bref.unpack(processing_type2_enabled_present, 1));
      if (max_mimo_layers_present) {
        HANDLE_CODE(unpack_integer(max_mimo_layers, bref, (uint8_t)1u, (uint8_t)4u));
      }
      if (processing_type2_enabled_present) {
        HANDLE_CODE(bref.unpack(processing_type2_enabled, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pusch_serving_cell_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (code_block_group_tx_present) {
    j.write_fieldname("codeBlockGroupTransmission");
    code_block_group_tx.to_json(j);
  }
  if (rate_matching_present) {
    j.write_str("rateMatching", "limitedBufferRM");
  }
  if (xoverhead_present) {
    j.write_str("xOverhead", xoverhead.to_string());
  }
  if (ext) {
    if (max_mimo_layers_present) {
      j.write_int("maxMIMO-Layers", max_mimo_layers);
    }
    if (processing_type2_enabled_present) {
      j.write_bool("processingType2Enabled", processing_type2_enabled);
    }
  }
  j.end_obj();
}

const char* pusch_serving_cell_cfg_s::xoverhead_opts::to_string() const
{
  static const char* names[] = {"xoh6", "xoh12", "xoh18"};
  return convert_enum_idx(names, 3, value, "pusch_serving_cell_cfg_s::xoverhead_e_");
}
uint8_t pusch_serving_cell_cfg_s::xoverhead_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12, 18};
  return map_enum_number(numbers, 3, value, "pusch_serving_cell_cfg_s::xoverhead_e_");
}

// RateMatchPatternLTE-CRS ::= SEQUENCE
SRSASN_CODE rate_match_pattern_lte_crs_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_dl, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(carrier_bw_dl.pack(bref));
  if (mbsfn_sf_cfg_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbsfn_sf_cfg_list, 1, 8));
  }
  HANDLE_CODE(nrof_crs_ports.pack(bref));
  HANDLE_CODE(v_shift.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rate_match_pattern_lte_crs_s::unpack(cbit_ref& bref)
{
  bool mbsfn_sf_cfg_list_present;
  HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_dl, bref, (uint16_t)0u, (uint16_t)16383u));
  HANDLE_CODE(carrier_bw_dl.unpack(bref));
  if (mbsfn_sf_cfg_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbsfn_sf_cfg_list, bref, 1, 8));
  }
  HANDLE_CODE(nrof_crs_ports.unpack(bref));
  HANDLE_CODE(v_shift.unpack(bref));

  return SRSASN_SUCCESS;
}
void rate_match_pattern_lte_crs_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreqDL", carrier_freq_dl);
  j.write_str("carrierBandwidthDL", carrier_bw_dl.to_string());
  if (mbsfn_sf_cfg_list.size() > 0) {
    j.start_array("mbsfn-SubframeConfigList");
    for (const auto& e1 : mbsfn_sf_cfg_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("nrofCRS-Ports", nrof_crs_ports.to_string());
  j.write_str("v-Shift", v_shift.to_string());
  j.end_obj();
}

const char* rate_match_pattern_lte_crs_s::carrier_bw_dl_opts::to_string() const
{
  static const char* names[] = {"n6", "n15", "n25", "n50", "n75", "n100", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "rate_match_pattern_lte_crs_s::carrier_bw_dl_e_");
}
uint8_t rate_match_pattern_lte_crs_s::carrier_bw_dl_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "rate_match_pattern_lte_crs_s::carrier_bw_dl_e_");
}

const char* rate_match_pattern_lte_crs_s::nrof_crs_ports_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "rate_match_pattern_lte_crs_s::nrof_crs_ports_e_");
}
uint8_t rate_match_pattern_lte_crs_s::nrof_crs_ports_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "rate_match_pattern_lte_crs_s::nrof_crs_ports_e_");
}

const char* rate_match_pattern_lte_crs_s::v_shift_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3", "n4", "n5"};
  return convert_enum_idx(names, 6, value, "rate_match_pattern_lte_crs_s::v_shift_e_");
}
uint8_t rate_match_pattern_lte_crs_s::v_shift_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5};
  return map_enum_number(numbers, 6, value, "rate_match_pattern_lte_crs_s::v_shift_e_");
}

// SRS-CarrierSwitching ::= SEQUENCE
SRSASN_CODE srs_carrier_switching_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(srs_switch_from_serv_cell_idx_present, 1));
  HANDLE_CODE(bref.pack(srs_tpc_pdcch_group_present, 1));
  HANDLE_CODE(bref.pack(monitoring_cells.size() > 0, 1));

  if (srs_switch_from_serv_cell_idx_present) {
    HANDLE_CODE(pack_integer(bref, srs_switch_from_serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(srs_switch_from_carrier.pack(bref));
  if (srs_tpc_pdcch_group_present) {
    HANDLE_CODE(srs_tpc_pdcch_group.pack(bref));
  }
  if (monitoring_cells.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, monitoring_cells, 1, 32, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_carrier_switching_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(srs_switch_from_serv_cell_idx_present, 1));
  HANDLE_CODE(bref.unpack(srs_tpc_pdcch_group_present, 1));
  bool monitoring_cells_present;
  HANDLE_CODE(bref.unpack(monitoring_cells_present, 1));

  if (srs_switch_from_serv_cell_idx_present) {
    HANDLE_CODE(unpack_integer(srs_switch_from_serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(srs_switch_from_carrier.unpack(bref));
  if (srs_tpc_pdcch_group_present) {
    HANDLE_CODE(srs_tpc_pdcch_group.unpack(bref));
  }
  if (monitoring_cells_present) {
    HANDLE_CODE(unpack_dyn_seq_of(monitoring_cells, bref, 1, 32, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
void srs_carrier_switching_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_switch_from_serv_cell_idx_present) {
    j.write_int("srs-SwitchFromServCellIndex", srs_switch_from_serv_cell_idx);
  }
  j.write_str("srs-SwitchFromCarrier", srs_switch_from_carrier.to_string());
  if (srs_tpc_pdcch_group_present) {
    j.write_fieldname("srs-TPC-PDCCH-Group");
    srs_tpc_pdcch_group.to_json(j);
  }
  if (monitoring_cells.size() > 0) {
    j.start_array("monitoringCells");
    for (const auto& e1 : monitoring_cells) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* srs_carrier_switching_s::srs_switch_from_carrier_opts::to_string() const
{
  static const char* names[] = {"sUL", "nUL"};
  return convert_enum_idx(names, 2, value, "srs_carrier_switching_s::srs_switch_from_carrier_e_");
}

void srs_carrier_switching_s::srs_tpc_pdcch_group_c_::destroy_()
{
  switch (type_) {
    case types::type_a:
      c.destroy<type_a_l_>();
      break;
    case types::type_b:
      c.destroy<srs_tpc_pdcch_cfg_s>();
      break;
    default:
      break;
  }
}
void srs_carrier_switching_s::srs_tpc_pdcch_group_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type_a:
      c.init<type_a_l_>();
      break;
    case types::type_b:
      c.init<srs_tpc_pdcch_cfg_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_");
  }
}
srs_carrier_switching_s::srs_tpc_pdcch_group_c_::srs_tpc_pdcch_group_c_(
    const srs_carrier_switching_s::srs_tpc_pdcch_group_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type_a:
      c.init(other.c.get<type_a_l_>());
      break;
    case types::type_b:
      c.init(other.c.get<srs_tpc_pdcch_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_");
  }
}
srs_carrier_switching_s::srs_tpc_pdcch_group_c_&
srs_carrier_switching_s::srs_tpc_pdcch_group_c_::operator=(const srs_carrier_switching_s::srs_tpc_pdcch_group_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type_a:
      c.set(other.c.get<type_a_l_>());
      break;
    case types::type_b:
      c.set(other.c.get<srs_tpc_pdcch_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_");
  }

  return *this;
}
srs_carrier_switching_s::srs_tpc_pdcch_group_c_::type_a_l_&
srs_carrier_switching_s::srs_tpc_pdcch_group_c_::set_type_a()
{
  set(types::type_a);
  return c.get<type_a_l_>();
}
srs_tpc_pdcch_cfg_s& srs_carrier_switching_s::srs_tpc_pdcch_group_c_::set_type_b()
{
  set(types::type_b);
  return c.get<srs_tpc_pdcch_cfg_s>();
}
void srs_carrier_switching_s::srs_tpc_pdcch_group_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type_a:
      j.start_array("typeA");
      for (const auto& e1 : c.get<type_a_l_>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::type_b:
      j.write_fieldname("typeB");
      c.get<srs_tpc_pdcch_cfg_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_carrier_switching_s::srs_tpc_pdcch_group_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type_a:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<type_a_l_>(), 1, 32));
      break;
    case types::type_b:
      HANDLE_CODE(c.get<srs_tpc_pdcch_cfg_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_carrier_switching_s::srs_tpc_pdcch_group_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type_a:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<type_a_l_>(), bref, 1, 32));
      break;
    case types::type_b:
      HANDLE_CODE(c.get<srs_tpc_pdcch_cfg_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SlotFormatIndicator ::= SEQUENCE
SRSASN_CODE slot_format_ind_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slot_format_comb_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(slot_format_comb_to_release_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, sfi_rnti, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, dci_payload_size, (uint8_t)1u, (uint8_t)128u));
  if (slot_format_comb_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slot_format_comb_to_add_mod_list, 1, 16));
  }
  if (slot_format_comb_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slot_format_comb_to_release_list, 1, 16, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE slot_format_ind_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool slot_format_comb_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(slot_format_comb_to_add_mod_list_present, 1));
  bool slot_format_comb_to_release_list_present;
  HANDLE_CODE(bref.unpack(slot_format_comb_to_release_list_present, 1));

  HANDLE_CODE(unpack_integer(sfi_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(dci_payload_size, bref, (uint8_t)1u, (uint8_t)128u));
  if (slot_format_comb_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slot_format_comb_to_add_mod_list, bref, 1, 16));
  }
  if (slot_format_comb_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slot_format_comb_to_release_list, bref, 1, 16, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
void slot_format_ind_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sfi-RNTI", sfi_rnti);
  j.write_int("dci-PayloadSize", dci_payload_size);
  if (slot_format_comb_to_add_mod_list.size() > 0) {
    j.start_array("slotFormatCombToAddModList");
    for (const auto& e1 : slot_format_comb_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (slot_format_comb_to_release_list.size() > 0) {
    j.start_array("slotFormatCombToReleaseList");
    for (const auto& e1 : slot_format_comb_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// TDD-UL-DL-SlotConfig ::= SEQUENCE
SRSASN_CODE tdd_ul_dl_slot_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, slot_idx, (uint16_t)0u, (uint16_t)319u));
  HANDLE_CODE(symbols.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_slot_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(slot_idx, bref, (uint16_t)0u, (uint16_t)319u));
  HANDLE_CODE(symbols.unpack(bref));

  return SRSASN_SUCCESS;
}
void tdd_ul_dl_slot_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("slotIndex", slot_idx);
  j.write_fieldname("symbols");
  symbols.to_json(j);
  j.end_obj();
}

void tdd_ul_dl_slot_cfg_s::symbols_c_::set(types::options e)
{
  type_ = e;
}
void tdd_ul_dl_slot_cfg_s::symbols_c_::set_all_dl()
{
  set(types::all_dl);
}
void tdd_ul_dl_slot_cfg_s::symbols_c_::set_all_ul()
{
  set(types::all_ul);
}
tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_& tdd_ul_dl_slot_cfg_s::symbols_c_::set_explicit_type()
{
  set(types::explicit_type);
  return c;
}
void tdd_ul_dl_slot_cfg_s::symbols_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::all_dl:
      break;
    case types::all_ul:
      break;
    case types::explicit_type:
      j.write_fieldname("explicit");
      j.start_obj();
      if (c.nrof_dl_symbols_present) {
        j.write_int("nrofDownlinkSymbols", c.nrof_dl_symbols);
      }
      if (c.nrof_ul_symbols_present) {
        j.write_int("nrofUplinkSymbols", c.nrof_ul_symbols);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_s::symbols_c_");
  }
  j.end_obj();
}
SRSASN_CODE tdd_ul_dl_slot_cfg_s::symbols_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::all_dl:
      break;
    case types::all_ul:
      break;
    case types::explicit_type:
      HANDLE_CODE(bref.pack(c.nrof_dl_symbols_present, 1));
      HANDLE_CODE(bref.pack(c.nrof_ul_symbols_present, 1));
      if (c.nrof_dl_symbols_present) {
        HANDLE_CODE(pack_integer(bref, c.nrof_dl_symbols, (uint8_t)1u, (uint8_t)13u));
      }
      if (c.nrof_ul_symbols_present) {
        HANDLE_CODE(pack_integer(bref, c.nrof_ul_symbols, (uint8_t)1u, (uint8_t)13u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_s::symbols_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_slot_cfg_s::symbols_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::all_dl:
      break;
    case types::all_ul:
      break;
    case types::explicit_type:
      HANDLE_CODE(bref.unpack(c.nrof_dl_symbols_present, 1));
      HANDLE_CODE(bref.unpack(c.nrof_ul_symbols_present, 1));
      if (c.nrof_dl_symbols_present) {
        HANDLE_CODE(unpack_integer(c.nrof_dl_symbols, bref, (uint8_t)1u, (uint8_t)13u));
      }
      if (c.nrof_ul_symbols_present) {
        HANDLE_CODE(unpack_integer(c.nrof_ul_symbols, bref, (uint8_t)1u, (uint8_t)13u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_s::symbols_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CrossCarrierSchedulingConfig ::= SEQUENCE
SRSASN_CODE cross_carrier_sched_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sched_cell_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sched_cell_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void cross_carrier_sched_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("schedulingCellInfo");
  sched_cell_info.to_json(j);
  j.end_obj();
}

void cross_carrier_sched_cfg_s::sched_cell_info_c_::destroy_()
{
  switch (type_) {
    case types::own:
      c.destroy<own_s_>();
      break;
    case types::other:
      c.destroy<other_s_>();
      break;
    default:
      break;
  }
}
void cross_carrier_sched_cfg_s::sched_cell_info_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::own:
      c.init<own_s_>();
      break;
    case types::other:
      c.init<other_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_s::sched_cell_info_c_");
  }
}
cross_carrier_sched_cfg_s::sched_cell_info_c_::sched_cell_info_c_(
    const cross_carrier_sched_cfg_s::sched_cell_info_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::own:
      c.init(other.c.get<own_s_>());
      break;
    case types::other:
      c.init(other.c.get<other_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_s::sched_cell_info_c_");
  }
}
cross_carrier_sched_cfg_s::sched_cell_info_c_&
cross_carrier_sched_cfg_s::sched_cell_info_c_::operator=(const cross_carrier_sched_cfg_s::sched_cell_info_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::own:
      c.set(other.c.get<own_s_>());
      break;
    case types::other:
      c.set(other.c.get<other_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_s::sched_cell_info_c_");
  }

  return *this;
}
cross_carrier_sched_cfg_s::sched_cell_info_c_::own_s_& cross_carrier_sched_cfg_s::sched_cell_info_c_::set_own()
{
  set(types::own);
  return c.get<own_s_>();
}
cross_carrier_sched_cfg_s::sched_cell_info_c_::other_s_& cross_carrier_sched_cfg_s::sched_cell_info_c_::set_other()
{
  set(types::other);
  return c.get<other_s_>();
}
void cross_carrier_sched_cfg_s::sched_cell_info_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::own:
      j.write_fieldname("own");
      j.start_obj();
      j.write_bool("cif-Presence", c.get<own_s_>().cif_presence);
      j.end_obj();
      break;
    case types::other:
      j.write_fieldname("other");
      j.start_obj();
      j.write_int("schedulingCellId", c.get<other_s_>().sched_cell_id);
      j.write_int("cif-InSchedulingCell", c.get<other_s_>().cif_in_sched_cell);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_s::sched_cell_info_c_");
  }
  j.end_obj();
}
SRSASN_CODE cross_carrier_sched_cfg_s::sched_cell_info_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::own:
      HANDLE_CODE(bref.pack(c.get<own_s_>().cif_presence, 1));
      break;
    case types::other:
      HANDLE_CODE(pack_integer(bref, c.get<other_s_>().sched_cell_id, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(pack_integer(bref, c.get<other_s_>().cif_in_sched_cell, (uint8_t)1u, (uint8_t)7u));
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_s::sched_cell_info_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_s::sched_cell_info_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::own:
      HANDLE_CODE(bref.unpack(c.get<own_s_>().cif_presence, 1));
      break;
    case types::other:
      HANDLE_CODE(unpack_integer(c.get<other_s_>().sched_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
      HANDLE_CODE(unpack_integer(c.get<other_s_>().cif_in_sched_cell, bref, (uint8_t)1u, (uint8_t)7u));
      break;
    default:
      log_invalid_choice_id(type_, "cross_carrier_sched_cfg_s::sched_cell_info_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// PDCCH-ServingCellConfig ::= SEQUENCE
SRSASN_CODE pdcch_serving_cell_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slot_format_ind_present, 1));

  if (slot_format_ind_present) {
    HANDLE_CODE(slot_format_ind.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_serving_cell_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(slot_format_ind_present, 1));

  if (slot_format_ind_present) {
    HANDLE_CODE(slot_format_ind.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcch_serving_cell_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (slot_format_ind_present) {
    j.write_fieldname("slotFormatIndicator");
    slot_format_ind.to_json(j);
  }
  j.end_obj();
}

// PDSCH-ServingCellConfig ::= SEQUENCE
SRSASN_CODE pdsch_serving_cell_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(code_block_group_tx_present, 1));
  HANDLE_CODE(bref.pack(xoverhead_present, 1));
  HANDLE_CODE(bref.pack(nrof_harq_processes_for_pdsch_present, 1));
  HANDLE_CODE(bref.pack(pucch_cell_present, 1));

  if (code_block_group_tx_present) {
    HANDLE_CODE(code_block_group_tx.pack(bref));
  }
  if (xoverhead_present) {
    HANDLE_CODE(xoverhead.pack(bref));
  }
  if (nrof_harq_processes_for_pdsch_present) {
    HANDLE_CODE(nrof_harq_processes_for_pdsch.pack(bref));
  }
  if (pucch_cell_present) {
    HANDLE_CODE(pack_integer(bref, pucch_cell, (uint8_t)0u, (uint8_t)31u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= max_mimo_layers_present;
    group_flags[0] |= processing_type2_enabled_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_mimo_layers_present, 1));
      HANDLE_CODE(bref.pack(processing_type2_enabled_present, 1));
      if (max_mimo_layers_present) {
        HANDLE_CODE(pack_integer(bref, max_mimo_layers, (uint8_t)1u, (uint8_t)8u));
      }
      if (processing_type2_enabled_present) {
        HANDLE_CODE(bref.pack(processing_type2_enabled, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_serving_cell_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(code_block_group_tx_present, 1));
  HANDLE_CODE(bref.unpack(xoverhead_present, 1));
  HANDLE_CODE(bref.unpack(nrof_harq_processes_for_pdsch_present, 1));
  HANDLE_CODE(bref.unpack(pucch_cell_present, 1));

  if (code_block_group_tx_present) {
    HANDLE_CODE(code_block_group_tx.unpack(bref));
  }
  if (xoverhead_present) {
    HANDLE_CODE(xoverhead.unpack(bref));
  }
  if (nrof_harq_processes_for_pdsch_present) {
    HANDLE_CODE(nrof_harq_processes_for_pdsch.unpack(bref));
  }
  if (pucch_cell_present) {
    HANDLE_CODE(unpack_integer(pucch_cell, bref, (uint8_t)0u, (uint8_t)31u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_mimo_layers_present, 1));
      HANDLE_CODE(bref.unpack(processing_type2_enabled_present, 1));
      if (max_mimo_layers_present) {
        HANDLE_CODE(unpack_integer(max_mimo_layers, bref, (uint8_t)1u, (uint8_t)8u));
      }
      if (processing_type2_enabled_present) {
        HANDLE_CODE(bref.unpack(processing_type2_enabled, 1));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pdsch_serving_cell_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (code_block_group_tx_present) {
    j.write_fieldname("codeBlockGroupTransmission");
    code_block_group_tx.to_json(j);
  }
  if (xoverhead_present) {
    j.write_str("xOverhead", xoverhead.to_string());
  }
  if (nrof_harq_processes_for_pdsch_present) {
    j.write_str("nrofHARQ-ProcessesForPDSCH", nrof_harq_processes_for_pdsch.to_string());
  }
  if (pucch_cell_present) {
    j.write_int("pucch-Cell", pucch_cell);
  }
  if (ext) {
    if (max_mimo_layers_present) {
      j.write_int("maxMIMO-Layers", max_mimo_layers);
    }
    if (processing_type2_enabled_present) {
      j.write_bool("processingType2Enabled", processing_type2_enabled);
    }
  }
  j.end_obj();
}

const char* pdsch_serving_cell_cfg_s::xoverhead_opts::to_string() const
{
  static const char* names[] = {"xOh6", "xOh12", "xOh18"};
  return convert_enum_idx(names, 3, value, "pdsch_serving_cell_cfg_s::xoverhead_e_");
}
uint8_t pdsch_serving_cell_cfg_s::xoverhead_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12, 18};
  return map_enum_number(numbers, 3, value, "pdsch_serving_cell_cfg_s::xoverhead_e_");
}

const char* pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n6", "n10", "n12", "n16"};
  return convert_enum_idx(names, 6, value, "pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_e_");
}
uint8_t pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 10, 12, 16};
  return map_enum_number(numbers, 6, value, "pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_e_");
}

// TDD-UL-DL-ConfigDedicated ::= SEQUENCE
SRSASN_CODE tdd_ul_dl_cfg_ded_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slot_specific_cfgs_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(slot_specific_cfgs_to_release_list.size() > 0, 1));

  if (slot_specific_cfgs_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slot_specific_cfgs_to_add_mod_list, 1, 320));
  }
  if (slot_specific_cfgs_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slot_specific_cfgs_to_release_list, 1, 320, integer_packer<uint16_t>(0, 319)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_cfg_ded_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool slot_specific_cfgs_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(slot_specific_cfgs_to_add_mod_list_present, 1));
  bool slot_specific_cfgs_to_release_list_present;
  HANDLE_CODE(bref.unpack(slot_specific_cfgs_to_release_list_present, 1));

  if (slot_specific_cfgs_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slot_specific_cfgs_to_add_mod_list, bref, 1, 320));
  }
  if (slot_specific_cfgs_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slot_specific_cfgs_to_release_list, bref, 1, 320, integer_packer<uint16_t>(0, 319)));
  }

  return SRSASN_SUCCESS;
}
void tdd_ul_dl_cfg_ded_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (slot_specific_cfgs_to_add_mod_list.size() > 0) {
    j.start_array("slotSpecificConfigurationsToAddModList");
    for (const auto& e1 : slot_specific_cfgs_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (slot_specific_cfgs_to_release_list.size() > 0) {
    j.start_array("slotSpecificConfigurationsToReleaseList");
    for (const auto& e1 : slot_specific_cfgs_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// UplinkConfig ::= SEQUENCE
SRSASN_CODE ul_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(init_ul_bwp_present, 1));
  HANDLE_CODE(bref.pack(ul_bwp_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ul_bwp_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(first_active_ul_bwp_id_present, 1));
  HANDLE_CODE(bref.pack(pusch_serving_cell_cfg_present, 1));
  HANDLE_CODE(bref.pack(carrier_switching_present, 1));

  if (init_ul_bwp_present) {
    HANDLE_CODE(init_ul_bwp.pack(bref));
  }
  if (ul_bwp_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_bwp_to_release_list, 1, 4, integer_packer<uint8_t>(0, 4)));
  }
  if (ul_bwp_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_bwp_to_add_mod_list, 1, 4));
  }
  if (first_active_ul_bwp_id_present) {
    HANDLE_CODE(pack_integer(bref, first_active_ul_bwp_id, (uint8_t)0u, (uint8_t)4u));
  }
  if (pusch_serving_cell_cfg_present) {
    HANDLE_CODE(pusch_serving_cell_cfg.pack(bref));
  }
  if (carrier_switching_present) {
    HANDLE_CODE(carrier_switching.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pwr_boost_pi2_bpsk_present;
    group_flags[0] |= ul_ch_bw_per_scs_list.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pwr_boost_pi2_bpsk_present, 1));
      HANDLE_CODE(bref.pack(ul_ch_bw_per_scs_list.is_present(), 1));
      if (pwr_boost_pi2_bpsk_present) {
        HANDLE_CODE(bref.pack(pwr_boost_pi2_bpsk, 1));
      }
      if (ul_ch_bw_per_scs_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ul_ch_bw_per_scs_list, 1, 5));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(init_ul_bwp_present, 1));
  bool ul_bwp_to_release_list_present;
  HANDLE_CODE(bref.unpack(ul_bwp_to_release_list_present, 1));
  bool ul_bwp_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(ul_bwp_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(first_active_ul_bwp_id_present, 1));
  HANDLE_CODE(bref.unpack(pusch_serving_cell_cfg_present, 1));
  HANDLE_CODE(bref.unpack(carrier_switching_present, 1));

  if (init_ul_bwp_present) {
    HANDLE_CODE(init_ul_bwp.unpack(bref));
  }
  if (ul_bwp_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_bwp_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 4)));
  }
  if (ul_bwp_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_bwp_to_add_mod_list, bref, 1, 4));
  }
  if (first_active_ul_bwp_id_present) {
    HANDLE_CODE(unpack_integer(first_active_ul_bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  }
  if (pusch_serving_cell_cfg_present) {
    HANDLE_CODE(pusch_serving_cell_cfg.unpack(bref));
  }
  if (carrier_switching_present) {
    HANDLE_CODE(carrier_switching.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pwr_boost_pi2_bpsk_present, 1));
      bool ul_ch_bw_per_scs_list_present;
      HANDLE_CODE(bref.unpack(ul_ch_bw_per_scs_list_present, 1));
      ul_ch_bw_per_scs_list.set_present(ul_ch_bw_per_scs_list_present);
      if (pwr_boost_pi2_bpsk_present) {
        HANDLE_CODE(bref.unpack(pwr_boost_pi2_bpsk, 1));
      }
      if (ul_ch_bw_per_scs_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_ch_bw_per_scs_list, bref, 1, 5));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void ul_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (init_ul_bwp_present) {
    j.write_fieldname("initialUplinkBWP");
    init_ul_bwp.to_json(j);
  }
  if (ul_bwp_to_release_list.size() > 0) {
    j.start_array("uplinkBWP-ToReleaseList");
    for (const auto& e1 : ul_bwp_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (ul_bwp_to_add_mod_list.size() > 0) {
    j.start_array("uplinkBWP-ToAddModList");
    for (const auto& e1 : ul_bwp_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (first_active_ul_bwp_id_present) {
    j.write_int("firstActiveUplinkBWP-Id", first_active_ul_bwp_id);
  }
  if (pusch_serving_cell_cfg_present) {
    j.write_fieldname("pusch-ServingCellConfig");
    pusch_serving_cell_cfg.to_json(j);
  }
  if (carrier_switching_present) {
    j.write_fieldname("carrierSwitching");
    carrier_switching.to_json(j);
  }
  if (ext) {
    if (pwr_boost_pi2_bpsk_present) {
      j.write_bool("powerBoostPi2BPSK", pwr_boost_pi2_bpsk);
    }
    if (ul_ch_bw_per_scs_list.is_present()) {
      j.start_array("uplinkChannelBW-PerSCS-List");
      for (const auto& e1 : *ul_ch_bw_per_scs_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// ServingCellConfig ::= SEQUENCE
SRSASN_CODE serving_cell_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tdd_ul_dl_cfg_ded_present, 1));
  HANDLE_CODE(bref.pack(init_dl_bwp_present, 1));
  HANDLE_CODE(bref.pack(dl_bwp_to_release_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(dl_bwp_to_add_mod_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(first_active_dl_bwp_id_present, 1));
  HANDLE_CODE(bref.pack(bwp_inactivity_timer_present, 1));
  HANDLE_CODE(bref.pack(default_dl_bwp_id_present, 1));
  HANDLE_CODE(bref.pack(ul_cfg_present, 1));
  HANDLE_CODE(bref.pack(supplementary_ul_present, 1));
  HANDLE_CODE(bref.pack(pdcch_serving_cell_cfg_present, 1));
  HANDLE_CODE(bref.pack(pdsch_serving_cell_cfg_present, 1));
  HANDLE_CODE(bref.pack(csi_meas_cfg_present, 1));
  HANDLE_CODE(bref.pack(scell_deactivation_timer_present, 1));
  HANDLE_CODE(bref.pack(cross_carrier_sched_cfg_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(pathloss_ref_linking_present, 1));
  HANDLE_CODE(bref.pack(serving_cell_mo_present, 1));

  if (tdd_ul_dl_cfg_ded_present) {
    HANDLE_CODE(tdd_ul_dl_cfg_ded.pack(bref));
  }
  if (init_dl_bwp_present) {
    HANDLE_CODE(init_dl_bwp.pack(bref));
  }
  if (dl_bwp_to_release_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_bwp_to_release_list, 1, 4, integer_packer<uint8_t>(0, 4)));
  }
  if (dl_bwp_to_add_mod_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_bwp_to_add_mod_list, 1, 4));
  }
  if (first_active_dl_bwp_id_present) {
    HANDLE_CODE(pack_integer(bref, first_active_dl_bwp_id, (uint8_t)0u, (uint8_t)4u));
  }
  if (bwp_inactivity_timer_present) {
    HANDLE_CODE(bwp_inactivity_timer.pack(bref));
  }
  if (default_dl_bwp_id_present) {
    HANDLE_CODE(pack_integer(bref, default_dl_bwp_id, (uint8_t)0u, (uint8_t)4u));
  }
  if (ul_cfg_present) {
    HANDLE_CODE(ul_cfg.pack(bref));
  }
  if (supplementary_ul_present) {
    HANDLE_CODE(supplementary_ul.pack(bref));
  }
  if (pdcch_serving_cell_cfg_present) {
    HANDLE_CODE(pdcch_serving_cell_cfg.pack(bref));
  }
  if (pdsch_serving_cell_cfg_present) {
    HANDLE_CODE(pdsch_serving_cell_cfg.pack(bref));
  }
  if (csi_meas_cfg_present) {
    HANDLE_CODE(csi_meas_cfg.pack(bref));
  }
  if (scell_deactivation_timer_present) {
    HANDLE_CODE(scell_deactivation_timer.pack(bref));
  }
  if (cross_carrier_sched_cfg_present) {
    HANDLE_CODE(cross_carrier_sched_cfg.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, tag_id, (uint8_t)0u, (uint8_t)3u));
  if (pathloss_ref_linking_present) {
    HANDLE_CODE(pathloss_ref_linking.pack(bref));
  }
  if (serving_cell_mo_present) {
    HANDLE_CODE(pack_integer(bref, serving_cell_mo, (uint8_t)1u, (uint8_t)64u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= lte_crs_to_match_around.is_present();
    group_flags[0] |= rate_match_pattern_to_add_mod_list.is_present();
    group_flags[0] |= rate_match_pattern_to_release_list.is_present();
    group_flags[0] |= dl_ch_bw_per_scs_list.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(lte_crs_to_match_around.is_present(), 1));
      HANDLE_CODE(bref.pack(rate_match_pattern_to_add_mod_list.is_present(), 1));
      HANDLE_CODE(bref.pack(rate_match_pattern_to_release_list.is_present(), 1));
      HANDLE_CODE(bref.pack(dl_ch_bw_per_scs_list.is_present(), 1));
      if (lte_crs_to_match_around.is_present()) {
        HANDLE_CODE(lte_crs_to_match_around->pack(bref));
      }
      if (rate_match_pattern_to_add_mod_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *rate_match_pattern_to_add_mod_list, 1, 4));
      }
      if (rate_match_pattern_to_release_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *rate_match_pattern_to_release_list, 1, 4, integer_packer<uint8_t>(0, 3)));
      }
      if (dl_ch_bw_per_scs_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *dl_ch_bw_per_scs_list, 1, 5));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tdd_ul_dl_cfg_ded_present, 1));
  HANDLE_CODE(bref.unpack(init_dl_bwp_present, 1));
  bool dl_bwp_to_release_list_present;
  HANDLE_CODE(bref.unpack(dl_bwp_to_release_list_present, 1));
  bool dl_bwp_to_add_mod_list_present;
  HANDLE_CODE(bref.unpack(dl_bwp_to_add_mod_list_present, 1));
  HANDLE_CODE(bref.unpack(first_active_dl_bwp_id_present, 1));
  HANDLE_CODE(bref.unpack(bwp_inactivity_timer_present, 1));
  HANDLE_CODE(bref.unpack(default_dl_bwp_id_present, 1));
  HANDLE_CODE(bref.unpack(ul_cfg_present, 1));
  HANDLE_CODE(bref.unpack(supplementary_ul_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_serving_cell_cfg_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_serving_cell_cfg_present, 1));
  HANDLE_CODE(bref.unpack(csi_meas_cfg_present, 1));
  HANDLE_CODE(bref.unpack(scell_deactivation_timer_present, 1));
  HANDLE_CODE(bref.unpack(cross_carrier_sched_cfg_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(pathloss_ref_linking_present, 1));
  HANDLE_CODE(bref.unpack(serving_cell_mo_present, 1));

  if (tdd_ul_dl_cfg_ded_present) {
    HANDLE_CODE(tdd_ul_dl_cfg_ded.unpack(bref));
  }
  if (init_dl_bwp_present) {
    HANDLE_CODE(init_dl_bwp.unpack(bref));
  }
  if (dl_bwp_to_release_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_bwp_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 4)));
  }
  if (dl_bwp_to_add_mod_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(dl_bwp_to_add_mod_list, bref, 1, 4));
  }
  if (first_active_dl_bwp_id_present) {
    HANDLE_CODE(unpack_integer(first_active_dl_bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  }
  if (bwp_inactivity_timer_present) {
    HANDLE_CODE(bwp_inactivity_timer.unpack(bref));
  }
  if (default_dl_bwp_id_present) {
    HANDLE_CODE(unpack_integer(default_dl_bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  }
  if (ul_cfg_present) {
    HANDLE_CODE(ul_cfg.unpack(bref));
  }
  if (supplementary_ul_present) {
    HANDLE_CODE(supplementary_ul.unpack(bref));
  }
  if (pdcch_serving_cell_cfg_present) {
    HANDLE_CODE(pdcch_serving_cell_cfg.unpack(bref));
  }
  if (pdsch_serving_cell_cfg_present) {
    HANDLE_CODE(pdsch_serving_cell_cfg.unpack(bref));
  }
  if (csi_meas_cfg_present) {
    HANDLE_CODE(csi_meas_cfg.unpack(bref));
  }
  if (scell_deactivation_timer_present) {
    HANDLE_CODE(scell_deactivation_timer.unpack(bref));
  }
  if (cross_carrier_sched_cfg_present) {
    HANDLE_CODE(cross_carrier_sched_cfg.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(tag_id, bref, (uint8_t)0u, (uint8_t)3u));
  if (pathloss_ref_linking_present) {
    HANDLE_CODE(pathloss_ref_linking.unpack(bref));
  }
  if (serving_cell_mo_present) {
    HANDLE_CODE(unpack_integer(serving_cell_mo, bref, (uint8_t)1u, (uint8_t)64u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool lte_crs_to_match_around_present;
      HANDLE_CODE(bref.unpack(lte_crs_to_match_around_present, 1));
      lte_crs_to_match_around.set_present(lte_crs_to_match_around_present);
      bool rate_match_pattern_to_add_mod_list_present;
      HANDLE_CODE(bref.unpack(rate_match_pattern_to_add_mod_list_present, 1));
      rate_match_pattern_to_add_mod_list.set_present(rate_match_pattern_to_add_mod_list_present);
      bool rate_match_pattern_to_release_list_present;
      HANDLE_CODE(bref.unpack(rate_match_pattern_to_release_list_present, 1));
      rate_match_pattern_to_release_list.set_present(rate_match_pattern_to_release_list_present);
      bool dl_ch_bw_per_scs_list_present;
      HANDLE_CODE(bref.unpack(dl_ch_bw_per_scs_list_present, 1));
      dl_ch_bw_per_scs_list.set_present(dl_ch_bw_per_scs_list_present);
      if (lte_crs_to_match_around.is_present()) {
        HANDLE_CODE(lte_crs_to_match_around->unpack(bref));
      }
      if (rate_match_pattern_to_add_mod_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*rate_match_pattern_to_add_mod_list, bref, 1, 4));
      }
      if (rate_match_pattern_to_release_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*rate_match_pattern_to_release_list, bref, 1, 4, integer_packer<uint8_t>(0, 3)));
      }
      if (dl_ch_bw_per_scs_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*dl_ch_bw_per_scs_list, bref, 1, 5));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void serving_cell_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (tdd_ul_dl_cfg_ded_present) {
    j.write_fieldname("tdd-UL-DL-ConfigurationDedicated");
    tdd_ul_dl_cfg_ded.to_json(j);
  }
  if (init_dl_bwp_present) {
    j.write_fieldname("initialDownlinkBWP");
    init_dl_bwp.to_json(j);
  }
  if (dl_bwp_to_release_list.size() > 0) {
    j.start_array("downlinkBWP-ToReleaseList");
    for (const auto& e1 : dl_bwp_to_release_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (dl_bwp_to_add_mod_list.size() > 0) {
    j.start_array("downlinkBWP-ToAddModList");
    for (const auto& e1 : dl_bwp_to_add_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (first_active_dl_bwp_id_present) {
    j.write_int("firstActiveDownlinkBWP-Id", first_active_dl_bwp_id);
  }
  if (bwp_inactivity_timer_present) {
    j.write_str("bwp-InactivityTimer", bwp_inactivity_timer.to_string());
  }
  if (default_dl_bwp_id_present) {
    j.write_int("defaultDownlinkBWP-Id", default_dl_bwp_id);
  }
  if (ul_cfg_present) {
    j.write_fieldname("uplinkConfig");
    ul_cfg.to_json(j);
  }
  if (supplementary_ul_present) {
    j.write_fieldname("supplementaryUplink");
    supplementary_ul.to_json(j);
  }
  if (pdcch_serving_cell_cfg_present) {
    j.write_fieldname("pdcch-ServingCellConfig");
    pdcch_serving_cell_cfg.to_json(j);
  }
  if (pdsch_serving_cell_cfg_present) {
    j.write_fieldname("pdsch-ServingCellConfig");
    pdsch_serving_cell_cfg.to_json(j);
  }
  if (csi_meas_cfg_present) {
    j.write_fieldname("csi-MeasConfig");
    csi_meas_cfg.to_json(j);
  }
  if (scell_deactivation_timer_present) {
    j.write_str("sCellDeactivationTimer", scell_deactivation_timer.to_string());
  }
  if (cross_carrier_sched_cfg_present) {
    j.write_fieldname("crossCarrierSchedulingConfig");
    cross_carrier_sched_cfg.to_json(j);
  }
  j.write_int("tag-Id", tag_id);
  if (dummy_present) {
    j.write_str("dummy", "enabled");
  }
  if (pathloss_ref_linking_present) {
    j.write_str("pathlossReferenceLinking", pathloss_ref_linking.to_string());
  }
  if (serving_cell_mo_present) {
    j.write_int("servingCellMO", serving_cell_mo);
  }
  if (ext) {
    if (lte_crs_to_match_around.is_present()) {
      j.write_fieldname("lte-CRS-ToMatchAround");
      lte_crs_to_match_around->to_json(j);
    }
    if (rate_match_pattern_to_add_mod_list.is_present()) {
      j.start_array("rateMatchPatternToAddModList");
      for (const auto& e1 : *rate_match_pattern_to_add_mod_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (rate_match_pattern_to_release_list.is_present()) {
      j.start_array("rateMatchPatternToReleaseList");
      for (const auto& e1 : *rate_match_pattern_to_release_list) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (dl_ch_bw_per_scs_list.is_present()) {
      j.start_array("downlinkChannelBW-PerSCS-List");
      for (const auto& e1 : *dl_ch_bw_per_scs_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* serving_cell_cfg_s::bwp_inactivity_timer_opts::to_string() const
{
  static const char* names[] = {"ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",     "ms10",   "ms20",
                                "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",   "ms200",  "ms300",
                                "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare10", "spare9", "spare8",
                                "spare7", "spare6", "spare5", "spare4", "spare3", "spare2",  "spare1"};
  return convert_enum_idx(names, 31, value, "serving_cell_cfg_s::bwp_inactivity_timer_e_");
}
uint16_t serving_cell_cfg_s::bwp_inactivity_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {2,  3,  4,   5,   6,   8,   10,  20,   30,   40,  50,
                                     60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 21, value, "serving_cell_cfg_s::bwp_inactivity_timer_e_");
}

const char* serving_cell_cfg_s::scell_deactivation_timer_opts::to_string() const
{
  static const char* names[] = {"ms20",
                                "ms40",
                                "ms80",
                                "ms160",
                                "ms200",
                                "ms240",
                                "ms320",
                                "ms400",
                                "ms480",
                                "ms520",
                                "ms640",
                                "ms720",
                                "ms840",
                                "ms1280",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "serving_cell_cfg_s::scell_deactivation_timer_e_");
}
uint16_t serving_cell_cfg_s::scell_deactivation_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 80, 160, 200, 240, 320, 400, 480, 520, 640, 720, 840, 1280};
  return map_enum_number(numbers, 14, value, "serving_cell_cfg_s::scell_deactivation_timer_e_");
}

const char* serving_cell_cfg_s::pathloss_ref_linking_opts::to_string() const
{
  static const char* names[] = {"spCell", "sCell"};
  return convert_enum_idx(names, 2, value, "serving_cell_cfg_s::pathloss_ref_linking_e_");
}
