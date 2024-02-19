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

#include "srsran/asn1/rrc_nr/serving_cell.h"
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

const char* eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneFrame", "fourFrames"};
  return convert_enum_idx(names, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types");
}
uint8_t eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(numbers, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types");
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

const char* eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneFrame", "fourFrames"};
  return convert_enum_idx(names, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types");
}
uint8_t eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(numbers, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types");
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

// SubgroupConfig-r17 ::= SEQUENCE
SRSASN_CODE subgroup_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(subgroups_num_for_ue_id_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, subgroups_num_per_po_r17, (uint8_t)1u, (uint8_t)8u));
  if (subgroups_num_for_ue_id_r17_present) {
    HANDLE_CODE(pack_integer(bref, subgroups_num_for_ue_id_r17, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE subgroup_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(subgroups_num_for_ue_id_r17_present, 1));

  HANDLE_CODE(unpack_integer(subgroups_num_per_po_r17, bref, (uint8_t)1u, (uint8_t)8u));
  if (subgroups_num_for_ue_id_r17_present) {
    HANDLE_CODE(unpack_integer(subgroups_num_for_ue_id_r17, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void subgroup_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("subgroupsNumPerPO-r17", subgroups_num_per_po_r17);
  if (subgroups_num_for_ue_id_r17_present) {
    j.write_int("subgroupsNumForUEID-r17", subgroups_num_for_ue_id_r17);
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present;
    group_flags[1] |= ran_paging_in_idle_po_r17_present;
    group_flags[1] |= first_pdcch_monitoring_occasion_of_po_v1710.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present, 1));
      if (nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present) {
        HANDLE_CODE(pack_integer(bref, nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16, (uint8_t)2u, (uint8_t)4u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ran_paging_in_idle_po_r17_present, 1));
      HANDLE_CODE(bref.pack(first_pdcch_monitoring_occasion_of_po_v1710.is_present(), 1));
      if (first_pdcch_monitoring_occasion_of_po_v1710.is_present()) {
        HANDLE_CODE(first_pdcch_monitoring_occasion_of_po_v1710->pack(bref));
      }
    }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present, 1));
      if (nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present) {
        HANDLE_CODE(unpack_integer(nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16, bref, (uint8_t)2u, (uint8_t)4u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ran_paging_in_idle_po_r17_present, 1));
      unpack_presence_flag(first_pdcch_monitoring_occasion_of_po_v1710, bref);
      if (first_pdcch_monitoring_occasion_of_po_v1710.is_present()) {
        HANDLE_CODE(first_pdcch_monitoring_occasion_of_po_v1710->unpack(bref));
      }
    }
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
  if (ext) {
    if (nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present) {
      j.write_int("nrofPDCCH-MonitoringOccasionPerSSB-InPO-r16", nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16);
    }
    if (ran_paging_in_idle_po_r17_present) {
      j.write_str("ranPagingInIdlePO-r17", "true");
    }
    if (first_pdcch_monitoring_occasion_of_po_v1710.is_present()) {
      j.write_fieldname("firstPDCCH-MonitoringOccasionOfPO-v1710");
      first_pdcch_monitoring_occasion_of_po_v1710->to_json(j);
    }
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

const char* pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneT", "halfT", "quarterT", "oneEighthT", "oneSixteenthT"};
  return convert_enum_idx(names, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
}
float pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_number() const
{
  static const float numbers[] = {1.0, 0.5, 0.25, 0.125, 0.0625};
  return map_enum_number(numbers, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
}
const char* pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_number_string() const
{
  static const char* number_strs[] = {"1", "0.5", "0.25", "1/8", "1/16"};
  return convert_enum_idx(number_strs, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
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
    case types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.destroy<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.destroy<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t:
      c.destroy<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
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
    case types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.init<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.init<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
      break;
    case types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t:
      c.init<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
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
    case types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.init(other.c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.init(other.c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t:
      c.init(other.c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>());
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
    case types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      c.set(other.c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      c.set(other.c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>());
      break;
    case types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t:
      c.set(other.c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>());
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
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
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
    case types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      j.start_array("sCS480KHZoneT-SCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT");
      for (const auto& e1 :
           c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      j.start_array("sCS480KHZhalfT-SCS120KHZoneEighthT-SCS60KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t:
      j.start_array("sCS480KHZquarterT-SCS120KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>()) {
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
    case types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(
          bref,
          c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>(),
          1,
          4,
          integer_packer<uint16_t>(0, 4479)));
      break;
    case types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>(),
                                  1,
                                  4,
                                  integer_packer<uint16_t>(0, 8959)));
      break;
    case types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t:
      HANDLE_CODE(pack_dyn_seq_of(
          bref, c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>(), 1, 4, integer_packer<uint16_t>(0, 17919)));
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
    case types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>(),
          bref,
          1,
          4,
          integer_packer<uint16_t>(0, 4479)));
      break;
    case types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>(),
                                    bref,
                                    1,
                                    4,
                                    integer_packer<uint16_t>(0, 8959)));
      break;
    case types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>(), bref, 1, 4, integer_packer<uint16_t>(0, 17919)));
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types_opts::to_string() const
{
  static const char* names[] = {"sCS15KHZoneT",
                                "sCS30KHZoneT-SCS15KHZhalfT",
                                "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT",
                                "sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                                "sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                                "sCS480KHZoneT-SCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                                "sCS480KHZhalfT-SCS120KHZoneEighthT-SCS60KHZoneSixteenthT",
                                "sCS480KHZquarterT-SCS120KHZoneSixteenthT"};
  return convert_enum_idx(names, 8, value, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types");
}

void pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::destroy_()
{
  switch (type_) {
    case types::scs480_kh_zone_eighth_t:
      c.destroy<scs480_kh_zone_eighth_t_l_>();
      break;
    case types::scs480_kh_zone_sixteenth_t:
      c.destroy<scs480_kh_zone_sixteenth_t_l_>();
      break;
    default:
      break;
  }
}
void pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::scs480_kh_zone_eighth_t:
      c.init<scs480_kh_zone_eighth_t_l_>();
      break;
    case types::scs480_kh_zone_sixteenth_t:
      c.init<scs480_kh_zone_sixteenth_t_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_");
  }
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::first_pdcch_monitoring_occasion_of_po_v1710_c_(
    const pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::scs480_kh_zone_eighth_t:
      c.init(other.c.get<scs480_kh_zone_eighth_t_l_>());
      break;
    case types::scs480_kh_zone_sixteenth_t:
      c.init(other.c.get<scs480_kh_zone_sixteenth_t_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_");
  }
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::operator=(
    const pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::scs480_kh_zone_eighth_t:
      c.set(other.c.get<scs480_kh_zone_eighth_t_l_>());
      break;
    case types::scs480_kh_zone_sixteenth_t:
      c.set(other.c.get<scs480_kh_zone_sixteenth_t_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_");
  }

  return *this;
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_eighth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::set_scs480_kh_zone_eighth_t()
{
  set(types::scs480_kh_zone_eighth_t);
  return c.get<scs480_kh_zone_eighth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_sixteenth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::set_scs480_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zone_sixteenth_t_l_>();
}
void pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scs480_kh_zone_eighth_t:
      j.start_array("sCS480KHZoneEighthT");
      for (const auto& e1 : c.get<scs480_kh_zone_eighth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scs480_kh_zone_sixteenth_t:
      j.start_array("sCS480KHZoneSixteenthT");
      for (const auto& e1 : c.get<scs480_kh_zone_sixteenth_t_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_");
  }
  j.end_obj();
}
SRSASN_CODE pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scs480_kh_zone_eighth_t:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scs480_kh_zone_eighth_t_l_>(), 1, 4, integer_packer<uint16_t>(0, 35839)));
      break;
    case types::scs480_kh_zone_sixteenth_t:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<scs480_kh_zone_sixteenth_t_l_>(), 1, 4, integer_packer<uint32_t>(0, 71679)));
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scs480_kh_zone_eighth_t:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<scs480_kh_zone_eighth_t_l_>(), bref, 1, 4, integer_packer<uint16_t>(0, 35839)));
      break;
    case types::scs480_kh_zone_sixteenth_t:
      HANDLE_CODE(
          unpack_dyn_seq_of(c.get<scs480_kh_zone_sixteenth_t_l_>(), bref, 1, 4, integer_packer<uint32_t>(0, 71679)));
      break;
    default:
      log_invalid_choice_id(type_, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types_opts::to_string() const
{
  static const char* names[] = {"sCS480KHZoneEighthT", "sCS480KHZoneSixteenthT"};
  return convert_enum_idx(names, 2, value, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types");
}

// PEI-Config-r17 ::= SEQUENCE
SRSASN_CODE pei_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(last_used_cell_only_r17_present, 1));

  HANDLE_CODE(po_num_per_pei_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, payload_size_dci_2_7_r17, (uint8_t)1u, (uint8_t)43u));
  HANDLE_CODE(pack_integer(bref, pei_frame_offset_r17, (uint8_t)0u, (uint8_t)16u));
  HANDLE_CODE(subgroup_cfg_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pei_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(last_used_cell_only_r17_present, 1));

  HANDLE_CODE(po_num_per_pei_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(payload_size_dci_2_7_r17, bref, (uint8_t)1u, (uint8_t)43u));
  HANDLE_CODE(unpack_integer(pei_frame_offset_r17, bref, (uint8_t)0u, (uint8_t)16u));
  HANDLE_CODE(subgroup_cfg_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void pei_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("po-NumPerPEI-r17", po_num_per_pei_r17.to_string());
  j.write_int("payloadSizeDCI-2-7-r17", payload_size_dci_2_7_r17);
  j.write_int("pei-FrameOffset-r17", pei_frame_offset_r17);
  j.write_fieldname("subgroupConfig-r17");
  subgroup_cfg_r17.to_json(j);
  if (last_used_cell_only_r17_present) {
    j.write_str("lastUsedCellOnly-r17", "true");
  }
  j.end_obj();
}

const char* pei_cfg_r17_s::po_num_per_pei_r17_opts::to_string() const
{
  static const char* names[] = {"po1", "po2", "po4", "po8"};
  return convert_enum_idx(names, 4, value, "pei_cfg_r17_s::po_num_per_pei_r17_e_");
}
uint8_t pei_cfg_r17_s::po_num_per_pei_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "pei_cfg_r17_s::po_num_per_pei_r17_e_");
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pei_cfg_r17.is_present();
    group_flags[0] |= init_dl_bwp_red_cap_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pei_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(init_dl_bwp_red_cap_r17.is_present(), 1));
      if (pei_cfg_r17.is_present()) {
        HANDLE_CODE(pei_cfg_r17->pack(bref));
      }
      if (init_dl_bwp_red_cap_r17.is_present()) {
        HANDLE_CODE(init_dl_bwp_red_cap_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_cfg_common_sib_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(freq_info_dl.unpack(bref));
  HANDLE_CODE(init_dl_bwp.unpack(bref));
  HANDLE_CODE(bcch_cfg.unpack(bref));
  HANDLE_CODE(pcch_cfg.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(pei_cfg_r17, bref);
      unpack_presence_flag(init_dl_bwp_red_cap_r17, bref);
      if (pei_cfg_r17.is_present()) {
        HANDLE_CODE(pei_cfg_r17->unpack(bref));
      }
      if (init_dl_bwp_red_cap_r17.is_present()) {
        HANDLE_CODE(init_dl_bwp_red_cap_r17->unpack(bref));
      }
    }
  }
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
  if (ext) {
    if (pei_cfg_r17.is_present()) {
      j.write_fieldname("pei-Config-r17");
      pei_cfg_r17->to_json(j);
    }
    if (init_dl_bwp_red_cap_r17.is_present()) {
      j.write_fieldname("initialDownlinkBWP-RedCap-r17");
      init_dl_bwp_red_cap_r17->to_json(j);
    }
  }
  j.end_obj();
}

// HighSpeedConfig-r16 ::= SEQUENCE
SRSASN_CODE high_speed_cfg_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(high_speed_meas_flag_r16_present, 1));
  HANDLE_CODE(bref.pack(high_speed_demod_flag_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE high_speed_cfg_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(high_speed_meas_flag_r16_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_demod_flag_r16_present, 1));

  return SRSASN_SUCCESS;
}
void high_speed_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (high_speed_meas_flag_r16_present) {
    j.write_str("highSpeedMeasFlag-r16", "true");
  }
  if (high_speed_demod_flag_r16_present) {
    j.write_str("highSpeedDemodFlag-r16", "true");
  }
  j.end_obj();
}

// HighSpeedConfigFR2-r17 ::= SEQUENCE
SRSASN_CODE high_speed_cfg_fr2_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(high_speed_meas_flag_fr2_r17_present, 1));
  HANDLE_CODE(bref.pack(high_speed_deployment_type_fr2_r17_present, 1));
  HANDLE_CODE(bref.pack(high_speed_large_one_step_ul_timing_fr2_r17_present, 1));

  if (high_speed_meas_flag_fr2_r17_present) {
    HANDLE_CODE(high_speed_meas_flag_fr2_r17.pack(bref));
  }
  if (high_speed_deployment_type_fr2_r17_present) {
    HANDLE_CODE(high_speed_deployment_type_fr2_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE high_speed_cfg_fr2_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(high_speed_meas_flag_fr2_r17_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_deployment_type_fr2_r17_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_large_one_step_ul_timing_fr2_r17_present, 1));

  if (high_speed_meas_flag_fr2_r17_present) {
    HANDLE_CODE(high_speed_meas_flag_fr2_r17.unpack(bref));
  }
  if (high_speed_deployment_type_fr2_r17_present) {
    HANDLE_CODE(high_speed_deployment_type_fr2_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void high_speed_cfg_fr2_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (high_speed_meas_flag_fr2_r17_present) {
    j.write_str("highSpeedMeasFlagFR2-r17", high_speed_meas_flag_fr2_r17.to_string());
  }
  if (high_speed_deployment_type_fr2_r17_present) {
    j.write_str("highSpeedDeploymentTypeFR2-r17", high_speed_deployment_type_fr2_r17.to_string());
  }
  if (high_speed_large_one_step_ul_timing_fr2_r17_present) {
    j.write_str("highSpeedLargeOneStepUL-TimingFR2-r17", "true");
  }
  j.end_obj();
}

const char* high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_opts::to_string() const
{
  static const char* names[] = {"set1", "set2"};
  return convert_enum_idx(names, 2, value, "high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_e_");
}
uint8_t high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_e_");
}

const char* high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17_opts::to_string() const
{
  static const char* names[] = {"unidirectional", "bidirectional"};
  return convert_enum_idx(names, 2, value, "high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17_e_");
}

// SemiStaticChannelAccessConfig-r16 ::= SEQUENCE
SRSASN_CODE semi_static_ch_access_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(period.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE semi_static_ch_access_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(period.unpack(bref));

  return SRSASN_SUCCESS;
}
void semi_static_ch_access_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("period", period.to_string());
  j.end_obj();
}

const char* semi_static_ch_access_cfg_r16_s::period_opts::to_string() const
{
  static const char* names[] = {"ms1", "ms2", "ms2dot5", "ms4", "ms5", "ms10"};
  return convert_enum_idx(names, 6, value, "semi_static_ch_access_cfg_r16_s::period_e_");
}
float semi_static_ch_access_cfg_r16_s::period_opts::to_number() const
{
  static const float numbers[] = {1.0, 2.0, 2.5, 4.0, 5.0, 10.0};
  return map_enum_number(numbers, 6, value, "semi_static_ch_access_cfg_r16_s::period_e_");
}
const char* semi_static_ch_access_cfg_r16_s::period_opts::to_number_string() const
{
  static const char* number_strs[] = {"1", "2", "2.5", "4", "5", "10"};
  return convert_enum_idx(number_strs, 6, value, "semi_static_ch_access_cfg_r16_s::period_e_");
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

// UplinkConfigCommonSIB-v1700 ::= SEQUENCE
SRSASN_CODE ul_cfg_common_sib_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(init_ul_bwp_red_cap_r17_present, 1));

  if (init_ul_bwp_red_cap_r17_present) {
    HANDLE_CODE(init_ul_bwp_red_cap_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_cfg_common_sib_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(init_ul_bwp_red_cap_r17_present, 1));

  if (init_ul_bwp_red_cap_r17_present) {
    HANDLE_CODE(init_ul_bwp_red_cap_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_cfg_common_sib_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (init_ul_bwp_red_cap_r17_present) {
    j.write_fieldname("initialUplinkBWP-RedCap-r17");
    init_ul_bwp_red_cap_r17.to_json(j);
  }
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ch_access_mode_r16.is_present();
    group_flags[0] |= discovery_burst_win_len_r16_present;
    group_flags[0] |= high_speed_cfg_r16.is_present();
    group_flags[1] |= ch_access_mode2_r17_present;
    group_flags[1] |= discovery_burst_win_len_v1700_present;
    group_flags[1] |= high_speed_cfg_fr2_r17.is_present();
    group_flags[1] |= ul_cfg_common_v1700.is_present();
    group_flags[2] |= enhanced_meas_leo_r17_present;
    group_flags[3] |= ra_ch_access_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ch_access_mode_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(discovery_burst_win_len_r16_present, 1));
      HANDLE_CODE(bref.pack(high_speed_cfg_r16.is_present(), 1));
      if (ch_access_mode_r16.is_present()) {
        HANDLE_CODE(ch_access_mode_r16->pack(bref));
      }
      if (discovery_burst_win_len_r16_present) {
        HANDLE_CODE(discovery_burst_win_len_r16.pack(bref));
      }
      if (high_speed_cfg_r16.is_present()) {
        HANDLE_CODE(high_speed_cfg_r16->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ch_access_mode2_r17_present, 1));
      HANDLE_CODE(bref.pack(discovery_burst_win_len_v1700_present, 1));
      HANDLE_CODE(bref.pack(high_speed_cfg_fr2_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_cfg_common_v1700.is_present(), 1));
      if (discovery_burst_win_len_v1700_present) {
        HANDLE_CODE(discovery_burst_win_len_v1700.pack(bref));
      }
      if (high_speed_cfg_fr2_r17.is_present()) {
        HANDLE_CODE(high_speed_cfg_fr2_r17->pack(bref));
      }
      if (ul_cfg_common_v1700.is_present()) {
        HANDLE_CODE(ul_cfg_common_v1700->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(enhanced_meas_leo_r17_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ra_ch_access_r17_present, 1));
    }
  }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ch_access_mode_r16, bref);
      HANDLE_CODE(bref.unpack(discovery_burst_win_len_r16_present, 1));
      unpack_presence_flag(high_speed_cfg_r16, bref);
      if (ch_access_mode_r16.is_present()) {
        HANDLE_CODE(ch_access_mode_r16->unpack(bref));
      }
      if (discovery_burst_win_len_r16_present) {
        HANDLE_CODE(discovery_burst_win_len_r16.unpack(bref));
      }
      if (high_speed_cfg_r16.is_present()) {
        HANDLE_CODE(high_speed_cfg_r16->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ch_access_mode2_r17_present, 1));
      HANDLE_CODE(bref.unpack(discovery_burst_win_len_v1700_present, 1));
      unpack_presence_flag(high_speed_cfg_fr2_r17, bref);
      unpack_presence_flag(ul_cfg_common_v1700, bref);
      if (discovery_burst_win_len_v1700_present) {
        HANDLE_CODE(discovery_burst_win_len_v1700.unpack(bref));
      }
      if (high_speed_cfg_fr2_r17.is_present()) {
        HANDLE_CODE(high_speed_cfg_fr2_r17->unpack(bref));
      }
      if (ul_cfg_common_v1700.is_present()) {
        HANDLE_CODE(ul_cfg_common_v1700->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(enhanced_meas_leo_r17_present, 1));
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ra_ch_access_r17_present, 1));
    }
  }
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
  if (ext) {
    if (ch_access_mode_r16.is_present()) {
      j.write_fieldname("channelAccessMode-r16");
      ch_access_mode_r16->to_json(j);
    }
    if (discovery_burst_win_len_r16_present) {
      j.write_str("discoveryBurstWindowLength-r16", discovery_burst_win_len_r16.to_string());
    }
    if (high_speed_cfg_r16.is_present()) {
      j.write_fieldname("highSpeedConfig-r16");
      high_speed_cfg_r16->to_json(j);
    }
    if (ch_access_mode2_r17_present) {
      j.write_str("channelAccessMode2-r17", "enabled");
    }
    if (discovery_burst_win_len_v1700_present) {
      j.write_str("discoveryBurstWindowLength-v1700", discovery_burst_win_len_v1700.to_string());
    }
    if (high_speed_cfg_fr2_r17.is_present()) {
      j.write_fieldname("highSpeedConfigFR2-r17");
      high_speed_cfg_fr2_r17->to_json(j);
    }
    if (ul_cfg_common_v1700.is_present()) {
      j.write_fieldname("uplinkConfigCommon-v1700");
      ul_cfg_common_v1700->to_json(j);
    }
    if (enhanced_meas_leo_r17_present) {
      j.write_str("enhancedMeasurementLEO-r17", "true");
    }
    if (ra_ch_access_r17_present) {
      j.write_str("ra-ChannelAccess-r17", "enabled");
    }
  }
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

void serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::set(types::options e)
{
  type_ = e;
}
void serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::set_dyn()
{
  set(types::dyn);
}
semi_static_ch_access_cfg_r16_s& serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::set_semi_static()
{
  set(types::semi_static);
  return c;
}
void serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::dyn:
      break;
    case types::semi_static:
      j.write_fieldname("semiStatic");
      c.to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::dyn:
      break;
    case types::semi_static:
      HANDLE_CODE(c.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::dyn:
      break;
    case types::semi_static:
      HANDLE_CODE(c.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(names, 2, value, "serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types");
}

const char* serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms1", "ms2", "ms3", "ms4", "ms5"};
  return convert_enum_idx(names, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_e_");
}
float serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_e_");
}
const char* serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "3", "4", "5"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_e_");
}

const char* serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_opts::to_string() const
{
  static const char* names[] = {"ms0dot125", "ms0dot25", "ms0dot5", "ms0dot75", "ms1", "ms1dot25"};
  return convert_enum_idx(names, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_e_");
}
float serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 0.75, 1.0, 1.25};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_e_");
}
const char* serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.125", "0.25", "0.5", "0.75", "1", "1.25"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_e_");
}

