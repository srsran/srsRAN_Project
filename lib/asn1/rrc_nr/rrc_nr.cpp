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

#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MIB ::= SEQUENCE
SRSASN_CODE mib_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sys_frame_num.pack(bref));
  HANDLE_CODE(sub_carrier_spacing_common.pack(bref));
  HANDLE_CODE(pack_integer(bref, ssb_subcarrier_offset, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(dmrs_type_a_position.pack(bref));
  HANDLE_CODE(pdcch_cfg_sib1.pack(bref));
  HANDLE_CODE(cell_barred.pack(bref));
  HANDLE_CODE(intra_freq_resel.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sys_frame_num.unpack(bref));
  HANDLE_CODE(sub_carrier_spacing_common.unpack(bref));
  HANDLE_CODE(unpack_integer(ssb_subcarrier_offset, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(dmrs_type_a_position.unpack(bref));
  HANDLE_CODE(pdcch_cfg_sib1.unpack(bref));
  HANDLE_CODE(cell_barred.unpack(bref));
  HANDLE_CODE(intra_freq_resel.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("systemFrameNumber", sys_frame_num.to_string());
  j.write_str("subCarrierSpacingCommon", sub_carrier_spacing_common.to_string());
  j.write_int("ssb-SubcarrierOffset", ssb_subcarrier_offset);
  j.write_str("dmrs-TypeA-Position", dmrs_type_a_position.to_string());
  j.write_fieldname("pdcch-ConfigSIB1");
  pdcch_cfg_sib1.to_json(j);
  j.write_str("cellBarred", cell_barred.to_string());
  j.write_str("intraFreqReselection", intra_freq_resel.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

const char* mib_s::sub_carrier_spacing_common_opts::to_string() const
{
  static const char* names[] = {"scs15or60", "scs30or120"};
  return convert_enum_idx(names, 2, value, "mib_s::sub_carrier_spacing_common_e_");
}
uint8_t mib_s::sub_carrier_spacing_common_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30};
  return map_enum_number(numbers, 2, value, "mib_s::sub_carrier_spacing_common_e_");
}

const char* mib_s::dmrs_type_a_position_opts::to_string() const
{
  static const char* names[] = {"pos2", "pos3"};
  return convert_enum_idx(names, 2, value, "mib_s::dmrs_type_a_position_e_");
}
uint8_t mib_s::dmrs_type_a_position_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "mib_s::dmrs_type_a_position_e_");
}

const char* mib_s::cell_barred_opts::to_string() const
{
  static const char* names[] = {"barred", "notBarred"};
  return convert_enum_idx(names, 2, value, "mib_s::cell_barred_e_");
}

const char* mib_s::intra_freq_resel_opts::to_string() const
{
  static const char* names[] = {"allowed", "notAllowed"};
  return convert_enum_idx(names, 2, value, "mib_s::intra_freq_resel_e_");
}

// BCCH-BCH-MessageType ::= CHOICE
void bcch_bch_msg_type_c::set(types::options e)
{
  type_ = e;
}
mib_s& bcch_bch_msg_type_c::set_mib()
{
  set(types::mib);
  return c;
}
void bcch_bch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void bcch_bch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mib:
      j.write_fieldname("mib");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_bch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_bch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mib:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_bch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mib:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_bch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bcch_bch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"mib", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "bcch_bch_msg_type_c::types");
}

// BCCH-BCH-Message ::= SEQUENCE
SRSASN_CODE bcch_bch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_bch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-BCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// EUTRA-NS-PmaxValue ::= SEQUENCE
SRSASN_CODE eutra_ns_pmax_value_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_pmax_present, 1));
  HANDLE_CODE(bref.pack(add_spec_emission_present, 1));

  if (add_pmax_present) {
    HANDLE_CODE(pack_integer(bref, add_pmax, (int8_t)-30, (int8_t)33));
  }
  if (add_spec_emission_present) {
    HANDLE_CODE(pack_integer(bref, add_spec_emission, (uint16_t)1u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_ns_pmax_value_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(add_pmax_present, 1));
  HANDLE_CODE(bref.unpack(add_spec_emission_present, 1));

  if (add_pmax_present) {
    HANDLE_CODE(unpack_integer(add_pmax, bref, (int8_t)-30, (int8_t)33));
  }
  if (add_spec_emission_present) {
    HANDLE_CODE(unpack_integer(add_spec_emission, bref, (uint16_t)1u, (uint16_t)288u));
  }

  return SRSASN_SUCCESS;
}
void eutra_ns_pmax_value_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (add_pmax_present) {
    j.write_int("additionalPmax", add_pmax);
  }
  if (add_spec_emission_present) {
    j.write_int("additionalSpectrumEmission", add_spec_emission);
  }
  j.end_obj();
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

const char*
rach_cfg_common_two_step_ra_r16_s::msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_::types_opts::to_string()
    const
{
  static const char* names[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(
      names,
      8,
      value,
      "rach_cfg_common_two_step_ra_r16_s::msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_::types");
}
float rach_cfg_common_two_step_ra_r16_s::msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_::types_opts::
    to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(
      numbers,
      8,
      value,
      "rach_cfg_common_two_step_ra_r16_s::msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_::types");
}
const char* rach_cfg_common_two_step_ra_r16_s::msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_::types_opts::
    to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(
      number_strs,
      8,
      value,
      "rach_cfg_common_two_step_ra_r16_s::msg_a_ssb_per_rach_occasion_and_cb_preambs_per_ssb_r16_c_::types");
}

const char* rach_cfg_common_two_step_ra_r16_s::msg_a_prach_root_seq_idx_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"l839", "l139", "l571", "l1151"};
  return convert_enum_idx(names, 4, value, "rach_cfg_common_two_step_ra_r16_s::msg_a_prach_root_seq_idx_r16_c_::types");
}
uint16_t rach_cfg_common_two_step_ra_r16_s::msg_a_prach_root_seq_idx_r16_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {839, 139, 571, 1151};
  return map_enum_number(
      numbers, 4, value, "rach_cfg_common_two_step_ra_r16_s::msg_a_prach_root_seq_idx_r16_c_::types");
}

// SI-RequestResources ::= SEQUENCE
SRSASN_CODE si_request_res_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ra_assoc_period_idx_present, 1));
  HANDLE_CODE(bref.pack(ra_ssb_occasion_mask_idx_present, 1));

  HANDLE_CODE(pack_integer(bref, ra_preamb_start_idx, (uint8_t)0u, (uint8_t)63u));
  if (ra_assoc_period_idx_present) {
    HANDLE_CODE(pack_integer(bref, ra_assoc_period_idx, (uint8_t)0u, (uint8_t)15u));
  }
  if (ra_ssb_occasion_mask_idx_present) {
    HANDLE_CODE(pack_integer(bref, ra_ssb_occasion_mask_idx, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_request_res_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ra_assoc_period_idx_present, 1));
  HANDLE_CODE(bref.unpack(ra_ssb_occasion_mask_idx_present, 1));

  HANDLE_CODE(unpack_integer(ra_preamb_start_idx, bref, (uint8_t)0u, (uint8_t)63u));
  if (ra_assoc_period_idx_present) {
    HANDLE_CODE(unpack_integer(ra_assoc_period_idx, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (ra_ssb_occasion_mask_idx_present) {
    HANDLE_CODE(unpack_integer(ra_ssb_occasion_mask_idx, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void si_request_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ra-PreambleStartIndex", ra_preamb_start_idx);
  if (ra_assoc_period_idx_present) {
    j.write_int("ra-AssociationPeriodIndex", ra_assoc_period_idx);
  }
  if (ra_ssb_occasion_mask_idx_present) {
    j.write_int("ra-ssb-OccasionMaskIndex", ra_ssb_occasion_mask_idx);
  }
  j.end_obj();
}

// SIB-TypeInfo-v1700 ::= SEQUENCE
SRSASN_CODE sib_type_info_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(value_tag_r17_present, 1));
  HANDLE_CODE(bref.pack(area_scope_r17_present, 1));

  HANDLE_CODE(sib_type_r17.pack(bref));
  if (value_tag_r17_present) {
    HANDLE_CODE(pack_integer(bref, value_tag_r17, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type_info_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(value_tag_r17_present, 1));
  HANDLE_CODE(bref.unpack(area_scope_r17_present, 1));

  HANDLE_CODE(sib_type_r17.unpack(bref));
  if (value_tag_r17_present) {
    HANDLE_CODE(unpack_integer(value_tag_r17, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void sib_type_info_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sibType-r17");
  sib_type_r17.to_json(j);
  if (value_tag_r17_present) {
    j.write_int("valueTag-r17", value_tag_r17);
  }
  if (area_scope_r17_present) {
    j.write_str("areaScope-r17", "true");
  }
  j.end_obj();
}

void sib_type_info_v1700_s::sib_type_r17_c_::destroy_()
{
  switch (type_) {
    case types::type2_r17:
      c.destroy<type2_r17_s_>();
      break;
    default:
      break;
  }
}
void sib_type_info_v1700_s::sib_type_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::type1_r17:
      break;
    case types::type2_r17:
      c.init<type2_r17_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }
}
sib_type_info_v1700_s::sib_type_r17_c_::sib_type_r17_c_(const sib_type_info_v1700_s::sib_type_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::type1_r17:
      c.init(other.c.get<type1_r17_e_>());
      break;
    case types::type2_r17:
      c.init(other.c.get<type2_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }
}
sib_type_info_v1700_s::sib_type_r17_c_&
sib_type_info_v1700_s::sib_type_r17_c_::operator=(const sib_type_info_v1700_s::sib_type_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::type1_r17:
      c.set(other.c.get<type1_r17_e_>());
      break;
    case types::type2_r17:
      c.set(other.c.get<type2_r17_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }

  return *this;
}
sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_& sib_type_info_v1700_s::sib_type_r17_c_::set_type1_r17()
{
  set(types::type1_r17);
  return c.get<type1_r17_e_>();
}
sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_& sib_type_info_v1700_s::sib_type_r17_c_::set_type2_r17()
{
  set(types::type2_r17);
  return c.get<type2_r17_s_>();
}
void sib_type_info_v1700_s::sib_type_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::type1_r17:
      j.write_str("type1-r17", c.get<type1_r17_e_>().to_string());
      break;
    case types::type2_r17:
      j.write_fieldname("type2-r17");
      j.start_obj();
      j.write_str("posSibType-r17", c.get<type2_r17_s_>().pos_sib_type_r17.to_string());
      if (c.get<type2_r17_s_>().encrypted_r17_present) {
        j.write_str("encrypted-r17", "true");
      }
      if (c.get<type2_r17_s_>().gnss_id_r17_present) {
        j.write_fieldname("gnss-id-r17");
        c.get<type2_r17_s_>().gnss_id_r17.to_json(j);
      }
      if (c.get<type2_r17_s_>().sbas_id_r17_present) {
        j.write_fieldname("sbas-id-r17");
        c.get<type2_r17_s_>().sbas_id_r17.to_json(j);
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib_type_info_v1700_s::sib_type_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::type1_r17:
      HANDLE_CODE(c.get<type1_r17_e_>().pack(bref));
      break;
    case types::type2_r17:
      HANDLE_CODE(bref.pack(c.get<type2_r17_s_>().encrypted_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type2_r17_s_>().gnss_id_r17_present, 1));
      HANDLE_CODE(bref.pack(c.get<type2_r17_s_>().sbas_id_r17_present, 1));
      HANDLE_CODE(c.get<type2_r17_s_>().pos_sib_type_r17.pack(bref));
      if (c.get<type2_r17_s_>().gnss_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().gnss_id_r17.pack(bref));
      }
      if (c.get<type2_r17_s_>().sbas_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().sbas_id_r17.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type_info_v1700_s::sib_type_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::type1_r17:
      HANDLE_CODE(c.get<type1_r17_e_>().unpack(bref));
      break;
    case types::type2_r17:
      HANDLE_CODE(bref.unpack(c.get<type2_r17_s_>().encrypted_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type2_r17_s_>().gnss_id_r17_present, 1));
      HANDLE_CODE(bref.unpack(c.get<type2_r17_s_>().sbas_id_r17_present, 1));
      HANDLE_CODE(c.get<type2_r17_s_>().pos_sib_type_r17.unpack(bref));
      if (c.get<type2_r17_s_>().gnss_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().gnss_id_r17.unpack(bref));
      }
      if (c.get<type2_r17_s_>().sbas_id_r17_present) {
        HANDLE_CODE(c.get<type2_r17_s_>().sbas_id_r17.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "sib_type_info_v1700_s::sib_type_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_opts::to_string() const
{
  static const char* names[] = {"sibType15",
                                "sibType16",
                                "sibType17",
                                "sibType18",
                                "sibType19",
                                "sibType20",
                                "sibType21",
                                "spare9",
                                "spare8",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_");
}
uint8_t sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 16, 17, 18, 19, 20, 21};
  return map_enum_number(numbers, 7, value, "sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_");
}

const char* sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17_opts::to_string() const
{
  static const char* names[] = {"posSibType1-9",
                                "posSibType1-10",
                                "posSibType2-24",
                                "posSibType2-25",
                                "posSibType6-4",
                                "posSibType6-5",
                                "posSibType6-6",
                                "spare9",
                                "spare8",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(
      names, 16, value, "sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17_e_");
}

const char* sib_type_info_v1700_s::sib_type_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"type1-r17", "type2-r17"};
  return convert_enum_idx(names, 2, value, "sib_type_info_v1700_s::sib_type_r17_c_::types");
}
uint8_t sib_type_info_v1700_s::sib_type_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sib_type_info_v1700_s::sib_type_r17_c_::types");
}

// EUTRA-FreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE eutra_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(dummy.pack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(dummy.unpack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void eutra_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("dummy", dummy.to_string());
  if (q_rx_lev_min_offset_cell_present) {
    j.write_int("q-RxLevMinOffsetCell", q_rx_lev_min_offset_cell);
  }
  if (q_qual_min_offset_cell_present) {
    j.write_int("q-QualMinOffsetCell", q_qual_min_offset_cell);
  }
  j.end_obj();
}

// EUTRA-MultiBandInfo ::= SEQUENCE
SRSASN_CODE eutra_multi_band_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_ns_pmax_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, eutra_freq_band_ind, (uint16_t)1u, (uint16_t)256u));
  if (eutra_ns_pmax_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_ns_pmax_list, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_multi_band_info_s::unpack(cbit_ref& bref)
{
  bool eutra_ns_pmax_list_present;
  HANDLE_CODE(bref.unpack(eutra_ns_pmax_list_present, 1));

  HANDLE_CODE(unpack_integer(eutra_freq_band_ind, bref, (uint16_t)1u, (uint16_t)256u));
  if (eutra_ns_pmax_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_ns_pmax_list, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void eutra_multi_band_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("eutra-FreqBandIndicator", eutra_freq_band_ind);
  if (eutra_ns_pmax_list.size() > 0) {
    j.start_array("eutra-NS-PmaxList");
    for (const auto& e1 : eutra_ns_pmax_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* ephemeris_info_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"positionVelocity-r17", "orbital-r17"};
  return convert_enum_idx(names, 2, value, "ephemeris_info_r17_c::types");
}

// InterFreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.pack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell_sul, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.unpack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell_sul, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("q-OffsetCell", q_offset_cell.to_string());
  if (q_rx_lev_min_offset_cell_present) {
    j.write_int("q-RxLevMinOffsetCell", q_rx_lev_min_offset_cell);
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    j.write_int("q-RxLevMinOffsetCellSUL", q_rx_lev_min_offset_cell_sul);
  }
  if (q_qual_min_offset_cell_present) {
    j.write_int("q-QualMinOffsetCell", q_qual_min_offset_cell);
  }
  j.end_obj();
}

// InterFreqNeighCellInfo-v1610 ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r16_present) {
    j.write_str("ssb-PositionQCL-r16", ssb_position_qcl_r16.to_string());
  }
  j.end_obj();
}

// InterFreqNeighCellInfo-v1710 ::= SEQUENCE
SRSASN_CODE inter_freq_neigh_cell_info_v1710_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_neigh_cell_info_v1710_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_neigh_cell_info_v1710_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r17_present) {
    j.write_str("ssb-PositionQCL-r17", ssb_position_qcl_r17.to_string());
  }
  j.end_obj();
}

const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(names, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types");
}
float rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(
      numbers, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types");
}
const char* rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(
      number_strs, 8, value, "rach_cfg_common_s::ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_::types");
}

const char* rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::to_string() const
{
  static const char* names[] = {"l839", "l139"};
  return convert_enum_idx(names, 2, value, "rach_cfg_common_s::prach_root_seq_idx_c_::types");
}
uint16_t rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {839, 139};
  return map_enum_number(numbers, 2, value, "rach_cfg_common_s::prach_root_seq_idx_c_::types");
}

const char* rach_cfg_common_s::prach_root_seq_idx_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"l571", "l1151"};
  return convert_enum_idx(names, 2, value, "rach_cfg_common_s::prach_root_seq_idx_r16_c_::types");
}
uint16_t rach_cfg_common_s::prach_root_seq_idx_r16_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {571, 1151};
  return map_enum_number(numbers, 2, value, "rach_cfg_common_s::prach_root_seq_idx_r16_c_::types");
}

// SI-RequestConfig ::= SEQUENCE
SRSASN_CODE si_request_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rach_occasions_si_present, 1));
  HANDLE_CODE(bref.pack(si_request_period_present, 1));

  if (rach_occasions_si_present) {
    HANDLE_CODE(rach_occasions_si.rach_cfg_si.pack(bref));
    HANDLE_CODE(rach_occasions_si.ssb_per_rach_occasion.pack(bref));
  }
  if (si_request_period_present) {
    HANDLE_CODE(si_request_period.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, si_request_res, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_request_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rach_occasions_si_present, 1));
  HANDLE_CODE(bref.unpack(si_request_period_present, 1));

  if (rach_occasions_si_present) {
    HANDLE_CODE(rach_occasions_si.rach_cfg_si.unpack(bref));
    HANDLE_CODE(rach_occasions_si.ssb_per_rach_occasion.unpack(bref));
  }
  if (si_request_period_present) {
    HANDLE_CODE(si_request_period.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(si_request_res, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void si_request_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rach_occasions_si_present) {
    j.write_fieldname("rach-OccasionsSI");
    j.start_obj();
    j.write_fieldname("rach-ConfigSI");
    rach_occasions_si.rach_cfg_si.to_json(j);
    j.write_str("ssb-perRACH-Occasion", rach_occasions_si.ssb_per_rach_occasion.to_string());
    j.end_obj();
  }
  if (si_request_period_present) {
    j.write_str("si-RequestPeriod", si_request_period.to_string());
  }
  j.start_array("si-RequestResources");
  for (const auto& e1 : si_request_res) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_string() const
{
  static const char* names[] = {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
  return convert_enum_idx(names, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}
float si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(numbers, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}
const char* si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(number_strs, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}

const char* si_request_cfg_s::si_request_period_opts::to_string() const
{
  static const char* names[] = {"one", "two", "four", "six", "eight", "ten", "twelve", "sixteen"};
  return convert_enum_idx(names, 8, value, "si_request_cfg_s::si_request_period_e_");
}

// SliceInfo-r17 ::= SEQUENCE
SRSASN_CODE slice_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nsag_cell_resel_prio_r17_present, 1));
  HANDLE_CODE(bref.pack(nsag_cell_resel_sub_prio_r17_present, 1));
  HANDLE_CODE(bref.pack(slice_cell_list_nr_r17_present, 1));

  HANDLE_CODE(nsag_id_info_r17.pack(bref));
  if (nsag_cell_resel_prio_r17_present) {
    HANDLE_CODE(pack_integer(bref, nsag_cell_resel_prio_r17, (uint8_t)0u, (uint8_t)7u));
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    HANDLE_CODE(nsag_cell_resel_sub_prio_r17.pack(bref));
  }
  if (slice_cell_list_nr_r17_present) {
    HANDLE_CODE(slice_cell_list_nr_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE slice_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nsag_cell_resel_prio_r17_present, 1));
  HANDLE_CODE(bref.unpack(nsag_cell_resel_sub_prio_r17_present, 1));
  HANDLE_CODE(bref.unpack(slice_cell_list_nr_r17_present, 1));

  HANDLE_CODE(nsag_id_info_r17.unpack(bref));
  if (nsag_cell_resel_prio_r17_present) {
    HANDLE_CODE(unpack_integer(nsag_cell_resel_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    HANDLE_CODE(nsag_cell_resel_sub_prio_r17.unpack(bref));
  }
  if (slice_cell_list_nr_r17_present) {
    HANDLE_CODE(slice_cell_list_nr_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void slice_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nsag-IdentityInfo-r17");
  nsag_id_info_r17.to_json(j);
  if (nsag_cell_resel_prio_r17_present) {
    j.write_int("nsag-CellReselectionPriority-r17", nsag_cell_resel_prio_r17);
  }
  if (nsag_cell_resel_sub_prio_r17_present) {
    j.write_str("nsag-CellReselectionSubPriority-r17", nsag_cell_resel_sub_prio_r17.to_string());
  }
  if (slice_cell_list_nr_r17_present) {
    j.write_fieldname("sliceCellListNR-r17");
    slice_cell_list_nr_r17.to_json(j);
  }
  j.end_obj();
}

void slice_info_r17_s::slice_cell_list_nr_r17_c_::destroy_()
{
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.destroy<slice_cell_list_nr_r17_l>();
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.destroy<slice_cell_list_nr_r17_l>();
      break;
    default:
      break;
  }
}
void slice_info_r17_s::slice_cell_list_nr_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.init<slice_cell_list_nr_r17_l>();
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.init<slice_cell_list_nr_r17_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }
}
slice_info_r17_s::slice_cell_list_nr_r17_c_::slice_cell_list_nr_r17_c_(
    const slice_info_r17_s::slice_cell_list_nr_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.init(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.init(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }
}
slice_info_r17_s::slice_cell_list_nr_r17_c_&
slice_info_r17_s::slice_cell_list_nr_r17_c_::operator=(const slice_info_r17_s::slice_cell_list_nr_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      c.set(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::slice_excluded_cell_list_nr_r17:
      c.set(other.c.get<slice_cell_list_nr_r17_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }

  return *this;
}
slice_cell_list_nr_r17_l& slice_info_r17_s::slice_cell_list_nr_r17_c_::set_slice_allowed_cell_list_nr_r17()
{
  set(types::slice_allowed_cell_list_nr_r17);
  return c.get<slice_cell_list_nr_r17_l>();
}
slice_cell_list_nr_r17_l& slice_info_r17_s::slice_cell_list_nr_r17_c_::set_slice_excluded_cell_list_nr_r17()
{
  set(types::slice_excluded_cell_list_nr_r17);
  return c.get<slice_cell_list_nr_r17_l>();
}
void slice_info_r17_s::slice_cell_list_nr_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      j.start_array("sliceAllowedCellListNR-r17");
      for (const auto& e1 : c.get<slice_cell_list_nr_r17_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::slice_excluded_cell_list_nr_r17:
      j.start_array("sliceExcludedCellListNR-r17");
      for (const auto& e1 : c.get<slice_cell_list_nr_r17_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE slice_info_r17_s::slice_cell_list_nr_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<slice_cell_list_nr_r17_l>(), 1, 16));
      break;
    case types::slice_excluded_cell_list_nr_r17:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<slice_cell_list_nr_r17_l>(), 1, 16));
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE slice_info_r17_s::slice_cell_list_nr_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slice_allowed_cell_list_nr_r17:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<slice_cell_list_nr_r17_l>(), bref, 1, 16));
      break;
    case types::slice_excluded_cell_list_nr_r17:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<slice_cell_list_nr_r17_l>(), bref, 1, 16));
      break;
    default:
      log_invalid_choice_id(type_, "slice_info_r17_s::slice_cell_list_nr_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* slice_info_r17_s::slice_cell_list_nr_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"sliceAllowedCellListNR-r17", "sliceExcludedCellListNR-r17"};
  return convert_enum_idx(names, 2, value, "slice_info_r17_s::slice_cell_list_nr_r17_c_::types");
}

const char* coreset_s::cce_reg_map_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"interleaved", "nonInterleaved"};
  return convert_enum_idx(names, 2, value, "coreset_s::cce_reg_map_type_c_::types");
}

// InterFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
SRSASN_CODE inter_freq_cag_cell_list_per_plmn_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r16, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(pack_dyn_seq_of(bref, cag_cell_list_r16, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_cag_cell_list_per_plmn_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(plmn_id_idx_r16, bref, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(unpack_dyn_seq_of(cag_cell_list_r16, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void inter_freq_cag_cell_list_per_plmn_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r16", plmn_id_idx_r16);
  j.start_array("cag-CellList-r16");
  for (const auto& e1 : cag_cell_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// PDSCH-ConfigPTM-r17 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_ptm_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(data_scrambling_id_pdsch_r17_present, 1));
  HANDLE_CODE(bref.pack(dmrs_scrambling_id0_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_aggregation_factor_r17_present, 1));

  if (data_scrambling_id_pdsch_r17_present) {
    HANDLE_CODE(pack_integer(bref, data_scrambling_id_pdsch_r17, (uint16_t)0u, (uint16_t)1023u));
  }
  if (dmrs_scrambling_id0_r17_present) {
    HANDLE_CODE(pack_integer(bref, dmrs_scrambling_id0_r17, (uint32_t)0u, (uint32_t)65535u));
  }
  if (pdsch_aggregation_factor_r17_present) {
    HANDLE_CODE(pdsch_aggregation_factor_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_ptm_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(data_scrambling_id_pdsch_r17_present, 1));
  HANDLE_CODE(bref.unpack(dmrs_scrambling_id0_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_aggregation_factor_r17_present, 1));

  if (data_scrambling_id_pdsch_r17_present) {
    HANDLE_CODE(unpack_integer(data_scrambling_id_pdsch_r17, bref, (uint16_t)0u, (uint16_t)1023u));
  }
  if (dmrs_scrambling_id0_r17_present) {
    HANDLE_CODE(unpack_integer(dmrs_scrambling_id0_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  }
  if (pdsch_aggregation_factor_r17_present) {
    HANDLE_CODE(pdsch_aggregation_factor_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_ptm_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (data_scrambling_id_pdsch_r17_present) {
    j.write_int("dataScramblingIdentityPDSCH-r17", data_scrambling_id_pdsch_r17);
  }
  if (dmrs_scrambling_id0_r17_present) {
    j.write_int("dmrs-ScramblingID0-r17", dmrs_scrambling_id0_r17);
  }
  if (pdsch_aggregation_factor_r17_present) {
    j.write_str("pdsch-AggregationFactor-r17", pdsch_aggregation_factor_r17.to_string());
  }
  j.end_obj();
}

const char* pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n8"};
  return convert_enum_idx(names, 3, value, "pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_e_");
}
uint8_t pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_e_");
}

// PosSIB-Type-r16 ::= SEQUENCE
SRSASN_CODE pos_sib_type_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(encrypted_r16_present, 1));
  HANDLE_CODE(bref.pack(gnss_id_r16_present, 1));
  HANDLE_CODE(bref.pack(sbas_id_r16_present, 1));
  HANDLE_CODE(bref.pack(area_scope_r16_present, 1));

  if (gnss_id_r16_present) {
    HANDLE_CODE(gnss_id_r16.pack(bref));
  }
  if (sbas_id_r16_present) {
    HANDLE_CODE(sbas_id_r16.pack(bref));
  }
  HANDLE_CODE(pos_sib_type_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sib_type_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(encrypted_r16_present, 1));
  HANDLE_CODE(bref.unpack(gnss_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(sbas_id_r16_present, 1));
  HANDLE_CODE(bref.unpack(area_scope_r16_present, 1));

  if (gnss_id_r16_present) {
    HANDLE_CODE(gnss_id_r16.unpack(bref));
  }
  if (sbas_id_r16_present) {
    HANDLE_CODE(sbas_id_r16.unpack(bref));
  }
  HANDLE_CODE(pos_sib_type_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void pos_sib_type_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (encrypted_r16_present) {
    j.write_str("encrypted-r16", "true");
  }
  if (gnss_id_r16_present) {
    j.write_fieldname("gnss-id-r16");
    gnss_id_r16.to_json(j);
  }
  if (sbas_id_r16_present) {
    j.write_fieldname("sbas-id-r16");
    sbas_id_r16.to_json(j);
  }
  j.write_str("posSibType-r16", pos_sib_type_r16.to_string());
  if (area_scope_r16_present) {
    j.write_str("areaScope-r16", "true");
  }
  j.end_obj();
}

const char* pos_sib_type_r16_s::pos_sib_type_r16_opts::to_string() const
{
  static const char* names[] = {
      "posSibType1-1",  "posSibType1-2",  "posSibType1-3",  "posSibType1-4",  "posSibType1-5",  "posSibType1-6",
      "posSibType1-7",  "posSibType1-8",  "posSibType2-1",  "posSibType2-2",  "posSibType2-3",  "posSibType2-4",
      "posSibType2-5",  "posSibType2-6",  "posSibType2-7",  "posSibType2-8",  "posSibType2-9",  "posSibType2-10",
      "posSibType2-11", "posSibType2-12", "posSibType2-13", "posSibType2-14", "posSibType2-15", "posSibType2-16",
      "posSibType2-17", "posSibType2-18", "posSibType2-19", "posSibType2-20", "posSibType2-21", "posSibType2-22",
      "posSibType2-23", "posSibType3-1",  "posSibType4-1",  "posSibType5-1",  "posSibType6-1",  "posSibType6-2",
      "posSibType6-3"};
  return convert_enum_idx(names, 37, value, "pos_sib_type_r16_s::pos_sib_type_r16_e_");
}

const char* rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneSlot", "twoSlots"};
  return convert_enum_idx(
      names, 2, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types");
}
uint8_t rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::symbols_in_res_block_c_::types");
}

const char* rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n5", "n8", "n10", "n20", "n40"};
  return convert_enum_idx(
      names, 7, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types");
}
uint8_t rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 5, 8, 10, 20, 40};
  return map_enum_number(
      numbers, 7, value, "rate_match_pattern_s::pattern_type_c_::bitmaps_s_::periodicity_and_pattern_c_::types");
}

const char* rate_match_pattern_s::pattern_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"bitmaps", "controlResourceSet"};
  return convert_enum_idx(names, 2, value, "rate_match_pattern_s::pattern_type_c_::types");
}

// SI-SchedulingInfo-v1740 ::= SEQUENCE
SRSASN_CODE si_sched_info_v1740_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(si_request_cfg_red_cap_r17_present, 1));

  if (si_request_cfg_red_cap_r17_present) {
    HANDLE_CODE(si_request_cfg_red_cap_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_sched_info_v1740_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(si_request_cfg_red_cap_r17_present, 1));

  if (si_request_cfg_red_cap_r17_present) {
    HANDLE_CODE(si_request_cfg_red_cap_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void si_sched_info_v1740_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (si_request_cfg_red_cap_r17_present) {
    j.write_fieldname("si-RequestConfigRedCap-r17");
    si_request_cfg_red_cap_r17.to_json(j);
  }
  j.end_obj();
}

const char* ssb_mtc_s::periodicity_and_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"sf5", "sf10", "sf20", "sf40", "sf80", "sf160"};
  return convert_enum_idx(names, 6, value, "ssb_mtc_s::periodicity_and_offset_c_::types");
}
uint8_t ssb_mtc_s::periodicity_and_offset_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "ssb_mtc_s::periodicity_and_offset_c_::types");
}

// SSB-MTC2-LP-r16 ::= SEQUENCE
SRSASN_CODE ssb_mtc2_lp_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pci_list.size() > 0, 1));

  if (pci_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pci_list, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_mtc2_lp_r16_s::unpack(cbit_ref& bref)
{
  bool pci_list_present;
  HANDLE_CODE(bref.unpack(pci_list_present, 1));

  if (pci_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pci_list, bref, 1, 64, integer_packer<uint16_t>(0, 1007)));
  }
  HANDLE_CODE(periodicity.unpack(bref));

  return SRSASN_SUCCESS;
}
void ssb_mtc2_lp_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pci_list.size() > 0) {
    j.start_array("pci-List");
    for (const auto& e1 : pci_list) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_str("periodicity", periodicity.to_string());
  j.end_obj();
}

const char* ssb_mtc2_lp_r16_s::periodicity_opts::to_string() const
{
  static const char* names[] = {"sf10", "sf20", "sf40", "sf80", "sf160", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ssb_mtc2_lp_r16_s::periodicity_e_");
}
uint8_t ssb_mtc2_lp_r16_s::periodicity_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 80, 160};
  return map_enum_number(numbers, 5, value, "ssb_mtc2_lp_r16_s::periodicity_e_");
}

const char* ssb_to_measure_c::types_opts::to_string() const
{
  static const char* names[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
  return convert_enum_idx(names, 3, value, "ssb_to_measure_c::types");
}

// SchedulingInfo2-r17 ::= SEQUENCE
SRSASN_CODE sched_info2_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_broadcast_status_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, si_win_position_r17, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(si_periodicity_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info_r17, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info2_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_broadcast_status_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(si_win_position_r17, bref, (uint16_t)1u, (uint16_t)256u));
  HANDLE_CODE(si_periodicity_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info_r17, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void sched_info2_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-BroadcastStatus-r17", si_broadcast_status_r17.to_string());
  j.write_int("si-WindowPosition-r17", si_win_position_r17);
  j.write_str("si-Periodicity-r17", si_periodicity_r17.to_string());
  j.start_array("sib-MappingInfo-r17");
  for (const auto& e1 : sib_map_info_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* sched_info2_r17_s::si_broadcast_status_r17_opts::to_string() const
{
  static const char* names[] = {"broadcasting", "notBroadcasting"};
  return convert_enum_idx(names, 2, value, "sched_info2_r17_s::si_broadcast_status_r17_e_");
}

const char* sched_info2_r17_s::si_periodicity_r17_opts::to_string() const
{
  static const char* names[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(names, 7, value, "sched_info2_r17_s::si_periodicity_r17_e_");
}
uint16_t sched_info2_r17_s::si_periodicity_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "sched_info2_r17_s::si_periodicity_r17_e_");
}

const char* search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"sl1",
                                "sl2",
                                "sl4",
                                "sl5",
                                "sl8",
                                "sl10",
                                "sl16",
                                "sl20",
                                "sl40",
                                "sl80",
                                "sl160",
                                "sl320",
                                "sl640",
                                "sl1280",
                                "sl2560"};
  return convert_enum_idx(names, 15, value, "search_space_s::monitoring_slot_periodicity_and_offset_c_::types");
}
uint16_t search_space_s::monitoring_slot_periodicity_and_offset_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 5, 8, 10, 16, 20, 40, 80, 160, 320, 640, 1280, 2560};
  return map_enum_number(numbers, 15, value, "search_space_s::monitoring_slot_periodicity_and_offset_c_::types");
}

const char* search_space_s::search_space_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"common", "ue-Specific"};
  return convert_enum_idx(names, 2, value, "search_space_s::search_space_type_c_::types");
}

const char* search_space_ext_v1700_s::monitoring_slot_periodicity_and_offset_v1710_c_::types_opts::to_string() const
{
  static const char* names[] = {"sl32", "sl64", "sl128", "sl5120", "sl10240", "sl20480"};
  return convert_enum_idx(
      names, 6, value, "search_space_ext_v1700_s::monitoring_slot_periodicity_and_offset_v1710_c_::types");
}
uint16_t search_space_ext_v1700_s::monitoring_slot_periodicity_and_offset_v1710_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {32, 64, 128, 5120, 10240, 20480};
  return map_enum_number(
      numbers, 6, value, "search_space_ext_v1700_s::monitoring_slot_periodicity_and_offset_v1710_c_::types");
}

const char* search_space_ext_v1700_s::monitoring_slots_within_slot_group_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"slotGroupLength4-r17", "slotGroupLength8-r17"};
  return convert_enum_idx(
      names, 2, value, "search_space_ext_v1700_s::monitoring_slots_within_slot_group_r17_c_::types");
}
uint8_t search_space_ext_v1700_s::monitoring_slots_within_slot_group_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8};
  return map_enum_number(
      numbers, 2, value, "search_space_ext_v1700_s::monitoring_slots_within_slot_group_r17_c_::types");
}

// SpeedStateScaleFactors ::= SEQUENCE
SRSASN_CODE speed_state_scale_factors_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_medium.pack(bref));
  HANDLE_CODE(sf_high.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE speed_state_scale_factors_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_medium.unpack(bref));
  HANDLE_CODE(sf_high.unpack(bref));

  return SRSASN_SUCCESS;
}
void speed_state_scale_factors_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sf-Medium", sf_medium.to_string());
  j.write_str("sf-High", sf_high.to_string());
  j.end_obj();
}

const char* speed_state_scale_factors_s::sf_medium_opts::to_string() const
{
  static const char* names[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(names, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
float speed_state_scale_factors_s::sf_medium_opts::to_number() const
{
  static const float numbers[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(numbers, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
const char* speed_state_scale_factors_s::sf_medium_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(number_strs, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}

const char* speed_state_scale_factors_s::sf_high_opts::to_string() const
{
  static const char* names[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(names, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
float speed_state_scale_factors_s::sf_high_opts::to_number() const
{
  static const float numbers[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(numbers, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
const char* speed_state_scale_factors_s::sf_high_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(number_strs, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}

// UAC-BarringInfoSet-v1700 ::= SEQUENCE
SRSASN_CODE uac_barr_info_set_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_barr_factor_for_ai3_r17_present, 1));

  if (uac_barr_factor_for_ai3_r17_present) {
    HANDLE_CODE(uac_barr_factor_for_ai3_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_info_set_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_barr_factor_for_ai3_r17_present, 1));

  if (uac_barr_factor_for_ai3_r17_present) {
    HANDLE_CODE(uac_barr_factor_for_ai3_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void uac_barr_info_set_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (uac_barr_factor_for_ai3_r17_present) {
    j.write_str("uac-BarringFactorForAI3-r17", uac_barr_factor_for_ai3_r17.to_string());
  }
  j.end_obj();
}

const char* uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_string() const
{
  static const char* names[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(names, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}
float uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(numbers, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}
const char* uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(number_strs, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}

// CarrierFreqEUTRA ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_multi_band_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(eutra_excluded_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.pack(thresh_x_q_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq, (uint32_t)0u, (uint32_t)262143u));
  if (eutra_multi_band_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_multi_band_info_list, 1, 8));
  }
  if (eutra_freq_neigh_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_freq_neigh_cell_list, 1, 8));
  }
  if (eutra_excluded_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_excluded_cell_list, 1, 16));
  }
  HANDLE_CODE(allowed_meas_bw.pack(bref));
  HANDLE_CODE(bref.pack(presence_ant_port1, 1));
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(pack_integer(bref, q_qual_min, (int8_t)-34, (int8_t)-3));
  HANDLE_CODE(pack_integer(bref, p_max_eutra, (int8_t)-30, (int8_t)33));
  if (thresh_x_q_present) {
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_high_q, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_low_q, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_s::unpack(cbit_ref& bref)
{
  bool eutra_multi_band_info_list_present;
  HANDLE_CODE(bref.unpack(eutra_multi_band_info_list_present, 1));
  bool eutra_freq_neigh_cell_list_present;
  HANDLE_CODE(bref.unpack(eutra_freq_neigh_cell_list_present, 1));
  bool eutra_excluded_cell_list_present;
  HANDLE_CODE(bref.unpack(eutra_excluded_cell_list_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.unpack(thresh_x_q_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq, bref, (uint32_t)0u, (uint32_t)262143u));
  if (eutra_multi_band_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_multi_band_info_list, bref, 1, 8));
  }
  if (eutra_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_freq_neigh_cell_list, bref, 1, 8));
  }
  if (eutra_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_excluded_cell_list, bref, 1, 16));
  }
  HANDLE_CODE(allowed_meas_bw.unpack(bref));
  HANDLE_CODE(bref.unpack(presence_ant_port1, 1));
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  HANDLE_CODE(unpack_integer(q_qual_min, bref, (int8_t)-34, (int8_t)-3));
  HANDLE_CODE(unpack_integer(p_max_eutra, bref, (int8_t)-30, (int8_t)33));
  if (thresh_x_q_present) {
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_high_q, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_low_q, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq", carrier_freq);
  if (eutra_multi_band_info_list.size() > 0) {
    j.start_array("eutra-multiBandInfoList");
    for (const auto& e1 : eutra_multi_band_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (eutra_freq_neigh_cell_list.size() > 0) {
    j.start_array("eutra-FreqNeighCellList");
    for (const auto& e1 : eutra_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (eutra_excluded_cell_list.size() > 0) {
    j.start_array("eutra-ExcludedCellList");
    for (const auto& e1 : eutra_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("allowedMeasBandwidth", allowed_meas_bw.to_string());
  j.write_bool("presenceAntennaPort1", presence_ant_port1);
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  if (cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_sub_prio.to_string());
  }
  j.write_int("threshX-High", thresh_x_high);
  j.write_int("threshX-Low", thresh_x_low);
  j.write_int("q-RxLevMin", q_rx_lev_min);
  j.write_int("q-QualMin", q_qual_min);
  j.write_int("p-MaxEUTRA", p_max_eutra);
  if (thresh_x_q_present) {
    j.write_fieldname("threshX-Q");
    j.start_obj();
    j.write_int("threshX-HighQ", thresh_x_q.thresh_x_high_q);
    j.write_int("threshX-LowQ", thresh_x_q.thresh_x_low_q);
    j.end_obj();
  }
  j.end_obj();
}

// CarrierFreqEUTRA-v1610 ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(high_speed_eutra_carrier_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(high_speed_eutra_carrier_r16_present, 1));

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (high_speed_eutra_carrier_r16_present) {
    j.write_str("highSpeedEUTRACarrier-r16", "true");
  }
  j.end_obj();
}

// CarrierFreqEUTRA-v1700 ::= SEQUENCE
SRSASN_CODE carrier_freq_eutra_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(eutra_freq_neigh_hsdn_cell_list_r17.size() > 0, 1));

  if (eutra_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, eutra_freq_neigh_hsdn_cell_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE carrier_freq_eutra_v1700_s::unpack(cbit_ref& bref)
{
  bool eutra_freq_neigh_hsdn_cell_list_r17_present;
  HANDLE_CODE(bref.unpack(eutra_freq_neigh_hsdn_cell_list_r17_present, 1));

  if (eutra_freq_neigh_hsdn_cell_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(eutra_freq_neigh_hsdn_cell_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void carrier_freq_eutra_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eutra_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    j.start_array("eutra-FreqNeighHSDN-CellList-r17");
    for (const auto& e1 : eutra_freq_neigh_hsdn_cell_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// FreqPrioritySlicing-r17 ::= SEQUENCE
SRSASN_CODE freq_prio_slicing_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(slice_info_list_r17.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, dl_implicit_carrier_freq_r17, (uint8_t)0u, (uint8_t)8u));
  if (slice_info_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slice_info_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_prio_slicing_r17_s::unpack(cbit_ref& bref)
{
  bool slice_info_list_r17_present;
  HANDLE_CODE(bref.unpack(slice_info_list_r17_present, 1));

  HANDLE_CODE(unpack_integer(dl_implicit_carrier_freq_r17, bref, (uint8_t)0u, (uint8_t)8u));
  if (slice_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slice_info_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void freq_prio_slicing_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-ImplicitCarrierFreq-r17", dl_implicit_carrier_freq_r17);
  if (slice_info_list_r17.size() > 0) {
    j.start_array("sliceInfoList-r17");
    for (const auto& e1 : slice_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// HRNN-r16 ::= SEQUENCE
SRSASN_CODE hrnn_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(hrnn_r16.size() > 0, 1));

  if (hrnn_r16.size() > 0) {
    HANDLE_CODE(hrnn_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE hrnn_r16_s::unpack(cbit_ref& bref)
{
  bool hrnn_r16_present;
  HANDLE_CODE(bref.unpack(hrnn_r16_present, 1));

  if (hrnn_r16_present) {
    HANDLE_CODE(hrnn_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void hrnn_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hrnn_r16.size() > 0) {
    j.write_str("hrnn-r16", hrnn_r16.to_string());
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_list_sul.size() > 0, 1));
  HANDLE_CODE(bref.pack(nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.pack(abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.pack(smtc_present, 1));
  HANDLE_CODE(bref.pack(ssb_to_measure_present, 1));
  HANDLE_CODE(bref.pack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.pack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_present, 1));
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(t_resel_nr_sf_present, 1));
  HANDLE_CODE(bref.pack(thresh_x_q_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.pack(q_offset_freq_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(inter_freq_excluded_cell_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, dl_carrier_freq, (uint32_t)0u, (uint32_t)3279165u));
  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8));
  }
  if (freq_band_list_sul.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_sul, 1, 8));
  }
  if (nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(pack_integer(bref, nrof_ss_blocks_to_average, (uint8_t)2u, (uint8_t)16u));
  }
  if (abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(abs_thresh_ss_blocks_consolidation.pack(bref));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.pack(bref));
  }
  HANDLE_CODE(ssb_subcarrier_spacing.pack(bref));
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.pack(bref));
  }
  HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell, 1));
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_sul, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min, (int8_t)-43, (int8_t)-12));
  }
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_nr, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_nr_sf_present) {
    HANDLE_CODE(t_resel_nr_sf.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, thresh_x_high_p, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, thresh_x_low_p, (uint8_t)0u, (uint8_t)31u));
  if (thresh_x_q_present) {
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_high_q, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, thresh_x_q.thresh_x_low_q, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_prio_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.pack(bref));
  }
  if (q_offset_freq_present) {
    HANDLE_CODE(q_offset_freq.pack(bref));
  }
  if (inter_freq_neigh_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list, 1, 16));
  }
  if (inter_freq_excluded_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_excluded_cell_list, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  bool freq_band_list_sul_present;
  HANDLE_CODE(bref.unpack(freq_band_list_sul_present, 1));
  HANDLE_CODE(bref.unpack(nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.unpack(abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.unpack(smtc_present, 1));
  HANDLE_CODE(bref.unpack(ssb_to_measure_present, 1));
  HANDLE_CODE(bref.unpack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.unpack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_present, 1));
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_nr_sf_present, 1));
  HANDLE_CODE(bref.unpack(thresh_x_q_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_prio_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_sub_prio_present, 1));
  HANDLE_CODE(bref.unpack(q_offset_freq_present, 1));
  bool inter_freq_neigh_cell_list_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_present, 1));
  bool inter_freq_excluded_cell_list_present;
  HANDLE_CODE(bref.unpack(inter_freq_excluded_cell_list_present, 1));

  HANDLE_CODE(unpack_integer(dl_carrier_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8));
  }
  if (freq_band_list_sul_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_sul, bref, 1, 8));
  }
  if (nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(unpack_integer(nrof_ss_blocks_to_average, bref, (uint8_t)2u, (uint8_t)16u));
  }
  if (abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(abs_thresh_ss_blocks_consolidation.unpack(bref));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.unpack(bref));
  }
  HANDLE_CODE(ssb_subcarrier_spacing.unpack(bref));
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell, 1));
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_sul, bref, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(unpack_integer(q_qual_min, bref, (int8_t)-43, (int8_t)-12));
  }
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }
  HANDLE_CODE(unpack_integer(t_resel_nr, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_nr_sf_present) {
    HANDLE_CODE(t_resel_nr_sf.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(thresh_x_high_p, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(thresh_x_low_p, bref, (uint8_t)0u, (uint8_t)31u));
  if (thresh_x_q_present) {
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_high_q, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(thresh_x_q.thresh_x_low_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_prio_present) {
    HANDLE_CODE(unpack_integer(cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_sub_prio.unpack(bref));
  }
  if (q_offset_freq_present) {
    HANDLE_CODE(q_offset_freq.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list, bref, 1, 16));
  }
  if (inter_freq_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_excluded_cell_list, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq", dl_carrier_freq);
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_list_sul.size() > 0) {
    j.start_array("frequencyBandListSUL");
    for (const auto& e1 : freq_band_list_sul) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nrof_ss_blocks_to_average_present) {
    j.write_int("nrofSS-BlocksToAverage", nrof_ss_blocks_to_average);
  }
  if (abs_thresh_ss_blocks_consolidation_present) {
    j.write_fieldname("absThreshSS-BlocksConsolidation");
    abs_thresh_ss_blocks_consolidation.to_json(j);
  }
  if (smtc_present) {
    j.write_fieldname("smtc");
    smtc.to_json(j);
  }
  j.write_str("ssbSubcarrierSpacing", ssb_subcarrier_spacing.to_string());
  if (ssb_to_measure_present) {
    j.write_fieldname("ssb-ToMeasure");
    ssb_to_measure.to_json(j);
  }
  j.write_bool("deriveSSB-IndexFromCell", derive_ssb_idx_from_cell);
  if (ss_rssi_meas_present) {
    j.write_fieldname("ss-RSSI-Measurement");
    ss_rssi_meas.to_json(j);
  }
  j.write_int("q-RxLevMin", q_rx_lev_min);
  if (q_rx_lev_min_sul_present) {
    j.write_int("q-RxLevMinSUL", q_rx_lev_min_sul);
  }
  if (q_qual_min_present) {
    j.write_int("q-QualMin", q_qual_min);
  }
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  j.write_int("t-ReselectionNR", t_resel_nr);
  if (t_resel_nr_sf_present) {
    j.write_fieldname("t-ReselectionNR-SF");
    t_resel_nr_sf.to_json(j);
  }
  j.write_int("threshX-HighP", thresh_x_high_p);
  j.write_int("threshX-LowP", thresh_x_low_p);
  if (thresh_x_q_present) {
    j.write_fieldname("threshX-Q");
    j.start_obj();
    j.write_int("threshX-HighQ", thresh_x_q.thresh_x_high_q);
    j.write_int("threshX-LowQ", thresh_x_q.thresh_x_low_q);
    j.end_obj();
  }
  if (cell_resel_prio_present) {
    j.write_int("cellReselectionPriority", cell_resel_prio);
  }
  if (cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_sub_prio.to_string());
  }
  if (q_offset_freq_present) {
    j.write_str("q-OffsetFreq", q_offset_freq.to_string());
  }
  if (inter_freq_neigh_cell_list.size() > 0) {
    j.start_array("interFreqNeighCellList");
    for (const auto& e1 : inter_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inter_freq_excluded_cell_list.size() > 0) {
    j.start_array("interFreqExcludedCellList");
    for (const auto& e1 : inter_freq_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1610 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_v1610.size() > 0, 1));
  HANDLE_CODE(bref.pack(smtc2_lp_r16_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_allowed_cell_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(ssb_position_qcl_common_r16_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_cag_cell_list_r16.size() > 0, 1));

  if (inter_freq_neigh_cell_list_v1610.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list_v1610, 1, 16));
  }
  if (smtc2_lp_r16_present) {
    HANDLE_CODE(smtc2_lp_r16.pack(bref));
  }
  if (inter_freq_allowed_cell_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_allowed_cell_list_r16, 1, 16));
  }
  if (ssb_position_qcl_common_r16_present) {
    HANDLE_CODE(ssb_position_qcl_common_r16.pack(bref));
  }
  if (inter_freq_cag_cell_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_cag_cell_list_r16, 1, 12));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1610_s::unpack(cbit_ref& bref)
{
  bool inter_freq_neigh_cell_list_v1610_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_v1610_present, 1));
  HANDLE_CODE(bref.unpack(smtc2_lp_r16_present, 1));
  bool inter_freq_allowed_cell_list_r16_present;
  HANDLE_CODE(bref.unpack(inter_freq_allowed_cell_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r16_present, 1));
  bool inter_freq_cag_cell_list_r16_present;
  HANDLE_CODE(bref.unpack(inter_freq_cag_cell_list_r16_present, 1));

  if (inter_freq_neigh_cell_list_v1610_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list_v1610, bref, 1, 16));
  }
  if (smtc2_lp_r16_present) {
    HANDLE_CODE(smtc2_lp_r16.unpack(bref));
  }
  if (inter_freq_allowed_cell_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_allowed_cell_list_r16, bref, 1, 16));
  }
  if (ssb_position_qcl_common_r16_present) {
    HANDLE_CODE(ssb_position_qcl_common_r16.unpack(bref));
  }
  if (inter_freq_cag_cell_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_cag_cell_list_r16, bref, 1, 12));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_neigh_cell_list_v1610.size() > 0) {
    j.start_array("interFreqNeighCellList-v1610");
    for (const auto& e1 : inter_freq_neigh_cell_list_v1610) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (smtc2_lp_r16_present) {
    j.write_fieldname("smtc2-LP-r16");
    smtc2_lp_r16.to_json(j);
  }
  if (inter_freq_allowed_cell_list_r16.size() > 0) {
    j.start_array("interFreqAllowedCellList-r16");
    for (const auto& e1 : inter_freq_allowed_cell_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ssb_position_qcl_common_r16_present) {
    j.write_str("ssb-PositionQCL-Common-r16", ssb_position_qcl_common_r16.to_string());
  }
  if (inter_freq_cag_cell_list_r16.size() > 0) {
    j.start_array("interFreqCAG-CellList-r16");
    for (const auto& e1 : inter_freq_cag_cell_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1700 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(inter_freq_neigh_hsdn_cell_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(high_speed_meas_inter_freq_r17_present, 1));
  HANDLE_CODE(bref.pack(red_cap_access_allowed_r17_present, 1));
  HANDLE_CODE(bref.pack(ssb_position_qcl_common_r17_present, 1));
  HANDLE_CODE(bref.pack(inter_freq_neigh_cell_list_v1710.size() > 0, 1));

  if (inter_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_hsdn_cell_list_r17, 1, 16));
  }
  if (ssb_position_qcl_common_r17_present) {
    HANDLE_CODE(ssb_position_qcl_common_r17.pack(bref));
  }
  if (inter_freq_neigh_cell_list_v1710.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_neigh_cell_list_v1710, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1700_s::unpack(cbit_ref& bref)
{
  bool inter_freq_neigh_hsdn_cell_list_r17_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_hsdn_cell_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_meas_inter_freq_r17_present, 1));
  HANDLE_CODE(bref.unpack(red_cap_access_allowed_r17_present, 1));
  HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r17_present, 1));
  bool inter_freq_neigh_cell_list_v1710_present;
  HANDLE_CODE(bref.unpack(inter_freq_neigh_cell_list_v1710_present, 1));

  if (inter_freq_neigh_hsdn_cell_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_hsdn_cell_list_r17, bref, 1, 16));
  }
  if (ssb_position_qcl_common_r17_present) {
    HANDLE_CODE(ssb_position_qcl_common_r17.unpack(bref));
  }
  if (inter_freq_neigh_cell_list_v1710_present) {
    HANDLE_CODE(unpack_dyn_seq_of(inter_freq_neigh_cell_list_v1710, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (inter_freq_neigh_hsdn_cell_list_r17.size() > 0) {
    j.start_array("interFreqNeighHSDN-CellList-r17");
    for (const auto& e1 : inter_freq_neigh_hsdn_cell_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (high_speed_meas_inter_freq_r17_present) {
    j.write_str("highSpeedMeasInterFreq-r17", "true");
  }
  if (red_cap_access_allowed_r17_present) {
    j.write_str("redCapAccessAllowed-r17", "true");
  }
  if (ssb_position_qcl_common_r17_present) {
    j.write_str("ssb-PositionQCL-Common-r17", ssb_position_qcl_common_r17.to_string());
  }
  if (inter_freq_neigh_cell_list_v1710.size() > 0) {
    j.start_array("interFreqNeighCellList-v1710");
    for (const auto& e1 : inter_freq_neigh_cell_list_v1710) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1720 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1720_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(smtc4list_r17.size() > 0, 1));

  if (smtc4list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, smtc4list_r17, 1, 3));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1720_s::unpack(cbit_ref& bref)
{
  bool smtc4list_r17_present;
  HANDLE_CODE(bref.unpack(smtc4list_r17_present, 1));

  if (smtc4list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(smtc4list_r17, bref, 1, 3));
  }

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1720_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (smtc4list_r17.size() > 0) {
    j.start_array("smtc4list-r17");
    for (const auto& e1 : smtc4list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// InterFreqCarrierFreqInfo-v1730 ::= SEQUENCE
SRSASN_CODE inter_freq_carrier_freq_info_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ch_access_mode2_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE inter_freq_carrier_freq_info_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ch_access_mode2_r17_present, 1));

  return SRSASN_SUCCESS;
}
void inter_freq_carrier_freq_info_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ch_access_mode2_r17_present) {
    j.write_str("channelAccessMode2-r17", "enabled");
  }
  j.end_obj();
}

// IntraFreqNeighCellInfo ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.pack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.pack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_offset_cell_sul, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min_offset_cell, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_present, 1));
  HANDLE_CODE(bref.unpack(q_rx_lev_min_offset_cell_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_offset_cell_present, 1));

  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(q_offset_cell.unpack(bref));
  if (q_rx_lev_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_offset_cell_sul, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (q_qual_min_offset_cell_present) {
    HANDLE_CODE(unpack_integer(q_qual_min_offset_cell, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId", pci);
  j.write_str("q-OffsetCell", q_offset_cell.to_string());
  if (q_rx_lev_min_offset_cell_present) {
    j.write_int("q-RxLevMinOffsetCell", q_rx_lev_min_offset_cell);
  }
  if (q_rx_lev_min_offset_cell_sul_present) {
    j.write_int("q-RxLevMinOffsetCellSUL", q_rx_lev_min_offset_cell_sul);
  }
  if (q_qual_min_offset_cell_present) {
    j.write_int("q-QualMinOffsetCell", q_qual_min_offset_cell);
  }
  j.end_obj();
}

// IntraFreqNeighCellInfo-v1610 ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r16_present, 1));

  if (ssb_position_qcl_r16_present) {
    HANDLE_CODE(ssb_position_qcl_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r16_present) {
    j.write_str("ssb-PositionQCL-r16", ssb_position_qcl_r16.to_string());
  }
  j.end_obj();
}

// IntraFreqNeighCellInfo-v1710 ::= SEQUENCE
SRSASN_CODE intra_freq_neigh_cell_info_v1710_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_neigh_cell_info_v1710_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_position_qcl_r17_present, 1));

  if (ssb_position_qcl_r17_present) {
    HANDLE_CODE(ssb_position_qcl_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void intra_freq_neigh_cell_info_v1710_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ssb_position_qcl_r17_present) {
    j.write_str("ssb-PositionQCL-r17", ssb_position_qcl_r17.to_string());
  }
  j.end_obj();
}

// LocationAndBandwidthBroadcast-r17 ::= CHOICE
void location_and_bw_broadcast_r17_c::set(types::options e)
{
  type_ = e;
}
void location_and_bw_broadcast_r17_c::set_same_as_sib1_cfg_location_and_bw()
{
  set(types::same_as_sib1_cfg_location_and_bw);
}
uint16_t& location_and_bw_broadcast_r17_c::set_location_and_bw()
{
  set(types::location_and_bw);
  return c;
}
void location_and_bw_broadcast_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::same_as_sib1_cfg_location_and_bw:
      break;
    case types::location_and_bw:
      j.write_int("locationAndBandwidth", c);
      break;
    default:
      log_invalid_choice_id(type_, "location_and_bw_broadcast_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE location_and_bw_broadcast_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::same_as_sib1_cfg_location_and_bw:
      break;
    case types::location_and_bw:
      HANDLE_CODE(pack_integer(bref, c, (uint16_t)0u, (uint16_t)37949u));
      break;
    default:
      log_invalid_choice_id(type_, "location_and_bw_broadcast_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE location_and_bw_broadcast_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::same_as_sib1_cfg_location_and_bw:
      break;
    case types::location_and_bw:
      HANDLE_CODE(unpack_integer(c, bref, (uint16_t)0u, (uint16_t)37949u));
      break;
    default:
      log_invalid_choice_id(type_, "location_and_bw_broadcast_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* location_and_bw_broadcast_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"sameAsSib1ConfiguredLocationAndBW", "locationAndBandwidth"};
  return convert_enum_idx(names, 2, value, "location_and_bw_broadcast_r17_c::types");
}
uint8_t location_and_bw_broadcast_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "location_and_bw_broadcast_r17_c::types");
}

// MBS-FSAI-InterFreq-r17 ::= SEQUENCE
SRSASN_CODE mbs_fsai_inter_freq_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, dl_carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, mbs_fsai_list_r17, 1, 64));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_fsai_inter_freq_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(dl_carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(mbs_fsai_list_r17, bref, 1, 64));

  return SRSASN_SUCCESS;
}
void mbs_fsai_inter_freq_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-CarrierFreq-r17", dl_carrier_freq_r17);
  j.start_array("mbs-FSAI-List-r17");
  for (const auto& e1 : mbs_fsai_list_r17) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// MCCH-RepetitionPeriodAndOffset-r17 ::= CHOICE
void mcch_repeat_period_and_offset_r17_c::destroy_() {}
void mcch_repeat_period_and_offset_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
mcch_repeat_period_and_offset_r17_c::mcch_repeat_period_and_offset_r17_c(
    const mcch_repeat_period_and_offset_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rf1_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf2_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf4_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf8_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf16_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf32_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf64_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf128_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rf256_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
  }
}
mcch_repeat_period_and_offset_r17_c&
mcch_repeat_period_and_offset_r17_c::operator=(const mcch_repeat_period_and_offset_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rf1_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf2_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf4_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf8_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf16_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf32_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf64_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf128_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rf256_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
  }

  return *this;
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf1_r17()
{
  set(types::rf1_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf2_r17()
{
  set(types::rf2_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf4_r17()
{
  set(types::rf4_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf8_r17()
{
  set(types::rf8_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf16_r17()
{
  set(types::rf16_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf32_r17()
{
  set(types::rf32_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf64_r17()
{
  set(types::rf64_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf128_r17()
{
  set(types::rf128_r17);
  return c.get<uint8_t>();
}
uint16_t& mcch_repeat_period_and_offset_r17_c::set_rf256_r17()
{
  set(types::rf256_r17);
  return c.get<uint16_t>();
}
void mcch_repeat_period_and_offset_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rf1_r17:
      j.write_int("rf1-r17", c.get<uint8_t>());
      break;
    case types::rf2_r17:
      j.write_int("rf2-r17", c.get<uint8_t>());
      break;
    case types::rf4_r17:
      j.write_int("rf4-r17", c.get<uint8_t>());
      break;
    case types::rf8_r17:
      j.write_int("rf8-r17", c.get<uint8_t>());
      break;
    case types::rf16_r17:
      j.write_int("rf16-r17", c.get<uint8_t>());
      break;
    case types::rf32_r17:
      j.write_int("rf32-r17", c.get<uint8_t>());
      break;
    case types::rf64_r17:
      j.write_int("rf64-r17", c.get<uint8_t>());
      break;
    case types::rf128_r17:
      j.write_int("rf128-r17", c.get<uint8_t>());
      break;
    case types::rf256_r17:
      j.write_int("rf256-r17", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE mcch_repeat_period_and_offset_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rf1_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)0u));
      break;
    case types::rf2_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::rf4_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    case types::rf8_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u));
      break;
    case types::rf16_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)15u));
      break;
    case types::rf32_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::rf64_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::rf128_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::rf256_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_repeat_period_and_offset_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rf1_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)0u));
      break;
    case types::rf2_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::rf4_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    case types::rf8_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::rf16_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)15u));
      break;
    case types::rf32_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::rf64_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::rf128_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::rf256_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mcch_repeat_period_and_offset_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mcch_repeat_period_and_offset_r17_c::types_opts::to_string() const
{
  static const char* names[] = {
      "rf1-r17", "rf2-r17", "rf4-r17", "rf8-r17", "rf16-r17", "rf32-r17", "rf64-r17", "rf128-r17", "rf256-r17"};
  return convert_enum_idx(names, 9, value, "mcch_repeat_period_and_offset_r17_c::types");
}
uint16_t mcch_repeat_period_and_offset_r17_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(numbers, 9, value, "mcch_repeat_period_and_offset_r17_c::types");
}

const char* npn_id_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"pni-npn-r16", "snpn-r16"};
  return convert_enum_idx(names, 2, value, "npn_id_r16_c::types");
}

// NTN-NeighCellConfig-r17 ::= SEQUENCE
SRSASN_CODE ntn_neigh_cell_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(carrier_freq_r17_present, 1));
  HANDLE_CODE(bref.pack(pci_r17_present, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.pack(bref));
  }
  if (carrier_freq_r17_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (pci_r17_present) {
    HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ntn_neigh_cell_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(carrier_freq_r17_present, 1));
  HANDLE_CODE(bref.unpack(pci_r17_present, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.unpack(bref));
  }
  if (carrier_freq_r17_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (pci_r17_present) {
    HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  }

  return SRSASN_SUCCESS;
}
void ntn_neigh_cell_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ntn_cfg_r17_present) {
    j.write_fieldname("ntn-Config-r17");
    ntn_cfg_r17.to_json(j);
  }
  if (carrier_freq_r17_present) {
    j.write_int("carrierFreq-r17", carrier_freq_r17);
  }
  if (pci_r17_present) {
    j.write_int("physCellId-r17", pci_r17);
  }
  j.end_obj();
}

const char* pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::types_opts::to_string() const
{
  static const char* names[] = {"sCS15KHZoneT",
                                "sCS30KHZoneT-SCS15KHZhalfT",
                                "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT",
                                "sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                                "sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                                "sCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                                "sCS120KHZoneEighthT-SCS60KHZoneSixteenthT",
                                "sCS120KHZoneSixteenthT"};
  return convert_enum_idx(names, 8, value, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_c_::types");
}

const char* pdcch_cfg_common_s::sdt_search_space_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"newSearchSpace", "existingSearchSpace"};
  return convert_enum_idx(names, 2, value, "pdcch_cfg_common_s::sdt_search_space_r17_c_::types");
}

const char* pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types_opts::to_string() const
{
  static const char* names[] = {"sCS480KHZoneEighthT", "sCS480KHZoneSixteenthT"};
  return convert_enum_idx(names, 2, value, "pdcch_cfg_common_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types");
}

const char*
pdcch_cfg_common_s::pei_cfg_bwp_r17_s_::first_pdcch_monitoring_occasion_of_pei_o_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"sCS15KHZoneT",
                                "sCS30KHZoneT-SCS15KHZhalfT",
                                "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT",
                                "sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
                                "sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
                                "sCS480KHZoneT-SCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
                                "sCS480KHZhalfT-SCS120KHZoneEighthT-SCS60KHZoneSixteenthT",
                                "sCS480KHZquarterT-SCS120KHZoneSixteenthT",
                                "sCS480KHZoneEighthT",
                                "sCS480KHZoneSixteenthT"};
  return convert_enum_idx(
      names,
      10,
      value,
      "pdcch_cfg_common_s::pei_cfg_bwp_r17_s_::first_pdcch_monitoring_occasion_of_pei_o_r17_c_::types");
}

// PDSCH-ConfigBroadcast-r17 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_broadcast_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdsch_time_domain_alloc_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(rate_match_pattern_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(lte_crs_to_match_around_r17_present, 1));
  HANDLE_CODE(bref.pack(mcs_table_r17_present, 1));
  HANDLE_CODE(bref.pack(xoverhead_r17_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pdsch_cfg_list_r17, 1, 16));
  if (pdsch_time_domain_alloc_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pdsch_time_domain_alloc_list_r17, 1, 16));
  }
  if (rate_match_pattern_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rate_match_pattern_to_add_mod_list_r17, 1, 4));
  }
  if (lte_crs_to_match_around_r17_present) {
    HANDLE_CODE(lte_crs_to_match_around_r17.pack(bref));
  }
  if (mcs_table_r17_present) {
    HANDLE_CODE(mcs_table_r17.pack(bref));
  }
  if (xoverhead_r17_present) {
    HANDLE_CODE(xoverhead_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_broadcast_r17_s::unpack(cbit_ref& bref)
{
  bool pdsch_time_domain_alloc_list_r17_present;
  HANDLE_CODE(bref.unpack(pdsch_time_domain_alloc_list_r17_present, 1));
  bool rate_match_pattern_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(rate_match_pattern_to_add_mod_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(lte_crs_to_match_around_r17_present, 1));
  HANDLE_CODE(bref.unpack(mcs_table_r17_present, 1));
  HANDLE_CODE(bref.unpack(xoverhead_r17_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pdsch_cfg_list_r17, bref, 1, 16));
  if (pdsch_time_domain_alloc_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pdsch_time_domain_alloc_list_r17, bref, 1, 16));
  }
  if (rate_match_pattern_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rate_match_pattern_to_add_mod_list_r17, bref, 1, 4));
  }
  if (lte_crs_to_match_around_r17_present) {
    HANDLE_CODE(lte_crs_to_match_around_r17.unpack(bref));
  }
  if (mcs_table_r17_present) {
    HANDLE_CODE(mcs_table_r17.unpack(bref));
  }
  if (xoverhead_r17_present) {
    HANDLE_CODE(xoverhead_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("pdschConfigList-r17");
  for (const auto& e1 : pdsch_cfg_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (pdsch_time_domain_alloc_list_r17.size() > 0) {
    j.start_array("pdsch-TimeDomainAllocationList-r17");
    for (const auto& e1 : pdsch_time_domain_alloc_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rate_match_pattern_to_add_mod_list_r17.size() > 0) {
    j.start_array("rateMatchPatternToAddModList-r17");
    for (const auto& e1 : rate_match_pattern_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (lte_crs_to_match_around_r17_present) {
    j.write_fieldname("lte-CRS-ToMatchAround-r17");
    lte_crs_to_match_around_r17.to_json(j);
  }
  if (mcs_table_r17_present) {
    j.write_str("mcs-Table-r17", mcs_table_r17.to_string());
  }
  if (xoverhead_r17_present) {
    j.write_str("xOverhead-r17", xoverhead_r17.to_string());
  }
  j.end_obj();
}

const char* pdsch_cfg_broadcast_r17_s::mcs_table_r17_opts::to_string() const
{
  static const char* names[] = {"qam256", "qam64LowSE"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_broadcast_r17_s::mcs_table_r17_e_");
}
uint16_t pdsch_cfg_broadcast_r17_s::mcs_table_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_broadcast_r17_s::mcs_table_r17_e_");
}

const char* pdsch_cfg_broadcast_r17_s::xoverhead_r17_opts::to_string() const
{
  static const char* names[] = {"xOh6", "xOh12", "xOh18"};
  return convert_enum_idx(names, 3, value, "pdsch_cfg_broadcast_r17_s::xoverhead_r17_e_");
}
uint8_t pdsch_cfg_broadcast_r17_s::xoverhead_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12, 18};
  return map_enum_number(numbers, 3, value, "pdsch_cfg_broadcast_r17_s::xoverhead_r17_e_");
}

// RedCap-ConfigCommonSIB-r17 ::= SEQUENCE
SRSASN_CODE red_cap_cfg_common_sib_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(half_duplex_red_cap_allowed_r17_present, 1));
  HANDLE_CODE(bref.pack(cell_barred_red_cap_r17_present, 1));

  if (cell_barred_red_cap_r17_present) {
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap1_rx_r17.pack(bref));
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap2_rx_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE red_cap_cfg_common_sib_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(half_duplex_red_cap_allowed_r17_present, 1));
  HANDLE_CODE(bref.unpack(cell_barred_red_cap_r17_present, 1));

  if (cell_barred_red_cap_r17_present) {
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap1_rx_r17.unpack(bref));
    HANDLE_CODE(cell_barred_red_cap_r17.cell_barred_red_cap2_rx_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void red_cap_cfg_common_sib_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (half_duplex_red_cap_allowed_r17_present) {
    j.write_str("halfDuplexRedCapAllowed-r17", "true");
  }
  if (cell_barred_red_cap_r17_present) {
    j.write_fieldname("cellBarredRedCap-r17");
    j.start_obj();
    j.write_str("cellBarredRedCap1Rx-r17", cell_barred_red_cap_r17.cell_barred_red_cap1_rx_r17.to_string());
    j.write_str("cellBarredRedCap2Rx-r17", cell_barred_red_cap_r17.cell_barred_red_cap2_rx_r17.to_string());
    j.end_obj();
  }
  j.end_obj();
}

const char*
red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17_opts::to_string() const
{
  static const char* names[] = {"barred", "notBarred"};
  return convert_enum_idx(
      names, 2, value, "red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17_e_");
}

const char*
red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17_opts::to_string() const
{
  static const char* names[] = {"barred", "notBarred"};
  return convert_enum_idx(
      names, 2, value, "red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17_e_");
}

// SDT-ConfigCommonSIB-r17 ::= SEQUENCE
SRSASN_CODE sdt_cfg_common_sib_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sdt_rsrp_thres_r17_present, 1));
  HANDLE_CODE(bref.pack(sdt_lc_ch_sr_delay_timer_r17_present, 1));

  if (sdt_rsrp_thres_r17_present) {
    HANDLE_CODE(pack_integer(bref, sdt_rsrp_thres_r17, (uint8_t)0u, (uint8_t)127u));
  }
  if (sdt_lc_ch_sr_delay_timer_r17_present) {
    HANDLE_CODE(sdt_lc_ch_sr_delay_timer_r17.pack(bref));
  }
  HANDLE_CODE(sdt_data_volume_thres_r17.pack(bref));
  HANDLE_CODE(t319a_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdt_cfg_common_sib_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sdt_rsrp_thres_r17_present, 1));
  HANDLE_CODE(bref.unpack(sdt_lc_ch_sr_delay_timer_r17_present, 1));

  if (sdt_rsrp_thres_r17_present) {
    HANDLE_CODE(unpack_integer(sdt_rsrp_thres_r17, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (sdt_lc_ch_sr_delay_timer_r17_present) {
    HANDLE_CODE(sdt_lc_ch_sr_delay_timer_r17.unpack(bref));
  }
  HANDLE_CODE(sdt_data_volume_thres_r17.unpack(bref));
  HANDLE_CODE(t319a_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sdt_cfg_common_sib_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sdt_rsrp_thres_r17_present) {
    j.write_int("sdt-RSRP-Threshold-r17", sdt_rsrp_thres_r17);
  }
  if (sdt_lc_ch_sr_delay_timer_r17_present) {
    j.write_str("sdt-LogicalChannelSR-DelayTimer-r17", sdt_lc_ch_sr_delay_timer_r17.to_string());
  }
  j.write_str("sdt-DataVolumeThreshold-r17", sdt_data_volume_thres_r17.to_string());
  j.write_str("t319a-r17", t319a_r17.to_string());
  j.end_obj();
}

const char* sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_opts::to_string() const
{
  static const char* names[] = {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
  return convert_enum_idx(names, 8, value, "sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_e_");
}
uint16_t sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(numbers, 7, value, "sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_e_");
}

const char* sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_opts::to_string() const
{
  static const char* names[] = {"byte32",
                                "byte100",
                                "byte200",
                                "byte400",
                                "byte600",
                                "byte800",
                                "byte1000",
                                "byte2000",
                                "byte4000",
                                "byte8000",
                                "byte9000",
                                "byte10000",
                                "byte12000",
                                "byte24000",
                                "byte48000",
                                "byte96000"};
  return convert_enum_idx(names, 16, value, "sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_e_");
}
uint32_t sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_opts::to_number() const
{
  static const uint32_t numbers[] = {
      32, 100, 200, 400, 600, 800, 1000, 2000, 4000, 8000, 9000, 10000, 12000, 24000, 48000, 96000};
  return map_enum_number(numbers, 16, value, "sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_e_");
}

const char* sdt_cfg_common_sib_r17_s::t319a_r17_opts::to_string() const
{
  static const char* names[] = {"ms100",
                                "ms200",
                                "ms300",
                                "ms400",
                                "ms600",
                                "ms1000",
                                "ms2000",
                                "ms3000",
                                "ms4000",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sdt_cfg_common_sib_r17_s::t319a_r17_e_");
}
uint16_t sdt_cfg_common_sib_r17_s::t319a_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 2000, 3000, 4000};
  return map_enum_number(numbers, 9, value, "sdt_cfg_common_sib_r17_s::t319a_r17_e_");
}

// SI-SchedulingInfo-v1700 ::= SEQUENCE
SRSASN_CODE si_sched_info_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dummy_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list2_r17, 1, 32));
  if (dummy_present) {
    HANDLE_CODE(dummy.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_sched_info_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sched_info_list2_r17, bref, 1, 32));
  if (dummy_present) {
    HANDLE_CODE(dummy.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void si_sched_info_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("schedulingInfoList2-r17");
  for (const auto& e1 : sched_info_list2_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (dummy_present) {
    j.write_fieldname("dummy");
    dummy.to_json(j);
  }
  j.end_obj();
}

// SIB-TypeInfo ::= SEQUENCE
SRSASN_CODE sib_type_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(value_tag_present, 1));
  HANDLE_CODE(bref.pack(area_scope_present, 1));

  HANDLE_CODE(type.pack(bref));
  if (value_tag_present) {
    HANDLE_CODE(pack_integer(bref, value_tag, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_type_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(value_tag_present, 1));
  HANDLE_CODE(bref.unpack(area_scope_present, 1));

  HANDLE_CODE(type.unpack(bref));
  if (value_tag_present) {
    HANDLE_CODE(unpack_integer(value_tag, bref, (uint8_t)0u, (uint8_t)31u));
  }

  return SRSASN_SUCCESS;
}
void sib_type_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("type", type.to_string());
  if (value_tag_present) {
    j.write_int("valueTag", value_tag);
  }
  if (area_scope_present) {
    j.write_str("areaScope", "true");
  }
  j.end_obj();
}

const char* sib_type_info_s::type_opts::to_string() const
{
  static const char* names[] = {"sibType2",
                                "sibType3",
                                "sibType4",
                                "sibType5",
                                "sibType6",
                                "sibType7",
                                "sibType8",
                                "sibType9",
                                "sibType10-v1610",
                                "sibType11-v1610",
                                "sibType12-v1610",
                                "sibType13-v1610",
                                "sibType14-v1610",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sib_type_info_s::type_e_");
}
uint8_t sib_type_info_s::type_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  return map_enum_number(numbers, 13, value, "sib_type_info_s::type_e_");
}

// SIB1-v1740-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1740_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(si_sched_info_v1740_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (si_sched_info_v1740_present) {
    HANDLE_CODE(si_sched_info_v1740.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1740_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(si_sched_info_v1740_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (si_sched_info_v1740_present) {
    HANDLE_CODE(si_sched_info_v1740.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1740_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (si_sched_info_v1740_present) {
    j.write_fieldname("si-SchedulingInfo-v1740");
    si_sched_info_v1740.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UAC-BarringPerCat ::= SEQUENCE
SRSASN_CODE uac_barr_per_cat_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, access_category, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, uac_barr_info_set_idx, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_cat_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(access_category, bref, (uint8_t)1u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(uac_barr_info_set_idx, bref, (uint8_t)1u, (uint8_t)8u));

  return SRSASN_SUCCESS;
}
void uac_barr_per_cat_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("accessCategory", access_category);
  j.write_int("uac-barringInfoSetIndex", uac_barr_info_set_idx);
  j.end_obj();
}

// ApplicableDisasterInfo-r17 ::= CHOICE
void applicable_disaster_info_r17_c::set(types::options e)
{
  type_ = e;
}
void applicable_disaster_info_r17_c::set_no_disaster_roaming_r17()
{
  set(types::no_disaster_roaming_r17);
}
void applicable_disaster_info_r17_c::set_disaster_related_ind_r17()
{
  set(types::disaster_related_ind_r17);
}
void applicable_disaster_info_r17_c::set_common_plmns_r17()
{
  set(types::common_plmns_r17);
}
applicable_disaster_info_r17_c::ded_plmns_r17_l_& applicable_disaster_info_r17_c::set_ded_plmns_r17()
{
  set(types::ded_plmns_r17);
  return c;
}
void applicable_disaster_info_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      j.start_array("dedicatedPLMNs-r17");
      for (const auto& e1 : c) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE applicable_disaster_info_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 12));
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE applicable_disaster_info_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::no_disaster_roaming_r17:
      break;
    case types::disaster_related_ind_r17:
      break;
    case types::common_plmns_r17:
      break;
    case types::ded_plmns_r17:
      HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 12));
      break;
    default:
      log_invalid_choice_id(type_, "applicable_disaster_info_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* applicable_disaster_info_r17_c::types_opts::to_string() const
{
  static const char* names[] = {
      "noDisasterRoaming-r17", "disasterRelatedIndication-r17", "commonPLMNs-r17", "dedicatedPLMNs-r17"};
  return convert_enum_idx(names, 4, value, "applicable_disaster_info_r17_c::types");
}

// CFR-ConfigMCCH-MTCH-r17 ::= SEQUENCE
SRSASN_CODE cfr_cfg_mcch_mtch_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(location_and_bw_broadcast_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_mcch_r17_present, 1));
  HANDLE_CODE(bref.pack(common_coreset_ext_r17_present, 1));

  if (location_and_bw_broadcast_r17_present) {
    HANDLE_CODE(location_and_bw_broadcast_r17.pack(bref));
  }
  if (pdsch_cfg_mcch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mcch_r17.pack(bref));
  }
  if (common_coreset_ext_r17_present) {
    HANDLE_CODE(common_coreset_ext_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfr_cfg_mcch_mtch_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(location_and_bw_broadcast_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_mcch_r17_present, 1));
  HANDLE_CODE(bref.unpack(common_coreset_ext_r17_present, 1));

  if (location_and_bw_broadcast_r17_present) {
    HANDLE_CODE(location_and_bw_broadcast_r17.unpack(bref));
  }
  if (pdsch_cfg_mcch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mcch_r17.unpack(bref));
  }
  if (common_coreset_ext_r17_present) {
    HANDLE_CODE(common_coreset_ext_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cfr_cfg_mcch_mtch_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (location_and_bw_broadcast_r17_present) {
    j.write_fieldname("locationAndBandwidthBroadcast-r17");
    location_and_bw_broadcast_r17.to_json(j);
  }
  if (pdsch_cfg_mcch_r17_present) {
    j.write_fieldname("pdsch-ConfigMCCH-r17");
    pdsch_cfg_mcch_r17.to_json(j);
  }
  if (common_coreset_ext_r17_present) {
    j.write_fieldname("commonControlResourceSetExt-r17");
    common_coreset_ext_r17.to_json(j);
  }
  j.end_obj();
}

// GIN-Element-r17 ::= SEQUENCE
SRSASN_CODE gin_elem_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_r17.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, nid_list_r17, 1, 24));

  return SRSASN_SUCCESS;
}
SRSASN_CODE gin_elem_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_r17.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(nid_list_r17, bref, 1, 24));

  return SRSASN_SUCCESS;
}
void gin_elem_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-r17");
  plmn_id_r17.to_json(j);
  j.start_array("nid-List-r17");
  for (const auto& e1 : nid_list_r17) {
    j.write_str(e1.to_string());
  }
  j.end_array();
  j.end_obj();
}

// GINs-PerSNPN-r17 ::= SEQUENCE
SRSASN_CODE gi_ns_per_sn_pn_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_gi_ns_r17_present, 1));

  if (supported_gi_ns_r17_present) {
    HANDLE_CODE(supported_gi_ns_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gi_ns_per_sn_pn_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(supported_gi_ns_r17_present, 1));

  if (supported_gi_ns_r17_present) {
    HANDLE_CODE(supported_gi_ns_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void gi_ns_per_sn_pn_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_gi_ns_r17_present) {
    j.write_str("supportedGINs-r17", supported_gi_ns_r17.to_string());
  }
  j.end_obj();
}

// IntraFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
SRSASN_CODE intra_freq_cag_cell_list_per_plmn_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, plmn_id_idx_r16, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(pack_dyn_seq_of(bref, cag_cell_list_r16, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE intra_freq_cag_cell_list_per_plmn_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(plmn_id_idx_r16, bref, (uint8_t)1u, (uint8_t)12u));
  HANDLE_CODE(unpack_dyn_seq_of(cag_cell_list_r16, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void intra_freq_cag_cell_list_per_plmn_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex-r16", plmn_id_idx_r16);
  j.start_array("cag-CellList-r16");
  for (const auto& e1 : cag_cell_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MCCH-Config-r17 ::= SEQUENCE
SRSASN_CODE mcch_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcch_win_dur_r17_present, 1));

  HANDLE_CODE(mcch_repeat_period_and_offset_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, mcch_win_start_slot_r17, (uint8_t)0u, (uint8_t)79u));
  if (mcch_win_dur_r17_present) {
    HANDLE_CODE(mcch_win_dur_r17.pack(bref));
  }
  HANDLE_CODE(mcch_mod_period_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcch_win_dur_r17_present, 1));

  HANDLE_CODE(mcch_repeat_period_and_offset_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(mcch_win_start_slot_r17, bref, (uint8_t)0u, (uint8_t)79u));
  if (mcch_win_dur_r17_present) {
    HANDLE_CODE(mcch_win_dur_r17.unpack(bref));
  }
  HANDLE_CODE(mcch_mod_period_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mcch_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mcch-RepetitionPeriodAndOffset-r17");
  mcch_repeat_period_and_offset_r17.to_json(j);
  j.write_int("mcch-WindowStartSlot-r17", mcch_win_start_slot_r17);
  if (mcch_win_dur_r17_present) {
    j.write_str("mcch-WindowDuration-r17", mcch_win_dur_r17.to_string());
  }
  j.write_str("mcch-ModificationPeriod-r17", mcch_mod_period_r17.to_string());
  j.end_obj();
}

const char* mcch_cfg_r17_s::mcch_win_dur_r17_opts::to_string() const
{
  static const char* names[] = {"sl2", "sl4", "sl8", "sl10", "sl20", "sl40", "sl80", "sl160"};
  return convert_enum_idx(names, 8, value, "mcch_cfg_r17_s::mcch_win_dur_r17_e_");
}
uint8_t mcch_cfg_r17_s::mcch_win_dur_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 8, value, "mcch_cfg_r17_s::mcch_win_dur_r17_e_");
}

const char* mcch_cfg_r17_s::mcch_mod_period_r17_opts::to_string() const
{
  static const char* names[] = {"rf2",
                                "rf4",
                                "rf8",
                                "rf16",
                                "rf32",
                                "rf64",
                                "rf128",
                                "rf256",
                                "rf512",
                                "rf1024",
                                "r2048",
                                "rf4096",
                                "rf8192",
                                "rf16384",
                                "rf32768",
                                "rf65536"};
  return convert_enum_idx(names, 16, value, "mcch_cfg_r17_s::mcch_mod_period_r17_e_");
}
uint32_t mcch_cfg_r17_s::mcch_mod_period_r17_opts::to_number() const
{
  static const uint32_t numbers[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
  return map_enum_number(numbers, 16, value, "mcch_cfg_r17_s::mcch_mod_period_r17_e_");
}

// MeasIdleConfigSIB-r16 ::= SEQUENCE
SRSASN_CODE meas_idle_cfg_sib_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_eutra_r16.size() > 0, 1));

  if (meas_idle_carrier_list_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_nr_r16, 1, 8));
  }
  if (meas_idle_carrier_list_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_eutra_r16, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_idle_cfg_sib_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_idle_carrier_list_nr_r16_present;
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_nr_r16_present, 1));
  bool meas_idle_carrier_list_eutra_r16_present;
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_eutra_r16_present, 1));

  if (meas_idle_carrier_list_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_nr_r16, bref, 1, 8));
  }
  if (meas_idle_carrier_list_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_eutra_r16, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_idle_cfg_sib_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_idle_carrier_list_nr_r16.size() > 0) {
    j.start_array("measIdleCarrierListNR-r16");
    for (const auto& e1 : meas_idle_carrier_list_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_idle_carrier_list_eutra_r16.size() > 0) {
    j.start_array("measIdleCarrierListEUTRA-r16");
    for (const auto& e1 : meas_idle_carrier_list_eutra_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MobilityStateParameters ::= SEQUENCE
SRSASN_CODE mob_state_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(t_eval.pack(bref));
  HANDLE_CODE(t_hyst_normal.pack(bref));
  HANDLE_CODE(pack_integer(bref, n_cell_change_medium, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(pack_integer(bref, n_cell_change_high, (uint8_t)1u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mob_state_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(t_eval.unpack(bref));
  HANDLE_CODE(t_hyst_normal.unpack(bref));
  HANDLE_CODE(unpack_integer(n_cell_change_medium, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(unpack_integer(n_cell_change_high, bref, (uint8_t)1u, (uint8_t)16u));

  return SRSASN_SUCCESS;
}
void mob_state_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t-Evaluation", t_eval.to_string());
  j.write_str("t-HystNormal", t_hyst_normal.to_string());
  j.write_int("n-CellChangeMedium", n_cell_change_medium);
  j.write_int("n-CellChangeHigh", n_cell_change_high);
  j.end_obj();
}

const char* mob_state_params_s::t_eval_opts::to_string() const
{
  static const char* names[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "mob_state_params_s::t_eval_e_");
}
uint8_t mob_state_params_s::t_eval_opts::to_number() const
{
  static const uint8_t numbers[] = {30, 60, 120, 180, 240};
  return map_enum_number(numbers, 5, value, "mob_state_params_s::t_eval_e_");
}

const char* mob_state_params_s::t_hyst_normal_opts::to_string() const
{
  static const char* names[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "mob_state_params_s::t_hyst_normal_e_");
}
uint8_t mob_state_params_s::t_hyst_normal_opts::to_number() const
{
  static const uint8_t numbers[] = {30, 60, 120, 180, 240};
  return map_enum_number(numbers, 5, value, "mob_state_params_s::t_hyst_normal_e_");
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

const char* pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types_opts::to_string() const
{
  static const char* names[] = {"sCS480KHZoneEighthT", "sCS480KHZoneSixteenthT"};
  return convert_enum_idx(names, 2, value, "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types");
}

// PosSchedulingInfo-r16 ::= SEQUENCE
SRSASN_CODE pos_sched_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(offset_to_si_used_r16_present, 1));

  HANDLE_CODE(pos_si_periodicity_r16.pack(bref));
  HANDLE_CODE(pos_si_broadcast_status_r16.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sib_map_info_r16, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sched_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(offset_to_si_used_r16_present, 1));

  HANDLE_CODE(pos_si_periodicity_r16.unpack(bref));
  HANDLE_CODE(pos_si_broadcast_status_r16.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(pos_sib_map_info_r16, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void pos_sched_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (offset_to_si_used_r16_present) {
    j.write_str("offsetToSI-Used-r16", "true");
  }
  j.write_str("posSI-Periodicity-r16", pos_si_periodicity_r16.to_string());
  j.write_str("posSI-BroadcastStatus-r16", pos_si_broadcast_status_r16.to_string());
  j.start_array("posSIB-MappingInfo-r16");
  for (const auto& e1 : pos_sib_map_info_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* pos_sched_info_r16_s::pos_si_periodicity_r16_opts::to_string() const
{
  static const char* names[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(names, 7, value, "pos_sched_info_r16_s::pos_si_periodicity_r16_e_");
}
uint16_t pos_sched_info_r16_s::pos_si_periodicity_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "pos_sched_info_r16_s::pos_si_periodicity_r16_e_");
}

const char* pos_sched_info_r16_s::pos_si_broadcast_status_r16_opts::to_string() const
{
  static const char* names[] = {"broadcasting", "notBroadcasting"};
  return convert_enum_idx(names, 2, value, "pos_sched_info_r16_s::pos_si_broadcast_status_r16_e_");
}

// SIB1-v1700-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(hsdn_cell_r17_present, 1));
  HANDLE_CODE(bref.pack(uac_barr_info_v1700_present, 1));
  HANDLE_CODE(bref.pack(sdt_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.pack(red_cap_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.pack(feature_priorities_r17_present, 1));
  HANDLE_CODE(bref.pack(si_sched_info_v1700_present, 1));
  HANDLE_CODE(bref.pack(hyper_sfn_r17_present, 1));
  HANDLE_CODE(bref.pack(edrx_allowed_idle_r17_present, 1));
  HANDLE_CODE(bref.pack(edrx_allowed_inactive_r17_present, 1));
  HANDLE_CODE(bref.pack(intra_freq_resel_red_cap_r17_present, 1));
  HANDLE_CODE(bref.pack(cell_barred_ntn_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (uac_barr_info_v1700_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info_v1700.uac_barr_info_set_list_v1700, 1, 8));
  }
  if (sdt_cfg_common_r17_present) {
    HANDLE_CODE(sdt_cfg_common_r17.pack(bref));
  }
  if (red_cap_cfg_common_r17_present) {
    HANDLE_CODE(red_cap_cfg_common_r17.pack(bref));
  }
  if (feature_priorities_r17_present) {
    HANDLE_CODE(bref.pack(feature_priorities_r17.red_cap_prio_r17_present, 1));
    HANDLE_CODE(bref.pack(feature_priorities_r17.slicing_prio_r17_present, 1));
    HANDLE_CODE(bref.pack(feature_priorities_r17.msg3_repeats_prio_r17_present, 1));
    HANDLE_CODE(bref.pack(feature_priorities_r17.sdt_prio_r17_present, 1));
    if (feature_priorities_r17.red_cap_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.red_cap_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.slicing_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.slicing_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.msg3_repeats_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.msg3_repeats_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.sdt_prio_r17_present) {
      HANDLE_CODE(pack_integer(bref, feature_priorities_r17.sdt_prio_r17, (uint8_t)0u, (uint8_t)7u));
    }
  }
  if (si_sched_info_v1700_present) {
    HANDLE_CODE(si_sched_info_v1700.pack(bref));
  }
  if (hyper_sfn_r17_present) {
    HANDLE_CODE(hyper_sfn_r17.pack(bref));
  }
  if (intra_freq_resel_red_cap_r17_present) {
    HANDLE_CODE(intra_freq_resel_red_cap_r17.pack(bref));
  }
  if (cell_barred_ntn_r17_present) {
    HANDLE_CODE(cell_barred_ntn_r17.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1700_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(hsdn_cell_r17_present, 1));
  HANDLE_CODE(bref.unpack(uac_barr_info_v1700_present, 1));
  HANDLE_CODE(bref.unpack(sdt_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.unpack(red_cap_cfg_common_r17_present, 1));
  HANDLE_CODE(bref.unpack(feature_priorities_r17_present, 1));
  HANDLE_CODE(bref.unpack(si_sched_info_v1700_present, 1));
  HANDLE_CODE(bref.unpack(hyper_sfn_r17_present, 1));
  HANDLE_CODE(bref.unpack(edrx_allowed_idle_r17_present, 1));
  HANDLE_CODE(bref.unpack(edrx_allowed_inactive_r17_present, 1));
  HANDLE_CODE(bref.unpack(intra_freq_resel_red_cap_r17_present, 1));
  HANDLE_CODE(bref.unpack(cell_barred_ntn_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (uac_barr_info_v1700_present) {
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info_v1700.uac_barr_info_set_list_v1700, bref, 1, 8));
  }
  if (sdt_cfg_common_r17_present) {
    HANDLE_CODE(sdt_cfg_common_r17.unpack(bref));
  }
  if (red_cap_cfg_common_r17_present) {
    HANDLE_CODE(red_cap_cfg_common_r17.unpack(bref));
  }
  if (feature_priorities_r17_present) {
    HANDLE_CODE(bref.unpack(feature_priorities_r17.red_cap_prio_r17_present, 1));
    HANDLE_CODE(bref.unpack(feature_priorities_r17.slicing_prio_r17_present, 1));
    HANDLE_CODE(bref.unpack(feature_priorities_r17.msg3_repeats_prio_r17_present, 1));
    HANDLE_CODE(bref.unpack(feature_priorities_r17.sdt_prio_r17_present, 1));
    if (feature_priorities_r17.red_cap_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.red_cap_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.slicing_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.slicing_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.msg3_repeats_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.msg3_repeats_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
    if (feature_priorities_r17.sdt_prio_r17_present) {
      HANDLE_CODE(unpack_integer(feature_priorities_r17.sdt_prio_r17, bref, (uint8_t)0u, (uint8_t)7u));
    }
  }
  if (si_sched_info_v1700_present) {
    HANDLE_CODE(si_sched_info_v1700.unpack(bref));
  }
  if (hyper_sfn_r17_present) {
    HANDLE_CODE(hyper_sfn_r17.unpack(bref));
  }
  if (intra_freq_resel_red_cap_r17_present) {
    HANDLE_CODE(intra_freq_resel_red_cap_r17.unpack(bref));
  }
  if (cell_barred_ntn_r17_present) {
    HANDLE_CODE(cell_barred_ntn_r17.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hsdn_cell_r17_present) {
    j.write_str("hsdn-Cell-r17", "true");
  }
  if (uac_barr_info_v1700_present) {
    j.write_fieldname("uac-BarringInfo-v1700");
    j.start_obj();
    j.start_array("uac-BarringInfoSetList-v1700");
    for (const auto& e1 : uac_barr_info_v1700.uac_barr_info_set_list_v1700) {
      e1.to_json(j);
    }
    j.end_array();
    j.end_obj();
  }
  if (sdt_cfg_common_r17_present) {
    j.write_fieldname("sdt-ConfigCommon-r17");
    sdt_cfg_common_r17.to_json(j);
  }
  if (red_cap_cfg_common_r17_present) {
    j.write_fieldname("redCap-ConfigCommon-r17");
    red_cap_cfg_common_r17.to_json(j);
  }
  if (feature_priorities_r17_present) {
    j.write_fieldname("featurePriorities-r17");
    j.start_obj();
    if (feature_priorities_r17.red_cap_prio_r17_present) {
      j.write_int("redCapPriority-r17", feature_priorities_r17.red_cap_prio_r17);
    }
    if (feature_priorities_r17.slicing_prio_r17_present) {
      j.write_int("slicingPriority-r17", feature_priorities_r17.slicing_prio_r17);
    }
    if (feature_priorities_r17.msg3_repeats_prio_r17_present) {
      j.write_int("msg3-Repetitions-Priority-r17", feature_priorities_r17.msg3_repeats_prio_r17);
    }
    if (feature_priorities_r17.sdt_prio_r17_present) {
      j.write_int("sdt-Priority-r17", feature_priorities_r17.sdt_prio_r17);
    }
    j.end_obj();
  }
  if (si_sched_info_v1700_present) {
    j.write_fieldname("si-SchedulingInfo-v1700");
    si_sched_info_v1700.to_json(j);
  }
  if (hyper_sfn_r17_present) {
    j.write_str("hyperSFN-r17", hyper_sfn_r17.to_string());
  }
  if (edrx_allowed_idle_r17_present) {
    j.write_str("eDRX-AllowedIdle-r17", "true");
  }
  if (edrx_allowed_inactive_r17_present) {
    j.write_str("eDRX-AllowedInactive-r17", "true");
  }
  if (intra_freq_resel_red_cap_r17_present) {
    j.write_str("intraFreqReselectionRedCap-r17", intra_freq_resel_red_cap_r17.to_string());
  }
  if (cell_barred_ntn_r17_present) {
    j.write_str("cellBarredNTN-r17", cell_barred_ntn_r17.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* sib1_v1700_ies_s::intra_freq_resel_red_cap_r17_opts::to_string() const
{
  static const char* names[] = {"allowed", "notAllowed"};
  return convert_enum_idx(names, 2, value, "sib1_v1700_ies_s::intra_freq_resel_red_cap_r17_e_");
}

const char* sib1_v1700_ies_s::cell_barred_ntn_r17_opts::to_string() const
{
  static const char* names[] = {"barred", "notBarred"};
  return convert_enum_idx(names, 2, value, "sib1_v1700_ies_s::cell_barred_ntn_r17_e_");
}

// UAC-AC1-SelectAssistInfo-r16 ::= ENUMERATED
const char* uac_ac1_select_assist_info_r16_opts::to_string() const
{
  static const char* names[] = {"a", "b", "c", "notConfigured"};
  return convert_enum_idx(names, 4, value, "uac_ac1_select_assist_info_r16_e");
}

// PosSI-SchedulingInfo-r16 ::= SEQUENCE
SRSASN_CODE pos_si_sched_info_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pos_si_request_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(pos_si_request_cfg_sul_r16_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sched_info_list_r16, 1, 32));
  if (pos_si_request_cfg_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_r16.pack(bref));
  }
  if (pos_si_request_cfg_sul_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_sul_r16.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= pos_si_request_cfg_red_cap_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pos_si_request_cfg_red_cap_r17.is_present(), 1));
      if (pos_si_request_cfg_red_cap_r17.is_present()) {
        HANDLE_CODE(pos_si_request_cfg_red_cap_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_si_sched_info_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pos_si_request_cfg_r16_present, 1));
  HANDLE_CODE(bref.unpack(pos_si_request_cfg_sul_r16_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pos_sched_info_list_r16, bref, 1, 32));
  if (pos_si_request_cfg_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_r16.unpack(bref));
  }
  if (pos_si_request_cfg_sul_r16_present) {
    HANDLE_CODE(pos_si_request_cfg_sul_r16.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(pos_si_request_cfg_red_cap_r17, bref);
      if (pos_si_request_cfg_red_cap_r17.is_present()) {
        HANDLE_CODE(pos_si_request_cfg_red_cap_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pos_si_sched_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("posSchedulingInfoList-r16");
  for (const auto& e1 : pos_sched_info_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (pos_si_request_cfg_r16_present) {
    j.write_fieldname("posSI-RequestConfig-r16");
    pos_si_request_cfg_r16.to_json(j);
  }
  if (pos_si_request_cfg_sul_r16_present) {
    j.write_fieldname("posSI-RequestConfigSUL-r16");
    pos_si_request_cfg_sul_r16.to_json(j);
  }
  if (ext) {
    if (pos_si_request_cfg_red_cap_r17.is_present()) {
      j.write_fieldname("posSI-RequestConfigRedCap-r17");
      pos_si_request_cfg_red_cap_r17->to_json(j);
    }
  }
  j.end_obj();
}

// SIB1-v1630-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1630_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_barr_info_v1630_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (uac_barr_info_v1630_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info_v1630.uac_ac1_select_assist_info_r16, 2, 12));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1630_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_barr_info_v1630_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (uac_barr_info_v1630_present) {
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info_v1630.uac_ac1_select_assist_info_r16, bref, 2, 12));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1630_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (uac_barr_info_v1630_present) {
    j.write_fieldname("uac-BarringInfo-v1630");
    j.start_obj();
    j.start_array("uac-AC1-SelectAssistInfo-r16");
    for (const auto& e1 : uac_barr_info_v1630.uac_ac1_select_assist_info_r16) {
      j.write_str(e1.to_string());
    }
    j.end_array();
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SIB10-r16 ::= SEQUENCE
SRSASN_CODE sib10_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(hrnn_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (hrnn_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, hrnn_list_r16, 1, 12));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib10_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool hrnn_list_r16_present;
  HANDLE_CODE(bref.unpack(hrnn_list_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (hrnn_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(hrnn_list_r16, bref, 1, 12));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib10_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (hrnn_list_r16.size() > 0) {
    j.start_array("hrnn-List-r16");
    for (const auto& e1 : hrnn_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB11-r16 ::= SEQUENCE
SRSASN_CODE sib11_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_idle_cfg_sib_r16_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (meas_idle_cfg_sib_r16_present) {
    HANDLE_CODE(meas_idle_cfg_sib_r16.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib11_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_idle_cfg_sib_r16_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (meas_idle_cfg_sib_r16_present) {
    HANDLE_CODE(meas_idle_cfg_sib_r16.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib11_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_idle_cfg_sib_r16_present) {
    j.write_fieldname("measIdleConfigSIB-r16");
    meas_idle_cfg_sib_r16.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB12-r16 ::= SEQUENCE
SRSASN_CODE sib12_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, segment_num_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r16.pack(bref));
  HANDLE_CODE(segment_container_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib12_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(segment_num_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r16.unpack(bref));
  HANDLE_CODE(segment_container_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib12_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("segmentNumber-r16", segment_num_r16);
  j.write_str("segmentType-r16", segment_type_r16.to_string());
  j.write_str("segmentContainer-r16", segment_container_r16.to_string());
  j.end_obj();
}

const char* sib12_r16_s::segment_type_r16_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib12_r16_s::segment_type_r16_e_");
}

// SIB13-r16 ::= SEQUENCE
SRSASN_CODE sib13_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(sl_v2x_cfg_common_r16.pack(bref));
  HANDLE_CODE(dummy.pack(bref));
  HANDLE_CODE(tdd_cfg_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib13_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sl_v2x_cfg_common_r16.unpack(bref));
  HANDLE_CODE(dummy.unpack(bref));
  HANDLE_CODE(tdd_cfg_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib13_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-V2X-ConfigCommon-r16", sl_v2x_cfg_common_r16.to_string());
  j.write_str("dummy", dummy.to_string());
  j.write_str("tdd-Config-r16", tdd_cfg_r16.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB14-r16 ::= SEQUENCE
SRSASN_CODE sib14_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(sl_v2x_cfg_common_ext_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib14_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sl_v2x_cfg_common_ext_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib14_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-V2X-ConfigCommonExt-r16", sl_v2x_cfg_common_ext_r16.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB15-r17 ::= SEQUENCE
SRSASN_CODE sib15_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(common_plmns_with_disaster_condition_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(applicable_disaster_info_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (common_plmns_with_disaster_condition_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, common_plmns_with_disaster_condition_r17, 1, 12));
  }
  if (applicable_disaster_info_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, applicable_disaster_info_list_r17, 1, 12));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib15_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool common_plmns_with_disaster_condition_r17_present;
  HANDLE_CODE(bref.unpack(common_plmns_with_disaster_condition_r17_present, 1));
  bool applicable_disaster_info_list_r17_present;
  HANDLE_CODE(bref.unpack(applicable_disaster_info_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (common_plmns_with_disaster_condition_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(common_plmns_with_disaster_condition_r17, bref, 1, 12));
  }
  if (applicable_disaster_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(applicable_disaster_info_list_r17, bref, 1, 12));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib15_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (common_plmns_with_disaster_condition_r17.size() > 0) {
    j.start_array("commonPLMNsWithDisasterCondition-r17");
    for (const auto& e1 : common_plmns_with_disaster_condition_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (applicable_disaster_info_list_r17.size() > 0) {
    j.start_array("applicableDisasterInfoList-r17");
    for (const auto& e1 : applicable_disaster_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB16-r17 ::= SEQUENCE
SRSASN_CODE sib16_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_prio_list_slicing_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (freq_prio_list_slicing_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_prio_list_slicing_r17, 1, 9));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib16_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool freq_prio_list_slicing_r17_present;
  HANDLE_CODE(bref.unpack(freq_prio_list_slicing_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (freq_prio_list_slicing_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_prio_list_slicing_r17, bref, 1, 9));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib16_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_prio_list_slicing_r17.size() > 0) {
    j.start_array("freqPriorityListSlicing-r17");
    for (const auto& e1 : freq_prio_list_slicing_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB17-r17 ::= SEQUENCE
SRSASN_CODE sib17_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, segment_num_r17, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r17.pack(bref));
  HANDLE_CODE(segment_container_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib17_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(segment_num_r17, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(segment_type_r17.unpack(bref));
  HANDLE_CODE(segment_container_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sib17_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("segmentNumber-r17", segment_num_r17);
  j.write_str("segmentType-r17", segment_type_r17.to_string());
  j.write_str("segmentContainer-r17", segment_container_r17.to_string());
  j.end_obj();
}

const char* sib17_r17_s::segment_type_r17_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib17_r17_s::segment_type_r17_e_");
}

// SIB18-r17 ::= SEQUENCE
SRSASN_CODE sib18_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gin_elem_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(gins_per_sn_pn_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (gin_elem_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gin_elem_list_r17, 1, 24));
  }
  if (gins_per_sn_pn_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gins_per_sn_pn_list_r17, 1, 12));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib18_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool gin_elem_list_r17_present;
  HANDLE_CODE(bref.unpack(gin_elem_list_r17_present, 1));
  bool gins_per_sn_pn_list_r17_present;
  HANDLE_CODE(bref.unpack(gins_per_sn_pn_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (gin_elem_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gin_elem_list_r17, bref, 1, 24));
  }
  if (gins_per_sn_pn_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gins_per_sn_pn_list_r17, bref, 1, 12));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib18_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gin_elem_list_r17.size() > 0) {
    j.start_array("gin-ElementList-r17");
    for (const auto& e1 : gin_elem_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (gins_per_sn_pn_list_r17.size() > 0) {
    j.start_array("gins-PerSNPN-List-r17");
    for (const auto& e1 : gins_per_sn_pn_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB19-r17 ::= SEQUENCE
SRSASN_CODE sib19_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(t_service_r17_present, 1));
  HANDLE_CODE(bref.pack(ref_location_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(distance_thresh_r17_present, 1));
  HANDLE_CODE(bref.pack(ntn_neigh_cell_cfg_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.pack(bref));
  }
  if (t_service_r17_present) {
    HANDLE_CODE(pack_integer(bref, t_service_r17, (uint64_t)0u, (uint64_t)549755813887u));
  }
  if (ref_location_r17.size() > 0) {
    HANDLE_CODE(ref_location_r17.pack(bref));
  }
  if (distance_thresh_r17_present) {
    HANDLE_CODE(pack_integer(bref, distance_thresh_r17, (uint16_t)0u, (uint16_t)65525u));
  }
  if (ntn_neigh_cell_cfg_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ntn_neigh_cell_cfg_list_r17, 1, 4));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ntn_neigh_cell_cfg_list_ext_v1720.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ntn_neigh_cell_cfg_list_ext_v1720.is_present(), 1));
      if (ntn_neigh_cell_cfg_list_ext_v1720.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *ntn_neigh_cell_cfg_list_ext_v1720, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib19_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ntn_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(t_service_r17_present, 1));
  bool ref_location_r17_present;
  HANDLE_CODE(bref.unpack(ref_location_r17_present, 1));
  HANDLE_CODE(bref.unpack(distance_thresh_r17_present, 1));
  bool ntn_neigh_cell_cfg_list_r17_present;
  HANDLE_CODE(bref.unpack(ntn_neigh_cell_cfg_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (ntn_cfg_r17_present) {
    HANDLE_CODE(ntn_cfg_r17.unpack(bref));
  }
  if (t_service_r17_present) {
    HANDLE_CODE(unpack_integer(t_service_r17, bref, (uint64_t)0u, (uint64_t)549755813887u));
  }
  if (ref_location_r17_present) {
    HANDLE_CODE(ref_location_r17.unpack(bref));
  }
  if (distance_thresh_r17_present) {
    HANDLE_CODE(unpack_integer(distance_thresh_r17, bref, (uint16_t)0u, (uint16_t)65525u));
  }
  if (ntn_neigh_cell_cfg_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ntn_neigh_cell_cfg_list_r17, bref, 1, 4));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ntn_neigh_cell_cfg_list_ext_v1720, bref);
      if (ntn_neigh_cell_cfg_list_ext_v1720.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*ntn_neigh_cell_cfg_list_ext_v1720, bref, 1, 4));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib19_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ntn_cfg_r17_present) {
    j.write_fieldname("ntn-Config-r17");
    ntn_cfg_r17.to_json(j);
  }
  if (t_service_r17_present) {
    j.write_int("t-Service-r17", t_service_r17);
  }
  if (ref_location_r17.size() > 0) {
    j.write_str("referenceLocation-r17", ref_location_r17.to_string());
  }
  if (distance_thresh_r17_present) {
    j.write_int("distanceThresh-r17", distance_thresh_r17);
  }
  if (ntn_neigh_cell_cfg_list_r17.size() > 0) {
    j.start_array("ntn-NeighCellConfigList-r17");
    for (const auto& e1 : ntn_neigh_cell_cfg_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (ntn_neigh_cell_cfg_list_ext_v1720.is_present()) {
      j.start_array("ntn-NeighCellConfigListExt-v1720");
      for (const auto& e1 : *ntn_neigh_cell_cfg_list_ext_v1720) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SIB2 ::= SEQUENCE
SRSASN_CODE sib2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  bref.pack(cell_resel_info_common.ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_info_common.nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_info_common.range_to_best_cell_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_info_common.speed_state_resel_pars_present, 1));
  if (cell_resel_info_common.nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_info_common.nrof_ss_blocks_to_average, (uint8_t)2u, (uint8_t)16u));
  }
  if (cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(cell_resel_info_common.abs_thresh_ss_blocks_consolidation.pack(bref));
  }
  if (cell_resel_info_common.range_to_best_cell_present) {
    HANDLE_CODE(cell_resel_info_common.range_to_best_cell.pack(bref));
  }
  HANDLE_CODE(cell_resel_info_common.q_hyst.pack(bref));
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.mob_state_params.pack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.pack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.pack(bref));
  }
  bref.pack(cell_resel_serving_freq_info.ext, 1);
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.s_non_intra_search_p_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.s_non_intra_search_q_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.thresh_serving_low_q_present, 1));
  HANDLE_CODE(bref.pack(cell_resel_serving_freq_info.cell_resel_sub_prio_present, 1));
  if (cell_resel_serving_freq_info.s_non_intra_search_p_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.s_non_intra_search_p, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_serving_freq_info.s_non_intra_search_q_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.s_non_intra_search_q, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.thresh_serving_low_p, (uint8_t)0u, (uint8_t)31u));
  if (cell_resel_serving_freq_info.thresh_serving_low_q_present) {
    HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.thresh_serving_low_q, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, cell_resel_serving_freq_info.cell_resel_prio, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_serving_freq_info.cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_serving_freq_info.cell_resel_sub_prio.pack(bref));
  }
  HANDLE_CODE(intra_freq_cell_resel_info.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= relaxed_meas_r16.is_present();
    group_flags[1] |= cell_equivalent_size_r17_present;
    group_flags[1] |= relaxed_meas_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(relaxed_meas_r16.is_present(), 1));
      if (relaxed_meas_r16.is_present()) {
        HANDLE_CODE(bref.pack(relaxed_meas_r16->low_mob_eval_r16_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r16->cell_edge_eval_r16_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r16->combine_relaxed_meas_condition_r16_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r16->high_prio_meas_relax_r16_present, 1));
        if (relaxed_meas_r16->low_mob_eval_r16_present) {
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.s_search_delta_p_r16.pack(bref));
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.t_search_delta_p_r16.pack(bref));
        }
        if (relaxed_meas_r16->cell_edge_eval_r16_present) {
          HANDLE_CODE(bref.pack(relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present, 1));
          HANDLE_CODE(
              pack_integer(bref, relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_p_r16, (uint8_t)0u, (uint8_t)31u));
          if (relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present) {
            HANDLE_CODE(pack_integer(
                bref, relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16, (uint8_t)0u, (uint8_t)31u));
          }
        }
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cell_equivalent_size_r17_present, 1));
      HANDLE_CODE(bref.pack(relaxed_meas_r17.is_present(), 1));
      if (cell_equivalent_size_r17_present) {
        HANDLE_CODE(pack_integer(bref, cell_equivalent_size_r17, (uint8_t)2u, (uint8_t)16u));
      }
      if (relaxed_meas_r17.is_present()) {
        HANDLE_CODE(bref.pack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present, 1));
        HANDLE_CODE(bref.pack(relaxed_meas_r17->combine_relaxed_meas_condition2_r17_present, 1));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.s_search_delta_p_stationary_r17.pack(bref));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.t_search_delta_p_stationary_r17.pack(bref));
        if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present) {
          HANDLE_CODE(
              bref.pack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present, 1));
          HANDLE_CODE(pack_integer(bref,
                                   relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_p2_r17,
                                   (uint8_t)0u,
                                   (uint8_t)31u));
          if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present) {
            HANDLE_CODE(pack_integer(bref,
                                     relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17,
                                     (uint8_t)0u,
                                     (uint8_t)31u));
          }
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bref.unpack(cell_resel_info_common.ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_info_common.nrof_ss_blocks_to_average_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_info_common.range_to_best_cell_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_info_common.speed_state_resel_pars_present, 1));
  if (cell_resel_info_common.nrof_ss_blocks_to_average_present) {
    HANDLE_CODE(unpack_integer(cell_resel_info_common.nrof_ss_blocks_to_average, bref, (uint8_t)2u, (uint8_t)16u));
  }
  if (cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present) {
    HANDLE_CODE(cell_resel_info_common.abs_thresh_ss_blocks_consolidation.unpack(bref));
  }
  if (cell_resel_info_common.range_to_best_cell_present) {
    HANDLE_CODE(cell_resel_info_common.range_to_best_cell.unpack(bref));
  }
  HANDLE_CODE(cell_resel_info_common.q_hyst.unpack(bref));
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.mob_state_params.unpack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.unpack(bref));
    HANDLE_CODE(cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.unpack(bref));
  }
  bref.unpack(cell_resel_serving_freq_info.ext, 1);
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.s_non_intra_search_p_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.s_non_intra_search_q_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.thresh_serving_low_q_present, 1));
  HANDLE_CODE(bref.unpack(cell_resel_serving_freq_info.cell_resel_sub_prio_present, 1));
  if (cell_resel_serving_freq_info.s_non_intra_search_p_present) {
    HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.s_non_intra_search_p, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (cell_resel_serving_freq_info.s_non_intra_search_q_present) {
    HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.s_non_intra_search_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.thresh_serving_low_p, bref, (uint8_t)0u, (uint8_t)31u));
  if (cell_resel_serving_freq_info.thresh_serving_low_q_present) {
    HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.thresh_serving_low_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(cell_resel_serving_freq_info.cell_resel_prio, bref, (uint8_t)0u, (uint8_t)7u));
  if (cell_resel_serving_freq_info.cell_resel_sub_prio_present) {
    HANDLE_CODE(cell_resel_serving_freq_info.cell_resel_sub_prio.unpack(bref));
  }
  HANDLE_CODE(intra_freq_cell_resel_info.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(relaxed_meas_r16, bref);
      if (relaxed_meas_r16.is_present()) {
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->low_mob_eval_r16_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->cell_edge_eval_r16_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->combine_relaxed_meas_condition_r16_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r16->high_prio_meas_relax_r16_present, 1));
        if (relaxed_meas_r16->low_mob_eval_r16_present) {
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.s_search_delta_p_r16.unpack(bref));
          HANDLE_CODE(relaxed_meas_r16->low_mob_eval_r16.t_search_delta_p_r16.unpack(bref));
        }
        if (relaxed_meas_r16->cell_edge_eval_r16_present) {
          HANDLE_CODE(bref.unpack(relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present, 1));
          HANDLE_CODE(unpack_integer(
              relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_p_r16, bref, (uint8_t)0u, (uint8_t)31u));
          if (relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present) {
            HANDLE_CODE(unpack_integer(
                relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16, bref, (uint8_t)0u, (uint8_t)31u));
          }
        }
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(cell_equivalent_size_r17_present, 1));
      unpack_presence_flag(relaxed_meas_r17, bref);
      if (cell_equivalent_size_r17_present) {
        HANDLE_CODE(unpack_integer(cell_equivalent_size_r17, bref, (uint8_t)2u, (uint8_t)16u));
      }
      if (relaxed_meas_r17.is_present()) {
        HANDLE_CODE(bref.unpack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present, 1));
        HANDLE_CODE(bref.unpack(relaxed_meas_r17->combine_relaxed_meas_condition2_r17_present, 1));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.s_search_delta_p_stationary_r17.unpack(bref));
        HANDLE_CODE(relaxed_meas_r17->stationary_mob_eval_r17.t_search_delta_p_stationary_r17.unpack(bref));
        if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present) {
          HANDLE_CODE(
              bref.unpack(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present, 1));
          HANDLE_CODE(unpack_integer(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_p2_r17,
                                     bref,
                                     (uint8_t)0u,
                                     (uint8_t)31u));
          if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present) {
            HANDLE_CODE(unpack_integer(relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17,
                                       bref,
                                       (uint8_t)0u,
                                       (uint8_t)31u));
          }
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellReselectionInfoCommon");
  j.start_obj();
  if (cell_resel_info_common.nrof_ss_blocks_to_average_present) {
    j.write_int("nrofSS-BlocksToAverage", cell_resel_info_common.nrof_ss_blocks_to_average);
  }
  if (cell_resel_info_common.abs_thresh_ss_blocks_consolidation_present) {
    j.write_fieldname("absThreshSS-BlocksConsolidation");
    cell_resel_info_common.abs_thresh_ss_blocks_consolidation.to_json(j);
  }
  if (cell_resel_info_common.range_to_best_cell_present) {
    j.write_str("rangeToBestCell", cell_resel_info_common.range_to_best_cell.to_string());
  }
  j.write_str("q-Hyst", cell_resel_info_common.q_hyst.to_string());
  if (cell_resel_info_common.speed_state_resel_pars_present) {
    j.write_fieldname("speedStateReselectionPars");
    j.start_obj();
    j.write_fieldname("mobilityStateParameters");
    cell_resel_info_common.speed_state_resel_pars.mob_state_params.to_json(j);
    j.write_fieldname("q-HystSF");
    j.start_obj();
    j.write_str("sf-Medium", cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_medium.to_string());
    j.write_str("sf-High", cell_resel_info_common.speed_state_resel_pars.q_hyst_sf.sf_high.to_string());
    j.end_obj();
    j.end_obj();
  }
  j.end_obj();
  j.write_fieldname("cellReselectionServingFreqInfo");
  j.start_obj();
  if (cell_resel_serving_freq_info.s_non_intra_search_p_present) {
    j.write_int("s-NonIntraSearchP", cell_resel_serving_freq_info.s_non_intra_search_p);
  }
  if (cell_resel_serving_freq_info.s_non_intra_search_q_present) {
    j.write_int("s-NonIntraSearchQ", cell_resel_serving_freq_info.s_non_intra_search_q);
  }
  j.write_int("threshServingLowP", cell_resel_serving_freq_info.thresh_serving_low_p);
  if (cell_resel_serving_freq_info.thresh_serving_low_q_present) {
    j.write_int("threshServingLowQ", cell_resel_serving_freq_info.thresh_serving_low_q);
  }
  j.write_int("cellReselectionPriority", cell_resel_serving_freq_info.cell_resel_prio);
  if (cell_resel_serving_freq_info.cell_resel_sub_prio_present) {
    j.write_str("cellReselectionSubPriority", cell_resel_serving_freq_info.cell_resel_sub_prio.to_string());
  }
  j.end_obj();
  j.write_fieldname("intraFreqCellReselectionInfo");
  intra_freq_cell_resel_info.to_json(j);
  if (ext) {
    if (relaxed_meas_r16.is_present()) {
      j.write_fieldname("relaxedMeasurement-r16");
      j.start_obj();
      if (relaxed_meas_r16->low_mob_eval_r16_present) {
        j.write_fieldname("lowMobilityEvaluation-r16");
        j.start_obj();
        j.write_str("s-SearchDeltaP-r16", relaxed_meas_r16->low_mob_eval_r16.s_search_delta_p_r16.to_string());
        j.write_str("t-SearchDeltaP-r16", relaxed_meas_r16->low_mob_eval_r16.t_search_delta_p_r16.to_string());
        j.end_obj();
      }
      if (relaxed_meas_r16->cell_edge_eval_r16_present) {
        j.write_fieldname("cellEdgeEvaluation-r16");
        j.start_obj();
        j.write_int("s-SearchThresholdP-r16", relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_p_r16);
        if (relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16_present) {
          j.write_int("s-SearchThresholdQ-r16", relaxed_meas_r16->cell_edge_eval_r16.s_search_thres_q_r16);
        }
        j.end_obj();
      }
      if (relaxed_meas_r16->combine_relaxed_meas_condition_r16_present) {
        j.write_str("combineRelaxedMeasCondition-r16", "true");
      }
      if (relaxed_meas_r16->high_prio_meas_relax_r16_present) {
        j.write_str("highPriorityMeasRelax-r16", "true");
      }
      j.end_obj();
    }
    if (cell_equivalent_size_r17_present) {
      j.write_int("cellEquivalentSize-r17", cell_equivalent_size_r17);
    }
    if (relaxed_meas_r17.is_present()) {
      j.write_fieldname("relaxedMeasurement-r17");
      j.start_obj();
      j.write_fieldname("stationaryMobilityEvaluation-r17");
      j.start_obj();
      j.write_str("s-SearchDeltaP-Stationary-r17",
                  relaxed_meas_r17->stationary_mob_eval_r17.s_search_delta_p_stationary_r17.to_string());
      j.write_str("t-SearchDeltaP-Stationary-r17",
                  relaxed_meas_r17->stationary_mob_eval_r17.t_search_delta_p_stationary_r17.to_string());
      j.end_obj();
      if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17_present) {
        j.write_fieldname("cellEdgeEvaluationWhileStationary-r17");
        j.start_obj();
        j.write_int("s-SearchThresholdP2-r17",
                    relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_p2_r17);
        if (relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17_present) {
          j.write_int("s-SearchThresholdQ2-r17",
                      relaxed_meas_r17->cell_edge_eval_while_stationary_r17.s_search_thres_q2_r17);
        }
        j.end_obj();
      }
      if (relaxed_meas_r17->combine_relaxed_meas_condition2_r17_present) {
        j.write_str("combineRelaxedMeasCondition2-r17", "true");
      }
      j.end_obj();
    }
  }
  j.end_obj();
}

const char* sib2_s::cell_resel_info_common_s_::q_hyst_opts::to_string() const
{
  static const char* names[] = {"dB0",
                                "dB1",
                                "dB2",
                                "dB3",
                                "dB4",
                                "dB5",
                                "dB6",
                                "dB8",
                                "dB10",
                                "dB12",
                                "dB14",
                                "dB16",
                                "dB18",
                                "dB20",
                                "dB22",
                                "dB24"};
  return convert_enum_idx(names, 16, value, "sib2_s::cell_resel_info_common_s_::q_hyst_e_");
}
uint8_t sib2_s::cell_resel_info_common_s_::q_hyst_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 16, value, "sib2_s::cell_resel_info_common_s_::q_hyst_e_");
}

const char*
sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_string() const
{
  static const char* names[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      names, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_number() const
{
  static const int8_t numbers[] = {-6, -4, -2, 0};
  return map_enum_number(
      numbers, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}

const char* sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_string() const
{
  static const char* names[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      names, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_number() const
{
  static const int8_t numbers[] = {-6, -4, -2, 0};
  return map_enum_number(
      numbers, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}

SRSASN_CODE sib2_s::intra_freq_cell_resel_info_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.pack(q_qual_min_present, 1));
  HANDLE_CODE(bref.pack(s_intra_search_q_present, 1));
  HANDLE_CODE(bref.pack(freq_band_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(freq_band_list_sul.size() > 0, 1));
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(smtc_present, 1));
  HANDLE_CODE(bref.pack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.pack(ssb_to_measure_present, 1));

  HANDLE_CODE(pack_integer(bref, q_rx_lev_min, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(pack_integer(bref, q_rx_lev_min_sul, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(pack_integer(bref, q_qual_min, (int8_t)-43, (int8_t)-12));
  }
  HANDLE_CODE(pack_integer(bref, s_intra_search_p, (uint8_t)0u, (uint8_t)31u));
  if (s_intra_search_q_present) {
    HANDLE_CODE(pack_integer(bref, s_intra_search_q, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_nr, (uint8_t)0u, (uint8_t)7u));
  if (freq_band_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list, 1, 8));
  }
  if (freq_band_list_sul.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_sul, 1, 8));
  }
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.pack(bref));
  }
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.pack(bref));
  }
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.pack(bref));
  }
  HANDLE_CODE(bref.pack(derive_ssb_idx_from_cell, 1));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= t_resel_nr_sf.is_present();
    group_flags[1] |= smtc2_lp_r16.is_present();
    group_flags[1] |= ssb_position_qcl_common_r16_present;
    group_flags[2] |= ssb_position_qcl_common_r17_present;
    group_flags[3] |= smtc4list_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(t_resel_nr_sf.is_present(), 1));
      if (t_resel_nr_sf.is_present()) {
        HANDLE_CODE(t_resel_nr_sf->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc2_lp_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ssb_position_qcl_common_r16_present, 1));
      if (smtc2_lp_r16.is_present()) {
        HANDLE_CODE(smtc2_lp_r16->pack(bref));
      }
      if (ssb_position_qcl_common_r16_present) {
        HANDLE_CODE(ssb_position_qcl_common_r16.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ssb_position_qcl_common_r17_present, 1));
      if (ssb_position_qcl_common_r17_present) {
        HANDLE_CODE(ssb_position_qcl_common_r17.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(smtc4list_r17.is_present(), 1));
      if (smtc4list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *smtc4list_r17, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib2_s::intra_freq_cell_resel_info_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(q_rx_lev_min_sul_present, 1));
  HANDLE_CODE(bref.unpack(q_qual_min_present, 1));
  HANDLE_CODE(bref.unpack(s_intra_search_q_present, 1));
  bool freq_band_list_present;
  HANDLE_CODE(bref.unpack(freq_band_list_present, 1));
  bool freq_band_list_sul_present;
  HANDLE_CODE(bref.unpack(freq_band_list_sul_present, 1));
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(smtc_present, 1));
  HANDLE_CODE(bref.unpack(ss_rssi_meas_present, 1));
  HANDLE_CODE(bref.unpack(ssb_to_measure_present, 1));

  HANDLE_CODE(unpack_integer(q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
  if (q_rx_lev_min_sul_present) {
    HANDLE_CODE(unpack_integer(q_rx_lev_min_sul, bref, (int8_t)-70, (int8_t)-22));
  }
  if (q_qual_min_present) {
    HANDLE_CODE(unpack_integer(q_qual_min, bref, (int8_t)-43, (int8_t)-12));
  }
  HANDLE_CODE(unpack_integer(s_intra_search_p, bref, (uint8_t)0u, (uint8_t)31u));
  if (s_intra_search_q_present) {
    HANDLE_CODE(unpack_integer(s_intra_search_q, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(unpack_integer(t_resel_nr, bref, (uint8_t)0u, (uint8_t)7u));
  if (freq_band_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list, bref, 1, 8));
  }
  if (freq_band_list_sul_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_sul, bref, 1, 8));
  }
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }
  if (smtc_present) {
    HANDLE_CODE(smtc.unpack(bref));
  }
  if (ss_rssi_meas_present) {
    HANDLE_CODE(ss_rssi_meas.unpack(bref));
  }
  if (ssb_to_measure_present) {
    HANDLE_CODE(ssb_to_measure.unpack(bref));
  }
  HANDLE_CODE(bref.unpack(derive_ssb_idx_from_cell, 1));

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(t_resel_nr_sf, bref);
      if (t_resel_nr_sf.is_present()) {
        HANDLE_CODE(t_resel_nr_sf->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(smtc2_lp_r16, bref);
      HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r16_present, 1));
      if (smtc2_lp_r16.is_present()) {
        HANDLE_CODE(smtc2_lp_r16->unpack(bref));
      }
      if (ssb_position_qcl_common_r16_present) {
        HANDLE_CODE(ssb_position_qcl_common_r16.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ssb_position_qcl_common_r17_present, 1));
      if (ssb_position_qcl_common_r17_present) {
        HANDLE_CODE(ssb_position_qcl_common_r17.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(smtc4list_r17, bref);
      if (smtc4list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*smtc4list_r17, bref, 1, 3));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib2_s::intra_freq_cell_resel_info_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("q-RxLevMin", q_rx_lev_min);
  if (q_rx_lev_min_sul_present) {
    j.write_int("q-RxLevMinSUL", q_rx_lev_min_sul);
  }
  if (q_qual_min_present) {
    j.write_int("q-QualMin", q_qual_min);
  }
  j.write_int("s-IntraSearchP", s_intra_search_p);
  if (s_intra_search_q_present) {
    j.write_int("s-IntraSearchQ", s_intra_search_q);
  }
  j.write_int("t-ReselectionNR", t_resel_nr);
  if (freq_band_list.size() > 0) {
    j.start_array("frequencyBandList");
    for (const auto& e1 : freq_band_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (freq_band_list_sul.size() > 0) {
    j.start_array("frequencyBandListSUL");
    for (const auto& e1 : freq_band_list_sul) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  if (smtc_present) {
    j.write_fieldname("smtc");
    smtc.to_json(j);
  }
  if (ss_rssi_meas_present) {
    j.write_fieldname("ss-RSSI-Measurement");
    ss_rssi_meas.to_json(j);
  }
  if (ssb_to_measure_present) {
    j.write_fieldname("ssb-ToMeasure");
    ssb_to_measure.to_json(j);
  }
  j.write_bool("deriveSSB-IndexFromCell", derive_ssb_idx_from_cell);
  if (ext) {
    if (t_resel_nr_sf.is_present()) {
      j.write_fieldname("t-ReselectionNR-SF");
      t_resel_nr_sf->to_json(j);
    }
    if (smtc2_lp_r16.is_present()) {
      j.write_fieldname("smtc2-LP-r16");
      smtc2_lp_r16->to_json(j);
    }
    if (ssb_position_qcl_common_r16_present) {
      j.write_str("ssb-PositionQCL-Common-r16", ssb_position_qcl_common_r16.to_string());
    }
    if (ssb_position_qcl_common_r17_present) {
      j.write_str("ssb-PositionQCL-Common-r17", ssb_position_qcl_common_r17.to_string());
    }
    if (smtc4list_r17.is_present()) {
      j.start_array("smtc4list-r17");
      for (const auto& e1 : *smtc4list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_opts::to_string() const
{
  static const char* names[] = {"dB3", "dB6", "dB9", "dB12", "dB15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_e_");
}
uint8_t sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 5, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_e_");
}

const char* sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_opts::to_string() const
{
  static const char* names[] = {"s5",
                                "s10",
                                "s20",
                                "s30",
                                "s60",
                                "s120",
                                "s180",
                                "s240",
                                "s300",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(
      names, 16, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_e_");
}
uint16_t sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_e_");
}

const char*
sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_opts::to_string() const
{
  static const char* names[] = {"dB2", "dB3", "dB6", "dB9", "dB12", "dB15", "spare2", "spare1"};
  return convert_enum_idx(
      names, 8, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_e_");
}
uint8_t sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 6, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_e_");
}

const char*
sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_opts::to_string() const
{
  static const char* names[] = {"s5",
                                "s10",
                                "s20",
                                "s30",
                                "s60",
                                "s120",
                                "s180",
                                "s240",
                                "s300",
                                "spare7",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(
      names, 16, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_e_");
}
uint16_t
sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_e_");
}

// SIB20-r17 ::= SEQUENCE
SRSASN_CODE sib20_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cfr_cfg_mcch_mtch_r17_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(mcch_cfg_r17.pack(bref));
  if (cfr_cfg_mcch_mtch_r17_present) {
    HANDLE_CODE(cfr_cfg_mcch_mtch_r17.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib20_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cfr_cfg_mcch_mtch_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(mcch_cfg_r17.unpack(bref));
  if (cfr_cfg_mcch_mtch_r17_present) {
    HANDLE_CODE(cfr_cfg_mcch_mtch_r17.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib20_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mcch-Config-r17");
  mcch_cfg_r17.to_json(j);
  if (cfr_cfg_mcch_mtch_r17_present) {
    j.write_fieldname("cfr-ConfigMCCH-MTCH-r17");
    cfr_cfg_mcch_mtch_r17.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB21-r17 ::= SEQUENCE
SRSASN_CODE sib21_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mbs_fsai_intra_freq_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(mbs_fsai_inter_freq_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (mbs_fsai_intra_freq_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_fsai_intra_freq_r17, 1, 64));
  }
  if (mbs_fsai_inter_freq_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_fsai_inter_freq_list_r17, 1, 8));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib21_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool mbs_fsai_intra_freq_r17_present;
  HANDLE_CODE(bref.unpack(mbs_fsai_intra_freq_r17_present, 1));
  bool mbs_fsai_inter_freq_list_r17_present;
  HANDLE_CODE(bref.unpack(mbs_fsai_inter_freq_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (mbs_fsai_intra_freq_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_fsai_intra_freq_r17, bref, 1, 64));
  }
  if (mbs_fsai_inter_freq_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_fsai_inter_freq_list_r17, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib21_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbs_fsai_intra_freq_r17.size() > 0) {
    j.start_array("mbs-FSAI-IntraFreq-r17");
    for (const auto& e1 : mbs_fsai_intra_freq_r17) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (mbs_fsai_inter_freq_list_r17.size() > 0) {
    j.start_array("mbs-FSAI-InterFreqList-r17");
    for (const auto& e1 : mbs_fsai_inter_freq_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB3 ::= SEQUENCE
SRSASN_CODE sib3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(intra_freq_excluded_cell_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (intra_freq_neigh_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_neigh_cell_list, 1, 16));
  }
  if (intra_freq_excluded_cell_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, intra_freq_excluded_cell_list, 1, 16));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= intra_freq_neigh_cell_list_v1610.is_present();
    group_flags[0] |= intra_freq_allowed_cell_list_r16.is_present();
    group_flags[0] |= intra_freq_cag_cell_list_r16.is_present();
    group_flags[1] |= intra_freq_neigh_hsdn_cell_list_r17.is_present();
    group_flags[1] |= intra_freq_neigh_cell_list_v1710.is_present();
    group_flags[2] |= ch_access_mode2_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_allowed_cell_list_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_cag_cell_list_r16.is_present(), 1));
      if (intra_freq_neigh_cell_list_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_cell_list_v1610, 1, 16));
      }
      if (intra_freq_allowed_cell_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_allowed_cell_list_r16, 1, 16));
      }
      if (intra_freq_cag_cell_list_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_cag_cell_list_r16, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(intra_freq_neigh_hsdn_cell_list_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(intra_freq_neigh_cell_list_v1710.is_present(), 1));
      if (intra_freq_neigh_hsdn_cell_list_r17.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_hsdn_cell_list_r17, 1, 16));
      }
      if (intra_freq_neigh_cell_list_v1710.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *intra_freq_neigh_cell_list_v1710, 1, 16));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ch_access_mode2_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool intra_freq_neigh_cell_list_present;
  HANDLE_CODE(bref.unpack(intra_freq_neigh_cell_list_present, 1));
  bool intra_freq_excluded_cell_list_present;
  HANDLE_CODE(bref.unpack(intra_freq_excluded_cell_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (intra_freq_neigh_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_neigh_cell_list, bref, 1, 16));
  }
  if (intra_freq_excluded_cell_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(intra_freq_excluded_cell_list, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(intra_freq_neigh_cell_list_v1610, bref);
      unpack_presence_flag(intra_freq_allowed_cell_list_r16, bref);
      unpack_presence_flag(intra_freq_cag_cell_list_r16, bref);
      if (intra_freq_neigh_cell_list_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_cell_list_v1610, bref, 1, 16));
      }
      if (intra_freq_allowed_cell_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_allowed_cell_list_r16, bref, 1, 16));
      }
      if (intra_freq_cag_cell_list_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_cag_cell_list_r16, bref, 1, 12));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(intra_freq_neigh_hsdn_cell_list_r17, bref);
      unpack_presence_flag(intra_freq_neigh_cell_list_v1710, bref);
      if (intra_freq_neigh_hsdn_cell_list_r17.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_hsdn_cell_list_r17, bref, 1, 16));
      }
      if (intra_freq_neigh_cell_list_v1710.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*intra_freq_neigh_cell_list_v1710, bref, 1, 16));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ch_access_mode2_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sib3_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (intra_freq_neigh_cell_list.size() > 0) {
    j.start_array("intraFreqNeighCellList");
    for (const auto& e1 : intra_freq_neigh_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (intra_freq_excluded_cell_list.size() > 0) {
    j.start_array("intraFreqExcludedCellList");
    for (const auto& e1 : intra_freq_excluded_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (intra_freq_neigh_cell_list_v1610.is_present()) {
      j.start_array("intraFreqNeighCellList-v1610");
      for (const auto& e1 : *intra_freq_neigh_cell_list_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_allowed_cell_list_r16.is_present()) {
      j.start_array("intraFreqAllowedCellList-r16");
      for (const auto& e1 : *intra_freq_allowed_cell_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_cag_cell_list_r16.is_present()) {
      j.start_array("intraFreqCAG-CellList-r16");
      for (const auto& e1 : *intra_freq_cag_cell_list_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_neigh_hsdn_cell_list_r17.is_present()) {
      j.start_array("intraFreqNeighHSDN-CellList-r17");
      for (const auto& e1 : *intra_freq_neigh_hsdn_cell_list_r17) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (intra_freq_neigh_cell_list_v1710.is_present()) {
      j.start_array("intraFreqNeighCellList-v1710");
      for (const auto& e1 : *intra_freq_neigh_cell_list_v1710) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (ch_access_mode2_r17_present) {
      j.write_str("channelAccessMode2-r17", "enabled");
    }
  }
  j.end_obj();
}

// SIB4 ::= SEQUENCE
SRSASN_CODE sib4_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, inter_freq_carrier_freq_list, 1, 8));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= inter_freq_carrier_freq_list_v1610.is_present();
    group_flags[1] |= inter_freq_carrier_freq_list_v1700.is_present();
    group_flags[2] |= inter_freq_carrier_freq_list_v1720.is_present();
    group_flags[3] |= inter_freq_carrier_freq_list_v1730.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1610.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1610, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1700.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1700, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1720.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1720.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1720, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_carrier_freq_list_v1730.is_present(), 1));
      if (inter_freq_carrier_freq_list_v1730.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *inter_freq_carrier_freq_list_v1730, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib4_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(inter_freq_carrier_freq_list, bref, 1, 8));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1610, bref);
      if (inter_freq_carrier_freq_list_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1610, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1700, bref);
      if (inter_freq_carrier_freq_list_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1700, bref, 1, 8));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1720, bref);
      if (inter_freq_carrier_freq_list_v1720.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1720, bref, 1, 8));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(inter_freq_carrier_freq_list_v1730, bref);
      if (inter_freq_carrier_freq_list_v1730.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*inter_freq_carrier_freq_list_v1730, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib4_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("interFreqCarrierFreqList");
  for (const auto& e1 : inter_freq_carrier_freq_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (inter_freq_carrier_freq_list_v1610.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1610");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1700.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1700");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1720.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1720");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1720) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (inter_freq_carrier_freq_list_v1730.is_present()) {
      j.start_array("interFreqCarrierFreqList-v1730");
      for (const auto& e1 : *inter_freq_carrier_freq_list_v1730) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SIB5 ::= SEQUENCE
SRSASN_CODE sib5_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(carrier_freq_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(t_resel_eutra_sf_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (carrier_freq_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, carrier_freq_list_eutra, 1, 8));
  }
  HANDLE_CODE(pack_integer(bref, t_resel_eutra, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_present) {
    HANDLE_CODE(t_resel_eutra_sf.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= carrier_freq_list_eutra_v1610.is_present();
    group_flags[1] |= carrier_freq_list_eutra_v1700.is_present();
    group_flags[1] |= idle_mode_meas_voice_fallback_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_eutra_v1610.is_present(), 1));
      if (carrier_freq_list_eutra_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_eutra_v1610, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(carrier_freq_list_eutra_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(idle_mode_meas_voice_fallback_r17_present, 1));
      if (carrier_freq_list_eutra_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *carrier_freq_list_eutra_v1700, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib5_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool carrier_freq_list_eutra_present;
  HANDLE_CODE(bref.unpack(carrier_freq_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(t_resel_eutra_sf_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (carrier_freq_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(carrier_freq_list_eutra, bref, 1, 8));
  }
  HANDLE_CODE(unpack_integer(t_resel_eutra, bref, (uint8_t)0u, (uint8_t)7u));
  if (t_resel_eutra_sf_present) {
    HANDLE_CODE(t_resel_eutra_sf.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(carrier_freq_list_eutra_v1610, bref);
      if (carrier_freq_list_eutra_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_eutra_v1610, bref, 1, 8));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(carrier_freq_list_eutra_v1700, bref);
      HANDLE_CODE(bref.unpack(idle_mode_meas_voice_fallback_r17_present, 1));
      if (carrier_freq_list_eutra_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*carrier_freq_list_eutra_v1700, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib5_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (carrier_freq_list_eutra.size() > 0) {
    j.start_array("carrierFreqListEUTRA");
    for (const auto& e1 : carrier_freq_list_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("t-ReselectionEUTRA", t_resel_eutra);
  if (t_resel_eutra_sf_present) {
    j.write_fieldname("t-ReselectionEUTRA-SF");
    t_resel_eutra_sf.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (carrier_freq_list_eutra_v1610.is_present()) {
      j.start_array("carrierFreqListEUTRA-v1610");
      for (const auto& e1 : *carrier_freq_list_eutra_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (carrier_freq_list_eutra_v1700.is_present()) {
      j.start_array("carrierFreqListEUTRA-v1700");
      for (const auto& e1 : *carrier_freq_list_eutra_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (idle_mode_meas_voice_fallback_r17_present) {
      j.write_str("idleModeMeasVoiceFallback-r17", "true");
    }
  }
  j.end_obj();
}

// SIB6 ::= SEQUENCE
SRSASN_CODE sib6_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_type.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib6_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_type.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib6_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningType", warning_type.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SIB7 ::= SEQUENCE
SRSASN_CODE sib7_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_coding_scheme_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_msg_segment_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, warning_msg_segment_num, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.pack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib7_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_coding_scheme_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_msg_segment_type.unpack(bref));
  HANDLE_CODE(unpack_integer(warning_msg_segment_num, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.unpack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib7_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningMessageSegmentType", warning_msg_segment_type.to_string());
  j.write_int("warningMessageSegmentNumber", warning_msg_segment_num);
  j.write_str("warningMessageSegment", warning_msg_segment.to_string());
  if (data_coding_scheme_present) {
    j.write_str("dataCodingScheme", data_coding_scheme.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

const char* sib7_s::warning_msg_segment_type_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib7_s::warning_msg_segment_type_e_");
}

// SIB8 ::= SEQUENCE
SRSASN_CODE sib8_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(data_coding_scheme_present, 1));
  HANDLE_CODE(bref.pack(warning_area_coordinates_segment.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(msg_id.pack(bref));
  HANDLE_CODE(serial_num.pack(bref));
  HANDLE_CODE(warning_msg_segment_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, warning_msg_segment_num, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.pack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.pack(bref));
  }
  if (warning_area_coordinates_segment.size() > 0) {
    HANDLE_CODE(warning_area_coordinates_segment.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib8_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(data_coding_scheme_present, 1));
  bool warning_area_coordinates_segment_present;
  HANDLE_CODE(bref.unpack(warning_area_coordinates_segment_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(msg_id.unpack(bref));
  HANDLE_CODE(serial_num.unpack(bref));
  HANDLE_CODE(warning_msg_segment_type.unpack(bref));
  HANDLE_CODE(unpack_integer(warning_msg_segment_num, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(warning_msg_segment.unpack(bref));
  if (data_coding_scheme_present) {
    HANDLE_CODE(data_coding_scheme.unpack(bref));
  }
  if (warning_area_coordinates_segment_present) {
    HANDLE_CODE(warning_area_coordinates_segment.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib8_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("messageIdentifier", msg_id.to_string());
  j.write_str("serialNumber", serial_num.to_string());
  j.write_str("warningMessageSegmentType", warning_msg_segment_type.to_string());
  j.write_int("warningMessageSegmentNumber", warning_msg_segment_num);
  j.write_str("warningMessageSegment", warning_msg_segment.to_string());
  if (data_coding_scheme_present) {
    j.write_str("dataCodingScheme", data_coding_scheme.to_string());
  }
  if (warning_area_coordinates_segment.size() > 0) {
    j.write_str("warningAreaCoordinatesSegment", warning_area_coordinates_segment.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

const char* sib8_s::warning_msg_segment_type_opts::to_string() const
{
  static const char* names[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(names, 2, value, "sib8_s::warning_msg_segment_type_e_");
}

// SIB9 ::= SEQUENCE
SRSASN_CODE sib9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(time_info_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  if (time_info_present) {
    HANDLE_CODE(bref.pack(time_info.day_light_saving_time_present, 1));
    HANDLE_CODE(bref.pack(time_info.leap_seconds_present, 1));
    HANDLE_CODE(bref.pack(time_info.local_time_offset_present, 1));
    HANDLE_CODE(pack_integer(bref, time_info.time_info_utc, (uint64_t)0u, (uint64_t)549755813887u));
    if (time_info.day_light_saving_time_present) {
      HANDLE_CODE(time_info.day_light_saving_time.pack(bref));
    }
    if (time_info.leap_seconds_present) {
      HANDLE_CODE(pack_integer(bref, time_info.leap_seconds, (int16_t)-127, (int16_t)128));
    }
    if (time_info.local_time_offset_present) {
      HANDLE_CODE(pack_integer(bref, time_info.local_time_offset, (int8_t)-63, (int8_t)64));
    }
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ref_time_info_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ref_time_info_r16.is_present(), 1));
      if (ref_time_info_r16.is_present()) {
        HANDLE_CODE(ref_time_info_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(time_info_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  if (time_info_present) {
    HANDLE_CODE(bref.unpack(time_info.day_light_saving_time_present, 1));
    HANDLE_CODE(bref.unpack(time_info.leap_seconds_present, 1));
    HANDLE_CODE(bref.unpack(time_info.local_time_offset_present, 1));
    HANDLE_CODE(unpack_integer(time_info.time_info_utc, bref, (uint64_t)0u, (uint64_t)549755813887u));
    if (time_info.day_light_saving_time_present) {
      HANDLE_CODE(time_info.day_light_saving_time.unpack(bref));
    }
    if (time_info.leap_seconds_present) {
      HANDLE_CODE(unpack_integer(time_info.leap_seconds, bref, (int16_t)-127, (int16_t)128));
    }
    if (time_info.local_time_offset_present) {
      HANDLE_CODE(unpack_integer(time_info.local_time_offset, bref, (int8_t)-63, (int8_t)64));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ref_time_info_r16, bref);
      if (ref_time_info_r16.is_present()) {
        HANDLE_CODE(ref_time_info_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib9_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (time_info_present) {
    j.write_fieldname("timeInfo");
    j.start_obj();
    j.write_int("timeInfoUTC", time_info.time_info_utc);
    if (time_info.day_light_saving_time_present) {
      j.write_str("dayLightSavingTime", time_info.day_light_saving_time.to_string());
    }
    if (time_info.leap_seconds_present) {
      j.write_int("leapSeconds", time_info.leap_seconds);
    }
    if (time_info.local_time_offset_present) {
      j.write_int("localTimeOffset", time_info.local_time_offset);
    }
    j.end_obj();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (ref_time_info_r16.is_present()) {
      j.write_fieldname("referenceTimeInfo-r16");
      ref_time_info_r16->to_json(j);
    }
  }
  j.end_obj();
}

// SIBpos-r16 ::= SEQUENCE
SRSASN_CODE sib_pos_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(assist_data_sib_elem_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib_pos_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(assist_data_sib_elem_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib_pos_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("assistanceDataSIB-Element-r16", assist_data_sib_elem_r16.to_string());
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

// SchedulingInfo ::= SEQUENCE
SRSASN_CODE sched_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(si_broadcast_status.pack(bref));
  HANDLE_CODE(si_periodicity.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, sib_map_info, 1, 32));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sched_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(si_broadcast_status.unpack(bref));
  HANDLE_CODE(si_periodicity.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(sib_map_info, bref, 1, 32));

  return SRSASN_SUCCESS;
}
void sched_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("si-BroadcastStatus", si_broadcast_status.to_string());
  j.write_str("si-Periodicity", si_periodicity.to_string());
  j.start_array("sib-MappingInfo");
  for (const auto& e1 : sib_map_info) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

const char* sched_info_s::si_broadcast_status_opts::to_string() const
{
  static const char* names[] = {"broadcasting", "notBroadcasting"};
  return convert_enum_idx(names, 2, value, "sched_info_s::si_broadcast_status_e_");
}

const char* sched_info_s::si_periodicity_opts::to_string() const
{
  static const char* names[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(names, 7, value, "sched_info_s::si_periodicity_e_");
}
uint16_t sched_info_s::si_periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "sched_info_s::si_periodicity_e_");
}

// UAC-BarringInfoSet ::= SEQUENCE
SRSASN_CODE uac_barr_info_set_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(uac_barr_factor.pack(bref));
  HANDLE_CODE(uac_barr_time.pack(bref));
  HANDLE_CODE(uac_barr_for_access_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_info_set_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(uac_barr_factor.unpack(bref));
  HANDLE_CODE(uac_barr_time.unpack(bref));
  HANDLE_CODE(uac_barr_for_access_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void uac_barr_info_set_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("uac-BarringFactor", uac_barr_factor.to_string());
  j.write_str("uac-BarringTime", uac_barr_time.to_string());
  j.write_str("uac-BarringForAccessIdentity", uac_barr_for_access_id.to_string());
  j.end_obj();
}

const char* uac_barr_info_set_s::uac_barr_factor_opts::to_string() const
{
  static const char* names[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(names, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}
float uac_barr_info_set_s::uac_barr_factor_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(numbers, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}
const char* uac_barr_info_set_s::uac_barr_factor_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(number_strs, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}

const char* uac_barr_info_set_s::uac_barr_time_opts::to_string() const
{
  static const char* names[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(names, 8, value, "uac_barr_info_set_s::uac_barr_time_e_");
}
uint16_t uac_barr_info_set_s::uac_barr_time_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 8, value, "uac_barr_info_set_s::uac_barr_time_e_");
}

// UAC-BarringPerPLMN ::= SEQUENCE
SRSASN_CODE uac_barr_per_plmn_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(uac_ac_barr_list_type_present, 1));

  HANDLE_CODE(pack_integer(bref, plmn_id_idx, (uint8_t)1u, (uint8_t)12u));
  if (uac_ac_barr_list_type_present) {
    HANDLE_CODE(uac_ac_barr_list_type.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_plmn_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(uac_ac_barr_list_type_present, 1));

  HANDLE_CODE(unpack_integer(plmn_id_idx, bref, (uint8_t)1u, (uint8_t)12u));
  if (uac_ac_barr_list_type_present) {
    HANDLE_CODE(uac_ac_barr_list_type.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void uac_barr_per_plmn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("plmn-IdentityIndex", plmn_id_idx);
  if (uac_ac_barr_list_type_present) {
    j.write_fieldname("uac-ACBarringListType");
    uac_ac_barr_list_type.to_json(j);
  }
  j.end_obj();
}

void uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::destroy_()
{
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.destroy<uac_implicit_ac_barr_list_l_>();
      break;
    case types::uac_explicit_ac_barr_list:
      c.destroy<uac_barr_per_cat_list_l>();
      break;
    default:
      break;
  }
}
void uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.init<uac_implicit_ac_barr_list_l_>();
      break;
    case types::uac_explicit_ac_barr_list:
      c.init<uac_barr_per_cat_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }
}
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::uac_ac_barr_list_type_c_(
    const uac_barr_per_plmn_s::uac_ac_barr_list_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.init(other.c.get<uac_implicit_ac_barr_list_l_>());
      break;
    case types::uac_explicit_ac_barr_list:
      c.init(other.c.get<uac_barr_per_cat_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }
}
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_&
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::operator=(const uac_barr_per_plmn_s::uac_ac_barr_list_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      c.set(other.c.get<uac_implicit_ac_barr_list_l_>());
      break;
    case types::uac_explicit_ac_barr_list:
      c.set(other.c.get<uac_barr_per_cat_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }

  return *this;
}
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::uac_implicit_ac_barr_list_l_&
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set_uac_implicit_ac_barr_list()
{
  set(types::uac_implicit_ac_barr_list);
  return c.get<uac_implicit_ac_barr_list_l_>();
}
uac_barr_per_cat_list_l& uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set_uac_explicit_ac_barr_list()
{
  set(types::uac_explicit_ac_barr_list);
  return c.get<uac_barr_per_cat_list_l>();
}
void uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      j.start_array("uac-ImplicitACBarringList");
      for (const auto& e1 : c.get<uac_implicit_ac_barr_list_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::uac_explicit_ac_barr_list:
      j.start_array("uac-ExplicitACBarringList");
      for (const auto& e1 : c.get<uac_barr_per_cat_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    c.get<uac_implicit_ac_barr_list_l_>(),
                                    c.get<uac_implicit_ac_barr_list_l_>().size(),
                                    integer_packer<uint8_t>(1, 8)));
      break;
    case types::uac_explicit_ac_barr_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uac_barr_per_cat_list_l>(), 1, 63));
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::uac_implicit_ac_barr_list:
      HANDLE_CODE(unpack_fixed_seq_of(c.get<uac_implicit_ac_barr_list_l_>(),
                                      bref,
                                      c.get<uac_implicit_ac_barr_list_l_>().size(),
                                      integer_packer<uint8_t>(1, 8)));
      break;
    case types::uac_explicit_ac_barr_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uac_barr_per_cat_list_l>(), bref, 1, 63));
      break;
    default:
      log_invalid_choice_id(type_, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"uac-ImplicitACBarringList", "uac-ExplicitACBarringList"};
  return convert_enum_idx(names, 2, value, "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types");
}

// ConnEstFailureControl ::= SEQUENCE
SRSASN_CODE conn_est_fail_ctrl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(conn_est_fail_offset_present, 1));

  HANDLE_CODE(conn_est_fail_count.pack(bref));
  HANDLE_CODE(conn_est_fail_offset_validity.pack(bref));
  if (conn_est_fail_offset_present) {
    HANDLE_CODE(pack_integer(bref, conn_est_fail_offset, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE conn_est_fail_ctrl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(conn_est_fail_offset_present, 1));

  HANDLE_CODE(conn_est_fail_count.unpack(bref));
  HANDLE_CODE(conn_est_fail_offset_validity.unpack(bref));
  if (conn_est_fail_offset_present) {
    HANDLE_CODE(unpack_integer(conn_est_fail_offset, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void conn_est_fail_ctrl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("connEstFailCount", conn_est_fail_count.to_string());
  j.write_str("connEstFailOffsetValidity", conn_est_fail_offset_validity.to_string());
  if (conn_est_fail_offset_present) {
    j.write_int("connEstFailOffset", conn_est_fail_offset);
  }
  j.end_obj();
}

const char* conn_est_fail_ctrl_s::conn_est_fail_count_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(names, 4, value, "conn_est_fail_ctrl_s::conn_est_fail_count_e_");
}
uint8_t conn_est_fail_ctrl_s::conn_est_fail_count_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "conn_est_fail_ctrl_s::conn_est_fail_count_e_");
}

const char* conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::to_string() const
{
  static const char* names[] = {"s30", "s60", "s120", "s240", "s300", "s420", "s600", "s900"};
  return convert_enum_idx(names, 8, value, "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_");
}
uint16_t conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::to_number() const
{
  static const uint16_t numbers[] = {30, 60, 120, 240, 300, 420, 600, 900};
  return map_enum_number(numbers, 8, value, "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_");
}

// PosSystemInformation-r16-IEs ::= SEQUENCE
SRSASN_CODE pos_sys_info_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, pos_sib_type_and_info_r16, 1, 32));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sys_info_r16_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(pos_sib_type_and_info_r16, bref, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pos_sys_info_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("posSIB-TypeAndInfo-r16");
  for (const auto& e1 : pos_sib_type_and_info_r16) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

void pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::destroy_()
{
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_2_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_3_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_4_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_5_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_6_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_7_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_8_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_2_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_3_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_4_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_5_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_6_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_7_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_8_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_9_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_10_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_11_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_12_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_13_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_14_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_15_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_16_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_17_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_18_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_19_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_20_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_21_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_22_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_23_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib3_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib4_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib5_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_1_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_2_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_3_r16:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_9_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib1_10_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_24_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib2_25_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_4_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_5_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    case types::pos_sib6_6_v1700:
      c.destroy<sib_pos_r16_s>();
      break;
    default:
      break;
  }
}
void pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_2_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_3_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_4_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_5_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_6_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_7_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_8_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_2_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_3_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_4_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_5_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_6_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_7_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_8_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_9_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_10_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_11_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_12_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_13_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_14_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_15_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_16_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_17_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_18_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_19_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_20_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_21_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_22_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_23_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib3_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib4_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib5_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_1_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_2_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_3_r16:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_9_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib1_10_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_24_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib2_25_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_4_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_5_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::pos_sib6_6_v1700:
      c.init<sib_pos_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }
}
pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::pos_sib_type_and_info_r16_item_c_(
    const pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_2_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_3_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_4_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_5_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_6_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_7_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_8_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_2_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_3_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_4_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_5_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_6_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_7_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_8_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_9_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_10_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_11_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_12_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_13_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_14_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_15_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_16_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_17_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_18_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_19_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_20_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_21_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_22_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_23_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib3_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib4_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib5_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_1_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_2_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_3_r16:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_9_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_10_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_24_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_25_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_4_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_5_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_6_v1700:
      c.init(other.c.get<sib_pos_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }
}
pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_&
pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::operator=(
    const pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pos_sib1_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_2_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_3_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_4_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_5_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_6_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_7_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_8_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_2_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_3_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_4_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_5_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_6_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_7_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_8_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_9_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_10_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_11_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_12_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_13_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_14_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_15_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_16_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_17_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_18_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_19_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_20_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_21_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_22_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_23_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib3_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib4_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib5_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_1_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_2_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_3_r16:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_9_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib1_10_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_24_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib2_25_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_4_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_5_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::pos_sib6_6_v1700:
      c.set(other.c.get<sib_pos_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }

  return *this;
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_1_r16()
{
  set(types::pos_sib1_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_2_r16()
{
  set(types::pos_sib1_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_3_r16()
{
  set(types::pos_sib1_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_4_r16()
{
  set(types::pos_sib1_4_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_5_r16()
{
  set(types::pos_sib1_5_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_6_r16()
{
  set(types::pos_sib1_6_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_7_r16()
{
  set(types::pos_sib1_7_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_8_r16()
{
  set(types::pos_sib1_8_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_1_r16()
{
  set(types::pos_sib2_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_2_r16()
{
  set(types::pos_sib2_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_3_r16()
{
  set(types::pos_sib2_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_4_r16()
{
  set(types::pos_sib2_4_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_5_r16()
{
  set(types::pos_sib2_5_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_6_r16()
{
  set(types::pos_sib2_6_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_7_r16()
{
  set(types::pos_sib2_7_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_8_r16()
{
  set(types::pos_sib2_8_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_9_r16()
{
  set(types::pos_sib2_9_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_10_r16()
{
  set(types::pos_sib2_10_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_11_r16()
{
  set(types::pos_sib2_11_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_12_r16()
{
  set(types::pos_sib2_12_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_13_r16()
{
  set(types::pos_sib2_13_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_14_r16()
{
  set(types::pos_sib2_14_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_15_r16()
{
  set(types::pos_sib2_15_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_16_r16()
{
  set(types::pos_sib2_16_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_17_r16()
{
  set(types::pos_sib2_17_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_18_r16()
{
  set(types::pos_sib2_18_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_19_r16()
{
  set(types::pos_sib2_19_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_20_r16()
{
  set(types::pos_sib2_20_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_21_r16()
{
  set(types::pos_sib2_21_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_22_r16()
{
  set(types::pos_sib2_22_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_23_r16()
{
  set(types::pos_sib2_23_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib3_1_r16()
{
  set(types::pos_sib3_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib4_1_r16()
{
  set(types::pos_sib4_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib5_1_r16()
{
  set(types::pos_sib5_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_1_r16()
{
  set(types::pos_sib6_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_2_r16()
{
  set(types::pos_sib6_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_3_r16()
{
  set(types::pos_sib6_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_9_v1700()
{
  set(types::pos_sib1_9_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib1_10_v1700()
{
  set(types::pos_sib1_10_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_24_v1700()
{
  set(types::pos_sib2_24_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib2_25_v1700()
{
  set(types::pos_sib2_25_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_4_v1700()
{
  set(types::pos_sib6_4_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_5_v1700()
{
  set(types::pos_sib6_5_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::set_pos_sib6_6_v1700()
{
  set(types::pos_sib6_6_v1700);
  return c.get<sib_pos_r16_s>();
}
void pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pos_sib1_1_r16:
      j.write_fieldname("posSib1-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_2_r16:
      j.write_fieldname("posSib1-2-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_3_r16:
      j.write_fieldname("posSib1-3-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_4_r16:
      j.write_fieldname("posSib1-4-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_5_r16:
      j.write_fieldname("posSib1-5-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_6_r16:
      j.write_fieldname("posSib1-6-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_7_r16:
      j.write_fieldname("posSib1-7-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_8_r16:
      j.write_fieldname("posSib1-8-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_1_r16:
      j.write_fieldname("posSib2-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_2_r16:
      j.write_fieldname("posSib2-2-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_3_r16:
      j.write_fieldname("posSib2-3-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_4_r16:
      j.write_fieldname("posSib2-4-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_5_r16:
      j.write_fieldname("posSib2-5-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_6_r16:
      j.write_fieldname("posSib2-6-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_7_r16:
      j.write_fieldname("posSib2-7-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_8_r16:
      j.write_fieldname("posSib2-8-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_9_r16:
      j.write_fieldname("posSib2-9-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_10_r16:
      j.write_fieldname("posSib2-10-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_11_r16:
      j.write_fieldname("posSib2-11-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_12_r16:
      j.write_fieldname("posSib2-12-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_13_r16:
      j.write_fieldname("posSib2-13-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_14_r16:
      j.write_fieldname("posSib2-14-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_15_r16:
      j.write_fieldname("posSib2-15-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_16_r16:
      j.write_fieldname("posSib2-16-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_17_r16:
      j.write_fieldname("posSib2-17-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_18_r16:
      j.write_fieldname("posSib2-18-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_19_r16:
      j.write_fieldname("posSib2-19-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_20_r16:
      j.write_fieldname("posSib2-20-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_21_r16:
      j.write_fieldname("posSib2-21-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_22_r16:
      j.write_fieldname("posSib2-22-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_23_r16:
      j.write_fieldname("posSib2-23-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib3_1_r16:
      j.write_fieldname("posSib3-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib4_1_r16:
      j.write_fieldname("posSib4-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib5_1_r16:
      j.write_fieldname("posSib5-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_1_r16:
      j.write_fieldname("posSib6-1-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_2_r16:
      j.write_fieldname("posSib6-2-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_3_r16:
      j.write_fieldname("posSib6-3-r16");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_9_v1700:
      j.write_fieldname("posSib1-9-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib1_10_v1700:
      j.write_fieldname("posSib1-10-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_24_v1700:
      j.write_fieldname("posSib2-24-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib2_25_v1700:
      j.write_fieldname("posSib2-25-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_4_v1700:
      j.write_fieldname("posSib6-4-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_5_v1700:
      j.write_fieldname("posSib6-5-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    case types::pos_sib6_6_v1700:
      j.write_fieldname("posSib6-6-v1700");
      c.get<sib_pos_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pos_sib1_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_9_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_10_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_11_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_12_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_13_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_14_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_15_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_16_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_17_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_18_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_19_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_20_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_21_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_22_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib2_23_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib3_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib4_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib5_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib6_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib6_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib6_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
      break;
    case types::pos_sib1_9_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib1_10_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib2_24_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib2_25_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib6_4_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib6_5_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    case types::pos_sib6_6_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pos_sib1_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_4_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_5_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_6_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_7_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_8_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_9_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_10_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_11_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_12_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_13_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_14_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_15_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_16_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_17_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_18_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_19_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_20_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_21_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_22_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib2_23_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib3_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib4_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib5_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib6_1_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib6_2_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib6_3_r16:
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
      break;
    case types::pos_sib1_9_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib1_10_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib2_24_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib2_25_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib6_4_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib6_5_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    case types::pos_sib6_6_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib_pos_r16_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "posSib1-1-r16",    "posSib1-2-r16",   "posSib1-3-r16",   "posSib1-4-r16",    "posSib1-5-r16",
      "posSib1-6-r16",    "posSib1-7-r16",   "posSib1-8-r16",   "posSib2-1-r16",    "posSib2-2-r16",
      "posSib2-3-r16",    "posSib2-4-r16",   "posSib2-5-r16",   "posSib2-6-r16",    "posSib2-7-r16",
      "posSib2-8-r16",    "posSib2-9-r16",   "posSib2-10-r16",  "posSib2-11-r16",   "posSib2-12-r16",
      "posSib2-13-r16",   "posSib2-14-r16",  "posSib2-15-r16",  "posSib2-16-r16",   "posSib2-17-r16",
      "posSib2-18-r16",   "posSib2-19-r16",  "posSib2-20-r16",  "posSib2-21-r16",   "posSib2-22-r16",
      "posSib2-23-r16",   "posSib3-1-r16",   "posSib4-1-r16",   "posSib5-1-r16",    "posSib6-1-r16",
      "posSib6-2-r16",    "posSib6-3-r16",   "posSib1-9-v1700", "posSib1-10-v1700", "posSib2-24-v1700",
      "posSib2-25-v1700", "posSib6-4-v1700", "posSib6-5-v1700", "posSib6-6-v1700"};
  return convert_enum_idx(names, 44, value, "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_item_c_::types");
}

// SI-SchedulingInfo ::= SEQUENCE
SRSASN_CODE si_sched_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(si_request_cfg_present, 1));
  HANDLE_CODE(bref.pack(si_request_cfg_sul_present, 1));
  HANDLE_CODE(bref.pack(sys_info_area_id_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sched_info_list, 1, 32));
  HANDLE_CODE(si_win_len.pack(bref));
  if (si_request_cfg_present) {
    HANDLE_CODE(si_request_cfg.pack(bref));
  }
  if (si_request_cfg_sul_present) {
    HANDLE_CODE(si_request_cfg_sul.pack(bref));
  }
  if (sys_info_area_id_present) {
    HANDLE_CODE(sys_info_area_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE si_sched_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(si_request_cfg_present, 1));
  HANDLE_CODE(bref.unpack(si_request_cfg_sul_present, 1));
  HANDLE_CODE(bref.unpack(sys_info_area_id_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sched_info_list, bref, 1, 32));
  HANDLE_CODE(si_win_len.unpack(bref));
  if (si_request_cfg_present) {
    HANDLE_CODE(si_request_cfg.unpack(bref));
  }
  if (si_request_cfg_sul_present) {
    HANDLE_CODE(si_request_cfg_sul.unpack(bref));
  }
  if (sys_info_area_id_present) {
    HANDLE_CODE(sys_info_area_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void si_sched_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("schedulingInfoList");
  for (const auto& e1 : sched_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("si-WindowLength", si_win_len.to_string());
  if (si_request_cfg_present) {
    j.write_fieldname("si-RequestConfig");
    si_request_cfg.to_json(j);
  }
  if (si_request_cfg_sul_present) {
    j.write_fieldname("si-RequestConfigSUL");
    si_request_cfg_sul.to_json(j);
  }
  if (sys_info_area_id_present) {
    j.write_str("systemInformationAreaID", sys_info_area_id.to_string());
  }
  j.end_obj();
}

const char* si_sched_info_s::si_win_len_opts::to_string() const
{
  static const char* names[] = {
      "s5", "s10", "s20", "s40", "s80", "s160", "s320", "s640", "s1280", "s2560-v1710", "s5120-v1710"};
  return convert_enum_idx(names, 11, value, "si_sched_info_s::si_win_len_e_");
}
uint16_t si_sched_info_s::si_win_len_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(numbers, 11, value, "si_sched_info_s::si_win_len_e_");
}

// SIB1-v1610-IEs ::= SEQUENCE
SRSASN_CODE sib1_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(idle_mode_meass_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(idle_mode_meass_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(pos_si_sched_info_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pos_si_sched_info_r16_present) {
    HANDLE_CODE(pos_si_sched_info_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(idle_mode_meass_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(idle_mode_meass_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(pos_si_sched_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pos_si_sched_info_r16_present) {
    HANDLE_CODE(pos_si_sched_info_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (idle_mode_meass_eutra_r16_present) {
    j.write_str("idleModeMeasurementsEUTRA-r16", "true");
  }
  if (idle_mode_meass_nr_r16_present) {
    j.write_str("idleModeMeasurementsNR-r16", "true");
  }
  if (pos_si_sched_info_r16_present) {
    j.write_fieldname("posSI-SchedulingInfo-r16");
    pos_si_sched_info_r16.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(names, 2, value, "serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types");
}

// SystemInformation-IEs ::= SEQUENCE
SRSASN_CODE sys_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sib_type_and_info, 1, 32));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sib_type_and_info, bref, 1, 32));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sys_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sib-TypeAndInfo");
  for (const auto& e1 : sib_type_and_info) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

void sys_info_ies_s::sib_type_and_info_item_c_::destroy_()
{
  switch (type_) {
    case types::sib2:
      c.destroy<sib2_s>();
      break;
    case types::sib3:
      c.destroy<sib3_s>();
      break;
    case types::sib4:
      c.destroy<sib4_s>();
      break;
    case types::sib5:
      c.destroy<sib5_s>();
      break;
    case types::sib6:
      c.destroy<sib6_s>();
      break;
    case types::sib7:
      c.destroy<sib7_s>();
      break;
    case types::sib8:
      c.destroy<sib8_s>();
      break;
    case types::sib9:
      c.destroy<sib9_s>();
      break;
    case types::sib10_v1610:
      c.destroy<sib10_r16_s>();
      break;
    case types::sib11_v1610:
      c.destroy<sib11_r16_s>();
      break;
    case types::sib12_v1610:
      c.destroy<sib12_r16_s>();
      break;
    case types::sib13_v1610:
      c.destroy<sib13_r16_s>();
      break;
    case types::sib14_v1610:
      c.destroy<sib14_r16_s>();
      break;
    case types::sib15_v1700:
      c.destroy<sib15_r17_s>();
      break;
    case types::sib16_v1700:
      c.destroy<sib16_r17_s>();
      break;
    case types::sib17_v1700:
      c.destroy<sib17_r17_s>();
      break;
    case types::sib18_v1700:
      c.destroy<sib18_r17_s>();
      break;
    case types::sib19_v1700:
      c.destroy<sib19_r17_s>();
      break;
    case types::sib20_v1700:
      c.destroy<sib20_r17_s>();
      break;
    case types::sib21_v1700:
      c.destroy<sib21_r17_s>();
      break;
    default:
      break;
  }
}
void sys_info_ies_s::sib_type_and_info_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sib2:
      c.init<sib2_s>();
      break;
    case types::sib3:
      c.init<sib3_s>();
      break;
    case types::sib4:
      c.init<sib4_s>();
      break;
    case types::sib5:
      c.init<sib5_s>();
      break;
    case types::sib6:
      c.init<sib6_s>();
      break;
    case types::sib7:
      c.init<sib7_s>();
      break;
    case types::sib8:
      c.init<sib8_s>();
      break;
    case types::sib9:
      c.init<sib9_s>();
      break;
    case types::sib10_v1610:
      c.init<sib10_r16_s>();
      break;
    case types::sib11_v1610:
      c.init<sib11_r16_s>();
      break;
    case types::sib12_v1610:
      c.init<sib12_r16_s>();
      break;
    case types::sib13_v1610:
      c.init<sib13_r16_s>();
      break;
    case types::sib14_v1610:
      c.init<sib14_r16_s>();
      break;
    case types::sib15_v1700:
      c.init<sib15_r17_s>();
      break;
    case types::sib16_v1700:
      c.init<sib16_r17_s>();
      break;
    case types::sib17_v1700:
      c.init<sib17_r17_s>();
      break;
    case types::sib18_v1700:
      c.init<sib18_r17_s>();
      break;
    case types::sib19_v1700:
      c.init<sib19_r17_s>();
      break;
    case types::sib20_v1700:
      c.init<sib20_r17_s>();
      break;
    case types::sib21_v1700:
      c.init<sib21_r17_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }
}
sys_info_ies_s::sib_type_and_info_item_c_::sib_type_and_info_item_c_(
    const sys_info_ies_s::sib_type_and_info_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sib2:
      c.init(other.c.get<sib2_s>());
      break;
    case types::sib3:
      c.init(other.c.get<sib3_s>());
      break;
    case types::sib4:
      c.init(other.c.get<sib4_s>());
      break;
    case types::sib5:
      c.init(other.c.get<sib5_s>());
      break;
    case types::sib6:
      c.init(other.c.get<sib6_s>());
      break;
    case types::sib7:
      c.init(other.c.get<sib7_s>());
      break;
    case types::sib8:
      c.init(other.c.get<sib8_s>());
      break;
    case types::sib9:
      c.init(other.c.get<sib9_s>());
      break;
    case types::sib10_v1610:
      c.init(other.c.get<sib10_r16_s>());
      break;
    case types::sib11_v1610:
      c.init(other.c.get<sib11_r16_s>());
      break;
    case types::sib12_v1610:
      c.init(other.c.get<sib12_r16_s>());
      break;
    case types::sib13_v1610:
      c.init(other.c.get<sib13_r16_s>());
      break;
    case types::sib14_v1610:
      c.init(other.c.get<sib14_r16_s>());
      break;
    case types::sib15_v1700:
      c.init(other.c.get<sib15_r17_s>());
      break;
    case types::sib16_v1700:
      c.init(other.c.get<sib16_r17_s>());
      break;
    case types::sib17_v1700:
      c.init(other.c.get<sib17_r17_s>());
      break;
    case types::sib18_v1700:
      c.init(other.c.get<sib18_r17_s>());
      break;
    case types::sib19_v1700:
      c.init(other.c.get<sib19_r17_s>());
      break;
    case types::sib20_v1700:
      c.init(other.c.get<sib20_r17_s>());
      break;
    case types::sib21_v1700:
      c.init(other.c.get<sib21_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }
}
sys_info_ies_s::sib_type_and_info_item_c_&
sys_info_ies_s::sib_type_and_info_item_c_::operator=(const sys_info_ies_s::sib_type_and_info_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sib2:
      c.set(other.c.get<sib2_s>());
      break;
    case types::sib3:
      c.set(other.c.get<sib3_s>());
      break;
    case types::sib4:
      c.set(other.c.get<sib4_s>());
      break;
    case types::sib5:
      c.set(other.c.get<sib5_s>());
      break;
    case types::sib6:
      c.set(other.c.get<sib6_s>());
      break;
    case types::sib7:
      c.set(other.c.get<sib7_s>());
      break;
    case types::sib8:
      c.set(other.c.get<sib8_s>());
      break;
    case types::sib9:
      c.set(other.c.get<sib9_s>());
      break;
    case types::sib10_v1610:
      c.set(other.c.get<sib10_r16_s>());
      break;
    case types::sib11_v1610:
      c.set(other.c.get<sib11_r16_s>());
      break;
    case types::sib12_v1610:
      c.set(other.c.get<sib12_r16_s>());
      break;
    case types::sib13_v1610:
      c.set(other.c.get<sib13_r16_s>());
      break;
    case types::sib14_v1610:
      c.set(other.c.get<sib14_r16_s>());
      break;
    case types::sib15_v1700:
      c.set(other.c.get<sib15_r17_s>());
      break;
    case types::sib16_v1700:
      c.set(other.c.get<sib16_r17_s>());
      break;
    case types::sib17_v1700:
      c.set(other.c.get<sib17_r17_s>());
      break;
    case types::sib18_v1700:
      c.set(other.c.get<sib18_r17_s>());
      break;
    case types::sib19_v1700:
      c.set(other.c.get<sib19_r17_s>());
      break;
    case types::sib20_v1700:
      c.set(other.c.get<sib20_r17_s>());
      break;
    case types::sib21_v1700:
      c.set(other.c.get<sib21_r17_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }

  return *this;
}
sib2_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib2()
{
  set(types::sib2);
  return c.get<sib2_s>();
}
sib3_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib3()
{
  set(types::sib3);
  return c.get<sib3_s>();
}
sib4_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib4()
{
  set(types::sib4);
  return c.get<sib4_s>();
}
sib5_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib5()
{
  set(types::sib5);
  return c.get<sib5_s>();
}
sib6_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib6()
{
  set(types::sib6);
  return c.get<sib6_s>();
}
sib7_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib7()
{
  set(types::sib7);
  return c.get<sib7_s>();
}
sib8_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib8()
{
  set(types::sib8);
  return c.get<sib8_s>();
}
sib9_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib9()
{
  set(types::sib9);
  return c.get<sib9_s>();
}
sib10_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib10_v1610()
{
  set(types::sib10_v1610);
  return c.get<sib10_r16_s>();
}
sib11_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib11_v1610()
{
  set(types::sib11_v1610);
  return c.get<sib11_r16_s>();
}
sib12_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib12_v1610()
{
  set(types::sib12_v1610);
  return c.get<sib12_r16_s>();
}
sib13_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib13_v1610()
{
  set(types::sib13_v1610);
  return c.get<sib13_r16_s>();
}
sib14_r16_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib14_v1610()
{
  set(types::sib14_v1610);
  return c.get<sib14_r16_s>();
}
sib15_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib15_v1700()
{
  set(types::sib15_v1700);
  return c.get<sib15_r17_s>();
}
sib16_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib16_v1700()
{
  set(types::sib16_v1700);
  return c.get<sib16_r17_s>();
}
sib17_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib17_v1700()
{
  set(types::sib17_v1700);
  return c.get<sib17_r17_s>();
}
sib18_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib18_v1700()
{
  set(types::sib18_v1700);
  return c.get<sib18_r17_s>();
}
sib19_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib19_v1700()
{
  set(types::sib19_v1700);
  return c.get<sib19_r17_s>();
}
sib20_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib20_v1700()
{
  set(types::sib20_v1700);
  return c.get<sib20_r17_s>();
}
sib21_r17_s& sys_info_ies_s::sib_type_and_info_item_c_::set_sib21_v1700()
{
  set(types::sib21_v1700);
  return c.get<sib21_r17_s>();
}
void sys_info_ies_s::sib_type_and_info_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sib2:
      j.write_fieldname("sib2");
      c.get<sib2_s>().to_json(j);
      break;
    case types::sib3:
      j.write_fieldname("sib3");
      c.get<sib3_s>().to_json(j);
      break;
    case types::sib4:
      j.write_fieldname("sib4");
      c.get<sib4_s>().to_json(j);
      break;
    case types::sib5:
      j.write_fieldname("sib5");
      c.get<sib5_s>().to_json(j);
      break;
    case types::sib6:
      j.write_fieldname("sib6");
      c.get<sib6_s>().to_json(j);
      break;
    case types::sib7:
      j.write_fieldname("sib7");
      c.get<sib7_s>().to_json(j);
      break;
    case types::sib8:
      j.write_fieldname("sib8");
      c.get<sib8_s>().to_json(j);
      break;
    case types::sib9:
      j.write_fieldname("sib9");
      c.get<sib9_s>().to_json(j);
      break;
    case types::sib10_v1610:
      j.write_fieldname("sib10-v1610");
      c.get<sib10_r16_s>().to_json(j);
      break;
    case types::sib11_v1610:
      j.write_fieldname("sib11-v1610");
      c.get<sib11_r16_s>().to_json(j);
      break;
    case types::sib12_v1610:
      j.write_fieldname("sib12-v1610");
      c.get<sib12_r16_s>().to_json(j);
      break;
    case types::sib13_v1610:
      j.write_fieldname("sib13-v1610");
      c.get<sib13_r16_s>().to_json(j);
      break;
    case types::sib14_v1610:
      j.write_fieldname("sib14-v1610");
      c.get<sib14_r16_s>().to_json(j);
      break;
    case types::sib15_v1700:
      j.write_fieldname("sib15-v1700");
      c.get<sib15_r17_s>().to_json(j);
      break;
    case types::sib16_v1700:
      j.write_fieldname("sib16-v1700");
      c.get<sib16_r17_s>().to_json(j);
      break;
    case types::sib17_v1700:
      j.write_fieldname("sib17-v1700");
      c.get<sib17_r17_s>().to_json(j);
      break;
    case types::sib18_v1700:
      j.write_fieldname("sib18-v1700");
      c.get<sib18_r17_s>().to_json(j);
      break;
    case types::sib19_v1700:
      j.write_fieldname("sib19-v1700");
      c.get<sib19_r17_s>().to_json(j);
      break;
    case types::sib20_v1700:
      j.write_fieldname("sib20-v1700");
      c.get<sib20_r17_s>().to_json(j);
      break;
    case types::sib21_v1700:
      j.write_fieldname("sib21-v1700");
      c.get<sib21_r17_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_ies_s::sib_type_and_info_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sib2:
      HANDLE_CODE(c.get<sib2_s>().pack(bref));
      break;
    case types::sib3:
      HANDLE_CODE(c.get<sib3_s>().pack(bref));
      break;
    case types::sib4:
      HANDLE_CODE(c.get<sib4_s>().pack(bref));
      break;
    case types::sib5:
      HANDLE_CODE(c.get<sib5_s>().pack(bref));
      break;
    case types::sib6:
      HANDLE_CODE(c.get<sib6_s>().pack(bref));
      break;
    case types::sib7:
      HANDLE_CODE(c.get<sib7_s>().pack(bref));
      break;
    case types::sib8:
      HANDLE_CODE(c.get<sib8_s>().pack(bref));
      break;
    case types::sib9:
      HANDLE_CODE(c.get<sib9_s>().pack(bref));
      break;
    case types::sib10_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib10_r16_s>().pack(bref));
    } break;
    case types::sib11_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib11_r16_s>().pack(bref));
    } break;
    case types::sib12_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib12_r16_s>().pack(bref));
    } break;
    case types::sib13_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib13_r16_s>().pack(bref));
    } break;
    case types::sib14_v1610: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib14_r16_s>().pack(bref));
    } break;
    case types::sib15_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib15_r17_s>().pack(bref));
    } break;
    case types::sib16_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib16_r17_s>().pack(bref));
    } break;
    case types::sib17_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib17_r17_s>().pack(bref));
    } break;
    case types::sib18_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib18_r17_s>().pack(bref));
    } break;
    case types::sib19_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib19_r17_s>().pack(bref));
    } break;
    case types::sib20_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib20_r17_s>().pack(bref));
    } break;
    case types::sib21_v1700: {
      varlength_field_pack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib21_r17_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_ies_s::sib_type_and_info_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sib2:
      HANDLE_CODE(c.get<sib2_s>().unpack(bref));
      break;
    case types::sib3:
      HANDLE_CODE(c.get<sib3_s>().unpack(bref));
      break;
    case types::sib4:
      HANDLE_CODE(c.get<sib4_s>().unpack(bref));
      break;
    case types::sib5:
      HANDLE_CODE(c.get<sib5_s>().unpack(bref));
      break;
    case types::sib6:
      HANDLE_CODE(c.get<sib6_s>().unpack(bref));
      break;
    case types::sib7:
      HANDLE_CODE(c.get<sib7_s>().unpack(bref));
      break;
    case types::sib8:
      HANDLE_CODE(c.get<sib8_s>().unpack(bref));
      break;
    case types::sib9:
      HANDLE_CODE(c.get<sib9_s>().unpack(bref));
      break;
    case types::sib10_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib10_r16_s>().unpack(bref));
    } break;
    case types::sib11_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib11_r16_s>().unpack(bref));
    } break;
    case types::sib12_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib12_r16_s>().unpack(bref));
    } break;
    case types::sib13_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib13_r16_s>().unpack(bref));
    } break;
    case types::sib14_v1610: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib14_r16_s>().unpack(bref));
    } break;
    case types::sib15_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib15_r17_s>().unpack(bref));
    } break;
    case types::sib16_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib16_r17_s>().unpack(bref));
    } break;
    case types::sib17_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib17_r17_s>().unpack(bref));
    } break;
    case types::sib18_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib18_r17_s>().unpack(bref));
    } break;
    case types::sib19_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib19_r17_s>().unpack(bref));
    } break;
    case types::sib20_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib20_r17_s>().unpack(bref));
    } break;
    case types::sib21_v1700: {
      varlength_field_unpack_guard varlen_scope(bref, false);
      HANDLE_CODE(c.get<sib21_r17_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "sys_info_ies_s::sib_type_and_info_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_info_ies_s::sib_type_and_info_item_c_::types_opts::to_string() const
{
  static const char* names[] = {"sib2",        "sib3",        "sib4",        "sib5",        "sib6",
                                "sib7",        "sib8",        "sib9",        "sib10-v1610", "sib11-v1610",
                                "sib12-v1610", "sib13-v1610", "sib14-v1610", "sib15-v1700", "sib16-v1700",
                                "sib17-v1700", "sib18-v1700", "sib19-v1700", "sib20-v1700", "sib21-v1700"};
  return convert_enum_idx(names, 20, value, "sys_info_ies_s::sib_type_and_info_item_c_::types");
}
uint8_t sys_info_ies_s::sib_type_and_info_item_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
  return map_enum_number(numbers, 20, value, "sys_info_ies_s::sib_type_and_info_item_c_::types");
}

// UAC-AccessCategory1-SelectionAssistanceInfo ::= ENUMERATED
const char* uac_access_category1_sel_assist_info_opts::to_string() const
{
  static const char* names[] = {"a", "b", "c"};
  return convert_enum_idx(names, 3, value, "uac_access_category1_sel_assist_info_e");
}

// UE-TimersAndConstants ::= SEQUENCE
SRSASN_CODE ue_timers_and_consts_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(t300.pack(bref));
  HANDLE_CODE(t301.pack(bref));
  HANDLE_CODE(t310.pack(bref));
  HANDLE_CODE(n310.pack(bref));
  HANDLE_CODE(t311.pack(bref));
  HANDLE_CODE(n311.pack(bref));
  HANDLE_CODE(t319.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_timers_and_consts_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(t300.unpack(bref));
  HANDLE_CODE(t301.unpack(bref));
  HANDLE_CODE(t310.unpack(bref));
  HANDLE_CODE(n310.unpack(bref));
  HANDLE_CODE(t311.unpack(bref));
  HANDLE_CODE(n311.unpack(bref));
  HANDLE_CODE(t319.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_timers_and_consts_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("t300", t300.to_string());
  j.write_str("t301", t301.to_string());
  j.write_str("t310", t310.to_string());
  j.write_str("n310", n310.to_string());
  j.write_str("t311", t311.to_string());
  j.write_str("n311", n311.to_string());
  j.write_str("t319", t319.to_string());
  j.end_obj();
}

const char* ue_timers_and_consts_s::t300_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::t300_e_");
}
uint16_t ue_timers_and_consts_s::t300_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t300_e_");
}

const char* ue_timers_and_consts_s::t301_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::t301_e_");
}
uint16_t ue_timers_and_consts_s::t301_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t301_e_");
}

const char* ue_timers_and_consts_s::t310_opts::to_string() const
{
  static const char* names[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(names, 7, value, "ue_timers_and_consts_s::t310_e_");
}
uint16_t ue_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(numbers, 7, value, "ue_timers_and_consts_s::t310_e_");
}

const char* ue_timers_and_consts_s::n310_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::n310_e_");
}
uint8_t ue_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::n310_e_");
}

const char* ue_timers_and_consts_s::t311_opts::to_string() const
{
  static const char* names[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(names, 7, value, "ue_timers_and_consts_s::t311_e_");
}
uint16_t ue_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t numbers[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(numbers, 7, value, "ue_timers_and_consts_s::t311_e_");
}

const char* ue_timers_and_consts_s::n311_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::n311_e_");
}
uint8_t ue_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::n311_e_");
}

const char* ue_timers_and_consts_s::t319_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_s::t319_e_");
}
uint16_t ue_timers_and_consts_s::t319_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t319_e_");
}

// SIB1 ::= SEQUENCE
SRSASN_CODE sib1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cell_sel_info_present, 1));
  HANDLE_CODE(bref.pack(conn_est_fail_ctrl_present, 1));
  HANDLE_CODE(bref.pack(si_sched_info_present, 1));
  HANDLE_CODE(bref.pack(serving_cell_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(ims_emergency_support_present, 1));
  HANDLE_CODE(bref.pack(ecall_over_ims_support_present, 1));
  HANDLE_CODE(bref.pack(ue_timers_and_consts_present, 1));
  HANDLE_CODE(bref.pack(uac_barr_info_present, 1));
  HANDLE_CODE(bref.pack(use_full_resume_id_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cell_sel_info_present) {
    HANDLE_CODE(bref.pack(cell_sel_info.q_rx_lev_min_offset_present, 1));
    HANDLE_CODE(bref.pack(cell_sel_info.q_rx_lev_min_sul_present, 1));
    HANDLE_CODE(bref.pack(cell_sel_info.q_qual_min_present, 1));
    HANDLE_CODE(bref.pack(cell_sel_info.q_qual_min_offset_present, 1));
    HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min, (int8_t)-70, (int8_t)-22));
    if (cell_sel_info.q_rx_lev_min_offset_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min_offset, (uint8_t)1u, (uint8_t)8u));
    }
    if (cell_sel_info.q_rx_lev_min_sul_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_rx_lev_min_sul, (int8_t)-70, (int8_t)-22));
    }
    if (cell_sel_info.q_qual_min_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_qual_min, (int8_t)-43, (int8_t)-12));
    }
    if (cell_sel_info.q_qual_min_offset_present) {
      HANDLE_CODE(pack_integer(bref, cell_sel_info.q_qual_min_offset, (uint8_t)1u, (uint8_t)8u));
    }
  }
  HANDLE_CODE(cell_access_related_info.pack(bref));
  if (conn_est_fail_ctrl_present) {
    HANDLE_CODE(conn_est_fail_ctrl.pack(bref));
  }
  if (si_sched_info_present) {
    HANDLE_CODE(si_sched_info.pack(bref));
  }
  if (serving_cell_cfg_common_present) {
    HANDLE_CODE(serving_cell_cfg_common.pack(bref));
  }
  if (ue_timers_and_consts_present) {
    HANDLE_CODE(ue_timers_and_consts.pack(bref));
  }
  if (uac_barr_info_present) {
    HANDLE_CODE(bref.pack(uac_barr_info.uac_barr_for_common.size() > 0, 1));
    HANDLE_CODE(bref.pack(uac_barr_info.uac_barr_per_plmn_list.size() > 0, 1));
    HANDLE_CODE(bref.pack(uac_barr_info.uac_access_category1_sel_assist_info_present, 1));
    if (uac_barr_info.uac_barr_for_common.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info.uac_barr_for_common, 1, 63));
    }
    if (uac_barr_info.uac_barr_per_plmn_list.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info.uac_barr_per_plmn_list, 1, 12));
    }
    HANDLE_CODE(pack_dyn_seq_of(bref, uac_barr_info.uac_barr_info_set_list, 1, 8));
    if (uac_barr_info.uac_access_category1_sel_assist_info_present) {
      HANDLE_CODE(uac_barr_info.uac_access_category1_sel_assist_info.pack(bref));
    }
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cell_sel_info_present, 1));
  HANDLE_CODE(bref.unpack(conn_est_fail_ctrl_present, 1));
  HANDLE_CODE(bref.unpack(si_sched_info_present, 1));
  HANDLE_CODE(bref.unpack(serving_cell_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(ims_emergency_support_present, 1));
  HANDLE_CODE(bref.unpack(ecall_over_ims_support_present, 1));
  HANDLE_CODE(bref.unpack(ue_timers_and_consts_present, 1));
  HANDLE_CODE(bref.unpack(uac_barr_info_present, 1));
  HANDLE_CODE(bref.unpack(use_full_resume_id_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cell_sel_info_present) {
    HANDLE_CODE(bref.unpack(cell_sel_info.q_rx_lev_min_offset_present, 1));
    HANDLE_CODE(bref.unpack(cell_sel_info.q_rx_lev_min_sul_present, 1));
    HANDLE_CODE(bref.unpack(cell_sel_info.q_qual_min_present, 1));
    HANDLE_CODE(bref.unpack(cell_sel_info.q_qual_min_offset_present, 1));
    HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min, bref, (int8_t)-70, (int8_t)-22));
    if (cell_sel_info.q_rx_lev_min_offset_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min_offset, bref, (uint8_t)1u, (uint8_t)8u));
    }
    if (cell_sel_info.q_rx_lev_min_sul_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_rx_lev_min_sul, bref, (int8_t)-70, (int8_t)-22));
    }
    if (cell_sel_info.q_qual_min_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_qual_min, bref, (int8_t)-43, (int8_t)-12));
    }
    if (cell_sel_info.q_qual_min_offset_present) {
      HANDLE_CODE(unpack_integer(cell_sel_info.q_qual_min_offset, bref, (uint8_t)1u, (uint8_t)8u));
    }
  }
  HANDLE_CODE(cell_access_related_info.unpack(bref));
  if (conn_est_fail_ctrl_present) {
    HANDLE_CODE(conn_est_fail_ctrl.unpack(bref));
  }
  if (si_sched_info_present) {
    HANDLE_CODE(si_sched_info.unpack(bref));
  }
  if (serving_cell_cfg_common_present) {
    HANDLE_CODE(serving_cell_cfg_common.unpack(bref));
  }
  if (ue_timers_and_consts_present) {
    HANDLE_CODE(ue_timers_and_consts.unpack(bref));
  }
  if (uac_barr_info_present) {
    bool uac_barr_for_common_present;
    HANDLE_CODE(bref.unpack(uac_barr_for_common_present, 1));
    bool uac_barr_per_plmn_list_present;
    HANDLE_CODE(bref.unpack(uac_barr_per_plmn_list_present, 1));
    HANDLE_CODE(bref.unpack(uac_barr_info.uac_access_category1_sel_assist_info_present, 1));
    if (uac_barr_for_common_present) {
      HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info.uac_barr_for_common, bref, 1, 63));
    }
    if (uac_barr_per_plmn_list_present) {
      HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info.uac_barr_per_plmn_list, bref, 1, 12));
    }
    HANDLE_CODE(unpack_dyn_seq_of(uac_barr_info.uac_barr_info_set_list, bref, 1, 8));
    if (uac_barr_info.uac_access_category1_sel_assist_info_present) {
      HANDLE_CODE(uac_barr_info.uac_access_category1_sel_assist_info.unpack(bref));
    }
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib1_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cell_sel_info_present) {
    j.write_fieldname("cellSelectionInfo");
    j.start_obj();
    j.write_int("q-RxLevMin", cell_sel_info.q_rx_lev_min);
    if (cell_sel_info.q_rx_lev_min_offset_present) {
      j.write_int("q-RxLevMinOffset", cell_sel_info.q_rx_lev_min_offset);
    }
    if (cell_sel_info.q_rx_lev_min_sul_present) {
      j.write_int("q-RxLevMinSUL", cell_sel_info.q_rx_lev_min_sul);
    }
    if (cell_sel_info.q_qual_min_present) {
      j.write_int("q-QualMin", cell_sel_info.q_qual_min);
    }
    if (cell_sel_info.q_qual_min_offset_present) {
      j.write_int("q-QualMinOffset", cell_sel_info.q_qual_min_offset);
    }
    j.end_obj();
  }
  j.write_fieldname("cellAccessRelatedInfo");
  cell_access_related_info.to_json(j);
  if (conn_est_fail_ctrl_present) {
    j.write_fieldname("connEstFailureControl");
    conn_est_fail_ctrl.to_json(j);
  }
  if (si_sched_info_present) {
    j.write_fieldname("si-SchedulingInfo");
    si_sched_info.to_json(j);
  }
  if (serving_cell_cfg_common_present) {
    j.write_fieldname("servingCellConfigCommon");
    serving_cell_cfg_common.to_json(j);
  }
  if (ims_emergency_support_present) {
    j.write_str("ims-EmergencySupport", "true");
  }
  if (ecall_over_ims_support_present) {
    j.write_str("eCallOverIMS-Support", "true");
  }
  if (ue_timers_and_consts_present) {
    j.write_fieldname("ue-TimersAndConstants");
    ue_timers_and_consts.to_json(j);
  }
  if (uac_barr_info_present) {
    j.write_fieldname("uac-BarringInfo");
    j.start_obj();
    if (uac_barr_info.uac_barr_for_common.size() > 0) {
      j.start_array("uac-BarringForCommon");
      for (const auto& e1 : uac_barr_info.uac_barr_for_common) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (uac_barr_info.uac_barr_per_plmn_list.size() > 0) {
      j.start_array("uac-BarringPerPLMN-List");
      for (const auto& e1 : uac_barr_info.uac_barr_per_plmn_list) {
        e1.to_json(j);
      }
      j.end_array();
    }
    j.start_array("uac-BarringInfoSetList");
    for (const auto& e1 : uac_barr_info.uac_barr_info_set_list) {
      e1.to_json(j);
    }
    j.end_array();
    if (uac_barr_info.uac_access_category1_sel_assist_info_present) {
      j.write_fieldname("uac-AccessCategory1-SelectionAssistanceInfo");
      uac_barr_info.uac_access_category1_sel_assist_info.to_json(j);
    }
    j.end_obj();
  }
  if (use_full_resume_id_present) {
    j.write_str("useFullResumeID", "true");
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

void sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::destroy_()
{
  switch (type_) {
    case types::individual_plmn_list:
      c.destroy<individual_plmn_list_l_>();
      break;
    default:
      break;
  }
}
void sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_common:
      break;
    case types::individual_plmn_list:
      c.init<individual_plmn_list_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }
}
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::uac_access_category1_sel_assist_info_c_(
    const sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_common:
      c.init(other.c.get<uac_access_category1_sel_assist_info_e>());
      break;
    case types::individual_plmn_list:
      c.init(other.c.get<individual_plmn_list_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }
}
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::operator=(
    const sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_common:
      c.set(other.c.get<uac_access_category1_sel_assist_info_e>());
      break;
    case types::individual_plmn_list:
      c.set(other.c.get<individual_plmn_list_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }

  return *this;
}
uac_access_category1_sel_assist_info_e&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set_plmn_common()
{
  set(types::plmn_common);
  return c.get<uac_access_category1_sel_assist_info_e>();
}
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::individual_plmn_list_l_&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set_individual_plmn_list()
{
  set(types::individual_plmn_list);
  return c.get<individual_plmn_list_l_>();
}
void sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_common:
      j.write_str("plmnCommon", c.get<uac_access_category1_sel_assist_info_e>().to_string());
      break;
    case types::individual_plmn_list:
      j.start_array("individualPLMNList");
      for (const auto& e1 : c.get<individual_plmn_list_l_>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
  }
  j.end_obj();
}
SRSASN_CODE sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_common:
      HANDLE_CODE(c.get<uac_access_category1_sel_assist_info_e>().pack(bref));
      break;
    case types::individual_plmn_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<individual_plmn_list_l_>(), 2, 12));
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_common:
      HANDLE_CODE(c.get<uac_access_category1_sel_assist_info_e>().unpack(bref));
      break;
    case types::individual_plmn_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<individual_plmn_list_l_>(), bref, 2, 12));
      break;
    default:
      log_invalid_choice_id(type_, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types_opts::to_string() const
{
  static const char* names[] = {"plmnCommon", "individualPLMNList"};
  return convert_enum_idx(names, 2, value, "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types");
}

// SystemInformation ::= SEQUENCE
SRSASN_CODE sys_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void sys_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void sys_info_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::sys_info:
      c.destroy<sys_info_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.destroy<crit_exts_future_r16_c_>();
      break;
    default:
      break;
  }
}
void sys_info_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info:
      c.init<sys_info_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.init<crit_exts_future_r16_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
}
sys_info_s::crit_exts_c_::crit_exts_c_(const sys_info_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info:
      c.init(other.c.get<sys_info_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.init(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
}
sys_info_s::crit_exts_c_& sys_info_s::crit_exts_c_::operator=(const sys_info_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info:
      c.set(other.c.get<sys_info_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.set(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }

  return *this;
}
sys_info_ies_s& sys_info_s::crit_exts_c_::set_sys_info()
{
  set(types::sys_info);
  return c.get<sys_info_ies_s>();
}
sys_info_s::crit_exts_c_::crit_exts_future_r16_c_& sys_info_s::crit_exts_c_::set_crit_exts_future_r16()
{
  set(types::crit_exts_future_r16);
  return c.get<crit_exts_future_r16_c_>();
}
void sys_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info:
      j.write_fieldname("systemInformation");
      c.get<sys_info_ies_s>().to_json(j);
      break;
    case types::crit_exts_future_r16:
      j.write_fieldname("criticalExtensionsFuture-r16");
      c.get<crit_exts_future_r16_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_ies_s>().pack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set(types::options e)
{
  type_ = e;
}
pos_sys_info_r16_ies_s& sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set_pos_sys_info_r16()
{
  set(types::pos_sys_info_r16);
  return c;
}
void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pos_sys_info_r16:
      j.write_fieldname("posSystemInformation-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pos_sys_info_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pos_sys_info_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"posSystemInformation-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types");
}

const char* sys_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"systemInformation", "criticalExtensionsFuture-r16"};
  return convert_enum_idx(names, 2, value, "sys_info_s::crit_exts_c_::types");
}

// BCCH-DL-SCH-MessageType ::= CHOICE
void bcch_dl_sch_msg_type_c::set(types::options e)
{
  type_ = e;
}
bcch_dl_sch_msg_type_c::c1_c_& bcch_dl_sch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void bcch_dl_sch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void bcch_dl_sch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void bcch_dl_sch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::sys_info:
      c.destroy<sys_info_s>();
      break;
    case types::sib_type1:
      c.destroy<sib1_s>();
      break;
    default:
      break;
  }
}
void bcch_dl_sch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info:
      c.init<sys_info_s>();
      break;
    case types::sib_type1:
      c.init<sib1_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_c::c1_c_::c1_c_(const bcch_dl_sch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info:
      c.init(other.c.get<sys_info_s>());
      break;
    case types::sib_type1:
      c.init(other.c.get<sib1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_c::c1_c_& bcch_dl_sch_msg_type_c::c1_c_::operator=(const bcch_dl_sch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info:
      c.set(other.c.get<sys_info_s>());
      break;
    case types::sib_type1:
      c.set(other.c.get<sib1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }

  return *this;
}
sys_info_s& bcch_dl_sch_msg_type_c::c1_c_::set_sys_info()
{
  set(types::sys_info);
  return c.get<sys_info_s>();
}
sib1_s& bcch_dl_sch_msg_type_c::c1_c_::set_sib_type1()
{
  set(types::sib_type1);
  return c.get<sib1_s>();
}
void bcch_dl_sch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info:
      j.write_fieldname("systemInformation");
      c.get<sys_info_s>().to_json(j);
      break;
    case types::sib_type1:
      j.write_fieldname("systemInformationBlockType1");
      c.get<sib1_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_s>().pack(bref));
      break;
    case types::sib_type1:
      HANDLE_CODE(c.get<sib1_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info:
      HANDLE_CODE(c.get<sys_info_s>().unpack(bref));
      break;
    case types::sib_type1:
      HANDLE_CODE(c.get<sib1_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"systemInformation", "systemInformationBlockType1"};
  return convert_enum_idx(names, 2, value, "bcch_dl_sch_msg_type_c::c1_c_::types");
}
uint8_t bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_number() const
{
  if (value == sib_type1) {
    return 1;
  }
  invalid_enum_number(value, "bcch_dl_sch_msg_type_c::c1_c_::types");
  return 0;
}

const char* bcch_dl_sch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "bcch_dl_sch_msg_type_c::types");
}
uint8_t bcch_dl_sch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "bcch_dl_sch_msg_type_c::types");
}

// BCCH-DL-SCH-Message ::= SEQUENCE
SRSASN_CODE bcch_dl_sch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_dl_sch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-DL-SCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* sl_res_reserve_period_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-ResourceReservePeriod1-r16", "sl-ResourceReservePeriod2-r16"};
  return convert_enum_idx(names, 2, value, "sl_res_reserve_period_r16_c::types");
}
uint8_t sl_res_reserve_period_r16_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_res_reserve_period_r16_c::types");
}

const char* sl_meas_report_quant_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-RSRP-r16"};
  return convert_enum_idx(names, 1, value, "sl_meas_report_quant_r16_c::types");
}

const char* sl_meas_trigger_quant_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-RSRP-r16"};
  return convert_enum_idx(names, 1, value, "sl_meas_trigger_quant_r16_c::types");
}

const char* sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"eventS1-r16", "eventS2-r16"};
  return convert_enum_idx(names, 2, value, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types");
}
uint8_t sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_event_trigger_cfg_r16_s::sl_event_id_r16_c_::types");
}

const char* sl_pqi_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-StandardizedPQI-r16", "sl-Non-StandardizedPQI-r16"};
  return convert_enum_idx(names, 2, value, "sl_pqi_r16_c::types");
}

const char* sl_tx_pwr_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"minusinfinity-r16", "txPower-r16"};
  return convert_enum_idx(names, 2, value, "sl_tx_pwr_r16_c::types");
}
int8_t sl_tx_pwr_r16_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-1};
  return map_enum_number(numbers, 1, value, "sl_tx_pwr_r16_c::types");
}

const char* sl_period_cg_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-PeriodCG1-r16", "sl-PeriodCG2-r16"};
  return convert_enum_idx(names, 2, value, "sl_period_cg_r16_c::types");
}
uint8_t sl_period_cg_r16_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sl_period_cg_r16_c::types");
}

const char* sl_report_cfg_r16_s::sl_report_type_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"sl-Periodical-r16", "sl-EventTriggered-r16"};
  return convert_enum_idx(names, 2, value, "sl_report_cfg_r16_s::sl_report_type_r16_c_::types");
}

const char* sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "sl_drx_cfg_uc_r17_s::sl_drx_on_dur_timer_r17_c_::types");
}

const char* sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types");
}
uint16_t sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "sl_drx_cfg_uc_r17_s::sl_drx_cycle_start_offset_r17_c_::types");
}

const char* sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "sl_drx_gc_bc_qos_r17_s::sl_drx_gc_bc_on_dur_timer_r17_c_::types");
}

const char* sl_rlc_cfg_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-AM-RLC-r16", "sl-UM-RLC-r16"};
  return convert_enum_idx(names, 2, value, "sl_rlc_cfg_r16_c::types");
}

const char* sl_remote_ue_rb_id_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"srb-Identity-r17", "drb-Identity-r17"};
  return convert_enum_idx(names, 2, value, "sl_remote_ue_rb_id_r17_c::types");
}

const char* ul_data_compress_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"newSetup", "drb-ContinueUDC"};
  return convert_enum_idx(names, 2, value, "ul_data_compress_r17_c::types");
}

const char* pdcp_cfg_s::drb_s_::hdr_compress_c_::types_opts::to_string() const
{
  static const char* names[] = {"notUsed", "rohc", "uplinkOnlyROHC"};
  return convert_enum_idx(names, 3, value, "pdcp_cfg_s::drb_s_::hdr_compress_c_::types");
}

const char* sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"sl-MappedQoS-FlowsList-r16", "sl-MappedQoS-FlowsListDedicated-r16"};
  return convert_enum_idx(names, 2, value, "sl_sdap_cfg_r16_s::sl_mapped_qos_flows_r16_c_::types");
}

const char* tmgi_r17_s::plmn_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"plmn-Index", "explicitValue"};
  return convert_enum_idx(names, 2, value, "tmgi_r17_s::plmn_id_r17_c_::types");
}

const char* drb_to_add_mod_s::cn_assoc_c_::types_opts::to_string() const
{
  static const char* names[] = {"eps-BearerIdentity", "sdap-Config"};
  return convert_enum_idx(names, 2, value, "drb_to_add_mod_s::cn_assoc_c_::types");
}

const char* rrc_reject_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReject", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_reject_s::crit_exts_c_::types");
}

const char* rrc_setup_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcSetup", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_setup_s::crit_exts_c_::types");
}

const char* dl_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReject", "rrcSetup", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "dl_ccch_msg_type_c::c1_c_::types");
}

const char* dl_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "dl_ccch_msg_type_c::types");
}
uint8_t dl_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "dl_ccch_msg_type_c::types");
}

const char* srs_periodicity_and_offset_c::types_opts::to_string() const
{
  static const char* names[] = {"sl1",
                                "sl2",
                                "sl4",
                                "sl5",
                                "sl8",
                                "sl10",
                                "sl16",
                                "sl20",
                                "sl32",
                                "sl40",
                                "sl64",
                                "sl80",
                                "sl160",
                                "sl320",
                                "sl640",
                                "sl1280",
                                "sl2560"};
  return convert_enum_idx(names, 17, value, "srs_periodicity_and_offset_c::types");
}
uint16_t srs_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560};
  return map_enum_number(numbers, 17, value, "srs_periodicity_and_offset_c::types");
}

const char* srs_spatial_relation_info_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index", "srs"};
  return convert_enum_idx(names, 3, value, "srs_spatial_relation_info_s::ref_sig_c_::types");
}

const char* spatial_relation_info_pdc_r17_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index", "dl-PRS-PDC", "srs"};
  return convert_enum_idx(names, 4, value, "spatial_relation_info_pdc_r17_s::ref_sig_c_::types");
}

const char* csi_rs_res_mob_s::slot_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms4", "ms5", "ms10", "ms20", "ms40"};
  return convert_enum_idx(names, 5, value, "csi_rs_res_mob_s::slot_cfg_c_::types");
}
uint8_t csi_rs_res_mob_s::slot_cfg_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 5, 10, 20, 40};
  return map_enum_number(numbers, 5, value, "csi_rs_res_mob_s::slot_cfg_c_::types");
}

const char* csi_rs_res_mob_s::freq_domain_alloc_c_::types_opts::to_string() const
{
  static const char* names[] = {"row1", "row2"};
  return convert_enum_idx(names, 2, value, "csi_rs_res_mob_s::freq_domain_alloc_c_::types");
}
uint8_t csi_rs_res_mob_s::freq_domain_alloc_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_rs_res_mob_s::freq_domain_alloc_c_::types");
}

const char* csi_rs_res_mob_s::slot_cfg_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms4", "ms5", "ms10", "ms20", "ms40"};
  return convert_enum_idx(names, 5, value, "csi_rs_res_mob_s::slot_cfg_r17_c_::types");
}
uint8_t csi_rs_res_mob_s::slot_cfg_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 5, 10, 20, 40};
  return map_enum_number(numbers, 5, value, "csi_rs_res_mob_s::slot_cfg_r17_c_::types");
}

const char* mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms20-r17",
                                "ms40-r17",
                                "ms80-r17",
                                "ms160-r17",
                                "ms320-r17",
                                "ms640-r17",
                                "ms1280-r17",
                                "ms2560-r17",
                                "ms5120-r17"};
  return convert_enum_idx(names, 9, value, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types");
}
uint16_t mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(numbers, 9, value, "mu_si_m_gap_info_r17_s::musim_gap_repeat_and_offset_r17_c_::types");
}

const char* rssi_periodicity_and_offset_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl10", "sl20", "sl40", "sl80", "sl160", "sl320", "s1640"};
  return convert_enum_idx(names, 7, value, "rssi_periodicity_and_offset_r16_c::types");
}
uint16_t rssi_periodicity_and_offset_r16_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 40, 80, 160, 320, 1640};
  return map_enum_number(numbers, 7, value, "rssi_periodicity_and_offset_r16_c::types");
}

const char* srs_res_s::tx_comb_c_::types_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(names, 2, value, "srs_res_s::tx_comb_c_::types");
}
uint8_t srs_res_s::tx_comb_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "srs_res_s::tx_comb_c_::types");
}

const char* srs_res_s::res_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"aperiodic", "semi-persistent", "periodic"};
  return convert_enum_idx(names, 3, value, "srs_res_s::res_type_c_::types");
}

const char* srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"startRBIndexAndFreqScalingFactor2-r17", "startRBIndexAndFreqScalingFactor4-r17"};
  return convert_enum_idx(names, 2, value, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types");
}
uint8_t srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types");
}

const char* srs_res_s::srs_tci_state_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"srs-UL-TCI-State", "srs-DLorJointTCI-State"};
  return convert_enum_idx(names, 2, value, "srs_res_s::srs_tci_state_r17_c_::types");
}

const char* iab_ip_address_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"iPv4-Address-r16", "iPv6-Address-r16", "iPv6-Prefix-r16"};
  return convert_enum_idx(names, 3, value, "iab_ip_address_r16_c::types");
}

const char* meas_trigger_quant_c::types_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(names, 3, value, "meas_trigger_quant_c::types");
}

const char* meas_trigger_quant_cli_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"srs-RSRP-r16", "cli-RSSI-r16"};
  return convert_enum_idx(names, 2, value, "meas_trigger_quant_cli_r16_c::types");
}

const char* meas_trigger_quant_eutra_c::types_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(names, 3, value, "meas_trigger_quant_eutra_c::types");
}

const char* meas_trigger_quant_offset_c::types_opts::to_string() const
{
  static const char* names[] = {"rsrp", "rsrq", "sinr"};
  return convert_enum_idx(names, 3, value, "meas_trigger_quant_offset_c::types");
}

const char* meas_trigger_quant_utra_fdd_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"utra-FDD-RSCP-r16", "utra-FDD-EcN0-r16"};
  return convert_enum_idx(names, 2, value, "meas_trigger_quant_utra_fdd_r16_c::types");
}
uint8_t meas_trigger_quant_utra_fdd_r16_c::types_opts::to_number() const
{
  if (value == utra_fdd_ec_n0_r16) {
    return 0;
  }
  invalid_enum_number(value, "meas_trigger_quant_utra_fdd_r16_c::types");
  return 0;
}

const char* ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "sf5-r16", "sf10-r16", "sf20-r16", "sf40-r16", "sf80-r16", "sf160-r16", "sf320-r16", "sf640-r16", "sf1280-r16"};
  return convert_enum_idx(names, 9, value, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types");
}
uint16_t ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 40, 80, 160, 320, 640, 1280};
  return map_enum_number(numbers, 9, value, "ssb_mtc3_r16_s::periodicity_and_offset_r16_c_::types");
}

const char* ue_tx_teg_request_ul_tdoa_cfg_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"oneShot-r17", "periodicReporting-r17"};
  return convert_enum_idx(names, 2, value, "ue_tx_teg_request_ul_tdoa_cfg_r17_c::types");
}
uint8_t ue_tx_teg_request_ul_tdoa_cfg_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_tx_teg_request_ul_tdoa_cfg_r17_c::types");
}

const char* cli_event_trigger_cfg_r16_s::event_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"eventI1-r16"};
  return convert_enum_idx(names, 1, value, "cli_event_trigger_cfg_r16_s::event_id_r16_c_::types");
}
uint8_t cli_event_trigger_cfg_r16_s::event_id_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cli_event_trigger_cfg_r16_s::event_id_r16_c_::types");
}

const char* cond_trigger_cfg_r16_s::cond_event_id_c_::types_opts::to_string() const
{
  static const char* names[] = {"condEventA3", "condEventA5", "condEventA4-r17", "condEventD1-r17", "condEventT1-r17"};
  return convert_enum_idx(names, 5, value, "cond_trigger_cfg_r16_s::cond_event_id_c_::types");
}

const char* event_trigger_cfg_s::event_id_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "eventA1", "eventA2", "eventA3", "eventA4", "eventA5", "eventA6", "eventX1-r17", "eventX2-r17", "eventD1-r17"};
  return convert_enum_idx(names, 9, value, "event_trigger_cfg_s::event_id_c_::types");
}

const char* event_trigger_cfg_inter_rat_s::event_id_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "eventB1", "eventB2", "eventB1-UTRA-FDD-r16", "eventB2-UTRA-FDD-r16", "eventY1-Relay-r17", "eventY2-Relay-r17"};
  return convert_enum_idx(names, 6, value, "event_trigger_cfg_inter_rat_s::event_id_c_::types");
}

const char* event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"eventC1", "eventC2-r16"};
  return convert_enum_idx(names, 2, value, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types");
}
uint8_t event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "event_trigger_cfg_nr_sl_r16_s::event_id_r16_c_::types");
}

const char* mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG", "eutra-SCG"};
  return convert_enum_idx(names, 2, value, "mrdc_secondary_cell_group_cfg_s::mrdc_secondary_cell_group_c_::types");
}

const char* meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"prs-Ref-r17", "csi-RS-Ref-r17"};
  return convert_enum_idx(names, 2, value, "meas_obj_rx_tx_diff_r17_s::dl_ref_r17_c_::types");
}

const char* report_cfg_inter_rat_s::report_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical", "eventTriggered", "reportCGI", "reportSFTD"};
  return convert_enum_idx(names, 4, value, "report_cfg_inter_rat_s::report_type_c_::types");
}

const char* report_cfg_nr_s::report_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical",
                                "eventTriggered",
                                "reportCGI",
                                "reportSFTD",
                                "condTriggerConfig-r16",
                                "cli-Periodical-r16",
                                "cli-EventTriggered-r16",
                                "rxTxPeriodical-r17"};
  return convert_enum_idx(names, 8, value, "report_cfg_nr_s::report_type_c_::types");
}

const char* report_cfg_nr_sl_r16_s::report_type_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical-r16", "eventTriggered-r16"};
  return convert_enum_idx(names, 2, value, "report_cfg_nr_sl_r16_s::report_type_r16_c_::types");
}

const char* area_cfg_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalIdList-r16", "trackingAreaCodeList-r16", "trackingAreaIdentityList-r16"};
  return convert_enum_idx(names, 3, value, "area_cfg_r16_c::types");
}

const char* meas_obj_to_add_mod_s::meas_obj_c_::types_opts::to_string() const
{
  static const char* names[] = {"measObjectNR",
                                "measObjectEUTRA",
                                "measObjectUTRA-FDD-r16",
                                "measObjectNR-SL-r16",
                                "measObjectCLI-r16",
                                "measObjectRxTxDiff-r17",
                                "measObjectRelay-r17"};
  return convert_enum_idx(names, 7, value, "meas_obj_to_add_mod_s::meas_obj_c_::types");
}

const char* report_cfg_to_add_mod_s::report_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"reportConfigNR", "reportConfigInterRAT", "reportConfigNR-SL-r16"};
  return convert_enum_idx(names, 3, value, "report_cfg_to_add_mod_s::report_cfg_c_::types");
}

const char* event_type_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"outOfCoverage", "eventL1"};
  return convert_enum_idx(names, 2, value, "event_type_r16_c::types");
}
uint8_t event_type_r16_c::types_opts::to_number() const
{
  if (value == event_l1) {
    return 1;
  }
  invalid_enum_number(value, "event_type_r16_c::types");
  return 0;
}

const char* other_cfg_s::delay_budget_report_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"release", "setup"};
  return convert_enum_idx(names, 2, value, "other_cfg_s::delay_budget_report_cfg_c_::types");
}

const char* ran_notif_area_info_c::types_opts::to_string() const
{
  static const char* names[] = {"cellList", "ran-AreaConfigList"};
  return convert_enum_idx(names, 2, value, "ran_notif_area_info_c::types");
}

const char* rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG-r16", "eutra-SCG-r16"};
  return convert_enum_idx(names, 2, value, "rrc_resume_v1610_ies_s::mrdc_secondary_cell_group_r16_c_::types");
}

const char* meas_cfg_s::s_measure_cfg_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-RSRP", "csi-RSRP"};
  return convert_enum_idx(names, 2, value, "meas_cfg_s::s_measure_cfg_c_::types");
}

const char* redirected_carrier_info_c::types_opts::to_string() const
{
  static const char* names[] = {"nr", "eutra"};
  return convert_enum_idx(names, 2, value, "redirected_carrier_info_c::types");
}

const char* logged_meas_cfg_r16_ies_s::report_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodical", "eventTriggered"};
  return convert_enum_idx(names, 2, value, "logged_meas_cfg_r16_ies_s::report_type_c_::types");
}

const char* counter_check_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"counterCheck", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "counter_check_s::crit_exts_c_::types");
}

const char* dl_ded_msg_segment_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"dlDedicatedMessageSegment-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "dl_ded_msg_segment_r16_s::crit_exts_c_::types");
}

const char* dl_info_transfer_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"dlInformationTransfer", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "dl_info_transfer_s::crit_exts_c_::types");
}

const char* dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"dlInformationTransferMRDC-r16", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::c1_c_::types");
}

const char* dl_info_transfer_mrdc_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::types");
}
uint8_t dl_info_transfer_mrdc_r16_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "dl_info_transfer_mrdc_r16_s::crit_exts_c_::types");
}

const char* logged_meas_cfg_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"loggedMeasurementConfiguration-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "logged_meas_cfg_r16_s::crit_exts_c_::types");
}

const char* mob_from_nr_cmd_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mobilityFromNRCommand", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mob_from_nr_cmd_s::crit_exts_c_::types");
}

const char* rrc_recfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReconfiguration", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_recfg_s::crit_exts_c_::types");
}

const char* rrc_reest_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReestablishment", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_reest_s::crit_exts_c_::types");
}

const char* rrc_release_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcRelease", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_release_s::crit_exts_c_::types");
}

const char* rrc_resume_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcResume", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_resume_s::crit_exts_c_::types");
}

const char* security_mode_cmd_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"securityModeCommand", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "security_mode_cmd_s::crit_exts_c_::types");
}

const char* ue_cap_enquiry_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueCapabilityEnquiry", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_cap_enquiry_s::crit_exts_c_::types");
}

const char* ue_info_request_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueInformationRequest-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_info_request_r16_s::crit_exts_c_::types");
}

const char* dl_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReconfiguration",
                                "rrcResume",
                                "rrcRelease",
                                "rrcReestablishment",
                                "securityModeCommand",
                                "dlInformationTransfer",
                                "ueCapabilityEnquiry",
                                "counterCheck",
                                "mobilityFromNRCommand",
                                "dlDedicatedMessageSegment-r16",
                                "ueInformationRequest-r16",
                                "dlInformationTransferMRDC-r16",
                                "loggedMeasurementConfiguration-r16",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "dl_dcch_msg_type_c::c1_c_::types");
}

const char* dl_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "dl_dcch_msg_type_c::types");
}
uint8_t dl_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "dl_dcch_msg_type_c::types");
}

// MRB-PDCP-ConfigBroadcast-r17 ::= SEQUENCE
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_sn_size_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(t_reordering_r17_present, 1));

  HANDLE_CODE(hdr_compress_r17.pack(bref));
  if (t_reordering_r17_present) {
    HANDLE_CODE(t_reordering_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_sn_size_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(t_reordering_r17_present, 1));

  HANDLE_CODE(hdr_compress_r17.unpack(bref));
  if (t_reordering_r17_present) {
    HANDLE_CODE(t_reordering_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrb_pdcp_cfg_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_sn_size_dl_r17_present) {
    j.write_str("pdcp-SN-SizeDL-r17", "len12bits");
  }
  j.write_fieldname("headerCompression-r17");
  hdr_compress_r17.to_json(j);
  if (t_reordering_r17_present) {
    j.write_str("t-Reordering-r17", t_reordering_r17.to_string());
  }
  j.end_obj();
}

void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set(types::options e)
{
  type_ = e;
}
void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set_not_used()
{
  set(types::not_used);
}
mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_&
mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set_rohc()
{
  set(types::rohc);
  return c;
}
void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      j.write_fieldname("rohc");
      j.start_obj();
      if (c.max_c_id_r17_present) {
        j.write_int("maxCID-r17", c.max_c_id_r17);
      }
      j.write_fieldname("profiles-r17");
      j.start_obj();
      j.write_bool("profile0x0000-r17", c.profiles_r17.profile0x0000_r17);
      j.write_bool("profile0x0001-r17", c.profiles_r17.profile0x0001_r17);
      j.write_bool("profile0x0002-r17", c.profiles_r17.profile0x0002_r17);
      j.end_obj();
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.pack(c.max_c_id_r17_present, 1));
      if (c.max_c_id_r17_present) {
        HANDLE_CODE(pack_integer(bref, c.max_c_id_r17, (uint8_t)1u, (uint8_t)16u));
      }
      HANDLE_CODE(bref.pack(c.profiles_r17.profile0x0000_r17, 1));
      HANDLE_CODE(bref.pack(c.profiles_r17.profile0x0001_r17, 1));
      HANDLE_CODE(bref.pack(c.profiles_r17.profile0x0002_r17, 1));
      break;
    default:
      log_invalid_choice_id(type_, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::not_used:
      break;
    case types::rohc:
      HANDLE_CODE(bref.unpack(c.max_c_id_r17_present, 1));
      if (c.max_c_id_r17_present) {
        HANDLE_CODE(unpack_integer(c.max_c_id_r17, bref, (uint8_t)1u, (uint8_t)16u));
      }
      HANDLE_CODE(bref.unpack(c.profiles_r17.profile0x0000_r17, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r17.profile0x0001_r17, 1));
      HANDLE_CODE(bref.unpack(c.profiles_r17.profile0x0002_r17, 1));
      break;
    default:
      log_invalid_choice_id(type_, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"notUsed", "rohc"};
  return convert_enum_idx(names, 2, value, "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types");
}

const char* mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_opts::to_string() const
{
  static const char* names[] = {"ms1", "ms10", "ms40", "ms160", "ms500", "ms1000", "ms1250", "ms2750"};
  return convert_enum_idx(names, 8, value, "mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_e_");
}
uint16_t mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 10, 40, 160, 500, 1000, 1250, 2750};
  return map_enum_number(numbers, 8, value, "mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_e_");
}

// MRB-RLC-ConfigBroadcast-r17 ::= SEQUENCE
SRSASN_CODE mrb_rlc_cfg_broadcast_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sn_field_len_r17_present, 1));
  HANDLE_CODE(bref.pack(t_reassembly_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, lc_ch_id_r17, (uint8_t)1u, (uint8_t)32u));
  if (t_reassembly_r17_present) {
    HANDLE_CODE(t_reassembly_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_rlc_cfg_broadcast_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sn_field_len_r17_present, 1));
  HANDLE_CODE(bref.unpack(t_reassembly_r17_present, 1));

  HANDLE_CODE(unpack_integer(lc_ch_id_r17, bref, (uint8_t)1u, (uint8_t)32u));
  if (t_reassembly_r17_present) {
    HANDLE_CODE(t_reassembly_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mrb_rlc_cfg_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("logicalChannelIdentity-r17", lc_ch_id_r17);
  if (sn_field_len_r17_present) {
    j.write_str("sn-FieldLength-r17", "size6");
  }
  if (t_reassembly_r17_present) {
    j.write_str("t-Reassembly-r17", t_reassembly_r17.to_string());
  }
  j.end_obj();
}

// MRB-InfoBroadcast-r17 ::= SEQUENCE
SRSASN_CODE mrb_info_broadcast_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pdcp_cfg_r17.pack(bref));
  HANDLE_CODE(rlc_cfg_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrb_info_broadcast_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(pdcp_cfg_r17.unpack(bref));
  HANDLE_CODE(rlc_cfg_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void mrb_info_broadcast_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pdcp-Config-r17");
  pdcp_cfg_r17.to_json(j);
  j.write_fieldname("rlc-Config-r17");
  rlc_cfg_r17.to_json(j);
  j.end_obj();
}

// MBS-NeighbourCell-r17 ::= SEQUENCE
SRSASN_CODE mbs_neighbour_cell_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));
  if (carrier_freq_r17_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_neighbour_cell_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_r17_present, 1));

  HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  if (carrier_freq_r17_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
void mbs_neighbour_cell_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r17", pci_r17);
  if (carrier_freq_r17_present) {
    j.write_int("carrierFreq-r17", carrier_freq_r17);
  }
  j.end_obj();
}

// MBS-SessionInfo-r17 ::= SEQUENCE
SRSASN_CODE mbs_session_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mtch_sched_info_r17_present, 1));
  HANDLE_CODE(bref.pack(mtch_neighbour_cell_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_idx_r17_present, 1));
  HANDLE_CODE(bref.pack(mtch_ssb_map_win_idx_r17_present, 1));

  HANDLE_CODE(mbs_session_id_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, g_rnti_r17, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_dyn_seq_of(bref, mrb_list_broadcast_r17, 1, 4));
  if (mtch_sched_info_r17_present) {
    HANDLE_CODE(pack_integer(bref, mtch_sched_info_r17, (uint8_t)0u, (uint8_t)63u));
  }
  if (mtch_neighbour_cell_r17_present) {
    HANDLE_CODE(mtch_neighbour_cell_r17.pack(bref));
  }
  if (pdsch_cfg_idx_r17_present) {
    HANDLE_CODE(pack_integer(bref, pdsch_cfg_idx_r17, (uint8_t)0u, (uint8_t)15u));
  }
  if (mtch_ssb_map_win_idx_r17_present) {
    HANDLE_CODE(pack_integer(bref, mtch_ssb_map_win_idx_r17, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_session_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mtch_sched_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(mtch_neighbour_cell_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_idx_r17_present, 1));
  HANDLE_CODE(bref.unpack(mtch_ssb_map_win_idx_r17_present, 1));

  HANDLE_CODE(mbs_session_id_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(g_rnti_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_dyn_seq_of(mrb_list_broadcast_r17, bref, 1, 4));
  if (mtch_sched_info_r17_present) {
    HANDLE_CODE(unpack_integer(mtch_sched_info_r17, bref, (uint8_t)0u, (uint8_t)63u));
  }
  if (mtch_neighbour_cell_r17_present) {
    HANDLE_CODE(mtch_neighbour_cell_r17.unpack(bref));
  }
  if (pdsch_cfg_idx_r17_present) {
    HANDLE_CODE(unpack_integer(pdsch_cfg_idx_r17, bref, (uint8_t)0u, (uint8_t)15u));
  }
  if (mtch_ssb_map_win_idx_r17_present) {
    HANDLE_CODE(unpack_integer(mtch_ssb_map_win_idx_r17, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void mbs_session_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbs-SessionId-r17");
  mbs_session_id_r17.to_json(j);
  j.write_int("g-RNTI-r17", g_rnti_r17);
  j.start_array("mrb-ListBroadcast-r17");
  for (const auto& e1 : mrb_list_broadcast_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (mtch_sched_info_r17_present) {
    j.write_int("mtch-SchedulingInfo-r17", mtch_sched_info_r17);
  }
  if (mtch_neighbour_cell_r17_present) {
    j.write_str("mtch-NeighbourCell-r17", mtch_neighbour_cell_r17.to_string());
  }
  if (pdsch_cfg_idx_r17_present) {
    j.write_int("pdsch-ConfigIndex-r17", pdsch_cfg_idx_r17);
  }
  if (mtch_ssb_map_win_idx_r17_present) {
    j.write_int("mtch-SSB-MappingWindowIndex-r17", mtch_ssb_map_win_idx_r17);
  }
  j.end_obj();
}

// MTCH-SSB-MappingWindowCycleOffset-r17 ::= CHOICE
void mtch_ssb_map_win_cycle_offset_r17_c::destroy_() {}
void mtch_ssb_map_win_cycle_offset_r17_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
mtch_ssb_map_win_cycle_offset_r17_c::mtch_ssb_map_win_cycle_offset_r17_c(
    const mtch_ssb_map_win_cycle_offset_r17_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
  }
}
mtch_ssb_map_win_cycle_offset_r17_c&
mtch_ssb_map_win_cycle_offset_r17_c::operator=(const mtch_ssb_map_win_cycle_offset_r17_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
  }

  return *this;
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint16_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
void mtch_ssb_map_win_cycle_offset_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms10:
      j.write_int("ms10", c.get<uint8_t>());
      break;
    case types::ms20:
      j.write_int("ms20", c.get<uint8_t>());
      break;
    case types::ms32:
      j.write_int("ms32", c.get<uint8_t>());
      break;
    case types::ms64:
      j.write_int("ms64", c.get<uint8_t>());
      break;
    case types::ms128:
      j.write_int("ms128", c.get<uint8_t>());
      break;
    case types::ms256:
      j.write_int("ms256", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE mtch_ssb_map_win_cycle_offset_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mtch_ssb_map_win_cycle_offset_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    default:
      log_invalid_choice_id(type_, "mtch_ssb_map_win_cycle_offset_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mtch_ssb_map_win_cycle_offset_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"ms10", "ms20", "ms32", "ms64", "ms128", "ms256"};
  return convert_enum_idx(names, 6, value, "mtch_ssb_map_win_cycle_offset_r17_c::types");
}
uint16_t mtch_ssb_map_win_cycle_offset_r17_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 32, 64, 128, 256};
  return map_enum_number(numbers, 6, value, "mtch_ssb_map_win_cycle_offset_r17_c::types");
}

const char* drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types");
}

const char* drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types");
}
uint16_t drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types");
}

// MBSBroadcastConfiguration-r17-IEs ::= SEQUENCE
SRSASN_CODE mbs_broadcast_cfg_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mbs_session_info_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(mbs_neighbour_cell_list_r17_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_ptm_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_mtch_r17_present, 1));
  HANDLE_CODE(bref.pack(mtch_ssb_map_win_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (mbs_session_info_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_session_info_list_r17, 1, 1024));
  }
  if (mbs_neighbour_cell_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mbs_neighbour_cell_list_r17, 0, 8));
  }
  if (drx_cfg_ptm_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drx_cfg_ptm_list_r17, 1, 64));
  }
  if (pdsch_cfg_mtch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mtch_r17.pack(bref));
  }
  if (mtch_ssb_map_win_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mtch_ssb_map_win_list_r17, 1, 16));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_broadcast_cfg_r17_ies_s::unpack(cbit_ref& bref)
{
  bool mbs_session_info_list_r17_present;
  HANDLE_CODE(bref.unpack(mbs_session_info_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(mbs_neighbour_cell_list_r17_present, 1));
  bool drx_cfg_ptm_list_r17_present;
  HANDLE_CODE(bref.unpack(drx_cfg_ptm_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_mtch_r17_present, 1));
  bool mtch_ssb_map_win_list_r17_present;
  HANDLE_CODE(bref.unpack(mtch_ssb_map_win_list_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (mbs_session_info_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_session_info_list_r17, bref, 1, 1024));
  }
  if (mbs_neighbour_cell_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mbs_neighbour_cell_list_r17, bref, 0, 8));
  }
  if (drx_cfg_ptm_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drx_cfg_ptm_list_r17, bref, 1, 64));
  }
  if (pdsch_cfg_mtch_r17_present) {
    HANDLE_CODE(pdsch_cfg_mtch_r17.unpack(bref));
  }
  if (mtch_ssb_map_win_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mtch_ssb_map_win_list_r17, bref, 1, 16));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbs_broadcast_cfg_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mbs_session_info_list_r17.size() > 0) {
    j.start_array("mbs-SessionInfoList-r17");
    for (const auto& e1 : mbs_session_info_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (mbs_neighbour_cell_list_r17_present) {
    j.start_array("mbs-NeighbourCellList-r17");
    for (const auto& e1 : mbs_neighbour_cell_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drx_cfg_ptm_list_r17.size() > 0) {
    j.start_array("drx-ConfigPTM-List-r17");
    for (const auto& e1 : drx_cfg_ptm_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdsch_cfg_mtch_r17_present) {
    j.write_fieldname("pdsch-ConfigMTCH-r17");
    pdsch_cfg_mtch_r17.to_json(j);
  }
  if (mtch_ssb_map_win_list_r17.size() > 0) {
    j.start_array("mtch-SSB-MappingWindowList-r17");
    for (const auto& e1 : mtch_ssb_map_win_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MBSBroadcastConfiguration-r17 ::= SEQUENCE
SRSASN_CODE mbs_broadcast_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_broadcast_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbs_broadcast_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void mbs_broadcast_cfg_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mbs_broadcast_cfg_r17_ies_s& mbs_broadcast_cfg_r17_s::crit_exts_c_::set_mbs_broadcast_cfg_r17()
{
  set(types::mbs_broadcast_cfg_r17);
  return c;
}
void mbs_broadcast_cfg_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void mbs_broadcast_cfg_r17_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      j.write_fieldname("mbsBroadcastConfiguration-r17");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_broadcast_cfg_r17_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbs_broadcast_cfg_r17_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_broadcast_cfg_r17_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbs_broadcast_cfg_r17_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "mbs_broadcast_cfg_r17_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mbs_broadcast_cfg_r17_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mbsBroadcastConfiguration-r17", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mbs_broadcast_cfg_r17_s::crit_exts_c_::types");
}

// MCCH-MessageType-r17 ::= CHOICE
void mcch_msg_type_r17_c::set(types::options e)
{
  type_ = e;
}
mcch_msg_type_r17_c::c1_c_& mcch_msg_type_r17_c::set_c1()
{
  set(types::c1);
  return c;
}
void mcch_msg_type_r17_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void mcch_msg_type_r17_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c");
  }
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_r17_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_r17_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mcch_msg_type_r17_c::c1_c_::set(types::options e)
{
  type_ = e;
}
mbs_broadcast_cfg_r17_s& mcch_msg_type_r17_c::c1_c_::set_mbs_broadcast_cfg_r17()
{
  set(types::mbs_broadcast_cfg_r17);
  return c;
}
void mcch_msg_type_r17_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void mcch_msg_type_r17_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      j.write_fieldname("mbsBroadcastConfiguration-r17");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_r17_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_r17_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::mbs_broadcast_cfg_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_r17_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mcch_msg_type_r17_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"mbsBroadcastConfiguration-r17", "spare1"};
  return convert_enum_idx(names, 2, value, "mcch_msg_type_r17_c::c1_c_::types");
}

const char* mcch_msg_type_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "mcch_msg_type_r17_c::types");
}
uint8_t mcch_msg_type_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "mcch_msg_type_r17_c::types");
}

// MCCH-Message-r17 ::= SEQUENCE
SRSASN_CODE mcch_msg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void mcch_msg_r17_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("MCCH-Message-r17");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// PagingRecord-v1700 ::= SEQUENCE
SRSASN_CODE paging_record_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_cause_r17_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_record_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(paging_cause_r17_present, 1));

  return SRSASN_SUCCESS;
}
void paging_record_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_cause_r17_present) {
    j.write_str("pagingCause-r17", "voice");
  }
  j.end_obj();
}

// PagingUE-Identity ::= CHOICE
void paging_ue_id_c::destroy_()
{
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.destroy<fixed_bitstring<48>>();
      break;
    case types::full_i_rnti:
      c.destroy<fixed_bitstring<40>>();
      break;
    default:
      break;
  }
}
void paging_ue_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.init<fixed_bitstring<48>>();
      break;
    case types::full_i_rnti:
      c.init<fixed_bitstring<40>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
}
paging_ue_id_c::paging_ue_id_c(const paging_ue_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.init(other.c.get<fixed_bitstring<48>>());
      break;
    case types::full_i_rnti:
      c.init(other.c.get<fixed_bitstring<40>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
}
paging_ue_id_c& paging_ue_id_c::operator=(const paging_ue_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      c.set(other.c.get<fixed_bitstring<48>>());
      break;
    case types::full_i_rnti:
      c.set(other.c.get<fixed_bitstring<40>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }

  return *this;
}
fixed_bitstring<48>& paging_ue_id_c::set_ng_5_g_s_tmsi()
{
  set(types::ng_5_g_s_tmsi);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<40>& paging_ue_id_c::set_full_i_rnti()
{
  set(types::full_i_rnti);
  return c.get<fixed_bitstring<40>>();
}
void paging_ue_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      j.write_str("ng-5G-S-TMSI", c.get<fixed_bitstring<48>>().to_string());
      break;
    case types::full_i_rnti:
      j.write_str("fullI-RNTI", c.get<fixed_bitstring<40>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
  }
  j.end_obj();
}
SRSASN_CODE paging_ue_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().pack(bref));
      break;
    case types::full_i_rnti:
      HANDLE_CODE(c.get<fixed_bitstring<40>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_ue_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_5_g_s_tmsi:
      HANDLE_CODE(c.get<fixed_bitstring<48>>().unpack(bref));
      break;
    case types::full_i_rnti:
      HANDLE_CODE(c.get<fixed_bitstring<40>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "paging_ue_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* paging_ue_id_c::types_opts::to_string() const
{
  static const char* names[] = {"ng-5G-S-TMSI", "fullI-RNTI"};
  return convert_enum_idx(names, 2, value, "paging_ue_id_c::types");
}
int8_t paging_ue_id_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-5};
  return map_enum_number(numbers, 1, value, "paging_ue_id_c::types");
}

// PagingRecord ::= SEQUENCE
SRSASN_CODE paging_record_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(access_type_present, 1));

  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_record_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(access_type_present, 1));

  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void paging_record_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  if (access_type_present) {
    j.write_str("accessType", "non3GPP");
  }
  j.end_obj();
}

// Paging-v1700-IEs ::= SEQUENCE
SRSASN_CODE paging_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_record_list_v1700.size() > 0, 1));
  HANDLE_CODE(bref.pack(paging_group_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (paging_record_list_v1700.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_record_list_v1700, 1, 32));
  }
  if (paging_group_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_group_list_r17, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool paging_record_list_v1700_present;
  HANDLE_CODE(bref.unpack(paging_record_list_v1700_present, 1));
  bool paging_group_list_r17_present;
  HANDLE_CODE(bref.unpack(paging_group_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (paging_record_list_v1700_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_record_list_v1700, bref, 1, 32));
  }
  if (paging_group_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_group_list_r17, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void paging_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_record_list_v1700.size() > 0) {
    j.start_array("pagingRecordList-v1700");
    for (const auto& e1 : paging_record_list_v1700) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (paging_group_list_r17.size() > 0) {
    j.start_array("pagingGroupList-r17");
    for (const auto& e1 : paging_group_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// Paging ::= SEQUENCE
SRSASN_CODE paging_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(paging_record_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (paging_record_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, paging_record_list, 1, 32));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE paging_s::unpack(cbit_ref& bref)
{
  bool paging_record_list_present;
  HANDLE_CODE(bref.unpack(paging_record_list_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (paging_record_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(paging_record_list, bref, 1, 32));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void paging_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (paging_record_list.size() > 0) {
    j.start_array("pagingRecordList");
    for (const auto& e1 : paging_record_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PCCH-MessageType ::= CHOICE
void pcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
pcch_msg_type_c::c1_c_& pcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void pcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void pcch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void pcch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
}
paging_s& pcch_msg_type_c::c1_c_::set_paging()
{
  set(types::paging);
  return c;
}
void pcch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void pcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::paging:
      j.write_fieldname("paging");
      c.to_json(j);
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE pcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::paging:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::paging:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "pcch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"paging", "spare1"};
  return convert_enum_idx(names, 2, value, "pcch_msg_type_c::c1_c_::types");
}

const char* pcch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "pcch_msg_type_c::types");
}
uint8_t pcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "pcch_msg_type_c::types");
}

// PCCH-Message ::= SEQUENCE
SRSASN_CODE pcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void pcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("PCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* init_ue_id_c::types_opts::to_string() const
{
  static const char* names[] = {"ng-5G-S-TMSI-Part1", "randomValue"};
  return convert_enum_idx(names, 2, value, "init_ue_id_c::types");
}
int8_t init_ue_id_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-5};
  return map_enum_number(numbers, 1, value, "init_ue_id_c::types");
}

const char* rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcPosSystemInfoRequest-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types");
}

const char* rrc_sys_info_request_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcSystemInfoRequest", "criticalExtensionsFuture-r16"};
  return convert_enum_idx(names, 2, value, "rrc_sys_info_request_s::crit_exts_c_::types");
}

const char* ul_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "rrcSetupRequest", "rrcResumeRequest", "rrcReestablishmentRequest", "rrcSystemInfoRequest"};
  return convert_enum_idx(names, 4, value, "ul_ccch_msg_type_c::c1_c_::types");
}

const char* ul_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "ul_ccch_msg_type_c::types");
}
uint8_t ul_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_ccch_msg_type_c::types");
}

// RRCResumeRequest1-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_request1_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id.pack(bref));
  HANDLE_CODE(resume_mac_i.pack(bref));
  HANDLE_CODE(resume_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request1_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id.unpack(bref));
  HANDLE_CODE(resume_mac_i.unpack(bref));
  HANDLE_CODE(resume_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request1_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("resumeIdentity", resume_id.to_string());
  j.write_str("resumeMAC-I", resume_mac_i.to_string());
  j.write_str("resumeCause", resume_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCResumeRequest1 ::= SEQUENCE
SRSASN_CODE rrc_resume_request1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_resume_request1.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_resume_request1.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcResumeRequest1");
  rrc_resume_request1.to_json(j);
  j.end_obj();
}

// UL-CCCH1-MessageType ::= CHOICE
void ul_ccch1_msg_type_c::set(types::options e)
{
  type_ = e;
}
ul_ccch1_msg_type_c::c1_c_& ul_ccch1_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void ul_ccch1_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void ul_ccch1_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch1_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch1_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch1_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_resume_request1_s& ul_ccch1_msg_type_c::c1_c_::set_rrc_resume_request1()
{
  set(types::rrc_resume_request1);
  return c;
}
void ul_ccch1_msg_type_c::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_ccch1_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_ccch1_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ul_ccch1_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_resume_request1:
      j.write_fieldname("rrcResumeRequest1");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch1_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_resume_request1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch1_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_resume_request1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch1_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_ccch1_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcResumeRequest1", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ul_ccch1_msg_type_c::c1_c_::types");
}
uint8_t ul_ccch1_msg_type_c::c1_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_ccch1_msg_type_c::c1_c_::types");
}

const char* ul_ccch1_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "ul_ccch1_msg_type_c::types");
}
uint8_t ul_ccch1_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_ccch1_msg_type_c::types");
}

// UL-CCCH1-Message ::= SEQUENCE
SRSASN_CODE ul_ccch1_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch1_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_ccch1_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-CCCH1-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* plmn_id_eutra_5_gc_c::types_opts::to_string() const
{
  static const char* names[] = {"plmn-Identity-EUTRA-5GC", "plmn-index"};
  return convert_enum_idx(names, 2, value, "plmn_id_eutra_5_gc_c::types");
}
int8_t plmn_id_eutra_5_gc_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-5};
  return map_enum_number(numbers, 1, value, "plmn_id_eutra_5_gc_c::types");
}

const char* cell_id_eutra_5_gc_c::types_opts::to_string() const
{
  static const char* names[] = {"cellIdentity-EUTRA", "cellId-index"};
  return convert_enum_idx(names, 2, value, "cell_id_eutra_5_gc_c::types");
}

const char* default_dc_location_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"ul", "dl", "ulAndDL"};
  return convert_enum_idx(names, 3, value, "default_dc_location_r17_c::types");
}

const char* cc_group_r17_s::offset_to_default_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"offsetValue", "offsetlist"};
  return convert_enum_idx(names, 2, value, "cc_group_r17_s::offset_to_default_r17_c_::types");
}

const char* ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"bwp-Id-r16", "deactivatedCarrier-r16"};
  return convert_enum_idx(names, 2, value, "ul_tx_direct_current_carrier_info_r16_s::serv_cell_info_r16_c_::types");
}

const char* sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"sl-AM-Mode-r16", "sl-UM-Mode-r16"};
  return convert_enum_idx(names, 2, value, "sl_rlc_mode_ind_r16_s::sl_mode_r16_c_::types");
}

const char* per_ra_info_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"perRASSBInfoList-r16", "perRACSI-RSInfoList-r16"};
  return convert_enum_idx(names, 2, value, "per_ra_info_r16_c::types");
}

const char* sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_on_dur_timer_r17_c_::types");
}

const char* sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types");
}
uint16_t sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(
      numbers, 20, value, "sl_drx_cfg_uc_semi_static_r17_s::sl_drx_cycle_start_offset_r17_c_::types");
}

const char* visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"cgi-Info-r17", "pci-arfcn-r17"};
  return convert_enum_idx(
      names, 2, value, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::nr_cell_id_r17_c_::types");
}

const char* visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r17", "pci-arfcn-r17"};
  return convert_enum_idx(
      names, 2, value, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::eutra_cell_id_r17_c_::types");
}

const char* visited_pscell_info_r17_s::visited_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-CellId-r17", "eutra-CellId-r17"};
  return convert_enum_idx(names, 2, value, "visited_pscell_info_r17_s::visited_cell_id_r17_c_::types");
}

const char* sl_tx_res_req_comm_relay_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"sl-TxResourceReqL2U2N-Relay-r17", "sl-TxResourceReqL3U2N-Relay-r17"};
  return convert_enum_idx(names, 2, value, "sl_tx_res_req_comm_relay_r17_c::types");
}
uint8_t sl_tx_res_req_comm_relay_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "sl_tx_res_req_comm_relay_r17_c::types");
}

const char* cho_candidate_cell_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r17", "pci-arfcn-r17"};
  return convert_enum_idx(names, 2, value, "cho_candidate_cell_r17_c::types");
}

const char* nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms20-r16", "ms40-r16", "ms80-r16", "ms160-r16"};
  return convert_enum_idx(names, 4, value, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types");
}
uint8_t nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 40, 80, 160};
  return map_enum_number(numbers, 4, value, "nr_prs_meas_info_r16_s::nr_meas_prs_repeat_and_offset_r16_c_::types");
}

const char* nr_time_stamp_r17_s::nr_slot_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"scs15-r17", "scs30-r17", "scs60-r17", "scs120-r17"};
  return convert_enum_idx(names, 4, value, "nr_time_stamp_r17_s::nr_slot_r17_c_::types");
}
uint8_t nr_time_stamp_r17_s::nr_slot_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "nr_time_stamp_r17_s::nr_slot_r17_c_::types");
}

const char* visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cgi-Info", "pci-arfcn-r16"};
  return convert_enum_idx(names, 2, value, "visited_cell_info_r16_s::visited_cell_id_r16_c_::nr_cell_id_r16_c_::types");
}

const char* visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(
      names, 2, value, "visited_cell_info_r16_s::visited_cell_id_r16_c_::eutra_cell_id_r16_c_::types");
}

const char* visited_cell_info_r16_s::visited_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-CellId-r16", "eutra-CellId-r16"};
  return convert_enum_idx(names, 2, value, "visited_cell_info_r16_s::visited_cell_id_r16_c_::types");
}

const char* meas_results_sl_r16_s::meas_results_list_sl_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"measResultNR-SL-r16"};
  return convert_enum_idx(names, 1, value, "meas_results_sl_r16_s::meas_results_list_sl_r16_c_::types");
}

const char* ra_report_r16_s::cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(names, 2, value, "ra_report_r16_s::cell_id_r16_c_::types");
}

const char* rrc_recfg_complete_v1560_ies_s::scg_resp_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG-Response", "eutra-SCG-Response"};
  return convert_enum_idx(names, 2, value, "rrc_recfg_complete_v1560_ies_s::scg_resp_c_::types");
}

const char* delay_budget_report_c::types_opts::to_string() const
{
  static const char* names[] = {"type1"};
  return convert_enum_idx(names, 1, value, "delay_budget_report_c::types");
}
uint8_t delay_budget_report_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "delay_budget_report_c::types");
}

const char* location_meas_info_c::types_opts::to_string() const
{
  static const char* names[] = {"eutra-RSTD", "eutra-FineTimingDetection", "nr-PRS-Measurement-r16"};
  return convert_enum_idx(names, 3, value, "location_meas_info_c::types");
}

const char* meas_results_s::meas_result_neigh_cells_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "measResultListNR", "measResultListEUTRA", "measResultListUTRA-FDD-r16", "sl-MeasResultsCandRelay-r17"};
  return convert_enum_idx(names, 4, value, "meas_results_s::meas_result_neigh_cells_c_::types");
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nrPreviousCell-r16", "eutraPreviousCell-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::prev_pcell_id_r16_c_::types");
}

const char*
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::nr_failed_pcell_id_r16_c_::types");
}

const char*
rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::types_opts::to_string()
    const
{
  static const char* names[] = {"cellGlobalId-r16", "pci-arfcn-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::eutra_failed_pcell_id_r16_c_::types");
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nrFailedPCellId-r16", "eutraFailedPCellId-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::failed_pcell_id_r16_c_::types");
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nrReconnectCellId-r16", "eutraReconnectCellId-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::reconnect_cell_id_r16_c_::types");
}

const char* rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellGlobalId-r17", "pci-arfcn-r17"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::nr_rlf_report_r16_s_::cho_cell_id_r17_c_::types");
}

const char* rlf_report_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"nr-RLF-Report-r16", "eutra-RLF-Report-r16"};
  return convert_enum_idx(names, 2, value, "rlf_report_r16_c::types");
}

const char* rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr-SCG-Response", "eutra-SCG-Response"};
  return convert_enum_idx(names, 2, value, "rrc_resume_complete_v1610_ies_s::scg_resp_r16_c_::types");
}

const char* s_nssai_c::types_opts::to_string() const
{
  static const char* names[] = {"sst", "sst-SD"};
  return convert_enum_idx(names, 2, value, "s_nssai_c::types");
}

const char* iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::
    types_opts::to_string() const
{
  static const char* names[] = {"iab-IPv6-AddressNumReq-r16", "iab-IPv6-AddressPrefixReq-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_request_r16_s_::iab_ip_v6_address_req_r16_c_::types");
}

const char*
iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::types_opts::to_string()
    const
{
  static const char* names[] = {"iab-IPv6-AddressReport-r16", "iab-IPv6-PrefixReport-r16"};
  return convert_enum_idx(
      names,
      2,
      value,
      "iab_other_info_r16_ies_s::ip_info_type_r16_c_::iab_ip_report_r16_s_::iab_ip_v6_report_r16_c_::types");
}

const char* iab_other_info_r16_ies_s::ip_info_type_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"iab-IP-Request-r16", "iab-IP-Report-r16"};
  return convert_enum_idx(names, 2, value, "iab_other_info_r16_ies_s::ip_info_type_r16_c_::types");
}

// MeasurementReportAppLayer-r17-IEs ::= SEQUENCE
SRSASN_CODE meas_report_app_layer_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_report_app_layer_list_r17, 1, 16));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_report_app_layer_r17_ies_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_report_app_layer_list_r17, bref, 1, 16));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_report_app_layer_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measurementReportAppLayerList-r17");
  for (const auto& e1 : meas_report_app_layer_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::types_opts::to_string() const
{
  static const char* names[] = {"ng-5G-S-TMSI", "ng-5G-S-TMSI-Part2"};
  return convert_enum_idx(names, 2, value, "rrc_setup_complete_ies_s::ng_5_g_s_tmsi_value_c_::types");
}

const char* counter_check_resp_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"counterCheckResponse", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "counter_check_resp_s::crit_exts_c_::types");
}

const char* ded_sib_request_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"dedicatedSIBRequest-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ded_sib_request_r16_s::crit_exts_c_::types");
}

const char* fail_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"failureInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "fail_info_s::crit_exts_c_::types");
}

const char* iab_other_info_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"iabOtherInformation-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "iab_other_info_r16_s::crit_exts_c_::types");
}

const char* location_meas_ind_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"locationMeasurementIndication", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "location_meas_ind_s::crit_exts_c_::types");
}

const char* mb_si_nterest_ind_r17_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mbsInterestIndication-r17", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mb_si_nterest_ind_r17_s::crit_exts_c_::types");
}

const char* mcg_fail_info_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"mcgFailureInformation-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "mcg_fail_info_r16_s::crit_exts_c_::types");
}

const char* meas_report_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"measurementReport", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "meas_report_s::crit_exts_c_::types");
}

const char* rrc_recfg_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReconfigurationComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_recfg_complete_s::crit_exts_c_::types");
}

const char* rrc_reest_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcReestablishmentComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_reest_complete_s::crit_exts_c_::types");
}

const char* rrc_resume_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcResumeComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_resume_complete_s::crit_exts_c_::types");
}

const char* rrc_setup_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"rrcSetupComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "rrc_setup_complete_s::crit_exts_c_::types");
}

const char* scg_fail_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"scgFailureInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "scg_fail_info_s::crit_exts_c_::types");
}

const char* scg_fail_info_eutra_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"scgFailureInformationEUTRA", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "scg_fail_info_eutra_s::crit_exts_c_::types");
}

const char* security_mode_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"securityModeComplete", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "security_mode_complete_s::crit_exts_c_::types");
}

const char* security_mode_fail_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"securityModeFailure", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "security_mode_fail_s::crit_exts_c_::types");
}

const char* sidelink_ue_info_nr_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"sidelinkUEInformationNR-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "sidelink_ue_info_nr_r16_s::crit_exts_c_::types");
}

const char* ue_assist_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueAssistanceInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_assist_info_s::crit_exts_c_::types");
}

const char* ue_cap_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueCapabilityInformation", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_cap_info_s::crit_exts_c_::types");
}

const char* ue_info_resp_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ueInformationResponse-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_info_resp_r16_s::crit_exts_c_::types");
}

const char* ue_positioning_assist_info_r17_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"uePositioningAssistanceInfo-r17", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_positioning_assist_info_r17_s::crit_exts_c_::types");
}

const char* ul_ded_msg_segment_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulDedicatedMessageSegment-r16", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_ded_msg_segment_r16_s::crit_exts_c_::types");
}

const char* ul_info_transfer_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulInformationTransfer", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_info_transfer_s::crit_exts_c_::types");
}

const char* ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulInformationTransferIRAT-r16", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ul_info_transfer_irat_r16_s::crit_exts_c_::c1_c_::types");
}

const char* ul_info_transfer_irat_r16_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_info_transfer_irat_r16_s::crit_exts_c_::types");
}
uint8_t ul_info_transfer_irat_r16_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_info_transfer_irat_r16_s::crit_exts_c_::types");
}

const char* ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulInformationTransferMRDC", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ul_info_transfer_mrdc_s::crit_exts_c_::c1_c_::types");
}

const char* ul_info_transfer_mrdc_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ul_info_transfer_mrdc_s::crit_exts_c_::types");
}
uint8_t ul_info_transfer_mrdc_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_info_transfer_mrdc_s::crit_exts_c_::types");
}

const char* ul_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"measurementReport",
                                "rrcReconfigurationComplete",
                                "rrcSetupComplete",
                                "rrcReestablishmentComplete",
                                "rrcResumeComplete",
                                "securityModeComplete",
                                "securityModeFailure",
                                "ulInformationTransfer",
                                "locationMeasurementIndication",
                                "ueCapabilityInformation",
                                "counterCheckResponse",
                                "ueAssistanceInformation",
                                "failureInformation",
                                "ulInformationTransferMRDC",
                                "scgFailureInformation",
                                "scgFailureInformationEUTRA"};
  return convert_enum_idx(names, 16, value, "ul_dcch_msg_type_c::c1_c_::types");
}

const char* ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* names[] = {"ulDedicatedMessageSegment-r16",
                                "dedicatedSIBRequest-r16",
                                "mcgFailureInformation-r16",
                                "ueInformationResponse-r16",
                                "sidelinkUEInformationNR-r16",
                                "ulInformationTransferIRAT-r16",
                                "iabOtherInformation-r16",
                                "mbsInterestIndication-r17",
                                "uePositioningAssistanceInfo-r17",
                                "measurementReportAppLayer-r17",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types");
}

const char* ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* names[] = {"c2", "messageClassExtensionFuture-r16"};
  return convert_enum_idx(names, 2, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}

const char* ul_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* names[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(names, 2, value, "ul_dcch_msg_type_c::types");
}
uint8_t ul_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_dcch_msg_type_c::types");
}

const char* bh_lc_ch_id_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"bh-LogicalChannelIdentity-r16", "bh-LogicalChannelIdentityExt-r16"};
  return convert_enum_idx(names, 2, value, "bh_lc_ch_id_r16_c::types");
}

const char* rlc_cfg_c::types_opts::to_string() const
{
  static const char* names[] = {"am", "um-Bi-Directional", "um-Uni-Directional-UL", "um-Uni-Directional-DL"};
  return convert_enum_idx(names, 4, value, "rlc_cfg_c::types");
}

const char* beam_link_monitoring_rs_r17_s::detection_res_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(names, 2, value, "beam_link_monitoring_rs_r17_s::detection_res_r17_c_::types");
}

const char* csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::to_string() const
{
  static const char* names[] = {"row1", "row2", "row4", "other"};
  return convert_enum_idx(names, 4, value, "csi_rs_res_map_s::freq_domain_alloc_c_::types");
}
uint8_t csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "csi_rs_res_map_s::freq_domain_alloc_c_::types");
}

const char* csi_rs_res_map_s::density_c_::types_opts::to_string() const
{
  static const char* names[] = {"dot5", "one", "three", "spare"};
  return convert_enum_idx(names, 4, value, "csi_rs_res_map_s::density_c_::types");
}
uint8_t csi_rs_res_map_s::density_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 1, 3};
  return map_enum_number(numbers, 3, value, "csi_rs_res_map_s::density_c_::types");
}

const char* csi_res_periodicity_and_offset_c::types_opts::to_string() const
{
  static const char* names[] = {"slots4",
                                "slots5",
                                "slots8",
                                "slots10",
                                "slots16",
                                "slots20",
                                "slots32",
                                "slots40",
                                "slots64",
                                "slots80",
                                "slots160",
                                "slots320",
                                "slots640"};
  return convert_enum_idx(names, 13, value, "csi_res_periodicity_and_offset_c::types");
}
uint16_t csi_res_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640};
  return map_enum_number(numbers, 13, value, "csi_res_periodicity_and_offset_c::types");
}

const char* qcl_info_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* names[] = {"csi-rs", "ssb"};
  return convert_enum_idx(names, 2, value, "qcl_info_s::ref_sig_c_::types");
}

const char* dl_ppw_periodicity_and_start_slot_r17_c::scs15_c_::types_opts::to_string() const
{
  static const char* names[] = {"n4",
                                "n5",
                                "n8",
                                "n10",
                                "n16",
                                "n20",
                                "n32",
                                "n40",
                                "n64",
                                "n80",
                                "n160",
                                "n320",
                                "n640",
                                "n1280",
                                "n2560",
                                "n5120",
                                "n10240"};
  return convert_enum_idx(names, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs15_c_::types");
}
uint16_t dl_ppw_periodicity_and_start_slot_r17_c::scs15_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(numbers, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs15_c_::types");
}

const char* dl_ppw_periodicity_and_start_slot_r17_c::scs30_c_::types_opts::to_string() const
{
  static const char* names[] = {"n8",
                                "n10",
                                "n16",
                                "n20",
                                "n32",
                                "n40",
                                "n64",
                                "n80",
                                "n128",
                                "n160",
                                "n320",
                                "n640",
                                "n1280",
                                "n2560",
                                "n5120",
                                "n10240",
                                "n20480"};
  return convert_enum_idx(names, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs30_c_::types");
}
uint16_t dl_ppw_periodicity_and_start_slot_r17_c::scs30_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, 5120, 10240, 20480};
  return map_enum_number(numbers, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs30_c_::types");
}

const char* dl_ppw_periodicity_and_start_slot_r17_c::scs60_c_::types_opts::to_string() const
{
  static const char* names[] = {"n16",
                                "n20",
                                "n32",
                                "n40",
                                "n64",
                                "n80",
                                "n128",
                                "n160",
                                "n256",
                                "n320",
                                "n640",
                                "n1280",
                                "n2560",
                                "n5120",
                                "n10240",
                                "n20480",
                                "n40960"};
  return convert_enum_idx(names, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs60_c_::types");
}
uint16_t dl_ppw_periodicity_and_start_slot_r17_c::scs60_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {
      16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960};
  return map_enum_number(numbers, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs60_c_::types");
}

const char* dl_ppw_periodicity_and_start_slot_r17_c::scs120_c_::types_opts::to_string() const
{
  static const char* names[] = {"n32",
                                "n40",
                                "n64",
                                "n80",
                                "n128",
                                "n160",
                                "n256",
                                "n320",
                                "n512",
                                "n640",
                                "n1280",
                                "n2560",
                                "n5120",
                                "n10240",
                                "n20480",
                                "n40960",
                                "n81920"};
  return convert_enum_idx(names, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs120_c_::types");
}
uint32_t dl_ppw_periodicity_and_start_slot_r17_c::scs120_c_::types_opts::to_number() const
{
  static const uint32_t numbers[] = {
      32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920};
  return map_enum_number(numbers, 17, value, "dl_ppw_periodicity_and_start_slot_r17_c::scs120_c_::types");
}

const char* dl_ppw_periodicity_and_start_slot_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"scs15", "scs30", "scs60", "scs120"};
  return convert_enum_idx(names, 4, value, "dl_ppw_periodicity_and_start_slot_r17_c::types");
}
uint8_t dl_ppw_periodicity_and_start_slot_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "dl_ppw_periodicity_and_start_slot_r17_c::types");
}

const char* rate_match_pattern_group_item_c_::types_opts::to_string() const
{
  static const char* names[] = {"cellLevel", "bwpLevel"};
  return convert_enum_idx(names, 2, value, "rate_match_pattern_group_item_c_::types");
}

const char* repeat_scheme_cfg_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"fdm-TDM-r16", "slotBased-r16"};
  return convert_enum_idx(names, 2, value, "repeat_scheme_cfg_r16_c::types");
}

const char* candidate_beam_rs_r16_s::candidate_beam_cfg_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-r16", "csi-RS-r16"};
  return convert_enum_idx(names, 2, value, "candidate_beam_rs_r16_s::candidate_beam_cfg_r16_c_::types");
}

const char* pdsch_cfg_s::prb_bundling_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"staticBundling", "dynamicBundling"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::prb_bundling_type_c_::types");
}

const char* pdsch_cfg_s::prb_bundling_type_dci_1_2_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"staticBundling-r16", "dynamicBundling-r16"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::prb_bundling_type_dci_1_2_r16_c_::types");
}

const char* pdsch_cfg_s::dl_or_joint_tci_state_list_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"explicitlist", "unifiedTCI-StateRef-r17"};
  return convert_enum_idx(names, 2, value, "pdsch_cfg_s::dl_or_joint_tci_state_list_r17_c_::types");
}

const char* radio_link_monitoring_rs_s::detection_res_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(names, 2, value, "radio_link_monitoring_rs_s::detection_res_c_::types");
}

// BWP-DownlinkDedicatedSDT-r17 ::= SEQUENCE
SRSASN_CODE bwp_dl_ded_sdt_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pdcch_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_r17_present, 1));

  if (pdcch_cfg_r17_present) {
    HANDLE_CODE(pdcch_cfg_r17.pack(bref));
  }
  if (pdsch_cfg_r17_present) {
    HANDLE_CODE(pdsch_cfg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_dl_ded_sdt_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pdcch_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_r17_present, 1));

  if (pdcch_cfg_r17_present) {
    HANDLE_CODE(pdcch_cfg_r17.unpack(bref));
  }
  if (pdsch_cfg_r17_present) {
    HANDLE_CODE(pdsch_cfg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void bwp_dl_ded_sdt_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcch_cfg_r17_present) {
    j.write_fieldname("pdcch-Config-r17");
    pdcch_cfg_r17.to_json(j);
  }
  if (pdsch_cfg_r17_present) {
    j.write_fieldname("pdsch-Config-r17");
    pdsch_cfg_r17.to_json(j);
  }
  j.end_obj();
}

const char* pucch_pathloss_ref_rs_r16_s::ref_sig_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index-r16", "csi-RS-Index-r16"};
  return convert_enum_idx(names, 2, value, "pucch_pathloss_ref_rs_r16_s::ref_sig_r16_c_::types");
}

const char* pathloss_ref_rs_cfg_c::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(names, 2, value, "pathloss_ref_rs_cfg_c::types");
}

const char* pucch_pathloss_ref_rs_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(names, 2, value, "pucch_pathloss_ref_rs_s::ref_sig_c_::types");
}

const char* beta_offsets_cross_pri_sel_cg_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"dynamic-r17", "semiStatic-r17"};
  return convert_enum_idx(names, 2, value, "beta_offsets_cross_pri_sel_cg_r17_c::types");
}

const char* cg_cot_sharing_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"noCOT-Sharing-r16", "cot-Sharing-r16"};
  return convert_enum_idx(names, 2, value, "cg_cot_sharing_r16_c::types");
}

const char* cg_cot_sharing_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"noCOT-Sharing-r17", "cot-Sharing-r17"};
  return convert_enum_idx(names, 2, value, "cg_cot_sharing_r17_c::types");
}

const char* cg_sdt_cfg_r17_s::sdt_ssb_subset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"shortBitmap-r17", "mediumBitmap-r17", "longBitmap-r17"};
  return convert_enum_idx(names, 3, value, "cg_sdt_cfg_r17_s::sdt_ssb_subset_r17_c_::types");
}

const char* cg_sdt_cfg_r17_s::sdt_dmrs_ports_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"dmrsType1-r17", "dmrsType2-r17"};
  return convert_enum_idx(names, 2, value, "cg_sdt_cfg_r17_s::sdt_dmrs_ports_r17_c_::types");
}
uint8_t cg_sdt_cfg_r17_s::sdt_dmrs_ports_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "cg_sdt_cfg_r17_s::sdt_dmrs_ports_r17_c_::types");
}

const char* cg_uci_on_pusch_c::types_opts::to_string() const
{
  static const char* names[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(names, 2, value, "cg_uci_on_pusch_c::types");
}

const char* prach_res_ded_bfr_c::types_opts::to_string() const
{
  static const char* names[] = {"ssb", "csi-RS"};
  return convert_enum_idx(names, 2, value, "prach_res_ded_bfr_c::types");
}

const char* pucch_res_s::format_c_::types_opts::to_string() const
{
  static const char* names[] = {"format0", "format1", "format2", "format3", "format4"};
  return convert_enum_idx(names, 5, value, "pucch_res_s::format_c_::types");
}
uint8_t pucch_res_s::format_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4};
  return map_enum_number(numbers, 5, value, "pucch_res_s::format_c_::types");
}

const char* pucch_res_ext_v1610_s::interlace_alloc_r16_s_::interlace0_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"scs15", "scs30"};
  return convert_enum_idx(names, 2, value, "pucch_res_ext_v1610_s::interlace_alloc_r16_s_::interlace0_r16_c_::types");
}
uint8_t pucch_res_ext_v1610_s::interlace_alloc_r16_s_::interlace0_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30};
  return map_enum_number(numbers, 2, value, "pucch_res_ext_v1610_s::interlace_alloc_r16_s_::interlace0_r16_c_::types");
}

const char* pucch_res_ext_v1610_s::format_v1610_c_::types_opts::to_string() const
{
  static const char* names[] = {"interlace1-v1610", "occ-v1610"};
  return convert_enum_idx(names, 2, value, "pucch_res_ext_v1610_s::format_v1610_c_::types");
}
uint8_t pucch_res_ext_v1610_s::format_v1610_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "pucch_res_ext_v1610_s::format_v1610_c_::types");
}

const char* pucch_spatial_relation_info_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index", "srs"};
  return convert_enum_idx(names, 3, value, "pucch_spatial_relation_info_s::ref_sig_c_::types");
}

const char* pusch_pathloss_ref_rs_s::ref_sig_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(names, 2, value, "pusch_pathloss_ref_rs_s::ref_sig_c_::types");
}

const char* pusch_pathloss_ref_rs_r16_s::ref_sig_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index-r16", "csi-RS-Index-r16"};
  return convert_enum_idx(names, 2, value, "pusch_pathloss_ref_rs_r16_s::ref_sig_r16_c_::types");
}

const char* srs_periodicity_and_offset_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl1",   "sl2",    "sl4",    "sl5",    "sl8",     "sl10",    "sl16",
                                "sl20",  "sl32",   "sl40",   "sl64",   "sl80",    "sl160",   "sl320",
                                "sl640", "sl1280", "sl2560", "sl5120", "sl10240", "sl40960", "sl81920"};
  return convert_enum_idx(names, 21, value, "srs_periodicity_and_offset_r16_c::types");
}
uint32_t srs_periodicity_and_offset_r16_c::types_opts::to_number() const
{
  static const uint32_t numbers[] = {1,  2,   4,   5,   8,    10,   16,   20,    32,    40,   64,
                                     80, 160, 320, 640, 1280, 2560, 5120, 10240, 40960, 81920};
  return map_enum_number(numbers, 21, value, "srs_periodicity_and_offset_r16_c::types");
}

const char* srs_periodicity_and_offset_ext_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"sl128", "sl256", "sl512", "sl20480"};
  return convert_enum_idx(names, 4, value, "srs_periodicity_and_offset_ext_r16_c::types");
}
uint16_t srs_periodicity_and_offset_ext_r16_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {128, 256, 512, 20480};
  return map_enum_number(numbers, 4, value, "srs_periodicity_and_offset_ext_r16_c::types");
}

const char* srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"srs-ResourceId-r16", "srs-PosResourceId-r16"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_"
                          "relation_r16_s_::res_sel_r16_c_::types");
}

const char* srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-IndexServing-r16", "csi-RS-IndexServing-r16", "srs-SpatialRelation-r16"};
  return convert_enum_idx(
      names, 3, value, "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::types");
}

const char* srs_spatial_relation_info_pos_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"servingRS-r16", "ssb-Ncell-r16", "dl-PRS-r16"};
  return convert_enum_idx(names, 3, value, "srs_spatial_relation_info_pos_r16_c::types");
}

const char* sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"sym2",
                                "sym6or7",
                                "sl1",
                                "sl2",
                                "sl4",
                                "sl5",
                                "sl8",
                                "sl10",
                                "sl16",
                                "sl20",
                                "sl40",
                                "sl80",
                                "sl160",
                                "sl320",
                                "sl640"};
  return convert_enum_idx(names, 15, value, "sched_request_res_cfg_s::periodicity_and_offset_c_::types");
}

const char* sched_request_res_cfg_ext_v1700_s::periodicity_and_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"sl1280", "sl2560", "sl5120"};
  return convert_enum_idx(names, 3, value, "sched_request_res_cfg_ext_v1700_s::periodicity_and_offset_r17_c_::types");
}
uint16_t sched_request_res_cfg_ext_v1700_s::periodicity_and_offset_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1280, 2560, 5120};
  return map_enum_number(numbers, 3, value, "sched_request_res_cfg_ext_v1700_s::periodicity_and_offset_r17_c_::types");
}

const char* uci_on_pusch_s::beta_offsets_c_::types_opts::to_string() const
{
  static const char* names[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(names, 2, value, "uci_on_pusch_s::beta_offsets_c_::types");
}

const char* uci_on_pusch_dci_0_2_r16_s::beta_offsets_dci_0_2_r16_c_::dyn_dci_0_2_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneBit-r16", "twoBits-r16"};
  return convert_enum_idx(
      names, 2, value, "uci_on_pusch_dci_0_2_r16_s::beta_offsets_dci_0_2_r16_c_::dyn_dci_0_2_r16_c_::types");
}
uint8_t uci_on_pusch_dci_0_2_r16_s::beta_offsets_dci_0_2_r16_c_::dyn_dci_0_2_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "uci_on_pusch_dci_0_2_r16_s::beta_offsets_dci_0_2_r16_c_::dyn_dci_0_2_r16_c_::types");
}

const char* uci_on_pusch_dci_0_2_r16_s::beta_offsets_dci_0_2_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"dynamicDCI-0-2-r16", "semiStaticDCI-0-2-r16"};
  return convert_enum_idx(names, 2, value, "uci_on_pusch_dci_0_2_r16_s::beta_offsets_dci_0_2_r16_c_::types");
}

const char* beta_offsets_cross_pri_sel_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"dynamic-r17", "semiStatic-r17"};
  return convert_enum_idx(names, 2, value, "beta_offsets_cross_pri_sel_r17_c::types");
}

const char* beta_offsets_cross_pri_sel_dci_0_2_r17_c::dyn_dci_0_2_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneBit-r17", "twoBits-r17"};
  return convert_enum_idx(names, 2, value, "beta_offsets_cross_pri_sel_dci_0_2_r17_c::dyn_dci_0_2_r17_c_::types");
}
uint8_t beta_offsets_cross_pri_sel_dci_0_2_r17_c::dyn_dci_0_2_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "beta_offsets_cross_pri_sel_dci_0_2_r17_c::dyn_dci_0_2_r17_c_::types");
}

const char* beta_offsets_cross_pri_sel_dci_0_2_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"dynamicDCI-0-2-r17", "semiStaticDCI-0-2-r17"};
  return convert_enum_idx(names, 2, value, "beta_offsets_cross_pri_sel_dci_0_2_r17_c::types");
}

const char* invalid_symbol_pattern_r16_s::symbols_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"oneSlot", "twoSlots"};
  return convert_enum_idx(names, 2, value, "invalid_symbol_pattern_r16_s::symbols_r16_c_::types");
}
uint8_t invalid_symbol_pattern_r16_s::symbols_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "invalid_symbol_pattern_r16_s::symbols_r16_c_::types");
}

const char* invalid_symbol_pattern_r16_s::periodicity_and_pattern_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"n2", "n4", "n5", "n8", "n10", "n20", "n40"};
  return convert_enum_idx(names, 7, value, "invalid_symbol_pattern_r16_s::periodicity_and_pattern_r16_c_::types");
}
uint8_t invalid_symbol_pattern_r16_s::periodicity_and_pattern_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 5, 8, 10, 20, 40};
  return map_enum_number(numbers, 7, value, "invalid_symbol_pattern_r16_s::periodicity_and_pattern_r16_c_::types");
}

const char* mpe_res_r17_s::mpe_ref_sig_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"csi-RS-Resource-r17", "ssb-Resource-r17"};
  return convert_enum_idx(names, 2, value, "mpe_res_r17_s::mpe_ref_sig_r17_c_::types");
}

const char* pucch_cfg_s::subslot_len_for_pucch_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"normalCP-r16", "extendedCP-r16"};
  return convert_enum_idx(names, 2, value, "pucch_cfg_s::subslot_len_for_pucch_r16_c_::types");
}

const char* srs_pos_res_r16_s::tx_comb_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"n2-r16", "n4-r16", "n8-r16"};
  return convert_enum_idx(names, 3, value, "srs_pos_res_r16_s::tx_comb_r16_c_::types");
}
uint8_t srs_pos_res_r16_s::tx_comb_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "srs_pos_res_r16_s::tx_comb_r16_c_::types");
}

const char* srs_pos_res_r16_s::res_type_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"aperiodic-r16", "semi-persistent-r16", "periodic-r16"};
  return convert_enum_idx(names, 3, value, "srs_pos_res_r16_s::res_type_r16_c_::types");
}

const char* srs_pos_res_set_r16_s::res_type_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"aperiodic-r16", "semi-persistent-r16", "periodic-r16"};
  return convert_enum_idx(names, 3, value, "srs_pos_res_set_r16_s::res_type_r16_c_::types");
}

const char* srs_pos_res_set_r16_s::pathloss_ref_rs_pos_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-IndexServing-r16", "ssb-Ncell-r16", "dl-PRS-r16"};
  return convert_enum_idx(names, 3, value, "srs_pos_res_set_r16_s::pathloss_ref_rs_pos_r16_c_::types");
}

const char* srs_res_set_s::res_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"aperiodic", "semi-persistent", "periodic"};
  return convert_enum_idx(names, 3, value, "srs_res_set_s::res_type_c_::types");
}

const char* pusch_cfg_s::freq_hop_dci_0_2_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"pusch-RepTypeA", "pusch-RepTypeB"};
  return convert_enum_idx(names, 2, value, "pusch_cfg_s::freq_hop_dci_0_2_r16_c_::types");
}

const char* pathloss_ref_rs_r17_s::ref_sig_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index", "csi-RS-Index"};
  return convert_enum_idx(names, 2, value, "pathloss_ref_rs_r17_s::ref_sig_r17_c_::types");
}

const char* tci_ul_state_r17_s::ref_sig_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb-Index-r17", "csi-RS-Index-r17", "srs-r17"};
  return convert_enum_idx(names, 3, value, "tci_ul_state_r17_s::ref_sig_r17_c_::types");
}

const char* bwp_ul_ded_s::ul_tci_state_list_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"explicitlist", "unifiedTCI-StateRef-r17"};
  return convert_enum_idx(names, 2, value, "bwp_ul_ded_s::ul_tci_state_list_r17_c_::types");
}

// BWP-UplinkDedicatedSDT-r17 ::= SEQUENCE
SRSASN_CODE bwp_ul_ded_sdt_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pusch_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(cfg_grant_cfg_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(cfg_grant_cfg_to_release_list_r17.size() > 0, 1));

  if (pusch_cfg_r17_present) {
    HANDLE_CODE(pusch_cfg_r17.pack(bref));
  }
  if (cfg_grant_cfg_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cfg_grant_cfg_to_add_mod_list_r17, 1, 12));
  }
  if (cfg_grant_cfg_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cfg_grant_cfg_to_release_list_r17, 1, 12, integer_packer<uint8_t>(0, 11)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bwp_ul_ded_sdt_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pusch_cfg_r17_present, 1));
  bool cfg_grant_cfg_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(cfg_grant_cfg_to_add_mod_list_r17_present, 1));
  bool cfg_grant_cfg_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(cfg_grant_cfg_to_release_list_r17_present, 1));

  if (pusch_cfg_r17_present) {
    HANDLE_CODE(pusch_cfg_r17.unpack(bref));
  }
  if (cfg_grant_cfg_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cfg_grant_cfg_to_add_mod_list_r17, bref, 1, 12));
  }
  if (cfg_grant_cfg_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cfg_grant_cfg_to_release_list_r17, bref, 1, 12, integer_packer<uint8_t>(0, 11)));
  }

  return SRSASN_SUCCESS;
}
void bwp_ul_ded_sdt_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pusch_cfg_r17_present) {
    j.write_fieldname("pusch-Config-r17");
    pusch_cfg_r17.to_json(j);
  }
  if (cfg_grant_cfg_to_add_mod_list_r17.size() > 0) {
    j.start_array("configuredGrantConfigToAddModList-r17");
    for (const auto& e1 : cfg_grant_cfg_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cfg_grant_cfg_to_release_list_r17.size() > 0) {
    j.start_array("configuredGrantConfigToReleaseList-r17");
    for (const auto& e1 : cfg_grant_cfg_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* band_params_c::types_opts::to_string() const
{
  static const char* names[] = {"eutra", "nr"};
  return convert_enum_idx(names, 2, value, "band_params_c::types");
}

const char* band_params_v1540_s::srs_carrier_switch_c_::types_opts::to_string() const
{
  static const char* names[] = {"nr", "eutra"};
  return convert_enum_idx(names, 2, value, "band_params_v1540_s::srs_carrier_switch_c_::types");
}

// PDCCH-BlindDetectionCA-MixedExt-r16 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_ca_mixed_ext_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca1_r16, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_ca2_r16, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_ca_mixed_ext_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca1_r16, bref, (uint8_t)1u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_ca2_r16, bref, (uint8_t)1u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_ca_mixed_ext_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdcch-BlindDetectionCA1-r16", pdcch_blind_detection_ca1_r16);
  j.write_int("pdcch-BlindDetectionCA2-r16", pdcch_blind_detection_ca2_r16);
  j.end_obj();
}

// PDCCH-BlindDetectionCG-UE-MixedExt-r16 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_cg_ue_mixed_ext_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_cg_ue1_r16, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_cg_ue2_r16, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_cg_ue_mixed_ext_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_cg_ue1_r16, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(pdcch_blind_detection_cg_ue2_r16, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_cg_ue_mixed_ext_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("pdcch-BlindDetectionCG-UE1-r16", pdcch_blind_detection_cg_ue1_r16);
  j.write_int("pdcch-BlindDetectionCG-UE2-r16", pdcch_blind_detection_cg_ue2_r16);
  j.end_obj();
}

// PDCCH-BlindDetectionMixedList-r16 ::= SEQUENCE
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcch_blind_detection_ca_mixed_ext_r16_present, 1));
  HANDLE_CODE(bref.pack(pdcch_blind_detection_cg_ue_mixed_ext_r16_present, 1));

  if (pdcch_blind_detection_ca_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_ca_mixed_ext_r16.pack(bref));
  }
  if (pdcch_blind_detection_cg_ue_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_mcg_ue_mixed_v16a0.pack(bref));
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_scg_ue_mixed_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcch_blind_detection_ca_mixed_ext_r16_present, 1));
  HANDLE_CODE(bref.unpack(pdcch_blind_detection_cg_ue_mixed_ext_r16_present, 1));

  if (pdcch_blind_detection_ca_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_ca_mixed_ext_r16.unpack(bref));
  }
  if (pdcch_blind_detection_cg_ue_mixed_ext_r16_present) {
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_mcg_ue_mixed_v16a0.unpack(bref));
    HANDLE_CODE(pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_scg_ue_mixed_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdcch_blind_detection_mixed_list_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcch_blind_detection_ca_mixed_ext_r16_present) {
    j.write_fieldname("pdcch-BlindDetectionCA-MixedExt-r16");
    pdcch_blind_detection_ca_mixed_ext_r16.to_json(j);
  }
  if (pdcch_blind_detection_cg_ue_mixed_ext_r16_present) {
    j.write_fieldname("pdcch-BlindDetectionCG-UE-MixedExt-r16");
    j.start_obj();
    j.write_fieldname("pdcch-BlindDetectionMCG-UE-Mixed-v16a0");
    pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_mcg_ue_mixed_v16a0.to_json(j);
    j.write_fieldname("pdcch-BlindDetectionSCG-UE-Mixed-v16a0");
    pdcch_blind_detection_cg_ue_mixed_ext_r16.pdcch_blind_detection_scg_ue_mixed_v16a0.to_json(j);
    j.end_obj();
  }
  j.end_obj();
}

void pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::destroy_()
{
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.destroy<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.destroy<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    default:
      break;
  }
}
void pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.init<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.init<pdcch_blind_detection_ca_mixed_ext_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }
}
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::
    pdcch_blind_detection_ca_mixed_ext_r16_c_(
        const pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.init(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.init(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }
}
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_&
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::operator=(
    const pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      c.set(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      c.set(other.c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }

  return *this;
}
pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_mixed_list_r16_s::
    pdcch_blind_detection_ca_mixed_ext_r16_c_::set_pdcch_blind_detection_ca_mixed_v16a0()
{
  set(types::pdcch_blind_detection_ca_mixed_v16a0);
  return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
}
pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_mixed_list_r16_s::
    pdcch_blind_detection_ca_mixed_ext_r16_c_::set_pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0()
{
  set(types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0);
  return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
}
void pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      j.write_fieldname("pdcch-BlindDetectionCA-Mixed-v16a0");
      c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().to_json(j);
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      j.write_fieldname("pdcch-BlindDetectionCA-Mixed-NonAlignedSpan-v16a0");
      c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().pack(bref));
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pdcch_blind_detection_ca_mixed_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().unpack(bref));
      break;
    case types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0:
      HANDLE_CODE(c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char*
pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"pdcch-BlindDetectionCA-Mixed-v16a0",
                                "pdcch-BlindDetectionCA-Mixed-NonAlignedSpan-v16a0"};
  return convert_enum_idx(
      names, 2, value, "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types");
}

// CA-ParametersNR-v16a0 ::= SEQUENCE
SRSASN_CODE ca_params_nr_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, pdcch_blind_detection_mixed_list_r16, 1, 7));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nr_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(pdcch_blind_detection_mixed_list_r16, bref, 1, 7));

  return SRSASN_SUCCESS;
}
void ca_params_nr_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("pdcch-BlindDetectionMixedList-r16");
  for (const auto& e1 : pdcch_blind_detection_mixed_list_r16) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// CA-ParametersNRDC-v16a0 ::= SEQUENCE
SRSASN_CODE ca_params_nrdc_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_params_nr_for_dc_v16a0_present, 1));

  if (ca_params_nr_for_dc_v16a0_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ca_params_nrdc_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_params_nr_for_dc_v16a0_present, 1));

  if (ca_params_nr_for_dc_v16a0_present) {
    HANDLE_CODE(ca_params_nr_for_dc_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ca_params_nrdc_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ca_params_nr_for_dc_v16a0_present) {
    j.write_fieldname("ca-ParametersNR-ForDC-v16a0");
    ca_params_nr_for_dc_v16a0.to_json(j);
  }
  j.end_obj();
}

// BandCombination-v16a0 ::= SEQUENCE
SRSASN_CODE band_combination_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ca_params_nr_v16a0_present, 1));
  HANDLE_CODE(bref.pack(ca_params_nrdc_v16a0_present, 1));

  if (ca_params_nr_v16a0_present) {
    HANDLE_CODE(ca_params_nr_v16a0.pack(bref));
  }
  if (ca_params_nrdc_v16a0_present) {
    HANDLE_CODE(ca_params_nrdc_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ca_params_nr_v16a0_present, 1));
  HANDLE_CODE(bref.unpack(ca_params_nrdc_v16a0_present, 1));

  if (ca_params_nr_v16a0_present) {
    HANDLE_CODE(ca_params_nr_v16a0.unpack(bref));
  }
  if (ca_params_nrdc_v16a0_present) {
    HANDLE_CODE(ca_params_nrdc_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ca_params_nr_v16a0_present) {
    j.write_fieldname("ca-ParametersNR-v16a0");
    ca_params_nr_v16a0.to_json(j);
  }
  if (ca_params_nrdc_v16a0_present) {
    j.write_fieldname("ca-ParametersNRDC-v16a0");
    ca_params_nrdc_v16a0.to_json(j);
  }
  j.end_obj();
}

// BandCombination-UplinkTxSwitch-v16a0 ::= SEQUENCE
SRSASN_CODE band_combination_ul_tx_switch_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(band_combination_v16a0_present, 1));

  if (band_combination_v16a0_present) {
    HANDLE_CODE(band_combination_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_ul_tx_switch_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(band_combination_v16a0_present, 1));

  if (band_combination_v16a0_present) {
    HANDLE_CODE(band_combination_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void band_combination_ul_tx_switch_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (band_combination_v16a0_present) {
    j.write_fieldname("bandCombination-v16a0");
    band_combination_v16a0.to_json(j);
  }
  j.end_obj();
}

const char* band_params_sidelink_discovery_r17_s::sl_tx_mode2_partial_sensing_r17_s_::
    scs_cp_pattern_tx_sidelink_mode_two_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-r17", "fr2-r17"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "band_params_sidelink_discovery_r17_s::sl_tx_mode2_partial_sensing_r17_s_::scs_cp_pattern_tx_"
                          "sidelink_mode_two_r17_c_::types");
}
uint8_t band_params_sidelink_discovery_r17_s::sl_tx_mode2_partial_sensing_r17_s_::
    scs_cp_pattern_tx_sidelink_mode_two_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "band_params_sidelink_discovery_r17_s::sl_tx_mode2_partial_sensing_r17_s_::scs_cp_pattern_tx_"
                         "sidelink_mode_two_r17_c_::types");
}

const char* band_params_sidelink_eutra_nr_r16_c::types_opts::to_string() const
{
  static const char* names[] = {"eutra", "nr"};
  return convert_enum_idx(names, 2, value, "band_params_sidelink_eutra_nr_r16_c::types");
}

const char* band_params_sidelink_eutra_nr_v1630_c::types_opts::to_string() const
{
  static const char* names[] = {"eutra", "nr"};
  return convert_enum_idx(names, 2, value, "band_params_sidelink_eutra_nr_v1630_c::types");
}

const char* band_params_sidelink_eutra_nr_v1710_c::nr_s_::sl_tx_mode2_partial_sensing_r17_s_::
    scs_cp_pattern_tx_sidelink_mode_two_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-r17", "fr2-r17"};
  return convert_enum_idx(names,
                          2,
                          value,
                          "band_params_sidelink_eutra_nr_v1710_c::nr_s_::sl_tx_mode2_partial_sensing_r17_s_::scs_cp_"
                          "pattern_tx_sidelink_mode_two_r17_c_::types");
}
uint8_t band_params_sidelink_eutra_nr_v1710_c::nr_s_::sl_tx_mode2_partial_sensing_r17_s_::
    scs_cp_pattern_tx_sidelink_mode_two_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "band_params_sidelink_eutra_nr_v1710_c::nr_s_::sl_tx_mode2_partial_sensing_r17_s_::scs_cp_"
                         "pattern_tx_sidelink_mode_two_r17_c_::types");
}

const char* band_params_sidelink_eutra_nr_v1710_c::types_opts::to_string() const
{
  static const char* names[] = {"eutra", "nr"};
  return convert_enum_idx(names, 2, value, "band_params_sidelink_eutra_nr_v1710_c::types");
}

const char*
prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_dur_of_prs_processing_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"ppw-durationOfPRS-Processing1-r17", "ppw-durationOfPRS-Processing2-r17"};
  return convert_enum_idx(
      names, 2, value, "prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_dur_of_prs_processing_r17_c_::types");
}
uint8_t
prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_dur_of_prs_processing_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers,
      2,
      value,
      "prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_dur_of_prs_processing_r17_c_::types");
}

const char*
prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_max_num_of_dl_bw_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-r17", "fr2-r17"};
  return convert_enum_idx(
      names, 2, value, "prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_max_num_of_dl_bw_r17_c_::types");
}
uint8_t prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_max_num_of_dl_bw_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "prs_processing_cap_outside_m_gin_ppw_per_type_r17_s::ppw_max_num_of_dl_bw_r17_c_::types");
}

const char* band_nr_s::ch_bws_dl_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1", "fr2"};
  return convert_enum_idx(names, 2, value, "band_nr_s::ch_bws_dl_c_::types");
}
uint8_t band_nr_s::ch_bws_dl_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_nr_s::ch_bws_dl_c_::types");
}

const char* band_nr_s::ch_bws_ul_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1", "fr2"};
  return convert_enum_idx(names, 2, value, "band_nr_s::ch_bws_ul_c_::types");
}
uint8_t band_nr_s::ch_bws_ul_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_nr_s::ch_bws_ul_c_::types");
}

const char* band_nr_s::ch_bws_dl_v1590_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1", "fr2"};
  return convert_enum_idx(names, 2, value, "band_nr_s::ch_bws_dl_v1590_c_::types");
}
uint8_t band_nr_s::ch_bws_dl_v1590_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_nr_s::ch_bws_dl_v1590_c_::types");
}

const char* band_nr_s::ch_bws_ul_v1590_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1", "fr2"};
  return convert_enum_idx(names, 2, value, "band_nr_s::ch_bws_ul_v1590_c_::types");
}
uint8_t band_nr_s::ch_bws_ul_v1590_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_nr_s::ch_bws_ul_v1590_c_::types");
}

const char* band_nr_s::ch_bw_dl_iab_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-100mhz", "fr2-200mhz"};
  return convert_enum_idx(names, 2, value, "band_nr_s::ch_bw_dl_iab_r16_c_::types");
}
uint8_t band_nr_s::ch_bw_dl_iab_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_nr_s::ch_bw_dl_iab_r16_c_::types");
}

const char* band_nr_s::ch_bw_ul_iab_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-100mhz", "fr2-200mhz"};
  return convert_enum_idx(names, 2, value, "band_nr_s::ch_bw_ul_iab_r16_c_::types");
}
uint8_t band_nr_s::ch_bw_ul_iab_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_nr_s::ch_bw_ul_iab_r16_c_::types");
}

const char* band_nr_s::max_mod_order_for_multicast_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-r17", "fr2-r17"};
  return convert_enum_idx(names, 2, value, "band_nr_s::max_mod_order_for_multicast_r17_c_::types");
}
uint8_t band_nr_s::max_mod_order_for_multicast_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_nr_s::max_mod_order_for_multicast_r17_c_::types");
}

const char* band_sidelink_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-r16", "fr2-r16"};
  return convert_enum_idx(
      names, 2, value, "band_sidelink_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types");
}
uint8_t band_sidelink_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "band_sidelink_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types");
}

const char*
band_sidelink_r16_s::sl_tx_mode1_r16_s_::scs_cp_pattern_tx_sidelink_mode_one_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"fr1-r16", "fr2-r16"};
  return convert_enum_idx(
      names, 2, value, "band_sidelink_r16_s::sl_tx_mode1_r16_s_::scs_cp_pattern_tx_sidelink_mode_one_r16_c_::types");
}
uint8_t
band_sidelink_r16_s::sl_tx_mode1_r16_s_::scs_cp_pattern_tx_sidelink_mode_one_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "band_sidelink_r16_s::sl_tx_mode1_r16_s_::scs_cp_pattern_tx_sidelink_mode_one_r16_c_::types");
}

const char* band_sidelink_r16_s::sl_tx_mode2_random_res_sel_r17_s_::scs_cp_pattern_tx_sidelink_mode_two_r17_c_::
    types_opts::to_string() const
{
  static const char* names[] = {"fr1-r17", "fr2-r17"};
  return convert_enum_idx(
      names,
      2,
      value,
      "band_sidelink_r16_s::sl_tx_mode2_random_res_sel_r17_s_::scs_cp_pattern_tx_sidelink_mode_two_r17_c_::types");
}
uint8_t band_sidelink_r16_s::sl_tx_mode2_random_res_sel_r17_s_::scs_cp_pattern_tx_sidelink_mode_two_r17_c_::types_opts::
    to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers,
      2,
      value,
      "band_sidelink_r16_s::sl_tx_mode2_random_res_sel_r17_s_::scs_cp_pattern_tx_sidelink_mode_two_r17_c_::types");
}

const char* carrier_state_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"deActivated-r17", "activeBWP-r17"};
  return convert_enum_idx(names, 2, value, "carrier_state_r17_c::types");
}

const char* cfra_s::res_c_::types_opts::to_string() const
{
  static const char* names[] = {"ssb", "csirs"};
  return convert_enum_idx(names, 2, value, "cfra_s::res_c_::types");
}

// CG-SDT-ConfigLCH-Restriction-r17 ::= SEQUENCE
SRSASN_CODE cg_sdt_cfg_lch_restrict_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cfg_grant_type1_allowed_r17_present, 1));
  HANDLE_CODE(bref.pack(allowed_cg_list_r17_present, 1));

  HANDLE_CODE(pack_integer(bref, lc_ch_id_r17, (uint8_t)1u, (uint8_t)32u));
  if (allowed_cg_list_r17_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_cg_list_r17, 0, 31, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_sdt_cfg_lch_restrict_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cfg_grant_type1_allowed_r17_present, 1));
  HANDLE_CODE(bref.unpack(allowed_cg_list_r17_present, 1));

  HANDLE_CODE(unpack_integer(lc_ch_id_r17, bref, (uint8_t)1u, (uint8_t)32u));
  if (allowed_cg_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_cg_list_r17, bref, 0, 31, integer_packer<uint8_t>(0, 31)));
  }

  return SRSASN_SUCCESS;
}
void cg_sdt_cfg_lch_restrict_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("logicalChannelIdentity-r17", lc_ch_id_r17);
  if (cfg_grant_type1_allowed_r17_present) {
    j.write_str("configuredGrantType1Allowed-r17", "true");
  }
  if (allowed_cg_list_r17_present) {
    j.start_array("allowedCG-List-r17");
    for (const auto& e1 : allowed_cg_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.end_obj();
}

const char* csi_associated_report_cfg_info_s::res_for_ch_c_::types_opts::to_string() const
{
  static const char* names[] = {"nzp-CSI-RS", "csi-SSB-ResourceSet"};
  return convert_enum_idx(names, 2, value, "csi_associated_report_cfg_info_s::res_for_ch_c_::types");
}

const char* csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"nzp-CSI-RS2-r17", "csi-SSB-ResourceSet2-r17"};
  return convert_enum_idx(names, 2, value, "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types");
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

const char* codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"typeII", "typeII-PortSelection"};
  return convert_enum_idx(names, 2, value, "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types");
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

const char* csi_report_cfg_s::report_cfg_type_c_::types_opts::to_string() const
{
  static const char* names[] = {"periodic", "semiPersistentOnPUCCH", "semiPersistentOnPUSCH", "aperiodic"};
  return convert_enum_idx(names, 4, value, "csi_report_cfg_s::report_cfg_type_c_::types");
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

const char* csi_report_cfg_s::group_based_beam_report_c_::types_opts::to_string() const
{
  static const char* names[] = {"enabled", "disabled"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::group_based_beam_report_c_::types");
}

const char* csi_report_cfg_s::report_quant_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"cri-SINR-r16", "ssb-Index-SINR-r16"};
  return convert_enum_idx(names, 2, value, "csi_report_cfg_s::report_quant_r16_c_::types");
}

const char* csi_report_cfg_s::report_quant_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "cri-RSRP-Index-r17", "ssb-Index-RSRP-Index-r17", "cri-SINR-Index-r17", "ssb-Index-SINR-Index-r17"};
  return convert_enum_idx(names, 4, value, "csi_report_cfg_s::report_quant_r17_c_::types");
}

const char* csi_res_cfg_s::csi_rs_res_set_list_c_::types_opts::to_string() const
{
  static const char* names[] = {"nzp-CSI-RS-SSB", "csi-IM-ResourceSetList"};
  return convert_enum_idx(names, 2, value, "csi_res_cfg_s::csi_rs_res_set_list_c_::types");
}

const char* dl_prs_qcl_info_r17_c::types_opts::to_string() const
{
  static const char* names[] = {"ssb-r17", "dl-PRS-r17"};
  return convert_enum_idx(names, 2, value, "dl_prs_qcl_info_r17_c::types");
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

const char* srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types_opts::to_string() const
{
  static const char* names[] = {"typeA", "typeB"};
  return convert_enum_idx(names, 2, value, "srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types");
}

const char* ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
  return convert_enum_idx(names, 3, value, "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types");
}

const char* tdd_ul_dl_slot_cfg_s::symbols_c_::types_opts::to_string() const
{
  static const char* names[] = {"allDownlink", "allUplink", "explicit"};
  return convert_enum_idx(names, 3, value, "tdd_ul_dl_slot_cfg_s::symbols_c_::types");
}

const char* tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"allDownlink-r16", "allUplink-r16", "explicit-r16", "explicit-IAB-MT-r16"};
  return convert_enum_idx(names, 4, value, "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types");
}

const char* ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"maxEnergyDetectionThreshold-r16", "energyDetectionThresholdOffset-r16"};
  return convert_enum_idx(names, 2, value, "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types");
}

const char* cross_carrier_sched_cfg_s::sched_cell_info_c_::types_opts::to_string() const
{
  static const char* names[] = {"own", "other"};
  return convert_enum_idx(names, 2, value, "cross_carrier_sched_cfg_s::sched_cell_info_c_::types");
}

const char* serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types_opts::to_string() const
{
  static const char* names[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
  return convert_enum_idx(names, 3, value, "serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types");
}

const char* serving_cell_cfg_common_s::ch_access_mode_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"dynamic", "semiStatic"};
  return convert_enum_idx(names, 2, value, "serving_cell_cfg_common_s::ch_access_mode_r16_c_::types");
}

const char* drx_cfg_s::drx_on_dur_timer_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "drx_cfg_s::drx_on_dur_timer_c_::types");
}

const char* drx_cfg_s::drx_long_cycle_start_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "drx_cfg_s::drx_long_cycle_start_offset_c_::types");
}
uint16_t drx_cfg_s::drx_long_cycle_start_offset_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "drx_cfg_s::drx_long_cycle_start_offset_c_::types");
}

const char* drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types");
}

const char* mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"g-RNTI", "g-CS-RNTI"};
  return convert_enum_idx(names, 2, value, "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types");
}

const char* pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"perCC", "perHARQ"};
  return convert_enum_idx(names, 2, value, "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types");
}

const char* recfg_with_sync_s::rach_cfg_ded_c_::types_opts::to_string() const
{
  static const char* names[] = {"uplink", "supplementaryUplink"};
  return convert_enum_idx(names, 2, value, "recfg_with_sync_s::rach_cfg_ded_c_::types");
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

const char* rlc_bearer_cfg_s::served_radio_bearer_c_::types_opts::to_string() const
{
  static const char* names[] = {"srb-Identity", "drb-Identity"};
  return convert_enum_idx(names, 2, value, "rlc_bearer_cfg_s::served_radio_bearer_c_::types");
}

// CellGrouping-r16 ::= SEQUENCE
SRSASN_CODE cell_grouping_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, mcg_r16, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(pack_dyn_seq_of(bref, scg_r16, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(mode_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_grouping_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(mcg_r16, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(unpack_dyn_seq_of(scg_r16, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  HANDLE_CODE(mode_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_grouping_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("mcg-r16");
  for (const auto& e1 : mcg_r16) {
    j.write_int(e1);
  }
  j.end_array();
  j.start_array("scg-r16");
  for (const auto& e1 : scg_r16) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_str("mode-r16", mode_r16.to_string());
  j.end_obj();
}

const char* cell_grouping_r16_s::mode_r16_opts::to_string() const
{
  static const char* names[] = {"sync", "async"};
  return convert_enum_idx(names, 2, value, "cell_grouping_r16_s::mode_r16_e_");
}

const char* feature_set_c::types_opts::to_string() const
{
  static const char* names[] = {"eutra", "nr"};
  return convert_enum_idx(names, 2, value, "feature_set_c::types");
}

const char* supported_bw_c::types_opts::to_string() const
{
  static const char* names[] = {"fr1", "fr2"};
  return convert_enum_idx(names, 2, value, "supported_bw_c::types");
}
uint8_t supported_bw_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "supported_bw_c::types");
}

const char* supported_bw_v1700_c::types_opts::to_string() const
{
  static const char* names[] = {"fr1-r17", "fr2-r17"};
  return convert_enum_idx(names, 2, value, "supported_bw_v1700_c::types");
}
uint8_t supported_bw_v1700_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "supported_bw_v1700_c::types");
}

const char* freq_band_info_c::types_opts::to_string() const
{
  static const char* names[] = {"bandInformationEUTRA", "bandInformationNR"};
  return convert_enum_idx(names, 2, value, "freq_band_info_c::types");
}

const char* meas_and_mob_params_common_s::concurrent_meas_gap_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"concurrentPerUE-OnlyMeasGap-r17", "concurrentPerUE-PerFRCombMeasGap-r17"};
  return convert_enum_idx(names, 2, value, "meas_and_mob_params_common_s::concurrent_meas_gap_r17_c_::types");
}

// MeasAndMobParametersMRDC-XDD-Diff-v1560 ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_xdd_diff_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sftd_meas_pscell_nedc_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_xdd_diff_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sftd_meas_pscell_nedc_present, 1));

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_xdd_diff_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sftd_meas_pscell_nedc_present) {
    j.write_str("sftd-MeasPSCell-NEDC", "supported");
  }
  j.end_obj();
}

// MeasAndMobParametersMRDC-v1560 ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    j.write_fieldname("measAndMobParametersMRDC-XDD-Diff-v1560");
    meas_and_mob_params_mrdc_xdd_diff_v1560.to_json(j);
  }
  j.end_obj();
}

// MeasAndMobParametersMRDC-Common-v1730 ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_common_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(independent_gap_cfg_max_cc_r17.fr1_only_r17_present, 1));
  HANDLE_CODE(bref.pack(independent_gap_cfg_max_cc_r17.fr2_only_r17_present, 1));
  HANDLE_CODE(bref.pack(independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17_present, 1));
  if (independent_gap_cfg_max_cc_r17.fr1_only_r17_present) {
    HANDLE_CODE(pack_integer(bref, independent_gap_cfg_max_cc_r17.fr1_only_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (independent_gap_cfg_max_cc_r17.fr2_only_r17_present) {
    HANDLE_CODE(pack_integer(bref, independent_gap_cfg_max_cc_r17.fr2_only_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17_present) {
    HANDLE_CODE(pack_integer(bref, independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_common_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(independent_gap_cfg_max_cc_r17.fr1_only_r17_present, 1));
  HANDLE_CODE(bref.unpack(independent_gap_cfg_max_cc_r17.fr2_only_r17_present, 1));
  HANDLE_CODE(bref.unpack(independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17_present, 1));
  if (independent_gap_cfg_max_cc_r17.fr1_only_r17_present) {
    HANDLE_CODE(unpack_integer(independent_gap_cfg_max_cc_r17.fr1_only_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (independent_gap_cfg_max_cc_r17.fr2_only_r17_present) {
    HANDLE_CODE(unpack_integer(independent_gap_cfg_max_cc_r17.fr2_only_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17_present) {
    HANDLE_CODE(unpack_integer(independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_common_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("independentGapConfig-maxCC-r17");
  j.start_obj();
  if (independent_gap_cfg_max_cc_r17.fr1_only_r17_present) {
    j.write_int("fr1-Only-r17", independent_gap_cfg_max_cc_r17.fr1_only_r17);
  }
  if (independent_gap_cfg_max_cc_r17.fr2_only_r17_present) {
    j.write_int("fr2-Only-r17", independent_gap_cfg_max_cc_r17.fr2_only_r17);
  }
  if (independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17_present) {
    j.write_int("fr1-AndFR2-r17", independent_gap_cfg_max_cc_r17.fr1_and_fr2_r17);
  }
  j.end_obj();
  j.end_obj();
}

// MeasAndMobParametersMRDC-v1730 ::= SEQUENCE
SRSASN_CODE meas_and_mob_params_mrdc_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_common_v1730_present, 1));

  if (meas_and_mob_params_mrdc_common_v1730_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_common_v1730.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_and_mob_params_mrdc_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_common_v1730_present, 1));

  if (meas_and_mob_params_mrdc_common_v1730_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_common_v1730.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_and_mob_params_mrdc_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_common_v1730_present) {
    j.write_fieldname("measAndMobParametersMRDC-Common-v1730");
    meas_and_mob_params_mrdc_common_v1730.to_json(j);
  }
  j.end_obj();
}

// MeasResultSCG-Failure ::= SEQUENCE
SRSASN_CODE meas_result_scg_fail_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_per_mo_list, 1, 8));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= location_info_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(location_info_r16.is_present(), 1));
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_scg_fail_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_per_mo_list, bref, 1, 8));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(location_info_r16, bref);
      if (location_info_r16.is_present()) {
        HANDLE_CODE(location_info_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_scg_fail_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measResultPerMOList");
  for (const auto& e1 : meas_result_per_mo_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (location_info_r16.is_present()) {
      j.write_fieldname("locationInfo-r16");
      location_info_r16->to_json(j);
    }
  }
  j.end_obj();
}

const char* phy_params_common_s::bwp_switching_multi_ccs_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"type1-r16", "type2-r16"};
  return convert_enum_idx(names, 2, value, "phy_params_common_s::bwp_switching_multi_ccs_r16_c_::types");
}
uint8_t phy_params_common_s::bwp_switching_multi_ccs_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "phy_params_common_s::bwp_switching_multi_ccs_r16_c_::types");
}

const char* phy_params_common_s::bwp_switching_multi_dormancy_ccs_r16_c_::types_opts::to_string() const
{
  static const char* names[] = {"type1-r16", "type2-r16"};
  return convert_enum_idx(names, 2, value, "phy_params_common_s::bwp_switching_multi_dormancy_ccs_r16_c_::types");
}
uint8_t phy_params_common_s::bwp_switching_multi_dormancy_ccs_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "phy_params_common_s::bwp_switching_multi_dormancy_ccs_r16_c_::types");
}

// Phy-ParametersCommon-v16a0 ::= SEQUENCE
SRSASN_CODE phy_params_common_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_periodicity_and_offset_ext_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_common_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_periodicity_and_offset_ext_r16_present, 1));

  return SRSASN_SUCCESS;
}
void phy_params_common_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_periodicity_and_offset_ext_r16_present) {
    j.write_str("srs-PeriodicityAndOffsetExt-r16", "supported");
  }
  j.end_obj();
}

// Phy-Parameters-v16a0 ::= SEQUENCE
SRSASN_CODE phy_params_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_params_common_v16a0_present, 1));

  if (phy_params_common_v16a0_present) {
    HANDLE_CODE(phy_params_common_v16a0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_params_common_v16a0_present, 1));

  if (phy_params_common_v16a0_present) {
    HANDLE_CODE(phy_params_common_v16a0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void phy_params_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_params_common_v16a0_present) {
    j.write_fieldname("phy-ParametersCommon-v16a0");
    phy_params_common_v16a0.to_json(j);
  }
  j.end_obj();
}

// NAICS-Capability-Entry ::= SEQUENCE
SRSASN_CODE naics_cap_entry_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, nof_naics_capable_cc, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(nof_aggr_prb.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE naics_cap_entry_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(nof_naics_capable_cc, bref, (uint8_t)1u, (uint8_t)5u));
  HANDLE_CODE(nof_aggr_prb.unpack(bref));

  return SRSASN_SUCCESS;
}
void naics_cap_entry_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("numberOfNAICS-CapableCC", nof_naics_capable_cc);
  j.write_str("numberOfAggregatedPRB", nof_aggr_prb.to_string());
  j.end_obj();
}

const char* naics_cap_entry_s::nof_aggr_prb_opts::to_string() const
{
  static const char* names[] = {"n50",
                                "n75",
                                "n100",
                                "n125",
                                "n150",
                                "n175",
                                "n200",
                                "n225",
                                "n250",
                                "n275",
                                "n300",
                                "n350",
                                "n400",
                                "n450",
                                "n500",
                                "spare"};
  return convert_enum_idx(names, 16, value, "naics_cap_entry_s::nof_aggr_prb_e_");
}
uint16_t naics_cap_entry_s::nof_aggr_prb_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500};
  return map_enum_number(numbers, 15, value, "naics_cap_entry_s::nof_aggr_prb_e_");
}

// Phy-ParametersMRDC ::= SEQUENCE
SRSASN_CODE phy_params_mrdc_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(naics_cap_list.size() > 0, 1));

  if (naics_cap_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, naics_cap_list, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sp_cell_placement.is_present();
    group_flags[1] |= tdd_pcell_ul_tx_all_ul_sf_r16_present;
    group_flags[1] |= fdd_pcell_ul_tx_all_ul_sf_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sp_cell_placement.is_present(), 1));
      if (sp_cell_placement.is_present()) {
        HANDLE_CODE(sp_cell_placement->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tdd_pcell_ul_tx_all_ul_sf_r16_present, 1));
      HANDLE_CODE(bref.pack(fdd_pcell_ul_tx_all_ul_sf_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_params_mrdc_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool naics_cap_list_present;
  HANDLE_CODE(bref.unpack(naics_cap_list_present, 1));

  if (naics_cap_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(naics_cap_list, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sp_cell_placement, bref);
      if (sp_cell_placement.is_present()) {
        HANDLE_CODE(sp_cell_placement->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(tdd_pcell_ul_tx_all_ul_sf_r16_present, 1));
      HANDLE_CODE(bref.unpack(fdd_pcell_ul_tx_all_ul_sf_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void phy_params_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (naics_cap_list.size() > 0) {
    j.start_array("naics-Capability-List");
    for (const auto& e1 : naics_cap_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (sp_cell_placement.is_present()) {
      j.write_fieldname("spCellPlacement");
      sp_cell_placement->to_json(j);
    }
    if (tdd_pcell_ul_tx_all_ul_sf_r16_present) {
      j.write_str("tdd-PCellUL-TX-AllUL-Subframe-r16", "supported");
    }
    if (fdd_pcell_ul_tx_all_ul_sf_r16_present) {
      j.write_str("fdd-PCellUL-TX-AllUL-Subframe-r16", "supported");
    }
  }
  j.end_obj();
}

// RF-Parameters-v15g0 ::= SEQUENCE
SRSASN_CODE rf_params_v15g0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_list_v15g0.size() > 0, 1));

  if (supported_band_combination_list_v15g0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_v15g0, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v15g0_s::unpack(cbit_ref& bref)
{
  bool supported_band_combination_list_v15g0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_v15g0_present, 1));

  if (supported_band_combination_list_v15g0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_v15g0, bref, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v15g0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_list_v15g0.size() > 0) {
    j.start_array("supportedBandCombinationList-v15g0");
    for (const auto& e1 : supported_band_combination_list_v15g0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RF-Parameters-v16a0 ::= SEQUENCE
SRSASN_CODE rf_params_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_list_v16a0.size() > 0, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v16a0.size() > 0, 1));

  if (supported_band_combination_list_v16a0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_v16a0, 1, 65536));
  }
  if (supported_band_combination_list_ul_tx_switch_v16a0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_ul_tx_switch_v16a0, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v16a0_s::unpack(cbit_ref& bref)
{
  bool supported_band_combination_list_v16a0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_v16a0_present, 1));
  bool supported_band_combination_list_ul_tx_switch_v16a0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_ul_tx_switch_v16a0_present, 1));

  if (supported_band_combination_list_v16a0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_v16a0, bref, 1, 65536));
  }
  if (supported_band_combination_list_ul_tx_switch_v16a0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_ul_tx_switch_v16a0, bref, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
void rf_params_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_list_v16a0.size() > 0) {
    j.start_array("supportedBandCombinationList-v16a0");
    for (const auto& e1 : supported_band_combination_list_v16a0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_list_ul_tx_switch_v16a0.size() > 0) {
    j.start_array("supportedBandCombinationList-UplinkTxSwitch-v16a0");
    for (const auto& e1 : supported_band_combination_list_ul_tx_switch_v16a0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BandNR-v16c0 ::= SEQUENCE
SRSASN_CODE band_nr_v16c0_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(pusch_repeat_type_a_v16c0_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_nr_v16c0_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(pusch_repeat_type_a_v16c0_present, 1));

  return SRSASN_SUCCESS;
}
void band_nr_v16c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pusch_repeat_type_a_v16c0_present) {
    j.write_str("pusch-RepetitionTypeA-v16c0", "supported");
  }
  j.end_obj();
}

// RF-Parameters-v16c0 ::= SEQUENCE
SRSASN_CODE rf_params_v16c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_nr_v16c0, 1, 1024));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_v16c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_nr_v16c0, bref, 1, 1024));

  return SRSASN_SUCCESS;
}
void rf_params_v16c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("supportedBandListNR-v16c0");
  for (const auto& e1 : supported_band_list_nr_v16c0) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// RF-ParametersMRDC ::= SEQUENCE
SRSASN_CODE rf_params_mrdc_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(supported_band_combination_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(applied_freq_band_list_filt.size() > 0, 1));

  if (supported_band_combination_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list, 1, 65536));
  }
  if (applied_freq_band_list_filt.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, applied_freq_band_list_filt, 1, 1280));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= srs_switching_time_requested_present;
    group_flags[0] |= supported_band_combination_list_v1540.is_present();
    group_flags[1] |= supported_band_combination_list_v1550.is_present();
    group_flags[2] |= supported_band_combination_list_v1560.is_present();
    group_flags[2] |= supported_band_combination_list_nedc_only.is_present();
    group_flags[3] |= supported_band_combination_list_v1570.is_present();
    group_flags[4] |= supported_band_combination_list_v1580.is_present();
    group_flags[5] |= supported_band_combination_list_v1590.is_present();
    group_flags[6] |= supported_band_combination_list_nedc_only_v15a0.is_present();
    group_flags[7] |= supported_band_combination_list_v1610.is_present();
    group_flags[7] |= supported_band_combination_list_nedc_only_v1610.is_present();
    group_flags[7] |= supported_band_combination_list_ul_tx_switch_r16.is_present();
    group_flags[8] |= supported_band_combination_list_v1630.is_present();
    group_flags[8] |= supported_band_combination_list_nedc_only_v1630.is_present();
    group_flags[8] |= supported_band_combination_list_ul_tx_switch_v1630.is_present();
    group_flags[9] |= supported_band_combination_list_v1640.is_present();
    group_flags[9] |= supported_band_combination_list_nedc_only_v1640.is_present();
    group_flags[9] |= supported_band_combination_list_ul_tx_switch_v1640.is_present();
    group_flags[10] |= supported_band_combination_list_ul_tx_switch_v1670.is_present();
    group_flags[11] |= supported_band_combination_list_v1700.is_present();
    group_flags[11] |= supported_band_combination_list_ul_tx_switch_v1700.is_present();
    group_flags[12] |= supported_band_combination_list_v1720.is_present();
    group_flags[12] |= supported_band_combination_list_nedc_only_v1720.is_present();
    group_flags[12] |= supported_band_combination_list_ul_tx_switch_v1720.is_present();
    group_flags[13] |= supported_band_combination_list_v1730.is_present();
    group_flags[13] |= supported_band_combination_list_nedc_only_v1730.is_present();
    group_flags[13] |= supported_band_combination_list_ul_tx_switch_v1730.is_present();
    group_flags[14] |= supported_band_combination_list_v1740.is_present();
    group_flags[14] |= supported_band_combination_list_nedc_only_v1740.is_present();
    group_flags[14] |= supported_band_combination_list_ul_tx_switch_v1740.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(srs_switching_time_requested_present, 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_v1540.is_present(), 1));
      if (supported_band_combination_list_v1540.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1540, 1, 65536));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1550.is_present(), 1));
      if (supported_band_combination_list_v1550.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1550, 1, 65536));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1560.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only.is_present(), 1));
      if (supported_band_combination_list_v1560.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1560, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_nedc_only, 1, 65536));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1570.is_present(), 1));
      if (supported_band_combination_list_v1570.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1570, 1, 65536));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1580.is_present(), 1));
      if (supported_band_combination_list_v1580.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1580, 1, 65536));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1590.is_present(), 1));
      if (supported_band_combination_list_v1590.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1590, 1, 65536));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v15a0.is_present(), 1));
      if (supported_band_combination_list_nedc_only_v15a0.is_present()) {
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590.size() > 0, 1));
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590, 1, 65536));
        }
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v1610.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_r16.is_present(), 1));
      if (supported_band_combination_list_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1610, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1610.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_nedc_only_v1610, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_r16, 1, 65536));
      }
    }
    if (group_flags[8]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1630.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v1630.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v1630.is_present(), 1));
      if (supported_band_combination_list_v1630.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1630, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1630.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_nedc_only_v1630, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1630.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_v1630, 1, 65536));
      }
    }
    if (group_flags[9]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1640.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v1640.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v1640.is_present(), 1));
      if (supported_band_combination_list_v1640.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1640, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1640.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_nedc_only_v1640, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1640.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_v1640, 1, 65536));
      }
    }
    if (group_flags[10]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v1670.is_present(), 1));
      if (supported_band_combination_list_ul_tx_switch_v1670.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_v1670, 1, 65536));
      }
    }
    if (group_flags[11]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1700.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v1700.is_present(), 1));
      if (supported_band_combination_list_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1700, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1700.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_v1700, 1, 65536));
      }
    }
    if (group_flags[12]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1720.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v1720.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v1720.is_present(), 1));
      if (supported_band_combination_list_v1720.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1720, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1720.is_present()) {
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1700.size() > 0, 1));
        HANDLE_CODE(bref.pack(
            supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1720.size() > 0, 1));
        if (supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1700.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1700, 1, 65536));
        }
        if (supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1720.size() > 0) {
          HANDLE_CODE(pack_dyn_seq_of(
              bref, supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1720, 1, 65536));
        }
      }
      if (supported_band_combination_list_ul_tx_switch_v1720.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_v1720, 1, 65536));
      }
    }
    if (group_flags[13]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1730.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v1730.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v1730.is_present(), 1));
      if (supported_band_combination_list_v1730.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1730, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1730.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_nedc_only_v1730, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1730.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_v1730, 1, 65536));
      }
    }
    if (group_flags[14]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(supported_band_combination_list_v1740.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v1740.is_present(), 1));
      HANDLE_CODE(bref.pack(supported_band_combination_list_ul_tx_switch_v1740.is_present(), 1));
      if (supported_band_combination_list_v1740.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_v1740, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1740.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_nedc_only_v1740, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1740.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *supported_band_combination_list_ul_tx_switch_v1740, 1, 65536));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_mrdc_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool supported_band_combination_list_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_present, 1));
  bool applied_freq_band_list_filt_present;
  HANDLE_CODE(bref.unpack(applied_freq_band_list_filt_present, 1));

  if (supported_band_combination_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list, bref, 1, 65536));
  }
  if (applied_freq_band_list_filt_present) {
    HANDLE_CODE(unpack_dyn_seq_of(applied_freq_band_list_filt, bref, 1, 1280));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(15);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(srs_switching_time_requested_present, 1));
      unpack_presence_flag(supported_band_combination_list_v1540, bref);
      if (supported_band_combination_list_v1540.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1540, bref, 1, 65536));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1550, bref);
      if (supported_band_combination_list_v1550.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1550, bref, 1, 65536));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1560, bref);
      unpack_presence_flag(supported_band_combination_list_nedc_only, bref);
      if (supported_band_combination_list_v1560.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1560, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_nedc_only, bref, 1, 65536));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1570, bref);
      if (supported_band_combination_list_v1570.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1570, bref, 1, 65536));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1580, bref);
      if (supported_band_combination_list_v1580.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1580, bref, 1, 65536));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1590, bref);
      if (supported_band_combination_list_v1590.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1590, bref, 1, 65536));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_nedc_only_v15a0, bref);
      if (supported_band_combination_list_nedc_only_v15a0.is_present()) {
        bool supported_band_combination_list_v1540_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1540_present, 1));
        bool supported_band_combination_list_v1560_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1560_present, 1));
        bool supported_band_combination_list_v1570_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1570_present, 1));
        bool supported_band_combination_list_v1580_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1580_present, 1));
        bool supported_band_combination_list_v1590_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1590_present, 1));
        if (supported_band_combination_list_v1540_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1560_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1570_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1580_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1590_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590, bref, 1, 65536));
        }
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1610, bref);
      unpack_presence_flag(supported_band_combination_list_nedc_only_v1610, bref);
      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_r16, bref);
      if (supported_band_combination_list_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1610, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1610.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_nedc_only_v1610, bref, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_r16, bref, 1, 65536));
      }
    }
    if (group_flags[8]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1630, bref);
      unpack_presence_flag(supported_band_combination_list_nedc_only_v1630, bref);
      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_v1630, bref);
      if (supported_band_combination_list_v1630.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1630, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1630.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_nedc_only_v1630, bref, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1630.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_v1630, bref, 1, 65536));
      }
    }
    if (group_flags[9]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1640, bref);
      unpack_presence_flag(supported_band_combination_list_nedc_only_v1640, bref);
      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_v1640, bref);
      if (supported_band_combination_list_v1640.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1640, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1640.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_nedc_only_v1640, bref, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1640.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_v1640, bref, 1, 65536));
      }
    }
    if (group_flags[10]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_v1670, bref);
      if (supported_band_combination_list_ul_tx_switch_v1670.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_v1670, bref, 1, 65536));
      }
    }
    if (group_flags[11]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1700, bref);
      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_v1700, bref);
      if (supported_band_combination_list_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1700, bref, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1700.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_v1700, bref, 1, 65536));
      }
    }
    if (group_flags[12]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1720, bref);
      unpack_presence_flag(supported_band_combination_list_nedc_only_v1720, bref);
      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_v1720, bref);
      if (supported_band_combination_list_v1720.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1720, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1720.is_present()) {
        bool supported_band_combination_list_v1700_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1700_present, 1));
        bool supported_band_combination_list_v1720_present;
        HANDLE_CODE(bref.unpack(supported_band_combination_list_v1720_present, 1));
        if (supported_band_combination_list_v1700_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1700, bref, 1, 65536));
        }
        if (supported_band_combination_list_v1720_present) {
          HANDLE_CODE(unpack_dyn_seq_of(
              supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1720, bref, 1, 65536));
        }
      }
      if (supported_band_combination_list_ul_tx_switch_v1720.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_v1720, bref, 1, 65536));
      }
    }
    if (group_flags[13]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1730, bref);
      unpack_presence_flag(supported_band_combination_list_nedc_only_v1730, bref);
      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_v1730, bref);
      if (supported_band_combination_list_v1730.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1730, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1730.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_nedc_only_v1730, bref, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1730.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_v1730, bref, 1, 65536));
      }
    }
    if (group_flags[14]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(supported_band_combination_list_v1740, bref);
      unpack_presence_flag(supported_band_combination_list_nedc_only_v1740, bref);
      unpack_presence_flag(supported_band_combination_list_ul_tx_switch_v1740, bref);
      if (supported_band_combination_list_v1740.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_v1740, bref, 1, 65536));
      }
      if (supported_band_combination_list_nedc_only_v1740.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_nedc_only_v1740, bref, 1, 65536));
      }
      if (supported_band_combination_list_ul_tx_switch_v1740.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*supported_band_combination_list_ul_tx_switch_v1740, bref, 1, 65536));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rf_params_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_list.size() > 0) {
    j.start_array("supportedBandCombinationList");
    for (const auto& e1 : supported_band_combination_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (applied_freq_band_list_filt.size() > 0) {
    j.start_array("appliedFreqBandListFilter");
    for (const auto& e1 : applied_freq_band_list_filt) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (srs_switching_time_requested_present) {
      j.write_str("srs-SwitchingTimeRequested", "true");
    }
    if (supported_band_combination_list_v1540.is_present()) {
      j.start_array("supportedBandCombinationList-v1540");
      for (const auto& e1 : *supported_band_combination_list_v1540) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1550.is_present()) {
      j.start_array("supportedBandCombinationList-v1550");
      for (const auto& e1 : *supported_band_combination_list_v1550) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1560.is_present()) {
      j.start_array("supportedBandCombinationList-v1560");
      for (const auto& e1 : *supported_band_combination_list_v1560) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only.is_present()) {
      j.start_array("supportedBandCombinationListNEDC-Only");
      for (const auto& e1 : *supported_band_combination_list_nedc_only) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1570.is_present()) {
      j.start_array("supportedBandCombinationList-v1570");
      for (const auto& e1 : *supported_band_combination_list_v1570) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1580.is_present()) {
      j.start_array("supportedBandCombinationList-v1580");
      for (const auto& e1 : *supported_band_combination_list_v1580) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1590.is_present()) {
      j.start_array("supportedBandCombinationList-v1590");
      for (const auto& e1 : *supported_band_combination_list_v1590) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v15a0.is_present()) {
      j.write_fieldname("supportedBandCombinationListNEDC-Only-v15a0");
      j.start_obj();
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540.size() > 0) {
        j.start_array("supportedBandCombinationList-v1540");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1540) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560.size() > 0) {
        j.start_array("supportedBandCombinationList-v1560");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1560) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570.size() > 0) {
        j.start_array("supportedBandCombinationList-v1570");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1570) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580.size() > 0) {
        j.start_array("supportedBandCombinationList-v1580");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1580) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590.size() > 0) {
        j.start_array("supportedBandCombinationList-v1590");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v15a0->supported_band_combination_list_v1590) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (supported_band_combination_list_v1610.is_present()) {
      j.start_array("supportedBandCombinationList-v1610");
      for (const auto& e1 : *supported_band_combination_list_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v1610.is_present()) {
      j.start_array("supportedBandCombinationListNEDC-Only-v1610");
      for (const auto& e1 : *supported_band_combination_list_nedc_only_v1610) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_ul_tx_switch_r16.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-r16");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1630.is_present()) {
      j.start_array("supportedBandCombinationList-v1630");
      for (const auto& e1 : *supported_band_combination_list_v1630) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v1630.is_present()) {
      j.start_array("supportedBandCombinationListNEDC-Only-v1630");
      for (const auto& e1 : *supported_band_combination_list_nedc_only_v1630) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_ul_tx_switch_v1630.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-v1630");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_v1630) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1640.is_present()) {
      j.start_array("supportedBandCombinationList-v1640");
      for (const auto& e1 : *supported_band_combination_list_v1640) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v1640.is_present()) {
      j.start_array("supportedBandCombinationListNEDC-Only-v1640");
      for (const auto& e1 : *supported_band_combination_list_nedc_only_v1640) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_ul_tx_switch_v1640.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-v1640");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_v1640) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_ul_tx_switch_v1670.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-v1670");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_v1670) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1700.is_present()) {
      j.start_array("supportedBandCombinationList-v1700");
      for (const auto& e1 : *supported_band_combination_list_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_ul_tx_switch_v1700.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-v1700");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_v1700) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1720.is_present()) {
      j.start_array("supportedBandCombinationList-v1720");
      for (const auto& e1 : *supported_band_combination_list_v1720) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v1720.is_present()) {
      j.write_fieldname("supportedBandCombinationListNEDC-Only-v1720");
      j.start_obj();
      if (supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1700.size() > 0) {
        j.start_array("supportedBandCombinationList-v1700");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1700) {
          e1.to_json(j);
        }
        j.end_array();
      }
      if (supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1720.size() > 0) {
        j.start_array("supportedBandCombinationList-v1720");
        for (const auto& e1 : supported_band_combination_list_nedc_only_v1720->supported_band_combination_list_v1720) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (supported_band_combination_list_ul_tx_switch_v1720.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-v1720");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_v1720) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1730.is_present()) {
      j.start_array("supportedBandCombinationList-v1730");
      for (const auto& e1 : *supported_band_combination_list_v1730) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v1730.is_present()) {
      j.start_array("supportedBandCombinationListNEDC-Only-v1730");
      for (const auto& e1 : *supported_band_combination_list_nedc_only_v1730) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_ul_tx_switch_v1730.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-v1730");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_v1730) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_v1740.is_present()) {
      j.start_array("supportedBandCombinationList-v1740");
      for (const auto& e1 : *supported_band_combination_list_v1740) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_nedc_only_v1740.is_present()) {
      j.start_array("supportedBandCombinationListNEDC-Only-v1740");
      for (const auto& e1 : *supported_band_combination_list_nedc_only_v1740) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (supported_band_combination_list_ul_tx_switch_v1740.is_present()) {
      j.start_array("supportedBandCombinationList-UplinkTxSwitch-v1740");
      for (const auto& e1 : *supported_band_combination_list_ul_tx_switch_v1740) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// RF-ParametersMRDC-v15g0 ::= SEQUENCE
SRSASN_CODE rf_params_mrdc_v15g0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_combination_list_v15g0.size() > 0, 1));
  HANDLE_CODE(bref.pack(supported_band_combination_list_nedc_only_v15g0.size() > 0, 1));

  if (supported_band_combination_list_v15g0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_v15g0, 1, 65536));
  }
  if (supported_band_combination_list_nedc_only_v15g0.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_combination_list_nedc_only_v15g0, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rf_params_mrdc_v15g0_s::unpack(cbit_ref& bref)
{
  bool supported_band_combination_list_v15g0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_v15g0_present, 1));
  bool supported_band_combination_list_nedc_only_v15g0_present;
  HANDLE_CODE(bref.unpack(supported_band_combination_list_nedc_only_v15g0_present, 1));

  if (supported_band_combination_list_v15g0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_v15g0, bref, 1, 65536));
  }
  if (supported_band_combination_list_nedc_only_v15g0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_combination_list_nedc_only_v15g0, bref, 1, 65536));
  }

  return SRSASN_SUCCESS;
}
void rf_params_mrdc_v15g0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_combination_list_v15g0.size() > 0) {
    j.start_array("supportedBandCombinationList-v15g0");
    for (const auto& e1 : supported_band_combination_list_v15g0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (supported_band_combination_list_nedc_only_v15g0.size() > 0) {
    j.start_array("supportedBandCombinationListNEDC-Only-v15g0");
    for (const auto& e1 : supported_band_combination_list_nedc_only_v15g0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// CG-SDT-TA-ValidationConfig-r17 ::= SEQUENCE
SRSASN_CODE cg_sdt_ta_validation_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cg_sdt_rsrp_change_thres_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_sdt_ta_validation_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cg_sdt_rsrp_change_thres_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_sdt_ta_validation_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cg-SDT-RSRP-ChangeThreshold-r17", cg_sdt_rsrp_change_thres_r17.to_string());
  j.end_obj();
}

const char* cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_opts::to_string() const
{
  static const char* names[] = {"dB2",
                                "dB4",
                                "dB6",
                                "dB8",
                                "dB10",
                                "dB14",
                                "dB18",
                                "dB22",
                                "dB26",
                                "dB30",
                                "dB34",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_e_");
}
uint8_t cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8, 10, 14, 18, 22, 26, 30, 34};
  return map_enum_number(numbers, 11, value, "cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_e_");
}

// SDT-MAC-PHY-CG-Config-r17 ::= SEQUENCE
SRSASN_CODE sdt_mac_phy_cg_cfg_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cg_sdt_cfg_lch_restrict_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_lch_restrict_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_init_bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_init_bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cfg_init_bwp_dl_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_rsrp_thres_ssb_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_ta_validation_cfg_r17_present, 1));
  HANDLE_CODE(bref.pack(cg_sdt_cs_rnti_r17_present, 1));

  if (cg_sdt_cfg_lch_restrict_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cg_sdt_cfg_lch_restrict_to_add_mod_list_r17, 1, 32));
  }
  if (cg_sdt_cfg_lch_restrict_to_release_list_r17.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, cg_sdt_cfg_lch_restrict_to_release_list_r17, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cg_sdt_cfg_init_bwp_n_ul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_n_ul_r17.pack(bref));
  }
  if (cg_sdt_cfg_init_bwp_sul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_sul_r17.pack(bref));
  }
  if (cg_sdt_cfg_init_bwp_dl_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_dl_r17.pack(bref));
  }
  if (cg_sdt_time_align_timer_r17_present) {
    HANDLE_CODE(cg_sdt_time_align_timer_r17.pack(bref));
  }
  if (cg_sdt_rsrp_thres_ssb_r17_present) {
    HANDLE_CODE(pack_integer(bref, cg_sdt_rsrp_thres_ssb_r17, (uint8_t)0u, (uint8_t)127u));
  }
  if (cg_sdt_ta_validation_cfg_r17_present) {
    HANDLE_CODE(cg_sdt_ta_validation_cfg_r17.pack(bref));
  }
  if (cg_sdt_cs_rnti_r17_present) {
    HANDLE_CODE(pack_integer(bref, cg_sdt_cs_rnti_r17, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sdt_mac_phy_cg_cfg_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_present, 1));
  bool cg_sdt_cfg_lch_restrict_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_lch_restrict_to_release_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_init_bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_init_bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cfg_init_bwp_dl_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_rsrp_thres_ssb_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_ta_validation_cfg_r17_present, 1));
  HANDLE_CODE(bref.unpack(cg_sdt_cs_rnti_r17_present, 1));

  if (cg_sdt_cfg_lch_restrict_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cg_sdt_cfg_lch_restrict_to_add_mod_list_r17, bref, 1, 32));
  }
  if (cg_sdt_cfg_lch_restrict_to_release_list_r17_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(cg_sdt_cfg_lch_restrict_to_release_list_r17, bref, 1, 32, integer_packer<uint8_t>(1, 32)));
  }
  if (cg_sdt_cfg_init_bwp_n_ul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_n_ul_r17.unpack(bref));
  }
  if (cg_sdt_cfg_init_bwp_sul_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_sul_r17.unpack(bref));
  }
  if (cg_sdt_cfg_init_bwp_dl_r17_present) {
    HANDLE_CODE(cg_sdt_cfg_init_bwp_dl_r17.unpack(bref));
  }
  if (cg_sdt_time_align_timer_r17_present) {
    HANDLE_CODE(cg_sdt_time_align_timer_r17.unpack(bref));
  }
  if (cg_sdt_rsrp_thres_ssb_r17_present) {
    HANDLE_CODE(unpack_integer(cg_sdt_rsrp_thres_ssb_r17, bref, (uint8_t)0u, (uint8_t)127u));
  }
  if (cg_sdt_ta_validation_cfg_r17_present) {
    HANDLE_CODE(cg_sdt_ta_validation_cfg_r17.unpack(bref));
  }
  if (cg_sdt_cs_rnti_r17_present) {
    HANDLE_CODE(unpack_integer(cg_sdt_cs_rnti_r17, bref, (uint32_t)0u, (uint32_t)65535u));
  }

  return SRSASN_SUCCESS;
}
void sdt_mac_phy_cg_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cg_sdt_cfg_lch_restrict_to_add_mod_list_r17.size() > 0) {
    j.start_array("cg-SDT-ConfigLCH-RestrictionToAddModList-r17");
    for (const auto& e1 : cg_sdt_cfg_lch_restrict_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cg_sdt_cfg_lch_restrict_to_release_list_r17.size() > 0) {
    j.start_array("cg-SDT-ConfigLCH-RestrictionToReleaseList-r17");
    for (const auto& e1 : cg_sdt_cfg_lch_restrict_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cg_sdt_cfg_init_bwp_n_ul_r17_present) {
    j.write_fieldname("cg-SDT-ConfigInitialBWP-NUL-r17");
    cg_sdt_cfg_init_bwp_n_ul_r17.to_json(j);
  }
  if (cg_sdt_cfg_init_bwp_sul_r17_present) {
    j.write_fieldname("cg-SDT-ConfigInitialBWP-SUL-r17");
    cg_sdt_cfg_init_bwp_sul_r17.to_json(j);
  }
  if (cg_sdt_cfg_init_bwp_dl_r17_present) {
    j.write_fieldname("cg-SDT-ConfigInitialBWP-DL-r17");
    cg_sdt_cfg_init_bwp_dl_r17.to_json(j);
  }
  if (cg_sdt_time_align_timer_r17_present) {
    j.write_str("cg-SDT-TimeAlignmentTimer-r17", cg_sdt_time_align_timer_r17.to_string());
  }
  if (cg_sdt_rsrp_thres_ssb_r17_present) {
    j.write_int("cg-SDT-RSRP-ThresholdSSB-r17", cg_sdt_rsrp_thres_ssb_r17);
  }
  if (cg_sdt_ta_validation_cfg_r17_present) {
    j.write_fieldname("cg-SDT-TA-ValidationConfig-r17");
    cg_sdt_ta_validation_cfg_r17.to_json(j);
  }
  if (cg_sdt_cs_rnti_r17_present) {
    j.write_int("cg-SDT-CS-RNTI-r17", cg_sdt_cs_rnti_r17);
  }
  j.end_obj();
}

// SL-ConfigCommonNR-r16 ::= SEQUENCE
SRSASN_CODE sl_cfg_common_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sl_freq_info_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_ue_sel_cfg_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_nr_anchor_carrier_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_eutra_anchor_carrier_freq_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_radio_bearer_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_rlc_bearer_cfg_list_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sl_meas_cfg_common_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_offset_dfn_r16_present, 1));
  HANDLE_CODE(bref.pack(t400_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.pack(sl_ssb_prio_nr_r16_present, 1));

  if (sl_freq_info_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_freq_info_list_r16, 1, 8));
  }
  if (sl_ue_sel_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_r16.pack(bref));
  }
  if (sl_nr_anchor_carrier_freq_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_nr_anchor_carrier_freq_list_r16, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sl_eutra_anchor_carrier_freq_list_r16.size() > 0) {
    HANDLE_CODE(
        pack_dyn_seq_of(bref, sl_eutra_anchor_carrier_freq_list_r16, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (sl_radio_bearer_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_radio_bearer_cfg_list_r16, 1, 512));
  }
  if (sl_rlc_bearer_cfg_list_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_rlc_bearer_cfg_list_r16, 1, 512));
  }
  if (sl_meas_cfg_common_r16_present) {
    HANDLE_CODE(sl_meas_cfg_common_r16.pack(bref));
  }
  if (sl_offset_dfn_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_offset_dfn_r16, (uint16_t)1u, (uint16_t)1000u));
  }
  if (t400_r16_present) {
    HANDLE_CODE(t400_r16.pack(bref));
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    HANDLE_CODE(sl_max_num_consecutive_dtx_r16.pack(bref));
  }
  if (sl_ssb_prio_nr_r16_present) {
    HANDLE_CODE(pack_integer(bref, sl_ssb_prio_nr_r16, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cfg_common_nr_r16_s::unpack(cbit_ref& bref)
{
  bool sl_freq_info_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_freq_info_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ue_sel_cfg_r16_present, 1));
  bool sl_nr_anchor_carrier_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_nr_anchor_carrier_freq_list_r16_present, 1));
  bool sl_eutra_anchor_carrier_freq_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_eutra_anchor_carrier_freq_list_r16_present, 1));
  bool sl_radio_bearer_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_radio_bearer_cfg_list_r16_present, 1));
  bool sl_rlc_bearer_cfg_list_r16_present;
  HANDLE_CODE(bref.unpack(sl_rlc_bearer_cfg_list_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_meas_cfg_common_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_csi_acquisition_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_offset_dfn_r16_present, 1));
  HANDLE_CODE(bref.unpack(t400_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_max_num_consecutive_dtx_r16_present, 1));
  HANDLE_CODE(bref.unpack(sl_ssb_prio_nr_r16_present, 1));

  if (sl_freq_info_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_freq_info_list_r16, bref, 1, 8));
  }
  if (sl_ue_sel_cfg_r16_present) {
    HANDLE_CODE(sl_ue_sel_cfg_r16.unpack(bref));
  }
  if (sl_nr_anchor_carrier_freq_list_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_nr_anchor_carrier_freq_list_r16, bref, 1, 8, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sl_eutra_anchor_carrier_freq_list_r16_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(sl_eutra_anchor_carrier_freq_list_r16, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (sl_radio_bearer_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_radio_bearer_cfg_list_r16, bref, 1, 512));
  }
  if (sl_rlc_bearer_cfg_list_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sl_rlc_bearer_cfg_list_r16, bref, 1, 512));
  }
  if (sl_meas_cfg_common_r16_present) {
    HANDLE_CODE(sl_meas_cfg_common_r16.unpack(bref));
  }
  if (sl_offset_dfn_r16_present) {
    HANDLE_CODE(unpack_integer(sl_offset_dfn_r16, bref, (uint16_t)1u, (uint16_t)1000u));
  }
  if (t400_r16_present) {
    HANDLE_CODE(t400_r16.unpack(bref));
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    HANDLE_CODE(sl_max_num_consecutive_dtx_r16.unpack(bref));
  }
  if (sl_ssb_prio_nr_r16_present) {
    HANDLE_CODE(unpack_integer(sl_ssb_prio_nr_r16, bref, (uint8_t)1u, (uint8_t)8u));
  }

  return SRSASN_SUCCESS;
}
void sl_cfg_common_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_freq_info_list_r16.size() > 0) {
    j.start_array("sl-FreqInfoList-r16");
    for (const auto& e1 : sl_freq_info_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_ue_sel_cfg_r16_present) {
    j.write_fieldname("sl-UE-SelectedConfig-r16");
    sl_ue_sel_cfg_r16.to_json(j);
  }
  if (sl_nr_anchor_carrier_freq_list_r16.size() > 0) {
    j.start_array("sl-NR-AnchorCarrierFreqList-r16");
    for (const auto& e1 : sl_nr_anchor_carrier_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_eutra_anchor_carrier_freq_list_r16.size() > 0) {
    j.start_array("sl-EUTRA-AnchorCarrierFreqList-r16");
    for (const auto& e1 : sl_eutra_anchor_carrier_freq_list_r16) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sl_radio_bearer_cfg_list_r16.size() > 0) {
    j.start_array("sl-RadioBearerConfigList-r16");
    for (const auto& e1 : sl_radio_bearer_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_rlc_bearer_cfg_list_r16.size() > 0) {
    j.start_array("sl-RLC-BearerConfigList-r16");
    for (const auto& e1 : sl_rlc_bearer_cfg_list_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_meas_cfg_common_r16_present) {
    j.write_fieldname("sl-MeasConfigCommon-r16");
    sl_meas_cfg_common_r16.to_json(j);
  }
  if (sl_csi_acquisition_r16_present) {
    j.write_str("sl-CSI-Acquisition-r16", "enabled");
  }
  if (sl_offset_dfn_r16_present) {
    j.write_int("sl-OffsetDFN-r16", sl_offset_dfn_r16);
  }
  if (t400_r16_present) {
    j.write_str("t400-r16", t400_r16.to_string());
  }
  if (sl_max_num_consecutive_dtx_r16_present) {
    j.write_str("sl-MaxNumConsecutiveDTX-r16", sl_max_num_consecutive_dtx_r16.to_string());
  }
  if (sl_ssb_prio_nr_r16_present) {
    j.write_int("sl-SSB-PriorityNR-r16", sl_ssb_prio_nr_r16);
  }
  j.end_obj();
}

const char* sl_cfg_common_nr_r16_s::t400_r16_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "sl_cfg_common_nr_r16_s::t400_r16_e_");
}
uint16_t sl_cfg_common_nr_r16_s::t400_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "sl_cfg_common_nr_r16_s::t400_r16_e_");
}

const char* sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n16", "n32"};
  return convert_enum_idx(names, 8, value, "sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}
uint8_t sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}

// SL-DiscConfigCommon-r17 ::= SEQUENCE
SRSASN_CODE sl_disc_cfg_common_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_relay_ue_cfg_common_r17.pack(bref));
  HANDLE_CODE(sl_remote_ue_cfg_common_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_disc_cfg_common_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_relay_ue_cfg_common_r17.unpack(bref));
  HANDLE_CODE(sl_remote_ue_cfg_common_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_disc_cfg_common_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-RelayUE-ConfigCommon-r17");
  sl_relay_ue_cfg_common_r17.to_json(j);
  j.write_fieldname("sl-RemoteUE-ConfigCommon-r17");
  sl_remote_ue_cfg_common_r17.to_json(j);
  j.end_obj();
}

// UE-TimersAndConstantsRemoteUE-r17 ::= SEQUENCE
SRSASN_CODE ue_timers_and_consts_remote_ue_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(t300_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.pack(t301_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.pack(t319_remote_ue_r17_present, 1));

  if (t300_remote_ue_r17_present) {
    HANDLE_CODE(t300_remote_ue_r17.pack(bref));
  }
  if (t301_remote_ue_r17_present) {
    HANDLE_CODE(t301_remote_ue_r17.pack(bref));
  }
  if (t319_remote_ue_r17_present) {
    HANDLE_CODE(t319_remote_ue_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_timers_and_consts_remote_ue_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(t300_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.unpack(t301_remote_ue_r17_present, 1));
  HANDLE_CODE(bref.unpack(t319_remote_ue_r17_present, 1));

  if (t300_remote_ue_r17_present) {
    HANDLE_CODE(t300_remote_ue_r17.unpack(bref));
  }
  if (t301_remote_ue_r17_present) {
    HANDLE_CODE(t301_remote_ue_r17.unpack(bref));
  }
  if (t319_remote_ue_r17_present) {
    HANDLE_CODE(t319_remote_ue_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_timers_and_consts_remote_ue_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (t300_remote_ue_r17_present) {
    j.write_str("t300-RemoteUE-r17", t300_remote_ue_r17.to_string());
  }
  if (t301_remote_ue_r17_present) {
    j.write_str("t301-RemoteUE-r17", t301_remote_ue_r17.to_string());
  }
  if (t319_remote_ue_r17_present) {
    j.write_str("t319-RemoteUE-r17", t319_remote_ue_r17.to_string());
  }
  j.end_obj();
}

const char* ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_e_");
}
uint16_t ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_e_");
}

const char* ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_e_");
}
uint16_t ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_e_");
}

const char* ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_opts::to_string() const
{
  static const char* names[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(names, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_e_");
}
uint16_t ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_e_");
}

// SIB12-IEs-r16 ::= SEQUENCE
SRSASN_CODE sib12_ies_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(sl_cfg_common_nr_r16.pack(bref));
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_drx_cfg_common_gc_bc_r17.is_present();
    group_flags[0] |= sl_disc_cfg_common_r17.is_present();
    group_flags[0] |= sl_l2_u2_n_relay_r17_present;
    group_flags[0] |= sl_non_relay_discovery_r17_present;
    group_flags[0] |= sl_l3_u2_n_relay_discovery_r17_present;
    group_flags[0] |= sl_timers_and_consts_remote_ue_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_drx_cfg_common_gc_bc_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_disc_cfg_common_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(sl_l2_u2_n_relay_r17_present, 1));
      HANDLE_CODE(bref.pack(sl_non_relay_discovery_r17_present, 1));
      HANDLE_CODE(bref.pack(sl_l3_u2_n_relay_discovery_r17_present, 1));
      HANDLE_CODE(bref.pack(sl_timers_and_consts_remote_ue_r17.is_present(), 1));
      if (sl_drx_cfg_common_gc_bc_r17.is_present()) {
        HANDLE_CODE(sl_drx_cfg_common_gc_bc_r17->pack(bref));
      }
      if (sl_disc_cfg_common_r17.is_present()) {
        HANDLE_CODE(sl_disc_cfg_common_r17->pack(bref));
      }
      if (sl_timers_and_consts_remote_ue_r17.is_present()) {
        HANDLE_CODE(sl_timers_and_consts_remote_ue_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sib12_ies_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(sl_cfg_common_nr_r16.unpack(bref));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sl_drx_cfg_common_gc_bc_r17, bref);
      unpack_presence_flag(sl_disc_cfg_common_r17, bref);
      HANDLE_CODE(bref.unpack(sl_l2_u2_n_relay_r17_present, 1));
      HANDLE_CODE(bref.unpack(sl_non_relay_discovery_r17_present, 1));
      HANDLE_CODE(bref.unpack(sl_l3_u2_n_relay_discovery_r17_present, 1));
      unpack_presence_flag(sl_timers_and_consts_remote_ue_r17, bref);
      if (sl_drx_cfg_common_gc_bc_r17.is_present()) {
        HANDLE_CODE(sl_drx_cfg_common_gc_bc_r17->unpack(bref));
      }
      if (sl_disc_cfg_common_r17.is_present()) {
        HANDLE_CODE(sl_disc_cfg_common_r17->unpack(bref));
      }
      if (sl_timers_and_consts_remote_ue_r17.is_present()) {
        HANDLE_CODE(sl_timers_and_consts_remote_ue_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sib12_ies_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("sl-ConfigCommonNR-r16");
  sl_cfg_common_nr_r16.to_json(j);
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (ext) {
    if (sl_drx_cfg_common_gc_bc_r17.is_present()) {
      j.write_fieldname("sl-DRX-ConfigCommonGC-BC-r17");
      sl_drx_cfg_common_gc_bc_r17->to_json(j);
    }
    if (sl_disc_cfg_common_r17.is_present()) {
      j.write_fieldname("sl-DiscConfigCommon-r17");
      sl_disc_cfg_common_r17->to_json(j);
    }
    if (sl_l2_u2_n_relay_r17_present) {
      j.write_str("sl-L2U2N-Relay-r17", "enabled");
    }
    if (sl_non_relay_discovery_r17_present) {
      j.write_str("sl-NonRelayDiscovery-r17", "enabled");
    }
    if (sl_l3_u2_n_relay_discovery_r17_present) {
      j.write_str("sl-L3U2N-RelayDiscovery-r17", "enabled");
    }
    if (sl_timers_and_consts_remote_ue_r17.is_present()) {
      j.write_fieldname("sl-TimersAndConstantsRemoteUE-r17");
      sl_timers_and_consts_remote_ue_r17->to_json(j);
    }
  }
  j.end_obj();
}

// TRS-ResourceSet-r17 ::= SEQUENCE
SRSASN_CODE trs_res_set_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pwr_ctrl_offset_ss_r17.pack(bref));
  HANDLE_CODE(scrambling_id_info_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, first_ofdm_symbol_in_time_domain_r17, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(pack_integer(bref, start_rb_r17, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(pack_integer(bref, nrof_rbs_r17, (uint16_t)24u, (uint16_t)276u));
  HANDLE_CODE(pack_integer(bref, ssb_idx_r17, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(periodicity_and_offset_r17.pack(bref));
  HANDLE_CODE(freq_domain_alloc_r17.pack(bref));
  HANDLE_CODE(pack_integer(bref, ind_bit_id_r17, (uint8_t)0u, (uint8_t)5u));
  HANDLE_CODE(nrof_res_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE trs_res_set_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pwr_ctrl_offset_ss_r17.unpack(bref));
  HANDLE_CODE(scrambling_id_info_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(first_ofdm_symbol_in_time_domain_r17, bref, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(unpack_integer(start_rb_r17, bref, (uint16_t)0u, (uint16_t)274u));
  HANDLE_CODE(unpack_integer(nrof_rbs_r17, bref, (uint16_t)24u, (uint16_t)276u));
  HANDLE_CODE(unpack_integer(ssb_idx_r17, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(periodicity_and_offset_r17.unpack(bref));
  HANDLE_CODE(freq_domain_alloc_r17.unpack(bref));
  HANDLE_CODE(unpack_integer(ind_bit_id_r17, bref, (uint8_t)0u, (uint8_t)5u));
  HANDLE_CODE(nrof_res_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void trs_res_set_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("powerControlOffsetSS-r17", pwr_ctrl_offset_ss_r17.to_string());
  j.write_fieldname("scramblingID-Info-r17");
  scrambling_id_info_r17.to_json(j);
  j.write_int("firstOFDMSymbolInTimeDomain-r17", first_ofdm_symbol_in_time_domain_r17);
  j.write_int("startingRB-r17", start_rb_r17);
  j.write_int("nrofRBs-r17", nrof_rbs_r17);
  j.write_int("ssb-Index-r17", ssb_idx_r17);
  j.write_fieldname("periodicityAndOffset-r17");
  periodicity_and_offset_r17.to_json(j);
  j.write_str("frequencyDomainAllocation-r17", freq_domain_alloc_r17.to_string());
  j.write_int("indBitID-r17", ind_bit_id_r17);
  j.write_str("nrofResources-r17", nrof_res_r17.to_string());
  j.end_obj();
}

const char* trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_opts::to_string() const
{
  static const char* names[] = {"db-3", "db0", "db3", "db6"};
  return convert_enum_idx(names, 4, value, "trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_e_");
}
int8_t trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_opts::to_number() const
{
  static const int8_t numbers[] = {-3, 0, 3, 6};
  return map_enum_number(numbers, 4, value, "trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_e_");
}

void trs_res_set_r17_s::scrambling_id_info_r17_c_::destroy_()
{
  switch (type_) {
    case types::scrambling_id_per_res_list_with2_r17:
      c.destroy<scrambling_id_per_res_list_with2_r17_l_>();
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.destroy<scrambling_id_per_res_list_with4_r17_l_>();
      break;
    default:
      break;
  }
}
void trs_res_set_r17_s::scrambling_id_info_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      c.init<scrambling_id_per_res_list_with2_r17_l_>();
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.init<scrambling_id_per_res_list_with4_r17_l_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_info_r17_c_(
    const trs_res_set_r17_s::scrambling_id_info_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      c.init(other.c.get<uint16_t>());
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      c.init(other.c.get<scrambling_id_per_res_list_with2_r17_l_>());
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.init(other.c.get<scrambling_id_per_res_list_with4_r17_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }
}
trs_res_set_r17_s::scrambling_id_info_r17_c_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::operator=(const trs_res_set_r17_s::scrambling_id_info_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      c.set(other.c.get<uint16_t>());
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      c.set(other.c.get<scrambling_id_per_res_list_with2_r17_l_>());
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      c.set(other.c.get<scrambling_id_per_res_list_with4_r17_l_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }

  return *this;
}
uint16_t& trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_for_common_r17()
{
  set(types::scrambling_id_for_common_r17);
  return c.get<uint16_t>();
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with2_r17_l_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_per_res_list_with2_r17()
{
  set(types::scrambling_id_per_res_list_with2_r17);
  return c.get<scrambling_id_per_res_list_with2_r17_l_>();
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with4_r17_l_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_per_res_list_with4_r17()
{
  set(types::scrambling_id_per_res_list_with4_r17);
  return c.get<scrambling_id_per_res_list_with4_r17_l_>();
}
void trs_res_set_r17_s::scrambling_id_info_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      j.write_int("scramblingIDforCommon-r17", c.get<uint16_t>());
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      j.start_array("scramblingIDperResourceListWith2-r17");
      for (const auto& e1 : c.get<scrambling_id_per_res_list_with2_r17_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      j.start_array("scramblingIDperResourceListWith4-r17");
      for (const auto& e1 : c.get<scrambling_id_per_res_list_with4_r17_l_>()) {
        j.write_int(e1);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE trs_res_set_r17_s::scrambling_id_info_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    c.get<scrambling_id_per_res_list_with2_r17_l_>(),
                                    c.get<scrambling_id_per_res_list_with2_r17_l_>().size(),
                                    integer_packer<uint16_t>(0, 1023)));
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      HANDLE_CODE(pack_fixed_seq_of(bref,
                                    c.get<scrambling_id_per_res_list_with4_r17_l_>(),
                                    c.get<scrambling_id_per_res_list_with4_r17_l_>().size(),
                                    integer_packer<uint16_t>(0, 1023)));
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE trs_res_set_r17_s::scrambling_id_info_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scrambling_id_for_common_r17:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::scrambling_id_per_res_list_with2_r17:
      HANDLE_CODE(unpack_fixed_seq_of(c.get<scrambling_id_per_res_list_with2_r17_l_>(),
                                      bref,
                                      c.get<scrambling_id_per_res_list_with2_r17_l_>().size(),
                                      integer_packer<uint16_t>(0, 1023)));
      break;
    case types::scrambling_id_per_res_list_with4_r17:
      HANDLE_CODE(unpack_fixed_seq_of(c.get<scrambling_id_per_res_list_with4_r17_l_>(),
                                      bref,
                                      c.get<scrambling_id_per_res_list_with4_r17_l_>().size(),
                                      integer_packer<uint16_t>(0, 1023)));
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::scrambling_id_info_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* trs_res_set_r17_s::scrambling_id_info_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "scramblingIDforCommon-r17", "scramblingIDperResourceListWith2-r17", "scramblingIDperResourceListWith4-r17"};
  return convert_enum_idx(names, 3, value, "trs_res_set_r17_s::scrambling_id_info_r17_c_::types");
}
uint8_t trs_res_set_r17_s::scrambling_id_info_r17_c_::types_opts::to_number() const
{
  switch (value) {
    case scrambling_id_per_res_list_with2_r17:
      return 2;
    case scrambling_id_per_res_list_with4_r17:
      return 4;
    default:
      invalid_enum_number(value, "trs_res_set_r17_s::scrambling_id_info_r17_c_::types");
  }
  return 0;
}

void trs_res_set_r17_s::periodicity_and_offset_r17_c_::destroy_() {}
void trs_res_set_r17_s::periodicity_and_offset_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
trs_res_set_r17_s::periodicity_and_offset_r17_c_::periodicity_and_offset_r17_c_(
    const trs_res_set_r17_s::periodicity_and_offset_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::slots10:
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
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
  }
}
trs_res_set_r17_s::periodicity_and_offset_r17_c_& trs_res_set_r17_s::periodicity_and_offset_r17_c_::operator=(
    const trs_res_set_r17_s::periodicity_and_offset_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::slots10:
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
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
  }

  return *this;
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots10()
{
  set(types::slots10);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots20()
{
  set(types::slots20);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots40()
{
  set(types::slots40);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots80()
{
  set(types::slots80);
  return c.get<uint8_t>();
}
void trs_res_set_r17_s::periodicity_and_offset_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::slots10:
      j.write_int("slots10", c.get<uint8_t>());
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
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE trs_res_set_r17_s::periodicity_and_offset_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::slots10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
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
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE trs_res_set_r17_s::periodicity_and_offset_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::slots10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
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
    default:
      log_invalid_choice_id(type_, "trs_res_set_r17_s::periodicity_and_offset_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* trs_res_set_r17_s::periodicity_and_offset_r17_c_::types_opts::to_string() const
{
  static const char* names[] = {"slots10", "slots20", "slots40", "slots80"};
  return convert_enum_idx(names, 4, value, "trs_res_set_r17_s::periodicity_and_offset_r17_c_::types");
}
uint8_t trs_res_set_r17_s::periodicity_and_offset_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 80};
  return map_enum_number(numbers, 4, value, "trs_res_set_r17_s::periodicity_and_offset_r17_c_::types");
}

const char* trs_res_set_r17_s::nrof_res_r17_opts::to_string() const
{
  static const char* names[] = {"n2", "n4"};
  return convert_enum_idx(names, 2, value, "trs_res_set_r17_s::nrof_res_r17_e_");
}
uint8_t trs_res_set_r17_s::nrof_res_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "trs_res_set_r17_s::nrof_res_r17_e_");
}

// SIB17-IEs-r17 ::= SEQUENCE
SRSASN_CODE sib17_ies_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(validity_dur_r17_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, trs_res_set_cfg_r17, 1, 64));
  if (validity_dur_r17_present) {
    HANDLE_CODE(validity_dur_r17.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sib17_ies_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(validity_dur_r17_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(trs_res_set_cfg_r17, bref, 1, 64));
  if (validity_dur_r17_present) {
    HANDLE_CODE(validity_dur_r17.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sib17_ies_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("trs-ResourceSetConfig-r17");
  for (const auto& e1 : trs_res_set_cfg_r17) {
    e1.to_json(j);
  }
  j.end_array();
  if (validity_dur_r17_present) {
    j.write_str("validityDuration-r17", validity_dur_r17.to_string());
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  j.end_obj();
}

const char* sib17_ies_r17_s::validity_dur_r17_opts::to_string() const
{
  static const char* names[] = {"t1",
                                "t2",
                                "t4",
                                "t8",
                                "t16",
                                "t32",
                                "t64",
                                "t128",
                                "t256",
                                "t512",
                                "infinity",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "sib17_ies_r17_s::validity_dur_r17_e_");
}
int16_t sib17_ies_r17_s::validity_dur_r17_opts::to_number() const
{
  static const int16_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, -1};
  return map_enum_number(numbers, 11, value, "sib17_ies_r17_s::validity_dur_r17_e_");
}

// SL-ServingCellInfo-r17 ::= SEQUENCE
SRSASN_CODE sl_serving_cell_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, sl_pci_r17, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(pack_integer(bref, sl_carrier_freq_nr_r17, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_serving_cell_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(sl_pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(unpack_integer(sl_carrier_freq_nr_r17, bref, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
void sl_serving_cell_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sl-PhysCellId-r17", sl_pci_r17);
  j.write_int("sl-CarrierFreqNR-r17", sl_carrier_freq_nr_r17);
  j.end_obj();
}

// SRS-PosConfig-r17 ::= SEQUENCE
SRSASN_CODE srs_pos_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_pos_res_set_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_pos_res_set_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_pos_res_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(srs_pos_res_to_add_mod_list_r17.size() > 0, 1));

  if (srs_pos_res_set_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_set_to_release_list_r17, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (srs_pos_res_set_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_set_to_add_mod_list_r17, 1, 16));
  }
  if (srs_pos_res_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_to_release_list_r17, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (srs_pos_res_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, srs_pos_res_to_add_mod_list_r17, 1, 64));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_cfg_r17_s::unpack(cbit_ref& bref)
{
  bool srs_pos_res_set_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_set_to_release_list_r17_present, 1));
  bool srs_pos_res_set_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_set_to_add_mod_list_r17_present, 1));
  bool srs_pos_res_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_to_release_list_r17_present, 1));
  bool srs_pos_res_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(srs_pos_res_to_add_mod_list_r17_present, 1));

  if (srs_pos_res_set_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_set_to_release_list_r17, bref, 1, 16, integer_packer<uint8_t>(0, 15)));
  }
  if (srs_pos_res_set_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_set_to_add_mod_list_r17, bref, 1, 16));
  }
  if (srs_pos_res_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_to_release_list_r17, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
  }
  if (srs_pos_res_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(srs_pos_res_to_add_mod_list_r17, bref, 1, 64));
  }

  return SRSASN_SUCCESS;
}
void srs_pos_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_pos_res_set_to_release_list_r17.size() > 0) {
    j.start_array("srs-PosResourceSetToReleaseList-r17");
    for (const auto& e1 : srs_pos_res_set_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (srs_pos_res_set_to_add_mod_list_r17.size() > 0) {
    j.start_array("srs-PosResourceSetToAddModList-r17");
    for (const auto& e1 : srs_pos_res_set_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srs_pos_res_to_release_list_r17.size() > 0) {
    j.start_array("srs-PosResourceToReleaseList-r17");
    for (const auto& e1 : srs_pos_res_to_release_list_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (srs_pos_res_to_add_mod_list_r17.size() > 0) {
    j.start_array("srs-PosResourceToAddModList-r17");
    for (const auto& e1 : srs_pos_res_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// RSRP-ChangeThreshold-r17 ::= ENUMERATED
const char* rsrp_change_thres_r17_opts::to_string() const
{
  static const char* names[] = {"dB4",
                                "dB6",
                                "dB8",
                                "dB10",
                                "dB14",
                                "dB18",
                                "dB22",
                                "dB26",
                                "dB30",
                                "dB34",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "rsrp_change_thres_r17_e");
}
uint8_t rsrp_change_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 6, 8, 10, 14, 18, 22, 26, 30, 34};
  return map_enum_number(numbers, 10, value, "rsrp_change_thres_r17_e");
}

// SRS-PosRRC-InactiveConfig-r17 ::= SEQUENCE
SRSASN_CODE srs_pos_rrc_inactive_cfg_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_pos_cfg_n_ul_r17_present, 1));
  HANDLE_CODE(bref.pack(srs_pos_cfg_sul_r17_present, 1));
  HANDLE_CODE(bref.pack(bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.pack(bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.pack(inactive_pos_srs_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.pack(inactive_pos_srs_rsrp_change_thres_r17_present, 1));

  if (srs_pos_cfg_n_ul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_n_ul_r17.pack(bref));
  }
  if (srs_pos_cfg_sul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_sul_r17.pack(bref));
  }
  if (bwp_n_ul_r17_present) {
    HANDLE_CODE(bwp_n_ul_r17.pack(bref));
  }
  if (bwp_sul_r17_present) {
    HANDLE_CODE(bwp_sul_r17.pack(bref));
  }
  if (inactive_pos_srs_time_align_timer_r17_present) {
    HANDLE_CODE(inactive_pos_srs_time_align_timer_r17.pack(bref));
  }
  if (inactive_pos_srs_rsrp_change_thres_r17_present) {
    HANDLE_CODE(inactive_pos_srs_rsrp_change_thres_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_rrc_inactive_cfg_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_pos_cfg_n_ul_r17_present, 1));
  HANDLE_CODE(bref.unpack(srs_pos_cfg_sul_r17_present, 1));
  HANDLE_CODE(bref.unpack(bwp_n_ul_r17_present, 1));
  HANDLE_CODE(bref.unpack(bwp_sul_r17_present, 1));
  HANDLE_CODE(bref.unpack(inactive_pos_srs_time_align_timer_r17_present, 1));
  HANDLE_CODE(bref.unpack(inactive_pos_srs_rsrp_change_thres_r17_present, 1));

  if (srs_pos_cfg_n_ul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_n_ul_r17.unpack(bref));
  }
  if (srs_pos_cfg_sul_r17_present) {
    HANDLE_CODE(srs_pos_cfg_sul_r17.unpack(bref));
  }
  if (bwp_n_ul_r17_present) {
    HANDLE_CODE(bwp_n_ul_r17.unpack(bref));
  }
  if (bwp_sul_r17_present) {
    HANDLE_CODE(bwp_sul_r17.unpack(bref));
  }
  if (inactive_pos_srs_time_align_timer_r17_present) {
    HANDLE_CODE(inactive_pos_srs_time_align_timer_r17.unpack(bref));
  }
  if (inactive_pos_srs_rsrp_change_thres_r17_present) {
    HANDLE_CODE(inactive_pos_srs_rsrp_change_thres_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_pos_rrc_inactive_cfg_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_pos_cfg_n_ul_r17_present) {
    j.write_fieldname("srs-PosConfigNUL-r17");
    srs_pos_cfg_n_ul_r17.to_json(j);
  }
  if (srs_pos_cfg_sul_r17_present) {
    j.write_fieldname("srs-PosConfigSUL-r17");
    srs_pos_cfg_sul_r17.to_json(j);
  }
  if (bwp_n_ul_r17_present) {
    j.write_fieldname("bwp-NUL-r17");
    bwp_n_ul_r17.to_json(j);
  }
  if (bwp_sul_r17_present) {
    j.write_fieldname("bwp-SUL-r17");
    bwp_sul_r17.to_json(j);
  }
  if (inactive_pos_srs_time_align_timer_r17_present) {
    j.write_str("inactivePosSRS-TimeAlignmentTimer-r17", inactive_pos_srs_time_align_timer_r17.to_string());
  }
  if (inactive_pos_srs_rsrp_change_thres_r17_present) {
    j.write_str("inactivePosSRS-RSRP-ChangeThreshold-r17", inactive_pos_srs_rsrp_change_thres_r17.to_string());
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterCommon ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(mrdc_request_present, 1));

  if (mrdc_request_present) {
    HANDLE_CODE(bref.pack(mrdc_request.omit_en_dc_present, 1));
    HANDLE_CODE(bref.pack(mrdc_request.include_nr_dc_present, 1));
    HANDLE_CODE(bref.pack(mrdc_request.include_ne_dc_present, 1));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= codebook_type_request_r16.is_present();
    group_flags[0] |= ul_tx_switch_request_r16_present;
    group_flags[1] |= requested_cell_grouping_r16.is_present();
    group_flags[2] |= fallback_group_five_request_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(codebook_type_request_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_tx_switch_request_r16_present, 1));
      if (codebook_type_request_r16.is_present()) {
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type1_single_panel_r16_present, 1));
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type1_multi_panel_r16_present, 1));
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type2_r16_present, 1));
        HANDLE_CODE(bref.pack(codebook_type_request_r16->type2_port_sel_r16_present, 1));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(requested_cell_grouping_r16.is_present(), 1));
      if (requested_cell_grouping_r16.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *requested_cell_grouping_r16, 1, 32));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(fallback_group_five_request_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(mrdc_request_present, 1));

  if (mrdc_request_present) {
    HANDLE_CODE(bref.unpack(mrdc_request.omit_en_dc_present, 1));
    HANDLE_CODE(bref.unpack(mrdc_request.include_nr_dc_present, 1));
    HANDLE_CODE(bref.unpack(mrdc_request.include_ne_dc_present, 1));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(codebook_type_request_r16, bref);
      HANDLE_CODE(bref.unpack(ul_tx_switch_request_r16_present, 1));
      if (codebook_type_request_r16.is_present()) {
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type1_single_panel_r16_present, 1));
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type1_multi_panel_r16_present, 1));
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type2_r16_present, 1));
        HANDLE_CODE(bref.unpack(codebook_type_request_r16->type2_port_sel_r16_present, 1));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(requested_cell_grouping_r16, bref);
      if (requested_cell_grouping_r16.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*requested_cell_grouping_r16, bref, 1, 32));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(fallback_group_five_request_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mrdc_request_present) {
    j.write_fieldname("mrdc-Request");
    j.start_obj();
    if (mrdc_request.omit_en_dc_present) {
      j.write_str("omitEN-DC", "true");
    }
    if (mrdc_request.include_nr_dc_present) {
      j.write_str("includeNR-DC", "true");
    }
    if (mrdc_request.include_ne_dc_present) {
      j.write_str("includeNE-DC", "true");
    }
    j.end_obj();
  }
  if (ext) {
    if (codebook_type_request_r16.is_present()) {
      j.write_fieldname("codebookTypeRequest-r16");
      j.start_obj();
      if (codebook_type_request_r16->type1_single_panel_r16_present) {
        j.write_str("type1-SinglePanel-r16", "true");
      }
      if (codebook_type_request_r16->type1_multi_panel_r16_present) {
        j.write_str("type1-MultiPanel-r16", "true");
      }
      if (codebook_type_request_r16->type2_r16_present) {
        j.write_str("type2-r16", "true");
      }
      if (codebook_type_request_r16->type2_port_sel_r16_present) {
        j.write_str("type2-PortSelection-r16", "true");
      }
      j.end_obj();
    }
    if (ul_tx_switch_request_r16_present) {
      j.write_str("uplinkTxSwitchRequest-r16", "true");
    }
    if (requested_cell_grouping_r16.is_present()) {
      j.start_array("requestedCellGrouping-r16");
      for (const auto& e1 : *requested_cell_grouping_r16) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (fallback_group_five_request_r17_present) {
      j.write_str("fallbackGroupFiveRequest-r17", "true");
    }
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterNR-v1710 ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_nr_v1710_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sidelink_request_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_nr_v1710_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sidelink_request_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_nr_v1710_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sidelink_request_r17_present) {
    j.write_str("sidelinkRequest-r17", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterNR-v1540 ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_nr_v1540_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_switching_time_request_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_nr_v1540_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_switching_time_request_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_nr_v1540_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (srs_switching_time_request_present) {
    j.write_str("srs-SwitchingTimeRequest", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-CapabilityRequestFilterNR ::= SEQUENCE
SRSASN_CODE ue_cap_request_filt_nr_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(freq_band_list_filt.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (freq_band_list_filt.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_filt, 1, 1280));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_request_filt_nr_s::unpack(cbit_ref& bref)
{
  bool freq_band_list_filt_present;
  HANDLE_CODE(bref.unpack(freq_band_list_filt_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (freq_band_list_filt_present) {
    HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_filt, bref, 1, 1280));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_request_filt_nr_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_band_list_filt.size() > 0) {
    j.start_array("frequencyBandListFilter");
    for (const auto& e1 : freq_band_list_filt) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-MRDC-Capability-v1730 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_v1730_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_v1730_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_v1730_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1730.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_v1730_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_v1730_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_v1730_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1730.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_v1730_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_v1730_present) {
    j.write_fieldname("measAndMobParametersMRDC-v1730");
    meas_and_mob_params_mrdc_v1730.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// GeneralParametersMRDC-v1610 ::= SEQUENCE
SRSASN_CODE general_params_mrdc_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(f1c_over_eutra_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE general_params_mrdc_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(f1c_over_eutra_r16_present, 1));

  return SRSASN_SUCCESS;
}
void general_params_mrdc_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (f1c_over_eutra_r16_present) {
    j.write_str("f1c-OverEUTRA-r16", "supported");
  }
  j.end_obj();
}

// PDCP-ParametersMRDC-v1610 ::= SEQUENCE
SRSASN_CODE pdcp_params_mrdc_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_drb_nr_iab_r16_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_mrdc_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_drb_nr_iab_r16_present, 1));

  return SRSASN_SUCCESS;
}
void pdcp_params_mrdc_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_drb_nr_iab_r16_present) {
    j.write_str("scg-DRB-NR-IAB-r16", "supported");
  }
  j.end_obj();
}

// UE-MRDC-Capability-v1700 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_v1700_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_and_mob_params_mrdc_v1700.pack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_v1700_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(meas_and_mob_params_mrdc_v1700.unpack(bref));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_v1700_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measAndMobParametersMRDC-v1700");
  meas_and_mob_params_mrdc_v1700.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-MRDC-Capability-v1610 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_v1610_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_v1610_present, 1));
  HANDLE_CODE(bref.pack(general_params_mrdc_v1610_present, 1));
  HANDLE_CODE(bref.pack(pdcp_params_mrdc_v1610_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_v1610_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1610.pack(bref));
  }
  if (general_params_mrdc_v1610_present) {
    HANDLE_CODE(general_params_mrdc_v1610.pack(bref));
  }
  if (pdcp_params_mrdc_v1610_present) {
    HANDLE_CODE(pdcp_params_mrdc_v1610.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_v1610_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_v1610_present, 1));
  HANDLE_CODE(bref.unpack(general_params_mrdc_v1610_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_params_mrdc_v1610_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_v1610_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1610.unpack(bref));
  }
  if (general_params_mrdc_v1610_present) {
    HANDLE_CODE(general_params_mrdc_v1610.unpack(bref));
  }
  if (pdcp_params_mrdc_v1610_present) {
    HANDLE_CODE(pdcp_params_mrdc_v1610.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_v1610_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_v1610_present) {
    j.write_fieldname("measAndMobParametersMRDC-v1610");
    meas_and_mob_params_mrdc_v1610.to_json(j);
  }
  if (general_params_mrdc_v1610_present) {
    j.write_fieldname("generalParametersMRDC-v1610");
    general_params_mrdc_v1610.to_json(j);
  }
  if (pdcp_params_mrdc_v1610_present) {
    j.write_fieldname("pdcp-ParametersMRDC-v1610");
    pdcp_params_mrdc_v1610.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-MRDC-CapabilityAddXDD-Mode-v1560 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_add_xdd_mode_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_add_xdd_mode_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_xdd_diff_v1560_present, 1));

  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_xdd_diff_v1560.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_add_xdd_mode_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_xdd_diff_v1560_present) {
    j.write_fieldname("measAndMobParametersMRDC-XDD-Diff-v1560");
    meas_and_mob_params_mrdc_xdd_diff_v1560.to_json(j);
  }
  j.end_obj();
}

// PDCP-ParametersMRDC ::= SEQUENCE
SRSASN_CODE pdcp_params_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcp_params_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
void pdcp_params_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_dupl_split_srb_present) {
    j.write_str("pdcp-DuplicationSplitSRB", "supported");
  }
  if (pdcp_dupl_split_drb_present) {
    j.write_str("pdcp-DuplicationSplitDRB", "supported");
  }
  j.end_obj();
}

// UE-MRDC-Capability-v1560 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rx_filts.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_v1560_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rx_filts.size() > 0) {
    HANDLE_CODE(rx_filts.pack(bref));
  }
  if (meas_and_mob_params_mrdc_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1560.pack(bref));
  }
  if (fdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap_v1560.pack(bref));
  }
  if (tdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap_v1560.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_v1560_s::unpack(cbit_ref& bref)
{
  bool rx_filts_present;
  HANDLE_CODE(bref.unpack(rx_filts_present, 1));
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_v1560_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_mrdc_cap_v1560_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rx_filts_present) {
    HANDLE_CODE(rx_filts.unpack(bref));
  }
  if (meas_and_mob_params_mrdc_v1560_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc_v1560.unpack(bref));
  }
  if (fdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap_v1560.unpack(bref));
  }
  if (tdd_add_ue_mrdc_cap_v1560_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap_v1560.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rx_filts.size() > 0) {
    j.write_str("receivedFilters", rx_filts.to_string());
  }
  if (meas_and_mob_params_mrdc_v1560_present) {
    j.write_fieldname("measAndMobParametersMRDC-v1560");
    meas_and_mob_params_mrdc_v1560.to_json(j);
  }
  if (fdd_add_ue_mrdc_cap_v1560_present) {
    j.write_fieldname("fdd-Add-UE-MRDC-Capabilities-v1560");
    fdd_add_ue_mrdc_cap_v1560.to_json(j);
  }
  if (tdd_add_ue_mrdc_cap_v1560_present) {
    j.write_fieldname("tdd-Add-UE-MRDC-Capabilities-v1560");
    tdd_add_ue_mrdc_cap_v1560.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-MRDC-Capability ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_and_mob_params_mrdc_present, 1));
  HANDLE_CODE(bref.pack(phy_params_mrdc_v1530_present, 1));
  HANDLE_CODE(bref.pack(general_params_mrdc_present, 1));
  HANDLE_CODE(bref.pack(fdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(tdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(fr1_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(fr2_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.pack(feature_set_combinations.size() > 0, 1));
  HANDLE_CODE(bref.pack(pdcp_params_mrdc_v1530_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc.pack(bref));
  }
  if (phy_params_mrdc_v1530_present) {
    HANDLE_CODE(phy_params_mrdc_v1530.pack(bref));
  }
  HANDLE_CODE(rf_params_mrdc.pack(bref));
  if (general_params_mrdc_present) {
    HANDLE_CODE(general_params_mrdc.pack(bref));
  }
  if (fdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap.pack(bref));
  }
  if (tdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap.pack(bref));
  }
  if (fr1_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr1_add_ue_mrdc_cap.pack(bref));
  }
  if (fr2_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr2_add_ue_mrdc_cap.pack(bref));
  }
  if (feature_set_combinations.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                feature_set_combinations,
                                1,
                                1024,
                                SeqOfPacker<SeqOfPacker<Packer>>(1, 32, SeqOfPacker<Packer>(1, 128, Packer()))));
  }
  if (pdcp_params_mrdc_v1530_present) {
    HANDLE_CODE(pdcp_params_mrdc_v1530.pack(bref));
  }
  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_and_mob_params_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(phy_params_mrdc_v1530_present, 1));
  HANDLE_CODE(bref.unpack(general_params_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(fdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(tdd_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr1_add_ue_mrdc_cap_present, 1));
  HANDLE_CODE(bref.unpack(fr2_add_ue_mrdc_cap_present, 1));
  bool feature_set_combinations_present;
  HANDLE_CODE(bref.unpack(feature_set_combinations_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_params_mrdc_v1530_present, 1));
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_and_mob_params_mrdc_present) {
    HANDLE_CODE(meas_and_mob_params_mrdc.unpack(bref));
  }
  if (phy_params_mrdc_v1530_present) {
    HANDLE_CODE(phy_params_mrdc_v1530.unpack(bref));
  }
  HANDLE_CODE(rf_params_mrdc.unpack(bref));
  if (general_params_mrdc_present) {
    HANDLE_CODE(general_params_mrdc.unpack(bref));
  }
  if (fdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fdd_add_ue_mrdc_cap.unpack(bref));
  }
  if (tdd_add_ue_mrdc_cap_present) {
    HANDLE_CODE(tdd_add_ue_mrdc_cap.unpack(bref));
  }
  if (fr1_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr1_add_ue_mrdc_cap.unpack(bref));
  }
  if (fr2_add_ue_mrdc_cap_present) {
    HANDLE_CODE(fr2_add_ue_mrdc_cap.unpack(bref));
  }
  if (feature_set_combinations_present) {
    HANDLE_CODE(unpack_dyn_seq_of(feature_set_combinations,
                                  bref,
                                  1,
                                  1024,
                                  SeqOfPacker<SeqOfPacker<Packer>>(1, 32, SeqOfPacker<Packer>(1, 128, Packer()))));
  }
  if (pdcp_params_mrdc_v1530_present) {
    HANDLE_CODE(pdcp_params_mrdc_v1530.unpack(bref));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_and_mob_params_mrdc_present) {
    j.write_fieldname("measAndMobParametersMRDC");
    meas_and_mob_params_mrdc.to_json(j);
  }
  if (phy_params_mrdc_v1530_present) {
    j.write_fieldname("phy-ParametersMRDC-v1530");
    phy_params_mrdc_v1530.to_json(j);
  }
  j.write_fieldname("rf-ParametersMRDC");
  rf_params_mrdc.to_json(j);
  if (general_params_mrdc_present) {
    j.write_fieldname("generalParametersMRDC");
    general_params_mrdc.to_json(j);
  }
  if (fdd_add_ue_mrdc_cap_present) {
    j.write_fieldname("fdd-Add-UE-MRDC-Capabilities");
    fdd_add_ue_mrdc_cap.to_json(j);
  }
  if (tdd_add_ue_mrdc_cap_present) {
    j.write_fieldname("tdd-Add-UE-MRDC-Capabilities");
    tdd_add_ue_mrdc_cap.to_json(j);
  }
  if (fr1_add_ue_mrdc_cap_present) {
    j.write_fieldname("fr1-Add-UE-MRDC-Capabilities");
    fr1_add_ue_mrdc_cap.to_json(j);
  }
  if (fr2_add_ue_mrdc_cap_present) {
    j.write_fieldname("fr2-Add-UE-MRDC-Capabilities");
    fr2_add_ue_mrdc_cap.to_json(j);
  }
  if (feature_set_combinations.size() > 0) {
    j.start_array("featureSetCombinations");
    for (const auto& e1 : feature_set_combinations) {
      j.start_array();
      for (const auto& e2 : e1) {
        j.start_array();
        for (const auto& e3 : e2) {
          e3.to_json(j);
        }
        j.end_array();
      }
      j.end_array();
    }
    j.end_array();
  }
  if (pdcp_params_mrdc_v1530_present) {
    j.write_fieldname("pdcp-ParametersMRDC-v1530");
    pdcp_params_mrdc_v1530.to_json(j);
  }
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-MRDC-Capability-v15g0 ::= SEQUENCE
SRSASN_CODE ue_mrdc_cap_v15g0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_mrdc_v15g0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_mrdc_v15g0_present) {
    HANDLE_CODE(rf_params_mrdc_v15g0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_mrdc_cap_v15g0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_mrdc_v15g0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_mrdc_v15g0_present) {
    HANDLE_CODE(rf_params_mrdc_v15g0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_mrdc_cap_v15g0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_mrdc_v15g0_present) {
    j.write_fieldname("rf-ParametersMRDC-v15g0");
    rf_params_mrdc_v15g0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* high_speed_params_v1650_c::types_opts::to_string() const
{
  static const char* names[] = {"intraNR-MeasurementEnhancement-r16", "interRAT-MeasurementEnhancement-r16"};
  return convert_enum_idx(names, 2, value, "high_speed_params_v1650_c::types");
}

// UE-NR-Capability-v16c0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v16c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v16c0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v16c0_present) {
    HANDLE_CODE(rf_params_v16c0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v16c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v16c0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v16c0_present) {
    HANDLE_CODE(rf_params_v16c0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v16c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v16c0_present) {
    j.write_fieldname("rf-Parameters-v16c0");
    rf_params_v16c0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UE-NR-Capability-v16a0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v16a0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(phy_params_v16a0_present, 1));
  HANDLE_CODE(bref.pack(rf_params_v16a0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (phy_params_v16a0_present) {
    HANDLE_CODE(phy_params_v16a0.pack(bref));
  }
  if (rf_params_v16a0_present) {
    HANDLE_CODE(rf_params_v16a0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v16a0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(phy_params_v16a0_present, 1));
  HANDLE_CODE(bref.unpack(rf_params_v16a0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (phy_params_v16a0_present) {
    HANDLE_CODE(phy_params_v16a0.unpack(bref));
  }
  if (rf_params_v16a0_present) {
    HANDLE_CODE(rf_params_v16a0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v16a0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (phy_params_v16a0_present) {
    j.write_fieldname("phy-Parameters-v16a0");
    phy_params_v16a0.to_json(j);
  }
  if (rf_params_v16a0_present) {
    j.write_fieldname("rf-Parameters-v16a0");
    rf_params_v16a0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-NR-Capability-v15j0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v15j0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext.size() > 0) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v15j0_s::unpack(cbit_ref& bref)
{
  bool late_non_crit_ext_present;
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v15j0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext.size() > 0) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// NRDC-Parameters-v15c0 ::= SEQUENCE
SRSASN_CODE nrdc_params_v15c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.pack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nrdc_params_v15c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_srb_present, 1));
  HANDLE_CODE(bref.unpack(pdcp_dupl_split_drb_present, 1));

  return SRSASN_SUCCESS;
}
void nrdc_params_v15c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdcp_dupl_split_srb_present) {
    j.write_str("pdcp-DuplicationSplitSRB", "supported");
  }
  if (pdcp_dupl_split_drb_present) {
    j.write_str("pdcp-DuplicationSplitDRB", "supported");
  }
  j.end_obj();
}

// UE-NR-Capability-v15g0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v15g0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rf_params_v15g0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rf_params_v15g0_present) {
    HANDLE_CODE(rf_params_v15g0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v15g0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rf_params_v15g0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rf_params_v15g0_present) {
    HANDLE_CODE(rf_params_v15g0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v15g0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rf_params_v15g0_present) {
    j.write_fieldname("rf-Parameters-v15g0");
    rf_params_v15g0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UE-NR-Capability-v15c0 ::= SEQUENCE
SRSASN_CODE ue_nr_cap_v15c0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nrdc_params_v15c0_present, 1));
  HANDLE_CODE(bref.pack(partial_fr2_fallback_rx_req_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (nrdc_params_v15c0_present) {
    HANDLE_CODE(nrdc_params_v15c0.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_nr_cap_v15c0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nrdc_params_v15c0_present, 1));
  HANDLE_CODE(bref.unpack(partial_fr2_fallback_rx_req_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (nrdc_params_v15c0_present) {
    HANDLE_CODE(nrdc_params_v15c0.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_nr_cap_v15c0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (nrdc_params_v15c0_present) {
    j.write_fieldname("nrdc-Parameters-v15c0");
    nrdc_params_v15c0.to_json(j);
  }
  if (partial_fr2_fallback_rx_req_present) {
    j.write_str("partialFR2-FallbackRX-Req", "true");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1610-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rrc_seg_allowed_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rrc_seg_allowed_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rrc_seg_allowed_r16_present) {
    j.write_str("rrc-SegAllowed-r16", "enabled");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UECapabilityEnquiry-v1560-IEs ::= SEQUENCE
SRSASN_CODE ue_cap_enquiry_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cap_request_filt_common_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cap_request_filt_common_present) {
    HANDLE_CODE(cap_request_filt_common.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_cap_enquiry_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(cap_request_filt_common_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cap_request_filt_common_present) {
    HANDLE_CODE(cap_request_filt_common.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_cap_enquiry_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cap_request_filt_common_present) {
    j.write_fieldname("capabilityRequestFilterCommon");
    cap_request_filt_common.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// AS-Config ::= SEQUENCE
SRSASN_CODE as_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(rrc_recfg.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= source_rb_sn_cfg.size() > 0;
    group_flags[0] |= source_scg_nr_cfg.size() > 0;
    group_flags[0] |= source_scg_eutra_cfg.size() > 0;
    group_flags[1] |= source_scg_cfg_present;
    group_flags[2] |= sdt_cfg_r17.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_rb_sn_cfg.size() > 0, 1));
      HANDLE_CODE(bref.pack(source_scg_nr_cfg.size() > 0, 1));
      HANDLE_CODE(bref.pack(source_scg_eutra_cfg.size() > 0, 1));
      if (source_rb_sn_cfg.size() > 0) {
        HANDLE_CODE(source_rb_sn_cfg.pack(bref));
      }
      if (source_scg_nr_cfg.size() > 0) {
        HANDLE_CODE(source_scg_nr_cfg.pack(bref));
      }
      if (source_scg_eutra_cfg.size() > 0) {
        HANDLE_CODE(source_scg_eutra_cfg.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(source_scg_cfg_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sdt_cfg_r17.is_present(), 1));
      if (sdt_cfg_r17.is_present()) {
        HANDLE_CODE(sdt_cfg_r17->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(rrc_recfg.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool source_rb_sn_cfg_present;
      HANDLE_CODE(bref.unpack(source_rb_sn_cfg_present, 1));
      bool source_scg_nr_cfg_present;
      HANDLE_CODE(bref.unpack(source_scg_nr_cfg_present, 1));
      bool source_scg_eutra_cfg_present;
      HANDLE_CODE(bref.unpack(source_scg_eutra_cfg_present, 1));
      if (source_rb_sn_cfg_present) {
        HANDLE_CODE(source_rb_sn_cfg.unpack(bref));
      }
      if (source_scg_nr_cfg_present) {
        HANDLE_CODE(source_scg_nr_cfg.unpack(bref));
      }
      if (source_scg_eutra_cfg_present) {
        HANDLE_CODE(source_scg_eutra_cfg.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(source_scg_cfg_present, 1));
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sdt_cfg_r17, bref);
      if (sdt_cfg_r17.is_present()) {
        HANDLE_CODE(sdt_cfg_r17->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void as_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("rrcReconfiguration", rrc_recfg.to_string());
  if (ext) {
    if (source_rb_sn_cfg.size() > 0) {
      j.write_str("sourceRB-SN-Config", source_rb_sn_cfg.to_string());
    }
    if (source_scg_nr_cfg.size() > 0) {
      j.write_str("sourceSCG-NR-Config", source_scg_nr_cfg.to_string());
    }
    if (source_scg_eutra_cfg.size() > 0) {
      j.write_str("sourceSCG-EUTRA-Config", source_scg_eutra_cfg.to_string());
    }
    if (source_scg_cfg_present) {
      j.write_str("sourceSCG-Configured", "true");
    }
    if (sdt_cfg_r17.is_present()) {
      j.write_fieldname("sdt-Config-r17");
      sdt_cfg_r17->to_json(j);
    }
  }
  j.end_obj();
}

// BandCombinationInfo ::= SEQUENCE
SRSASN_CODE band_combination_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_combination_idx, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(pack_dyn_seq_of(bref, allowed_feature_sets_list, 1, 128, integer_packer<uint8_t>(1, 128)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_combination_idx, bref, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(unpack_dyn_seq_of(allowed_feature_sets_list, bref, 1, 128, integer_packer<uint8_t>(1, 128)));

  return SRSASN_SUCCESS;
}
void band_combination_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandCombinationIndex", band_combination_idx);
  j.start_array("allowedFeatureSetsList");
  for (const auto& e1 : allowed_feature_sets_list) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// ReestabNCellInfo ::= SEQUENCE
SRSASN_CODE reestab_ncell_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_id.pack(bref));
  HANDLE_CODE(key_g_node_b_star.pack(bref));
  HANDLE_CODE(short_mac_i.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reestab_ncell_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_id.unpack(bref));
  HANDLE_CODE(key_g_node_b_star.unpack(bref));
  HANDLE_CODE(short_mac_i.unpack(bref));

  return SRSASN_SUCCESS;
}
void reestab_ncell_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellIdentity", cell_id.to_string());
  j.write_str("key-gNodeB-Star", key_g_node_b_star.to_string());
  j.write_str("shortMAC-I", short_mac_i.to_string());
  j.end_obj();
}

// T-Offset-r16 ::= ENUMERATED
const char* t_offset_r16_opts::to_string() const
{
  static const char* names[] = {"ms0dot5", "ms0dot75", "ms1", "ms1dot5", "ms2", "ms2dot5", "ms3", "spare1"};
  return convert_enum_idx(names, 8, value, "t_offset_r16_e");
}
float t_offset_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0};
  return map_enum_number(numbers, 7, value, "t_offset_r16_e");
}
const char* t_offset_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "0.75", "1", "1.5", "2", "2.5", "3"};
  return convert_enum_idx(number_strs, 8, value, "t_offset_r16_e");
}

// BandCombinationInfoSN ::= SEQUENCE
SRSASN_CODE band_combination_info_sn_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, band_combination_idx, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(pack_integer(bref, requested_feature_sets, (uint8_t)1u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE band_combination_info_sn_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(band_combination_idx, bref, (uint32_t)1u, (uint32_t)65536u));
  HANDLE_CODE(unpack_integer(requested_feature_sets, bref, (uint8_t)1u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
void band_combination_info_sn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("bandCombinationIndex", band_combination_idx);
  j.write_int("requestedFeatureSets", requested_feature_sets);
  j.end_obj();
}

// ConfigRestrictInfoDAPS-r16 ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_daps_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pwr_coordination_r16_present, 1));

  if (pwr_coordination_r16_present) {
    HANDLE_CODE(pack_integer(bref, pwr_coordination_r16.p_daps_source_r16, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(pack_integer(bref, pwr_coordination_r16.p_daps_target_r16, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(pwr_coordination_r16.ul_pwr_sharing_daps_mode_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_daps_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pwr_coordination_r16_present, 1));

  if (pwr_coordination_r16_present) {
    HANDLE_CODE(unpack_integer(pwr_coordination_r16.p_daps_source_r16, bref, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(unpack_integer(pwr_coordination_r16.p_daps_target_r16, bref, (int8_t)-30, (int8_t)33));
    HANDLE_CODE(pwr_coordination_r16.ul_pwr_sharing_daps_mode_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cfg_restrict_info_daps_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pwr_coordination_r16_present) {
    j.write_fieldname("powerCoordination-r16");
    j.start_obj();
    j.write_int("p-DAPS-Source-r16", pwr_coordination_r16.p_daps_source_r16);
    j.write_int("p-DAPS-Target-r16", pwr_coordination_r16.p_daps_target_r16);
    j.write_str("uplinkPowerSharingDAPS-Mode-r16", pwr_coordination_r16.ul_pwr_sharing_daps_mode_r16.to_string());
    j.end_obj();
  }
  j.end_obj();
}

const char* cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(
      names, 3, value, "cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_e_");
}
uint8_t cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_e_");
}

// ConfigRestrictInfoDAPS-v1640 ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_daps_v1640_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, source_feature_set_per_dl_cc_r16, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(pack_integer(bref, source_feature_set_per_ul_cc_r16, (uint16_t)1u, (uint16_t)1024u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_daps_v1640_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(source_feature_set_per_dl_cc_r16, bref, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(unpack_integer(source_feature_set_per_ul_cc_r16, bref, (uint16_t)1u, (uint16_t)1024u));

  return SRSASN_SUCCESS;
}
void cfg_restrict_info_daps_v1640_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourceFeatureSetPerDownlinkCC-r16", source_feature_set_per_dl_cc_r16);
  j.write_int("sourceFeatureSetPerUplinkCC-r16", source_feature_set_per_ul_cc_r16);
  j.end_obj();
}

// ConfigRestrictInfoSCG ::= SEQUENCE
SRSASN_CODE cfg_restrict_info_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(allowed_bc_list_mrdc.size() > 0, 1));
  HANDLE_CODE(bref.pack(pwr_coordination_fr1_present, 1));
  HANDLE_CODE(bref.pack(serv_cell_idx_range_scg_present, 1));
  HANDLE_CODE(bref.pack(max_meas_freqs_scg_present, 1));
  HANDLE_CODE(bref.pack(dummy_present, 1));

  if (allowed_bc_list_mrdc.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, allowed_bc_list_mrdc, 1, 65536));
  }
  if (pwr_coordination_fr1_present) {
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_nr_fr1_present, 1));
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_eutra_present, 1));
    HANDLE_CODE(bref.pack(pwr_coordination_fr1.p_max_ue_fr1_present, 1));
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_nr_fr1, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_eutra, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      HANDLE_CODE(pack_integer(bref, pwr_coordination_fr1.p_max_ue_fr1, (int8_t)-30, (int8_t)33));
    }
  }
  if (serv_cell_idx_range_scg_present) {
    HANDLE_CODE(pack_integer(bref, serv_cell_idx_range_scg.low_bound, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(pack_integer(bref, serv_cell_idx_range_scg.up_bound, (uint8_t)0u, (uint8_t)31u));
  }
  if (max_meas_freqs_scg_present) {
    HANDLE_CODE(pack_integer(bref, max_meas_freqs_scg, (uint8_t)1u, (uint8_t)32u));
  }
  if (dummy_present) {
    HANDLE_CODE(pack_integer(bref, dummy, (uint8_t)1u, (uint8_t)62u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sel_band_entries_mn_list.is_present();
    group_flags[0] |= pdcch_blind_detection_scg_present;
    group_flags[0] |= max_num_rohc_context_sessions_sn_present;
    group_flags[1] |= max_intra_freq_meas_identities_scg_present;
    group_flags[1] |= max_inter_freq_meas_identities_scg_present;
    group_flags[2] |= p_max_nr_fr1_mcg_r16_present;
    group_flags[2] |= pwr_coordination_fr2_r16.is_present();
    group_flags[2] |= nrdc_pc_mode_fr1_r16_present;
    group_flags[2] |= nrdc_pc_mode_fr2_r16_present;
    group_flags[2] |= max_meas_srs_res_scg_r16_present;
    group_flags[2] |= max_meas_cli_res_scg_r16_present;
    group_flags[2] |= max_num_ehc_contexts_sn_r16_present;
    group_flags[2] |= allowed_reduced_cfg_for_overheat_r16.is_present();
    group_flags[2] |= max_toffset_r16_present;
    group_flags[3] |= allowed_reduced_cfg_for_overheat_r17.is_present();
    group_flags[3] |= max_num_udc_drb_r17_present;
    group_flags[3] |= max_num_cp_cc_andidates_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sel_band_entries_mn_list.is_present(), 1));
      HANDLE_CODE(bref.pack(pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.pack(max_num_rohc_context_sessions_sn_present, 1));
      if (sel_band_entries_mn_list.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref,
                                    *sel_band_entries_mn_list,
                                    1,
                                    65536,
                                    SeqOfPacker<integer_packer<uint8_t>>(1, 32, integer_packer<uint8_t>(0, 32))));
      }
      if (pdcch_blind_detection_scg_present) {
        HANDLE_CODE(pack_integer(bref, pdcch_blind_detection_scg, (uint8_t)1u, (uint8_t)15u));
      }
      if (max_num_rohc_context_sessions_sn_present) {
        HANDLE_CODE(pack_integer(bref, max_num_rohc_context_sessions_sn, (uint16_t)0u, (uint16_t)16384u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(max_intra_freq_meas_identities_scg_present, 1));
      HANDLE_CODE(bref.pack(max_inter_freq_meas_identities_scg_present, 1));
      if (max_intra_freq_meas_identities_scg_present) {
        HANDLE_CODE(pack_integer(bref, max_intra_freq_meas_identities_scg, (uint8_t)1u, (uint8_t)62u));
      }
      if (max_inter_freq_meas_identities_scg_present) {
        HANDLE_CODE(pack_integer(bref, max_inter_freq_meas_identities_scg, (uint8_t)1u, (uint8_t)62u));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(p_max_nr_fr1_mcg_r16_present, 1));
      HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(nrdc_pc_mode_fr1_r16_present, 1));
      HANDLE_CODE(bref.pack(nrdc_pc_mode_fr2_r16_present, 1));
      HANDLE_CODE(bref.pack(max_meas_srs_res_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(max_meas_cli_res_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(max_num_ehc_contexts_sn_r16_present, 1));
      HANDLE_CODE(bref.pack(allowed_reduced_cfg_for_overheat_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(max_toffset_r16_present, 1));
      if (p_max_nr_fr1_mcg_r16_present) {
        HANDLE_CODE(pack_integer(bref, p_max_nr_fr1_mcg_r16, (int8_t)-30, (int8_t)33));
      }
      if (pwr_coordination_fr2_r16.is_present()) {
        HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present, 1));
        HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present, 1));
        HANDLE_CODE(bref.pack(pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present, 1));
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present) {
          HANDLE_CODE(pack_integer(bref, pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present) {
          HANDLE_CODE(pack_integer(bref, pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present) {
          HANDLE_CODE(pack_integer(bref, pwr_coordination_fr2_r16->p_max_ue_fr2_r16, (int8_t)-30, (int8_t)33));
        }
      }
      if (nrdc_pc_mode_fr1_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr1_r16.pack(bref));
      }
      if (nrdc_pc_mode_fr2_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr2_r16.pack(bref));
      }
      if (max_meas_srs_res_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, max_meas_srs_res_scg_r16, (uint8_t)0u, (uint8_t)32u));
      }
      if (max_meas_cli_res_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, max_meas_cli_res_scg_r16, (uint8_t)0u, (uint8_t)64u));
      }
      if (max_num_ehc_contexts_sn_r16_present) {
        HANDLE_CODE(pack_integer(bref, max_num_ehc_contexts_sn_r16, (uint32_t)0u, (uint32_t)65536u));
      }
      if (allowed_reduced_cfg_for_overheat_r16.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r16->pack(bref));
      }
      if (max_toffset_r16_present) {
        HANDLE_CODE(max_toffset_r16.pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(allowed_reduced_cfg_for_overheat_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(max_num_udc_drb_r17_present, 1));
      HANDLE_CODE(bref.pack(max_num_cp_cc_andidates_r17_present, 1));
      if (allowed_reduced_cfg_for_overheat_r17.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r17->pack(bref));
      }
      if (max_num_udc_drb_r17_present) {
        HANDLE_CODE(pack_integer(bref, max_num_udc_drb_r17, (uint8_t)0u, (uint8_t)2u));
      }
      if (max_num_cp_cc_andidates_r17_present) {
        HANDLE_CODE(pack_integer(bref, max_num_cp_cc_andidates_r17, (uint8_t)0u, (uint8_t)7u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_info_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool allowed_bc_list_mrdc_present;
  HANDLE_CODE(bref.unpack(allowed_bc_list_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(pwr_coordination_fr1_present, 1));
  HANDLE_CODE(bref.unpack(serv_cell_idx_range_scg_present, 1));
  HANDLE_CODE(bref.unpack(max_meas_freqs_scg_present, 1));
  HANDLE_CODE(bref.unpack(dummy_present, 1));

  if (allowed_bc_list_mrdc_present) {
    HANDLE_CODE(unpack_dyn_seq_of(allowed_bc_list_mrdc, bref, 1, 65536));
  }
  if (pwr_coordination_fr1_present) {
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_nr_fr1_present, 1));
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_eutra_present, 1));
    HANDLE_CODE(bref.unpack(pwr_coordination_fr1.p_max_ue_fr1_present, 1));
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_nr_fr1, bref, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_eutra, bref, (int8_t)-30, (int8_t)33));
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      HANDLE_CODE(unpack_integer(pwr_coordination_fr1.p_max_ue_fr1, bref, (int8_t)-30, (int8_t)33));
    }
  }
  if (serv_cell_idx_range_scg_present) {
    HANDLE_CODE(unpack_integer(serv_cell_idx_range_scg.low_bound, bref, (uint8_t)0u, (uint8_t)31u));
    HANDLE_CODE(unpack_integer(serv_cell_idx_range_scg.up_bound, bref, (uint8_t)0u, (uint8_t)31u));
  }
  if (max_meas_freqs_scg_present) {
    HANDLE_CODE(unpack_integer(max_meas_freqs_scg, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (dummy_present) {
    HANDLE_CODE(unpack_integer(dummy, bref, (uint8_t)1u, (uint8_t)62u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(4);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sel_band_entries_mn_list, bref);
      HANDLE_CODE(bref.unpack(pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.unpack(max_num_rohc_context_sessions_sn_present, 1));
      if (sel_band_entries_mn_list.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sel_band_entries_mn_list,
                                      bref,
                                      1,
                                      65536,
                                      SeqOfPacker<integer_packer<uint8_t>>(1, 32, integer_packer<uint8_t>(0, 32))));
      }
      if (pdcch_blind_detection_scg_present) {
        HANDLE_CODE(unpack_integer(pdcch_blind_detection_scg, bref, (uint8_t)1u, (uint8_t)15u));
      }
      if (max_num_rohc_context_sessions_sn_present) {
        HANDLE_CODE(unpack_integer(max_num_rohc_context_sessions_sn, bref, (uint16_t)0u, (uint16_t)16384u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(max_intra_freq_meas_identities_scg_present, 1));
      HANDLE_CODE(bref.unpack(max_inter_freq_meas_identities_scg_present, 1));
      if (max_intra_freq_meas_identities_scg_present) {
        HANDLE_CODE(unpack_integer(max_intra_freq_meas_identities_scg, bref, (uint8_t)1u, (uint8_t)62u));
      }
      if (max_inter_freq_meas_identities_scg_present) {
        HANDLE_CODE(unpack_integer(max_inter_freq_meas_identities_scg, bref, (uint8_t)1u, (uint8_t)62u));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(p_max_nr_fr1_mcg_r16_present, 1));
      unpack_presence_flag(pwr_coordination_fr2_r16, bref);
      HANDLE_CODE(bref.unpack(nrdc_pc_mode_fr1_r16_present, 1));
      HANDLE_CODE(bref.unpack(nrdc_pc_mode_fr2_r16_present, 1));
      HANDLE_CODE(bref.unpack(max_meas_srs_res_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(max_meas_cli_res_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(max_num_ehc_contexts_sn_r16_present, 1));
      unpack_presence_flag(allowed_reduced_cfg_for_overheat_r16, bref);
      HANDLE_CODE(bref.unpack(max_toffset_r16_present, 1));
      if (p_max_nr_fr1_mcg_r16_present) {
        HANDLE_CODE(unpack_integer(p_max_nr_fr1_mcg_r16, bref, (int8_t)-30, (int8_t)33));
      }
      if (pwr_coordination_fr2_r16.is_present()) {
        HANDLE_CODE(bref.unpack(pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present, 1));
        HANDLE_CODE(bref.unpack(pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present, 1));
        HANDLE_CODE(bref.unpack(pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present, 1));
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present) {
          HANDLE_CODE(unpack_integer(pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16, bref, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present) {
          HANDLE_CODE(unpack_integer(pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16, bref, (int8_t)-30, (int8_t)33));
        }
        if (pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present) {
          HANDLE_CODE(unpack_integer(pwr_coordination_fr2_r16->p_max_ue_fr2_r16, bref, (int8_t)-30, (int8_t)33));
        }
      }
      if (nrdc_pc_mode_fr1_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr1_r16.unpack(bref));
      }
      if (nrdc_pc_mode_fr2_r16_present) {
        HANDLE_CODE(nrdc_pc_mode_fr2_r16.unpack(bref));
      }
      if (max_meas_srs_res_scg_r16_present) {
        HANDLE_CODE(unpack_integer(max_meas_srs_res_scg_r16, bref, (uint8_t)0u, (uint8_t)32u));
      }
      if (max_meas_cli_res_scg_r16_present) {
        HANDLE_CODE(unpack_integer(max_meas_cli_res_scg_r16, bref, (uint8_t)0u, (uint8_t)64u));
      }
      if (max_num_ehc_contexts_sn_r16_present) {
        HANDLE_CODE(unpack_integer(max_num_ehc_contexts_sn_r16, bref, (uint32_t)0u, (uint32_t)65536u));
      }
      if (allowed_reduced_cfg_for_overheat_r16.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r16->unpack(bref));
      }
      if (max_toffset_r16_present) {
        HANDLE_CODE(max_toffset_r16.unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(allowed_reduced_cfg_for_overheat_r17, bref);
      HANDLE_CODE(bref.unpack(max_num_udc_drb_r17_present, 1));
      HANDLE_CODE(bref.unpack(max_num_cp_cc_andidates_r17_present, 1));
      if (allowed_reduced_cfg_for_overheat_r17.is_present()) {
        HANDLE_CODE(allowed_reduced_cfg_for_overheat_r17->unpack(bref));
      }
      if (max_num_udc_drb_r17_present) {
        HANDLE_CODE(unpack_integer(max_num_udc_drb_r17, bref, (uint8_t)0u, (uint8_t)2u));
      }
      if (max_num_cp_cc_andidates_r17_present) {
        HANDLE_CODE(unpack_integer(max_num_cp_cc_andidates_r17, bref, (uint8_t)0u, (uint8_t)7u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cfg_restrict_info_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (allowed_bc_list_mrdc.size() > 0) {
    j.start_array("allowedBC-ListMRDC");
    for (const auto& e1 : allowed_bc_list_mrdc) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pwr_coordination_fr1_present) {
    j.write_fieldname("powerCoordination-FR1");
    j.start_obj();
    if (pwr_coordination_fr1.p_max_nr_fr1_present) {
      j.write_int("p-maxNR-FR1", pwr_coordination_fr1.p_max_nr_fr1);
    }
    if (pwr_coordination_fr1.p_max_eutra_present) {
      j.write_int("p-maxEUTRA", pwr_coordination_fr1.p_max_eutra);
    }
    if (pwr_coordination_fr1.p_max_ue_fr1_present) {
      j.write_int("p-maxUE-FR1", pwr_coordination_fr1.p_max_ue_fr1);
    }
    j.end_obj();
  }
  if (serv_cell_idx_range_scg_present) {
    j.write_fieldname("servCellIndexRangeSCG");
    j.start_obj();
    j.write_int("lowBound", serv_cell_idx_range_scg.low_bound);
    j.write_int("upBound", serv_cell_idx_range_scg.up_bound);
    j.end_obj();
  }
  if (max_meas_freqs_scg_present) {
    j.write_int("maxMeasFreqsSCG", max_meas_freqs_scg);
  }
  if (dummy_present) {
    j.write_int("dummy", dummy);
  }
  if (ext) {
    if (sel_band_entries_mn_list.is_present()) {
      j.start_array("selectedBandEntriesMNList");
      for (const auto& e1 : *sel_band_entries_mn_list) {
        j.start_array();
        for (const auto& e2 : e1) {
          j.write_int(e2);
        }
        j.end_array();
      }
      j.end_array();
    }
    if (pdcch_blind_detection_scg_present) {
      j.write_int("pdcch-BlindDetectionSCG", pdcch_blind_detection_scg);
    }
    if (max_num_rohc_context_sessions_sn_present) {
      j.write_int("maxNumberROHC-ContextSessionsSN", max_num_rohc_context_sessions_sn);
    }
    if (max_intra_freq_meas_identities_scg_present) {
      j.write_int("maxIntraFreqMeasIdentitiesSCG", max_intra_freq_meas_identities_scg);
    }
    if (max_inter_freq_meas_identities_scg_present) {
      j.write_int("maxInterFreqMeasIdentitiesSCG", max_inter_freq_meas_identities_scg);
    }
    if (p_max_nr_fr1_mcg_r16_present) {
      j.write_int("p-maxNR-FR1-MCG-r16", p_max_nr_fr1_mcg_r16);
    }
    if (pwr_coordination_fr2_r16.is_present()) {
      j.write_fieldname("powerCoordination-FR2-r16");
      j.start_obj();
      if (pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16_present) {
        j.write_int("p-maxNR-FR2-MCG-r16", pwr_coordination_fr2_r16->p_max_nr_fr2_mcg_r16);
      }
      if (pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16_present) {
        j.write_int("p-maxNR-FR2-SCG-r16", pwr_coordination_fr2_r16->p_max_nr_fr2_scg_r16);
      }
      if (pwr_coordination_fr2_r16->p_max_ue_fr2_r16_present) {
        j.write_int("p-maxUE-FR2-r16", pwr_coordination_fr2_r16->p_max_ue_fr2_r16);
      }
      j.end_obj();
    }
    if (nrdc_pc_mode_fr1_r16_present) {
      j.write_str("nrdc-PC-mode-FR1-r16", nrdc_pc_mode_fr1_r16.to_string());
    }
    if (nrdc_pc_mode_fr2_r16_present) {
      j.write_str("nrdc-PC-mode-FR2-r16", nrdc_pc_mode_fr2_r16.to_string());
    }
    if (max_meas_srs_res_scg_r16_present) {
      j.write_int("maxMeasSRS-ResourceSCG-r16", max_meas_srs_res_scg_r16);
    }
    if (max_meas_cli_res_scg_r16_present) {
      j.write_int("maxMeasCLI-ResourceSCG-r16", max_meas_cli_res_scg_r16);
    }
    if (max_num_ehc_contexts_sn_r16_present) {
      j.write_int("maxNumberEHC-ContextsSN-r16", max_num_ehc_contexts_sn_r16);
    }
    if (allowed_reduced_cfg_for_overheat_r16.is_present()) {
      j.write_fieldname("allowedReducedConfigForOverheating-r16");
      allowed_reduced_cfg_for_overheat_r16->to_json(j);
    }
    if (max_toffset_r16_present) {
      j.write_str("maxToffset-r16", max_toffset_r16.to_string());
    }
    if (allowed_reduced_cfg_for_overheat_r17.is_present()) {
      j.write_fieldname("allowedReducedConfigForOverheating-r17");
      allowed_reduced_cfg_for_overheat_r17->to_json(j);
    }
    if (max_num_udc_drb_r17_present) {
      j.write_int("maxNumberUDC-DRB-r17", max_num_udc_drb_r17);
    }
    if (max_num_cp_cc_andidates_r17_present) {
      j.write_int("maxNumberCPCCandidates-r17", max_num_cp_cc_andidates_r17);
    }
  }
  j.end_obj();
}

const char* cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(names, 3, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_e_");
}
uint8_t cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr1_r16_e_");
}

const char* cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_opts::to_string() const
{
  static const char* names[] = {"semi-static-mode1", "semi-static-mode2", "dynamic"};
  return convert_enum_idx(names, 3, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_e_");
}
uint8_t cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "cfg_restrict_info_scg_s::nrdc_pc_mode_fr2_r16_e_");
}

// ReestablishmentInfo ::= SEQUENCE
SRSASN_CODE reest_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(add_reestab_info_list.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, source_pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_short_mac_i.pack(bref));
  if (add_reestab_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, add_reestab_info_list, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reest_info_s::unpack(cbit_ref& bref)
{
  bool add_reestab_info_list_present;
  HANDLE_CODE(bref.unpack(add_reestab_info_list_present, 1));

  HANDLE_CODE(unpack_integer(source_pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(target_cell_short_mac_i.unpack(bref));
  if (add_reestab_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(add_reestab_info_list, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void reest_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sourcePhysCellId", source_pci);
  j.write_str("targetCellShortMAC-I", target_cell_short_mac_i.to_string());
  if (add_reestab_info_list.size() > 0) {
    j.start_array("additionalReestabInfoList");
    for (const auto& e1 : add_reestab_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// AS-Context ::= SEQUENCE
SRSASN_CODE as_context_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(reest_info_present, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_info_present, 1));

  if (reest_info_present) {
    HANDLE_CODE(reest_info.pack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ran_notif_area_info.is_present();
    group_flags[1] |= ue_assist_info.size() > 0;
    group_flags[2] |= sel_band_combination_sn.is_present();
    group_flags[3] |= cfg_restrict_info_daps_r16.is_present();
    group_flags[3] |= sidelink_ue_info_nr_r16.size() > 0;
    group_flags[3] |= sidelink_ue_info_eutra_r16.size() > 0;
    group_flags[3] |= ue_assist_info_eutra_r16.size() > 0;
    group_flags[3] |= ue_assist_info_scg_r16.size() > 0;
    group_flags[3] |= need_for_gaps_info_nr_r16.is_present();
    group_flags[4] |= cfg_restrict_info_daps_v1640.is_present();
    group_flags[5] |= need_for_gap_ncsg_info_nr_r17.is_present();
    group_flags[5] |= need_for_gap_ncsg_info_eutra_r17.is_present();
    group_flags[5] |= mbs_interest_ind_r17.size() > 0;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ran_notif_area_info.is_present(), 1));
      if (ran_notif_area_info.is_present()) {
        HANDLE_CODE(ran_notif_area_info->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ue_assist_info.size() > 0, 1));
      if (ue_assist_info.size() > 0) {
        HANDLE_CODE(ue_assist_info.pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sel_band_combination_sn.is_present(), 1));
      if (sel_band_combination_sn.is_present()) {
        HANDLE_CODE(sel_band_combination_sn->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cfg_restrict_info_daps_r16.is_present(), 1));
      HANDLE_CODE(bref.pack(sidelink_ue_info_nr_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(sidelink_ue_info_eutra_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(ue_assist_info_eutra_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(ue_assist_info_scg_r16.size() > 0, 1));
      HANDLE_CODE(bref.pack(need_for_gaps_info_nr_r16.is_present(), 1));
      if (cfg_restrict_info_daps_r16.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_r16->pack(bref));
      }
      if (sidelink_ue_info_nr_r16.size() > 0) {
        HANDLE_CODE(sidelink_ue_info_nr_r16.pack(bref));
      }
      if (sidelink_ue_info_eutra_r16.size() > 0) {
        HANDLE_CODE(sidelink_ue_info_eutra_r16.pack(bref));
      }
      if (ue_assist_info_eutra_r16.size() > 0) {
        HANDLE_CODE(ue_assist_info_eutra_r16.pack(bref));
      }
      if (ue_assist_info_scg_r16.size() > 0) {
        HANDLE_CODE(ue_assist_info_scg_r16.pack(bref));
      }
      if (need_for_gaps_info_nr_r16.is_present()) {
        HANDLE_CODE(need_for_gaps_info_nr_r16->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(cfg_restrict_info_daps_v1640.is_present(), 1));
      if (cfg_restrict_info_daps_v1640.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_v1640->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_nr_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(need_for_gap_ncsg_info_eutra_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(mbs_interest_ind_r17.size() > 0, 1));
      if (need_for_gap_ncsg_info_nr_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_nr_r17->pack(bref));
      }
      if (need_for_gap_ncsg_info_eutra_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17->pack(bref));
      }
      if (mbs_interest_ind_r17.size() > 0) {
        HANDLE_CODE(mbs_interest_ind_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE as_context_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(reest_info_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_info_present, 1));

  if (reest_info_present) {
    HANDLE_CODE(reest_info.unpack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(6);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ran_notif_area_info, bref);
      if (ran_notif_area_info.is_present()) {
        HANDLE_CODE(ran_notif_area_info->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ue_assist_info_present;
      HANDLE_CODE(bref.unpack(ue_assist_info_present, 1));
      if (ue_assist_info_present) {
        HANDLE_CODE(ue_assist_info.unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(sel_band_combination_sn, bref);
      if (sel_band_combination_sn.is_present()) {
        HANDLE_CODE(sel_band_combination_sn->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(cfg_restrict_info_daps_r16, bref);
      bool sidelink_ue_info_nr_r16_present;
      HANDLE_CODE(bref.unpack(sidelink_ue_info_nr_r16_present, 1));
      bool sidelink_ue_info_eutra_r16_present;
      HANDLE_CODE(bref.unpack(sidelink_ue_info_eutra_r16_present, 1));
      bool ue_assist_info_eutra_r16_present;
      HANDLE_CODE(bref.unpack(ue_assist_info_eutra_r16_present, 1));
      bool ue_assist_info_scg_r16_present;
      HANDLE_CODE(bref.unpack(ue_assist_info_scg_r16_present, 1));
      unpack_presence_flag(need_for_gaps_info_nr_r16, bref);
      if (cfg_restrict_info_daps_r16.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_r16->unpack(bref));
      }
      if (sidelink_ue_info_nr_r16_present) {
        HANDLE_CODE(sidelink_ue_info_nr_r16.unpack(bref));
      }
      if (sidelink_ue_info_eutra_r16_present) {
        HANDLE_CODE(sidelink_ue_info_eutra_r16.unpack(bref));
      }
      if (ue_assist_info_eutra_r16_present) {
        HANDLE_CODE(ue_assist_info_eutra_r16.unpack(bref));
      }
      if (ue_assist_info_scg_r16_present) {
        HANDLE_CODE(ue_assist_info_scg_r16.unpack(bref));
      }
      if (need_for_gaps_info_nr_r16.is_present()) {
        HANDLE_CODE(need_for_gaps_info_nr_r16->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(cfg_restrict_info_daps_v1640, bref);
      if (cfg_restrict_info_daps_v1640.is_present()) {
        HANDLE_CODE(cfg_restrict_info_daps_v1640->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(need_for_gap_ncsg_info_nr_r17, bref);
      unpack_presence_flag(need_for_gap_ncsg_info_eutra_r17, bref);
      bool mbs_interest_ind_r17_present;
      HANDLE_CODE(bref.unpack(mbs_interest_ind_r17_present, 1));
      if (need_for_gap_ncsg_info_nr_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_nr_r17->unpack(bref));
      }
      if (need_for_gap_ncsg_info_eutra_r17.is_present()) {
        HANDLE_CODE(need_for_gap_ncsg_info_eutra_r17->unpack(bref));
      }
      if (mbs_interest_ind_r17_present) {
        HANDLE_CODE(mbs_interest_ind_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void as_context_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (reest_info_present) {
    j.write_fieldname("reestablishmentInfo");
    reest_info.to_json(j);
  }
  if (cfg_restrict_info_present) {
    j.write_fieldname("configRestrictInfo");
    cfg_restrict_info.to_json(j);
  }
  if (ext) {
    if (ran_notif_area_info.is_present()) {
      j.write_fieldname("ran-NotificationAreaInfo");
      ran_notif_area_info->to_json(j);
    }
    if (ue_assist_info.size() > 0) {
      j.write_str("ueAssistanceInformation", ue_assist_info.to_string());
    }
    if (sel_band_combination_sn.is_present()) {
      j.write_fieldname("selectedBandCombinationSN");
      sel_band_combination_sn->to_json(j);
    }
    if (cfg_restrict_info_daps_r16.is_present()) {
      j.write_fieldname("configRestrictInfoDAPS-r16");
      cfg_restrict_info_daps_r16->to_json(j);
    }
    if (sidelink_ue_info_nr_r16.size() > 0) {
      j.write_str("sidelinkUEInformationNR-r16", sidelink_ue_info_nr_r16.to_string());
    }
    if (sidelink_ue_info_eutra_r16.size() > 0) {
      j.write_str("sidelinkUEInformationEUTRA-r16", sidelink_ue_info_eutra_r16.to_string());
    }
    if (ue_assist_info_eutra_r16.size() > 0) {
      j.write_str("ueAssistanceInformationEUTRA-r16", ue_assist_info_eutra_r16.to_string());
    }
    if (ue_assist_info_scg_r16.size() > 0) {
      j.write_str("ueAssistanceInformationSCG-r16", ue_assist_info_scg_r16.to_string());
    }
    if (need_for_gaps_info_nr_r16.is_present()) {
      j.write_fieldname("needForGapsInfoNR-r16");
      need_for_gaps_info_nr_r16->to_json(j);
    }
    if (cfg_restrict_info_daps_v1640.is_present()) {
      j.write_fieldname("configRestrictInfoDAPS-v1640");
      cfg_restrict_info_daps_v1640->to_json(j);
    }
    if (need_for_gap_ncsg_info_nr_r17.is_present()) {
      j.write_fieldname("needForGapNCSG-InfoNR-r17");
      need_for_gap_ncsg_info_nr_r17->to_json(j);
    }
    if (need_for_gap_ncsg_info_eutra_r17.is_present()) {
      j.write_fieldname("needForGapNCSG-InfoEUTRA-r17");
      need_for_gap_ncsg_info_eutra_r17->to_json(j);
    }
    if (mbs_interest_ind_r17.size() > 0) {
      j.write_str("mbsInterestIndication-r17", mbs_interest_ind_r17.to_string());
    }
  }
  j.end_obj();
}

// VictimSystemType ::= SEQUENCE
SRSASN_CODE victim_sys_type_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(gps_present, 1));
  HANDLE_CODE(bref.pack(glonass_present, 1));
  HANDLE_CODE(bref.pack(bds_present, 1));
  HANDLE_CODE(bref.pack(galileo_present, 1));
  HANDLE_CODE(bref.pack(wlan_present, 1));
  HANDLE_CODE(bref.pack(bluetooth_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE victim_sys_type_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(gps_present, 1));
  HANDLE_CODE(bref.unpack(glonass_present, 1));
  HANDLE_CODE(bref.unpack(bds_present, 1));
  HANDLE_CODE(bref.unpack(galileo_present, 1));
  HANDLE_CODE(bref.unpack(wlan_present, 1));
  HANDLE_CODE(bref.unpack(bluetooth_present, 1));

  return SRSASN_SUCCESS;
}
void victim_sys_type_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (gps_present) {
    j.write_str("gps", "true");
  }
  if (glonass_present) {
    j.write_str("glonass", "true");
  }
  if (bds_present) {
    j.write_str("bds", "true");
  }
  if (galileo_present) {
    j.write_str("galileo", "true");
  }
  if (wlan_present) {
    j.write_str("wlan", "true");
  }
  if (bluetooth_present) {
    j.write_str("bluetooth", "true");
  }
  j.end_obj();
}

// AffectedCarrierFreqCombInfoMRDC ::= SEQUENCE
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc_present, 1));

  HANDLE_CODE(victim_sys_type.pack(bref));
  HANDLE_CODE(interference_direction_mrdc.pack(bref));
  if (affected_carrier_freq_comb_mrdc_present) {
    HANDLE_CODE(bref.pack(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0, 1));
    if (affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0) {
      HANDLE_CODE(pack_dyn_seq_of(bref,
                                  affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra,
                                  1,
                                  32,
                                  integer_packer<uint32_t>(0, 262143)));
    }
    HANDLE_CODE(pack_dyn_seq_of(bref,
                                affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr,
                                1,
                                32,
                                integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE affected_carrier_freq_comb_info_mrdc_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_mrdc_present, 1));

  HANDLE_CODE(victim_sys_type.unpack(bref));
  HANDLE_CODE(interference_direction_mrdc.unpack(bref));
  if (affected_carrier_freq_comb_mrdc_present) {
    bool affected_carrier_freq_comb_eutra_present;
    HANDLE_CODE(bref.unpack(affected_carrier_freq_comb_eutra_present, 1));
    if (affected_carrier_freq_comb_eutra_present) {
      HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra,
                                    bref,
                                    1,
                                    32,
                                    integer_packer<uint32_t>(0, 262143)));
    }
    HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr,
                                  bref,
                                  1,
                                  32,
                                  integer_packer<uint32_t>(0, 3279165)));
  }

  return SRSASN_SUCCESS;
}
void affected_carrier_freq_comb_info_mrdc_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("victimSystemType");
  victim_sys_type.to_json(j);
  j.write_str("interferenceDirectionMRDC", interference_direction_mrdc.to_string());
  if (affected_carrier_freq_comb_mrdc_present) {
    j.write_fieldname("affectedCarrierFreqCombMRDC");
    j.start_obj();
    if (affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra.size() > 0) {
      j.start_array("affectedCarrierFreqCombEUTRA");
      for (const auto& e1 : affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_eutra) {
        j.write_int(e1);
      }
      j.end_array();
    }
    j.start_array("affectedCarrierFreqCombNR");
    for (const auto& e1 : affected_carrier_freq_comb_mrdc.affected_carrier_freq_comb_nr) {
      j.write_int(e1);
    }
    j.end_array();
    j.end_obj();
  }
  j.end_obj();
}

const char* affected_carrier_freq_comb_info_mrdc_s::interference_direction_mrdc_opts::to_string() const
{
  static const char* names[] = {"eutra-nr", "nr", "other", "utra-nr-other", "nr-other", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "affected_carrier_freq_comb_info_mrdc_s::interference_direction_mrdc_e_");
}

// CG-CandidateInfoId-r17 ::= SEQUENCE
SRSASN_CODE cg_candidate_info_id_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ssb_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_info_id_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ssb_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));

  return SRSASN_SUCCESS;
}
void cg_candidate_info_id_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssbFrequency-r17", ssb_freq_r17);
  j.write_int("physCellId-r17", pci_r17);
  j.end_obj();
}

// CG-CandidateInfo-r17 ::= SEQUENCE
SRSASN_CODE cg_candidate_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cg_candidate_info_id_r17.pack(bref));
  HANDLE_CODE(candidate_cg_cfg_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cg_candidate_info_id_r17.unpack(bref));
  HANDLE_CODE(candidate_cg_cfg_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_candidate_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cg-CandidateInfoId-r17");
  cg_candidate_info_id_r17.to_json(j);
  j.write_str("candidateCG-Config-r17", candidate_cg_cfg_r17.to_string());
  j.end_obj();
}

// CG-CandidateList-r17-IEs ::= SEQUENCE
SRSASN_CODE cg_candidate_list_r17_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cg_candidate_to_add_mod_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(cg_candidate_to_release_list_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (cg_candidate_to_add_mod_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cg_candidate_to_add_mod_list_r17, 1, 8));
  }
  if (cg_candidate_to_release_list_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cg_candidate_to_release_list_r17, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_r17_ies_s::unpack(cbit_ref& bref)
{
  bool cg_candidate_to_add_mod_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_candidate_to_add_mod_list_r17_present, 1));
  bool cg_candidate_to_release_list_r17_present;
  HANDLE_CODE(bref.unpack(cg_candidate_to_release_list_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (cg_candidate_to_add_mod_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cg_candidate_to_add_mod_list_r17, bref, 1, 8));
  }
  if (cg_candidate_to_release_list_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cg_candidate_to_release_list_r17, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void cg_candidate_list_r17_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (cg_candidate_to_add_mod_list_r17.size() > 0) {
    j.start_array("cg-CandidateToAddModList-r17");
    for (const auto& e1 : cg_candidate_to_add_mod_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cg_candidate_to_release_list_r17.size() > 0) {
    j.start_array("cg-CandidateToReleaseList-r17");
    for (const auto& e1 : cg_candidate_to_release_list_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CG-CandidateList ::= SEQUENCE
SRSASN_CODE cg_candidate_list_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_candidate_list_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void cg_candidate_list_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_candidate_list_s::crit_exts_c_::c1_c_& cg_candidate_list_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_candidate_list_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void cg_candidate_list_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cg_candidate_list_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_candidate_list_r17_ies_s& cg_candidate_list_s::crit_exts_c_::c1_c_::set_cg_candidate_list_r17()
{
  set(types::cg_candidate_list_r17);
  return c;
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cg_candidate_list_r17:
      j.write_fieldname("cg-CandidateList-r17");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cg_candidate_list_r17:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_candidate_list_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cg_candidate_list_r17:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_candidate_list_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cg_candidate_list_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"cg-CandidateList-r17", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "cg_candidate_list_s::crit_exts_c_::c1_c_::types");
}

const char* cg_candidate_list_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "cg_candidate_list_s::crit_exts_c_::types");
}
uint8_t cg_candidate_list_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_candidate_list_s::crit_exts_c_::types");
}

// CandidateCell-r17 ::= SEQUENCE
SRSASN_CODE candidate_cell_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(cond_execution_cond_scg_r17.size() > 0, 1));

  HANDLE_CODE(pack_integer(bref, pci_r17, (uint16_t)0u, (uint16_t)1007u));
  if (cond_execution_cond_scg_r17.size() > 0) {
    HANDLE_CODE(cond_execution_cond_scg_r17.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_cell_r17_s::unpack(cbit_ref& bref)
{
  bool cond_execution_cond_scg_r17_present;
  HANDLE_CODE(bref.unpack(cond_execution_cond_scg_r17_present, 1));

  HANDLE_CODE(unpack_integer(pci_r17, bref, (uint16_t)0u, (uint16_t)1007u));
  if (cond_execution_cond_scg_r17_present) {
    HANDLE_CODE(cond_execution_cond_scg_r17.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void candidate_cell_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r17", pci_r17);
  if (cond_execution_cond_scg_r17.size() > 0) {
    j.write_str("condExecutionCondSCG-r17", cond_execution_cond_scg_r17.to_string());
  }
  j.end_obj();
}

// CandidateCellInfo-r17 ::= SEQUENCE
SRSASN_CODE candidate_cell_info_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ssb_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, candidate_list_r17, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_cell_info_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ssb_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(candidate_list_r17, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void candidate_cell_info_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssbFrequency-r17", ssb_freq_r17);
  j.start_array("candidateList-r17");
  for (const auto& e1 : candidate_list_r17) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// FrequencyConfig-NR-r16 ::= SEQUENCE
SRSASN_CODE freq_cfg_nr_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, freq_band_ind_nr_r16, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(pack_integer(bref, carrier_center_freq_nr_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_integer(bref, carrier_bw_nr_r16, (uint16_t)1u, (uint16_t)275u));
  HANDLE_CODE(subcarrier_spacing_nr_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_cfg_nr_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(freq_band_ind_nr_r16, bref, (uint16_t)1u, (uint16_t)1024u));
  HANDLE_CODE(unpack_integer(carrier_center_freq_nr_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_integer(carrier_bw_nr_r16, bref, (uint16_t)1u, (uint16_t)275u));
  HANDLE_CODE(subcarrier_spacing_nr_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void freq_cfg_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("freqBandIndicatorNR-r16", freq_band_ind_nr_r16);
  j.write_int("carrierCenterFreq-NR-r16", carrier_center_freq_nr_r16);
  j.write_int("carrierBandwidth-NR-r16", carrier_bw_nr_r16);
  j.write_str("subcarrierSpacing-NR-r16", subcarrier_spacing_nr_r16.to_string());
  j.end_obj();
}

// TransmissionBandwidth-EUTRA-r16 ::= ENUMERATED
const char* tx_bw_eutra_r16_opts::to_string() const
{
  static const char* names[] = {"rb6", "rb15", "rb25", "rb50", "rb75", "rb100"};
  return convert_enum_idx(names, 6, value, "tx_bw_eutra_r16_e");
}
uint8_t tx_bw_eutra_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "tx_bw_eutra_r16_e");
}

// CG-Config-v1730-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1730_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fr1_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.pack(fr2_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fr1_carriers_scg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr1_carriers_scg_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_scg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr2_carriers_scg_r17, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1730_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fr1_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.unpack(fr2_carriers_scg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fr1_carriers_scg_r17_present) {
    HANDLE_CODE(unpack_integer(fr1_carriers_scg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_scg_r17_present) {
    HANDLE_CODE(unpack_integer(fr2_carriers_scg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1730_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fr1_carriers_scg_r17_present) {
    j.write_int("fr1-Carriers-SCG-r17", fr1_carriers_scg_r17);
  }
  if (fr2_carriers_scg_r17_present) {
    j.write_int("fr2-Carriers-SCG-r17", fr2_carriers_scg_r17);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// ServCellInfoXCG-EUTRA-r16 ::= SEQUENCE
SRSASN_CODE serv_cell_info_xcg_eutra_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(ul_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.pack(tx_bw_eutra_r16_present, 1));

  if (dl_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(pack_integer(bref, dl_carrier_freq_eutra_r16, (uint32_t)0u, (uint32_t)262143u));
  }
  if (ul_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(pack_integer(bref, ul_carrier_freq_eutra_r16, (uint32_t)0u, (uint32_t)262143u));
  }
  if (tx_bw_eutra_r16_present) {
    HANDLE_CODE(tx_bw_eutra_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE serv_cell_info_xcg_eutra_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(ul_carrier_freq_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(tx_bw_eutra_r16_present, 1));

  if (dl_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(unpack_integer(dl_carrier_freq_eutra_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (ul_carrier_freq_eutra_r16_present) {
    HANDLE_CODE(unpack_integer(ul_carrier_freq_eutra_r16, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (tx_bw_eutra_r16_present) {
    HANDLE_CODE(tx_bw_eutra_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void serv_cell_info_xcg_eutra_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_carrier_freq_eutra_r16_present) {
    j.write_int("dl-CarrierFreq-EUTRA-r16", dl_carrier_freq_eutra_r16);
  }
  if (ul_carrier_freq_eutra_r16_present) {
    j.write_int("ul-CarrierFreq-EUTRA-r16", ul_carrier_freq_eutra_r16);
  }
  if (tx_bw_eutra_r16_present) {
    j.write_str("transmissionBandwidth-EUTRA-r16", tx_bw_eutra_r16.to_string());
  }
  j.end_obj();
}

// ServCellInfoXCG-NR-r16 ::= SEQUENCE
SRSASN_CODE serv_cell_info_xcg_nr_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(dl_freq_info_nr_r16_present, 1));
  HANDLE_CODE(bref.pack(ul_freq_info_nr_r16_present, 1));

  if (dl_freq_info_nr_r16_present) {
    HANDLE_CODE(dl_freq_info_nr_r16.pack(bref));
  }
  if (ul_freq_info_nr_r16_present) {
    HANDLE_CODE(ul_freq_info_nr_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE serv_cell_info_xcg_nr_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(dl_freq_info_nr_r16_present, 1));
  HANDLE_CODE(bref.unpack(ul_freq_info_nr_r16_present, 1));

  if (dl_freq_info_nr_r16_present) {
    HANDLE_CODE(dl_freq_info_nr_r16.unpack(bref));
  }
  if (ul_freq_info_nr_r16_present) {
    HANDLE_CODE(ul_freq_info_nr_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void serv_cell_info_xcg_nr_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_freq_info_nr_r16_present) {
    j.write_fieldname("dl-FreqInfo-NR-r16");
    dl_freq_info_nr_r16.to_json(j);
  }
  if (ul_freq_info_nr_r16_present) {
    j.write_fieldname("ul-FreqInfo-NR-r16");
    ul_freq_info_nr_r16.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1700-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(candidate_cell_info_list_cp_c_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(two_phr_mode_scg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_cp_c_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_info_list_cp_c_r17, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool candidate_cell_info_list_cp_c_r17_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_cp_c_r17_present, 1));
  HANDLE_CODE(bref.unpack(two_phr_mode_scg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_cp_c_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_info_list_cp_c_r17, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_cell_info_list_cp_c_r17.size() > 0) {
    j.start_array("candidateCellInfoListCPC-r17");
    for (const auto& e1 : candidate_cell_info_list_cp_c_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (two_phr_mode_scg_r17_present) {
    j.write_str("twoPHRModeSCG-r17", "enabled");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1640-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1640_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_cell_info_list_scg_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(serv_cell_info_list_scg_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (serv_cell_info_list_scg_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_scg_nr_r16, 1, 32));
  }
  if (serv_cell_info_list_scg_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_scg_eutra_r16, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1640_ies_s::unpack(cbit_ref& bref)
{
  bool serv_cell_info_list_scg_nr_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_scg_nr_r16_present, 1));
  bool serv_cell_info_list_scg_eutra_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_scg_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (serv_cell_info_list_scg_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_scg_nr_r16, bref, 1, 32));
  }
  if (serv_cell_info_list_scg_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_scg_eutra_r16, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1640_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_cell_info_list_scg_nr_r16.size() > 0) {
    j.start_array("servCellInfoListSCG-NR-r16");
    for (const auto& e1 : serv_cell_info_list_scg_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (serv_cell_info_list_scg_eutra_r16.size() > 0) {
    j.start_array("servCellInfoListSCG-EUTRA-r16");
    for (const auto& e1 : serv_cell_info_list_scg_eutra_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1630-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1630_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sel_toffset_r16_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sel_toffset_r16_present) {
    HANDLE_CODE(sel_toffset_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1630_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sel_toffset_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sel_toffset_r16_present) {
    HANDLE_CODE(sel_toffset_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1630_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sel_toffset_r16_present) {
    j.write_str("selectedToffset-r16", sel_toffset_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config-v1620-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1620_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_assist_info_scg_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_assist_info_scg_r16.size() > 0) {
    HANDLE_CODE(ue_assist_info_scg_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1620_ies_s::unpack(cbit_ref& bref)
{
  bool ue_assist_info_scg_r16_present;
  HANDLE_CODE(bref.unpack(ue_assist_info_scg_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_assist_info_scg_r16_present) {
    HANDLE_CODE(ue_assist_info_scg_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1620_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_assist_info_scg_r16.size() > 0) {
    j.write_str("ueAssistanceInformationSCG-r16", ue_assist_info_scg_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// DRX-Info2 ::= SEQUENCE
SRSASN_CODE drx_info2_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(drx_on_dur_timer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info2_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(drx_on_dur_timer.unpack(bref));

  return SRSASN_SUCCESS;
}
void drx_info2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-onDurationTimer");
  drx_on_dur_timer.to_json(j);
  j.end_obj();
}

void drx_info2_s::drx_on_dur_timer_c_::destroy_() {}
void drx_info2_s::drx_on_dur_timer_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_info2_s::drx_on_dur_timer_c_::drx_on_dur_timer_c_(const drx_info2_s::drx_on_dur_timer_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sub_milli_seconds:
      c.init(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.init(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
  }
}
drx_info2_s::drx_on_dur_timer_c_&
drx_info2_s::drx_on_dur_timer_c_::operator=(const drx_info2_s::drx_on_dur_timer_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sub_milli_seconds:
      c.set(other.c.get<uint8_t>());
      break;
    case types::milli_seconds:
      c.set(other.c.get<milli_seconds_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
  }

  return *this;
}
uint8_t& drx_info2_s::drx_on_dur_timer_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
drx_info2_s::drx_on_dur_timer_c_::milli_seconds_e_& drx_info2_s::drx_on_dur_timer_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
void drx_info2_s::drx_on_dur_timer_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sub_milli_seconds:
      j.write_int("subMilliSeconds", c.get<uint8_t>());
      break;
    case types::milli_seconds:
      j.write_str("milliSeconds", c.get<milli_seconds_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_info2_s::drx_on_dur_timer_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info2_s::drx_on_dur_timer_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sub_milli_seconds:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)31u));
      break;
    case types::milli_seconds:
      HANDLE_CODE(c.get<milli_seconds_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info2_s::drx_on_dur_timer_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_info2_s::drx_on_dur_timer_c_::milli_seconds_opts::to_string() const
{
  static const char* names[] = {"ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",
                                "ms20",   "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",
                                "ms300",  "ms400",  "ms500",  "ms600",  "ms800",  "ms1000", "ms1200", "ms1600",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_info2_s::drx_on_dur_timer_c_::milli_seconds_e_");
}
uint16_t drx_info2_s::drx_on_dur_timer_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 24, value, "drx_info2_s::drx_on_dur_timer_c_::milli_seconds_e_");
}

const char* drx_info2_s::drx_on_dur_timer_c_::types_opts::to_string() const
{
  static const char* names[] = {"subMilliSeconds", "milliSeconds"};
  return convert_enum_idx(names, 2, value, "drx_info2_s::drx_on_dur_timer_c_::types");
}

// CG-Config-v1610-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_info_scg2_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (drx_info_scg2_present) {
    HANDLE_CODE(drx_info_scg2.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_info_scg2_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (drx_info_scg2_present) {
    HANDLE_CODE(drx_info_scg2.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drx_info_scg2_present) {
    j.write_fieldname("drx-InfoSCG2");
    drx_info_scg2.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-UplinkCarrierSCG ::= SEQUENCE
SRSASN_CODE ph_ul_carrier_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ph_type1or3.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_ul_carrier_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ph_type1or3.unpack(bref));

  return SRSASN_SUCCESS;
}
void ph_ul_carrier_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ph-Type1or3", ph_type1or3.to_string());
  j.end_obj();
}

const char* ph_ul_carrier_scg_s::ph_type1or3_opts::to_string() const
{
  static const char* names[] = {"type1", "type3"};
  return convert_enum_idx(names, 2, value, "ph_ul_carrier_scg_s::ph_type1or3_e_");
}
uint8_t ph_ul_carrier_scg_s::ph_type1or3_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 3};
  return map_enum_number(numbers, 2, value, "ph_ul_carrier_scg_s::ph_type1or3_e_");
}

// CG-Config-v1590-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scell_frequencies_sn_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(scell_frequencies_sn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scell_frequencies_sn_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_frequencies_sn_nr, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (scell_frequencies_sn_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_frequencies_sn_eutra, 1, 31, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool scell_frequencies_sn_nr_present;
  HANDLE_CODE(bref.unpack(scell_frequencies_sn_nr_present, 1));
  bool scell_frequencies_sn_eutra_present;
  HANDLE_CODE(bref.unpack(scell_frequencies_sn_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scell_frequencies_sn_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_frequencies_sn_nr, bref, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (scell_frequencies_sn_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_frequencies_sn_eutra, bref, 1, 31, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scell_frequencies_sn_nr.size() > 0) {
    j.start_array("scellFrequenciesSN-NR");
    for (const auto& e1 : scell_frequencies_sn_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (scell_frequencies_sn_eutra.size() > 0) {
    j.start_array("scellFrequenciesSN-EUTRA");
    for (const auto& e1 : scell_frequencies_sn_eutra) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-InfoSCG ::= SEQUENCE
SRSASN_CODE ph_info_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.pack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= two_srs_pusch_repeat_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_info_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.unpack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void ph_info_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_fieldname("ph-Uplink");
  ph_ul.to_json(j);
  if (ph_supplementary_ul_present) {
    j.write_fieldname("ph-SupplementaryUplink");
    ph_supplementary_ul.to_json(j);
  }
  if (ext) {
    if (two_srs_pusch_repeat_r17_present) {
      j.write_str("twoSRS-PUSCH-Repetition-r17", "enabled");
    }
  }
  j.end_obj();
}

// CG-Config-v1560-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pscell_freq_eutra_present, 1));
  HANDLE_CODE(bref.pack(scg_cell_group_cfg_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_serving_freq_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(need_for_gaps_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_scg_present, 1));
  HANDLE_CODE(bref.pack(report_cgi_request_eutra_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pscell_freq_eutra_present) {
    HANDLE_CODE(pack_integer(bref, pscell_freq_eutra, (uint32_t)0u, (uint32_t)262143u));
  }
  if (scg_cell_group_cfg_eutra.size() > 0) {
    HANDLE_CODE(scg_cell_group_cfg_eutra.pack(bref));
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.pack(bref));
  }
  if (candidate_serving_freq_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_serving_freq_list_eutra, 1, 32, integer_packer<uint32_t>(0, 262143)));
  }
  if (drx_cfg_scg_present) {
    HANDLE_CODE(drx_cfg_scg.pack(bref));
  }
  if (report_cgi_request_eutra_present) {
    HANDLE_CODE(bref.pack(report_cgi_request_eutra.requested_cell_info_eutra_present, 1));
    if (report_cgi_request_eutra.requested_cell_info_eutra_present) {
      HANDLE_CODE(pack_integer(
          bref, report_cgi_request_eutra.requested_cell_info_eutra.eutra_freq, (uint32_t)0u, (uint32_t)262143u));
      HANDLE_CODE(pack_integer(bref,
                               report_cgi_request_eutra.requested_cell_info_eutra.cell_for_which_to_report_cgi_eutra,
                               (uint16_t)0u,
                               (uint16_t)503u));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1560_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pscell_freq_eutra_present, 1));
  bool scg_cell_group_cfg_eutra_present;
  HANDLE_CODE(bref.unpack(scg_cell_group_cfg_eutra_present, 1));
  bool candidate_cell_info_list_sn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_eutra_present, 1));
  bool candidate_serving_freq_list_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_serving_freq_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(need_for_gaps_present, 1));
  HANDLE_CODE(bref.unpack(drx_cfg_scg_present, 1));
  HANDLE_CODE(bref.unpack(report_cgi_request_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pscell_freq_eutra_present) {
    HANDLE_CODE(unpack_integer(pscell_freq_eutra, bref, (uint32_t)0u, (uint32_t)262143u));
  }
  if (scg_cell_group_cfg_eutra_present) {
    HANDLE_CODE(scg_cell_group_cfg_eutra.unpack(bref));
  }
  if (candidate_cell_info_list_sn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.unpack(bref));
  }
  if (candidate_serving_freq_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_serving_freq_list_eutra, bref, 1, 32, integer_packer<uint32_t>(0, 262143)));
  }
  if (drx_cfg_scg_present) {
    HANDLE_CODE(drx_cfg_scg.unpack(bref));
  }
  if (report_cgi_request_eutra_present) {
    HANDLE_CODE(bref.unpack(report_cgi_request_eutra.requested_cell_info_eutra_present, 1));
    if (report_cgi_request_eutra.requested_cell_info_eutra_present) {
      HANDLE_CODE(unpack_integer(
          report_cgi_request_eutra.requested_cell_info_eutra.eutra_freq, bref, (uint32_t)0u, (uint32_t)262143u));
      HANDLE_CODE(unpack_integer(report_cgi_request_eutra.requested_cell_info_eutra.cell_for_which_to_report_cgi_eutra,
                                 bref,
                                 (uint16_t)0u,
                                 (uint16_t)503u));
    }
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pscell_freq_eutra_present) {
    j.write_int("pSCellFrequencyEUTRA", pscell_freq_eutra);
  }
  if (scg_cell_group_cfg_eutra.size() > 0) {
    j.write_str("scg-CellGroupConfigEUTRA", scg_cell_group_cfg_eutra.to_string());
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListSN-EUTRA", candidate_cell_info_list_sn_eutra.to_string());
  }
  if (candidate_serving_freq_list_eutra.size() > 0) {
    j.start_array("candidateServingFreqListEUTRA");
    for (const auto& e1 : candidate_serving_freq_list_eutra) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (need_for_gaps_present) {
    j.write_str("needForGaps", "true");
  }
  if (drx_cfg_scg_present) {
    j.write_fieldname("drx-ConfigSCG");
    drx_cfg_scg.to_json(j);
  }
  if (report_cgi_request_eutra_present) {
    j.write_fieldname("reportCGI-RequestEUTRA");
    j.start_obj();
    if (report_cgi_request_eutra.requested_cell_info_eutra_present) {
      j.write_fieldname("requestedCellInfoEUTRA");
      j.start_obj();
      j.write_int("eutraFrequency", report_cgi_request_eutra.requested_cell_info_eutra.eutra_freq);
      j.write_int("cellForWhichToReportCGI-EUTRA",
                  report_cgi_request_eutra.requested_cell_info_eutra.cell_for_which_to_report_cgi_eutra);
      j.end_obj();
    }
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// FR-Info ::= SEQUENCE
SRSASN_CODE fr_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(fr_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE fr_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(fr_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void fr_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_str("fr-Type", fr_type.to_string());
  j.end_obj();
}

const char* fr_info_s::fr_type_opts::to_string() const
{
  static const char* names[] = {"fr1", "fr2"};
  return convert_enum_idx(names, 2, value, "fr_info_s::fr_type_e_");
}
uint8_t fr_info_s::fr_type_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "fr_info_s::fr_type_e_");
}

// NR-FreqInfo ::= SEQUENCE
SRSASN_CODE nr_freq_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_freq_present, 1));

  if (measured_freq_present) {
    HANDLE_CODE(pack_integer(bref, measured_freq, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_freq_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(measured_freq_present, 1));

  if (measured_freq_present) {
    HANDLE_CODE(unpack_integer(measured_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }

  return SRSASN_SUCCESS;
}
void nr_freq_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_freq_present) {
    j.write_int("measuredFrequency", measured_freq);
  }
  j.end_obj();
}

// CG-Config-v1540-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pscell_freq_present, 1));
  HANDLE_CODE(bref.pack(report_cgi_request_nr_present, 1));
  HANDLE_CODE(bref.pack(ph_info_scg.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pscell_freq_present) {
    HANDLE_CODE(pack_integer(bref, pscell_freq, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (report_cgi_request_nr_present) {
    HANDLE_CODE(bref.pack(report_cgi_request_nr.requested_cell_info_present, 1));
    if (report_cgi_request_nr.requested_cell_info_present) {
      HANDLE_CODE(
          pack_integer(bref, report_cgi_request_nr.requested_cell_info.ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
      HANDLE_CODE(pack_integer(
          bref, report_cgi_request_nr.requested_cell_info.cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)1007u));
    }
  }
  if (ph_info_scg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ph_info_scg, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_v1540_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pscell_freq_present, 1));
  HANDLE_CODE(bref.unpack(report_cgi_request_nr_present, 1));
  bool ph_info_scg_present;
  HANDLE_CODE(bref.unpack(ph_info_scg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pscell_freq_present) {
    HANDLE_CODE(unpack_integer(pscell_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
  }
  if (report_cgi_request_nr_present) {
    HANDLE_CODE(bref.unpack(report_cgi_request_nr.requested_cell_info_present, 1));
    if (report_cgi_request_nr.requested_cell_info_present) {
      HANDLE_CODE(
          unpack_integer(report_cgi_request_nr.requested_cell_info.ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
      HANDLE_CODE(unpack_integer(
          report_cgi_request_nr.requested_cell_info.cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)1007u));
    }
  }
  if (ph_info_scg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ph_info_scg, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pscell_freq_present) {
    j.write_int("pSCellFrequency", pscell_freq);
  }
  if (report_cgi_request_nr_present) {
    j.write_fieldname("reportCGI-RequestNR");
    j.start_obj();
    if (report_cgi_request_nr.requested_cell_info_present) {
      j.write_fieldname("requestedCellInfo");
      j.start_obj();
      j.write_int("ssbFrequency", report_cgi_request_nr.requested_cell_info.ssb_freq);
      j.write_int("cellForWhichToReportCGI", report_cgi_request_nr.requested_cell_info.cell_for_which_to_report_cgi);
      j.end_obj();
    }
    j.end_obj();
  }
  if (ph_info_scg.size() > 0) {
    j.start_array("ph-InfoSCG");
    for (const auto& e1 : ph_info_scg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// ConfigRestrictModReqSCG ::= SEQUENCE
SRSASN_CODE cfg_restrict_mod_req_scg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(requested_bc_mrdc_present, 1));
  HANDLE_CODE(bref.pack(requested_p_max_fr1_present, 1));

  if (requested_bc_mrdc_present) {
    HANDLE_CODE(requested_bc_mrdc.pack(bref));
  }
  if (requested_p_max_fr1_present) {
    HANDLE_CODE(pack_integer(bref, requested_p_max_fr1, (int8_t)-30, (int8_t)33));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= requested_pdcch_blind_detection_scg_present;
    group_flags[0] |= requested_p_max_eutra_present;
    group_flags[1] |= requested_p_max_fr2_r16_present;
    group_flags[1] |= requested_max_inter_freq_meas_id_scg_r16_present;
    group_flags[1] |= requested_max_intra_freq_meas_id_scg_r16_present;
    group_flags[1] |= requested_toffset_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(requested_pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.pack(requested_p_max_eutra_present, 1));
      if (requested_pdcch_blind_detection_scg_present) {
        HANDLE_CODE(pack_integer(bref, requested_pdcch_blind_detection_scg, (uint8_t)1u, (uint8_t)15u));
      }
      if (requested_p_max_eutra_present) {
        HANDLE_CODE(pack_integer(bref, requested_p_max_eutra, (int8_t)-30, (int8_t)33));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(requested_p_max_fr2_r16_present, 1));
      HANDLE_CODE(bref.pack(requested_max_inter_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(requested_max_intra_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.pack(requested_toffset_r16_present, 1));
      if (requested_p_max_fr2_r16_present) {
        HANDLE_CODE(pack_integer(bref, requested_p_max_fr2_r16, (int8_t)-30, (int8_t)33));
      }
      if (requested_max_inter_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, requested_max_inter_freq_meas_id_scg_r16, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_max_intra_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(pack_integer(bref, requested_max_intra_freq_meas_id_scg_r16, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_toffset_r16_present) {
        HANDLE_CODE(requested_toffset_r16.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cfg_restrict_mod_req_scg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(requested_bc_mrdc_present, 1));
  HANDLE_CODE(bref.unpack(requested_p_max_fr1_present, 1));

  if (requested_bc_mrdc_present) {
    HANDLE_CODE(requested_bc_mrdc.unpack(bref));
  }
  if (requested_p_max_fr1_present) {
    HANDLE_CODE(unpack_integer(requested_p_max_fr1, bref, (int8_t)-30, (int8_t)33));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(requested_pdcch_blind_detection_scg_present, 1));
      HANDLE_CODE(bref.unpack(requested_p_max_eutra_present, 1));
      if (requested_pdcch_blind_detection_scg_present) {
        HANDLE_CODE(unpack_integer(requested_pdcch_blind_detection_scg, bref, (uint8_t)1u, (uint8_t)15u));
      }
      if (requested_p_max_eutra_present) {
        HANDLE_CODE(unpack_integer(requested_p_max_eutra, bref, (int8_t)-30, (int8_t)33));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(requested_p_max_fr2_r16_present, 1));
      HANDLE_CODE(bref.unpack(requested_max_inter_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(requested_max_intra_freq_meas_id_scg_r16_present, 1));
      HANDLE_CODE(bref.unpack(requested_toffset_r16_present, 1));
      if (requested_p_max_fr2_r16_present) {
        HANDLE_CODE(unpack_integer(requested_p_max_fr2_r16, bref, (int8_t)-30, (int8_t)33));
      }
      if (requested_max_inter_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(unpack_integer(requested_max_inter_freq_meas_id_scg_r16, bref, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_max_intra_freq_meas_id_scg_r16_present) {
        HANDLE_CODE(unpack_integer(requested_max_intra_freq_meas_id_scg_r16, bref, (uint8_t)1u, (uint8_t)62u));
      }
      if (requested_toffset_r16_present) {
        HANDLE_CODE(requested_toffset_r16.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void cfg_restrict_mod_req_scg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (requested_bc_mrdc_present) {
    j.write_fieldname("requestedBC-MRDC");
    requested_bc_mrdc.to_json(j);
  }
  if (requested_p_max_fr1_present) {
    j.write_int("requestedP-MaxFR1", requested_p_max_fr1);
  }
  if (ext) {
    if (requested_pdcch_blind_detection_scg_present) {
      j.write_int("requestedPDCCH-BlindDetectionSCG", requested_pdcch_blind_detection_scg);
    }
    if (requested_p_max_eutra_present) {
      j.write_int("requestedP-MaxEUTRA", requested_p_max_eutra);
    }
    if (requested_p_max_fr2_r16_present) {
      j.write_int("requestedP-MaxFR2-r16", requested_p_max_fr2_r16);
    }
    if (requested_max_inter_freq_meas_id_scg_r16_present) {
      j.write_int("requestedMaxInterFreqMeasIdSCG-r16", requested_max_inter_freq_meas_id_scg_r16);
    }
    if (requested_max_intra_freq_meas_id_scg_r16_present) {
      j.write_int("requestedMaxIntraFreqMeasIdSCG-r16", requested_max_intra_freq_meas_id_scg_r16);
    }
    if (requested_toffset_r16_present) {
      j.write_str("requestedToffset-r16", requested_toffset_r16.to_string());
    }
  }
  j.end_obj();
}

// DRX-Info ::= SEQUENCE
SRSASN_CODE drx_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(short_drx_present, 1));

  HANDLE_CODE(drx_long_cycle_start_offset.pack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.pack(bref));
    HANDLE_CODE(pack_integer(bref, short_drx.drx_short_cycle_timer, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(short_drx_present, 1));

  HANDLE_CODE(drx_long_cycle_start_offset.unpack(bref));
  if (short_drx_present) {
    HANDLE_CODE(short_drx.drx_short_cycle.unpack(bref));
    HANDLE_CODE(unpack_integer(short_drx.drx_short_cycle_timer, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void drx_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("drx-LongCycleStartOffset");
  drx_long_cycle_start_offset.to_json(j);
  if (short_drx_present) {
    j.write_fieldname("shortDRX");
    j.start_obj();
    j.write_str("drx-ShortCycle", short_drx.drx_short_cycle.to_string());
    j.write_int("drx-ShortCycleTimer", short_drx.drx_short_cycle_timer);
    j.end_obj();
  }
  j.end_obj();
}

void drx_info_s::drx_long_cycle_start_offset_c_::destroy_() {}
void drx_info_s::drx_long_cycle_start_offset_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
drx_info_s::drx_long_cycle_start_offset_c_::drx_long_cycle_start_offset_c_(
    const drx_info_s::drx_long_cycle_start_offset_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ms10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }
}
drx_info_s::drx_long_cycle_start_offset_c_&
drx_info_s::drx_long_cycle_start_offset_c_::operator=(const drx_info_s::drx_long_cycle_start_offset_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ms10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms60:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms70:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::ms256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms1280:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms2560:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms5120:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ms10240:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }

  return *this;
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& drx_info_s::drx_long_cycle_start_offset_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
void drx_info_s::drx_long_cycle_start_offset_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ms10:
      j.write_int("ms10", c.get<uint8_t>());
      break;
    case types::ms20:
      j.write_int("ms20", c.get<uint8_t>());
      break;
    case types::ms32:
      j.write_int("ms32", c.get<uint8_t>());
      break;
    case types::ms40:
      j.write_int("ms40", c.get<uint8_t>());
      break;
    case types::ms60:
      j.write_int("ms60", c.get<uint8_t>());
      break;
    case types::ms64:
      j.write_int("ms64", c.get<uint8_t>());
      break;
    case types::ms70:
      j.write_int("ms70", c.get<uint8_t>());
      break;
    case types::ms80:
      j.write_int("ms80", c.get<uint8_t>());
      break;
    case types::ms128:
      j.write_int("ms128", c.get<uint8_t>());
      break;
    case types::ms160:
      j.write_int("ms160", c.get<uint8_t>());
      break;
    case types::ms256:
      j.write_int("ms256", c.get<uint16_t>());
      break;
    case types::ms320:
      j.write_int("ms320", c.get<uint16_t>());
      break;
    case types::ms512:
      j.write_int("ms512", c.get<uint16_t>());
      break;
    case types::ms640:
      j.write_int("ms640", c.get<uint16_t>());
      break;
    case types::ms1024:
      j.write_int("ms1024", c.get<uint16_t>());
      break;
    case types::ms1280:
      j.write_int("ms1280", c.get<uint16_t>());
      break;
    case types::ms2048:
      j.write_int("ms2048", c.get<uint16_t>());
      break;
    case types::ms2560:
      j.write_int("ms2560", c.get<uint16_t>());
      break;
    case types::ms5120:
      j.write_int("ms5120", c.get<uint16_t>());
      break;
    case types::ms10240:
      j.write_int("ms10240", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
  }
  j.end_obj();
}
SRSASN_CODE drx_info_s::drx_long_cycle_start_offset_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE drx_info_s::drx_long_cycle_start_offset_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ms10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::ms20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::ms32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::ms40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::ms60:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)59u));
      break;
    case types::ms64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::ms70:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)69u));
      break;
    case types::ms80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::ms128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::ms160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::ms256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::ms320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::ms512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::ms640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::ms1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::ms1280:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1279u));
      break;
    case types::ms2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::ms2560:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2559u));
      break;
    case types::ms5120:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::ms10240:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    default:
      log_invalid_choice_id(type_, "drx_info_s::drx_long_cycle_start_offset_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* drx_info_s::drx_long_cycle_start_offset_c_::types_opts::to_string() const
{
  static const char* names[] = {"ms10",   "ms20",   "ms32",   "ms40",   "ms60",   "ms64",   "ms70",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",
                                "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
  return convert_enum_idx(names, 20, value, "drx_info_s::drx_long_cycle_start_offset_c_::types");
}
uint16_t drx_info_s::drx_long_cycle_start_offset_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "drx_info_s::drx_long_cycle_start_offset_c_::types");
}

const char* drx_info_s::short_drx_s_::drx_short_cycle_opts::to_string() const
{
  static const char* names[] = {"ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms7",    "ms8",    "ms10",
                                "ms14",   "ms16",   "ms20",   "ms30",   "ms32",   "ms35",   "ms40",   "ms64",
                                "ms80",   "ms128",  "ms160",  "ms256",  "ms320",  "ms512",  "ms640",  "spare9",
                                "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 32, value, "drx_info_s::short_drx_s_::drx_short_cycle_e_");
}
uint16_t drx_info_s::short_drx_s_::drx_short_cycle_opts::to_number() const
{
  static const uint16_t numbers[] = {2,  3,  4,  5,  6,  7,   8,   10,  14,  16,  20, 30,
                                     32, 35, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(numbers, 23, value, "drx_info_s::short_drx_s_::drx_short_cycle_e_");
}

// MeasConfigSN ::= SEQUENCE
SRSASN_CODE meas_cfg_sn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_frequencies_sn.size() > 0, 1));

  if (measured_frequencies_sn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, measured_frequencies_sn, 1, 32));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_sn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool measured_frequencies_sn_present;
  HANDLE_CODE(bref.unpack(measured_frequencies_sn_present, 1));

  if (measured_frequencies_sn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(measured_frequencies_sn, bref, 1, 32));
  }

  return SRSASN_SUCCESS;
}
void meas_cfg_sn_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_frequencies_sn.size() > 0) {
    j.start_array("measuredFrequenciesSN");
    for (const auto& e1 : measured_frequencies_sn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// CG-Config-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_cell_group_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_mod_req_present, 1));
  HANDLE_CODE(bref.pack(drx_info_scg_present, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_cfg_sn_present, 1));
  HANDLE_CODE(bref.pack(sel_band_combination_present, 1));
  HANDLE_CODE(bref.pack(fr_info_list_scg.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_serving_freq_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_cell_group_cfg.size() > 0) {
    HANDLE_CODE(scg_cell_group_cfg.pack(bref));
  }
  if (scg_rb_cfg.size() > 0) {
    HANDLE_CODE(scg_rb_cfg.pack(bref));
  }
  if (cfg_restrict_mod_req_present) {
    HANDLE_CODE(cfg_restrict_mod_req.pack(bref));
  }
  if (drx_info_scg_present) {
    HANDLE_CODE(drx_info_scg.pack(bref));
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn.pack(bref));
  }
  if (meas_cfg_sn_present) {
    HANDLE_CODE(meas_cfg_sn.pack(bref));
  }
  if (sel_band_combination_present) {
    HANDLE_CODE(sel_band_combination.pack(bref));
  }
  if (fr_info_list_scg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, fr_info_list_scg, 1, 31));
  }
  if (candidate_serving_freq_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_serving_freq_list_nr, 1, 32, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_ies_s::unpack(cbit_ref& bref)
{
  bool scg_cell_group_cfg_present;
  HANDLE_CODE(bref.unpack(scg_cell_group_cfg_present, 1));
  bool scg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(scg_rb_cfg_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_mod_req_present, 1));
  HANDLE_CODE(bref.unpack(drx_info_scg_present, 1));
  bool candidate_cell_info_list_sn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_sn_present, 1));
  HANDLE_CODE(bref.unpack(sel_band_combination_present, 1));
  bool fr_info_list_scg_present;
  HANDLE_CODE(bref.unpack(fr_info_list_scg_present, 1));
  bool candidate_serving_freq_list_nr_present;
  HANDLE_CODE(bref.unpack(candidate_serving_freq_list_nr_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_cell_group_cfg_present) {
    HANDLE_CODE(scg_cell_group_cfg.unpack(bref));
  }
  if (scg_rb_cfg_present) {
    HANDLE_CODE(scg_rb_cfg.unpack(bref));
  }
  if (cfg_restrict_mod_req_present) {
    HANDLE_CODE(cfg_restrict_mod_req.unpack(bref));
  }
  if (drx_info_scg_present) {
    HANDLE_CODE(drx_info_scg.unpack(bref));
  }
  if (candidate_cell_info_list_sn_present) {
    HANDLE_CODE(candidate_cell_info_list_sn.unpack(bref));
  }
  if (meas_cfg_sn_present) {
    HANDLE_CODE(meas_cfg_sn.unpack(bref));
  }
  if (sel_band_combination_present) {
    HANDLE_CODE(sel_band_combination.unpack(bref));
  }
  if (fr_info_list_scg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(fr_info_list_scg, bref, 1, 31));
  }
  if (candidate_serving_freq_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_serving_freq_list_nr, bref, 1, 32, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_cell_group_cfg.size() > 0) {
    j.write_str("scg-CellGroupConfig", scg_cell_group_cfg.to_string());
  }
  if (scg_rb_cfg.size() > 0) {
    j.write_str("scg-RB-Config", scg_rb_cfg.to_string());
  }
  if (cfg_restrict_mod_req_present) {
    j.write_fieldname("configRestrictModReq");
    cfg_restrict_mod_req.to_json(j);
  }
  if (drx_info_scg_present) {
    j.write_fieldname("drx-InfoSCG");
    drx_info_scg.to_json(j);
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    j.write_str("candidateCellInfoListSN", candidate_cell_info_list_sn.to_string());
  }
  if (meas_cfg_sn_present) {
    j.write_fieldname("measConfigSN");
    meas_cfg_sn.to_json(j);
  }
  if (sel_band_combination_present) {
    j.write_fieldname("selectedBandCombination");
    sel_band_combination.to_json(j);
  }
  if (fr_info_list_scg.size() > 0) {
    j.start_array("fr-InfoListSCG");
    for (const auto& e1 : fr_info_list_scg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (candidate_serving_freq_list_nr.size() > 0) {
    j.start_array("candidateServingFreqListNR");
    for (const auto& e1 : candidate_serving_freq_list_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-Config ::= SEQUENCE
SRSASN_CODE cg_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void cg_cfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_s::crit_exts_c_::c1_c_& cg_cfg_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_cfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void cg_cfg_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cg_cfg_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_ies_s& cg_cfg_s::crit_exts_c_::c1_c_::set_cg_cfg()
{
  set(types::cg_cfg);
  return c;
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void cg_cfg_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cg_cfg:
      j.write_fieldname("cg-Config");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cg_cfg:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cg_cfg:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cg_cfg_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"cg-Config", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "cg_cfg_s::crit_exts_c_::c1_c_::types");
}

const char* cg_cfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "cg_cfg_s::crit_exts_c_::types");
}
uint8_t cg_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_cfg_s::crit_exts_c_::types");
}

// CandidateCellCPC-r17 ::= SEQUENCE
SRSASN_CODE candidate_cell_cp_c_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ssb_freq_r17, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_list_r17, 1, 8, integer_packer<uint16_t>(0, 1007)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE candidate_cell_cp_c_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ssb_freq_r17, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_list_r17, bref, 1, 8, integer_packer<uint16_t>(0, 1007)));

  return SRSASN_SUCCESS;
}
void candidate_cell_cp_c_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssbFrequency-r17", ssb_freq_r17);
  j.start_array("candidateCellList-r17");
  for (const auto& e1 : candidate_cell_list_r17) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// CG-ConfigInfo-v1730-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1730_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fr1_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.pack(fr2_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fr1_carriers_mcg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr1_carriers_mcg_r17, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_mcg_r17_present) {
    HANDLE_CODE(pack_integer(bref, fr2_carriers_mcg_r17, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1730_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fr1_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.unpack(fr2_carriers_mcg_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fr1_carriers_mcg_r17_present) {
    HANDLE_CODE(unpack_integer(fr1_carriers_mcg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }
  if (fr2_carriers_mcg_r17_present) {
    HANDLE_CODE(unpack_integer(fr2_carriers_mcg_r17, bref, (uint8_t)1u, (uint8_t)32u));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1730_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fr1_carriers_mcg_r17_present) {
    j.write_int("fr1-Carriers-MCG-r17", fr1_carriers_mcg_r17);
  }
  if (fr2_carriers_mcg_r17_present) {
    j.write_int("fr2-Carriers-MCG-r17", fr2_carriers_mcg_r17);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// CG-ConfigInfo-v1700-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(candidate_cell_list_cp_c_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(two_phr_mode_mcg_r17_present, 1));
  HANDLE_CODE(bref.pack(low_mob_eval_connected_in_pcell_r17_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (candidate_cell_list_cp_c_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_list_cp_c_r17, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool candidate_cell_list_cp_c_r17_present;
  HANDLE_CODE(bref.unpack(candidate_cell_list_cp_c_r17_present, 1));
  HANDLE_CODE(bref.unpack(two_phr_mode_mcg_r17_present, 1));
  HANDLE_CODE(bref.unpack(low_mob_eval_connected_in_pcell_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (candidate_cell_list_cp_c_r17_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_list_cp_c_r17, bref, 1, 8));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_cell_list_cp_c_r17.size() > 0) {
    j.start_array("candidateCellListCPC-r17");
    for (const auto& e1 : candidate_cell_list_cp_c_r17) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (two_phr_mode_mcg_r17_present) {
    j.write_str("twoPHRModeMCG-r17", "enabled");
  }
  if (low_mob_eval_connected_in_pcell_r17_present) {
    j.write_str("lowMobilityEvaluationConnectedInPCell-r17", "enabled");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-ConfigInfo-v1640-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1640_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_cell_info_list_mcg_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(serv_cell_info_list_mcg_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (serv_cell_info_list_mcg_nr_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_mcg_nr_r16, 1, 32));
  }
  if (serv_cell_info_list_mcg_eutra_r16.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_cell_info_list_mcg_eutra_r16, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1640_ies_s::unpack(cbit_ref& bref)
{
  bool serv_cell_info_list_mcg_nr_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_mcg_nr_r16_present, 1));
  bool serv_cell_info_list_mcg_eutra_r16_present;
  HANDLE_CODE(bref.unpack(serv_cell_info_list_mcg_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (serv_cell_info_list_mcg_nr_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_mcg_nr_r16, bref, 1, 32));
  }
  if (serv_cell_info_list_mcg_eutra_r16_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_cell_info_list_mcg_eutra_r16, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1640_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_cell_info_list_mcg_nr_r16.size() > 0) {
    j.start_array("servCellInfoListMCG-NR-r16");
    for (const auto& e1 : serv_cell_info_list_mcg_nr_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (serv_cell_info_list_mcg_eutra_r16.size() > 0) {
    j.start_array("servCellInfoListMCG-EUTRA-r16");
    for (const auto& e1 : serv_cell_info_list_mcg_eutra_r16) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-ConfigInfo-v1620-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1620_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_assist_info_source_scg_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_assist_info_source_scg_r16.size() > 0) {
    HANDLE_CODE(ue_assist_info_source_scg_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1620_ies_s::unpack(cbit_ref& bref)
{
  bool ue_assist_info_source_scg_r16_present;
  HANDLE_CODE(bref.unpack(ue_assist_info_source_scg_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_assist_info_source_scg_r16_present) {
    HANDLE_CODE(ue_assist_info_source_scg_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1620_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_assist_info_source_scg_r16.size() > 0) {
    j.write_str("ueAssistanceInformationSourceSCG-r16", ue_assist_info_source_scg_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// CG-ConfigInfo-v1610-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drx_info_mcg2_present, 1));
  HANDLE_CODE(bref.pack(aligned_drx_ind_present, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_r16_present, 1));
  HANDLE_CODE(bref.pack(dummy1_present, 1));
  HANDLE_CODE(bref.pack(sidelink_ue_info_nr_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(sidelink_ue_info_eutra_r16.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (drx_info_mcg2_present) {
    HANDLE_CODE(drx_info_mcg2.pack(bref));
  }
  if (scg_fail_info_r16_present) {
    HANDLE_CODE(scg_fail_info_r16.fail_type_r16.pack(bref));
    HANDLE_CODE(scg_fail_info_r16.meas_result_scg_r16.pack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.fail_type_eutra_r16.pack(bref));
    HANDLE_CODE(dummy1.meas_result_scg_eutra_r16.pack(bref));
  }
  if (sidelink_ue_info_nr_r16.size() > 0) {
    HANDLE_CODE(sidelink_ue_info_nr_r16.pack(bref));
  }
  if (sidelink_ue_info_eutra_r16.size() > 0) {
    HANDLE_CODE(sidelink_ue_info_eutra_r16.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drx_info_mcg2_present, 1));
  HANDLE_CODE(bref.unpack(aligned_drx_ind_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_r16_present, 1));
  HANDLE_CODE(bref.unpack(dummy1_present, 1));
  bool sidelink_ue_info_nr_r16_present;
  HANDLE_CODE(bref.unpack(sidelink_ue_info_nr_r16_present, 1));
  bool sidelink_ue_info_eutra_r16_present;
  HANDLE_CODE(bref.unpack(sidelink_ue_info_eutra_r16_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (drx_info_mcg2_present) {
    HANDLE_CODE(drx_info_mcg2.unpack(bref));
  }
  if (scg_fail_info_r16_present) {
    HANDLE_CODE(scg_fail_info_r16.fail_type_r16.unpack(bref));
    HANDLE_CODE(scg_fail_info_r16.meas_result_scg_r16.unpack(bref));
  }
  if (dummy1_present) {
    HANDLE_CODE(dummy1.fail_type_eutra_r16.unpack(bref));
    HANDLE_CODE(dummy1.meas_result_scg_eutra_r16.unpack(bref));
  }
  if (sidelink_ue_info_nr_r16_present) {
    HANDLE_CODE(sidelink_ue_info_nr_r16.unpack(bref));
  }
  if (sidelink_ue_info_eutra_r16_present) {
    HANDLE_CODE(sidelink_ue_info_eutra_r16.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drx_info_mcg2_present) {
    j.write_fieldname("drx-InfoMCG2");
    drx_info_mcg2.to_json(j);
  }
  if (aligned_drx_ind_present) {
    j.write_str("alignedDRX-Indication", "true");
  }
  if (scg_fail_info_r16_present) {
    j.write_fieldname("scgFailureInfo-r16");
    j.start_obj();
    j.write_str("failureType-r16", scg_fail_info_r16.fail_type_r16.to_string());
    j.write_str("measResultSCG-r16", scg_fail_info_r16.meas_result_scg_r16.to_string());
    j.end_obj();
  }
  if (dummy1_present) {
    j.write_fieldname("dummy1");
    j.start_obj();
    j.write_str("failureTypeEUTRA-r16", dummy1.fail_type_eutra_r16.to_string());
    j.write_str("measResultSCG-EUTRA-r16", dummy1.meas_result_scg_eutra_r16.to_string());
    j.end_obj();
  }
  if (sidelink_ue_info_nr_r16.size() > 0) {
    j.write_str("sidelinkUEInformationNR-r16", sidelink_ue_info_nr_r16.to_string());
  }
  if (sidelink_ue_info_eutra_r16.size() > 0) {
    j.write_str("sidelinkUEInformationEUTRA-r16", sidelink_ue_info_eutra_r16.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_opts::to_string() const
{
  static const char* names[] = {"scg-lbtFailure-r16",
                                "beamFailureRecoveryFailure-r16",
                                "t312-Expiry-r16",
                                "bh-RLF-r16",
                                "beamFailure-r17",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_e_");
}
uint16_t cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_opts::to_number() const
{
  if (value == t312_expiry_r16) {
    return 312;
  }
  invalid_enum_number(value, "cg_cfg_info_v1610_ies_s::scg_fail_info_r16_s_::fail_type_r16_e_");
  return 0;
}

const char* cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_opts::to_string() const
{
  static const char* names[] = {"scg-lbtFailure-r16",
                                "beamFailureRecoveryFailure-r16",
                                "t312-Expiry-r16",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 8, value, "cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_e_");
}
uint16_t cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_opts::to_number() const
{
  if (value == t312_expiry_r16) {
    return 312;
  }
  invalid_enum_number(value, "cg_cfg_info_v1610_ies_s::dummy1_s_::fail_type_eutra_r16_e_");
  return 0;
}

// CG-ConfigInfo-v1590-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1590_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serv_frequencies_mn_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (serv_frequencies_mn_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, serv_frequencies_mn_nr, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1590_ies_s::unpack(cbit_ref& bref)
{
  bool serv_frequencies_mn_nr_present;
  HANDLE_CODE(bref.unpack(serv_frequencies_mn_nr_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (serv_frequencies_mn_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(serv_frequencies_mn_nr, bref, 1, 31, integer_packer<uint32_t>(0, 3279165)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1590_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serv_frequencies_mn_nr.size() > 0) {
    j.start_array("servFrequenciesMN-NR");
    for (const auto& e1 : serv_frequencies_mn_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-UplinkCarrierMCG ::= SEQUENCE
SRSASN_CODE ph_ul_carrier_mcg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ph_type1or3.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_ul_carrier_mcg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ph_type1or3.unpack(bref));

  return SRSASN_SUCCESS;
}
void ph_ul_carrier_mcg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ph-Type1or3", ph_type1or3.to_string());
  j.end_obj();
}

const char* ph_ul_carrier_mcg_s::ph_type1or3_opts::to_string() const
{
  static const char* names[] = {"type1", "type3"};
  return convert_enum_idx(names, 2, value, "ph_ul_carrier_mcg_s::ph_type1or3_e_");
}
uint8_t ph_ul_carrier_mcg_s::ph_type1or3_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 3};
  return map_enum_number(numbers, 2, value, "ph_ul_carrier_mcg_s::ph_type1or3_e_");
}

// CG-ConfigInfo-v1570-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1570_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sftd_freq_list_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(sftd_freq_list_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (sftd_freq_list_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sftd_freq_list_nr, 1, 3, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sftd_freq_list_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, sftd_freq_list_eutra, 1, 3, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1570_ies_s::unpack(cbit_ref& bref)
{
  bool sftd_freq_list_nr_present;
  HANDLE_CODE(bref.unpack(sftd_freq_list_nr_present, 1));
  bool sftd_freq_list_eutra_present;
  HANDLE_CODE(bref.unpack(sftd_freq_list_eutra_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (sftd_freq_list_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sftd_freq_list_nr, bref, 1, 3, integer_packer<uint32_t>(0, 3279165)));
  }
  if (sftd_freq_list_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(sftd_freq_list_eutra, bref, 1, 3, integer_packer<uint32_t>(0, 262143)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1570_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sftd_freq_list_nr.size() > 0) {
    j.start_array("sftdFrequencyList-NR");
    for (const auto& e1 : sftd_freq_list_nr) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (sftd_freq_list_eutra.size() > 0) {
    j.start_array("sftdFrequencyList-EUTRA");
    for (const auto& e1 : sftd_freq_list_eutra) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PH-InfoMCG ::= SEQUENCE
SRSASN_CODE ph_info_mcg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.pack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= two_srs_pusch_repeat_r17_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ph_info_mcg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ph_supplementary_ul_present, 1));

  HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(ph_ul.unpack(bref));
  if (ph_supplementary_ul_present) {
    HANDLE_CODE(ph_supplementary_ul.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(two_srs_pusch_repeat_r17_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void ph_info_mcg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellIndex", serv_cell_idx);
  j.write_fieldname("ph-Uplink");
  ph_ul.to_json(j);
  if (ph_supplementary_ul_present) {
    j.write_fieldname("ph-SupplementaryUplink");
    ph_supplementary_ul.to_json(j);
  }
  if (ext) {
    if (two_srs_pusch_repeat_r17_present) {
      j.write_str("twoSRS-PUSCH-Repetition-r17", "enabled");
    }
  }
  j.end_obj();
}

// CG-ConfigInfo-v1560-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1560_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(candidate_cell_info_list_mn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(source_cfg_scg_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_eutra_present, 1));
  HANDLE_CODE(bref.pack(drx_cfg_mcg_present, 1));
  HANDLE_CODE(bref.pack(meas_result_report_cgi_eutra_present, 1));
  HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_eutra.size() > 0, 1));
  HANDLE_CODE(bref.pack(fr_info_list_mcg.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_mn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_mn_eutra.pack(bref));
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.pack(bref));
  }
  if (source_cfg_scg_eutra.size() > 0) {
    HANDLE_CODE(source_cfg_scg_eutra.pack(bref));
  }
  if (scg_fail_info_eutra_present) {
    HANDLE_CODE(scg_fail_info_eutra.fail_type_eutra.pack(bref));
    HANDLE_CODE(scg_fail_info_eutra.meas_result_scg_eutra.pack(bref));
  }
  if (drx_cfg_mcg_present) {
    HANDLE_CODE(drx_cfg_mcg.pack(bref));
  }
  if (meas_result_report_cgi_eutra_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi_eutra.eutra_freq, (uint32_t)0u, (uint32_t)262143u));
    HANDLE_CODE(pack_integer(
        bref, meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(meas_result_report_cgi_eutra.cgi_info_eutra.pack(bref));
  }
  if (meas_result_cell_list_sftd_eutra.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_cell_list_sftd_eutra, 1, 3));
  }
  if (fr_info_list_mcg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, fr_info_list_mcg, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1560_ies_s::unpack(cbit_ref& bref)
{
  bool candidate_cell_info_list_mn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_mn_eutra_present, 1));
  bool candidate_cell_info_list_sn_eutra_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_eutra_present, 1));
  bool source_cfg_scg_eutra_present;
  HANDLE_CODE(bref.unpack(source_cfg_scg_eutra_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_eutra_present, 1));
  HANDLE_CODE(bref.unpack(drx_cfg_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_report_cgi_eutra_present, 1));
  bool meas_result_cell_list_sftd_eutra_present;
  HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_eutra_present, 1));
  bool fr_info_list_mcg_present;
  HANDLE_CODE(bref.unpack(fr_info_list_mcg_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (candidate_cell_info_list_mn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_mn_eutra.unpack(bref));
  }
  if (candidate_cell_info_list_sn_eutra_present) {
    HANDLE_CODE(candidate_cell_info_list_sn_eutra.unpack(bref));
  }
  if (source_cfg_scg_eutra_present) {
    HANDLE_CODE(source_cfg_scg_eutra.unpack(bref));
  }
  if (scg_fail_info_eutra_present) {
    HANDLE_CODE(scg_fail_info_eutra.fail_type_eutra.unpack(bref));
    HANDLE_CODE(scg_fail_info_eutra.meas_result_scg_eutra.unpack(bref));
  }
  if (drx_cfg_mcg_present) {
    HANDLE_CODE(drx_cfg_mcg.unpack(bref));
  }
  if (meas_result_report_cgi_eutra_present) {
    HANDLE_CODE(unpack_integer(meas_result_report_cgi_eutra.eutra_freq, bref, (uint32_t)0u, (uint32_t)262143u));
    HANDLE_CODE(unpack_integer(
        meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra, bref, (uint16_t)0u, (uint16_t)503u));
    HANDLE_CODE(meas_result_report_cgi_eutra.cgi_info_eutra.unpack(bref));
  }
  if (meas_result_cell_list_sftd_eutra_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_cell_list_sftd_eutra, bref, 1, 3));
  }
  if (fr_info_list_mcg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(fr_info_list_mcg, bref, 1, 31));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1560_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (candidate_cell_info_list_mn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListMN-EUTRA", candidate_cell_info_list_mn_eutra.to_string());
  }
  if (candidate_cell_info_list_sn_eutra.size() > 0) {
    j.write_str("candidateCellInfoListSN-EUTRA", candidate_cell_info_list_sn_eutra.to_string());
  }
  if (source_cfg_scg_eutra.size() > 0) {
    j.write_str("sourceConfigSCG-EUTRA", source_cfg_scg_eutra.to_string());
  }
  if (scg_fail_info_eutra_present) {
    j.write_fieldname("scgFailureInfoEUTRA");
    j.start_obj();
    j.write_str("failureTypeEUTRA", scg_fail_info_eutra.fail_type_eutra.to_string());
    j.write_str("measResultSCG-EUTRA", scg_fail_info_eutra.meas_result_scg_eutra.to_string());
    j.end_obj();
  }
  if (drx_cfg_mcg_present) {
    j.write_fieldname("drx-ConfigMCG");
    drx_cfg_mcg.to_json(j);
  }
  if (meas_result_report_cgi_eutra_present) {
    j.write_fieldname("measResultReportCGI-EUTRA");
    j.start_obj();
    j.write_int("eutraFrequency", meas_result_report_cgi_eutra.eutra_freq);
    j.write_int("cellForWhichToReportCGI-EUTRA", meas_result_report_cgi_eutra.cell_for_which_to_report_cgi_eutra);
    j.write_fieldname("cgi-InfoEUTRA");
    meas_result_report_cgi_eutra.cgi_info_eutra.to_json(j);
    j.end_obj();
  }
  if (meas_result_cell_list_sftd_eutra.size() > 0) {
    j.start_array("measResultCellListSFTD-EUTRA");
    for (const auto& e1 : meas_result_cell_list_sftd_eutra) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (fr_info_list_mcg.size() > 0) {
    j.start_array("fr-InfoListMCG");
    for (const auto& e1 : fr_info_list_mcg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_opts::to_string() const
{
  static const char* names[] = {"t313-Expiry", "randomAccessProblem", "rlc-MaxNumRetx", "scg-ChangeFailure"};
  return convert_enum_idx(names, 4, value, "cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_e_");
}
uint16_t cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_opts::to_number() const
{
  static const uint16_t numbers[] = {313};
  return map_enum_number(numbers, 1, value, "cg_cfg_info_v1560_ies_s::scg_fail_info_eutra_s_::fail_type_eutra_e_");
}

// CG-ConfigInfo-v1540-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_v1540_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ph_info_mcg.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_report_cgi_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ph_info_mcg.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ph_info_mcg, 1, 32));
  }
  if (meas_result_report_cgi_present) {
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi.ssb_freq, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(pack_integer(bref, meas_result_report_cgi.cell_for_which_to_report_cgi, (uint16_t)0u, (uint16_t)1007u));
    HANDLE_CODE(meas_result_report_cgi.cgi_info.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_v1540_ies_s::unpack(cbit_ref& bref)
{
  bool ph_info_mcg_present;
  HANDLE_CODE(bref.unpack(ph_info_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_report_cgi_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ph_info_mcg_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ph_info_mcg, bref, 1, 32));
  }
  if (meas_result_report_cgi_present) {
    HANDLE_CODE(unpack_integer(meas_result_report_cgi.ssb_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(
        unpack_integer(meas_result_report_cgi.cell_for_which_to_report_cgi, bref, (uint16_t)0u, (uint16_t)1007u));
    HANDLE_CODE(meas_result_report_cgi.cgi_info.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_v1540_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ph_info_mcg.size() > 0) {
    j.start_array("ph-InfoMCG");
    for (const auto& e1 : ph_info_mcg) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_report_cgi_present) {
    j.write_fieldname("measResultReportCGI");
    j.start_obj();
    j.write_int("ssbFrequency", meas_result_report_cgi.ssb_freq);
    j.write_int("cellForWhichToReportCGI", meas_result_report_cgi.cell_for_which_to_report_cgi);
    j.write_fieldname("cgi-Info");
    meas_result_report_cgi.cgi_info.to_json(j);
    j.end_obj();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MRDC-AssistanceInfo ::= SEQUENCE
SRSASN_CODE mrdc_assist_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, affected_carrier_freq_comb_info_list_mrdc, 1, 128));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= overheat_assist_scg_r16.size() > 0;
    group_flags[1] |= overheat_assist_scg_fr2_2_r17.size() > 0;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(overheat_assist_scg_r16.size() > 0, 1));
      if (overheat_assist_scg_r16.size() > 0) {
        HANDLE_CODE(overheat_assist_scg_r16.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(overheat_assist_scg_fr2_2_r17.size() > 0, 1));
      if (overheat_assist_scg_fr2_2_r17.size() > 0) {
        HANDLE_CODE(overheat_assist_scg_fr2_2_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_assist_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(affected_carrier_freq_comb_info_list_mrdc, bref, 1, 128));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool overheat_assist_scg_r16_present;
      HANDLE_CODE(bref.unpack(overheat_assist_scg_r16_present, 1));
      if (overheat_assist_scg_r16_present) {
        HANDLE_CODE(overheat_assist_scg_r16.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool overheat_assist_scg_fr2_2_r17_present;
      HANDLE_CODE(bref.unpack(overheat_assist_scg_fr2_2_r17_present, 1));
      if (overheat_assist_scg_fr2_2_r17_present) {
        HANDLE_CODE(overheat_assist_scg_fr2_2_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void mrdc_assist_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("affectedCarrierFreqCombInfoListMRDC");
  for (const auto& e1 : affected_carrier_freq_comb_info_list_mrdc) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (overheat_assist_scg_r16.size() > 0) {
      j.write_str("overheatingAssistanceSCG-r16", overheat_assist_scg_r16.to_string());
    }
    if (overheat_assist_scg_fr2_2_r17.size() > 0) {
      j.write_str("overheatingAssistanceSCG-FR2-2-r17", overheat_assist_scg_fr2_2_r17.to_string());
    }
  }
  j.end_obj();
}

// MeasConfigMN ::= SEQUENCE
SRSASN_CODE meas_cfg_mn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(measured_frequencies_mn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.pack(gap_purpose_present, 1));

  if (measured_frequencies_mn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, measured_frequencies_mn, 1, 32));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.pack(bref));
  }
  if (gap_purpose_present) {
    HANDLE_CODE(gap_purpose.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= meas_gap_cfg_fr2.is_present();
    group_flags[1] |= inter_freq_no_gap_r16_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(meas_gap_cfg_fr2.is_present(), 1));
      if (meas_gap_cfg_fr2.is_present()) {
        HANDLE_CODE(meas_gap_cfg_fr2->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(inter_freq_no_gap_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cfg_mn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool measured_frequencies_mn_present;
  HANDLE_CODE(bref.unpack(measured_frequencies_mn_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_cfg_present, 1));
  HANDLE_CODE(bref.unpack(gap_purpose_present, 1));

  if (measured_frequencies_mn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(measured_frequencies_mn, bref, 1, 32));
  }
  if (meas_gap_cfg_present) {
    HANDLE_CODE(meas_gap_cfg.unpack(bref));
  }
  if (gap_purpose_present) {
    HANDLE_CODE(gap_purpose.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(meas_gap_cfg_fr2, bref);
      if (meas_gap_cfg_fr2.is_present()) {
        HANDLE_CODE(meas_gap_cfg_fr2->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(inter_freq_no_gap_r16_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cfg_mn_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (measured_frequencies_mn.size() > 0) {
    j.start_array("measuredFrequenciesMN");
    for (const auto& e1 : measured_frequencies_mn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_gap_cfg_present) {
    j.write_fieldname("measGapConfig");
    meas_gap_cfg.to_json(j);
  }
  if (gap_purpose_present) {
    j.write_str("gapPurpose", gap_purpose.to_string());
  }
  if (ext) {
    if (meas_gap_cfg_fr2.is_present()) {
      j.write_fieldname("measGapConfigFR2");
      meas_gap_cfg_fr2->to_json(j);
    }
    if (inter_freq_no_gap_r16_present) {
      j.write_str("interFreqNoGap-r16", "true");
    }
  }
  j.end_obj();
}

const char* meas_cfg_mn_s::gap_purpose_opts::to_string() const
{
  static const char* names[] = {"perUE", "perFR1"};
  return convert_enum_idx(names, 2, value, "meas_cfg_mn_s::gap_purpose_e_");
}
uint8_t meas_cfg_mn_s::gap_purpose_opts::to_number() const
{
  if (value == per_fr1) {
    return 1;
  }
  invalid_enum_number(value, "meas_cfg_mn_s::gap_purpose_e_");
  return 0;
}

// CG-ConfigInfo-IEs ::= SEQUENCE
SRSASN_CODE cg_cfg_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_cap_info.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_mn.size() > 0, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list_sn.size() > 0, 1));
  HANDLE_CODE(bref.pack(meas_result_cell_list_sftd_nr.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_fail_info_present, 1));
  HANDLE_CODE(bref.pack(cfg_restrict_info_present, 1));
  HANDLE_CODE(bref.pack(drx_info_mcg_present, 1));
  HANDLE_CODE(bref.pack(meas_cfg_mn_present, 1));
  HANDLE_CODE(bref.pack(source_cfg_scg.size() > 0, 1));
  HANDLE_CODE(bref.pack(scg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(mcg_rb_cfg.size() > 0, 1));
  HANDLE_CODE(bref.pack(mrdc_assist_info_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_cap_info.size() > 0) {
    HANDLE_CODE(ue_cap_info.pack(bref));
  }
  if (candidate_cell_info_list_mn.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_info_list_mn, 1, 8));
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    HANDLE_CODE(candidate_cell_info_list_sn.pack(bref));
  }
  if (meas_result_cell_list_sftd_nr.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_cell_list_sftd_nr, 1, 3));
  }
  if (scg_fail_info_present) {
    HANDLE_CODE(scg_fail_info.fail_type.pack(bref));
    HANDLE_CODE(scg_fail_info.meas_result_scg.pack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.pack(bref));
  }
  if (drx_info_mcg_present) {
    HANDLE_CODE(drx_info_mcg.pack(bref));
  }
  if (meas_cfg_mn_present) {
    HANDLE_CODE(meas_cfg_mn.pack(bref));
  }
  if (source_cfg_scg.size() > 0) {
    HANDLE_CODE(source_cfg_scg.pack(bref));
  }
  if (scg_rb_cfg.size() > 0) {
    HANDLE_CODE(scg_rb_cfg.pack(bref));
  }
  if (mcg_rb_cfg.size() > 0) {
    HANDLE_CODE(mcg_rb_cfg.pack(bref));
  }
  if (mrdc_assist_info_present) {
    HANDLE_CODE(mrdc_assist_info.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_ies_s::unpack(cbit_ref& bref)
{
  bool ue_cap_info_present;
  HANDLE_CODE(bref.unpack(ue_cap_info_present, 1));
  bool candidate_cell_info_list_mn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_mn_present, 1));
  bool candidate_cell_info_list_sn_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_sn_present, 1));
  bool meas_result_cell_list_sftd_nr_present;
  HANDLE_CODE(bref.unpack(meas_result_cell_list_sftd_nr_present, 1));
  HANDLE_CODE(bref.unpack(scg_fail_info_present, 1));
  HANDLE_CODE(bref.unpack(cfg_restrict_info_present, 1));
  HANDLE_CODE(bref.unpack(drx_info_mcg_present, 1));
  HANDLE_CODE(bref.unpack(meas_cfg_mn_present, 1));
  bool source_cfg_scg_present;
  HANDLE_CODE(bref.unpack(source_cfg_scg_present, 1));
  bool scg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(scg_rb_cfg_present, 1));
  bool mcg_rb_cfg_present;
  HANDLE_CODE(bref.unpack(mcg_rb_cfg_present, 1));
  HANDLE_CODE(bref.unpack(mrdc_assist_info_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_cap_info_present) {
    HANDLE_CODE(ue_cap_info.unpack(bref));
  }
  if (candidate_cell_info_list_mn_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_info_list_mn, bref, 1, 8));
  }
  if (candidate_cell_info_list_sn_present) {
    HANDLE_CODE(candidate_cell_info_list_sn.unpack(bref));
  }
  if (meas_result_cell_list_sftd_nr_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_cell_list_sftd_nr, bref, 1, 3));
  }
  if (scg_fail_info_present) {
    HANDLE_CODE(scg_fail_info.fail_type.unpack(bref));
    HANDLE_CODE(scg_fail_info.meas_result_scg.unpack(bref));
  }
  if (cfg_restrict_info_present) {
    HANDLE_CODE(cfg_restrict_info.unpack(bref));
  }
  if (drx_info_mcg_present) {
    HANDLE_CODE(drx_info_mcg.unpack(bref));
  }
  if (meas_cfg_mn_present) {
    HANDLE_CODE(meas_cfg_mn.unpack(bref));
  }
  if (source_cfg_scg_present) {
    HANDLE_CODE(source_cfg_scg.unpack(bref));
  }
  if (scg_rb_cfg_present) {
    HANDLE_CODE(scg_rb_cfg.unpack(bref));
  }
  if (mcg_rb_cfg_present) {
    HANDLE_CODE(mcg_rb_cfg.unpack(bref));
  }
  if (mrdc_assist_info_present) {
    HANDLE_CODE(mrdc_assist_info.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void cg_cfg_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_cap_info.size() > 0) {
    j.write_str("ue-CapabilityInfo", ue_cap_info.to_string());
  }
  if (candidate_cell_info_list_mn.size() > 0) {
    j.start_array("candidateCellInfoListMN");
    for (const auto& e1 : candidate_cell_info_list_mn) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (candidate_cell_info_list_sn.size() > 0) {
    j.write_str("candidateCellInfoListSN", candidate_cell_info_list_sn.to_string());
  }
  if (meas_result_cell_list_sftd_nr.size() > 0) {
    j.start_array("measResultCellListSFTD-NR");
    for (const auto& e1 : meas_result_cell_list_sftd_nr) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scg_fail_info_present) {
    j.write_fieldname("scgFailureInfo");
    j.start_obj();
    j.write_str("failureType", scg_fail_info.fail_type.to_string());
    j.write_str("measResultSCG", scg_fail_info.meas_result_scg.to_string());
    j.end_obj();
  }
  if (cfg_restrict_info_present) {
    j.write_fieldname("configRestrictInfo");
    cfg_restrict_info.to_json(j);
  }
  if (drx_info_mcg_present) {
    j.write_fieldname("drx-InfoMCG");
    drx_info_mcg.to_json(j);
  }
  if (meas_cfg_mn_present) {
    j.write_fieldname("measConfigMN");
    meas_cfg_mn.to_json(j);
  }
  if (source_cfg_scg.size() > 0) {
    j.write_str("sourceConfigSCG", source_cfg_scg.to_string());
  }
  if (scg_rb_cfg.size() > 0) {
    j.write_str("scg-RB-Config", scg_rb_cfg.to_string());
  }
  if (mcg_rb_cfg.size() > 0) {
    j.write_str("mcg-RB-Config", mcg_rb_cfg.to_string());
  }
  if (mrdc_assist_info_present) {
    j.write_fieldname("mrdc-AssistanceInfo");
    mrdc_assist_info.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_opts::to_string() const
{
  static const char* names[] = {"t310-Expiry",
                                "randomAccessProblem",
                                "rlc-MaxNumRetx",
                                "synchReconfigFailure-SCG",
                                "scg-reconfigFailure",
                                "srb3-IntegrityFailure"};
  return convert_enum_idx(names, 6, value, "cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_e_");
}
uint16_t cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case srb3_integrity_fail:
      return 3;
    default:
      invalid_enum_number(value, "cg_cfg_info_ies_s::scg_fail_info_s_::fail_type_e_");
  }
  return 0;
}

// CG-ConfigInfo ::= SEQUENCE
SRSASN_CODE cg_cfg_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void cg_cfg_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void cg_cfg_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_info_s::crit_exts_c_::c1_c_& cg_cfg_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_cfg_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void cg_cfg_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cg_cfg_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_info_ies_s& cg_cfg_info_s::crit_exts_c_::c1_c_::set_cg_cfg_info()
{
  set(types::cg_cfg_info);
  return c;
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void cg_cfg_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cg_cfg_info:
      j.write_fieldname("cg-ConfigInfo");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cg_cfg_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cg_cfg_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cg_cfg_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "cg_cfg_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cg_cfg_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"cg-ConfigInfo", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "cg_cfg_info_s::crit_exts_c_::c1_c_::types");
}

const char* cg_cfg_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "cg_cfg_info_s::crit_exts_c_::types");
}
uint8_t cg_cfg_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_cfg_info_s::crit_exts_c_::types");
}

// HandoverCommand-IEs ::= SEQUENCE
SRSASN_CODE ho_cmd_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ho_cmd_msg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ho_cmd_msg.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_cmd_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("handoverCommandMessage", ho_cmd_msg.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// HandoverCommand ::= SEQUENCE
SRSASN_CODE ho_cmd_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_cmd_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_s::crit_exts_c_::c1_c_& ho_cmd_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ho_cmd_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_cmd_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_ies_s& ho_cmd_s::crit_exts_c_::c1_c_::set_ho_cmd()
{
  set(types::ho_cmd);
  return c;
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ho_cmd_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_cmd:
      j.write_fieldname("handoverCommand");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_cmd:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_cmd_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_cmd:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_cmd_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ho_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"handoverCommand", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ho_cmd_s::crit_exts_c_::c1_c_::types");
}

const char* ho_cmd_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ho_cmd_s::crit_exts_c_::types");
}
uint8_t ho_cmd_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ho_cmd_s::crit_exts_c_::types");
}

// RRM-Config ::= SEQUENCE
SRSASN_CODE rrm_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ue_inactive_time_present, 1));
  HANDLE_CODE(bref.pack(candidate_cell_info_list.size() > 0, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.pack(bref));
  }
  if (candidate_cell_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_cell_info_list, 1, 8));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= candidate_cell_info_list_sn_eutra.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(candidate_cell_info_list_sn_eutra.is_present(), 1));
      if (candidate_cell_info_list_sn_eutra.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *candidate_cell_info_list_sn_eutra, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrm_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ue_inactive_time_present, 1));
  bool candidate_cell_info_list_present;
  HANDLE_CODE(bref.unpack(candidate_cell_info_list_present, 1));

  if (ue_inactive_time_present) {
    HANDLE_CODE(ue_inactive_time.unpack(bref));
  }
  if (candidate_cell_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(candidate_cell_info_list, bref, 1, 8));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(candidate_cell_info_list_sn_eutra, bref);
      if (candidate_cell_info_list_sn_eutra.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*candidate_cell_info_list_sn_eutra, bref, 1, 32));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rrm_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_inactive_time_present) {
    j.write_str("ue-InactiveTime", ue_inactive_time.to_string());
  }
  if (candidate_cell_info_list.size() > 0) {
    j.start_array("candidateCellInfoList");
    for (const auto& e1 : candidate_cell_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (candidate_cell_info_list_sn_eutra.is_present()) {
      j.start_array("candidateCellInfoListSN-EUTRA");
      for (const auto& e1 : *candidate_cell_info_list_sn_eutra) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* rrm_cfg_s::ue_inactive_time_opts::to_string() const
{
  static const char* names[] = {
      "s1",    "s2",       "s3",    "s5",           "s7",      "s10",   "s15",     "s20",      "s25",     "s30",
      "s40",   "s50",      "min1",  "min1s20",      "min1s40", "min2",  "min2s30", "min3",     "min3s30", "min4",
      "min5",  "min6",     "min7",  "min8",         "min9",    "min10", "min12",   "min14",    "min17",   "min20",
      "min24", "min28",    "min33", "min38",        "min44",   "min50", "hr1",     "hr1min30", "hr2",     "hr2min30",
      "hr3",   "hr3min30", "hr4",   "hr5",          "hr6",     "hr8",   "hr10",    "hr13",     "hr16",    "hr20",
      "day1",  "day1hr12", "day2",  "day2hr12",     "day3",    "day4",  "day5",    "day7",     "day10",   "day14",
      "day19", "day24",    "day30", "dayMoreThan30"};
  return convert_enum_idx(names, 64, value, "rrm_cfg_s::ue_inactive_time_e_");
}

// HandoverPreparationInformation-IEs ::= SEQUENCE
SRSASN_CODE ho_prep_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(source_cfg_present, 1));
  HANDLE_CODE(bref.pack(rrm_cfg_present, 1));
  HANDLE_CODE(bref.pack(as_context_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, ue_cap_rat_list, 0, 8));
  if (source_cfg_present) {
    HANDLE_CODE(source_cfg.pack(bref));
  }
  if (rrm_cfg_present) {
    HANDLE_CODE(rrm_cfg.pack(bref));
  }
  if (as_context_present) {
    HANDLE_CODE(as_context.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(source_cfg_present, 1));
  HANDLE_CODE(bref.unpack(rrm_cfg_present, 1));
  HANDLE_CODE(bref.unpack(as_context_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(ue_cap_rat_list, bref, 0, 8));
  if (source_cfg_present) {
    HANDLE_CODE(source_cfg.unpack(bref));
  }
  if (rrm_cfg_present) {
    HANDLE_CODE(rrm_cfg.unpack(bref));
  }
  if (as_context_present) {
    HANDLE_CODE(as_context.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ho_prep_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("ue-CapabilityRAT-List");
  for (const auto& e1 : ue_cap_rat_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (source_cfg_present) {
    j.write_fieldname("sourceConfig");
    source_cfg.to_json(j);
  }
  if (rrm_cfg_present) {
    j.write_fieldname("rrm-Config");
    rrm_cfg.to_json(j);
  }
  if (as_context_present) {
    j.write_fieldname("as-Context");
    as_context.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// HandoverPreparationInformation ::= SEQUENCE
SRSASN_CODE ho_prep_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ho_prep_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ho_prep_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_s::crit_exts_c_::c1_c_& ho_prep_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_prep_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ho_prep_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ho_prep_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_ies_s& ho_prep_info_s::crit_exts_c_::c1_c_::set_ho_prep_info()
{
  set(types::ho_prep_info);
  return c;
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ho_prep_info:
      j.write_fieldname("handoverPreparationInformation");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ho_prep_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ho_prep_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ho_prep_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ho_prep_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"handoverPreparationInformation", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "ho_prep_info_s::crit_exts_c_::c1_c_::types");
}

const char* ho_prep_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ho_prep_info_s::crit_exts_c_::types");
}
uint8_t ho_prep_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ho_prep_info_s::crit_exts_c_::types");
}

// MeasTiming ::= SEQUENCE
SRSASN_CODE meas_timing_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(freq_and_timing_present, 1));

  if (freq_and_timing_present) {
    HANDLE_CODE(bref.pack(freq_and_timing.ss_rssi_meas_present, 1));
    HANDLE_CODE(pack_integer(bref, freq_and_timing.carrier_freq, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(freq_and_timing.ssb_subcarrier_spacing.pack(bref));
    HANDLE_CODE(freq_and_timing.ssb_meas_timing_cfg.pack(bref));
    if (freq_and_timing.ss_rssi_meas_present) {
      HANDLE_CODE(freq_and_timing.ss_rssi_meas.pack(bref));
    }
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ssb_to_measure.is_present();
    group_flags[0] |= pci_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ssb_to_measure.is_present(), 1));
      HANDLE_CODE(bref.pack(pci_present, 1));
      if (ssb_to_measure.is_present()) {
        HANDLE_CODE(ssb_to_measure->pack(bref));
      }
      if (pci_present) {
        HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(freq_and_timing_present, 1));

  if (freq_and_timing_present) {
    HANDLE_CODE(bref.unpack(freq_and_timing.ss_rssi_meas_present, 1));
    HANDLE_CODE(unpack_integer(freq_and_timing.carrier_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
    HANDLE_CODE(freq_and_timing.ssb_subcarrier_spacing.unpack(bref));
    HANDLE_CODE(freq_and_timing.ssb_meas_timing_cfg.unpack(bref));
    if (freq_and_timing.ss_rssi_meas_present) {
      HANDLE_CODE(freq_and_timing.ss_rssi_meas.unpack(bref));
    }
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(ssb_to_measure, bref);
      HANDLE_CODE(bref.unpack(pci_present, 1));
      if (ssb_to_measure.is_present()) {
        HANDLE_CODE(ssb_to_measure->unpack(bref));
      }
      if (pci_present) {
        HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_timing_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (freq_and_timing_present) {
    j.write_fieldname("frequencyAndTiming");
    j.start_obj();
    j.write_int("carrierFreq", freq_and_timing.carrier_freq);
    j.write_str("ssbSubcarrierSpacing", freq_and_timing.ssb_subcarrier_spacing.to_string());
    j.write_fieldname("ssb-MeasurementTimingConfiguration");
    freq_and_timing.ssb_meas_timing_cfg.to_json(j);
    if (freq_and_timing.ss_rssi_meas_present) {
      j.write_fieldname("ss-RSSI-Measurement");
      freq_and_timing.ss_rssi_meas.to_json(j);
    }
    j.end_obj();
  }
  if (ext) {
    if (ssb_to_measure.is_present()) {
      j.write_fieldname("ssb-ToMeasure");
      ssb_to_measure->to_json(j);
    }
    if (pci_present) {
      j.write_int("physCellId", pci);
    }
  }
  j.end_obj();
}

// MeasurementTimingConfiguration-v1610-IEs ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_v1610_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_subcarrier_spacing_r16.pack(bref));
  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_cell_mob_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, csi_rs_cfg_r16.ref_ssb_freq_r16, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_v1610_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_subcarrier_spacing_r16.unpack(bref));
  HANDLE_CODE(csi_rs_cfg_r16.csi_rs_cell_mob_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(csi_rs_cfg_r16.ref_ssb_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_v1610_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("csi-RS-Config-r16");
  j.start_obj();
  j.write_str("csi-RS-SubcarrierSpacing-r16", csi_rs_cfg_r16.csi_rs_subcarrier_spacing_r16.to_string());
  j.write_fieldname("csi-RS-CellMobility-r16");
  csi_rs_cfg_r16.csi_rs_cell_mob_r16.to_json(j);
  j.write_int("refSSBFreq-r16", csi_rs_cfg_r16.ref_ssb_freq_r16);
  j.end_obj();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MeasurementTimingConfiguration-v1550-IEs ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_v1550_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.pack(camp_on_first_ssb, 1));
  HANDLE_CODE(bref.pack(ps_cell_only_on_first_ssb, 1));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_v1550_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(bref.unpack(camp_on_first_ssb, 1));
  HANDLE_CODE(bref.unpack(ps_cell_only_on_first_ssb, 1));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_v1550_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("campOnFirstSSB", camp_on_first_ssb);
  j.write_bool("psCellOnlyOnFirstSSB", ps_cell_only_on_first_ssb);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MeasurementTimingConfiguration-IEs ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_timing.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_timing.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_timing, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_ies_s::unpack(cbit_ref& bref)
{
  bool meas_timing_present;
  HANDLE_CODE(bref.unpack(meas_timing_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_timing_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_timing, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_timing.size() > 0) {
    j.start_array("measTiming");
    for (const auto& e1 : meas_timing) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MeasurementTimingConfiguration ::= SEQUENCE
SRSASN_CODE meas_timing_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_timing_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void meas_timing_cfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_timing_cfg_s::crit_exts_c_::c1_c_& meas_timing_cfg_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void meas_timing_cfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void meas_timing_cfg_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void meas_timing_cfg_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
meas_timing_cfg_ies_s& meas_timing_cfg_s::crit_exts_c_::c1_c_::set_meas_timing_conf()
{
  set(types::meas_timing_conf);
  return c;
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_timing_conf:
      j.write_fieldname("measTimingConf");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_timing_conf:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_timing_cfg_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_timing_conf:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "meas_timing_cfg_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_timing_cfg_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {"measTimingConf", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 4, value, "meas_timing_cfg_s::crit_exts_c_::c1_c_::types");
}

const char* meas_timing_cfg_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "meas_timing_cfg_s::crit_exts_c_::types");
}
uint8_t meas_timing_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "meas_timing_cfg_s::crit_exts_c_::types");
}

// SL-AccessInfo-L2U2N-r17 ::= SEQUENCE
SRSASN_CODE sl_access_info_l2_u2_n_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(cell_access_related_info_r17.pack(bref));
  HANDLE_CODE(sl_serving_cell_info_r17.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_access_info_l2_u2_n_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(cell_access_related_info_r17.unpack(bref));
  HANDLE_CODE(sl_serving_cell_info_r17.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_access_info_l2_u2_n_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("cellAccessRelatedInfo-r17");
  cell_access_related_info_r17.to_json(j);
  j.write_fieldname("sl-ServingCellInfo-r17");
  sl_serving_cell_info_r17.to_json(j);
  j.end_obj();
}

// UERadioAccessCapabilityInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(ue_radio_access_cap_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ue-RadioAccessCapabilityInfo", ue_radio_access_cap_info.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// UERadioAccessCapabilityInformation ::= SEQUENCE
SRSASN_CODE ue_radio_access_cap_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_access_cap_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_access_cap_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_s::crit_exts_c_::c1_c_& ue_radio_access_cap_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_radio_access_cap_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_ies_s& ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_ue_radio_access_cap_info()
{
  set(types::ue_radio_access_cap_info);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_access_cap_info:
      j.write_fieldname("ueRadioAccessCapabilityInformation");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_access_cap_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_access_cap_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "ueRadioAccessCapabilityInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_access_cap_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_radio_access_cap_info_s::crit_exts_c_::types");
}
uint8_t ue_radio_access_cap_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_radio_access_cap_info_s::crit_exts_c_::types");
}

// UERadioPagingInformation-v1700-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_v1700_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ue_radio_paging_info_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(inactive_state_po_determination_r17_present, 1));
  HANDLE_CODE(bref.pack(nof_rx_red_cap_r17_present, 1));
  HANDLE_CODE(bref.pack(half_duplex_fdd_type_a_red_cap_r17.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (ue_radio_paging_info_r17.size() > 0) {
    HANDLE_CODE(ue_radio_paging_info_r17.pack(bref));
  }
  if (nof_rx_red_cap_r17_present) {
    HANDLE_CODE(nof_rx_red_cap_r17.pack(bref));
  }
  if (half_duplex_fdd_type_a_red_cap_r17.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, half_duplex_fdd_type_a_red_cap_r17, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_v1700_ies_s::unpack(cbit_ref& bref)
{
  bool ue_radio_paging_info_r17_present;
  HANDLE_CODE(bref.unpack(ue_radio_paging_info_r17_present, 1));
  HANDLE_CODE(bref.unpack(inactive_state_po_determination_r17_present, 1));
  HANDLE_CODE(bref.unpack(nof_rx_red_cap_r17_present, 1));
  bool half_duplex_fdd_type_a_red_cap_r17_present;
  HANDLE_CODE(bref.unpack(half_duplex_fdd_type_a_red_cap_r17_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (ue_radio_paging_info_r17_present) {
    HANDLE_CODE(ue_radio_paging_info_r17.unpack(bref));
  }
  if (nof_rx_red_cap_r17_present) {
    HANDLE_CODE(nof_rx_red_cap_r17.unpack(bref));
  }
  if (half_duplex_fdd_type_a_red_cap_r17_present) {
    HANDLE_CODE(
        unpack_dyn_seq_of(half_duplex_fdd_type_a_red_cap_r17, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_v1700_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (ue_radio_paging_info_r17.size() > 0) {
    j.write_str("ue-RadioPagingInfo-r17", ue_radio_paging_info_r17.to_string());
  }
  if (inactive_state_po_determination_r17_present) {
    j.write_str("inactiveStatePO-Determination-r17", "supported");
  }
  if (nof_rx_red_cap_r17_present) {
    j.write_str("numberOfRxRedCap-r17", nof_rx_red_cap_r17.to_string());
  }
  if (half_duplex_fdd_type_a_red_cap_r17.size() > 0) {
    j.start_array("halfDuplexFDD-TypeA-RedCap-r17");
    for (const auto& e1 : half_duplex_fdd_type_a_red_cap_r17) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_opts::to_string() const
{
  static const char* names[] = {"one", "two"};
  return convert_enum_idx(names, 2, value, "ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_e_");
}
uint8_t ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_e_");
}

// UERadioPagingInformation-v15e0-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_v15e0_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_fdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_tdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_a_tdd_fr2_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_fdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_tdd_fr1_present, 1));
  HANDLE_CODE(bref.pack(dl_sched_offset_pdsch_type_b_tdd_fr2_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_v15e0_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_fdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_tdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_a_tdd_fr2_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_fdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_tdd_fr1_present, 1));
  HANDLE_CODE(bref.unpack(dl_sched_offset_pdsch_type_b_tdd_fr2_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_v15e0_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dl_sched_offset_pdsch_type_a_fdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeA-FDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_a_tdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeA-TDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_a_tdd_fr2_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeA-TDD-FR2", "supported");
  }
  if (dl_sched_offset_pdsch_type_b_fdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeB-FDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_b_tdd_fr1_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeB-TDD-FR1", "supported");
  }
  if (dl_sched_offset_pdsch_type_b_tdd_fr2_present) {
    j.write_str("dl-SchedulingOffset-PDSCH-TypeB-TDD-FR2", "supported");
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioPagingInformation-IEs ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(supported_band_list_nr_for_paging.size() > 0, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (supported_band_list_nr_for_paging.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_band_list_nr_for_paging, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_ies_s::unpack(cbit_ref& bref)
{
  bool supported_band_list_nr_for_paging_present;
  HANDLE_CODE(bref.unpack(supported_band_list_nr_for_paging_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (supported_band_list_nr_for_paging_present) {
    HANDLE_CODE(unpack_dyn_seq_of(supported_band_list_nr_for_paging, bref, 1, 1024, integer_packer<uint16_t>(1, 1024)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (supported_band_list_nr_for_paging.size() > 0) {
    j.start_array("supportedBandListNRForPaging");
    for (const auto& e1 : supported_band_list_nr_for_paging) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// UERadioPagingInformation ::= SEQUENCE
SRSASN_CODE ue_radio_paging_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_radio_paging_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void ue_radio_paging_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_s::crit_exts_c_::c1_c_& ue_radio_paging_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void ue_radio_paging_info_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_ies_s& ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_ue_radio_paging_info()
{
  set(types::ue_radio_paging_info);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ue_radio_paging_info:
      j.write_fieldname("ueRadioPagingInformation");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ue_radio_paging_info:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_radio_paging_info_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ue_radio_paging_info:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ue_radio_paging_info_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ue_radio_paging_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* names[] = {
      "ueRadioPagingInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ue_radio_paging_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_paging_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* names[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(names, 2, value, "ue_radio_paging_info_s::crit_exts_c_::types");
}
uint8_t ue_radio_paging_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_radio_paging_info_s::crit_exts_c_::types");
}