// AvailabilityCombination-r16 ::= SEQUENCE
SRSASN_CODE availability_combination_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, availability_combination_id_r16, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(pack_dyn_seq_of(bref, res_availability_r16, 1, 256, integer_packer<uint8_t>(0, 7)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE availability_combination_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(availability_combination_id_r16, bref, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(unpack_dyn_seq_of(res_availability_r16, bref, 1, 256, integer_packer<uint8_t>(0, 7)));

  return SRSASN_SUCCESS;
}
void availability_combination_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("availabilityCombinationId-r16", availability_combination_id_r16);
  j.start_array("resourceAvailability-r16");
  for (const auto& e1 : res_availability_r16) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// RB-SetGroup-r17 ::= SEQUENCE
SRSASN_CODE rb_set_group_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(res_availability_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(rb_sets_r17.size() > 0, 1));

  if (res_availability_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, res_availability_r17, 1, 256, integer_packer<uint8_t>(0, 7)));
  }
  if (rb_sets_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rb_sets_r17, 1, 8, integer_packer<uint8_t>(0, 7)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rb_set_group_r17_s::unpack(cbit_ref& bref)
{
  bool res_availability_r17_present;
  HANDLE_CODE(bref.unpack(res_availability_r17_present, 1));
  bool rb_sets_r17_present;
  HANDLE_CODE(bref.unpack(rb_sets_r17_present, 1));

  if (res_availability_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(res_availability_r17, bref, 1, 256, integer_packer<uint8_t>(0, 7)));
  }
  if (rb_sets_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rb_sets_r17, bref, 1, 8, integer_packer<uint8_t>(0, 7)));
  }

  return SRSASN_SUCCESS;
}
void rb_set_group_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (res_availability_r17.size() > 0) {
    j.start_array("resourceAvailability-r17");
    for (const auto& e1 : res_availability_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (rb_sets_r17.size() > 0) {
    j.start_array("rb-Sets-r17");
    for (const auto& e1 : rb_sets_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// AvailabilityCombinationRB-Groups-r17 ::= SEQUENCE
SRSASN_CODE availability_combination_rb_groups_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rb_set_groups_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(res_availability_r17.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, availability_combination_id_r17, (uint16_t)0u, (uint16_t)511u));
  if (rb_set_groups_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rb_set_groups_r17, 1, 8));
  }
  if (res_availability_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, res_availability_r17, 1, 256, integer_packer<uint8_t>(0, 7)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE availability_combination_rb_groups_r17_s::unpack(cbit_ref& bref)
{
  bool rb_set_groups_r17_present;
  HANDLE_CODE(bref.unpack(rb_set_groups_r17_present, 1));
  bool res_availability_r17_present;
  HANDLE_CODE(bref.unpack(res_availability_r17_present, 1));

  HANDLE_CODE(unpack_integer(availability_combination_id_r17, bref, (uint16_t)0u, (uint16_t)511u));
  if (rb_set_groups_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rb_set_groups_r17, bref, 1, 8));
  }
  if (res_availability_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(res_availability_r17, bref, 1, 256, integer_packer<uint8_t>(0, 7)));
  }

  return SRSASN_SUCCESS;
}
void availability_combination_rb_groups_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("availabilityCombinationId-r17", availability_combination_id_r17);
  if (rb_set_groups_r17.size() > 0) {
    j.start_array("rb-SetGroups-r17");
    for (const auto& e1 : rb_set_groups_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (res_availability_r17.size() > 0) {
    j.start_array("resourceAvailability-r17");
    for (const auto& e1 : res_availability_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// AvailabilityCombinationsPerCell-r16 ::= SEQUENCE
SRSASN_CODE availability_combinations_per_cell_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(position_in_dci_ai_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, availability_combinations_per_cell_idx_r16, (uint16_t)0u, (uint16_t)512u));
  HANDLE_CODE(iab_du_cell_id_r16.pack(bref));
  if (position_in_dci_ai_r16_present) {
    HANDLE_CODE(pack_integer(bref, position_in_dci_ai_r16, (uint8_t)0u, (uint8_t)127u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, availability_combinations_r16, 1, 512));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= availability_combinations_rb_groups_r17.is_present();
    group_flags[1] |= position_in_dci_ai_rb_groups_v1720_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(availability_combinations_rb_groups_r17.is_present(), 1));
      if (availability_combinations_rb_groups_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *availability_combinations_rb_groups_r17, 1, 512));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(position_in_dci_ai_rb_groups_v1720_present, 1));
      if (position_in_dci_ai_rb_groups_v1720_present) {
        HANDLE_CODE(pack_integer(bref, position_in_dci_ai_rb_groups_v1720, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE availability_combinations_per_cell_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(position_in_dci_ai_r16_present, 1));

  HANDLE_CODE(unpack_integer(availability_combinations_per_cell_idx_r16, bref, (uint16_t)0u, (uint16_t)512u));
  HANDLE_CODE(iab_du_cell_id_r16.unpack(bref));
  if (position_in_dci_ai_r16_present) {
    HANDLE_CODE(unpack_integer(position_in_dci_ai_r16, bref, (uint8_t)0u, (uint8_t)127u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(availability_combinations_r16, bref, 1, 512));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(availability_combinations_rb_groups_r17, bref);
      if (availability_combinations_rb_groups_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*availability_combinations_rb_groups_r17, bref, 1, 512));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(position_in_dci_ai_rb_groups_v1720_present, 1));
      if (position_in_dci_ai_rb_groups_v1720_present) {
        HANDLE_CODE(unpack_integer(position_in_dci_ai_rb_groups_v1720, bref, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void availability_combinations_per_cell_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("availabilityCombinationsPerCellIndex-r16", availability_combinations_per_cell_idx_r16);
  j.write_str("iab-DU-CellIdentity-r16", iab_du_cell_id_r16.to_string());
  if (position_in_dci_ai_r16_present) {
    j.write_int("positionInDCI-AI-r16", position_in_dci_ai_r16);
  }
  j.start_array("availabilityCombinations-r16");
  for (const auto& e1 : availability_combinations_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (availability_combinations_rb_groups_r17.is_present()) {
      j.start_array("availabilityCombinationsRB-Groups-r17");
      for (const auto& e1 : *availability_combinations_rb_groups_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (position_in_dci_ai_rb_groups_v1720_present) {
      j.write_int("positionInDCI-AI-RBGroups-v1720", position_in_dci_ai_rb_groups_v1720);
    }
  }
  j.end_obj();
}

// AvailabilityIndicator-r16 ::= SEQUENCE
SRSASN_CODE availability_ind_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(available_comb_to_add_mod_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(available_comb_to_release_list_r16.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, ai_rnti_r16, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, dci_payload_size_ai_r16, (uint8_t)1u, (uint8_t)128u));
  if (available_comb_to_add_mod_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, available_comb_to_add_mod_list_r16, 1, 512));
  }
  if (available_comb_to_release_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, available_comb_to_release_list_r16, 1, 512, integer_packer<uint16_t>(0, 512)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE availability_ind_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool available_comb_to_add_mod_list_r16_present;
  HANDLE_CODE(bref.unpack(available_comb_to_add_mod_list_r16_present, 1));
  bool available_comb_to_release_list_r16_present;
  HANDLE_CODE(bref.unpack(available_comb_to_release_list_r16_present, 1));

  HANDLE_CODE(unpack_integer(ai_rnti_r16, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(dci_payload_size_ai_r16, bref, (uint8_t)1u, (uint8_t)128u));
  if (available_comb_to_add_mod_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(available_comb_to_add_mod_list_r16, bref, 1, 512));
  }
  if (available_comb_to_release_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(available_comb_to_release_list_r16, bref, 1, 512, integer_packer<uint16_t>(0, 512)));
  }

  return SRSASN_SUCCESS;
}
void availability_ind_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ai-RNTI-r16", ai_rnti_r16);
  j.write_int("dci-PayloadSizeAI-r16", dci_payload_size_ai_r16);
  if (available_comb_to_add_mod_list_r16.size() > 0) {
    j.start_array("availableCombToAddModList-r16");
    for (const auto& e1 : available_comb_to_add_mod_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (available_comb_to_release_list_r16.size() > 0) {
    j.start_array("availableCombToReleaseList-r16");
    for (const auto& e1 : available_comb_to_release_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

// AvailableRB-SetsPerCell-r16 ::= SEQUENCE
SRSASN_CODE available_rb_sets_per_cell_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serving_cell_id_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, position_in_dci_r16, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE available_rb_sets_per_cell_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serving_cell_id_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(position_in_dci_r16, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void available_rb_sets_per_cell_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servingCellId-r16", serving_cell_id_r16);
  j.write_int("positionInDCI-r16", position_in_dci_r16);
  j.end_obj();
}

// NZP-CSI-RS-Pairing-r17 ::= SEQUENCE
SRSASN_CODE nzp_csi_rs_pairing_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, nzp_csi_rs_res_id1_r17, (uint8_t)1u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, nzp_csi_rs_res_id2_r17, (uint8_t)1u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nzp_csi_rs_pairing_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(nzp_csi_rs_res_id1_r17, bref, (uint8_t)1u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(nzp_csi_rs_res_id2_r17, bref, (uint8_t)1u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void nzp_csi_rs_pairing_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nzp-CSI-RS-ResourceId1-r17", nzp_csi_rs_res_id1_r17);
  j.write_int("nzp-CSI-RS-ResourceId2-r17", nzp_csi_rs_res_id2_r17);
  j.end_obj();
}

// CMRGroupingAndPairing-r17 ::= SEQUENCE
SRSASN_CODE cmr_grouping_and_pairing_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pair1_of_nzp_csi_rs_r17_present, 1));
  HANDLE_CODE(bref.pack(pair2_of_nzp_csi_rs_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, nrof_res_group1_r17, (uint8_t)1u, (uint8_t)7u));
  if (pair1_of_nzp_csi_rs_r17_present) {
    HANDLE_CODE(pair1_of_nzp_csi_rs_r17.pack(bref));
  }
  if (pair2_of_nzp_csi_rs_r17_present) {
    HANDLE_CODE(pair2_of_nzp_csi_rs_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cmr_grouping_and_pairing_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pair1_of_nzp_csi_rs_r17_present, 1));
  HANDLE_CODE(bref.unpack(pair2_of_nzp_csi_rs_r17_present, 1));

  HANDLE_CODE(unpack_integer(nrof_res_group1_r17, bref, (uint8_t)1u, (uint8_t)7u));
  if (pair1_of_nzp_csi_rs_r17_present) {
    HANDLE_CODE(pair1_of_nzp_csi_rs_r17.unpack(bref));
  }
  if (pair2_of_nzp_csi_rs_r17_present) {
    HANDLE_CODE(pair2_of_nzp_csi_rs_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cmr_grouping_and_pairing_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nrofResourcesGroup1-r17", nrof_res_group1_r17);
  if (pair1_of_nzp_csi_rs_r17_present) {
    j.write_fieldname("pair1OfNZP-CSI-RS-r17");
    pair1_of_nzp_csi_rs_r17.to_json(j);
  }
  if (pair2_of_nzp_csi_rs_r17_present) {
    j.write_fieldname("pair2OfNZP-CSI-RS-r17");
    pair2_of_nzp_csi_rs_r17.to_json(j);
  }
  j.end_obj();
}

// CO-DurationsPerCell-r16 ::= SEQUENCE
SRSASN_CODE co_durs_per_cell_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serving_cell_id_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, position_in_dci_r16, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(subcarrier_spacing_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, co_dur_list_r16, 1, 64, integer_packer<uint16_t>(0, 1120)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE co_durs_per_cell_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serving_cell_id_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(position_in_dci_r16, bref, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(subcarrier_spacing_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(co_dur_list_r16, bref, 1, 64, integer_packer<uint16_t>(0, 1120)));

  return SRSASN_SUCCESS;
}
void co_durs_per_cell_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servingCellId-r16", serving_cell_id_r16);
  j.write_int("positionInDCI-r16", position_in_dci_r16);
  j.write_str("subcarrierSpacing-r16", subcarrier_spacing_r16.to_string());
  j.start_array("co-DurationList-r16");
  for (const auto& e1 : co_dur_list_r16) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// CO-DurationsPerCell-r17 ::= SEQUENCE
SRSASN_CODE co_durs_per_cell_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serving_cell_id_r17, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, position_in_dci_r17, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(subcarrier_spacing_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, co_dur_list_r17, 1, 64, integer_packer<uint16_t>(0, 4480)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE co_durs_per_cell_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serving_cell_id_r17, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(position_in_dci_r17, bref, (uint8_t)0u, (uint8_t)127u));
  HANDLE_CODE(subcarrier_spacing_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(co_dur_list_r17, bref, 1, 64, integer_packer<uint16_t>(0, 4480)));

  return SRSASN_SUCCESS;
}
void co_durs_per_cell_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servingCellId-r17", serving_cell_id_r17);
  j.write_int("positionInDCI-r17", position_in_dci_r17);
  j.write_str("subcarrierSpacing-r17", subcarrier_spacing_r17.to_string());
  j.start_array("co-DurationList-r17");
  for (const auto& e1 : co_dur_list_r17) {
    j.write_int(e1);
  }
  j.end_array();
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= res_for_ch2_r17.is_present();
    group_flags[0] |= csi_ssb_res_set_ext_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(res_for_ch2_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_ssb_res_set_ext_present, 1));
      if (res_for_ch2_r17.is_present()) {
        HANDLE_CODE(res_for_ch2_r17->pack(bref));
      }
      if (csi_ssb_res_set_ext_present) {
        HANDLE_CODE(pack_integer(bref, csi_ssb_res_set_ext, (uint8_t)1u, (uint8_t)2u));
      }
    }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(res_for_ch2_r17, bref);
      HANDLE_CODE(bref.unpack(csi_ssb_res_set_ext_present, 1));
      if (res_for_ch2_r17.is_present()) {
        HANDLE_CODE(res_for_ch2_r17->unpack(bref));
      }
      if (csi_ssb_res_set_ext_present) {
        HANDLE_CODE(unpack_integer(csi_ssb_res_set_ext, bref, (uint8_t)1u, (uint8_t)2u));
      }
    }
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
  if (ext) {
    if (res_for_ch2_r17.is_present()) {
      j.write_fieldname("resourcesForChannel2-r17");
      res_for_ch2_r17->to_json(j);
    }
    if (csi_ssb_res_set_ext_present) {
      j.write_int("csi-SSB-ResourceSetExt", csi_ssb_res_set_ext);
    }
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

const char* csi_associated_report_cfg_info_s::res_for_ch_c_::types_opts::to_string() const
{
  static const char* names[] = {"nzp-CSI-RS", "csi-SSB-ResourceSet"};
  return convert_enum_idx(names, 2, value, "csi_associated_report_cfg_info_s::res_for_ch_c_::types");
}

void csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::destroy_()
{
  switch (type_) {
    case types::nzp_csi_rs2_r17:
      c.destroy<nzp_csi_rs2_r17_s_>();
      break;
    default:
      break;
  }
}
void csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nzp_csi_rs2_r17:
      c.init<nzp_csi_rs2_r17_s_>();
      break;
    case types::csi_ssb_res_set2_r17:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_");
  }
}
csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::res_for_ch2_r17_c_(
    const csi_associated_report_cfg_info_s::res_for_ch2_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nzp_csi_rs2_r17:
      c.init(other.c.get<nzp_csi_rs2_r17_s_>());
      break;
    case types::csi_ssb_res_set2_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_");
  }
}
csi_associated_report_cfg_info_s::res_for_ch2_r17_c_& csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::operator=(
    const csi_associated_report_cfg_info_s::res_for_ch2_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nzp_csi_rs2_r17:
      c.set(other.c.get<nzp_csi_rs2_r17_s_>());
      break;
    case types::csi_ssb_res_set2_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_");
  }

  return *this;
}
csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::nzp_csi_rs2_r17_s_&
csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::set_nzp_csi_rs2_r17()
{
  set(types::nzp_csi_rs2_r17);
  return c.get<nzp_csi_rs2_r17_s_>();
}
uint8_t& csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::set_csi_ssb_res_set2_r17()
{
  set(types::csi_ssb_res_set2_r17);
  return c.get<uint8_t>();
}
void csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nzp_csi_rs2_r17:
      j.write_fieldname("nzp-CSI-RS2-r17");
      j.start_obj();
      j.write_int("resourceSet2-r17", c.get<nzp_csi_rs2_r17_s_>().res_set2_r17);
      if (c.get<nzp_csi_rs2_r17_s_>().qcl_info2_r17.size() > 0) {
        j.start_array("qcl-info2-r17");
        for (const auto& e1 : c.get<nzp_csi_rs2_r17_s_>().qcl_info2_r17) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    case types::csi_ssb_res_set2_r17:
      j.write_int("csi-SSB-ResourceSet2-r17", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nzp_csi_rs2_r17:
      HANDLE_CODE(bref.pack(c.get<nzp_csi_rs2_r17_s_>().qcl_info2_r17.size() > 0, 1));
      HANDLE_CODE(pack_integer(bref, c.get<nzp_csi_rs2_r17_s_>().res_set2_r17, (uint8_t)1u, (uint8_t)16u));
      if (c.get<nzp_csi_rs2_r17_s_>().qcl_info2_r17.size() > 0) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, c.get<nzp_csi_rs2_r17_s_>().qcl_info2_r17, 1, 16, integer_packer<uint8_t>(0, 127)));
      }
      break;
    case types::csi_ssb_res_set2_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)2u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nzp_csi_rs2_r17:
      bool qcl_info2_r17_present;
      HANDLE_CODE(bref.unpack(qcl_info2_r17_present, 1));
      HANDLE_CODE(unpack_integer(c.get<nzp_csi_rs2_r17_s_>().res_set2_r17, bref, (uint8_t)1u, (uint8_t)16u));
      if (qcl_info2_r17_present) {
        HANDLE_CODE(
            unpack_dyn_seq_of(c.get<nzp_csi_rs2_r17_s_>().qcl_info2_r17, bref, 1, 16, integer_packer<uint8_t>(0, 127)));
      }
      break;
    case types::csi_ssb_res_set2_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)2u));
      break;
    default:
      log_invalid_choice_id(type_, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"nzp-CSI-RS2-r17", "csi-SSB-ResourceSet2-r17"};
  return convert_enum_idx(names, 2, value, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types");
}

// CSI-AperiodicTriggerState ::= SEQUENCE
SRSASN_CODE csi_aperiodic_trigger_state_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, associated_report_cfg_info_list, 1, 16));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ap_csi_mux_mode_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ap_csi_mux_mode_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_aperiodic_trigger_state_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(associated_report_cfg_info_list, bref, 1, 16));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ap_csi_mux_mode_r17_present, 1));
    }
  }
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
  if (ext) {
    if (ap_csi_mux_mode_r17_present) {
      j.write_str("ap-CSI-MultiplexingMode-r17", "enabled");
    }
  }
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

const char* csi_im_res_s::csi_im_res_elem_pattern_c_::types_opts::to_string() const
{
  static const char* names[] = {"pattern0", "pattern1"};
  return convert_enum_idx(names, 2, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::types");
}
uint8_t csi_im_res_s::csi_im_res_elem_pattern_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::types");
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

const char* csi_report_periodicity_and_offset_c::types_opts::to_string() const
{
  static const char* names[] = {
      "slots4", "slots5", "slots8", "slots10", "slots16", "slots20", "slots40", "slots80", "slots160", "slots320"};
  return convert_enum_idx(names, 10, value, "csi_report_periodicity_and_offset_c::types");
}
uint16_t csi_report_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 5, 8, 10, 16, 20, 40, 80, 160, 320};
  return map_enum_number(numbers, 10, value, "csi_report_periodicity_and_offset_c::types");
}

// CSI-SemiPersistentOnPUSCH-TriggerState ::= SEQUENCE
SRSASN_CODE csi_semi_persistent_on_pusch_trigger_state_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, associated_report_cfg_info, (uint8_t)0u, (uint8_t)47u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sp_csi_mux_mode_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sp_csi_mux_mode_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_semi_persistent_on_pusch_trigger_state_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(associated_report_cfg_info, bref, (uint8_t)0u, (uint8_t)47u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sp_csi_mux_mode_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void csi_semi_persistent_on_pusch_trigger_state_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("associatedReportConfigInfo", associated_report_cfg_info);
  if (ext) {
    if (sp_csi_mux_mode_r17_present) {
      j.write_str("sp-CSI-MultiplexingMode-r17", "enabled");
    }
  }
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

const char* codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::types_opts::to_string() const
{
  static const char* names[] = {"two-one-TypeI-SinglePanel-Restriction",
                                "two-two-TypeI-SinglePanel-Restriction",
                                "four-one-TypeI-SinglePanel-Restriction",
                                "three-two-TypeI-SinglePanel-Restriction",
                                "six-one-TypeI-SinglePanel-Restriction",
                                "four-two-TypeI-SinglePanel-Restriction",
                                "eight-one-TypeI-SinglePanel-Restriction",
                                "four-three-TypeI-SinglePanel-Restriction",
                                "six-two-TypeI-SinglePanel-Restriction",
                                "twelve-one-TypeI-SinglePanel-Restriction",
                                "four-four-TypeI-SinglePanel-Restriction",
                                "eight-two-TypeI-SinglePanel-Restriction",
                                "sixteen-one-TypeI-SinglePanel-Restriction"};
  return convert_enum_idx(names,
                          13,
                          value,
                          "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_"
                          "ports_c_::more_than_two_s_::n1_n2_c_::types");
}

const char* codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    types_opts::to_string() const
{
  static const char* names[] = {"two", "moreThanTwo"};
  return convert_enum_idx(
      names,
      2,
      value,
      "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types");
}
uint8_t codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(
      numbers,
      1,
      value,
      "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types");
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

const char*
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types_opts::to_string()
    const
{
  static const char* names[] = {"two-two-one-TypeI-MultiPanel-Restriction",
                                "two-four-one-TypeI-MultiPanel-Restriction",
                                "four-two-one-TypeI-MultiPanel-Restriction",
                                "two-two-two-TypeI-MultiPanel-Restriction",
                                "two-eight-one-TypeI-MultiPanel-Restriction",
                                "four-four-one-TypeI-MultiPanel-Restriction",
                                "two-four-two-TypeI-MultiPanel-Restriction",
                                "four-two-two-TypeI-MultiPanel-Restriction"};
  return convert_enum_idx(
      names,
      8,
      value,
      "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types");
}

const char* codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"typeI-SinglePanel", "typeI-MultiPanel"};
  return convert_enum_idx(names, 2, value, "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types");
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

const char* codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::
    types_opts::to_string() const
{
  static const char* names[] = {"two-one",
                                "two-two",
                                "four-one",
                                "three-two",
                                "six-one",
                                "four-two",
                                "eight-one",
                                "four-three",
                                "six-two",
                                "twelve-one",
                                "four-four",
                                "eight-two",
                                "sixteen-one"};
  return convert_enum_idx(
      names,
      13,
      value,
      "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::types");
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

const char* codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"typeII", "typeII-PortSelection"};
  return convert_enum_idx(names, 2, value, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types");
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

const char* codebook_cfg_s::codebook_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"type1", "type2"};
  return convert_enum_idx(names, 2, value, "codebook_cfg_s::codebook_type_c_::types");
}
uint8_t codebook_cfg_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "codebook_cfg_s::codebook_type_c_::types");
}

// CodebookConfig-r16 ::= SEQUENCE
SRSASN_CODE codebook_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(codebook_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(codebook_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void codebook_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("codebookType");
  codebook_type.to_json(j);
  j.end_obj();
}

void codebook_cfg_r16_s::codebook_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("type2");
  j.start_obj();
  j.write_fieldname("subType");
  c.sub_type.to_json(j);
  j.write_int("numberOfPMI-SubbandsPerCQI-Subband-r16", c.nof_pmi_subbands_per_cqi_subband_r16);
  j.write_int("paramCombination-r16", c.param_combination_r16);
  j.end_obj();
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r16_s::codebook_type_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.sub_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, c.nof_pmi_subbands_per_cqi_subband_r16, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(pack_integer(bref, c.param_combination_r16, (uint8_t)1u, (uint8_t)8u));
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r16_s::codebook_type_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.sub_type.unpack(bref));
  HANDLE_CODE(unpack_integer(c.nof_pmi_subbands_per_cqi_subband_r16, bref, (uint8_t)1u, (uint8_t)2u));
  HANDLE_CODE(unpack_integer(c.param_combination_r16, bref, (uint8_t)1u, (uint8_t)8u));
  return SRSASN_SUCCESS;
}

void codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::destroy_()
{
  switch (type_) {
    case types::type_ii_r16:
      c.destroy<type_ii_r16_s_>();
      break;
    case types::type_ii_port_sel_r16:
      c.destroy<type_ii_port_sel_r16_s_>();
      break;
    default:
      break;
  }
}
void codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type_ii_r16:
      c.init<type_ii_r16_s_>();
      break;
    case types::type_ii_port_sel_r16:
      c.init<type_ii_port_sel_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_");
  }
}
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::sub_type_c_(
    const codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type_ii_r16:
      c.init(other.c.get<type_ii_r16_s_>());
      break;
    case types::type_ii_port_sel_r16:
      c.init(other.c.get<type_ii_port_sel_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_");
  }
}
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_&
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::operator=(
    const codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type_ii_r16:
      c.set(other.c.get<type_ii_r16_s_>());
      break;
    case types::type_ii_port_sel_r16:
      c.set(other.c.get<type_ii_port_sel_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_");
  }

  return *this;
}
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_&
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii_r16()
{
  set(types::type_ii_r16);
  return c.get<type_ii_r16_s_>();
}
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_&
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii_port_sel_r16()
{
  set(types::type_ii_port_sel_r16);
  return c.get<type_ii_port_sel_r16_s_>();
}
void codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type_ii_r16:
      j.write_fieldname("typeII-r16");
      j.start_obj();
      j.write_fieldname("n1-n2-codebookSubsetRestriction-r16");
      c.get<type_ii_r16_s_>().n1_n2_codebook_subset_restrict_r16.to_json(j);
      j.write_str("typeII-RI-Restriction-r16", c.get<type_ii_r16_s_>().type_ii_ri_restrict_r16.to_string());
      j.end_obj();
      break;
    case types::type_ii_port_sel_r16:
      j.write_fieldname("typeII-PortSelection-r16");
      j.start_obj();
      j.write_str("portSelectionSamplingSize-r16",
                  c.get<type_ii_port_sel_r16_s_>().port_sel_sampling_size_r16.to_string());
      j.write_str("typeII-PortSelectionRI-Restriction-r16",
                  c.get<type_ii_port_sel_r16_s_>().type_ii_port_sel_ri_restrict_r16.to_string());
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type_ii_r16:
      HANDLE_CODE(c.get<type_ii_r16_s_>().n1_n2_codebook_subset_restrict_r16.pack(bref));
      HANDLE_CODE(c.get<type_ii_r16_s_>().type_ii_ri_restrict_r16.pack(bref));
      break;
    case types::type_ii_port_sel_r16:
      HANDLE_CODE(c.get<type_ii_port_sel_r16_s_>().port_sel_sampling_size_r16.pack(bref));
      HANDLE_CODE(c.get<type_ii_port_sel_r16_s_>().type_ii_port_sel_ri_restrict_r16.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type_ii_r16:
      HANDLE_CODE(c.get<type_ii_r16_s_>().n1_n2_codebook_subset_restrict_r16.unpack(bref));
      HANDLE_CODE(c.get<type_ii_r16_s_>().type_ii_ri_restrict_r16.unpack(bref));
      break;
    case types::type_ii_port_sel_r16:
      HANDLE_CODE(c.get<type_ii_port_sel_r16_s_>().port_sel_sampling_size_r16.unpack(bref));
      HANDLE_CODE(c.get<type_ii_port_sel_r16_s_>().type_ii_port_sel_ri_restrict_r16.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::destroy_()
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
void codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set(types::options e)
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
      log_invalid_choice_id(type_,
                            "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_"
                            "codebook_subset_restrict_r16_c_");
  }
}
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_codebook_subset_restrict_r16_c_::
    n1_n2_codebook_subset_restrict_r16_c_(const codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::
                                              type_ii_r16_s_::n1_n2_codebook_subset_restrict_r16_c_& other)
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
      log_invalid_choice_id(type_,
                            "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_"
                            "codebook_subset_restrict_r16_c_");
  }
}
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_codebook_subset_restrict_r16_c_&
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_codebook_subset_restrict_r16_c_::
operator=(const codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
              n1_n2_codebook_subset_restrict_r16_c_& other)
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
      log_invalid_choice_id(type_,
                            "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_"
                            "codebook_subset_restrict_r16_c_");
  }

  return *this;
}
fixed_bitstring<16>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_two_one()
{
  set(types::two_one);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<43>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_two_two()
{
  set(types::two_two);
  return c.get<fixed_bitstring<43>>();
}
fixed_bitstring<32>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_one()
{
  set(types::four_one);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<59>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_three_two()
{
  set(types::three_two);
  return c.get<fixed_bitstring<59>>();
}
fixed_bitstring<48>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_six_one()
{
  set(types::six_one);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<75>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_two()
{
  set(types::four_two);
  return c.get<fixed_bitstring<75>>();
}
fixed_bitstring<64>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_eight_one()
{
  set(types::eight_one);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<107>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_three()
{
  set(types::four_three);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<107>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_six_two()
{
  set(types::six_two);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<96>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_twelve_one()
{
  set(types::twelve_one);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<139>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_four()
{
  set(types::four_four);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<139>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_eight_two()
{
  set(types::eight_two);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<128>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_sixteen_one()
{
  set(types::sixteen_one);
  return c.get<fixed_bitstring<128>>();
}
void codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_,
                            "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_"
                            "codebook_subset_restrict_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_,
                            "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_"
                            "codebook_subset_restrict_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_,
                            "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_"
                            "codebook_subset_restrict_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"two-one",
                                "two-two",
                                "four-one",
                                "three-two",
                                "six-one",
                                "four-two",
                                "eight-one",
                                "four-three",
                                "six-two",
                                "twelve-one",
                                "four-four",
                                "eight-two",
                                "sixteen-one"};
  return convert_enum_idx(names,
                          13,
                          value,
                          "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_codebook_"
                          "subset_restrict_r16_c_::types");
}

const char* codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
    port_sel_sampling_size_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(names,
                          4,
                          value,
                          "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::port_"
                          "sel_sampling_size_r16_e_");
}
uint8_t codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
    port_sel_sampling_size_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers,
                         4,
                         value,
                         "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::port_"
                         "sel_sampling_size_r16_e_");
}

const char* codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"typeII-r16", "typeII-PortSelection-r16"};
  return convert_enum_idx(names, 2, value, "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types");
}

const char* codebook_cfg_r16_s::codebook_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"type2"};
  return convert_enum_idx(names, 1, value, "codebook_cfg_r16_s::codebook_type_c_::types");
}
uint8_t codebook_cfg_r16_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "codebook_cfg_r16_s::codebook_type_c_::types");
}

// CodebookConfig-r17 ::= SEQUENCE
SRSASN_CODE codebook_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(codebook_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(codebook_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void codebook_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("codebookType");
  codebook_type.to_json(j);
  j.end_obj();
}

void codebook_cfg_r17_s::codebook_type_c_::destroy_()
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
void codebook_cfg_r17_s::codebook_type_c_::set(types::options e)
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
      log_invalid_choice_id(type_, "codebook_cfg_r17_s::codebook_type_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::codebook_type_c_(const codebook_cfg_r17_s::codebook_type_c_& other)
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
      log_invalid_choice_id(type_, "codebook_cfg_r17_s::codebook_type_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_&
codebook_cfg_r17_s::codebook_type_c_::operator=(const codebook_cfg_r17_s::codebook_type_c_& other)
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
      log_invalid_choice_id(type_, "codebook_cfg_r17_s::codebook_type_c_");
  }

  return *this;
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_& codebook_cfg_r17_s::codebook_type_c_::set_type1()
{
  set(types::type1);
  return c.get<type1_s_>();
}
codebook_cfg_r17_s::codebook_type_c_::type2_s_& codebook_cfg_r17_s::codebook_type_c_::set_type2()
{
  set(types::type2);
  return c.get<type2_s_>();
}
void codebook_cfg_r17_s::codebook_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type1:
      j.write_fieldname("type1");
      j.start_obj();
      if (c.get<type1_s_>().type_i_single_panel_group1_r17_present) {
        j.write_fieldname("typeI-SinglePanel-Group1-r17");
        j.start_obj();
        j.write_fieldname("nrOfAntennaPorts");
        c.get<type1_s_>().type_i_single_panel_group1_r17.nr_of_ant_ports.to_json(j);
        j.end_obj();
      }
      if (c.get<type1_s_>().type_i_single_panel_group2_r17_present) {
        j.write_fieldname("typeI-SinglePanel-Group2-r17");
        j.start_obj();
        j.write_fieldname("nrOfAntennaPorts");
        c.get<type1_s_>().type_i_single_panel_group2_r17.nr_of_ant_ports.to_json(j);
        j.end_obj();
      }
      if (c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17_present) {
        j.write_str("typeI-SinglePanel-ri-RestrictionSTRP-r17",
                    c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17.to_string());
      }
      if (c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17_present) {
        j.write_str("typeI-SinglePanel-ri-RestrictionSDM-r17",
                    c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17.to_string());
      }
      j.end_obj();
      break;
    case types::type2:
      j.write_fieldname("type2");
      j.start_obj();
      j.write_fieldname("typeII-PortSelection-r17");
      j.start_obj();
      j.write_int("paramCombination-r17", c.get<type2_s_>().type_ii_port_sel_r17.param_combination_r17);
      if (c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17_present) {
        j.write_str("valueOfN-r17", c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17.to_string());
      }
      if (c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17_present) {
        j.write_int("numberOfPMI-SubbandsPerCQI-Subband-r17",
                    c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17);
      }
      j.write_str("typeII-PortSelectionRI-Restriction-r17",
                  c.get<type2_s_>().type_ii_port_sel_r17.type_ii_port_sel_ri_restrict_r17.to_string());
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r17_s::codebook_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type1:
      HANDLE_CODE(bref.pack(c.get<type1_s_>().type_i_single_panel_group1_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type1_s_>().type_i_single_panel_group2_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17_present, 1));
      if (c.get<type1_s_>().type_i_single_panel_group1_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_group1_r17.nr_of_ant_ports.pack(bref));
      }
      if (c.get<type1_s_>().type_i_single_panel_group2_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_group2_r17.nr_of_ant_ports.pack(bref));
      }
      if (c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17.pack(bref));
      }
      if (c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17.pack(bref));
      }
      break;
    case types::type2:
      HANDLE_CODE(bref.pack(c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17_present, 1));
      HANDLE_CODE(
          pack_integer(bref, c.get<type2_s_>().type_ii_port_sel_r17.param_combination_r17, (uint8_t)1u, (uint8_t)8u));
      if (c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17_present) {
        HANDLE_CODE(c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17.pack(bref));
      }
      if (c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17_present) {
        HANDLE_CODE(pack_integer(bref,
                                 c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17,
                                 (uint8_t)1u,
                                 (uint8_t)2u));
      }
      HANDLE_CODE(c.get<type2_s_>().type_ii_port_sel_r17.type_ii_port_sel_ri_restrict_r17.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r17_s::codebook_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type1:
      HANDLE_CODE(bref.unpack(c.get<type1_s_>().type_i_single_panel_group1_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type1_s_>().type_i_single_panel_group2_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17_present, 1));
      if (c.get<type1_s_>().type_i_single_panel_group1_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_group1_r17.nr_of_ant_ports.unpack(bref));
      }
      if (c.get<type1_s_>().type_i_single_panel_group2_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_group2_r17.nr_of_ant_ports.unpack(bref));
      }
      if (c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_ri_restrict_strp_r17.unpack(bref));
      }
      if (c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17_present) {
        HANDLE_CODE(c.get<type1_s_>().type_i_single_panel_ri_restrict_sdm_r17.unpack(bref));
      }
      break;
    case types::type2:
      HANDLE_CODE(bref.unpack(c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17_present, 1));
      HANDLE_CODE(
          unpack_integer(c.get<type2_s_>().type_ii_port_sel_r17.param_combination_r17, bref, (uint8_t)1u, (uint8_t)8u));
      if (c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17_present) {
        HANDLE_CODE(c.get<type2_s_>().type_ii_port_sel_r17.value_of_n_r17.unpack(bref));
      }
      if (c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17_present) {
        HANDLE_CODE(unpack_integer(c.get<type2_s_>().type_ii_port_sel_r17.nof_pmi_subbands_per_cqi_subband_r17,
                                   bref,
                                   (uint8_t)1u,
                                   (uint8_t)2u));
      }
      HANDLE_CODE(c.get<type2_s_>().type_ii_port_sel_r17.type_ii_port_sel_ri_restrict_r17.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "codebook_cfg_r17_s::codebook_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::destroy_()
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
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::set(
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
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    nr_of_ant_ports_c_(
        const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_&
            other)
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
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::operator=(
    const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_& other)
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
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_");
  }

  return *this;
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::set_two()
{
  set(types::two);
  return c.get<two_s_>();
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::more_than_two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    set_more_than_two()
{
  set(types::more_than_two);
  return c.get<more_than_two_s_>();
}
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two:
      j.write_fieldname("two");
      j.start_obj();
      j.write_str("twoTX-CodebookSubsetRestriction1-r17",
                  c.get<two_s_>().two_tx_codebook_subset_restrict1_r17.to_string());
      j.end_obj();
      break;
    case types::more_than_two:
      j.write_fieldname("moreThanTwo");
      j.start_obj();
      j.write_fieldname("n1-n2");
      c.get<more_than_two_s_>().n1_n2.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::pack(
    bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two:
      HANDLE_CODE(c.get<two_s_>().two_tx_codebook_subset_restrict1_r17.pack(bref));
      break;
    case types::more_than_two:
      HANDLE_CODE(c.get<more_than_two_s_>().n1_n2.pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::unpack(
    cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two:
      HANDLE_CODE(c.get<two_s_>().two_tx_codebook_subset_restrict1_r17.unpack(bref));
      break;
    case types::more_than_two:
      HANDLE_CODE(c.get<more_than_two_s_>().n1_n2.unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::destroy_()
{
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::two_two_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<64>>();
      break;
    case types::four_one_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<16>>();
      break;
    case types::three_two_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<96>>();
      break;
    case types::six_one_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<24>>();
      break;
    case types::four_two_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<128>>();
      break;
    case types::eight_one_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<32>>();
      break;
    case types::four_three_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<192>>();
      break;
    case types::six_two_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<192>>();
      break;
    case types::twelve_one_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<48>>();
      break;
    case types::four_four_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<256>>();
      break;
    case types::eight_two_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<256>>();
      break;
    case types::sixteen_one_type_i_single_panel_restrict1_r17:
      c.destroy<fixed_bitstring<64>>();
      break;
    default:
      break;
  }
}
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<8>>();
      break;
    case types::two_two_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<64>>();
      break;
    case types::four_one_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<16>>();
      break;
    case types::three_two_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<96>>();
      break;
    case types::six_one_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<24>>();
      break;
    case types::four_two_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<128>>();
      break;
    case types::eight_one_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<32>>();
      break;
    case types::four_three_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<192>>();
      break;
    case types::six_two_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<192>>();
      break;
    case types::twelve_one_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<48>>();
      break;
    case types::four_four_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<256>>();
      break;
    case types::eight_two_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<256>>();
      break;
    case types::sixteen_one_type_i_single_panel_restrict1_r17:
      c.init<fixed_bitstring<64>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::n1_n2_c_(
        const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_one_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<16>>());
      break;
    case types::three_two_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<96>>());
      break;
    case types::six_one_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<24>>());
      break;
    case types::four_two_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<128>>());
      break;
    case types::eight_one_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_three_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<192>>());
      break;
    case types::six_two_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<192>>());
      break;
    case types::twelve_one_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_four_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<256>>());
      break;
    case types::eight_two_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<256>>());
      break;
    case types::sixteen_one_type_i_single_panel_restrict1_r17:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_&
    codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
        more_than_two_s_::n1_n2_c_::operator=(
            const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
                nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_one_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<16>>());
      break;
    case types::three_two_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<96>>());
      break;
    case types::six_one_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<24>>());
      break;
    case types::four_two_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<128>>());
      break;
    case types::eight_one_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_three_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<192>>());
      break;
    case types::six_two_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<192>>());
      break;
    case types::twelve_one_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_four_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<256>>());
      break;
    case types::eight_two_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<256>>());
      break;
    case types::sixteen_one_type_i_single_panel_restrict1_r17:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }

  return *this;
}
fixed_bitstring<8>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_one_type_i_single_panel_restrict1_r17()
{
  set(types::two_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_two_type_i_single_panel_restrict1_r17()
{
  set(types::two_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<16>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_one_type_i_single_panel_restrict1_r17()
{
  set(types::four_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<96>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_three_two_type_i_single_panel_restrict1_r17()
{
  set(types::three_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<24>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_one_type_i_single_panel_restrict1_r17()
{
  set(types::six_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<24>>();
}
fixed_bitstring<128>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_two_type_i_single_panel_restrict1_r17()
{
  set(types::four_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<32>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_one_type_i_single_panel_restrict1_r17()
{
  set(types::eight_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_three_type_i_single_panel_restrict1_r17()
{
  set(types::four_three_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_two_type_i_single_panel_restrict1_r17()
{
  set(types::six_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<48>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_twelve_one_type_i_single_panel_restrict1_r17()
{
  set(types::twelve_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_four_type_i_single_panel_restrict1_r17()
{
  set(types::four_four_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_two_type_i_single_panel_restrict1_r17()
{
  set(types::eight_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_sixteen_one_type_i_single_panel_restrict1_r17()
{
  set(types::sixteen_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<64>>();
}
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict1_r17:
      j.write_str("two-one-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::two_two_type_i_single_panel_restrict1_r17:
      j.write_str("two-two-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<64>>().to_string());
      break;
    case types::four_one_type_i_single_panel_restrict1_r17:
      j.write_str("four-one-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<16>>().to_string());
      break;
    case types::three_two_type_i_single_panel_restrict1_r17:
      j.write_str("three-two-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<96>>().to_string());
      break;
    case types::six_one_type_i_single_panel_restrict1_r17:
      j.write_str("six-one-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<24>>().to_string());
      break;
    case types::four_two_type_i_single_panel_restrict1_r17:
      j.write_str("four-two-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<128>>().to_string());
      break;
    case types::eight_one_type_i_single_panel_restrict1_r17:
      j.write_str("eight-one-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<32>>().to_string());
      break;
    case types::four_three_type_i_single_panel_restrict1_r17:
      j.write_str("four-three-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<192>>().to_string());
      break;
    case types::six_two_type_i_single_panel_restrict1_r17:
      j.write_str("six-two-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<192>>().to_string());
      break;
    case types::twelve_one_type_i_single_panel_restrict1_r17:
      j.write_str("twelve-one-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<48>>().to_string());
      break;
    case types::four_four_type_i_single_panel_restrict1_r17:
      j.write_str("four-four-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<256>>().to_string());
      break;
    case types::eight_two_type_i_single_panel_restrict1_r17:
      j.write_str("eight-two-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<256>>().to_string());
      break;
    case types::sixteen_one_type_i_single_panel_restrict1_r17:
      j.write_str("sixteen-one-TypeI-SinglePanel-Restriction1-r17", c.get<fixed_bitstring<64>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::two_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    case types::four_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().pack(bref));
      break;
    case types::three_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().pack(bref));
      break;
    case types::six_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().pack(bref));
      break;
    case types::four_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().pack(bref));
      break;
    case types::eight_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().pack(bref));
      break;
    case types::four_three_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().pack(bref));
      break;
    case types::six_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().pack(bref));
      break;
    case types::twelve_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().pack(bref));
      break;
    case types::four_four_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().pack(bref));
      break;
    case types::eight_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().pack(bref));
      break;
    case types::sixteen_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::two_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    case types::four_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().unpack(bref));
      break;
    case types::three_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().unpack(bref));
      break;
    case types::six_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().unpack(bref));
      break;
    case types::four_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().unpack(bref));
      break;
    case types::eight_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().unpack(bref));
      break;
    case types::four_three_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().unpack(bref));
      break;
    case types::six_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().unpack(bref));
      break;
    case types::twelve_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().unpack(bref));
      break;
    case types::four_four_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().unpack(bref));
      break;
    case types::eight_two_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().unpack(bref));
      break;
    case types::sixteen_one_type_i_single_panel_restrict1_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::types_opts::to_string() const
{
  static const char* names[] = {"two-one-TypeI-SinglePanel-Restriction1-r17",
                                "two-two-TypeI-SinglePanel-Restriction1-r17",
                                "four-one-TypeI-SinglePanel-Restriction1-r17",
                                "three-two-TypeI-SinglePanel-Restriction1-r17",
                                "six-one-TypeI-SinglePanel-Restriction1-r17",
                                "four-two-TypeI-SinglePanel-Restriction1-r17",
                                "eight-one-TypeI-SinglePanel-Restriction1-r17",
                                "four-three-TypeI-SinglePanel-Restriction1-r17",
                                "six-two-TypeI-SinglePanel-Restriction1-r17",
                                "twelve-one-TypeI-SinglePanel-Restriction1-r17",
                                "four-four-TypeI-SinglePanel-Restriction1-r17",
                                "eight-two-TypeI-SinglePanel-Restriction1-r17",
                                "sixteen-one-TypeI-SinglePanel-Restriction1-r17"};
  return convert_enum_idx(names,
                          13,
                          value,
                          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_"
                          "ant_ports_c_::more_than_two_s_::n1_n2_c_::types");
}

const char* codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    types_opts::to_string() const
{
  static const char* names[] = {"two", "moreThanTwo"};
  return convert_enum_idx(
      names,
      2,
      value,
      "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::types");
}
uint8_t codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(
      numbers,
      1,
      value,
      "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::types");
}

void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::destroy_()
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
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::set(
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
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    nr_of_ant_ports_c_(
        const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_&
            other)
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
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::operator=(
    const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_& other)
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
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_");
  }

  return *this;
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::set_two()
{
  set(types::two);
  return c.get<two_s_>();
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::more_than_two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    set_more_than_two()
{
  set(types::more_than_two);
  return c.get<more_than_two_s_>();
}
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::to_json(
    json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two:
      j.write_fieldname("two");
      j.start_obj();
      j.write_str("twoTX-CodebookSubsetRestriction2-r17",
                  c.get<two_s_>().two_tx_codebook_subset_restrict2_r17.to_string());
      j.end_obj();
      break;
    case types::more_than_two:
      j.write_fieldname("moreThanTwo");
      j.start_obj();
      j.write_fieldname("n1-n2");
      c.get<more_than_two_s_>().n1_n2.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::pack(
    bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two:
      HANDLE_CODE(c.get<two_s_>().two_tx_codebook_subset_restrict2_r17.pack(bref));
      break;
    case types::more_than_two:
      HANDLE_CODE(c.get<more_than_two_s_>().n1_n2.pack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::unpack(
    cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two:
      HANDLE_CODE(c.get<two_s_>().two_tx_codebook_subset_restrict2_r17.unpack(bref));
      break;
    case types::more_than_two:
      HANDLE_CODE(c.get<more_than_two_s_>().n1_n2.unpack(bref));
      break;
    default:
      log_invalid_choice_id(
          type_,
          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::destroy_()
{
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::two_two_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<64>>();
      break;
    case types::four_one_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<16>>();
      break;
    case types::three_two_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<96>>();
      break;
    case types::six_one_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<24>>();
      break;
    case types::four_two_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<128>>();
      break;
    case types::eight_one_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<32>>();
      break;
    case types::four_three_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<192>>();
      break;
    case types::six_two_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<192>>();
      break;
    case types::twelve_one_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<48>>();
      break;
    case types::four_four_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<256>>();
      break;
    case types::eight_two_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<256>>();
      break;
    case types::sixteen_one_type_i_single_panel_restrict2_r17:
      c.destroy<fixed_bitstring<64>>();
      break;
    default:
      break;
  }
}
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<8>>();
      break;
    case types::two_two_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<64>>();
      break;
    case types::four_one_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<16>>();
      break;
    case types::three_two_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<96>>();
      break;
    case types::six_one_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<24>>();
      break;
    case types::four_two_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<128>>();
      break;
    case types::eight_one_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<32>>();
      break;
    case types::four_three_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<192>>();
      break;
    case types::six_two_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<192>>();
      break;
    case types::twelve_one_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<48>>();
      break;
    case types::four_four_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<256>>();
      break;
    case types::eight_two_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<256>>();
      break;
    case types::sixteen_one_type_i_single_panel_restrict2_r17:
      c.init<fixed_bitstring<64>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::n1_n2_c_(
        const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_one_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<16>>());
      break;
    case types::three_two_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<96>>());
      break;
    case types::six_one_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<24>>());
      break;
    case types::four_two_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<128>>());
      break;
    case types::eight_one_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_three_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<192>>());
      break;
    case types::six_two_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<192>>());
      break;
    case types::twelve_one_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_four_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<256>>());
      break;
    case types::eight_two_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<256>>());
      break;
    case types::sixteen_one_type_i_single_panel_restrict2_r17:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_&
    codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
        more_than_two_s_::n1_n2_c_::operator=(
            const codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
                nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::two_two_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::four_one_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<16>>());
      break;
    case types::three_two_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<96>>());
      break;
    case types::six_one_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<24>>());
      break;
    case types::four_two_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<128>>());
      break;
    case types::eight_one_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<32>>());
      break;
    case types::four_three_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<192>>());
      break;
    case types::six_two_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<192>>());
      break;
    case types::twelve_one_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<48>>());
      break;
    case types::four_four_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<256>>());
      break;
    case types::eight_two_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<256>>());
      break;
    case types::sixteen_one_type_i_single_panel_restrict2_r17:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }

  return *this;
}
fixed_bitstring<8>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_one_type_i_single_panel_restrict2_r17()
{
  set(types::two_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_two_type_i_single_panel_restrict2_r17()
{
  set(types::two_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<16>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_one_type_i_single_panel_restrict2_r17()
{
  set(types::four_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<96>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_three_two_type_i_single_panel_restrict2_r17()
{
  set(types::three_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<24>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_one_type_i_single_panel_restrict2_r17()
{
  set(types::six_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<24>>();
}
fixed_bitstring<128>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_two_type_i_single_panel_restrict2_r17()
{
  set(types::four_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<32>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_one_type_i_single_panel_restrict2_r17()
{
  set(types::eight_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_three_type_i_single_panel_restrict2_r17()
{
  set(types::four_three_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_two_type_i_single_panel_restrict2_r17()
{
  set(types::six_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<48>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_twelve_one_type_i_single_panel_restrict2_r17()
{
  set(types::twelve_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_four_type_i_single_panel_restrict2_r17()
{
  set(types::four_four_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_two_type_i_single_panel_restrict2_r17()
{
  set(types::eight_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_sixteen_one_type_i_single_panel_restrict2_r17()
{
  set(types::sixteen_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<64>>();
}
void codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict2_r17:
      j.write_str("two-one-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::two_two_type_i_single_panel_restrict2_r17:
      j.write_str("two-two-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<64>>().to_string());
      break;
    case types::four_one_type_i_single_panel_restrict2_r17:
      j.write_str("four-one-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<16>>().to_string());
      break;
    case types::three_two_type_i_single_panel_restrict2_r17:
      j.write_str("three-two-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<96>>().to_string());
      break;
    case types::six_one_type_i_single_panel_restrict2_r17:
      j.write_str("six-one-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<24>>().to_string());
      break;
    case types::four_two_type_i_single_panel_restrict2_r17:
      j.write_str("four-two-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<128>>().to_string());
      break;
    case types::eight_one_type_i_single_panel_restrict2_r17:
      j.write_str("eight-one-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<32>>().to_string());
      break;
    case types::four_three_type_i_single_panel_restrict2_r17:
      j.write_str("four-three-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<192>>().to_string());
      break;
    case types::six_two_type_i_single_panel_restrict2_r17:
      j.write_str("six-two-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<192>>().to_string());
      break;
    case types::twelve_one_type_i_single_panel_restrict2_r17:
      j.write_str("twelve-one-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<48>>().to_string());
      break;
    case types::four_four_type_i_single_panel_restrict2_r17:
      j.write_str("four-four-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<256>>().to_string());
      break;
    case types::eight_two_type_i_single_panel_restrict2_r17:
      j.write_str("eight-two-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<256>>().to_string());
      break;
    case types::sixteen_one_type_i_single_panel_restrict2_r17:
      j.write_str("sixteen-one-TypeI-SinglePanel-Restriction2-r17", c.get<fixed_bitstring<64>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
  }
  j.end_obj();
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::two_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    case types::four_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().pack(bref));
      break;
    case types::three_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().pack(bref));
      break;
    case types::six_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().pack(bref));
      break;
    case types::four_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().pack(bref));
      break;
    case types::eight_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().pack(bref));
      break;
    case types::four_three_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().pack(bref));
      break;
    case types::six_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().pack(bref));
      break;
    case types::twelve_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().pack(bref));
      break;
    case types::four_four_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().pack(bref));
      break;
    case types::eight_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().pack(bref));
      break;
    case types::sixteen_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::two_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::two_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    case types::four_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<16>>().unpack(bref));
      break;
    case types::three_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<96>>().unpack(bref));
      break;
    case types::six_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<24>>().unpack(bref));
      break;
    case types::four_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<128>>().unpack(bref));
      break;
    case types::eight_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<32>>().unpack(bref));
      break;
    case types::four_three_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().unpack(bref));
      break;
    case types::six_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<192>>().unpack(bref));
      break;
    case types::twelve_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().unpack(bref));
      break;
    case types::four_four_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().unpack(bref));
      break;
    case types::eight_two_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<256>>().unpack(bref));
      break;
    case types::sixteen_one_type_i_single_panel_restrict2_r17:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_,
                            "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_"
                            "ant_ports_c_::more_than_two_s_::n1_n2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    more_than_two_s_::n1_n2_c_::types_opts::to_string() const
{
  static const char* names[] = {"two-one-TypeI-SinglePanel-Restriction2-r17",
                                "two-two-TypeI-SinglePanel-Restriction2-r17",
                                "four-one-TypeI-SinglePanel-Restriction2-r17",
                                "three-two-TypeI-SinglePanel-Restriction2-r17",
                                "six-one-TypeI-SinglePanel-Restriction2-r17",
                                "four-two-TypeI-SinglePanel-Restriction2-r17",
                                "eight-one-TypeI-SinglePanel-Restriction2-r17",
                                "four-three-TypeI-SinglePanel-Restriction2-r17",
                                "six-two-TypeI-SinglePanel-Restriction2-r17",
                                "twelve-one-TypeI-SinglePanel-Restriction2-r17",
                                "four-four-TypeI-SinglePanel-Restriction2-r17",
                                "eight-two-TypeI-SinglePanel-Restriction2-r17",
                                "sixteen-one-TypeI-SinglePanel-Restriction2-r17"};
  return convert_enum_idx(names,
                          13,
                          value,
                          "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_"
                          "ant_ports_c_::more_than_two_s_::n1_n2_c_::types");
}

const char* codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    types_opts::to_string() const
{
  static const char* names[] = {"two", "moreThanTwo"};
  return convert_enum_idx(
      names,
      2,
      value,
      "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::types");
}
uint8_t codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(
      numbers,
      1,
      value,
      "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::types");
}

const char*
codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(
      names, 2, value, "codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_e_");
}
uint8_t codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(
      numbers, 2, value, "codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_e_");
}

const char* codebook_cfg_r17_s::codebook_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"type1", "type2"};
  return convert_enum_idx(names, 2, value, "codebook_cfg_r17_s::codebook_type_c_::types");
}
uint8_t codebook_cfg_r17_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "codebook_cfg_r17_s::codebook_type_c_::types");
}

// CodebookConfig-v1730 ::= SEQUENCE
SRSASN_CODE codebook_cfg_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(codebook_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(codebook_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void codebook_cfg_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("codebookType");
  codebook_type.to_json(j);
  j.end_obj();
}

void codebook_cfg_v1730_s::codebook_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("type1");
  j.start_obj();
  if (c.codebook_mode_present) {
    j.write_int("codebookMode", c.codebook_mode);
  }
  j.end_obj();
  j.end_obj();
}
SRSASN_CODE codebook_cfg_v1730_s::codebook_type_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(c.codebook_mode_present, 1));
  if (c.codebook_mode_present) {
    HANDLE_CODE(pack_integer(bref, c.codebook_mode, (uint8_t)1u, (uint8_t)2u));
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE codebook_cfg_v1730_s::codebook_type_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(c.codebook_mode_present, 1));
  if (c.codebook_mode_present) {
    HANDLE_CODE(unpack_integer(c.codebook_mode, bref, (uint8_t)1u, (uint8_t)2u));
  }
  return SRSASN_SUCCESS;
}

const char* codebook_cfg_v1730_s::codebook_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"type1"};
  return convert_enum_idx(names, 1, value, "codebook_cfg_v1730_s::codebook_type_c_::types");
}
uint8_t codebook_cfg_v1730_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "codebook_cfg_v1730_s::codebook_type_c_::types");
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
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx8_s_>().rank2_8, c.get<port_idx8_s_>().rank2_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank3_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx8_s_>().rank3_8, c.get<port_idx8_s_>().rank3_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank4_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx8_s_>().rank4_8, c.get<port_idx8_s_>().rank4_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank5_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx8_s_>().rank5_8, c.get<port_idx8_s_>().rank5_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank6_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx8_s_>().rank6_8, c.get<port_idx8_s_>().rank6_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank7_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx8_s_>().rank7_8, c.get<port_idx8_s_>().rank7_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank8_8_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx8_s_>().rank8_8, c.get<port_idx8_s_>().rank8_8.size(), integer_packer<uint8_t>(0, 7)));
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
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx4_s_>().rank2_4, c.get<port_idx4_s_>().rank2_4.size(), integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank3_4_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx4_s_>().rank3_4, c.get<port_idx4_s_>().rank3_4.size(), integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank4_4_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx4_s_>().rank4_4, c.get<port_idx4_s_>().rank4_4.size(), integer_packer<uint8_t>(0, 3)));
      }
      break;
    case types::port_idx2:
      HANDLE_CODE(bref.pack(c.get<port_idx2_s_>().rank1_2_present, 1));
      HANDLE_CODE(bref.pack(c.get<port_idx2_s_>().rank2_2_present, 1));
      if (c.get<port_idx2_s_>().rank1_2_present) {
        HANDLE_CODE(pack_integer(bref, c.get<port_idx2_s_>().rank1_2, (uint8_t)0u, (uint8_t)1u));
      }
      if (c.get<port_idx2_s_>().rank2_2_present) {
        HANDLE_CODE(pack_fixed_seq_of(
            bref, c.get<port_idx2_s_>().rank2_2, c.get<port_idx2_s_>().rank2_2.size(), integer_packer<uint8_t>(0, 1)));
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
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx8_s_>().rank2_8, bref, c.get<port_idx8_s_>().rank2_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank3_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx8_s_>().rank3_8, bref, c.get<port_idx8_s_>().rank3_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank4_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx8_s_>().rank4_8, bref, c.get<port_idx8_s_>().rank4_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank5_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx8_s_>().rank5_8, bref, c.get<port_idx8_s_>().rank5_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank6_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx8_s_>().rank6_8, bref, c.get<port_idx8_s_>().rank6_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank7_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx8_s_>().rank7_8, bref, c.get<port_idx8_s_>().rank7_8.size(), integer_packer<uint8_t>(0, 7)));
      }
      if (c.get<port_idx8_s_>().rank8_8_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx8_s_>().rank8_8, bref, c.get<port_idx8_s_>().rank8_8.size(), integer_packer<uint8_t>(0, 7)));
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
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx4_s_>().rank2_4, bref, c.get<port_idx4_s_>().rank2_4.size(), integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank3_4_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx4_s_>().rank3_4, bref, c.get<port_idx4_s_>().rank3_4.size(), integer_packer<uint8_t>(0, 3)));
      }
      if (c.get<port_idx4_s_>().rank4_4_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx4_s_>().rank4_4, bref, c.get<port_idx4_s_>().rank4_4.size(), integer_packer<uint8_t>(0, 3)));
      }
      break;
    case types::port_idx2:
      HANDLE_CODE(bref.unpack(c.get<port_idx2_s_>().rank1_2_present, 1));
      HANDLE_CODE(bref.unpack(c.get<port_idx2_s_>().rank2_2_present, 1));
      if (c.get<port_idx2_s_>().rank1_2_present) {
        HANDLE_CODE(unpack_integer(c.get<port_idx2_s_>().rank1_2, bref, (uint8_t)0u, (uint8_t)1u));
      }
      if (c.get<port_idx2_s_>().rank2_2_present) {
        HANDLE_CODE(unpack_fixed_seq_of(
            c.get<port_idx2_s_>().rank2_2, bref, c.get<port_idx2_s_>().rank2_2.size(), integer_packer<uint8_t>(0, 1)));
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

const char* port_idx_for8_ranks_c::types_opts::to_string() const
{
  static const char* names[] = {"portIndex8", "portIndex4", "portIndex2", "portIndex1"};
  return convert_enum_idx(names, 4, value, "port_idx_for8_ranks_c::types");
}
uint8_t port_idx_for8_ranks_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 4, 2, 1};
  return map_enum_number(numbers, 4, value, "port_idx_for8_ranks_c::types");
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
    group_flags[1] |= semi_persistent_on_pusch_v1610.is_present();
    group_flags[1] |= aperiodic_v1610.is_present();
    group_flags[1] |= report_quant_r16.is_present();
    group_flags[1] |= codebook_cfg_r16.is_present();
    group_flags[2] |= cqi_bits_per_subband_r17_present;
    group_flags[2] |= group_based_beam_report_v1710.is_present();
    group_flags[2] |= codebook_cfg_r17.is_present();
    group_flags[2] |= shared_cmr_r17_present;
    group_flags[2] |= csi_report_mode_r17_present;
    group_flags[2] |= nof_single_trp_csi_mode1_r17_present;
    group_flags[2] |= report_quant_r17.is_present();
    group_flags[3] |= semi_persistent_on_pusch_v1720.is_present();
    group_flags[3] |= aperiodic_v1720.is_present();
    group_flags[4] |= codebook_cfg_v1730.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1530.is_present(), 1));
      if (semi_persistent_on_pusch_v1530.is_present()) {
        HANDLE_CODE(semi_persistent_on_pusch_v1530->report_slot_cfg_v1530.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(aperiodic_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(report_quant_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(codebook_cfg_r16.is_present(), 1));
      if (semi_persistent_on_pusch_v1610.is_present()) {
        HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_2_r16.size() > 0, 1));
        HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_1_r16.size() > 0, 1));
        if (semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_2_r16.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(bref,
                                      semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_2_r16,
                                      1,
                                      64,
                                      integer_packer<uint8_t>(0, 32)));
        }
        if (semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_1_r16.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(bref,
                                      semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_1_r16,
                                      1,
                                      64,
                                      integer_packer<uint8_t>(0, 32)));
        }
      }
      if (aperiodic_v1610.is_present()) {
        HANDLE_CODE(bref.pack(aperiodic_v1610->report_slot_offset_list_dci_0_2_r16.size() > 0, 1));
        HANDLE_CODE(bref.pack(aperiodic_v1610->report_slot_offset_list_dci_0_1_r16.size() > 0, 1));
        if (aperiodic_v1610->report_slot_offset_list_dci_0_2_r16.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, aperiodic_v1610->report_slot_offset_list_dci_0_2_r16, 1, 64, integer_packer<uint8_t>(0, 32)));
        }
        if (aperiodic_v1610->report_slot_offset_list_dci_0_1_r16.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, aperiodic_v1610->report_slot_offset_list_dci_0_1_r16, 1, 64, integer_packer<uint8_t>(0, 32)));
        }
      }
      if (report_quant_r16.is_present()) {
        HANDLE_CODE(report_quant_r16->pack(bref));
      }
      if (codebook_cfg_r16.is_present()) {
        HANDLE_CODE(codebook_cfg_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cqi_bits_per_subband_r17_present, 1));
      HANDLE_CODE(bref.pack(group_based_beam_report_v1710.is_present(), 1));
      HANDLE_CODE(bref.pack(codebook_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(shared_cmr_r17_present, 1));
      HANDLE_CODE(bref.pack(csi_report_mode_r17_present, 1));
      HANDLE_CODE(bref.pack(nof_single_trp_csi_mode1_r17_present, 1));
      HANDLE_CODE(bref.pack(report_quant_r17.is_present(), 1));
      if (group_based_beam_report_v1710.is_present()) {
        HANDLE_CODE(group_based_beam_report_v1710->nrof_reported_groups_r17.pack(bref));
      }
      if (codebook_cfg_r17.is_present()) {
        HANDLE_CODE(codebook_cfg_r17->pack(bref));
      }
      if (csi_report_mode_r17_present) {
        HANDLE_CODE(csi_report_mode_r17.pack(bref));
      }
      if (nof_single_trp_csi_mode1_r17_present) {
        HANDLE_CODE(nof_single_trp_csi_mode1_r17.pack(bref));
      }
      if (report_quant_r17.is_present()) {
        HANDLE_CODE(report_quant_r17->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1720.is_present(), 1));
      HANDLE_CODE(bref.pack(aperiodic_v1720.is_present(), 1));
      if (semi_persistent_on_pusch_v1720.is_present()) {
        HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1720->report_slot_offset_list_r17.size() > 0, 1));
        HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_2_r17.size() > 0, 1));
        HANDLE_CODE(bref.pack(semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_1_r17.size() > 0, 1));
        if (semi_persistent_on_pusch_v1720->report_slot_offset_list_r17.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(bref,
                                      semi_persistent_on_pusch_v1720->report_slot_offset_list_r17,
                                      1,
                                      64,
                                      integer_packer<uint8_t>(0, 128)));
        }
        if (semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_2_r17.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(bref,
                                      semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_2_r17,
                                      1,
                                      64,
                                      integer_packer<uint8_t>(0, 128)));
        }
        if (semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_1_r17.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(bref,
                                      semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_1_r17,
                                      1,
                                      64,
                                      integer_packer<uint8_t>(0, 128)));
        }
      }
      if (aperiodic_v1720.is_present()) {
        HANDLE_CODE(bref.pack(aperiodic_v1720->report_slot_offset_list_r17.size() > 0, 1));
        HANDLE_CODE(bref.pack(aperiodic_v1720->report_slot_offset_list_dci_0_2_r17.size() > 0, 1));
        HANDLE_CODE(bref.pack(aperiodic_v1720->report_slot_offset_list_dci_0_1_r17.size() > 0, 1));
        if (aperiodic_v1720->report_slot_offset_list_r17.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, aperiodic_v1720->report_slot_offset_list_r17, 1, 64, integer_packer<uint8_t>(0, 128)));
        }
        if (aperiodic_v1720->report_slot_offset_list_dci_0_2_r17.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, aperiodic_v1720->report_slot_offset_list_dci_0_2_r17, 1, 64, integer_packer<uint8_t>(0, 128)));
        }
        if (aperiodic_v1720->report_slot_offset_list_dci_0_1_r17.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, aperiodic_v1720->report_slot_offset_list_dci_0_1_r17, 1, 64, integer_packer<uint8_t>(0, 128)));
        }
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(codebook_cfg_v1730.is_present(), 1));
      if (codebook_cfg_v1730.is_present()) {
        HANDLE_CODE(codebook_cfg_v1730->pack(bref));
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
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(semi_persistent_on_pusch_v1530, bref);
      if (semi_persistent_on_pusch_v1530.is_present()) {
        HANDLE_CODE(semi_persistent_on_pusch_v1530->report_slot_cfg_v1530.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(semi_persistent_on_pusch_v1610, bref);
      unpack_presence_flag(aperiodic_v1610, bref);
      unpack_presence_flag(report_quant_r16, bref);
      unpack_presence_flag(codebook_cfg_r16, bref);
      if (semi_persistent_on_pusch_v1610.is_present()) {
        bool report_slot_offset_list_dci_0_2_r16_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_2_r16_present, 1));
        bool report_slot_offset_list_dci_0_1_r16_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_1_r16_present, 1));
        if (report_slot_offset_list_dci_0_2_r16_present) {
          HANDLE_CODE(unpack_dyn_seq_of(semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_2_r16,
                                        bref,
                                        1,
                                        64,
                                        integer_packer<uint8_t>(0, 32)));
        }
        if (report_slot_offset_list_dci_0_1_r16_present) {
          HANDLE_CODE(unpack_dyn_seq_of(semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_1_r16,
                                        bref,
                                        1,
                                        64,
                                        integer_packer<uint8_t>(0, 32)));
        }
      }
      if (aperiodic_v1610.is_present()) {
        bool report_slot_offset_list_dci_0_2_r16_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_2_r16_present, 1));
        bool report_slot_offset_list_dci_0_1_r16_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_1_r16_present, 1));
        if (report_slot_offset_list_dci_0_2_r16_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              aperiodic_v1610->report_slot_offset_list_dci_0_2_r16, bref, 1, 64, integer_packer<uint8_t>(0, 32)));
        }
        if (report_slot_offset_list_dci_0_1_r16_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              aperiodic_v1610->report_slot_offset_list_dci_0_1_r16, bref, 1, 64, integer_packer<uint8_t>(0, 32)));
        }
      }
      if (report_quant_r16.is_present()) {
        HANDLE_CODE(report_quant_r16->unpack(bref));
      }
      if (codebook_cfg_r16.is_present()) {
        HANDLE_CODE(codebook_cfg_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cqi_bits_per_subband_r17_present, 1));
      unpack_presence_flag(group_based_beam_report_v1710, bref);
      unpack_presence_flag(codebook_cfg_r17, bref);
      HANDLE_CODE(bref.unpack(shared_cmr_r17_present, 1));
      HANDLE_CODE(bref.unpack(csi_report_mode_r17_present, 1));
      HANDLE_CODE(bref.unpack(nof_single_trp_csi_mode1_r17_present, 1));
      unpack_presence_flag(report_quant_r17, bref);
      if (group_based_beam_report_v1710.is_present()) {
        HANDLE_CODE(group_based_beam_report_v1710->nrof_reported_groups_r17.unpack(bref));
      }
      if (codebook_cfg_r17.is_present()) {
        HANDLE_CODE(codebook_cfg_r17->unpack(bref));
      }
      if (csi_report_mode_r17_present) {
        HANDLE_CODE(csi_report_mode_r17.unpack(bref));
      }
      if (nof_single_trp_csi_mode1_r17_present) {
        HANDLE_CODE(nof_single_trp_csi_mode1_r17.unpack(bref));
      }
      if (report_quant_r17.is_present()) {
        HANDLE_CODE(report_quant_r17->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(semi_persistent_on_pusch_v1720, bref);
      unpack_presence_flag(aperiodic_v1720, bref);
      if (semi_persistent_on_pusch_v1720.is_present()) {
        bool report_slot_offset_list_r17_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_r17_present, 1));
        bool report_slot_offset_list_dci_0_2_r17_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_2_r17_present, 1));
        bool report_slot_offset_list_dci_0_1_r17_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_1_r17_present, 1));
        if (report_slot_offset_list_r17_present) {
          HANDLE_CODE(unpack_dyn_seq_of(semi_persistent_on_pusch_v1720->report_slot_offset_list_r17,
                                        bref,
                                        1,
                                        64,
                                        integer_packer<uint8_t>(0, 128)));
        }
        if (report_slot_offset_list_dci_0_2_r17_present) {
          HANDLE_CODE(unpack_dyn_seq_of(semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_2_r17,
                                        bref,
                                        1,
                                        64,
                                        integer_packer<uint8_t>(0, 128)));
        }
        if (report_slot_offset_list_dci_0_1_r17_present) {
          HANDLE_CODE(unpack_dyn_seq_of(semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_1_r17,
                                        bref,
                                        1,
                                        64,
                                        integer_packer<uint8_t>(0, 128)));
        }
      }
      if (aperiodic_v1720.is_present()) {
        bool report_slot_offset_list_r17_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_r17_present, 1));
        bool report_slot_offset_list_dci_0_2_r17_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_2_r17_present, 1));
        bool report_slot_offset_list_dci_0_1_r17_present;
        HANDLE_CODE(bref.unpack(report_slot_offset_list_dci_0_1_r17_present, 1));
        if (report_slot_offset_list_r17_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              aperiodic_v1720->report_slot_offset_list_r17, bref, 1, 64, integer_packer<uint8_t>(0, 128)));
        }
        if (report_slot_offset_list_dci_0_2_r17_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              aperiodic_v1720->report_slot_offset_list_dci_0_2_r17, bref, 1, 64, integer_packer<uint8_t>(0, 128)));
        }
        if (report_slot_offset_list_dci_0_1_r17_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              aperiodic_v1720->report_slot_offset_list_dci_0_1_r17, bref, 1, 64, integer_packer<uint8_t>(0, 128)));
        }
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(codebook_cfg_v1730, bref);
      if (codebook_cfg_v1730.is_present()) {
        HANDLE_CODE(codebook_cfg_v1730->unpack(bref));
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
    if (semi_persistent_on_pusch_v1610.is_present()) {
      j.write_fieldname("semiPersistentOnPUSCH-v1610");
      j.start_obj();
      if (semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_2_r16.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-2-r16");
        for (const auto& e1 : semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_2_r16) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_1_r16.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-1-r16");
        for (const auto& e1 : semi_persistent_on_pusch_v1610->report_slot_offset_list_dci_0_1_r16) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (aperiodic_v1610.is_present()) {
      j.write_fieldname("aperiodic-v1610");
      j.start_obj();
      if (aperiodic_v1610->report_slot_offset_list_dci_0_2_r16.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-2-r16");
        for (const auto& e1 : aperiodic_v1610->report_slot_offset_list_dci_0_2_r16) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (aperiodic_v1610->report_slot_offset_list_dci_0_1_r16.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-1-r16");
        for (const auto& e1 : aperiodic_v1610->report_slot_offset_list_dci_0_1_r16) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (report_quant_r16.is_present()) {
      j.write_fieldname("reportQuantity-r16");
      report_quant_r16->to_json(j);
    }
    if (codebook_cfg_r16.is_present()) {
      j.write_fieldname("codebookConfig-r16");
      codebook_cfg_r16->to_json(j);
    }
    if (cqi_bits_per_subband_r17_present) {
      j.write_str("cqi-BitsPerSubband-r17", "bits4");
    }
    if (group_based_beam_report_v1710.is_present()) {
      j.write_fieldname("groupBasedBeamReporting-v1710");
      j.start_obj();
      j.write_str("nrofReportedGroups-r17", group_based_beam_report_v1710->nrof_reported_groups_r17.to_string());
      j.end_obj();
    }
    if (codebook_cfg_r17.is_present()) {
      j.write_fieldname("codebookConfig-r17");
      codebook_cfg_r17->to_json(j);
    }
    if (shared_cmr_r17_present) {
      j.write_str("sharedCMR-r17", "enable");
    }
    if (csi_report_mode_r17_present) {
      j.write_str("csi-ReportMode-r17", csi_report_mode_r17.to_string());
    }
    if (nof_single_trp_csi_mode1_r17_present) {
      j.write_str("numberOfSingleTRP-CSI-Mode1-r17", nof_single_trp_csi_mode1_r17.to_string());
    }
    if (report_quant_r17.is_present()) {
      j.write_fieldname("reportQuantity-r17");
      report_quant_r17->to_json(j);
    }
    if (semi_persistent_on_pusch_v1720.is_present()) {
      j.write_fieldname("semiPersistentOnPUSCH-v1720");
      j.start_obj();
      if (semi_persistent_on_pusch_v1720->report_slot_offset_list_r17.size() > 0) {
        j.start_array("reportSlotOffsetList-r17");
        for (const auto& e1 : semi_persistent_on_pusch_v1720->report_slot_offset_list_r17) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_2_r17.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-2-r17");
        for (const auto& e1 : semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_2_r17) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_1_r17.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-1-r17");
        for (const auto& e1 : semi_persistent_on_pusch_v1720->report_slot_offset_list_dci_0_1_r17) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (aperiodic_v1720.is_present()) {
      j.write_fieldname("aperiodic-v1720");
      j.start_obj();
      if (aperiodic_v1720->report_slot_offset_list_r17.size() > 0) {
        j.start_array("reportSlotOffsetList-r17");
        for (const auto& e1 : aperiodic_v1720->report_slot_offset_list_r17) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (aperiodic_v1720->report_slot_offset_list_dci_0_2_r17.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-2-r17");
        for (const auto& e1 : aperiodic_v1720->report_slot_offset_list_dci_0_2_r17) {
          j.write_int(e1);
        }
        j.end_array();
      }
      if (aperiodic_v1720->report_slot_offset_list_dci_0_1_r17.size() > 0) {
        j.start_array("reportSlotOffsetListDCI-0-1-r17");
        for (const auto& e1 : aperiodic_v1720->report_slot_offset_list_dci_0_1_r17) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (codebook_cfg_v1730.is_present()) {
      j.write_fieldname("codebookConfig-v1730");
      codebook_cfg_v1730->to_json(j);
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

const char* csi_report_cfg_s::report_cfg_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodic", "semiPersistentOnPUCCH", "semiPersistentOnPUSCH", "aperiodic"};
  return convert_enum_idx(names, 4, value, "csi_report_cfg_s::report_cfg_type_c_::types");
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

const char* csi_report_cfg_s::report_quant_c_::types_opts::to_string() const
{
  static const char* names[] = {"none",
                                "cri-RI-PMI-CQI",
                                "cri-RI-i1",
                                "cri-RI-i1-CQI",
                                "cri-RI-CQI",
                                "cri-RSRP",
                                "ssb-Index-RSRP",
                                "cri-RI-LI-PMI-CQI"};
  return convert_enum_idx(names, 8, value, "csi_report_cfg_s::report_quant_c_::types");
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

const char* csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types_opts::to_string() const
{
  static const char* names[] = {"subbands3",
                                "subbands4",
                                "subbands5",
                                "subbands6",
                                "subbands7",
                                "subbands8",
                                "subbands9",
                                "subbands10",
                                "subbands11",
                                "subbands12",
                                "subbands13",
                                "subbands14",
                                "subbands15",
                                "subbands16",
                                "subbands17",
                                "subbands18",
                                "subbands19-v1530"};
  return convert_enum_idx(names, 17, value, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types");
}
uint8_t csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  return map_enum_number(numbers, 17, value, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types");
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

const char* csi_report_cfg_s::group_based_beam_report_c_::types_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::group_based_beam_report_c_::types");
}

const char* csi_report_cfg_s::cqi_table_opts::to_string() const
{
  static const char* names[] = {"table1", "table2", "table3", "table4-r17"};
  return convert_enum_idx(names, 4, value, "csi_report_cfg_s::cqi_table_e_");
}
uint8_t csi_report_cfg_s::cqi_table_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "csi_report_cfg_s::cqi_table_e_");
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

void csi_report_cfg_s::report_quant_r16_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::report_quant_r16_c_::set_cri_sinr_r16()
{
  set(types::cri_sinr_r16);
}
void csi_report_cfg_s::report_quant_r16_c_::set_ssb_idx_sinr_r16()
{
  set(types::ssb_idx_sinr_r16);
}
void csi_report_cfg_s::report_quant_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE csi_report_cfg_s::report_quant_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_cfg_s::report_quant_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  return SRSASN_SUCCESS;
}

const char* csi_report_cfg_s::report_quant_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cri-SINR-r16", "ssb-Index-SINR-r16"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::report_quant_r16_c_::types");
}

const char* csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(
      names, 4, value, "csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_e_");
}
uint8_t csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(
      numbers, 4, value, "csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_e_");
}

const char* csi_report_cfg_s::csi_report_mode_r17_opts::to_string() const
{
  static const char* names[] = {"mode1", "mode2"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::csi_report_mode_r17_e_");
}
uint8_t csi_report_cfg_s::csi_report_mode_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_report_cfg_s::csi_report_mode_r17_e_");
}

const char* csi_report_cfg_s::nof_single_trp_csi_mode1_r17_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2"};
  return convert_enum_idx(names, 3, value, "csi_report_cfg_s::nof_single_trp_csi_mode1_r17_e_");
}
uint8_t csi_report_cfg_s::nof_single_trp_csi_mode1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2};
  return map_enum_number(numbers, 3, value, "csi_report_cfg_s::nof_single_trp_csi_mode1_r17_e_");
}

void csi_report_cfg_s::report_quant_r17_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::report_quant_r17_c_::set_cri_rsrp_idx_r17()
{
  set(types::cri_rsrp_idx_r17);
}
void csi_report_cfg_s::report_quant_r17_c_::set_ssb_idx_rsrp_idx_r17()
{
  set(types::ssb_idx_rsrp_idx_r17);
}
void csi_report_cfg_s::report_quant_r17_c_::set_cri_sinr_idx_r17()
{
  set(types::cri_sinr_idx_r17);
}
void csi_report_cfg_s::report_quant_r17_c_::set_ssb_idx_sinr_idx_r17()
{
  set(types::ssb_idx_sinr_idx_r17);
}
void csi_report_cfg_s::report_quant_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE csi_report_cfg_s::report_quant_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_report_cfg_s::report_quant_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  return SRSASN_SUCCESS;
}

const char* csi_report_cfg_s::report_quant_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "cri-RSRP-Index-r17", "ssb-Index-RSRP-Index-r17", "cri-SINR-Index-r17", "ssb-Index-SINR-Index-r17"};
  return convert_enum_idx(names, 4, value, "csi_report_cfg_s::report_quant_r17_c_::types");
}

// CSI-ResourceConfig ::= SEQUENCE
SRSASN_CODE csi_res_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, csi_res_cfg_id, (uint8_t)0u, (uint8_t)111u));
  HANDLE_CODE(csi_rs_res_set_list.pack(bref));
  HANDLE_CODE(pack_integer(bref, bwp_id, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(res_type.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= csi_ssb_res_set_list_ext_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(csi_ssb_res_set_list_ext_r17_present, 1));
      if (csi_ssb_res_set_list_ext_r17_present) {
        HANDLE_CODE(pack_integer(bref, csi_ssb_res_set_list_ext_r17, (uint8_t)0u, (uint8_t)63u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_res_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_res_cfg_id, bref, (uint8_t)0u, (uint8_t)111u));
  HANDLE_CODE(csi_rs_res_set_list.unpack(bref));
  HANDLE_CODE(unpack_integer(bwp_id, bref, (uint8_t)0u, (uint8_t)4u));
  HANDLE_CODE(res_type.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(csi_ssb_res_set_list_ext_r17_present, 1));
      if (csi_ssb_res_set_list_ext_r17_present) {
        HANDLE_CODE(unpack_integer(csi_ssb_res_set_list_ext_r17, bref, (uint8_t)0u, (uint8_t)63u));
      }
    }
  }
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
  if (ext) {
    if (csi_ssb_res_set_list_ext_r17_present) {
      j.write_int("csi-SSB-ResourceSetListExt-r17", csi_ssb_res_set_list_ext_r17);
    }
  }
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
                                      c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list,
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
        HANDLE_CODE(unpack_fixed_seq_of(c.get<nzp_csi_rs_ssb_s_>().csi_ssb_res_set_list,
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

const char* csi_res_cfg_s::csi_rs_res_set_list_c_::types_opts::to_string() const
{
  static const char* names[] = {"nzp-CSI-RS-SSB", "csi-IM-ResourceSetList"};
  return convert_enum_idx(names, 2, value, "csi_res_cfg_s::csi_rs_res_set_list_c_::types");
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= serving_add_pci_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(serving_add_pci_list_r17.is_present(), 1));
      if (serving_add_pci_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *serving_add_pci_list_r17, 1, 64, integer_packer<uint8_t>(0, 7)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE csi_ssb_res_set_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(csi_ssb_res_set_id, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_dyn_seq_of(csi_ssb_res_list, bref, 1, 64, integer_packer<uint8_t>(0, 63)));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(serving_add_pci_list_r17, bref);
      if (serving_add_pci_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*serving_add_pci_list_r17, bref, 1, 64, integer_packer<uint8_t>(0, 7)));
      }
    }
  }
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
  if (ext) {
    if (serving_add_pci_list_r17.is_present()) {
      j.start_array("servingAdditionalPCIList-r17");
      for (const auto& e1 : *serving_add_pci_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= aperiodic_trigger_offset_r16_present;
    group_flags[1] |= pdc_info_r17_present;
    group_flags[1] |= cmr_grouping_and_pairing_r17.is_present();
    group_flags[1] |= aperiodic_trigger_offset_r17_present;
    group_flags[1] |= aperiodic_trigger_offset_l2_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(aperiodic_trigger_offset_r16_present, 1));
      if (aperiodic_trigger_offset_r16_present) {
        HANDLE_CODE(pack_integer(bref, aperiodic_trigger_offset_r16, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdc_info_r17_present, 1));
      HANDLE_CODE(bref.pack(cmr_grouping_and_pairing_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(aperiodic_trigger_offset_r17_present, 1));
      HANDLE_CODE(bref.pack(aperiodic_trigger_offset_l2_r17_present, 1));
      if (cmr_grouping_and_pairing_r17.is_present()) {
        HANDLE_CODE(cmr_grouping_and_pairing_r17->pack(bref));
      }
      if (aperiodic_trigger_offset_r17_present) {
        HANDLE_CODE(pack_integer(bref, aperiodic_trigger_offset_r17, (uint8_t)0u, (uint8_t)124u));
      }
      if (aperiodic_trigger_offset_l2_r17_present) {
        HANDLE_CODE(pack_integer(bref, aperiodic_trigger_offset_l2_r17, (uint8_t)0u, (uint8_t)31u));
      }
    }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(aperiodic_trigger_offset_r16_present, 1));
      if (aperiodic_trigger_offset_r16_present) {
        HANDLE_CODE(unpack_integer(aperiodic_trigger_offset_r16, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pdc_info_r17_present, 1));
      unpack_presence_flag(cmr_grouping_and_pairing_r17, bref);
      HANDLE_CODE(bref.unpack(aperiodic_trigger_offset_r17_present, 1));
      HANDLE_CODE(bref.unpack(aperiodic_trigger_offset_l2_r17_present, 1));
      if (cmr_grouping_and_pairing_r17.is_present()) {
        HANDLE_CODE(cmr_grouping_and_pairing_r17->unpack(bref));
      }
      if (aperiodic_trigger_offset_r17_present) {
        HANDLE_CODE(unpack_integer(aperiodic_trigger_offset_r17, bref, (uint8_t)0u, (uint8_t)124u));
      }
      if (aperiodic_trigger_offset_l2_r17_present) {
        HANDLE_CODE(unpack_integer(aperiodic_trigger_offset_l2_r17, bref, (uint8_t)0u, (uint8_t)31u));
      }
    }
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
  if (ext) {
    if (aperiodic_trigger_offset_r16_present) {
      j.write_int("aperiodicTriggeringOffset-r16", aperiodic_trigger_offset_r16);
    }
    if (pdc_info_r17_present) {
      j.write_str("pdc-Info-r17", "true");
    }
    if (cmr_grouping_and_pairing_r17.is_present()) {
      j.write_fieldname("cmrGroupingAndPairing-r17");
      cmr_grouping_and_pairing_r17->to_json(j);
    }
    if (aperiodic_trigger_offset_r17_present) {
      j.write_int("aperiodicTriggeringOffset-r17", aperiodic_trigger_offset_r17);
    }
    if (aperiodic_trigger_offset_l2_r17_present) {
      j.write_int("aperiodicTriggeringOffsetL2-r17", aperiodic_trigger_offset_l2_r17);
    }
  }
  j.end_obj();
}

const char* nzp_csi_rs_res_set_s::repeat_opts::to_string() const
{
  static const char* names[] = {"on", "off"};
  return convert_enum_idx(names, 2, value, "nzp_csi_rs_res_set_s::repeat_e_");
}

// SCellActivationRS-Config-r17 ::= SEQUENCE
SRSASN_CODE scell_activation_rs_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gap_between_bursts_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, scell_activation_rs_id_r17, (uint16_t)1u, (uint16_t)255u));
  HANDLE_CODE(pack_integer(bref, res_set_r17, (uint8_t)0u, (uint8_t)63u));
  if (gap_between_bursts_r17_present) {
    HANDLE_CODE(pack_integer(bref, gap_between_bursts_r17, (uint8_t)2u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, qcl_info_r17, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scell_activation_rs_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(gap_between_bursts_r17_present, 1));

  HANDLE_CODE(unpack_integer(scell_activation_rs_id_r17, bref, (uint16_t)1u, (uint16_t)255u));
  HANDLE_CODE(unpack_integer(res_set_r17, bref, (uint8_t)0u, (uint8_t)63u));
  if (gap_between_bursts_r17_present) {
    HANDLE_CODE(unpack_integer(gap_between_bursts_r17, bref, (uint8_t)2u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(qcl_info_r17, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void scell_activation_rs_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("scellActivationRS-Id-r17", scell_activation_rs_id_r17);
  j.write_int("resourceSet-r17", res_set_r17);
  if (gap_between_bursts_r17_present) {
    j.write_int("gapBetweenBursts-r17", gap_between_bursts_r17);
  }
  j.write_int("qcl-Info-r17", qcl_info_r17);
  j.end_obj();
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= report_trigger_size_dci_0_2_r16_present;
    group_flags[1] |= scell_activation_rs_cfg_to_add_mod_list_r17.is_present();
    group_flags[1] |= scell_activation_rs_cfg_to_release_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(report_trigger_size_dci_0_2_r16_present, 1));
      if (report_trigger_size_dci_0_2_r16_present) {
        HANDLE_CODE(pack_integer(bref, report_trigger_size_dci_0_2_r16, (uint8_t)0u, (uint8_t)6u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(scell_activation_rs_cfg_to_add_mod_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(scell_activation_rs_cfg_to_release_list_r17.is_present(), 1));
      if (scell_activation_rs_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *scell_activation_rs_cfg_to_add_mod_list_r17, 1, 255));
      }
      if (scell_activation_rs_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(
            bref, *scell_activation_rs_cfg_to_release_list_r17, 1, 255, integer_packer<uint16_t>(1, 255)));
      }
    }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(report_trigger_size_dci_0_2_r16_present, 1));
      if (report_trigger_size_dci_0_2_r16_present) {
        HANDLE_CODE(unpack_integer(report_trigger_size_dci_0_2_r16, bref, (uint8_t)0u, (uint8_t)6u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(scell_activation_rs_cfg_to_add_mod_list_r17, bref);
      unpack_presence_flag(scell_activation_rs_cfg_to_release_list_r17, bref);
      if (scell_activation_rs_cfg_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*scell_activation_rs_cfg_to_add_mod_list_r17, bref, 1, 255));
      }
      if (scell_activation_rs_cfg_to_release_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(
            *scell_activation_rs_cfg_to_release_list_r17, bref, 1, 255, integer_packer<uint16_t>(1, 255)));
      }
    }
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
  if (ext) {
    if (report_trigger_size_dci_0_2_r16_present) {
      j.write_int("reportTriggerSizeDCI-0-2-r16", report_trigger_size_dci_0_2_r16);
    }
    if (scell_activation_rs_cfg_to_add_mod_list_r17.is_present()) {
      j.start_array("sCellActivationRS-ConfigToAddModList-r17");
      for (const auto& e1 : *scell_activation_rs_cfg_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (scell_activation_rs_cfg_to_release_list_r17.is_present()) {
      j.start_array("sCellActivationRS-ConfigToReleaseList-r17");
      for (const auto& e1 : *scell_activation_rs_cfg_to_release_list_r17) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// DL-PRS-QCL-Info-r17 ::= CHOICE
void dl_prs_qcl_info_r17_c::destroy_()
{
  switch (type_) {
    case types::ssb_r17:
      c.destroy<ssb_r17_s_>();
      break;
    case types::dl_prs_r17:
      c.destroy<dl_prs_r17_s_>();
      break;
    default:
      break;
  }
}
void dl_prs_qcl_info_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ssb_r17:
      c.init<ssb_r17_s_>();
      break;
    case types::dl_prs_r17:
      c.init<dl_prs_r17_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_prs_qcl_info_r17_c");
  }
}
dl_prs_qcl_info_r17_c::dl_prs_qcl_info_r17_c(const dl_prs_qcl_info_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_r17:
      c.init(other.c.get<ssb_r17_s_>());
      break;
    case types::dl_prs_r17:
      c.init(other.c.get<dl_prs_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_prs_qcl_info_r17_c");
  }
}
dl_prs_qcl_info_r17_c& dl_prs_qcl_info_r17_c::operator=(const dl_prs_qcl_info_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_r17:
      c.set(other.c.get<ssb_r17_s_>());
      break;
    case types::dl_prs_r17:
      c.set(other.c.get<dl_prs_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_prs_qcl_info_r17_c");
  }

  return *this;
}
dl_prs_qcl_info_r17_c::ssb_r17_s_& dl_prs_qcl_info_r17_c::set_ssb_r17()
{
  set(types::ssb_r17);
  return c.get<ssb_r17_s_>();
}
dl_prs_qcl_info_r17_c::dl_prs_r17_s_& dl_prs_qcl_info_r17_c::set_dl_prs_r17()
{
  set(types::dl_prs_r17);
  return c.get<dl_prs_r17_s_>();
}
void dl_prs_qcl_info_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_r17:
      j.write_fieldname("ssb-r17");
      j.start_obj();
      j.write_int("ssb-Index-r17", c.get<ssb_r17_s_>().ssb_idx_r17);
      j.write_str("rs-Type-r17", c.get<ssb_r17_s_>().rs_type_r17.to_string());
      j.end_obj();
      break;
    case types::dl_prs_r17:
      j.write_fieldname("dl-PRS-r17");
      j.start_obj();
      j.write_int("qcl-DL-PRS-ResourceID-r17", c.get<dl_prs_r17_s_>().qcl_dl_prs_res_id_r17);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "dl_prs_qcl_info_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_prs_qcl_info_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_r17:
      bref.pack(c.get<ssb_r17_s_>().ext, 1);
      HANDLE_CODE(pack_integer(bref, c.get<ssb_r17_s_>().ssb_idx_r17, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(c.get<ssb_r17_s_>().rs_type_r17.pack(bref));
      break;
    case types::dl_prs_r17:
      bref.pack(c.get<dl_prs_r17_s_>().ext, 1);
      HANDLE_CODE(pack_integer(bref, c.get<dl_prs_r17_s_>().qcl_dl_prs_res_id_r17, (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_, "dl_prs_qcl_info_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_prs_qcl_info_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_r17:
      bref.unpack(c.get<ssb_r17_s_>().ext, 1);
      HANDLE_CODE(unpack_integer(c.get<ssb_r17_s_>().ssb_idx_r17, bref, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(c.get<ssb_r17_s_>().rs_type_r17.unpack(bref));
      break;
    case types::dl_prs_r17:
      bref.unpack(c.get<dl_prs_r17_s_>().ext, 1);
      HANDLE_CODE(unpack_integer(c.get<dl_prs_r17_s_>().qcl_dl_prs_res_id_r17, bref, (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_, "dl_prs_qcl_info_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17_opts::to_string() const
{
  static const char* names[] = {"typeC", "typeD", "typeC-plus-typeD"};
  return convert_enum_idx(names, 3, value, "dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17_e_");
}

const char* dl_prs_qcl_info_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"ssb-r17", "dl-PRS-r17"};
  return convert_enum_idx(names, 2, value, "dl_prs_qcl_info_r17_c::types");
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

// NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17 ::= CHOICE
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::destroy_()
{
  switch (type_) {
    case types::scs15_r17:
      c.destroy<scs15_r17_c_>();
      break;
    case types::scs30_r17:
      c.destroy<scs30_r17_c_>();
      break;
    case types::scs60_r17:
      c.destroy<scs60_r17_c_>();
      break;
    case types::scs120_r17:
      c.destroy<scs120_r17_c_>();
      break;
    default:
      break;
  }
}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::scs15_r17:
      c.init<scs15_r17_c_>();
      break;
    case types::scs30_r17:
      c.init<scs30_r17_c_>();
      break;
    case types::scs60_r17:
      c.init<scs60_r17_c_>();
      break;
    case types::scs120_r17:
      c.init<scs120_r17_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c");
  }
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::scs15_r17:
      c.init(other.c.get<scs15_r17_c_>());
      break;
    case types::scs30_r17:
      c.init(other.c.get<scs30_r17_c_>());
      break;
    case types::scs60_r17:
      c.init(other.c.get<scs60_r17_c_>());
      break;
    case types::scs120_r17:
      c.init(other.c.get<scs120_r17_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c");
  }
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::operator=(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::scs15_r17:
      c.set(other.c.get<scs15_r17_c_>());
      break;
    case types::scs30_r17:
      c.set(other.c.get<scs30_r17_c_>());
      break;
    case types::scs60_r17:
      c.set(other.c.get<scs60_r17_c_>());
      break;
    case types::scs120_r17:
      c.set(other.c.get<scs120_r17_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c");
  }

  return *this;
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs15_r17()
{
  set(types::scs15_r17);
  return c.get<scs15_r17_c_>();
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs30_r17()
{
  set(types::scs30_r17);
  return c.get<scs30_r17_c_>();
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs60_r17()
{
  set(types::scs60_r17);
  return c.get<scs60_r17_c_>();
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs120_r17()
{
  set(types::scs120_r17);
  return c.get<scs120_r17_c_>();
}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scs15_r17:
      j.write_fieldname("scs15-r17");
      c.get<scs15_r17_c_>().to_json(j);
      break;
    case types::scs30_r17:
      j.write_fieldname("scs30-r17");
      c.get<scs30_r17_c_>().to_json(j);
      break;
    case types::scs60_r17:
      j.write_fieldname("scs60-r17");
      c.get<scs60_r17_c_>().to_json(j);
      break;
    case types::scs120_r17:
      j.write_fieldname("scs120-r17");
      c.get<scs120_r17_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scs15_r17:
      HANDLE_CODE(c.get<scs15_r17_c_>().pack(bref));
      break;
    case types::scs30_r17:
      HANDLE_CODE(c.get<scs30_r17_c_>().pack(bref));
      break;
    case types::scs60_r17:
      HANDLE_CODE(c.get<scs60_r17_c_>().pack(bref));
      break;
    case types::scs120_r17:
      HANDLE_CODE(c.get<scs120_r17_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scs15_r17:
      HANDLE_CODE(c.get<scs15_r17_c_>().unpack(bref));
      break;
    case types::scs30_r17:
      HANDLE_CODE(c.get<scs30_r17_c_>().unpack(bref));
      break;
    case types::scs60_r17:
      HANDLE_CODE(c.get<scs60_r17_c_>().unpack(bref));
      break;
    case types::scs120_r17:
      HANDLE_CODE(c.get<scs120_r17_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::destroy_() {}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::scs15_r17_c_(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n4_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n5_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n8_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n10_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n16_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n20_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n32_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n320_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_");
  }
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::operator=(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n4_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n5_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n8_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n10_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n16_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n20_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n32_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n320_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_");
  }

  return *this;
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n4_r17()
{
  set(types::n4_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n5_r17()
{
  set(types::n5_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n8_r17()
{
  set(types::n8_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n10_r17()
{
  set(types::n10_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n16_r17()
{
  set(types::n16_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n20_r17()
{
  set(types::n20_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n4_r17:
      j.write_int("n4-r17", c.get<uint8_t>());
      break;
    case types::n5_r17:
      j.write_int("n5-r17", c.get<uint8_t>());
      break;
    case types::n8_r17:
      j.write_int("n8-r17", c.get<uint8_t>());
      break;
    case types::n10_r17:
      j.write_int("n10-r17", c.get<uint8_t>());
      break;
    case types::n16_r17:
      j.write_int("n16-r17", c.get<uint8_t>());
      break;
    case types::n20_r17:
      j.write_int("n20-r17", c.get<uint8_t>());
      break;
    case types::n32_r17:
      j.write_int("n32-r17", c.get<uint8_t>());
      break;
    case types::n40_r17:
      j.write_int("n40-r17", c.get<uint8_t>());
      break;
    case types::n64_r17:
      j.write_int("n64-r17", c.get<uint8_t>());
      break;
    case types::n80_r17:
      j.write_int("n80-r17", c.get<uint8_t>());
      break;
    case types::n160_r17:
      j.write_int("n160-r17", c.get<uint8_t>());
      break;
    case types::n320_r17:
      j.write_int("n320-r17", c.get<uint16_t>());
      break;
    case types::n640_r17:
      j.write_int("n640-r17", c.get<uint16_t>());
      break;
    case types::n1280_r17:
      j.write_int("n1280-r17", c.get<uint16_t>());
      break;
    case types::n2560_r17:
      j.write_int("n2560-r17", c.get<uint16_t>());
      break;
    case types::n5120_r17:
      j.write_int("n5120-r17", c.get<uint16_t>());
      break;
    case types::n10240_r17:
      j.write_int("n10240-r17", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n4_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::n5_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)4u));
      break;
    case types::n8_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n10_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::n16_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::n20_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::n32_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n160_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n320_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n640_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n4_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::n5_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)4u));
      break;
    case types::n8_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n10_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::n16_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::n20_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::n32_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n160_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n320_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n640_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"n4-r17",
                                "n5-r17",
                                "n8-r17",
                                "n10-r17",
                                "n16-r17",
                                "n20-r17",
                                "n32-r17",
                                "n40-r17",
                                "n64-r17",
                                "n80-r17",
                                "n160-r17",
                                "n320-r17",
                                "n640-r17",
                                "n1280-r17",
                                "n2560-r17",
                                "n5120-r17",
                                "n10240-r17"};
  return convert_enum_idx(names, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types");
}
uint16_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types");
}

void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::destroy_() {}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::scs30_r17_c_(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n8_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n10_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n16_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n20_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n32_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n128_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n320_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n20480_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_");
  }
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::operator=(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n8_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n10_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n16_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n20_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n32_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n128_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n320_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n20480_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_");
  }

  return *this;
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n8_r17()
{
  set(types::n8_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n10_r17()
{
  set(types::n10_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n16_r17()
{
  set(types::n16_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n20_r17()
{
  set(types::n20_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n128_r17()
{
  set(types::n128_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n20480_r17()
{
  set(types::n20480_r17);
  return c.get<uint16_t>();
}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n8_r17:
      j.write_int("n8-r17", c.get<uint8_t>());
      break;
    case types::n10_r17:
      j.write_int("n10-r17", c.get<uint8_t>());
      break;
    case types::n16_r17:
      j.write_int("n16-r17", c.get<uint8_t>());
      break;
    case types::n20_r17:
      j.write_int("n20-r17", c.get<uint8_t>());
      break;
    case types::n32_r17:
      j.write_int("n32-r17", c.get<uint8_t>());
      break;
    case types::n40_r17:
      j.write_int("n40-r17", c.get<uint8_t>());
      break;
    case types::n64_r17:
      j.write_int("n64-r17", c.get<uint8_t>());
      break;
    case types::n80_r17:
      j.write_int("n80-r17", c.get<uint8_t>());
      break;
    case types::n128_r17:
      j.write_int("n128-r17", c.get<uint8_t>());
      break;
    case types::n160_r17:
      j.write_int("n160-r17", c.get<uint8_t>());
      break;
    case types::n320_r17:
      j.write_int("n320-r17", c.get<uint16_t>());
      break;
    case types::n640_r17:
      j.write_int("n640-r17", c.get<uint16_t>());
      break;
    case types::n1280_r17:
      j.write_int("n1280-r17", c.get<uint16_t>());
      break;
    case types::n2560_r17:
      j.write_int("n2560-r17", c.get<uint16_t>());
      break;
    case types::n5120_r17:
      j.write_int("n5120-r17", c.get<uint16_t>());
      break;
    case types::n10240_r17:
      j.write_int("n10240-r17", c.get<uint16_t>());
      break;
    case types::n20480_r17:
      j.write_int("n20480-r17", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n8_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n10_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::n16_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::n20_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::n32_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n128_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::n160_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n320_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n640_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::n20480_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)20479u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n8_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n10_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::n16_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::n20_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::n32_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n128_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::n160_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n320_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n640_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::n20480_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)20479u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"n8-r17",
                                "n10-r17",
                                "n16-r17",
                                "n20-r17",
                                "n32-r17",
                                "n40-r17",
                                "n64-r17",
                                "n80-r17",
                                "n128-r17",
                                "n160-r17",
                                "n320-r17",
                                "n640-r17",
                                "n1280-r17",
                                "n2560-r17",
                                "n5120-r17",
                                "n10240-r17",
                                "n20480-r17"};
  return convert_enum_idx(names, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types");
}
uint16_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, 5120, 10240, 20480};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types");
}

void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::destroy_() {}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::scs60_r17_c_(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n16_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n20_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n32_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n128_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n256_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n320_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n20480_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n40960_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_");
  }
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::operator=(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n16_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n20_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n32_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n128_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n256_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n320_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n20480_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n40960_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_");
  }

  return *this;
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n16_r17()
{
  set(types::n16_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n20_r17()
{
  set(types::n20_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n128_r17()
{
  set(types::n128_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n256_r17()
{
  set(types::n256_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n20480_r17()
{
  set(types::n20480_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n40960_r17()
{
  set(types::n40960_r17);
  return c.get<uint16_t>();
}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n16_r17:
      j.write_int("n16-r17", c.get<uint8_t>());
      break;
    case types::n20_r17:
      j.write_int("n20-r17", c.get<uint8_t>());
      break;
    case types::n32_r17:
      j.write_int("n32-r17", c.get<uint8_t>());
      break;
    case types::n40_r17:
      j.write_int("n40-r17", c.get<uint8_t>());
      break;
    case types::n64_r17:
      j.write_int("n64-r17", c.get<uint8_t>());
      break;
    case types::n80_r17:
      j.write_int("n80-r17", c.get<uint8_t>());
      break;
    case types::n128_r17:
      j.write_int("n128-r17", c.get<uint8_t>());
      break;
    case types::n160_r17:
      j.write_int("n160-r17", c.get<uint8_t>());
      break;
    case types::n256_r17:
      j.write_int("n256-r17", c.get<uint16_t>());
      break;
    case types::n320_r17:
      j.write_int("n320-r17", c.get<uint16_t>());
      break;
    case types::n640_r17:
      j.write_int("n640-r17", c.get<uint16_t>());
      break;
    case types::n1280_r17:
      j.write_int("n1280-r17", c.get<uint16_t>());
      break;
    case types::n2560_r17:
      j.write_int("n2560-r17", c.get<uint16_t>());
      break;
    case types::n5120_r17:
      j.write_int("n5120-r17", c.get<uint16_t>());
      break;
    case types::n10240_r17:
      j.write_int("n10240-r17", c.get<uint16_t>());
      break;
    case types::n20480_r17:
      j.write_int("n20480-r17", c.get<uint16_t>());
      break;
    case types::n40960_r17:
      j.write_int("n40960-r17", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n16_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::n20_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::n32_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n128_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::n160_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n256_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::n320_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n640_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::n20480_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)20479u));
      break;
    case types::n40960_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)40959u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n16_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::n20_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::n32_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n128_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::n160_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n256_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::n320_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n640_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::n20480_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)20479u));
      break;
    case types::n40960_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)40959u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"n16-r17",
                                "n20-r17",
                                "n32-r17",
                                "n40-r17",
                                "n64-r17",
                                "n80-r17",
                                "n128-r17",
                                "n160-r17",
                                "n256-r17",
                                "n320-r17",
                                "n640-r17",
                                "n1280-r17",
                                "n2560-r17",
                                "n5120-r17",
                                "n10240-r17",
                                "n20480-r17",
                                "n40960-r17"};
  return convert_enum_idx(names, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types");
}
uint16_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {
      16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types");
}

void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::destroy_() {}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::scs120_r17_c_(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n32_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n128_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n256_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n320_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n512_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n20480_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n40960_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::n81920_r17:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_");
  }
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::operator=(
    const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n32_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n40_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n64_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n80_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n128_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n160_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n256_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n320_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n512_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n640_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n1280_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n2560_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n5120_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n10240_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n20480_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n40960_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::n81920_r17:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_");
  }

  return *this;
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n128_r17()
{
  set(types::n128_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n256_r17()
{
  set(types::n256_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n512_r17()
{
  set(types::n512_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n20480_r17()
{
  set(types::n20480_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n40960_r17()
{
  set(types::n40960_r17);
  return c.get<uint16_t>();
}
uint32_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n81920_r17()
{
  set(types::n81920_r17);
  return c.get<uint32_t>();
}
void nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n32_r17:
      j.write_int("n32-r17", c.get<uint8_t>());
      break;
    case types::n40_r17:
      j.write_int("n40-r17", c.get<uint8_t>());
      break;
    case types::n64_r17:
      j.write_int("n64-r17", c.get<uint8_t>());
      break;
    case types::n80_r17:
      j.write_int("n80-r17", c.get<uint8_t>());
      break;
    case types::n128_r17:
      j.write_int("n128-r17", c.get<uint8_t>());
      break;
    case types::n160_r17:
      j.write_int("n160-r17", c.get<uint8_t>());
      break;
    case types::n256_r17:
      j.write_int("n256-r17", c.get<uint16_t>());
      break;
    case types::n320_r17:
      j.write_int("n320-r17", c.get<uint16_t>());
      break;
    case types::n512_r17:
      j.write_int("n512-r17", c.get<uint16_t>());
      break;
    case types::n640_r17:
      j.write_int("n640-r17", c.get<uint16_t>());
      break;
    case types::n1280_r17:
      j.write_int("n1280-r17", c.get<uint16_t>());
      break;
    case types::n2560_r17:
      j.write_int("n2560-r17", c.get<uint16_t>());
      break;
    case types::n5120_r17:
      j.write_int("n5120-r17", c.get<uint16_t>());
      break;
    case types::n10240_r17:
      j.write_int("n10240-r17", c.get<uint16_t>());
      break;
    case types::n20480_r17:
      j.write_int("n20480-r17", c.get<uint16_t>());
      break;
    case types::n40960_r17:
      j.write_int("n40960-r17", c.get<uint16_t>());
      break;
    case types::n81920_r17:
      j.write_int("n81920-r17", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n32_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n128_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::n160_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n256_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::n320_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n512_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::n640_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::n20480_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)20479u));
      break;
    case types::n40960_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)40959u));
      break;
    case types::n81920_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)81919u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n32_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::n40_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::n64_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::n80_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::n128_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::n160_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::n256_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::n320_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::n512_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::n640_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::n1280_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::n2560_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::n5120_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::n10240_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::n20480_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)20479u));
      break;
    case types::n40960_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)40959u));
      break;
    case types::n81920_r17:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)81919u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"n32-r17",
                                "n40-r17",
                                "n64-r17",
                                "n80-r17",
                                "n128-r17",
                                "n160-r17",
                                "n256-r17",
                                "n320-r17",
                                "n512-r17",
                                "n640-r17",
                                "n1280-r17",
                                "n2560-r17",
                                "n5120-r17",
                                "n10240-r17",
                                "n20480-r17",
                                "n40960-r17",
                                "n81920-r17"};
  return convert_enum_idx(
      names, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types");
}
uint32_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types_opts::to_number() const
{
  static const uint32_t numbers[] = {
      32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types");
}

const char* nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"scs15-r17", "scs30-r17", "scs60-r17", "scs120-r17"};
  return convert_enum_idx(names, 4, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types");
}
uint8_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types");
}

// NR-DL-PRS-Resource-r17 ::= SEQUENCE
SRSASN_CODE nr_dl_prs_res_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_prs_qcl_info_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, nr_dl_prs_res_id_r17, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, dl_prs_seq_id_r17, (uint16_t)0u, (uint16_t)4095u));
  HANDLE_CODE(dl_prs_comb_size_n_and_re_offset_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, dl_prs_res_slot_offset_r17, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(pack_integer(bref, dl_prs_res_symbol_offset_r17, (uint8_t)0u, (uint8_t)12u));
  if (dl_prs_qcl_info_r17_present) {
    HANDLE_CODE(dl_prs_qcl_info_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_res_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_prs_qcl_info_r17_present, 1));

  HANDLE_CODE(unpack_integer(nr_dl_prs_res_id_r17, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(dl_prs_seq_id_r17, bref, (uint16_t)0u, (uint16_t)4095u));
  HANDLE_CODE(dl_prs_comb_size_n_and_re_offset_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(dl_prs_res_slot_offset_r17, bref, (uint16_t)0u, (uint16_t)511u));
  HANDLE_CODE(unpack_integer(dl_prs_res_symbol_offset_r17, bref, (uint8_t)0u, (uint8_t)12u));
  if (dl_prs_qcl_info_r17_present) {
    HANDLE_CODE(dl_prs_qcl_info_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nr_dl_prs_res_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nr-DL-PRS-ResourceID-r17", nr_dl_prs_res_id_r17);
  j.write_int("dl-PRS-SequenceID-r17", dl_prs_seq_id_r17);
  j.write_fieldname("dl-PRS-CombSizeN-AndReOffset-r17");
  dl_prs_comb_size_n_and_re_offset_r17.to_json(j);
  j.write_int("dl-PRS-ResourceSlotOffset-r17", dl_prs_res_slot_offset_r17);
  j.write_int("dl-PRS-ResourceSymbolOffset-r17", dl_prs_res_symbol_offset_r17);
  if (dl_prs_qcl_info_r17_present) {
    j.write_fieldname("dl-PRS-QCL-Info-r17");
    dl_prs_qcl_info_r17.to_json(j);
  }
  j.end_obj();
}

void nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::destroy_() {}
void nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::dl_prs_comb_size_n_and_re_offset_r17_c_(
    const nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n2_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n4_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n6_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::n12_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_");
  }
}
nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_&
nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::operator=(
    const nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n2_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n4_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n6_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::n12_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_");
  }

  return *this;
}
uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n2_r17()
{
  set(types::n2_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n4_r17()
{
  set(types::n4_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n6_r17()
{
  set(types::n6_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n12_r17()
{
  set(types::n12_r17);
  return c.get<uint8_t>();
}
void nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n2_r17:
      j.write_int("n2-r17", c.get<uint8_t>());
      break;
    case types::n4_r17:
      j.write_int("n4-r17", c.get<uint8_t>());
      break;
    case types::n6_r17:
      j.write_int("n6-r17", c.get<uint8_t>());
      break;
    case types::n12_r17:
      j.write_int("n12-r17", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n2_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::n4_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::n6_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)5u));
      break;
    case types::n12_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)11u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n2_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::n4_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::n6_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)5u));
      break;
    case types::n12_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)11u));
      break;
    default:
      log_invalid_choice_id(type_, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"n2-r17", "n4-r17", "n6-r17", "n12-r17"};
  return convert_enum_idx(names, 4, value, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types");
}
uint8_t nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12};
  return map_enum_number(numbers, 4, value, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types");
}

// P0AlphaSet-r17 ::= SEQUENCE
SRSASN_CODE p0_alpha_set_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p0_r17_present, 1));
  HANDLE_CODE(bref.pack(alpha_r17_present, 1));

  if (p0_r17_present) {
    HANDLE_CODE(pack_integer(bref, p0_r17, (int8_t)-16, (int8_t)15));
  }
  if (alpha_r17_present) {
    HANDLE_CODE(alpha_r17.pack(bref));
  }
  HANDLE_CODE(closed_loop_idx_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE p0_alpha_set_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p0_r17_present, 1));
  HANDLE_CODE(bref.unpack(alpha_r17_present, 1));

  if (p0_r17_present) {
    HANDLE_CODE(unpack_integer(p0_r17, bref, (int8_t)-16, (int8_t)15));
  }
  if (alpha_r17_present) {
    HANDLE_CODE(alpha_r17.unpack(bref));
  }
  HANDLE_CODE(closed_loop_idx_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void p0_alpha_set_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (p0_r17_present) {
    j.write_int("p0-r17", p0_r17);
  }
  if (alpha_r17_present) {
    j.write_str("alpha-r17", alpha_r17.to_string());
  }
  j.write_str("closedLoopIndex-r17", closed_loop_idx_r17.to_string());
  j.end_obj();
}

const char* p0_alpha_set_r17_s::closed_loop_idx_r17_opts::to_string() const
{
  static const char* names[] = {"i0", "i1"};
  return convert_enum_idx(names, 2, value, "p0_alpha_set_r17_s::closed_loop_idx_r17_e_");
}
uint8_t p0_alpha_set_r17_s::closed_loop_idx_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "p0_alpha_set_r17_s::closed_loop_idx_r17_e_");
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

// RepFactorAndTimeGap-r17 ::= SEQUENCE
SRSASN_CODE rep_factor_and_time_gap_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(repeat_factor_r17.pack(bref));
  HANDLE_CODE(time_gap_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rep_factor_and_time_gap_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(repeat_factor_r17.unpack(bref));
  HANDLE_CODE(time_gap_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void rep_factor_and_time_gap_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("repetitionFactor-r17", repeat_factor_r17.to_string());
  j.write_str("timeGap-r17", time_gap_r17.to_string());
  j.end_obj();
}

const char* rep_factor_and_time_gap_r17_s::repeat_factor_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n6", "n8", "n16", "n32", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "rep_factor_and_time_gap_r17_s::repeat_factor_r17_e_");
}
uint8_t rep_factor_and_time_gap_r17_s::repeat_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "rep_factor_and_time_gap_r17_s::repeat_factor_r17_e_");
}

const char* rep_factor_and_time_gap_r17_s::time_gap_r17_opts::to_string() const
{
  static const char* names[] = {"s1", "s2", "s4", "s8", "s16", "s32", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "rep_factor_and_time_gap_r17_s::time_gap_r17_e_");
}
uint8_t rep_factor_and_time_gap_r17_s::time_gap_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "rep_factor_and_time_gap_r17_s::time_gap_r17_e_");
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

// SearchSpaceSwitchTrigger-r16 ::= SEQUENCE
SRSASN_CODE search_space_switch_trigger_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serving_cell_id_r16, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, position_in_dci_r16, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE search_space_switch_trigger_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serving_cell_id_r16, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(position_in_dci_r16, bref, (uint8_t)0u, (uint8_t)127u));

  return SRSASN_SUCCESS;
}
void search_space_switch_trigger_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servingCellId-r16", serving_cell_id_r16);
  j.write_int("positionInDCI-r16", position_in_dci_r16);
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= enable_cfg_ul_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(enable_cfg_ul_r16_present, 1));
    }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(enable_cfg_ul_r16_present, 1));
    }
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
  if (ext) {
    if (enable_cfg_ul_r16_present) {
      j.write_str("enableConfiguredUL-r16", "enabled");
    }
  }
  j.end_obj();
}

// GuardBand-r16 ::= SEQUENCE
SRSASN_CODE guard_band_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, start_crb_r16, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(pack_integer(bref, nrof_c_rbs_r16, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE guard_band_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(start_crb_r16, bref, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(unpack_integer(nrof_c_rbs_r16, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void guard_band_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("startCRB-r16", start_crb_r16);
  j.write_int("nrofCRBs-r16", nrof_c_rbs_r16);
  j.end_obj();
}

// LTE-NeighCellsCRS-AssistInfo-r17 ::= SEQUENCE
SRSASN_CODE lte_neigh_cells_crs_assist_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(neigh_carrier_bw_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(neigh_carrier_freq_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(neigh_cell_id_r17_present, 1));
  HANDLE_CODE(bref.pack(neigh_crs_muting_r17_present, 1));
  HANDLE_CODE(bref.pack(neigh_mbsfn_sf_cfg_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(neigh_nrof_crs_ports_r17_present, 1));
  HANDLE_CODE(bref.pack(neigh_v_shift_r17_present, 1));

  if (neigh_carrier_bw_dl_r17_present) {
    HANDLE_CODE(neigh_carrier_bw_dl_r17.pack(bref));
  }
  if (neigh_carrier_freq_dl_r17_present) {
    HANDLE_CODE(pack_integer(bref, neigh_carrier_freq_dl_r17, (uint16_t)0u, (uint16_t)16383u));
  }
  if (neigh_cell_id_r17_present) {
    HANDLE_CODE(pack_integer(bref, neigh_cell_id_r17, (uint16_t)0u, (uint16_t)503u));
  }
  if (neigh_mbsfn_sf_cfg_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, neigh_mbsfn_sf_cfg_list_r17, 1, 8));
  }
  if (neigh_nrof_crs_ports_r17_present) {
    HANDLE_CODE(neigh_nrof_crs_ports_r17.pack(bref));
  }
  if (neigh_v_shift_r17_present) {
    HANDLE_CODE(neigh_v_shift_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE lte_neigh_cells_crs_assist_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(neigh_carrier_bw_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(neigh_carrier_freq_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(neigh_cell_id_r17_present, 1));
  HANDLE_CODE(bref.unpack(neigh_crs_muting_r17_present, 1));
  bool neigh_mbsfn_sf_cfg_list_r17_present;
  HANDLE_CODE(bref.unpack(neigh_mbsfn_sf_cfg_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(neigh_nrof_crs_ports_r17_present, 1));
  HANDLE_CODE(bref.unpack(neigh_v_shift_r17_present, 1));

  if (neigh_carrier_bw_dl_r17_present) {
    HANDLE_CODE(neigh_carrier_bw_dl_r17.unpack(bref));
  }
  if (neigh_carrier_freq_dl_r17_present) {
    HANDLE_CODE(unpack_integer(neigh_carrier_freq_dl_r17, bref, (uint16_t)0u, (uint16_t)16383u));
  }
  if (neigh_cell_id_r17_present) {
    HANDLE_CODE(unpack_integer(neigh_cell_id_r17, bref, (uint16_t)0u, (uint16_t)503u));
  }
  if (neigh_mbsfn_sf_cfg_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(neigh_mbsfn_sf_cfg_list_r17, bref, 1, 8));
  }
  if (neigh_nrof_crs_ports_r17_present) {
    HANDLE_CODE(neigh_nrof_crs_ports_r17.unpack(bref));
  }
  if (neigh_v_shift_r17_present) {
    HANDLE_CODE(neigh_v_shift_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void lte_neigh_cells_crs_assist_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (neigh_carrier_bw_dl_r17_present) {
    j.write_str("neighCarrierBandwidthDL-r17", neigh_carrier_bw_dl_r17.to_string());
  }
  if (neigh_carrier_freq_dl_r17_present) {
    j.write_int("neighCarrierFreqDL-r17", neigh_carrier_freq_dl_r17);
  }
  if (neigh_cell_id_r17_present) {
    j.write_int("neighCellId-r17", neigh_cell_id_r17);
  }
  if (neigh_crs_muting_r17_present) {
    j.write_str("neighCRS-muting-r17", "enabled");
  }
  if (neigh_mbsfn_sf_cfg_list_r17.size() > 0) {
    j.start_array("neighMBSFN-SubframeConfigList-r17");
    for (const auto& e1 : neigh_mbsfn_sf_cfg_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (neigh_nrof_crs_ports_r17_present) {
    j.write_str("neighNrofCRS-Ports-r17", neigh_nrof_crs_ports_r17.to_string());
  }
  if (neigh_v_shift_r17_present) {
    j.write_str("neighV-Shift-r17", neigh_v_shift_r17.to_string());
  }
  j.end_obj();
}

const char* lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_opts::to_string() const
{
  static const char* names[] = {"n6", "n15", "n25", "n50", "n75", "n100", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_e_");
}
uint8_t lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_e_");
}

const char* lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_e_");
}
uint8_t lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_e_");
}

const char* lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_opts::to_string() const
{
  static const char* names[] = {"n0", "n1", "n2", "n3", "n4", "n5"};
  return convert_enum_idx(names, 6, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_e_");
}
uint8_t lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5};
  return map_enum_number(numbers, 6, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_e_");
}

// NR-DL-PRS-PDC-ResourceSet-r17 ::= SEQUENCE
SRSASN_CODE nr_dl_prs_pdc_res_set_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rep_factor_and_time_gap_r17_present, 1));

  HANDLE_CODE(periodicity_and_offset_r17.pack(bref));
  HANDLE_CODE(num_symbols_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, dl_prs_res_bw_r17, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, dl_prs_start_prb_r17, (uint16_t)0u, (uint16_t)2176u));
  HANDLE_CODE(pack_dyn_seq_of(bref, res_list_r17, 1, 64));
  if (rep_factor_and_time_gap_r17_present) {
    HANDLE_CODE(rep_factor_and_time_gap_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_pdc_res_set_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rep_factor_and_time_gap_r17_present, 1));

  HANDLE_CODE(periodicity_and_offset_r17.unpack(bref));
  HANDLE_CODE(num_symbols_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(dl_prs_res_bw_r17, bref, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(dl_prs_start_prb_r17, bref, (uint16_t)0u, (uint16_t)2176u));
  HANDLE_CODE(unpack_dyn_seq_of(res_list_r17, bref, 1, 64));
  if (rep_factor_and_time_gap_r17_present) {
    HANDLE_CODE(rep_factor_and_time_gap_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nr_dl_prs_pdc_res_set_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityAndOffset-r17");
  periodicity_and_offset_r17.to_json(j);
  j.write_str("numSymbols-r17", num_symbols_r17.to_string());
  j.write_int("dl-PRS-ResourceBandwidth-r17", dl_prs_res_bw_r17);
  j.write_int("dl-PRS-StartPRB-r17", dl_prs_start_prb_r17);
  j.start_array("resourceList-r17");
  for (const auto& e1 : res_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (rep_factor_and_time_gap_r17_present) {
    j.write_fieldname("repFactorAndTimeGap-r17");
    rep_factor_and_time_gap_r17.to_json(j);
  }
  j.end_obj();
}

const char* nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n6", "n12", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_e_");
}
uint8_t nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12};
  return map_enum_number(numbers, 4, value, "nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_e_");
}

// OutsideActiveTimeConfig-r16 ::= SEQUENCE
SRSASN_CODE outside_active_time_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(first_outside_active_time_bwp_id_r16_present, 1));
  HANDLE_CODE(bref.pack(dormancy_group_outside_active_time_r16_present, 1));

  if (first_outside_active_time_bwp_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, first_outside_active_time_bwp_id_r16, (uint8_t)0u, (uint8_t)4u));
  }
  if (dormancy_group_outside_active_time_r16_present) {
    HANDLE_CODE(pack_integer(bref, dormancy_group_outside_active_time_r16, (uint8_t)0u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE outside_active_time_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(first_outside_active_time_bwp_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(dormancy_group_outside_active_time_r16_present, 1));

  if (first_outside_active_time_bwp_id_r16_present) {
    HANDLE_CODE(unpack_integer(first_outside_active_time_bwp_id_r16, bref, (uint8_t)0u, (uint8_t)4u));
  }
  if (dormancy_group_outside_active_time_r16_present) {
    HANDLE_CODE(unpack_integer(dormancy_group_outside_active_time_r16, bref, (uint8_t)0u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
void outside_active_time_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (first_outside_active_time_bwp_id_r16_present) {
    j.write_int("firstOutsideActiveTimeBWP-Id-r16", first_outside_active_time_bwp_id_r16);
  }
  if (dormancy_group_outside_active_time_r16_present) {
    j.write_int("dormancyGroupOutsideActiveTime-r16", dormancy_group_outside_active_time_r16);
  }
  j.end_obj();
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
    group_flags[1] |= max_mimo_layers_dci_0_2_r16.is_present();
    group_flags[2] |= nrof_harq_processes_for_pusch_r17_present;
    group_flags[2] |= ul_harq_mode_r17.is_present();
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
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_mimo_layers_dci_0_2_r16.is_present(), 1));
      if (max_mimo_layers_dci_0_2_r16.is_present()) {
        HANDLE_CODE(max_mimo_layers_dci_0_2_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nrof_harq_processes_for_pusch_r17_present, 1));
      HANDLE_CODE(bref.pack(ul_harq_mode_r17.is_present(), 1));
      if (ul_harq_mode_r17.is_present()) {
        HANDLE_CODE(ul_harq_mode_r17->pack(bref));
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
    ext_groups_unpacker_guard group_flags(3);
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
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(max_mimo_layers_dci_0_2_r16, bref);
      if (max_mimo_layers_dci_0_2_r16.is_present()) {
        HANDLE_CODE(max_mimo_layers_dci_0_2_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(nrof_harq_processes_for_pusch_r17_present, 1));
      unpack_presence_flag(ul_harq_mode_r17, bref);
      if (ul_harq_mode_r17.is_present()) {
        HANDLE_CODE(ul_harq_mode_r17->unpack(bref));
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
    if (max_mimo_layers_dci_0_2_r16.is_present()) {
      j.write_fieldname("maxMIMO-LayersDCI-0-2-r16");
      max_mimo_layers_dci_0_2_r16->to_json(j);
    }
    if (nrof_harq_processes_for_pusch_r17_present) {
      j.write_str("nrofHARQ-ProcessesForPUSCH-r17", "n32");
    }
    if (ul_harq_mode_r17.is_present()) {
      j.write_fieldname("uplinkHARQ-mode-r17");
      ul_harq_mode_r17->to_json(j);
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

const char* srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types_opts::to_string() const
{
  static const char* names[] = {"typeA", "typeB"};
  return convert_enum_idx(names, 2, value, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types");
}

// SSB-MTC-AdditionalPCI-r17 ::= SEQUENCE
SRSASN_CODE ssb_mtc_add_pci_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, add_pci_idx_r17, (uint8_t)1u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, add_pci_r17, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(periodicity_r17.pack(bref));
  HANDLE_CODE(ssb_positions_in_burst_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, ss_pbch_block_pwr_r17, (int8_t)-60, (int8_t)50));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc_add_pci_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(add_pci_idx_r17, bref, (uint8_t)1u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(add_pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(periodicity_r17.unpack(bref));
  HANDLE_CODE(ssb_positions_in_burst_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(ss_pbch_block_pwr_r17, bref, (int8_t)-60, (int8_t)50));

  return SRSASN_SUCCESS;
}
void ssb_mtc_add_pci_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("additionalPCIIndex-r17", add_pci_idx_r17);
  j.write_int("additionalPCI-r17", add_pci_r17);
  j.write_str("periodicity-r17", periodicity_r17.to_string());
  j.write_fieldname("ssb-PositionsInBurst-r17");
  ssb_positions_in_burst_r17.to_json(j);
  j.write_int("ss-PBCH-BlockPower-r17", ss_pbch_block_pwr_r17);
  j.end_obj();
}

const char* ssb_mtc_add_pci_r17_s::periodicity_r17_opts::to_string() const
{
  static const char* names[] = {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ssb_mtc_add_pci_r17_s::periodicity_r17_e_");
}
uint8_t ssb_mtc_add_pci_r17_s::periodicity_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "ssb_mtc_add_pci_r17_s::periodicity_r17_e_");
}

void ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::destroy_()
{
  switch (type_) {
    case types::short_bitmap:
      c.destroy<fixed_bitstring<4>>();
      break;
    case types::medium_bitmap:
      c.destroy<fixed_bitstring<8>>();
      break;
    case types::long_bitmap:
      c.destroy<fixed_bitstring<64>>();
      break;
    default:
      break;
  }
}
void ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::short_bitmap:
      c.init<fixed_bitstring<4>>();
      break;
    case types::medium_bitmap:
      c.init<fixed_bitstring<8>>();
      break;
    case types::long_bitmap:
      c.init<fixed_bitstring<64>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_");
  }
}
ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::ssb_positions_in_burst_r17_c_(
    const ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::short_bitmap:
      c.init(other.c.get<fixed_bitstring<4>>());
      break;
    case types::medium_bitmap:
      c.init(other.c.get<fixed_bitstring<8>>());
      break;
    case types::long_bitmap:
      c.init(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_");
  }
}
ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_& ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::operator=(
    const ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::short_bitmap:
      c.set(other.c.get<fixed_bitstring<4>>());
      break;
    case types::medium_bitmap:
      c.set(other.c.get<fixed_bitstring<8>>());
      break;
    case types::long_bitmap:
      c.set(other.c.get<fixed_bitstring<64>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_");
  }

  return *this;
}
fixed_bitstring<4>& ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::set_short_bitmap()
{
  set(types::short_bitmap);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>& ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64>>();
}
void ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::short_bitmap:
      j.write_str("shortBitmap", c.get<fixed_bitstring<4>>().to_string());
      break;
    case types::medium_bitmap:
      j.write_str("mediumBitmap", c.get<fixed_bitstring<8>>().to_string());
      break;
    case types::long_bitmap:
      j.write_str("longBitmap", c.get<fixed_bitstring<64>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::short_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().pack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().pack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::short_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<4>>().unpack(bref));
      break;
    case types::medium_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<8>>().unpack(bref));
      break;
    case types::long_bitmap:
      HANDLE_CODE(c.get<fixed_bitstring<64>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
  return convert_enum_idx(names, 3, value, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types");
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

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= available_rb_sets_to_add_mod_list_r16.is_present();
    group_flags[0] |= available_rb_sets_to_release_list_r16.is_present();
    group_flags[0] |= switch_trigger_to_add_mod_list_r16.is_present();
    group_flags[0] |= switch_trigger_to_release_list_r16.is_present();
    group_flags[0] |= co_durs_per_cell_to_add_mod_list_r16.is_present();
    group_flags[0] |= co_durs_per_cell_to_release_list_r16.is_present();
    group_flags[1] |= switch_trigger_to_add_mod_list_size_ext_r16.is_present();
    group_flags[1] |= switch_trigger_to_release_list_size_ext_r16.is_present();
    group_flags[2] |= co_durs_per_cell_to_add_mod_list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(available_rb_sets_to_add_mod_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(available_rb_sets_to_release_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(switch_trigger_to_add_mod_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(switch_trigger_to_release_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(co_durs_per_cell_to_add_mod_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(co_durs_per_cell_to_release_list_r16.is_present(), 1));
      if (available_rb_sets_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *available_rb_sets_to_add_mod_list_r16, 1, 16));
      }
      if (available_rb_sets_to_release_list_r16.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *available_rb_sets_to_release_list_r16, 1, 16, integer_packer<uint8_t>(0, 31)));
      }
      if (switch_trigger_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *switch_trigger_to_add_mod_list_r16, 1, 4));
      }
      if (switch_trigger_to_release_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *switch_trigger_to_release_list_r16, 1, 4, integer_packer<uint8_t>(0, 31)));
      }
      if (co_durs_per_cell_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *co_durs_per_cell_to_add_mod_list_r16, 1, 16));
      }
      if (co_durs_per_cell_to_release_list_r16.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *co_durs_per_cell_to_release_list_r16, 1, 16, integer_packer<uint8_t>(0, 31)));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(switch_trigger_to_add_mod_list_size_ext_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(switch_trigger_to_release_list_size_ext_r16.is_present(), 1));
      if (switch_trigger_to_add_mod_list_size_ext_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *switch_trigger_to_add_mod_list_size_ext_r16, 1, 12));
      }
      if (switch_trigger_to_release_list_size_ext_r16.is_present()) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, *switch_trigger_to_release_list_size_ext_r16, 1, 12, integer_packer<uint8_t>(0, 31)));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(co_durs_per_cell_to_add_mod_list_r17.is_present(), 1));
      if (co_durs_per_cell_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *co_durs_per_cell_to_add_mod_list_r17, 1, 16));
      }
    }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(available_rb_sets_to_add_mod_list_r16, bref);
      unpack_presence_flag(available_rb_sets_to_release_list_r16, bref);
      unpack_presence_flag(switch_trigger_to_add_mod_list_r16, bref);
      unpack_presence_flag(switch_trigger_to_release_list_r16, bref);
      unpack_presence_flag(co_durs_per_cell_to_add_mod_list_r16, bref);
      unpack_presence_flag(co_durs_per_cell_to_release_list_r16, bref);
      if (available_rb_sets_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*available_rb_sets_to_add_mod_list_r16, bref, 1, 16));
      }
      if (available_rb_sets_to_release_list_r16.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*available_rb_sets_to_release_list_r16, bref, 1, 16, integer_packer<uint8_t>(0, 31)));
      }
      if (switch_trigger_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*switch_trigger_to_add_mod_list_r16, bref, 1, 4));
      }
      if (switch_trigger_to_release_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*switch_trigger_to_release_list_r16, bref, 1, 4, integer_packer<uint8_t>(0, 31)));
      }
      if (co_durs_per_cell_to_add_mod_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*co_durs_per_cell_to_add_mod_list_r16, bref, 1, 16));
      }
      if (co_durs_per_cell_to_release_list_r16.is_present()) {
        HANDLE_CODE(
            unpack_dyn_seq_of(*co_durs_per_cell_to_release_list_r16, bref, 1, 16, integer_packer<uint8_t>(0, 31)));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(switch_trigger_to_add_mod_list_size_ext_r16, bref);
      unpack_presence_flag(switch_trigger_to_release_list_size_ext_r16, bref);
      if (switch_trigger_to_add_mod_list_size_ext_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*switch_trigger_to_add_mod_list_size_ext_r16, bref, 1, 12));
      }
      if (switch_trigger_to_release_list_size_ext_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(
            *switch_trigger_to_release_list_size_ext_r16, bref, 1, 12, integer_packer<uint8_t>(0, 31)));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(co_durs_per_cell_to_add_mod_list_r17, bref);
      if (co_durs_per_cell_to_add_mod_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*co_durs_per_cell_to_add_mod_list_r17, bref, 1, 16));
      }
    }
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
  if (ext) {
    if (available_rb_sets_to_add_mod_list_r16.is_present()) {
      j.start_array("availableRB-SetsToAddModList-r16");
      for (const auto& e1 : *available_rb_sets_to_add_mod_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (available_rb_sets_to_release_list_r16.is_present()) {
      j.start_array("availableRB-SetsToReleaseList-r16");
      for (const auto& e1 : *available_rb_sets_to_release_list_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (switch_trigger_to_add_mod_list_r16.is_present()) {
      j.start_array("switchTriggerToAddModList-r16");
      for (const auto& e1 : *switch_trigger_to_add_mod_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (switch_trigger_to_release_list_r16.is_present()) {
      j.start_array("switchTriggerToReleaseList-r16");
      for (const auto& e1 : *switch_trigger_to_release_list_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (co_durs_per_cell_to_add_mod_list_r16.is_present()) {
      j.start_array("co-DurationsPerCellToAddModList-r16");
      for (const auto& e1 : *co_durs_per_cell_to_add_mod_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (co_durs_per_cell_to_release_list_r16.is_present()) {
      j.start_array("co-DurationsPerCellToReleaseList-r16");
      for (const auto& e1 : *co_durs_per_cell_to_release_list_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (switch_trigger_to_add_mod_list_size_ext_r16.is_present()) {
      j.start_array("switchTriggerToAddModListSizeExt-r16");
      for (const auto& e1 : *switch_trigger_to_add_mod_list_size_ext_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (switch_trigger_to_release_list_size_ext_r16.is_present()) {
      j.start_array("switchTriggerToReleaseListSizeExt-r16");
      for (const auto& e1 : *switch_trigger_to_release_list_size_ext_r16) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (co_durs_per_cell_to_add_mod_list_r17.is_present()) {
      j.start_array("co-DurationsPerCellToAddModList-r17");
      for (const auto& e1 : *co_durs_per_cell_to_add_mod_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
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

const char* tdd_ul_dl_slot_cfg_s::symbols_c_::types_opts::to_string() const
{
  static const char* names[] = {"allDownlink", "allUplink", "explicit"};
  return convert_enum_idx(names, 3, value, "tdd_ul_dl_slot_cfg_s::symbols_c_::types");
}

// TDD-UL-DL-SlotConfig-IAB-MT-r16 ::= SEQUENCE
SRSASN_CODE tdd_ul_dl_slot_cfg_iab_mt_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, slot_idx_r16, (uint16_t)0u, (uint16_t)319u));
  HANDLE_CODE(symbols_iab_mt_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_slot_cfg_iab_mt_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(slot_idx_r16, bref, (uint16_t)0u, (uint16_t)319u));
  HANDLE_CODE(symbols_iab_mt_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void tdd_ul_dl_slot_cfg_iab_mt_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("slotIndex-r16", slot_idx_r16);
  j.write_fieldname("symbols-IAB-MT-r16");
  symbols_iab_mt_r16.to_json(j);
  j.end_obj();
}

void tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::destroy_()
{
  switch (type_) {
    case types::explicit_r16:
      c.destroy<explicit_r16_s_>();
      break;
    case types::explicit_iab_mt_r16:
      c.destroy<explicit_iab_mt_r16_s_>();
      break;
    default:
      break;
  }
}
void tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::all_dl_r16:
      break;
    case types::all_ul_r16:
      break;
    case types::explicit_r16:
      c.init<explicit_r16_s_>();
      break;
    case types::explicit_iab_mt_r16:
      c.init<explicit_iab_mt_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_");
  }
}
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::symbols_iab_mt_r16_c_(
    const tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::all_dl_r16:
      break;
    case types::all_ul_r16:
      break;
    case types::explicit_r16:
      c.init(other.c.get<explicit_r16_s_>());
      break;
    case types::explicit_iab_mt_r16:
      c.init(other.c.get<explicit_iab_mt_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_");
  }
}
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_&
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::operator=(
    const tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::all_dl_r16:
      break;
    case types::all_ul_r16:
      break;
    case types::explicit_r16:
      c.set(other.c.get<explicit_r16_s_>());
      break;
    case types::explicit_iab_mt_r16:
      c.set(other.c.get<explicit_iab_mt_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_");
  }

  return *this;
}
void tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_all_dl_r16()
{
  set(types::all_dl_r16);
}
void tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_all_ul_r16()
{
  set(types::all_ul_r16);
}
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_&
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_explicit_r16()
{
  set(types::explicit_r16);
  return c.get<explicit_r16_s_>();
}
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_&
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_explicit_iab_mt_r16()
{
  set(types::explicit_iab_mt_r16);
  return c.get<explicit_iab_mt_r16_s_>();
}
void tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::all_dl_r16:
      break;
    case types::all_ul_r16:
      break;
    case types::explicit_r16:
      j.write_fieldname("explicit-r16");
      j.start_obj();
      if (c.get<explicit_r16_s_>().nrof_dl_symbols_r16_present) {
        j.write_int("nrofDownlinkSymbols-r16", c.get<explicit_r16_s_>().nrof_dl_symbols_r16);
      }
      if (c.get<explicit_r16_s_>().nrof_ul_symbols_r16_present) {
        j.write_int("nrofUplinkSymbols-r16", c.get<explicit_r16_s_>().nrof_ul_symbols_r16);
      }
      j.end_obj();
      break;
    case types::explicit_iab_mt_r16:
      j.write_fieldname("explicit-IAB-MT-r16");
      j.start_obj();
      if (c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16_present) {
        j.write_int("nrofDownlinkSymbols-r16", c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16);
      }
      if (c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16_present) {
        j.write_int("nrofUplinkSymbols-r16", c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::all_dl_r16:
      break;
    case types::all_ul_r16:
      break;
    case types::explicit_r16:
      HANDLE_CODE(bref.pack(c.get<explicit_r16_s_>().nrof_dl_symbols_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<explicit_r16_s_>().nrof_ul_symbols_r16_present, 1));
      if (c.get<explicit_r16_s_>().nrof_dl_symbols_r16_present) {
        HANDLE_CODE(pack_integer(bref, c.get<explicit_r16_s_>().nrof_dl_symbols_r16, (uint8_t)1u, (uint8_t)13u));
      }
      if (c.get<explicit_r16_s_>().nrof_ul_symbols_r16_present) {
        HANDLE_CODE(pack_integer(bref, c.get<explicit_r16_s_>().nrof_ul_symbols_r16, (uint8_t)1u, (uint8_t)13u));
      }
      break;
    case types::explicit_iab_mt_r16:
      HANDLE_CODE(bref.pack(c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16_present, 1));
      HANDLE_CODE(bref.pack(c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16_present, 1));
      if (c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16_present) {
        HANDLE_CODE(pack_integer(bref, c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16, (uint8_t)1u, (uint8_t)13u));
      }
      if (c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16_present) {
        HANDLE_CODE(pack_integer(bref, c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16, (uint8_t)1u, (uint8_t)13u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::all_dl_r16:
      break;
    case types::all_ul_r16:
      break;
    case types::explicit_r16:
      HANDLE_CODE(bref.unpack(c.get<explicit_r16_s_>().nrof_dl_symbols_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<explicit_r16_s_>().nrof_ul_symbols_r16_present, 1));
      if (c.get<explicit_r16_s_>().nrof_dl_symbols_r16_present) {
        HANDLE_CODE(unpack_integer(c.get<explicit_r16_s_>().nrof_dl_symbols_r16, bref, (uint8_t)1u, (uint8_t)13u));
      }
      if (c.get<explicit_r16_s_>().nrof_ul_symbols_r16_present) {
        HANDLE_CODE(unpack_integer(c.get<explicit_r16_s_>().nrof_ul_symbols_r16, bref, (uint8_t)1u, (uint8_t)13u));
      }
      break;
    case types::explicit_iab_mt_r16:
      HANDLE_CODE(bref.unpack(c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16_present, 1));
      HANDLE_CODE(bref.unpack(c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16_present, 1));
      if (c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16_present) {
        HANDLE_CODE(
            unpack_integer(c.get<explicit_iab_mt_r16_s_>().nrof_dl_symbols_r16, bref, (uint8_t)1u, (uint8_t)13u));
      }
      if (c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16_present) {
        HANDLE_CODE(
            unpack_integer(c.get<explicit_iab_mt_r16_s_>().nrof_ul_symbols_r16, bref, (uint8_t)1u, (uint8_t)13u));
      }
      break;
    default:
      log_invalid_choice_id(type_, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"allDownlink-r16", "allUplink-r16", "explicit-r16", "explicit-IAB-MT-r16"};
  return convert_enum_idx(names, 4, value, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types");
}

// Uplink-powerControl-r17 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p0_alpha_setfor_pusch_r17_present, 1));
  HANDLE_CODE(bref.pack(p0_alpha_setfor_pucch_r17_present, 1));
  HANDLE_CODE(bref.pack(p0_alpha_setfor_srs_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, ul_pwrctrl_id_r17, (uint8_t)1u, (uint8_t)64u));
  if (p0_alpha_setfor_pusch_r17_present) {
    HANDLE_CODE(p0_alpha_setfor_pusch_r17.pack(bref));
  }
  if (p0_alpha_setfor_pucch_r17_present) {
    HANDLE_CODE(p0_alpha_setfor_pucch_r17.pack(bref));
  }
  if (p0_alpha_setfor_srs_r17_present) {
    HANDLE_CODE(p0_alpha_setfor_srs_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p0_alpha_setfor_pusch_r17_present, 1));
  HANDLE_CODE(bref.unpack(p0_alpha_setfor_pucch_r17_present, 1));
  HANDLE_CODE(bref.unpack(p0_alpha_setfor_srs_r17_present, 1));

  HANDLE_CODE(unpack_integer(ul_pwrctrl_id_r17, bref, (uint8_t)1u, (uint8_t)64u));
  if (p0_alpha_setfor_pusch_r17_present) {
    HANDLE_CODE(p0_alpha_setfor_pusch_r17.unpack(bref));
  }
  if (p0_alpha_setfor_pucch_r17_present) {
    HANDLE_CODE(p0_alpha_setfor_pucch_r17.unpack(bref));
  }
  if (p0_alpha_setfor_srs_r17_present) {
    HANDLE_CODE(p0_alpha_setfor_srs_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ul-powercontrolId-r17", ul_pwrctrl_id_r17);
  if (p0_alpha_setfor_pusch_r17_present) {
    j.write_fieldname("p0AlphaSetforPUSCH-r17");
    p0_alpha_setfor_pusch_r17.to_json(j);
  }
  if (p0_alpha_setfor_pucch_r17_present) {
    j.write_fieldname("p0AlphaSetforPUCCH-r17");
    p0_alpha_setfor_pucch_r17.to_json(j);
  }
  if (p0_alpha_setfor_srs_r17_present) {
    j.write_fieldname("p0AlphaSetforSRS-r17");
    p0_alpha_setfor_srs_r17.to_json(j);
  }
  j.end_obj();
}

// UplinkTxSwitching-r16 ::= SEQUENCE
SRSASN_CODE ul_tx_switching_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_tx_switching_period_location_r16, 1));
  HANDLE_CODE(ul_tx_switching_carrier_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_tx_switching_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_tx_switching_period_location_r16, 1));
  HANDLE_CODE(ul_tx_switching_carrier_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_tx_switching_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("uplinkTxSwitchingPeriodLocation-r16", ul_tx_switching_period_location_r16);
  j.write_str("uplinkTxSwitchingCarrier-r16", ul_tx_switching_carrier_r16.to_string());
  j.end_obj();
}

const char* ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_opts::to_string() const
{
  static const char* names[] = {"carrier1", "carrier2"};
  return convert_enum_idx(names, 2, value, "ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_e_");
}
uint8_t ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_e_");
}

// WithinActiveTimeConfig-r16 ::= SEQUENCE
SRSASN_CODE within_active_time_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(first_within_active_time_bwp_id_r16_present, 1));
  HANDLE_CODE(bref.pack(dormancy_group_within_active_time_r16_present, 1));

  if (first_within_active_time_bwp_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, first_within_active_time_bwp_id_r16, (uint8_t)0u, (uint8_t)4u));
  }
  if (dormancy_group_within_active_time_r16_present) {
    HANDLE_CODE(pack_integer(bref, dormancy_group_within_active_time_r16, (uint8_t)0u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE within_active_time_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(first_within_active_time_bwp_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(dormancy_group_within_active_time_r16_present, 1));

  if (first_within_active_time_bwp_id_r16_present) {
    HANDLE_CODE(unpack_integer(first_within_active_time_bwp_id_r16, bref, (uint8_t)0u, (uint8_t)4u));
  }
  if (dormancy_group_within_active_time_r16_present) {
    HANDLE_CODE(unpack_integer(dormancy_group_within_active_time_r16, bref, (uint8_t)0u, (uint8_t)4u));
  }

  return SRSASN_SUCCESS;
}
void within_active_time_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (first_within_active_time_bwp_id_r16_present) {
    j.write_int("firstWithinActiveTimeBWP-Id-r16", first_within_active_time_bwp_id_r16);
  }
  if (dormancy_group_within_active_time_r16_present) {
    j.write_int("dormancyGroupWithinActiveTime-r16", dormancy_group_within_active_time_r16);
  }
  j.end_obj();
}

// ChannelAccessConfig-r16 ::= SEQUENCE
SRSASN_CODE ch_access_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(energy_detection_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(ul_to_dl_cot_sharing_ed_thres_r16_present, 1));
  HANDLE_CODE(bref.pack(absence_of_any_other_technology_r16_present, 1));

  if (energy_detection_cfg_r16_present) {
    HANDLE_CODE(energy_detection_cfg_r16.pack(bref));
  }
  if (ul_to_dl_cot_sharing_ed_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, ul_to_dl_cot_sharing_ed_thres_r16, (int8_t)-85, (int8_t)-52));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ch_access_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(energy_detection_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(ul_to_dl_cot_sharing_ed_thres_r16_present, 1));
  HANDLE_CODE(bref.unpack(absence_of_any_other_technology_r16_present, 1));

  if (energy_detection_cfg_r16_present) {
    HANDLE_CODE(energy_detection_cfg_r16.unpack(bref));
  }
  if (ul_to_dl_cot_sharing_ed_thres_r16_present) {
    HANDLE_CODE(unpack_integer(ul_to_dl_cot_sharing_ed_thres_r16, bref, (int8_t)-85, (int8_t)-52));
  }

  return SRSASN_SUCCESS;
}
void ch_access_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (energy_detection_cfg_r16_present) {
    j.write_fieldname("energyDetectionConfig-r16");
    energy_detection_cfg_r16.to_json(j);
  }
  if (ul_to_dl_cot_sharing_ed_thres_r16_present) {
    j.write_int("ul-toDL-COT-SharingED-Threshold-r16", ul_to_dl_cot_sharing_ed_thres_r16);
  }
  if (absence_of_any_other_technology_r16_present) {
    j.write_str("absenceOfAnyOtherTechnology-r16", "true");
  }
  j.end_obj();
}

void ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::destroy_() {}
void ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::energy_detection_cfg_r16_c_(
    const ch_access_cfg_r16_s::energy_detection_cfg_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::max_energy_detection_thres_r16:
      c.init(other.c.get<int8_t>());
      break;
    case types::energy_detection_thres_offset_r16:
      c.init(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_");
  }
}
ch_access_cfg_r16_s::energy_detection_cfg_r16_c_& ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::operator=(
    const ch_access_cfg_r16_s::energy_detection_cfg_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::max_energy_detection_thres_r16:
      c.set(other.c.get<int8_t>());
      break;
    case types::energy_detection_thres_offset_r16:
      c.set(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_");
  }

  return *this;
}
int8_t& ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::set_max_energy_detection_thres_r16()
{
  set(types::max_energy_detection_thres_r16);
  return c.get<int8_t>();
}
int8_t& ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::set_energy_detection_thres_offset_r16()
{
  set(types::energy_detection_thres_offset_r16);
  return c.get<int8_t>();
}
void ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::max_energy_detection_thres_r16:
      j.write_int("maxEnergyDetectionThreshold-r16", c.get<int8_t>());
      break;
    case types::energy_detection_thres_offset_r16:
      j.write_int("energyDetectionThresholdOffset-r16", c.get<int8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::max_energy_detection_thres_r16:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-85, (int8_t)-52));
      break;
    case types::energy_detection_thres_offset_r16:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-13, (int8_t)20));
      break;
    default:
      log_invalid_choice_id(type_, "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::max_energy_detection_thres_r16:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-85, (int8_t)-52));
      break;
    case types::energy_detection_thres_offset_r16:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-13, (int8_t)20));
      break;
    default:
      log_invalid_choice_id(type_, "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"maxEnergyDetectionThreshold-r16", "energyDetectionThresholdOffset-r16"};
  return convert_enum_idx(names, 2, value, "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types");
}

// CrossCarrierSchedulingConfig ::= SEQUENCE
SRSASN_CODE cross_carrier_sched_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sched_cell_info.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= carrier_ind_size_r16.is_present();
    group_flags[0] |= enable_default_beam_for_cc_s_r16_present;
    group_flags[1] |= ccs_blind_detection_split_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_ind_size_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(enable_default_beam_for_cc_s_r16_present, 1));
      if (carrier_ind_size_r16.is_present()) {
        HANDLE_CODE(pack_integer(bref, carrier_ind_size_r16->carrier_ind_size_dci_1_2_r16, (uint8_t)0u, (uint8_t)3u));
        HANDLE_CODE(pack_integer(bref, carrier_ind_size_r16->carrier_ind_size_dci_0_2_r16, (uint8_t)0u, (uint8_t)3u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ccs_blind_detection_split_r17_present, 1));
      if (ccs_blind_detection_split_r17_present) {
        HANDLE_CODE(ccs_blind_detection_split_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cross_carrier_sched_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sched_cell_info.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(carrier_ind_size_r16, bref);
      HANDLE_CODE(bref.unpack(enable_default_beam_for_cc_s_r16_present, 1));
      if (carrier_ind_size_r16.is_present()) {
        HANDLE_CODE(unpack_integer(carrier_ind_size_r16->carrier_ind_size_dci_1_2_r16, bref, (uint8_t)0u, (uint8_t)3u));
        HANDLE_CODE(unpack_integer(carrier_ind_size_r16->carrier_ind_size_dci_0_2_r16, bref, (uint8_t)0u, (uint8_t)3u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ccs_blind_detection_split_r17_present, 1));
      if (ccs_blind_detection_split_r17_present) {
        HANDLE_CODE(ccs_blind_detection_split_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cross_carrier_sched_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("schedulingCellInfo");
  sched_cell_info.to_json(j);
  if (ext) {
    if (carrier_ind_size_r16.is_present()) {
      j.write_fieldname("carrierIndicatorSize-r16");
      j.start_obj();
      j.write_int("carrierIndicatorSizeDCI-1-2-r16", carrier_ind_size_r16->carrier_ind_size_dci_1_2_r16);
      j.write_int("carrierIndicatorSizeDCI-0-2-r16", carrier_ind_size_r16->carrier_ind_size_dci_0_2_r16);
      j.end_obj();
    }
    if (enable_default_beam_for_cc_s_r16_present) {
      j.write_str("enableDefaultBeamForCCS-r16", "enabled");
    }
    if (ccs_blind_detection_split_r17_present) {
      j.write_str("ccs-BlindDetectionSplit-r17", ccs_blind_detection_split_r17.to_string());
    }
  }
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

const char* cross_carrier_sched_cfg_s::sched_cell_info_c_::types_opts::to_string() const
{
  static const char* names[] = {"own", "other"};
  return convert_enum_idx(names, 2, value, "cross_carrier_sched_cfg_s::sched_cell_info_c_::types");
}

const char* cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_opts::to_string() const
{
  static const char* names[] = {
      "oneSeventh", "threeFourteenth", "twoSeventh", "threeSeventh", "oneHalf", "fourSeventh", "fiveSeventh", "spare1"};
  return convert_enum_idx(names, 8, value, "cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_e_");
}
float cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_opts::to_number() const
{
  static const float numbers[] = {0.14285714285714285,
                                  0.21428571428571427,
                                  0.2857142857142857,
                                  0.42857142857142855,
                                  0.5,
                                  0.5714285714285714,
                                  0.7142857142857143};
  return map_enum_number(numbers, 7, value, "cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_e_");
}
const char* cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/7", "3/14", "2/7", "3/7", "1/2", "4/7", "5/7"};
  return convert_enum_idx(number_strs, 8, value, "cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_e_");
}

// DormantBWP-Config-r16 ::= SEQUENCE
SRSASN_CODE dormant_bwp_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dormant_bwp_id_r16_present, 1));
  HANDLE_CODE(bref.pack(within_active_time_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(outside_active_time_cfg_r16_present, 1));

  if (dormant_bwp_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, dormant_bwp_id_r16, (uint8_t)0u, (uint8_t)4u));
  }
  if (within_active_time_cfg_r16_present) {
    HANDLE_CODE(within_active_time_cfg_r16.pack(bref));
  }
  if (outside_active_time_cfg_r16_present) {
    HANDLE_CODE(outside_active_time_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dormant_bwp_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dormant_bwp_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(within_active_time_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(outside_active_time_cfg_r16_present, 1));

  if (dormant_bwp_id_r16_present) {
    HANDLE_CODE(unpack_integer(dormant_bwp_id_r16, bref, (uint8_t)0u, (uint8_t)4u));
  }
  if (within_active_time_cfg_r16_present) {
    HANDLE_CODE(within_active_time_cfg_r16.unpack(bref));
  }
  if (outside_active_time_cfg_r16_present) {
    HANDLE_CODE(outside_active_time_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void dormant_bwp_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dormant_bwp_id_r16_present) {
    j.write_int("dormantBWP-Id-r16", dormant_bwp_id_r16);
  }
  if (within_active_time_cfg_r16_present) {
    j.write_fieldname("withinActiveTimeConfig-r16");
    within_active_time_cfg_r16.to_json(j);
  }
  if (outside_active_time_cfg_r16_present) {
    j.write_fieldname("outsideActiveTimeConfig-r16");
    outside_active_time_cfg_r16.to_json(j);
  }
  j.end_obj();
}

// DummyJ ::= SEQUENCE
SRSASN_CODE dummy_j_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ul_to_dl_cot_sharing_ed_thres_r16_present, 1));
  HANDLE_CODE(bref.pack(absence_of_any_other_technology_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, max_energy_detection_thres_r16, (int8_t)-85, (int8_t)-52));
  HANDLE_CODE(pack_integer(bref, energy_detection_thres_offset_r16, (int8_t)-20, (int8_t)-13));
  if (ul_to_dl_cot_sharing_ed_thres_r16_present) {
    HANDLE_CODE(pack_integer(bref, ul_to_dl_cot_sharing_ed_thres_r16, (int8_t)-85, (int8_t)-52));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dummy_j_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ul_to_dl_cot_sharing_ed_thres_r16_present, 1));
  HANDLE_CODE(bref.unpack(absence_of_any_other_technology_r16_present, 1));

  HANDLE_CODE(unpack_integer(max_energy_detection_thres_r16, bref, (int8_t)-85, (int8_t)-52));
  HANDLE_CODE(unpack_integer(energy_detection_thres_offset_r16, bref, (int8_t)-20, (int8_t)-13));
  if (ul_to_dl_cot_sharing_ed_thres_r16_present) {
    HANDLE_CODE(unpack_integer(ul_to_dl_cot_sharing_ed_thres_r16, bref, (int8_t)-85, (int8_t)-52));
  }

  return SRSASN_SUCCESS;
}
void dummy_j_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxEnergyDetectionThreshold-r16", max_energy_detection_thres_r16);
  j.write_int("energyDetectionThresholdOffset-r16", energy_detection_thres_offset_r16);
  if (ul_to_dl_cot_sharing_ed_thres_r16_present) {
    j.write_int("ul-toDL-COT-SharingED-Threshold-r16", ul_to_dl_cot_sharing_ed_thres_r16);
  }
  if (absence_of_any_other_technology_r16_present) {
    j.write_str("absenceOfAnyOtherTechnology-r16", "true");
  }
  j.end_obj();
}

// IntraCellGuardBandsPerSCS-r16 ::= SEQUENCE
SRSASN_CODE intra_cell_guard_bands_per_scs_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(guard_band_scs_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, intra_cell_guard_bands_r16, 1, 4));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_cell_guard_bands_per_scs_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(guard_band_scs_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(intra_cell_guard_bands_r16, bref, 1, 4));

  return SRSASN_SUCCESS;
}
void intra_cell_guard_bands_per_scs_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("guardBandSCS-r16", guard_band_scs_r16.to_string());
  j.start_array("intraCellGuardBands-r16");
  for (const auto& e1 : intra_cell_guard_bands_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MIMOParam-r17 ::= SEQUENCE
SRSASN_CODE mimo_param_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_pci_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(add_pci_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(unified_tci_state_type_r17_present, 1));
  HANDLE_CODE(bref.pack(ul_pwr_ctrl_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(ul_pwr_ctrl_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(sfn_schem_epdcch_r17_present, 1));
  HANDLE_CODE(bref.pack(sfn_scheme_pdsch_r17_present, 1));

  if (add_pci_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, add_pci_to_add_mod_list_r17, 1, 7));
  }
  if (add_pci_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, add_pci_to_release_list_r17, 1, 7, integer_packer<uint8_t>(1, 7)));
  }
  if (unified_tci_state_type_r17_present) {
    HANDLE_CODE(unified_tci_state_type_r17.pack(bref));
  }
  if (ul_pwr_ctrl_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_pwr_ctrl_to_add_mod_list_r17, 1, 64));
  }
  if (ul_pwr_ctrl_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_pwr_ctrl_to_release_list_r17, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sfn_schem_epdcch_r17_present) {
    HANDLE_CODE(sfn_schem_epdcch_r17.pack(bref));
  }
  if (sfn_scheme_pdsch_r17_present) {
    HANDLE_CODE(sfn_scheme_pdsch_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_param_r17_s::unpack(cbit_ref& bref)
{
  bool add_pci_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(add_pci_to_add_mod_list_r17_present, 1));
  bool add_pci_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(add_pci_to_release_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(unified_tci_state_type_r17_present, 1));
  bool ul_pwr_ctrl_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(ul_pwr_ctrl_to_add_mod_list_r17_present, 1));
  bool ul_pwr_ctrl_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(ul_pwr_ctrl_to_release_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(sfn_schem_epdcch_r17_present, 1));
  HANDLE_CODE(bref.unpack(sfn_scheme_pdsch_r17_present, 1));

  if (add_pci_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(add_pci_to_add_mod_list_r17, bref, 1, 7));
  }
  if (add_pci_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(add_pci_to_release_list_r17, bref, 1, 7, integer_packer<uint8_t>(1, 7)));
  }
  if (unified_tci_state_type_r17_present) {
    HANDLE_CODE(unified_tci_state_type_r17.unpack(bref));
  }
  if (ul_pwr_ctrl_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_pwr_ctrl_to_add_mod_list_r17, bref, 1, 64));
  }
  if (ul_pwr_ctrl_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ul_pwr_ctrl_to_release_list_r17, bref, 1, 64, integer_packer<uint8_t>(1, 64)));
  }
  if (sfn_schem_epdcch_r17_present) {
    HANDLE_CODE(sfn_schem_epdcch_r17.unpack(bref));
  }
  if (sfn_scheme_pdsch_r17_present) {
    HANDLE_CODE(sfn_scheme_pdsch_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_param_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_pci_to_add_mod_list_r17.size() > 0) {
    j.start_array("additionalPCI-ToAddModList-r17");
    for (const auto& e1 : add_pci_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (add_pci_to_release_list_r17.size() > 0) {
    j.start_array("additionalPCI-ToReleaseList-r17");
    for (const auto& e1 : add_pci_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (unified_tci_state_type_r17_present) {
    j.write_str("unifiedTCI-StateType-r17", unified_tci_state_type_r17.to_string());
  }
  if (ul_pwr_ctrl_to_add_mod_list_r17.size() > 0) {
    j.start_array("uplink-PowerControlToAddModList-r17");
    for (const auto& e1 : ul_pwr_ctrl_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ul_pwr_ctrl_to_release_list_r17.size() > 0) {
    j.start_array("uplink-PowerControlToReleaseList-r17");
    for (const auto& e1 : ul_pwr_ctrl_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sfn_schem_epdcch_r17_present) {
    j.write_str("sfnSchemePDCCH-r17", sfn_schem_epdcch_r17.to_string());
  }
  if (sfn_scheme_pdsch_r17_present) {
    j.write_str("sfnSchemePDSCH-r17", sfn_scheme_pdsch_r17.to_string());
  }
  j.end_obj();
}

const char* mimo_param_r17_s::unified_tci_state_type_r17_opts::to_string() const
{
  static const char* names[] = {"separate", "joint"};
  return convert_enum_idx(names, 2, value, "mimo_param_r17_s::unified_tci_state_type_r17_e_");
}

const char* mimo_param_r17_s::sfn_schem_epdcch_r17_opts::to_string() const
{
  static const char* names[] = {"sfnSchemeA", "sfnSchemeB"};
  return convert_enum_idx(names, 2, value, "mimo_param_r17_s::sfn_schem_epdcch_r17_e_");
}

const char* mimo_param_r17_s::sfn_scheme_pdsch_r17_opts::to_string() const
{
  static const char* names[] = {"sfnSchemeA", "sfnSchemeB"};
  return convert_enum_idx(names, 2, value, "mimo_param_r17_s::sfn_scheme_pdsch_r17_e_");
}

// NR-DL-PRS-PDC-Info-r17 ::= SEQUENCE
SRSASN_CODE nr_dl_prs_pdc_info_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(nr_dl_prs_pdc_res_set_r17_present, 1));

  if (nr_dl_prs_pdc_res_set_r17_present) {
    HANDLE_CODE(nr_dl_prs_pdc_res_set_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_dl_prs_pdc_info_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(nr_dl_prs_pdc_res_set_r17_present, 1));

  if (nr_dl_prs_pdc_res_set_r17_present) {
    HANDLE_CODE(nr_dl_prs_pdc_res_set_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void nr_dl_prs_pdc_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nr_dl_prs_pdc_res_set_r17_present) {
    j.write_fieldname("nr-DL-PRS-PDC-ResourceSet-r17");
    nr_dl_prs_pdc_res_set_r17.to_json(j);
  }
  j.end_obj();
}

// PDCCH-ServingCellConfig ::= SEQUENCE
SRSASN_CODE pdcch_serving_cell_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slot_format_ind_present, 1));

  if (slot_format_ind_present) {
    HANDLE_CODE(slot_format_ind.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= availability_ind_r16.is_present();
    group_flags[0] |= search_space_switch_timer_r16_present;
    group_flags[1] |= search_space_switch_timer_v1710_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(availability_ind_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(search_space_switch_timer_r16_present, 1));
      if (availability_ind_r16.is_present()) {
        HANDLE_CODE(availability_ind_r16->pack(bref));
      }
      if (search_space_switch_timer_r16_present) {
        HANDLE_CODE(pack_integer(bref, search_space_switch_timer_r16, (uint8_t)1u, (uint8_t)80u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(search_space_switch_timer_v1710_present, 1));
      if (search_space_switch_timer_v1710_present) {
        HANDLE_CODE(pack_integer(bref, search_space_switch_timer_v1710, (uint16_t)81u, (uint16_t)1280u));
      }
    }
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

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(availability_ind_r16, bref);
      HANDLE_CODE(bref.unpack(search_space_switch_timer_r16_present, 1));
      if (availability_ind_r16.is_present()) {
        HANDLE_CODE(availability_ind_r16->unpack(bref));
      }
      if (search_space_switch_timer_r16_present) {
        HANDLE_CODE(unpack_integer(search_space_switch_timer_r16, bref, (uint8_t)1u, (uint8_t)80u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(search_space_switch_timer_v1710_present, 1));
      if (search_space_switch_timer_v1710_present) {
        HANDLE_CODE(unpack_integer(search_space_switch_timer_v1710, bref, (uint16_t)81u, (uint16_t)1280u));
      }
    }
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
  if (ext) {
    if (availability_ind_r16.is_present()) {
      j.write_fieldname("availabilityIndicator-r16");
      availability_ind_r16->to_json(j);
    }
    if (search_space_switch_timer_r16_present) {
      j.write_int("searchSpaceSwitchTimer-r16", search_space_switch_timer_r16);
    }
    if (search_space_switch_timer_v1710_present) {
      j.write_int("searchSpaceSwitchTimer-v1710", search_space_switch_timer_v1710);
    }
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
    group_flags[1] |= pdsch_code_block_group_tx_list_r16.is_present();
    group_flags[2] |= dl_harq_feedback_disabled_r17.is_present();
    group_flags[2] |= nrof_harq_processes_for_pdsch_v1700_present;
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
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pdsch_code_block_group_tx_list_r16.is_present(), 1));
      if (pdsch_code_block_group_tx_list_r16.is_present()) {
        HANDLE_CODE(pdsch_code_block_group_tx_list_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(dl_harq_feedback_disabled_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(nrof_harq_processes_for_pdsch_v1700_present, 1));
      if (dl_harq_feedback_disabled_r17.is_present()) {
        HANDLE_CODE(dl_harq_feedback_disabled_r17->pack(bref));
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
    ext_groups_unpacker_guard group_flags(3);
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
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(pdsch_code_block_group_tx_list_r16, bref);
      if (pdsch_code_block_group_tx_list_r16.is_present()) {
        HANDLE_CODE(pdsch_code_block_group_tx_list_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(dl_harq_feedback_disabled_r17, bref);
      HANDLE_CODE(bref.unpack(nrof_harq_processes_for_pdsch_v1700_present, 1));
      if (dl_harq_feedback_disabled_r17.is_present()) {
        HANDLE_CODE(dl_harq_feedback_disabled_r17->unpack(bref));
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
    if (pdsch_code_block_group_tx_list_r16.is_present()) {
      j.write_fieldname("pdsch-CodeBlockGroupTransmissionList-r16");
      pdsch_code_block_group_tx_list_r16->to_json(j);
    }
    if (dl_harq_feedback_disabled_r17.is_present()) {
      j.write_fieldname("downlinkHARQ-FeedbackDisabled-r17");
      dl_harq_feedback_disabled_r17->to_json(j);
    }
    if (nrof_harq_processes_for_pdsch_v1700_present) {
      j.write_str("nrofHARQ-ProcessesForPDSCH-v1700", "n32");
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

// SemiStaticChannelAccessConfigUE-r17 ::= SEQUENCE
SRSASN_CODE semi_static_ch_access_cfg_ue_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(period_ue_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, offset_ue_r17, (uint16_t)0u, (uint16_t)559u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE semi_static_ch_access_cfg_ue_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(period_ue_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(offset_ue_r17, bref, (uint16_t)0u, (uint16_t)559u));

  return SRSASN_SUCCESS;
}
void semi_static_ch_access_cfg_ue_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("periodUE-r17", period_ue_r17.to_string());
  j.write_int("offsetUE-r17", offset_ue_r17);
  j.end_obj();
}

const char* semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_opts::to_string() const
{
  static const char* names[] = {"ms1", "ms2", "ms2dot5", "ms4", "ms5", "ms10", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_e_");
}
float semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_opts::to_number() const
{
  static const float numbers[] = {1.0, 2.0, 2.5, 4.0, 5.0, 10.0};
  return map_enum_number(numbers, 6, value, "semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_e_");
}
const char* semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"1", "2", "2.5", "4", "5", "10"};
  return convert_enum_idx(number_strs, 8, value, "semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_e_");
}

// TCI-ActivatedConfig-r17 ::= SEQUENCE
SRSASN_CODE tci_activ_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, pdcch_tci_r17, 1, 5, integer_packer<uint8_t>(0, 127)));
  HANDLE_CODE(pdsch_tci_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tci_activ_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(pdcch_tci_r17, bref, 1, 5, integer_packer<uint8_t>(0, 127)));
  HANDLE_CODE(pdsch_tci_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void tci_activ_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("pdcch-TCI-r17");
  for (const auto& e1 : pdcch_tci_r17) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_str("pdsch-TCI-r17", pdsch_tci_r17.to_string());
  j.end_obj();
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

// TDD-UL-DL-ConfigDedicated-IAB-MT-r16 ::= SEQUENCE
SRSASN_CODE tdd_ul_dl_cfg_ded_iab_mt_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slot_specific_cfgs_to_add_mod_list_iab_mt_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(slot_specific_cfgs_to_release_list_iab_mt_r16.size() > 0, 1));

  if (slot_specific_cfgs_to_add_mod_list_iab_mt_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slot_specific_cfgs_to_add_mod_list_iab_mt_r16, 1, 320));
  }
  if (slot_specific_cfgs_to_release_list_iab_mt_r16.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, slot_specific_cfgs_to_release_list_iab_mt_r16, 1, 320, integer_packer<uint16_t>(0, 319)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_ul_dl_cfg_ded_iab_mt_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool slot_specific_cfgs_to_add_mod_list_iab_mt_r16_present;
  HANDLE_CODE(bref.unpack(slot_specific_cfgs_to_add_mod_list_iab_mt_r16_present, 1));
  bool slot_specific_cfgs_to_release_list_iab_mt_r16_present;
  HANDLE_CODE(bref.unpack(slot_specific_cfgs_to_release_list_iab_mt_r16_present, 1));

  if (slot_specific_cfgs_to_add_mod_list_iab_mt_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slot_specific_cfgs_to_add_mod_list_iab_mt_r16, bref, 1, 320));
  }
  if (slot_specific_cfgs_to_release_list_iab_mt_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(
        slot_specific_cfgs_to_release_list_iab_mt_r16, bref, 1, 320, integer_packer<uint16_t>(0, 319)));
  }

  return SRSASN_SUCCESS;
}
void tdd_ul_dl_cfg_ded_iab_mt_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (slot_specific_cfgs_to_add_mod_list_iab_mt_r16.size() > 0) {
    j.start_array("slotSpecificConfigurationsToAddModList-IAB-MT-r16");
    for (const auto& e1 : slot_specific_cfgs_to_add_mod_list_iab_mt_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (slot_specific_cfgs_to_release_list_iab_mt_r16.size() > 0) {
    j.start_array("slotSpecificConfigurationsToReleaseList-IAB-MT-r16");
    for (const auto& e1 : slot_specific_cfgs_to_release_list_iab_mt_r16) {
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
    group_flags[1] |= enable_pl_rs_upd_for_pusch_srs_r16_present;
    group_flags[1] |= enable_default_beam_pl_for_pusch0_0_r16_present;
    group_flags[1] |= enable_default_beam_pl_for_pucch_r16_present;
    group_flags[1] |= enable_default_beam_pl_for_srs_r16_present;
    group_flags[1] |= ul_tx_switching_r16.is_present();
    group_flags[1] |= mpr_pwr_boost_fr2_r16_present;
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
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(enable_pl_rs_upd_for_pusch_srs_r16_present, 1));
      HANDLE_CODE(bref.pack(enable_default_beam_pl_for_pusch0_0_r16_present, 1));
      HANDLE_CODE(bref.pack(enable_default_beam_pl_for_pucch_r16_present, 1));
      HANDLE_CODE(bref.pack(enable_default_beam_pl_for_srs_r16_present, 1));
      HANDLE_CODE(bref.pack(ul_tx_switching_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(mpr_pwr_boost_fr2_r16_present, 1));
      if (ul_tx_switching_r16.is_present()) {
        HANDLE_CODE(ul_tx_switching_r16->pack(bref));
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
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(pwr_boost_pi2_bpsk_present, 1));
      unpack_presence_flag(ul_ch_bw_per_scs_list, bref);
      if (pwr_boost_pi2_bpsk_present) {
        HANDLE_CODE(bref.unpack(pwr_boost_pi2_bpsk, 1));
      }
      if (ul_ch_bw_per_scs_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ul_ch_bw_per_scs_list, bref, 1, 5));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(enable_pl_rs_upd_for_pusch_srs_r16_present, 1));
      HANDLE_CODE(bref.unpack(enable_default_beam_pl_for_pusch0_0_r16_present, 1));
      HANDLE_CODE(bref.unpack(enable_default_beam_pl_for_pucch_r16_present, 1));
      HANDLE_CODE(bref.unpack(enable_default_beam_pl_for_srs_r16_present, 1));
      unpack_presence_flag(ul_tx_switching_r16, bref);
      HANDLE_CODE(bref.unpack(mpr_pwr_boost_fr2_r16_present, 1));
      if (ul_tx_switching_r16.is_present()) {
        HANDLE_CODE(ul_tx_switching_r16->unpack(bref));
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
    if (enable_pl_rs_upd_for_pusch_srs_r16_present) {
      j.write_str("enablePL-RS-UpdateForPUSCH-SRS-r16", "enabled");
    }
    if (enable_default_beam_pl_for_pusch0_0_r16_present) {
      j.write_str("enableDefaultBeamPL-ForPUSCH0-0-r16", "enabled");
    }
    if (enable_default_beam_pl_for_pucch_r16_present) {
      j.write_str("enableDefaultBeamPL-ForPUCCH-r16", "enabled");
    }
    if (enable_default_beam_pl_for_srs_r16_present) {
      j.write_str("enableDefaultBeamPL-ForSRS-r16", "enabled");
    }
    if (ul_tx_switching_r16.is_present()) {
      j.write_fieldname("uplinkTxSwitching-r16");
      ul_tx_switching_r16->to_json(j);
    }
    if (mpr_pwr_boost_fr2_r16_present) {
      j.write_str("mpr-PowerBoost-FR2-r16", "true");
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
  HANDLE_CODE(bref.pack(dummy1_present, 1));
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
    group_flags[1] |= supplementary_ul_release_r16_present;
    group_flags[1] |= tdd_ul_dl_cfg_ded_iab_mt_r16.is_present();
    group_flags[1] |= dormant_bwp_cfg_r16.is_present();
    group_flags[1] |= ca_slot_offset_r16.is_present();
    group_flags[1] |= dummy2.is_present();
    group_flags[1] |= intra_cell_guard_bands_dl_list_r16.is_present();
    group_flags[1] |= intra_cell_guard_bands_ul_list_r16.is_present();
    group_flags[1] |= csi_rs_validation_with_dci_r16_present;
    group_flags[1] |= lte_crs_pattern_list1_r16.is_present();
    group_flags[1] |= lte_crs_pattern_list2_r16.is_present();
    group_flags[1] |= crs_rate_match_per_coreset_pool_idx_r16_present;
    group_flags[1] |= enable_two_default_tci_states_r16_present;
    group_flags[1] |= enable_default_tci_state_per_coreset_pool_idx_r16_present;
    group_flags[1] |= enable_beam_switch_timing_r16_present;
    group_flags[1] |= cbg_tx_diff_t_bs_processing_type1_r16_present;
    group_flags[1] |= cbg_tx_diff_t_bs_processing_type2_r16_present;
    group_flags[2] |= directional_collision_handling_r16_present;
    group_flags[2] |= ch_access_cfg_r16.is_present();
    group_flags[3] |= nr_dl_prs_pdc_info_r17.is_present();
    group_flags[3] |= semi_static_ch_access_cfg_ue_r17.is_present();
    group_flags[3] |= mimo_param_r17.is_present();
    group_flags[3] |= ch_access_mode2_r17_present;
    group_flags[3] |= time_domain_harq_bundling_type1_r17_present;
    group_flags[3] |= nrof_harq_bundling_groups_r17_present;
    group_flags[3] |= fdmed_reception_multicast_r17_present;
    group_flags[3] |= more_than_one_nack_only_mode_r17_present;
    group_flags[3] |= tci_activ_cfg_r17.is_present();
    group_flags[3] |= directional_collision_handling_dc_r17_present;
    group_flags[3] |= lte_neigh_cells_crs_assist_info_list_r17.is_present();
    group_flags[4] |= lte_neigh_cells_crs_assumptions_r17_present;
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
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supplementary_ul_release_r16_present, 1));
      HANDLE_CODE(bref.pack(tdd_ul_dl_cfg_ded_iab_mt_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(dormant_bwp_cfg_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ca_slot_offset_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(dummy2.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_cell_guard_bands_dl_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_cell_guard_bands_ul_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(csi_rs_validation_with_dci_r16_present, 1));
      HANDLE_CODE(bref.pack(lte_crs_pattern_list1_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(lte_crs_pattern_list2_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(crs_rate_match_per_coreset_pool_idx_r16_present, 1));
      HANDLE_CODE(bref.pack(enable_two_default_tci_states_r16_present, 1));
      HANDLE_CODE(bref.pack(enable_default_tci_state_per_coreset_pool_idx_r16_present, 1));
      HANDLE_CODE(bref.pack(enable_beam_switch_timing_r16_present, 1));
      HANDLE_CODE(bref.pack(cbg_tx_diff_t_bs_processing_type1_r16_present, 1));
      HANDLE_CODE(bref.pack(cbg_tx_diff_t_bs_processing_type2_r16_present, 1));
      if (tdd_ul_dl_cfg_ded_iab_mt_r16.is_present()) {
        HANDLE_CODE(tdd_ul_dl_cfg_ded_iab_mt_r16->pack(bref));
      }
      if (dormant_bwp_cfg_r16.is_present()) {
        HANDLE_CODE(dormant_bwp_cfg_r16->pack(bref));
      }
      if (ca_slot_offset_r16.is_present()) {
        HANDLE_CODE(ca_slot_offset_r16->pack(bref));
      }
      if (dummy2.is_present()) {
        HANDLE_CODE(dummy2->pack(bref));
      }
      if (intra_cell_guard_bands_dl_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_cell_guard_bands_dl_list_r16, 1, 5));
      }
      if (intra_cell_guard_bands_ul_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_cell_guard_bands_ul_list_r16, 1, 5));
      }
      if (lte_crs_pattern_list1_r16.is_present()) {
        HANDLE_CODE(lte_crs_pattern_list1_r16->pack(bref));
      }
      if (lte_crs_pattern_list2_r16.is_present()) {
        HANDLE_CODE(lte_crs_pattern_list2_r16->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(directional_collision_handling_r16_present, 1));
      HANDLE_CODE(bref.pack(ch_access_cfg_r16.is_present(), 1));
      if (ch_access_cfg_r16.is_present()) {
        HANDLE_CODE(ch_access_cfg_r16->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(nr_dl_prs_pdc_info_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(semi_static_ch_access_cfg_ue_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(mimo_param_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(ch_access_mode2_r17_present, 1));
      HANDLE_CODE(bref.pack(time_domain_harq_bundling_type1_r17_present, 1));
      HANDLE_CODE(bref.pack(nrof_harq_bundling_groups_r17_present, 1));
      HANDLE_CODE(bref.pack(fdmed_reception_multicast_r17_present, 1));
      HANDLE_CODE(bref.pack(more_than_one_nack_only_mode_r17_present, 1));
      HANDLE_CODE(bref.pack(tci_activ_cfg_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(directional_collision_handling_dc_r17_present, 1));
      HANDLE_CODE(bref.pack(lte_neigh_cells_crs_assist_info_list_r17.is_present(), 1));
      if (nr_dl_prs_pdc_info_r17.is_present()) {
        HANDLE_CODE(nr_dl_prs_pdc_info_r17->pack(bref));
      }
      if (semi_static_ch_access_cfg_ue_r17.is_present()) {
        HANDLE_CODE(semi_static_ch_access_cfg_ue_r17->pack(bref));
      }
      if (mimo_param_r17.is_present()) {
        HANDLE_CODE(mimo_param_r17->pack(bref));
      }
      if (nrof_harq_bundling_groups_r17_present) {
        HANDLE_CODE(nrof_harq_bundling_groups_r17.pack(bref));
      }
      if (tci_activ_cfg_r17.is_present()) {
        HANDLE_CODE(tci_activ_cfg_r17->pack(bref));
      }
      if (lte_neigh_cells_crs_assist_info_list_r17.is_present()) {
        HANDLE_CODE(lte_neigh_cells_crs_assist_info_list_r17->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(lte_neigh_cells_crs_assumptions_r17_present, 1));
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
  HANDLE_CODE(bref.unpack(dummy1_present, 1));
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
    ext_groups_unpacker_guard group_flags(5);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(lte_crs_to_match_around, bref);
      unpack_presence_flag(rate_match_pattern_to_add_mod_list, bref);
      unpack_presence_flag(rate_match_pattern_to_release_list, bref);
      unpack_presence_flag(dl_ch_bw_per_scs_list, bref);
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
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(supplementary_ul_release_r16_present, 1));
      unpack_presence_flag(tdd_ul_dl_cfg_ded_iab_mt_r16, bref);
      unpack_presence_flag(dormant_bwp_cfg_r16, bref);
      unpack_presence_flag(ca_slot_offset_r16, bref);
      unpack_presence_flag(dummy2, bref);
      unpack_presence_flag(intra_cell_guard_bands_dl_list_r16, bref);
      unpack_presence_flag(intra_cell_guard_bands_ul_list_r16, bref);
      HANDLE_CODE(bref.unpack(csi_rs_validation_with_dci_r16_present, 1));
      unpack_presence_flag(lte_crs_pattern_list1_r16, bref);
      unpack_presence_flag(lte_crs_pattern_list2_r16, bref);
      HANDLE_CODE(bref.unpack(crs_rate_match_per_coreset_pool_idx_r16_present, 1));
      HANDLE_CODE(bref.unpack(enable_two_default_tci_states_r16_present, 1));
      HANDLE_CODE(bref.unpack(enable_default_tci_state_per_coreset_pool_idx_r16_present, 1));
      HANDLE_CODE(bref.unpack(enable_beam_switch_timing_r16_present, 1));
      HANDLE_CODE(bref.unpack(cbg_tx_diff_t_bs_processing_type1_r16_present, 1));
      HANDLE_CODE(bref.unpack(cbg_tx_diff_t_bs_processing_type2_r16_present, 1));
      if (tdd_ul_dl_cfg_ded_iab_mt_r16.is_present()) {
        HANDLE_CODE(tdd_ul_dl_cfg_ded_iab_mt_r16->unpack(bref));
      }
      if (dormant_bwp_cfg_r16.is_present()) {
        HANDLE_CODE(dormant_bwp_cfg_r16->unpack(bref));
      }
      if (ca_slot_offset_r16.is_present()) {
        HANDLE_CODE(ca_slot_offset_r16->unpack(bref));
      }
      if (dummy2.is_present()) {
        HANDLE_CODE(dummy2->unpack(bref));
      }
      if (intra_cell_guard_bands_dl_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_cell_guard_bands_dl_list_r16, bref, 1, 5));
      }
      if (intra_cell_guard_bands_ul_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_cell_guard_bands_ul_list_r16, bref, 1, 5));
      }
      if (lte_crs_pattern_list1_r16.is_present()) {
        HANDLE_CODE(lte_crs_pattern_list1_r16->unpack(bref));
      }
      if (lte_crs_pattern_list2_r16.is_present()) {
        HANDLE_CODE(lte_crs_pattern_list2_r16->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(directional_collision_handling_r16_present, 1));
      unpack_presence_flag(ch_access_cfg_r16, bref);
      if (ch_access_cfg_r16.is_present()) {
        HANDLE_CODE(ch_access_cfg_r16->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(nr_dl_prs_pdc_info_r17, bref);
      unpack_presence_flag(semi_static_ch_access_cfg_ue_r17, bref);
      unpack_presence_flag(mimo_param_r17, bref);
      HANDLE_CODE(bref.unpack(ch_access_mode2_r17_present, 1));
      HANDLE_CODE(bref.unpack(time_domain_harq_bundling_type1_r17_present, 1));
      HANDLE_CODE(bref.unpack(nrof_harq_bundling_groups_r17_present, 1));
      HANDLE_CODE(bref.unpack(fdmed_reception_multicast_r17_present, 1));
      HANDLE_CODE(bref.unpack(more_than_one_nack_only_mode_r17_present, 1));
      unpack_presence_flag(tci_activ_cfg_r17, bref);
      HANDLE_CODE(bref.unpack(directional_collision_handling_dc_r17_present, 1));
      unpack_presence_flag(lte_neigh_cells_crs_assist_info_list_r17, bref);
      if (nr_dl_prs_pdc_info_r17.is_present()) {
        HANDLE_CODE(nr_dl_prs_pdc_info_r17->unpack(bref));
      }
      if (semi_static_ch_access_cfg_ue_r17.is_present()) {
        HANDLE_CODE(semi_static_ch_access_cfg_ue_r17->unpack(bref));
      }
      if (mimo_param_r17.is_present()) {
        HANDLE_CODE(mimo_param_r17->unpack(bref));
      }
      if (nrof_harq_bundling_groups_r17_present) {
        HANDLE_CODE(nrof_harq_bundling_groups_r17.unpack(bref));
      }
      if (tci_activ_cfg_r17.is_present()) {
        HANDLE_CODE(tci_activ_cfg_r17->unpack(bref));
      }
      if (lte_neigh_cells_crs_assist_info_list_r17.is_present()) {
        HANDLE_CODE(lte_neigh_cells_crs_assist_info_list_r17->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(lte_neigh_cells_crs_assumptions_r17_present, 1));
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
  if (dummy1_present) {
    j.write_str("dummy1", "enabled");
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
    if (supplementary_ul_release_r16_present) {
      j.write_str("supplementaryUplinkRelease-r16", "true");
    }
    if (tdd_ul_dl_cfg_ded_iab_mt_r16.is_present()) {
      j.write_fieldname("tdd-UL-DL-ConfigurationDedicated-IAB-MT-r16");
      tdd_ul_dl_cfg_ded_iab_mt_r16->to_json(j);
    }
    if (dormant_bwp_cfg_r16.is_present()) {
      j.write_fieldname("dormantBWP-Config-r16");
      dormant_bwp_cfg_r16->to_json(j);
    }
    if (ca_slot_offset_r16.is_present()) {
      j.write_fieldname("ca-SlotOffset-r16");
      ca_slot_offset_r16->to_json(j);
    }
    if (dummy2.is_present()) {
      j.write_fieldname("dummy2");
      dummy2->to_json(j);
    }
    if (intra_cell_guard_bands_dl_list_r16.is_present()) {
      j.start_array("intraCellGuardBandsDL-List-r16");
      for (const auto& e1 : *intra_cell_guard_bands_dl_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_cell_guard_bands_ul_list_r16.is_present()) {
      j.start_array("intraCellGuardBandsUL-List-r16");
      for (const auto& e1 : *intra_cell_guard_bands_ul_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (csi_rs_validation_with_dci_r16_present) {
      j.write_str("csi-RS-ValidationWithDCI-r16", "enabled");
    }
    if (lte_crs_pattern_list1_r16.is_present()) {
      j.write_fieldname("lte-CRS-PatternList1-r16");
      lte_crs_pattern_list1_r16->to_json(j);
    }
    if (lte_crs_pattern_list2_r16.is_present()) {
      j.write_fieldname("lte-CRS-PatternList2-r16");
      lte_crs_pattern_list2_r16->to_json(j);
    }
    if (crs_rate_match_per_coreset_pool_idx_r16_present) {
      j.write_str("crs-RateMatch-PerCORESETPoolIndex-r16", "enabled");
    }
    if (enable_two_default_tci_states_r16_present) {
      j.write_str("enableTwoDefaultTCI-States-r16", "enabled");
    }
    if (enable_default_tci_state_per_coreset_pool_idx_r16_present) {
      j.write_str("enableDefaultTCI-StatePerCoresetPoolIndex-r16", "enabled");
    }
    if (enable_beam_switch_timing_r16_present) {
      j.write_str("enableBeamSwitchTiming-r16", "true");
    }
    if (cbg_tx_diff_t_bs_processing_type1_r16_present) {
      j.write_str("cbg-TxDiffTBsProcessingType1-r16", "enabled");
    }
    if (cbg_tx_diff_t_bs_processing_type2_r16_present) {
      j.write_str("cbg-TxDiffTBsProcessingType2-r16", "enabled");
    }
    if (directional_collision_handling_r16_present) {
      j.write_str("directionalCollisionHandling-r16", "enabled");
    }
    if (ch_access_cfg_r16.is_present()) {
      j.write_fieldname("channelAccessConfig-r16");
      ch_access_cfg_r16->to_json(j);
    }
    if (nr_dl_prs_pdc_info_r17.is_present()) {
      j.write_fieldname("nr-dl-PRS-PDC-Info-r17");
      nr_dl_prs_pdc_info_r17->to_json(j);
    }
    if (semi_static_ch_access_cfg_ue_r17.is_present()) {
      j.write_fieldname("semiStaticChannelAccessConfigUE-r17");
      semi_static_ch_access_cfg_ue_r17->to_json(j);
    }
    if (mimo_param_r17.is_present()) {
      j.write_fieldname("mimoParam-r17");
      mimo_param_r17->to_json(j);
    }
    if (ch_access_mode2_r17_present) {
      j.write_str("channelAccessMode2-r17", "enabled");
    }
    if (time_domain_harq_bundling_type1_r17_present) {
      j.write_str("timeDomainHARQ-BundlingType1-r17", "enabled");
    }
    if (nrof_harq_bundling_groups_r17_present) {
      j.write_str("nrofHARQ-BundlingGroups-r17", nrof_harq_bundling_groups_r17.to_string());
    }
    if (fdmed_reception_multicast_r17_present) {
      j.write_str("fdmed-ReceptionMulticast-r17", "true");
    }
    if (more_than_one_nack_only_mode_r17_present) {
      j.write_str("moreThanOneNackOnlyMode-r17", "mode2");
    }
    if (tci_activ_cfg_r17.is_present()) {
      j.write_fieldname("tci-ActivatedConfig-r17");
      tci_activ_cfg_r17->to_json(j);
    }
    if (directional_collision_handling_dc_r17_present) {
      j.write_str("directionalCollisionHandling-DC-r17", "enabled");
    }
    if (lte_neigh_cells_crs_assist_info_list_r17.is_present()) {
      j.write_fieldname("lte-NeighCellsCRS-AssistInfoList-r17");
      lte_neigh_cells_crs_assist_info_list_r17->to_json(j);
    }
    if (lte_neigh_cells_crs_assumptions_r17_present) {
      j.write_str("lte-NeighCellsCRS-Assumptions-r17", "false");
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

void serving_cell_cfg_s::ca_slot_offset_r16_c_::destroy_() {}
void serving_cell_cfg_s::ca_slot_offset_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
serving_cell_cfg_s::ca_slot_offset_r16_c_::ca_slot_offset_r16_c_(const serving_cell_cfg_s::ca_slot_offset_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ref_scs15k_hz:
      c.init(other.c.get<int8_t>());
      break;
    case types::ref_scs30_k_hz:
      c.init(other.c.get<int8_t>());
      break;
    case types::ref_scs60_k_hz:
      c.init(other.c.get<int8_t>());
      break;
    case types::ref_scs120_k_hz:
      c.init(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_s::ca_slot_offset_r16_c_");
  }
}
serving_cell_cfg_s::ca_slot_offset_r16_c_&
serving_cell_cfg_s::ca_slot_offset_r16_c_::operator=(const serving_cell_cfg_s::ca_slot_offset_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ref_scs15k_hz:
      c.set(other.c.get<int8_t>());
      break;
    case types::ref_scs30_k_hz:
      c.set(other.c.get<int8_t>());
      break;
    case types::ref_scs60_k_hz:
      c.set(other.c.get<int8_t>());
      break;
    case types::ref_scs120_k_hz:
      c.set(other.c.get<int8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_s::ca_slot_offset_r16_c_");
  }

  return *this;
}
int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs15k_hz()
{
  set(types::ref_scs15k_hz);
  return c.get<int8_t>();
}
int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs30_k_hz()
{
  set(types::ref_scs30_k_hz);
  return c.get<int8_t>();
}
int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs60_k_hz()
{
  set(types::ref_scs60_k_hz);
  return c.get<int8_t>();
}
int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs120_k_hz()
{
  set(types::ref_scs120_k_hz);
  return c.get<int8_t>();
}
void serving_cell_cfg_s::ca_slot_offset_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ref_scs15k_hz:
      j.write_int("refSCS15kHz", c.get<int8_t>());
      break;
    case types::ref_scs30_k_hz:
      j.write_int("refSCS30KHz", c.get<int8_t>());
      break;
    case types::ref_scs60_k_hz:
      j.write_int("refSCS60KHz", c.get<int8_t>());
      break;
    case types::ref_scs120_k_hz:
      j.write_int("refSCS120KHz", c.get<int8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_s::ca_slot_offset_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE serving_cell_cfg_s::ca_slot_offset_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ref_scs15k_hz:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-2, (int8_t)2));
      break;
    case types::ref_scs30_k_hz:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-5, (int8_t)5));
      break;
    case types::ref_scs60_k_hz:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-10, (int8_t)10));
      break;
    case types::ref_scs120_k_hz:
      HANDLE_CODE(pack_integer(bref, c.get<int8_t>(), (int8_t)-20, (int8_t)20));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_s::ca_slot_offset_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_cfg_s::ca_slot_offset_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ref_scs15k_hz:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-2, (int8_t)2));
      break;
    case types::ref_scs30_k_hz:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-5, (int8_t)5));
      break;
    case types::ref_scs60_k_hz:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-10, (int8_t)10));
      break;
    case types::ref_scs120_k_hz:
      HANDLE_CODE(unpack_integer(c.get<int8_t>(), bref, (int8_t)-20, (int8_t)20));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_cfg_s::ca_slot_offset_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_cell_cfg_s::ca_slot_offset_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"refSCS15kHz", "refSCS30KHz", "refSCS60KHz", "refSCS120KHz"};
  return convert_enum_idx(names, 4, value, "serving_cell_cfg_s::ca_slot_offset_r16_c_::types");
}
uint8_t serving_cell_cfg_s::ca_slot_offset_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "serving_cell_cfg_s::ca_slot_offset_r16_c_::types");
}

const char* serving_cell_cfg_s::nrof_harq_bundling_groups_r17_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "serving_cell_cfg_s::nrof_harq_bundling_groups_r17_e_");
}
uint8_t serving_cell_cfg_s::nrof_harq_bundling_groups_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "serving_cell_cfg_s::nrof_harq_bundling_groups_r17_e_");
}
