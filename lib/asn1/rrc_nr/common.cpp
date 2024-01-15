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

#include "srsran/asn1/rrc_nr/common.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SubcarrierSpacing ::= ENUMERATED
const char* subcarrier_spacing_opts::to_string() const
{
  static const char* names[] = {
      "kHz15", "kHz30", "kHz60", "kHz120", "kHz240", "kHz480-v1700", "kHz960-v1700", "spare1"};
  return convert_enum_idx(names, 8, value, "subcarrier_spacing_e");
}
uint16_t subcarrier_spacing_opts::to_number() const
{
  static const uint16_t numbers[] = {15, 30, 60, 120, 240, 480, 960};
  return map_enum_number(numbers, 7, value, "subcarrier_spacing_e");
}

// PLMN-Identity ::= SEQUENCE
SRSASN_CODE plmn_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, mcc, mcc.size(), integer_packer<uint8_t>(0, 9)));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, mnc, 2, 3, integer_packer<uint8_t>(0, 9)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(unpack_fixed_seq_of(mcc, bref, mcc.size(), integer_packer<uint8_t>(0, 9)));
  }
  HANDLE_CODE(unpack_dyn_seq_of(mnc, bref, 2, 3, integer_packer<uint8_t>(0, 9)));

  return SRSASN_SUCCESS;
}
void plmn_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mcc_present) {
    j.start_array("mcc");
    for (const auto& e1 : mcc) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.start_array("mnc");
  for (const auto& e1 : mnc) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// SRS-PeriodicityAndOffset ::= CHOICE
void srs_periodicity_and_offset_c::destroy_() {}
void srs_periodicity_and_offset_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
srs_periodicity_and_offset_c::srs_periodicity_and_offset_c(const srs_periodicity_and_offset_c& other)
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
    case types::sl32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl64:
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
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_c");
  }
}
srs_periodicity_and_offset_c& srs_periodicity_and_offset_c::operator=(const srs_periodicity_and_offset_c& other)
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
    case types::sl32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl64:
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
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_c");
  }

  return *this;
}
void srs_periodicity_and_offset_c::set_sl1()
{
  set(types::sl1);
}
uint8_t& srs_periodicity_and_offset_c::set_sl2()
{
  set(types::sl2);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl4()
{
  set(types::sl4);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl5()
{
  set(types::sl5);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl8()
{
  set(types::sl8);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl10()
{
  set(types::sl10);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl16()
{
  set(types::sl16);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl20()
{
  set(types::sl20);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl32()
{
  set(types::sl32);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl40()
{
  set(types::sl40);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl64()
{
  set(types::sl64);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl80()
{
  set(types::sl80);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl160()
{
  set(types::sl160);
  return c.get<uint8_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl320()
{
  set(types::sl320);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl640()
{
  set(types::sl640);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl1280()
{
  set(types::sl1280);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl2560()
{
  set(types::sl2560);
  return c.get<uint16_t>();
}
void srs_periodicity_and_offset_c::to_json(json_writer& j) const
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
    case types::sl32:
      j.write_int("sl32", c.get<uint8_t>());
      break;
    case types::sl40:
      j.write_int("sl40", c.get<uint8_t>());
      break;
    case types::sl64:
      j.write_int("sl64", c.get<uint8_t>());
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
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_periodicity_and_offset_c::pack(bit_ref& bref) const
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
    case types::sl32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sl40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
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
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_periodicity_and_offset_c::unpack(cbit_ref& bref)
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
    case types::sl32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sl40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
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
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRS-SpatialRelationInfo ::= SEQUENCE
SRSASN_CODE srs_spatial_relation_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(serving_cell_id_present, 1));

  if (serving_cell_id_present) {
    HANDLE_CODE(pack_integer(bref, serving_cell_id, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(ref_sig.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_spatial_relation_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(serving_cell_id_present, 1));

  if (serving_cell_id_present) {
    HANDLE_CODE(unpack_integer(serving_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
  }
  HANDLE_CODE(ref_sig.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_spatial_relation_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (serving_cell_id_present) {
    j.write_int("servingCellId", serving_cell_id);
  }
  j.write_fieldname("referenceSignal");
  ref_sig.to_json(j);
  j.end_obj();
}

void srs_spatial_relation_info_s::ref_sig_c_::destroy_()
{
  switch (type_) {
    case types::srs:
      c.destroy<srs_s_>();
      break;
    default:
      break;
  }
}
void srs_spatial_relation_info_s::ref_sig_c_::set(types::options e)
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
      log_invalid_choice_id(type_, "srs_spatial_relation_info_s::ref_sig_c_");
  }
}
srs_spatial_relation_info_s::ref_sig_c_::ref_sig_c_(const srs_spatial_relation_info_s::ref_sig_c_& other)
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
      log_invalid_choice_id(type_, "srs_spatial_relation_info_s::ref_sig_c_");
  }
}
srs_spatial_relation_info_s::ref_sig_c_&
srs_spatial_relation_info_s::ref_sig_c_::operator=(const srs_spatial_relation_info_s::ref_sig_c_& other)
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
      log_invalid_choice_id(type_, "srs_spatial_relation_info_s::ref_sig_c_");
  }

  return *this;
}
uint8_t& srs_spatial_relation_info_s::ref_sig_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& srs_spatial_relation_info_s::ref_sig_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
srs_spatial_relation_info_s::ref_sig_c_::srs_s_& srs_spatial_relation_info_s::ref_sig_c_::set_srs()
{
  set(types::srs);
  return c.get<srs_s_>();
}
void srs_spatial_relation_info_s::ref_sig_c_::to_json(json_writer& j) const
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
      j.write_int("resourceId", c.get<srs_s_>().res_id);
      j.write_int("uplinkBWP", c.get<srs_s_>().ul_bwp);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_s::ref_sig_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_spatial_relation_info_s::ref_sig_c_::pack(bit_ref& bref) const
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
      HANDLE_CODE(pack_integer(bref, c.get<srs_s_>().res_id, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(pack_integer(bref, c.get<srs_s_>().ul_bwp, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_s::ref_sig_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_spatial_relation_info_s::ref_sig_c_::unpack(cbit_ref& bref)
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
      HANDLE_CODE(unpack_integer(c.get<srs_s_>().res_id, bref, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(unpack_integer(c.get<srs_s_>().ul_bwp, bref, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_s::ref_sig_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ServingCellAndBWP-Id-r17 ::= SEQUENCE
SRSASN_CODE serving_cell_and_bwp_id_r17_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, servingcell_r17, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(pack_integer(bref, bwp_r17, (uint8_t)0u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_and_bwp_id_r17_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(servingcell_r17, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(unpack_integer(bwp_r17, bref, (uint8_t)0u, (uint8_t)4u));

  return SRSASN_SUCCESS;
}
void serving_cell_and_bwp_id_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servingcell-r17", servingcell_r17);
  j.write_int("bwp-r17", bwp_r17);
  j.end_obj();
}

// SpatialRelationInfo-PDC-r17 ::= SEQUENCE
SRSASN_CODE spatial_relation_info_pdc_r17_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ref_sig.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE spatial_relation_info_pdc_r17_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ref_sig.unpack(bref));

  return SRSASN_SUCCESS;
}
void spatial_relation_info_pdc_r17_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("referenceSignal");
  ref_sig.to_json(j);
  j.end_obj();
}

void spatial_relation_info_pdc_r17_s::ref_sig_c_::destroy_()
{
  switch (type_) {
    case types::srs:
      c.destroy<srs_s_>();
      break;
    default:
      break;
  }
}
void spatial_relation_info_pdc_r17_s::ref_sig_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ssb_idx:
      break;
    case types::csi_rs_idx:
      break;
    case types::dl_prs_pdc:
      break;
    case types::srs:
      c.init<srs_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "spatial_relation_info_pdc_r17_s::ref_sig_c_");
  }
}
spatial_relation_info_pdc_r17_s::ref_sig_c_::ref_sig_c_(const spatial_relation_info_pdc_r17_s::ref_sig_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::dl_prs_pdc:
      c.init(other.c.get<uint8_t>());
      break;
    case types::srs:
      c.init(other.c.get<srs_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "spatial_relation_info_pdc_r17_s::ref_sig_c_");
  }
}
spatial_relation_info_pdc_r17_s::ref_sig_c_&
spatial_relation_info_pdc_r17_s::ref_sig_c_::operator=(const spatial_relation_info_pdc_r17_s::ref_sig_c_& other)
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
    case types::dl_prs_pdc:
      c.set(other.c.get<uint8_t>());
      break;
    case types::srs:
      c.set(other.c.get<srs_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "spatial_relation_info_pdc_r17_s::ref_sig_c_");
  }

  return *this;
}
uint8_t& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
uint8_t& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_dl_prs_pdc()
{
  set(types::dl_prs_pdc);
  return c.get<uint8_t>();
}
spatial_relation_info_pdc_r17_s::ref_sig_c_::srs_s_& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_srs()
{
  set(types::srs);
  return c.get<srs_s_>();
}
void spatial_relation_info_pdc_r17_s::ref_sig_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_idx:
      j.write_int("ssb-Index", c.get<uint8_t>());
      break;
    case types::csi_rs_idx:
      j.write_int("csi-RS-Index", c.get<uint8_t>());
      break;
    case types::dl_prs_pdc:
      j.write_int("dl-PRS-PDC", c.get<uint8_t>());
      break;
    case types::srs:
      j.write_fieldname("srs");
      j.start_obj();
      j.write_int("resourceId", c.get<srs_s_>().res_id);
      j.write_int("uplinkBWP", c.get<srs_s_>().ul_bwp);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "spatial_relation_info_pdc_r17_s::ref_sig_c_");
  }
  j.end_obj();
}
SRSASN_CODE spatial_relation_info_pdc_r17_s::ref_sig_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    case types::dl_prs_pdc:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::srs:
      HANDLE_CODE(pack_integer(bref, c.get<srs_s_>().res_id, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(pack_integer(bref, c.get<srs_s_>().ul_bwp, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "spatial_relation_info_pdc_r17_s::ref_sig_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE spatial_relation_info_pdc_r17_s::ref_sig_c_::unpack(cbit_ref& bref)
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
    case types::dl_prs_pdc:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::srs:
      HANDLE_CODE(unpack_integer(c.get<srs_s_>().res_id, bref, (uint8_t)0u, (uint8_t)63u));
      HANDLE_CODE(unpack_integer(c.get<srs_s_>().ul_bwp, bref, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "spatial_relation_info_pdc_r17_s::ref_sig_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRS-Resource ::= SEQUENCE
SRSASN_CODE srs_res_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ptrs_port_idx_present, 1));
  HANDLE_CODE(bref.pack(spatial_relation_info_present, 1));

  HANDLE_CODE(pack_integer(bref, srs_res_id, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(nrof_srs_ports.pack(bref));
  if (ptrs_port_idx_present) {
    HANDLE_CODE(ptrs_port_idx.pack(bref));
  }
  HANDLE_CODE(tx_comb.pack(bref));
  HANDLE_CODE(pack_integer(bref, res_map.start_position, (uint8_t)0u, (uint8_t)5u));
  HANDLE_CODE(res_map.nrof_symbols.pack(bref));
  HANDLE_CODE(res_map.repeat_factor.pack(bref));
  HANDLE_CODE(pack_integer(bref, freq_domain_position, (uint8_t)0u, (uint8_t)67u));
  HANDLE_CODE(pack_integer(bref, freq_domain_shift, (uint16_t)0u, (uint16_t)268u));
  HANDLE_CODE(pack_integer(bref, freq_hop.c_srs, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, freq_hop.b_srs, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, freq_hop.b_hop, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(group_or_seq_hop.pack(bref));
  HANDLE_CODE(res_type.pack(bref));
  HANDLE_CODE(pack_integer(bref, seq_id, (uint16_t)0u, (uint16_t)1023u));
  if (spatial_relation_info_present) {
    HANDLE_CODE(spatial_relation_info.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= res_map_r16.is_present();
    group_flags[1] |= spatial_relation_info_pdc_r17.is_present();
    group_flags[1] |= res_map_r17.is_present();
    group_flags[1] |= partial_freq_sr17.is_present();
    group_flags[1] |= tx_comb_n8_r17.is_present();
    group_flags[1] |= srs_tci_state_r17.is_present();
    group_flags[2] |= repeat_factor_v1730_present;
    group_flags[2] |= srs_dl_or_joint_tci_state_v1730.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(res_map_r16.is_present(), 1));
      if (res_map_r16.is_present()) {
        HANDLE_CODE(pack_integer(bref, res_map_r16->start_position_r16, (uint8_t)0u, (uint8_t)13u));
        HANDLE_CODE(res_map_r16->nrof_symbols_r16.pack(bref));
        HANDLE_CODE(res_map_r16->repeat_factor_r16.pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(spatial_relation_info_pdc_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(res_map_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(partial_freq_sr17.is_present(), 1));
      HANDLE_CODE(bref.pack(tx_comb_n8_r17.is_present(), 1));
      HANDLE_CODE(bref.pack(srs_tci_state_r17.is_present(), 1));
      if (spatial_relation_info_pdc_r17.is_present()) {
        HANDLE_CODE(spatial_relation_info_pdc_r17->pack(bref));
      }
      if (res_map_r17.is_present()) {
        HANDLE_CODE(pack_integer(bref, res_map_r17->start_position_r17, (uint8_t)0u, (uint8_t)13u));
        HANDLE_CODE(res_map_r17->nrof_symbols_r17.pack(bref));
        HANDLE_CODE(res_map_r17->repeat_factor_r17.pack(bref));
      }
      if (partial_freq_sr17.is_present()) {
        HANDLE_CODE(bref.pack(partial_freq_sr17->enable_start_rb_hop_r17_present, 1));
        HANDLE_CODE(partial_freq_sr17->start_rb_idx_f_scaling_r17.pack(bref));
      }
      if (tx_comb_n8_r17.is_present()) {
        HANDLE_CODE(pack_integer(bref, tx_comb_n8_r17->comb_offset_n8_r17, (uint8_t)0u, (uint8_t)7u));
        HANDLE_CODE(pack_integer(bref, tx_comb_n8_r17->cyclic_shift_n8_r17, (uint8_t)0u, (uint8_t)5u));
      }
      if (srs_tci_state_r17.is_present()) {
        HANDLE_CODE(srs_tci_state_r17->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(repeat_factor_v1730_present, 1));
      HANDLE_CODE(bref.pack(srs_dl_or_joint_tci_state_v1730.is_present(), 1));
      if (srs_dl_or_joint_tci_state_v1730.is_present()) {
        HANDLE_CODE(srs_dl_or_joint_tci_state_v1730->cell_and_bwp_r17.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ptrs_port_idx_present, 1));
  HANDLE_CODE(bref.unpack(spatial_relation_info_present, 1));

  HANDLE_CODE(unpack_integer(srs_res_id, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(nrof_srs_ports.unpack(bref));
  if (ptrs_port_idx_present) {
    HANDLE_CODE(ptrs_port_idx.unpack(bref));
  }
  HANDLE_CODE(tx_comb.unpack(bref));
  HANDLE_CODE(unpack_integer(res_map.start_position, bref, (uint8_t)0u, (uint8_t)5u));
  HANDLE_CODE(res_map.nrof_symbols.unpack(bref));
  HANDLE_CODE(res_map.repeat_factor.unpack(bref));
  HANDLE_CODE(unpack_integer(freq_domain_position, bref, (uint8_t)0u, (uint8_t)67u));
  HANDLE_CODE(unpack_integer(freq_domain_shift, bref, (uint16_t)0u, (uint16_t)268u));
  HANDLE_CODE(unpack_integer(freq_hop.c_srs, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(freq_hop.b_srs, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(freq_hop.b_hop, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(group_or_seq_hop.unpack(bref));
  HANDLE_CODE(res_type.unpack(bref));
  HANDLE_CODE(unpack_integer(seq_id, bref, (uint16_t)0u, (uint16_t)1023u));
  if (spatial_relation_info_present) {
    HANDLE_CODE(spatial_relation_info.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(3);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(res_map_r16, bref);
      if (res_map_r16.is_present()) {
        HANDLE_CODE(unpack_integer(res_map_r16->start_position_r16, bref, (uint8_t)0u, (uint8_t)13u));
        HANDLE_CODE(res_map_r16->nrof_symbols_r16.unpack(bref));
        HANDLE_CODE(res_map_r16->repeat_factor_r16.unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(spatial_relation_info_pdc_r17, bref);
      unpack_presence_flag(res_map_r17, bref);
      unpack_presence_flag(partial_freq_sr17, bref);
      unpack_presence_flag(tx_comb_n8_r17, bref);
      unpack_presence_flag(srs_tci_state_r17, bref);
      if (spatial_relation_info_pdc_r17.is_present()) {
        HANDLE_CODE(spatial_relation_info_pdc_r17->unpack(bref));
      }
      if (res_map_r17.is_present()) {
        HANDLE_CODE(unpack_integer(res_map_r17->start_position_r17, bref, (uint8_t)0u, (uint8_t)13u));
        HANDLE_CODE(res_map_r17->nrof_symbols_r17.unpack(bref));
        HANDLE_CODE(res_map_r17->repeat_factor_r17.unpack(bref));
      }
      if (partial_freq_sr17.is_present()) {
        HANDLE_CODE(bref.unpack(partial_freq_sr17->enable_start_rb_hop_r17_present, 1));
        HANDLE_CODE(partial_freq_sr17->start_rb_idx_f_scaling_r17.unpack(bref));
      }
      if (tx_comb_n8_r17.is_present()) {
        HANDLE_CODE(unpack_integer(tx_comb_n8_r17->comb_offset_n8_r17, bref, (uint8_t)0u, (uint8_t)7u));
        HANDLE_CODE(unpack_integer(tx_comb_n8_r17->cyclic_shift_n8_r17, bref, (uint8_t)0u, (uint8_t)5u));
      }
      if (srs_tci_state_r17.is_present()) {
        HANDLE_CODE(srs_tci_state_r17->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(repeat_factor_v1730_present, 1));
      unpack_presence_flag(srs_dl_or_joint_tci_state_v1730, bref);
      if (srs_dl_or_joint_tci_state_v1730.is_present()) {
        HANDLE_CODE(srs_dl_or_joint_tci_state_v1730->cell_and_bwp_r17.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srs_res_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srs-ResourceId", srs_res_id);
  j.write_str("nrofSRS-Ports", nrof_srs_ports.to_string());
  if (ptrs_port_idx_present) {
    j.write_str("ptrs-PortIndex", ptrs_port_idx.to_string());
  }
  j.write_fieldname("transmissionComb");
  tx_comb.to_json(j);
  j.write_fieldname("resourceMapping");
  j.start_obj();
  j.write_int("startPosition", res_map.start_position);
  j.write_str("nrofSymbols", res_map.nrof_symbols.to_string());
  j.write_str("repetitionFactor", res_map.repeat_factor.to_string());
  j.end_obj();
  j.write_int("freqDomainPosition", freq_domain_position);
  j.write_int("freqDomainShift", freq_domain_shift);
  j.write_fieldname("freqHopping");
  j.start_obj();
  j.write_int("c-SRS", freq_hop.c_srs);
  j.write_int("b-SRS", freq_hop.b_srs);
  j.write_int("b-hop", freq_hop.b_hop);
  j.end_obj();
  j.write_str("groupOrSequenceHopping", group_or_seq_hop.to_string());
  j.write_fieldname("resourceType");
  res_type.to_json(j);
  j.write_int("sequenceId", seq_id);
  if (spatial_relation_info_present) {
    j.write_fieldname("spatialRelationInfo");
    spatial_relation_info.to_json(j);
  }
  if (ext) {
    if (res_map_r16.is_present()) {
      j.write_fieldname("resourceMapping-r16");
      j.start_obj();
      j.write_int("startPosition-r16", res_map_r16->start_position_r16);
      j.write_str("nrofSymbols-r16", res_map_r16->nrof_symbols_r16.to_string());
      j.write_str("repetitionFactor-r16", res_map_r16->repeat_factor_r16.to_string());
      j.end_obj();
    }
    if (spatial_relation_info_pdc_r17.is_present()) {
      j.write_fieldname("spatialRelationInfo-PDC-r17");
      spatial_relation_info_pdc_r17->to_json(j);
    }
    if (res_map_r17.is_present()) {
      j.write_fieldname("resourceMapping-r17");
      j.start_obj();
      j.write_int("startPosition-r17", res_map_r17->start_position_r17);
      j.write_str("nrofSymbols-r17", res_map_r17->nrof_symbols_r17.to_string());
      j.write_str("repetitionFactor-r17", res_map_r17->repeat_factor_r17.to_string());
      j.end_obj();
    }
    if (partial_freq_sr17.is_present()) {
      j.write_fieldname("partialFreqSounding-r17");
      j.start_obj();
      j.write_fieldname("startRBIndexFScaling-r17");
      partial_freq_sr17->start_rb_idx_f_scaling_r17.to_json(j);
      if (partial_freq_sr17->enable_start_rb_hop_r17_present) {
        j.write_str("enableStartRBHopping-r17", "enable");
      }
      j.end_obj();
    }
    if (tx_comb_n8_r17.is_present()) {
      j.write_fieldname("transmissionComb-n8-r17");
      j.start_obj();
      j.write_int("combOffset-n8-r17", tx_comb_n8_r17->comb_offset_n8_r17);
      j.write_int("cyclicShift-n8-r17", tx_comb_n8_r17->cyclic_shift_n8_r17);
      j.end_obj();
    }
    if (srs_tci_state_r17.is_present()) {
      j.write_fieldname("srs-TCI-State-r17");
      srs_tci_state_r17->to_json(j);
    }
    if (repeat_factor_v1730_present) {
      j.write_str("repetitionFactor-v1730", "n3");
    }
    if (srs_dl_or_joint_tci_state_v1730.is_present()) {
      j.write_fieldname("srs-DLorJointTCI-State-v1730");
      j.start_obj();
      j.write_fieldname("cellAndBWP-r17");
      srs_dl_or_joint_tci_state_v1730->cell_and_bwp_r17.to_json(j);
      j.end_obj();
    }
  }
  j.end_obj();
}

const char* srs_res_s::nrof_srs_ports_opts::to_string() const
{
  static const char* names[] = {"port1", "ports2", "ports4"};
  return convert_enum_idx(names, 3, value, "srs_res_s::nrof_srs_ports_e_");
}
uint8_t srs_res_s::nrof_srs_ports_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::nrof_srs_ports_e_");
}

const char* srs_res_s::ptrs_port_idx_opts::to_string() const
{
  static const char* names[] = {"n0", "n1"};
  return convert_enum_idx(names, 2, value, "srs_res_s::ptrs_port_idx_e_");
}
uint8_t srs_res_s::ptrs_port_idx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "srs_res_s::ptrs_port_idx_e_");
}

void srs_res_s::tx_comb_c_::destroy_()
{
  switch (type_) {
    case types::n2:
      c.destroy<n2_s_>();
      break;
    case types::n4:
      c.destroy<n4_s_>();
      break;
    default:
      break;
  }
}
void srs_res_s::tx_comb_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::n2:
      c.init<n2_s_>();
      break;
    case types::n4:
      c.init<n4_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::tx_comb_c_");
  }
}
srs_res_s::tx_comb_c_::tx_comb_c_(const srs_res_s::tx_comb_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n2:
      c.init(other.c.get<n2_s_>());
      break;
    case types::n4:
      c.init(other.c.get<n4_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::tx_comb_c_");
  }
}
srs_res_s::tx_comb_c_& srs_res_s::tx_comb_c_::operator=(const srs_res_s::tx_comb_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n2:
      c.set(other.c.get<n2_s_>());
      break;
    case types::n4:
      c.set(other.c.get<n4_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::tx_comb_c_");
  }

  return *this;
}
srs_res_s::tx_comb_c_::n2_s_& srs_res_s::tx_comb_c_::set_n2()
{
  set(types::n2);
  return c.get<n2_s_>();
}
srs_res_s::tx_comb_c_::n4_s_& srs_res_s::tx_comb_c_::set_n4()
{
  set(types::n4);
  return c.get<n4_s_>();
}
void srs_res_s::tx_comb_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n2:
      j.write_fieldname("n2");
      j.start_obj();
      j.write_int("combOffset-n2", c.get<n2_s_>().comb_offset_n2);
      j.write_int("cyclicShift-n2", c.get<n2_s_>().cyclic_shift_n2);
      j.end_obj();
      break;
    case types::n4:
      j.write_fieldname("n4");
      j.start_obj();
      j.write_int("combOffset-n4", c.get<n4_s_>().comb_offset_n4);
      j.write_int("cyclicShift-n4", c.get<n4_s_>().cyclic_shift_n4);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::tx_comb_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_res_s::tx_comb_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n2:
      HANDLE_CODE(pack_integer(bref, c.get<n2_s_>().comb_offset_n2, (uint8_t)0u, (uint8_t)1u));
      HANDLE_CODE(pack_integer(bref, c.get<n2_s_>().cyclic_shift_n2, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n4:
      HANDLE_CODE(pack_integer(bref, c.get<n4_s_>().comb_offset_n4, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(pack_integer(bref, c.get<n4_s_>().cyclic_shift_n4, (uint8_t)0u, (uint8_t)11u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::tx_comb_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_s::tx_comb_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n2:
      HANDLE_CODE(unpack_integer(c.get<n2_s_>().comb_offset_n2, bref, (uint8_t)0u, (uint8_t)1u));
      HANDLE_CODE(unpack_integer(c.get<n2_s_>().cyclic_shift_n2, bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n4:
      HANDLE_CODE(unpack_integer(c.get<n4_s_>().comb_offset_n4, bref, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(unpack_integer(c.get<n4_s_>().cyclic_shift_n4, bref, (uint8_t)0u, (uint8_t)11u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::tx_comb_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* srs_res_s::res_map_s_::nrof_symbols_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "srs_res_s::res_map_s_::nrof_symbols_e_");
}
uint8_t srs_res_s::res_map_s_::nrof_symbols_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_s_::nrof_symbols_e_");
}

const char* srs_res_s::res_map_s_::repeat_factor_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "srs_res_s::res_map_s_::repeat_factor_e_");
}
uint8_t srs_res_s::res_map_s_::repeat_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_s_::repeat_factor_e_");
}

const char* srs_res_s::group_or_seq_hop_opts::to_string() const
{
  static const char* names[] = {"neither", "groupHopping", "sequenceHopping"};
  return convert_enum_idx(names, 3, value, "srs_res_s::group_or_seq_hop_e_");
}

void srs_res_s::res_type_c_::destroy_()
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
void srs_res_s::res_type_c_::set(types::options e)
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
      log_invalid_choice_id(type_, "srs_res_s::res_type_c_");
  }
}
srs_res_s::res_type_c_::res_type_c_(const srs_res_s::res_type_c_& other)
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
      log_invalid_choice_id(type_, "srs_res_s::res_type_c_");
  }
}
srs_res_s::res_type_c_& srs_res_s::res_type_c_::operator=(const srs_res_s::res_type_c_& other)
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
      log_invalid_choice_id(type_, "srs_res_s::res_type_c_");
  }

  return *this;
}
srs_res_s::res_type_c_::aperiodic_s_& srs_res_s::res_type_c_::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<aperiodic_s_>();
}
srs_res_s::res_type_c_::semi_persistent_s_& srs_res_s::res_type_c_::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<semi_persistent_s_>();
}
srs_res_s::res_type_c_::periodic_s_& srs_res_s::res_type_c_::set_periodic()
{
  set(types::periodic);
  return c.get<periodic_s_>();
}
void srs_res_s::res_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::aperiodic:
      j.write_fieldname("aperiodic");
      j.start_obj();
      j.end_obj();
      break;
    case types::semi_persistent:
      j.write_fieldname("semi-persistent");
      j.start_obj();
      j.write_fieldname("periodicityAndOffset-sp");
      c.get<semi_persistent_s_>().periodicity_and_offset_sp.to_json(j);
      j.end_obj();
      break;
    case types::periodic:
      j.write_fieldname("periodic");
      j.start_obj();
      j.write_fieldname("periodicityAndOffset-p");
      c.get<periodic_s_>().periodicity_and_offset_p.to_json(j);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::res_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_res_s::res_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::aperiodic:
      bref.pack(c.get<aperiodic_s_>().ext, 1);
      break;
    case types::semi_persistent:
      bref.pack(c.get<semi_persistent_s_>().ext, 1);
      HANDLE_CODE(c.get<semi_persistent_s_>().periodicity_and_offset_sp.pack(bref));
      break;
    case types::periodic:
      bref.pack(c.get<periodic_s_>().ext, 1);
      HANDLE_CODE(c.get<periodic_s_>().periodicity_and_offset_p.pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::res_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_s::res_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::aperiodic:
      bref.unpack(c.get<aperiodic_s_>().ext, 1);
      break;
    case types::semi_persistent:
      bref.unpack(c.get<semi_persistent_s_>().ext, 1);
      HANDLE_CODE(c.get<semi_persistent_s_>().periodicity_and_offset_sp.unpack(bref));
      break;
    case types::periodic:
      bref.unpack(c.get<periodic_s_>().ext, 1);
      HANDLE_CODE(c.get<periodic_s_>().periodicity_and_offset_p.unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::res_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* srs_res_s::res_map_r16_s_::nrof_symbols_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "srs_res_s::res_map_r16_s_::nrof_symbols_r16_e_");
}
uint8_t srs_res_s::res_map_r16_s_::nrof_symbols_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_r16_s_::nrof_symbols_r16_e_");
}

const char* srs_res_s::res_map_r16_s_::repeat_factor_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4"};
  return convert_enum_idx(names, 3, value, "srs_res_s::res_map_r16_s_::repeat_factor_r16_e_");
}
uint8_t srs_res_s::res_map_r16_s_::repeat_factor_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_r16_s_::repeat_factor_r16_e_");
}

const char* srs_res_s::res_map_r17_s_::nrof_symbols_r17_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n10", "n12", "n14"};
  return convert_enum_idx(names, 7, value, "srs_res_s::res_map_r17_s_::nrof_symbols_r17_e_");
}
uint8_t srs_res_s::res_map_r17_s_::nrof_symbols_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 10, 12, 14};
  return map_enum_number(numbers, 7, value, "srs_res_s::res_map_r17_s_::nrof_symbols_r17_e_");
}

const char* srs_res_s::res_map_r17_s_::repeat_factor_r17_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n5", "n6", "n7", "n8", "n10", "n12", "n14"};
  return convert_enum_idx(names, 10, value, "srs_res_s::res_map_r17_s_::repeat_factor_r17_e_");
}
uint8_t srs_res_s::res_map_r17_s_::repeat_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 5, 6, 7, 8, 10, 12, 14};
  return map_enum_number(numbers, 10, value, "srs_res_s::res_map_r17_s_::repeat_factor_r17_e_");
}

void srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::destroy_() {}
void srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::start_rb_idx_f_scaling_r17_c_(
    const srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::start_rb_idx_and_freq_scaling_factor2_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::start_rb_idx_and_freq_scaling_factor4_r17:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_");
  }
}
srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_&
srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::operator=(
    const srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::start_rb_idx_and_freq_scaling_factor2_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::start_rb_idx_and_freq_scaling_factor4_r17:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_");
  }

  return *this;
}
uint8_t& srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::set_start_rb_idx_and_freq_scaling_factor2_r17()
{
  set(types::start_rb_idx_and_freq_scaling_factor2_r17);
  return c.get<uint8_t>();
}
uint8_t& srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::set_start_rb_idx_and_freq_scaling_factor4_r17()
{
  set(types::start_rb_idx_and_freq_scaling_factor4_r17);
  return c.get<uint8_t>();
}
void srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::start_rb_idx_and_freq_scaling_factor2_r17:
      j.write_int("startRBIndexAndFreqScalingFactor2-r17", c.get<uint8_t>());
      break;
    case types::start_rb_idx_and_freq_scaling_factor4_r17:
      j.write_int("startRBIndexAndFreqScalingFactor4-r17", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::start_rb_idx_and_freq_scaling_factor2_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)1u));
      break;
    case types::start_rb_idx_and_freq_scaling_factor4_r17:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::start_rb_idx_and_freq_scaling_factor2_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)1u));
      break;
    case types::start_rb_idx_and_freq_scaling_factor4_r17:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void srs_res_s::srs_tci_state_r17_c_::destroy_() {}
void srs_res_s::srs_tci_state_r17_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
srs_res_s::srs_tci_state_r17_c_::srs_tci_state_r17_c_(const srs_res_s::srs_tci_state_r17_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::srs_ul_tci_state:
      c.init(other.c.get<uint8_t>());
      break;
    case types::srs_dl_or_joint_tci_state:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::srs_tci_state_r17_c_");
  }
}
srs_res_s::srs_tci_state_r17_c_&
srs_res_s::srs_tci_state_r17_c_::operator=(const srs_res_s::srs_tci_state_r17_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::srs_ul_tci_state:
      c.set(other.c.get<uint8_t>());
      break;
    case types::srs_dl_or_joint_tci_state:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::srs_tci_state_r17_c_");
  }

  return *this;
}
uint8_t& srs_res_s::srs_tci_state_r17_c_::set_srs_ul_tci_state()
{
  set(types::srs_ul_tci_state);
  return c.get<uint8_t>();
}
uint8_t& srs_res_s::srs_tci_state_r17_c_::set_srs_dl_or_joint_tci_state()
{
  set(types::srs_dl_or_joint_tci_state);
  return c.get<uint8_t>();
}
void srs_res_s::srs_tci_state_r17_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::srs_ul_tci_state:
      j.write_int("srs-UL-TCI-State", c.get<uint8_t>());
      break;
    case types::srs_dl_or_joint_tci_state:
      j.write_int("srs-DLorJointTCI-State", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::srs_tci_state_r17_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_res_s::srs_tci_state_r17_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::srs_ul_tci_state:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::srs_dl_or_joint_tci_state:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::srs_tci_state_r17_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_res_s::srs_tci_state_r17_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::srs_ul_tci_state:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::srs_dl_or_joint_tci_state:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_res_s::srs_tci_state_r17_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SSB-Configuration-r16 ::= SEQUENCE
SRSASN_CODE ssb_cfg_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_periodicity_r16_present, 1));
  HANDLE_CODE(bref.pack(sfn0_offset_r16_present, 1));
  HANDLE_CODE(bref.pack(ss_pbch_block_pwr_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, ssb_freq_r16, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(half_frame_idx_r16.pack(bref));
  HANDLE_CODE(ssb_subcarrier_spacing_r16.pack(bref));
  if (ssb_periodicity_r16_present) {
    HANDLE_CODE(ssb_periodicity_r16.pack(bref));
  }
  if (sfn0_offset_r16_present) {
    HANDLE_CODE(bref.pack(sfn0_offset_r16.integer_sf_offset_r16_present, 1));
    HANDLE_CODE(pack_integer(bref, sfn0_offset_r16.sfn_offset_r16, (uint16_t)0u, (uint16_t)1023u));
    if (sfn0_offset_r16.integer_sf_offset_r16_present) {
      HANDLE_CODE(pack_integer(bref, sfn0_offset_r16.integer_sf_offset_r16, (uint8_t)0u, (uint8_t)9u));
    }
  }
  HANDLE_CODE(pack_integer(bref, sfn_ssb_offset_r16, (uint8_t)0u, (uint8_t)15u));
  if (ss_pbch_block_pwr_r16_present) {
    HANDLE_CODE(pack_integer(bref, ss_pbch_block_pwr_r16, (int8_t)-60, (int8_t)50));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_cfg_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_periodicity_r16_present, 1));
  HANDLE_CODE(bref.unpack(sfn0_offset_r16_present, 1));
  HANDLE_CODE(bref.unpack(ss_pbch_block_pwr_r16_present, 1));

  HANDLE_CODE(unpack_integer(ssb_freq_r16, bref, (uint32_t)0u, (uint32_t)3279165u));
  HANDLE_CODE(half_frame_idx_r16.unpack(bref));
  HANDLE_CODE(ssb_subcarrier_spacing_r16.unpack(bref));
  if (ssb_periodicity_r16_present) {
    HANDLE_CODE(ssb_periodicity_r16.unpack(bref));
  }
  if (sfn0_offset_r16_present) {
    HANDLE_CODE(bref.unpack(sfn0_offset_r16.integer_sf_offset_r16_present, 1));
    HANDLE_CODE(unpack_integer(sfn0_offset_r16.sfn_offset_r16, bref, (uint16_t)0u, (uint16_t)1023u));
    if (sfn0_offset_r16.integer_sf_offset_r16_present) {
      HANDLE_CODE(unpack_integer(sfn0_offset_r16.integer_sf_offset_r16, bref, (uint8_t)0u, (uint8_t)9u));
    }
  }
  HANDLE_CODE(unpack_integer(sfn_ssb_offset_r16, bref, (uint8_t)0u, (uint8_t)15u));
  if (ss_pbch_block_pwr_r16_present) {
    HANDLE_CODE(unpack_integer(ss_pbch_block_pwr_r16, bref, (int8_t)-60, (int8_t)50));
  }

  return SRSASN_SUCCESS;
}
void ssb_cfg_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ssb-Freq-r16", ssb_freq_r16);
  j.write_str("halfFrameIndex-r16", half_frame_idx_r16.to_string());
  j.write_str("ssbSubcarrierSpacing-r16", ssb_subcarrier_spacing_r16.to_string());
  if (ssb_periodicity_r16_present) {
    j.write_str("ssb-Periodicity-r16", ssb_periodicity_r16.to_string());
  }
  if (sfn0_offset_r16_present) {
    j.write_fieldname("sfn0-Offset-r16");
    j.start_obj();
    j.write_int("sfn-Offset-r16", sfn0_offset_r16.sfn_offset_r16);
    if (sfn0_offset_r16.integer_sf_offset_r16_present) {
      j.write_int("integerSubframeOffset-r16", sfn0_offset_r16.integer_sf_offset_r16);
    }
    j.end_obj();
  }
  j.write_int("sfn-SSB-Offset-r16", sfn_ssb_offset_r16);
  if (ss_pbch_block_pwr_r16_present) {
    j.write_int("ss-PBCH-BlockPower-r16", ss_pbch_block_pwr_r16);
  }
  j.end_obj();
}

const char* ssb_cfg_r16_s::half_frame_idx_r16_opts::to_string() const
{
  static const char* names[] = {"zero", "one"};
  return convert_enum_idx(names, 2, value, "ssb_cfg_r16_s::half_frame_idx_r16_e_");
}
uint8_t ssb_cfg_r16_s::half_frame_idx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "ssb_cfg_r16_s::half_frame_idx_r16_e_");
}

const char* ssb_cfg_r16_s::ssb_periodicity_r16_opts::to_string() const
{
  static const char* names[] = {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160", "spare2", "spare1"};
  return convert_enum_idx(names, 8, value, "ssb_cfg_r16_s::ssb_periodicity_r16_e_");
}
uint8_t ssb_cfg_r16_s::ssb_periodicity_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "ssb_cfg_r16_s::ssb_periodicity_r16_e_");
}

// DL-PRS-Info-r16 ::= SEQUENCE
SRSASN_CODE dl_prs_info_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dl_prs_res_id_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, dl_prs_id_r16, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(pack_integer(bref, dl_prs_res_set_id_r16, (uint8_t)0u, (uint8_t)7u));
  if (dl_prs_res_id_r16_present) {
    HANDLE_CODE(pack_integer(bref, dl_prs_res_id_r16, (uint8_t)0u, (uint8_t)63u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_prs_info_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dl_prs_res_id_r16_present, 1));

  HANDLE_CODE(unpack_integer(dl_prs_id_r16, bref, (uint16_t)0u, (uint16_t)255u));
  HANDLE_CODE(unpack_integer(dl_prs_res_set_id_r16, bref, (uint8_t)0u, (uint8_t)7u));
  if (dl_prs_res_id_r16_present) {
    HANDLE_CODE(unpack_integer(dl_prs_res_id_r16, bref, (uint8_t)0u, (uint8_t)63u));
  }

  return SRSASN_SUCCESS;
}
void dl_prs_info_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("dl-PRS-ID-r16", dl_prs_id_r16);
  j.write_int("dl-PRS-ResourceSetId-r16", dl_prs_res_set_id_r16);
  if (dl_prs_res_id_r16_present) {
    j.write_int("dl-PRS-ResourceId-r16", dl_prs_res_id_r16);
  }
  j.end_obj();
}

// SSB-InfoNcell-r16 ::= SEQUENCE
SRSASN_CODE ssb_info_ncell_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ssb_idx_ncell_r16_present, 1));
  HANDLE_CODE(bref.pack(ssb_cfg_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, phys_cell_id_r16, (uint16_t)0u, (uint16_t)1007u));
  if (ssb_idx_ncell_r16_present) {
    HANDLE_CODE(pack_integer(bref, ssb_idx_ncell_r16, (uint8_t)0u, (uint8_t)63u));
  }
  if (ssb_cfg_r16_present) {
    HANDLE_CODE(ssb_cfg_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ssb_info_ncell_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ssb_idx_ncell_r16_present, 1));
  HANDLE_CODE(bref.unpack(ssb_cfg_r16_present, 1));

  HANDLE_CODE(unpack_integer(phys_cell_id_r16, bref, (uint16_t)0u, (uint16_t)1007u));
  if (ssb_idx_ncell_r16_present) {
    HANDLE_CODE(unpack_integer(ssb_idx_ncell_r16, bref, (uint8_t)0u, (uint8_t)63u));
  }
  if (ssb_cfg_r16_present) {
    HANDLE_CODE(ssb_cfg_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ssb_info_ncell_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physicalCellId-r16", phys_cell_id_r16);
  if (ssb_idx_ncell_r16_present) {
    j.write_int("ssb-IndexNcell-r16", ssb_idx_ncell_r16);
  }
  if (ssb_cfg_r16_present) {
    j.write_fieldname("ssb-Configuration-r16");
    ssb_cfg_r16.to_json(j);
  }
  j.end_obj();
}

// SRS-PeriodicityAndOffset-r16 ::= CHOICE
void srs_periodicity_and_offset_r16_c::destroy_() {}
void srs_periodicity_and_offset_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
srs_periodicity_and_offset_r16_c::srs_periodicity_and_offset_r16_c(const srs_periodicity_and_offset_r16_c& other)
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
    case types::sl32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl64:
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
    case types::sl5120:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl10240:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl40960:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl81920:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_r16_c");
  }
}
srs_periodicity_and_offset_r16_c&
srs_periodicity_and_offset_r16_c::operator=(const srs_periodicity_and_offset_r16_c& other)
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
    case types::sl32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl64:
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
    case types::sl5120:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl10240:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl40960:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl81920:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_r16_c");
  }

  return *this;
}
void srs_periodicity_and_offset_r16_c::set_sl1()
{
  set(types::sl1);
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl2()
{
  set(types::sl2);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl4()
{
  set(types::sl4);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl5()
{
  set(types::sl5);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl8()
{
  set(types::sl8);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl10()
{
  set(types::sl10);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl16()
{
  set(types::sl16);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl20()
{
  set(types::sl20);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl32()
{
  set(types::sl32);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl40()
{
  set(types::sl40);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl64()
{
  set(types::sl64);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl80()
{
  set(types::sl80);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_r16_c::set_sl160()
{
  set(types::sl160);
  return c.get<uint8_t>();
}
uint16_t& srs_periodicity_and_offset_r16_c::set_sl320()
{
  set(types::sl320);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_r16_c::set_sl640()
{
  set(types::sl640);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_r16_c::set_sl1280()
{
  set(types::sl1280);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_r16_c::set_sl2560()
{
  set(types::sl2560);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_r16_c::set_sl5120()
{
  set(types::sl5120);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_r16_c::set_sl10240()
{
  set(types::sl10240);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_r16_c::set_sl40960()
{
  set(types::sl40960);
  return c.get<uint16_t>();
}
uint32_t& srs_periodicity_and_offset_r16_c::set_sl81920()
{
  set(types::sl81920);
  return c.get<uint32_t>();
}
void srs_periodicity_and_offset_r16_c::to_json(json_writer& j) const
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
    case types::sl32:
      j.write_int("sl32", c.get<uint8_t>());
      break;
    case types::sl40:
      j.write_int("sl40", c.get<uint8_t>());
      break;
    case types::sl64:
      j.write_int("sl64", c.get<uint8_t>());
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
    case types::sl5120:
      j.write_int("sl5120", c.get<uint16_t>());
      break;
    case types::sl10240:
      j.write_int("sl10240", c.get<uint16_t>());
      break;
    case types::sl40960:
      j.write_int("sl40960", c.get<uint16_t>());
      break;
    case types::sl81920:
      j.write_int("sl81920", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_periodicity_and_offset_r16_c::pack(bit_ref& bref) const
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
    case types::sl32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sl40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
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
    case types::sl5120:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::sl10240:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::sl40960:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)40959u));
      break;
    case types::sl81920:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)81919u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_periodicity_and_offset_r16_c::unpack(cbit_ref& bref)
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
    case types::sl32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sl40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sl64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
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
    case types::sl5120:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)5119u));
      break;
    case types::sl10240:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)10239u));
      break;
    case types::sl40960:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)40959u));
      break;
    case types::sl81920:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)81919u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRS-PeriodicityAndOffsetExt-r16 ::= CHOICE
void srs_periodicity_and_offset_ext_r16_c::destroy_() {}
void srs_periodicity_and_offset_ext_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
srs_periodicity_and_offset_ext_r16_c::srs_periodicity_and_offset_ext_r16_c(
    const srs_periodicity_and_offset_ext_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sl128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sl256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sl20480:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_ext_r16_c");
  }
}
srs_periodicity_and_offset_ext_r16_c&
srs_periodicity_and_offset_ext_r16_c::operator=(const srs_periodicity_and_offset_ext_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sl128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sl256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sl20480:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_ext_r16_c");
  }

  return *this;
}
uint8_t& srs_periodicity_and_offset_ext_r16_c::set_sl128()
{
  set(types::sl128);
  return c.get<uint8_t>();
}
uint16_t& srs_periodicity_and_offset_ext_r16_c::set_sl256()
{
  set(types::sl256);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_ext_r16_c::set_sl512()
{
  set(types::sl512);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_ext_r16_c::set_sl20480()
{
  set(types::sl20480);
  return c.get<uint16_t>();
}
void srs_periodicity_and_offset_ext_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sl128:
      j.write_int("sl128", c.get<uint8_t>());
      break;
    case types::sl256:
      j.write_int("sl256", c.get<uint16_t>());
      break;
    case types::sl512:
      j.write_int("sl512", c.get<uint16_t>());
      break;
    case types::sl20480:
      j.write_int("sl20480", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_ext_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_periodicity_and_offset_ext_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sl128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sl256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sl512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sl20480:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)20479u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_ext_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_periodicity_and_offset_ext_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sl128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sl256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sl512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sl20480:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)20479u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_periodicity_and_offset_ext_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRS-SpatialRelationInfoPos-r16 ::= CHOICE
void srs_spatial_relation_info_pos_r16_c::destroy_()
{
  switch (type_) {
    case types::serving_rs_r16:
      c.destroy<serving_rs_r16_s_>();
      break;
    case types::ssb_ncell_r16:
      c.destroy<ssb_info_ncell_r16_s>();
      break;
    case types::dl_prs_r16:
      c.destroy<dl_prs_info_r16_s>();
      break;
    default:
      break;
  }
}
void srs_spatial_relation_info_pos_r16_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::serving_rs_r16:
      c.init<serving_rs_r16_s_>();
      break;
    case types::ssb_ncell_r16:
      c.init<ssb_info_ncell_r16_s>();
      break;
    case types::dl_prs_r16:
      c.init<dl_prs_info_r16_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c");
  }
}
srs_spatial_relation_info_pos_r16_c::srs_spatial_relation_info_pos_r16_c(
    const srs_spatial_relation_info_pos_r16_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::serving_rs_r16:
      c.init(other.c.get<serving_rs_r16_s_>());
      break;
    case types::ssb_ncell_r16:
      c.init(other.c.get<ssb_info_ncell_r16_s>());
      break;
    case types::dl_prs_r16:
      c.init(other.c.get<dl_prs_info_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c");
  }
}
srs_spatial_relation_info_pos_r16_c&
srs_spatial_relation_info_pos_r16_c::operator=(const srs_spatial_relation_info_pos_r16_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::serving_rs_r16:
      c.set(other.c.get<serving_rs_r16_s_>());
      break;
    case types::ssb_ncell_r16:
      c.set(other.c.get<ssb_info_ncell_r16_s>());
      break;
    case types::dl_prs_r16:
      c.set(other.c.get<dl_prs_info_r16_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c");
  }

  return *this;
}
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_& srs_spatial_relation_info_pos_r16_c::set_serving_rs_r16()
{
  set(types::serving_rs_r16);
  return c.get<serving_rs_r16_s_>();
}
ssb_info_ncell_r16_s& srs_spatial_relation_info_pos_r16_c::set_ssb_ncell_r16()
{
  set(types::ssb_ncell_r16);
  return c.get<ssb_info_ncell_r16_s>();
}
dl_prs_info_r16_s& srs_spatial_relation_info_pos_r16_c::set_dl_prs_r16()
{
  set(types::dl_prs_r16);
  return c.get<dl_prs_info_r16_s>();
}
void srs_spatial_relation_info_pos_r16_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::serving_rs_r16:
      j.write_fieldname("servingRS-r16");
      j.start_obj();
      if (c.get<serving_rs_r16_s_>().serving_cell_id_present) {
        j.write_int("servingCellId", c.get<serving_rs_r16_s_>().serving_cell_id);
      }
      j.write_fieldname("referenceSignal-r16");
      c.get<serving_rs_r16_s_>().ref_sig_r16.to_json(j);
      j.end_obj();
      break;
    case types::ssb_ncell_r16:
      j.write_fieldname("ssb-Ncell-r16");
      c.get<ssb_info_ncell_r16_s>().to_json(j);
      break;
    case types::dl_prs_r16:
      j.write_fieldname("dl-PRS-r16");
      c.get<dl_prs_info_r16_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_spatial_relation_info_pos_r16_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::serving_rs_r16:
      HANDLE_CODE(bref.pack(c.get<serving_rs_r16_s_>().serving_cell_id_present, 1));
      if (c.get<serving_rs_r16_s_>().serving_cell_id_present) {
        HANDLE_CODE(pack_integer(bref, c.get<serving_rs_r16_s_>().serving_cell_id, (uint8_t)0u, (uint8_t)31u));
      }
      HANDLE_CODE(c.get<serving_rs_r16_s_>().ref_sig_r16.pack(bref));
      break;
    case types::ssb_ncell_r16:
      HANDLE_CODE(c.get<ssb_info_ncell_r16_s>().pack(bref));
      break;
    case types::dl_prs_r16:
      HANDLE_CODE(c.get<dl_prs_info_r16_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_spatial_relation_info_pos_r16_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::serving_rs_r16:
      HANDLE_CODE(bref.unpack(c.get<serving_rs_r16_s_>().serving_cell_id_present, 1));
      if (c.get<serving_rs_r16_s_>().serving_cell_id_present) {
        HANDLE_CODE(unpack_integer(c.get<serving_rs_r16_s_>().serving_cell_id, bref, (uint8_t)0u, (uint8_t)31u));
      }
      HANDLE_CODE(c.get<serving_rs_r16_s_>().ref_sig_r16.unpack(bref));
      break;
    case types::ssb_ncell_r16:
      HANDLE_CODE(c.get<ssb_info_ncell_r16_s>().unpack(bref));
      break;
    case types::dl_prs_r16:
      HANDLE_CODE(c.get<dl_prs_info_r16_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::destroy_()
{
  switch (type_) {
    case types::srs_spatial_relation_r16:
      c.destroy<srs_spatial_relation_r16_s_>();
      break;
    default:
      break;
  }
}
void srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ssb_idx_serving_r16:
      break;
    case types::csi_rs_idx_serving_r16:
      break;
    case types::srs_spatial_relation_r16:
      c.init<srs_spatial_relation_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_");
  }
}
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::ref_sig_r16_c_(
    const srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ssb_idx_serving_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx_serving_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::srs_spatial_relation_r16:
      c.init(other.c.get<srs_spatial_relation_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_");
  }
}
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_&
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::operator=(
    const srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ssb_idx_serving_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::csi_rs_idx_serving_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::srs_spatial_relation_r16:
      c.set(other.c.get<srs_spatial_relation_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_");
  }

  return *this;
}
uint8_t& srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::set_ssb_idx_serving_r16()
{
  set(types::ssb_idx_serving_r16);
  return c.get<uint8_t>();
}
uint8_t& srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::set_csi_rs_idx_serving_r16()
{
  set(types::csi_rs_idx_serving_r16);
  return c.get<uint8_t>();
}
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_&
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::set_srs_spatial_relation_r16()
{
  set(types::srs_spatial_relation_r16);
  return c.get<srs_spatial_relation_r16_s_>();
}
void srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ssb_idx_serving_r16:
      j.write_int("ssb-IndexServing-r16", c.get<uint8_t>());
      break;
    case types::csi_rs_idx_serving_r16:
      j.write_int("csi-RS-IndexServing-r16", c.get<uint8_t>());
      break;
    case types::srs_spatial_relation_r16:
      j.write_fieldname("srs-SpatialRelation-r16");
      j.start_obj();
      j.write_fieldname("resourceSelection-r16");
      c.get<srs_spatial_relation_r16_s_>().res_sel_r16.to_json(j);
      j.write_int("uplinkBWP-r16", c.get<srs_spatial_relation_r16_s_>().ul_bwp_r16);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ssb_idx_serving_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx_serving_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)191u));
      break;
    case types::srs_spatial_relation_r16:
      HANDLE_CODE(c.get<srs_spatial_relation_r16_s_>().res_sel_r16.pack(bref));
      HANDLE_CODE(pack_integer(bref, c.get<srs_spatial_relation_r16_s_>().ul_bwp_r16, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ssb_idx_serving_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::csi_rs_idx_serving_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)191u));
      break;
    case types::srs_spatial_relation_r16:
      HANDLE_CODE(c.get<srs_spatial_relation_r16_s_>().res_sel_r16.unpack(bref));
      HANDLE_CODE(unpack_integer(c.get<srs_spatial_relation_r16_s_>().ul_bwp_r16, bref, (uint8_t)0u, (uint8_t)4u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::destroy_()
{
}
void srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::res_sel_r16_c_::
    res_sel_r16_c_(const srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::
                       srs_spatial_relation_r16_s_::res_sel_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::srs_res_id_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::srs_pos_res_id_r16:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_"
                            "relation_r16_s_::res_sel_r16_c_");
  }
}
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::res_sel_r16_c_&
srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::res_sel_r16_c_::
operator=(const srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
              res_sel_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::srs_res_id_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::srs_pos_res_id_r16:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_,
                            "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_"
                            "relation_r16_s_::res_sel_r16_c_");
  }

  return *this;
}
uint8_t& srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::set_srs_res_id_r16()
{
  set(types::srs_res_id_r16);
  return c.get<uint8_t>();
}
uint8_t& srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::set_srs_pos_res_id_r16()
{
  set(types::srs_pos_res_id_r16);
  return c.get<uint8_t>();
}
void srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::srs_res_id_r16:
      j.write_int("srs-ResourceId-r16", c.get<uint8_t>());
      break;
    case types::srs_pos_res_id_r16:
      j.write_int("srs-PosResourceId-r16", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_,
                            "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_"
                            "relation_r16_s_::res_sel_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::srs_res_id_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::srs_pos_res_id_r16:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_,
                            "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_"
                            "relation_r16_s_::res_sel_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_relation_r16_s_::
    res_sel_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::srs_res_id_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::srs_pos_res_id_r16:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    default:
      log_invalid_choice_id(type_,
                            "srs_spatial_relation_info_pos_r16_c::serving_rs_r16_s_::ref_sig_r16_c_::srs_spatial_"
                            "relation_r16_s_::res_sel_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// SRS-PosResource-r16 ::= SEQUENCE
SRSASN_CODE srs_pos_res_r16_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(spatial_relation_info_pos_r16_present, 1));

  HANDLE_CODE(pack_integer(bref, srs_pos_res_id_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(tx_comb_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, res_map_r16.start_position_r16, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(res_map_r16.nrof_symbols_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, freq_domain_shift_r16, (uint16_t)0u, (uint16_t)268u));
  bref.pack(freq_hop_r16.ext, 1);
  HANDLE_CODE(pack_integer(bref, freq_hop_r16.c_srs_r16, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(group_or_seq_hop_r16.pack(bref));
  HANDLE_CODE(res_type_r16.pack(bref));
  HANDLE_CODE(pack_integer(bref, seq_id_r16, (uint32_t)0u, (uint32_t)65535u));
  if (spatial_relation_info_pos_r16_present) {
    HANDLE_CODE(spatial_relation_info_pos_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_res_r16_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(spatial_relation_info_pos_r16_present, 1));

  HANDLE_CODE(unpack_integer(srs_pos_res_id_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(tx_comb_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(res_map_r16.start_position_r16, bref, (uint8_t)0u, (uint8_t)13u));
  HANDLE_CODE(res_map_r16.nrof_symbols_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(freq_domain_shift_r16, bref, (uint16_t)0u, (uint16_t)268u));
  bref.unpack(freq_hop_r16.ext, 1);
  HANDLE_CODE(unpack_integer(freq_hop_r16.c_srs_r16, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(group_or_seq_hop_r16.unpack(bref));
  HANDLE_CODE(res_type_r16.unpack(bref));
  HANDLE_CODE(unpack_integer(seq_id_r16, bref, (uint32_t)0u, (uint32_t)65535u));
  if (spatial_relation_info_pos_r16_present) {
    HANDLE_CODE(spatial_relation_info_pos_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_pos_res_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("srs-PosResourceId-r16", srs_pos_res_id_r16);
  j.write_fieldname("transmissionComb-r16");
  tx_comb_r16.to_json(j);
  j.write_fieldname("resourceMapping-r16");
  j.start_obj();
  j.write_int("startPosition-r16", res_map_r16.start_position_r16);
  j.write_str("nrofSymbols-r16", res_map_r16.nrof_symbols_r16.to_string());
  j.end_obj();
  j.write_int("freqDomainShift-r16", freq_domain_shift_r16);
  j.write_fieldname("freqHopping-r16");
  j.start_obj();
  j.write_int("c-SRS-r16", freq_hop_r16.c_srs_r16);
  j.end_obj();
  j.write_str("groupOrSequenceHopping-r16", group_or_seq_hop_r16.to_string());
  j.write_fieldname("resourceType-r16");
  res_type_r16.to_json(j);
  j.write_int("sequenceId-r16", seq_id_r16);
  if (spatial_relation_info_pos_r16_present) {
    j.write_fieldname("spatialRelationInfoPos-r16");
    spatial_relation_info_pos_r16.to_json(j);
  }
  j.end_obj();
}

void srs_pos_res_r16_s::tx_comb_r16_c_::destroy_()
{
  switch (type_) {
    case types::n2_r16:
      c.destroy<n2_r16_s_>();
      break;
    case types::n4_r16:
      c.destroy<n4_r16_s_>();
      break;
    case types::n8_r16:
      c.destroy<n8_r16_s_>();
      break;
    default:
      break;
  }
}
void srs_pos_res_r16_s::tx_comb_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::n2_r16:
      c.init<n2_r16_s_>();
      break;
    case types::n4_r16:
      c.init<n4_r16_s_>();
      break;
    case types::n8_r16:
      c.init<n8_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::tx_comb_r16_c_");
  }
}
srs_pos_res_r16_s::tx_comb_r16_c_::tx_comb_r16_c_(const srs_pos_res_r16_s::tx_comb_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::n2_r16:
      c.init(other.c.get<n2_r16_s_>());
      break;
    case types::n4_r16:
      c.init(other.c.get<n4_r16_s_>());
      break;
    case types::n8_r16:
      c.init(other.c.get<n8_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::tx_comb_r16_c_");
  }
}
srs_pos_res_r16_s::tx_comb_r16_c_&
srs_pos_res_r16_s::tx_comb_r16_c_::operator=(const srs_pos_res_r16_s::tx_comb_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::n2_r16:
      c.set(other.c.get<n2_r16_s_>());
      break;
    case types::n4_r16:
      c.set(other.c.get<n4_r16_s_>());
      break;
    case types::n8_r16:
      c.set(other.c.get<n8_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::tx_comb_r16_c_");
  }

  return *this;
}
srs_pos_res_r16_s::tx_comb_r16_c_::n2_r16_s_& srs_pos_res_r16_s::tx_comb_r16_c_::set_n2_r16()
{
  set(types::n2_r16);
  return c.get<n2_r16_s_>();
}
srs_pos_res_r16_s::tx_comb_r16_c_::n4_r16_s_& srs_pos_res_r16_s::tx_comb_r16_c_::set_n4_r16()
{
  set(types::n4_r16);
  return c.get<n4_r16_s_>();
}
srs_pos_res_r16_s::tx_comb_r16_c_::n8_r16_s_& srs_pos_res_r16_s::tx_comb_r16_c_::set_n8_r16()
{
  set(types::n8_r16);
  return c.get<n8_r16_s_>();
}
void srs_pos_res_r16_s::tx_comb_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::n2_r16:
      j.write_fieldname("n2-r16");
      j.start_obj();
      j.write_int("combOffset-n2-r16", c.get<n2_r16_s_>().comb_offset_n2_r16);
      j.write_int("cyclicShift-n2-r16", c.get<n2_r16_s_>().cyclic_shift_n2_r16);
      j.end_obj();
      break;
    case types::n4_r16:
      j.write_fieldname("n4-r16");
      j.start_obj();
      j.write_int("combOffset-n4-r16", c.get<n4_r16_s_>().comb_offset_n4_r16);
      j.write_int("cyclicShift-n4-r16", c.get<n4_r16_s_>().cyclic_shift_n4_r16);
      j.end_obj();
      break;
    case types::n8_r16:
      j.write_fieldname("n8-r16");
      j.start_obj();
      j.write_int("combOffset-n8-r16", c.get<n8_r16_s_>().comb_offset_n8_r16);
      j.write_int("cyclicShift-n8-r16", c.get<n8_r16_s_>().cyclic_shift_n8_r16);
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::tx_comb_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_pos_res_r16_s::tx_comb_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::n2_r16:
      HANDLE_CODE(pack_integer(bref, c.get<n2_r16_s_>().comb_offset_n2_r16, (uint8_t)0u, (uint8_t)1u));
      HANDLE_CODE(pack_integer(bref, c.get<n2_r16_s_>().cyclic_shift_n2_r16, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n4_r16:
      HANDLE_CODE(pack_integer(bref, c.get<n4_r16_s_>().comb_offset_n4_r16, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(pack_integer(bref, c.get<n4_r16_s_>().cyclic_shift_n4_r16, (uint8_t)0u, (uint8_t)11u));
      break;
    case types::n8_r16:
      HANDLE_CODE(pack_integer(bref, c.get<n8_r16_s_>().comb_offset_n8_r16, (uint8_t)0u, (uint8_t)7u));
      HANDLE_CODE(pack_integer(bref, c.get<n8_r16_s_>().cyclic_shift_n8_r16, (uint8_t)0u, (uint8_t)5u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::tx_comb_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_res_r16_s::tx_comb_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::n2_r16:
      HANDLE_CODE(unpack_integer(c.get<n2_r16_s_>().comb_offset_n2_r16, bref, (uint8_t)0u, (uint8_t)1u));
      HANDLE_CODE(unpack_integer(c.get<n2_r16_s_>().cyclic_shift_n2_r16, bref, (uint8_t)0u, (uint8_t)7u));
      break;
    case types::n4_r16:
      HANDLE_CODE(unpack_integer(c.get<n4_r16_s_>().comb_offset_n4_r16, bref, (uint8_t)0u, (uint8_t)3u));
      HANDLE_CODE(unpack_integer(c.get<n4_r16_s_>().cyclic_shift_n4_r16, bref, (uint8_t)0u, (uint8_t)11u));
      break;
    case types::n8_r16:
      HANDLE_CODE(unpack_integer(c.get<n8_r16_s_>().comb_offset_n8_r16, bref, (uint8_t)0u, (uint8_t)7u));
      HANDLE_CODE(unpack_integer(c.get<n8_r16_s_>().cyclic_shift_n8_r16, bref, (uint8_t)0u, (uint8_t)5u));
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::tx_comb_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* srs_pos_res_r16_s::res_map_r16_s_::nrof_symbols_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n12"};
  return convert_enum_idx(names, 5, value, "srs_pos_res_r16_s::res_map_r16_s_::nrof_symbols_r16_e_");
}
uint8_t srs_pos_res_r16_s::res_map_r16_s_::nrof_symbols_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 12};
  return map_enum_number(numbers, 5, value, "srs_pos_res_r16_s::res_map_r16_s_::nrof_symbols_r16_e_");
}

const char* srs_pos_res_r16_s::group_or_seq_hop_r16_opts::to_string() const
{
  static const char* names[] = {"neither", "groupHopping", "sequenceHopping"};
  return convert_enum_idx(names, 3, value, "srs_pos_res_r16_s::group_or_seq_hop_r16_e_");
}

void srs_pos_res_r16_s::res_type_r16_c_::destroy_()
{
  switch (type_) {
    case types::aperiodic_r16:
      c.destroy<aperiodic_r16_s_>();
      break;
    case types::semi_persistent_r16:
      c.destroy<semi_persistent_r16_s_>();
      break;
    case types::periodic_r16:
      c.destroy<periodic_r16_s_>();
      break;
    default:
      break;
  }
}
void srs_pos_res_r16_s::res_type_r16_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::aperiodic_r16:
      c.init<aperiodic_r16_s_>();
      break;
    case types::semi_persistent_r16:
      c.init<semi_persistent_r16_s_>();
      break;
    case types::periodic_r16:
      c.init<periodic_r16_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::res_type_r16_c_");
  }
}
srs_pos_res_r16_s::res_type_r16_c_::res_type_r16_c_(const srs_pos_res_r16_s::res_type_r16_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::aperiodic_r16:
      c.init(other.c.get<aperiodic_r16_s_>());
      break;
    case types::semi_persistent_r16:
      c.init(other.c.get<semi_persistent_r16_s_>());
      break;
    case types::periodic_r16:
      c.init(other.c.get<periodic_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::res_type_r16_c_");
  }
}
srs_pos_res_r16_s::res_type_r16_c_&
srs_pos_res_r16_s::res_type_r16_c_::operator=(const srs_pos_res_r16_s::res_type_r16_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::aperiodic_r16:
      c.set(other.c.get<aperiodic_r16_s_>());
      break;
    case types::semi_persistent_r16:
      c.set(other.c.get<semi_persistent_r16_s_>());
      break;
    case types::periodic_r16:
      c.set(other.c.get<periodic_r16_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::res_type_r16_c_");
  }

  return *this;
}
srs_pos_res_r16_s::res_type_r16_c_::aperiodic_r16_s_& srs_pos_res_r16_s::res_type_r16_c_::set_aperiodic_r16()
{
  set(types::aperiodic_r16);
  return c.get<aperiodic_r16_s_>();
}
srs_pos_res_r16_s::res_type_r16_c_::semi_persistent_r16_s_&
srs_pos_res_r16_s::res_type_r16_c_::set_semi_persistent_r16()
{
  set(types::semi_persistent_r16);
  return c.get<semi_persistent_r16_s_>();
}
srs_pos_res_r16_s::res_type_r16_c_::periodic_r16_s_& srs_pos_res_r16_s::res_type_r16_c_::set_periodic_r16()
{
  set(types::periodic_r16);
  return c.get<periodic_r16_s_>();
}
void srs_pos_res_r16_s::res_type_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::aperiodic_r16:
      j.write_fieldname("aperiodic-r16");
      j.start_obj();
      if (c.get<aperiodic_r16_s_>().slot_offset_r16_present) {
        j.write_int("slotOffset-r16", c.get<aperiodic_r16_s_>().slot_offset_r16);
      }
      j.end_obj();
      break;
    case types::semi_persistent_r16:
      j.write_fieldname("semi-persistent-r16");
      c.get<semi_persistent_r16_s_>().to_json(j);
      break;
    case types::periodic_r16:
      j.write_fieldname("periodic-r16");
      c.get<periodic_r16_s_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::res_type_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE srs_pos_res_r16_s::res_type_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::aperiodic_r16:
      bref.pack(c.get<aperiodic_r16_s_>().ext, 1);
      HANDLE_CODE(bref.pack(c.get<aperiodic_r16_s_>().slot_offset_r16_present, 1));
      if (c.get<aperiodic_r16_s_>().slot_offset_r16_present) {
        HANDLE_CODE(pack_integer(bref, c.get<aperiodic_r16_s_>().slot_offset_r16, (uint8_t)1u, (uint8_t)32u));
      }
      break;
    case types::semi_persistent_r16:
      HANDLE_CODE(c.get<semi_persistent_r16_s_>().pack(bref));
      break;
    case types::periodic_r16:
      HANDLE_CODE(c.get<periodic_r16_s_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::res_type_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_res_r16_s::res_type_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::aperiodic_r16:
      bref.unpack(c.get<aperiodic_r16_s_>().ext, 1);
      HANDLE_CODE(bref.unpack(c.get<aperiodic_r16_s_>().slot_offset_r16_present, 1));
      if (c.get<aperiodic_r16_s_>().slot_offset_r16_present) {
        HANDLE_CODE(unpack_integer(c.get<aperiodic_r16_s_>().slot_offset_r16, bref, (uint8_t)1u, (uint8_t)32u));
      }
      break;
    case types::semi_persistent_r16:
      HANDLE_CODE(c.get<semi_persistent_r16_s_>().unpack(bref));
      break;
    case types::periodic_r16:
      HANDLE_CODE(c.get<periodic_r16_s_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "srs_pos_res_r16_s::res_type_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE srs_pos_res_r16_s::res_type_r16_c_::semi_persistent_r16_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(periodicity_and_offset_sp_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= periodicity_and_offset_sp_ext_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(periodicity_and_offset_sp_ext_r16.is_present(), 1));
      if (periodicity_and_offset_sp_ext_r16.is_present()) {
        HANDLE_CODE(periodicity_and_offset_sp_ext_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_res_r16_s::res_type_r16_c_::semi_persistent_r16_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(periodicity_and_offset_sp_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(periodicity_and_offset_sp_ext_r16, bref);
      if (periodicity_and_offset_sp_ext_r16.is_present()) {
        HANDLE_CODE(periodicity_and_offset_sp_ext_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srs_pos_res_r16_s::res_type_r16_c_::semi_persistent_r16_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityAndOffset-sp-r16");
  periodicity_and_offset_sp_r16.to_json(j);
  if (ext) {
    if (periodicity_and_offset_sp_ext_r16.is_present()) {
      j.write_fieldname("periodicityAndOffset-sp-Ext-r16");
      periodicity_and_offset_sp_ext_r16->to_json(j);
    }
  }
  j.end_obj();
}

SRSASN_CODE srs_pos_res_r16_s::res_type_r16_c_::periodic_r16_s_::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(periodicity_and_offset_p_r16.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= periodicity_and_offset_p_ext_r16.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(periodicity_and_offset_p_ext_r16.is_present(), 1));
      if (periodicity_and_offset_p_ext_r16.is_present()) {
        HANDLE_CODE(periodicity_and_offset_p_ext_r16->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_res_r16_s::res_type_r16_c_::periodic_r16_s_::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(periodicity_and_offset_p_r16.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      unpack_presence_flag(periodicity_and_offset_p_ext_r16, bref);
      if (periodicity_and_offset_p_ext_r16.is_present()) {
        HANDLE_CODE(periodicity_and_offset_p_ext_r16->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void srs_pos_res_r16_s::res_type_r16_c_::periodic_r16_s_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("periodicityAndOffset-p-r16");
  periodicity_and_offset_p_r16.to_json(j);
  if (ext) {
    if (periodicity_and_offset_p_ext_r16.is_present()) {
      j.write_fieldname("periodicityAndOffset-p-Ext-r16");
      periodicity_and_offset_p_ext_r16->to_json(j);
    }
  }
  j.end_obj();
}

// SRS-PosResourceAP-r16 ::= SEQUENCE
SRSASN_CODE srs_pos_res_ap_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_ap_srs_pos_res_per_bwp_r16.pack(bref));
  HANDLE_CODE(max_num_ap_srs_pos_res_per_bwp_per_slot_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_res_ap_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_ap_srs_pos_res_per_bwp_r16.unpack(bref));
  HANDLE_CODE(max_num_ap_srs_pos_res_per_bwp_per_slot_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_pos_res_ap_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberAP-SRS-PosResourcesPerBWP-r16", max_num_ap_srs_pos_res_per_bwp_r16.to_string());
  j.write_str("maxNumberAP-SRS-PosResourcesPerBWP-PerSlot-r16",
              max_num_ap_srs_pos_res_per_bwp_per_slot_r16.to_string());
  j.end_obj();
}

const char* srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 7, value, "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_e_");
}
uint8_t srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_e_");
}

const char* srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10", "n12", "n14"};
  return convert_enum_idx(names, 10, value, "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e_");
}
uint8_t srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 14};
  return map_enum_number(numbers, 10, value, "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e_");
}

// SRS-PosResourceSP-r16 ::= SEQUENCE
SRSASN_CODE srs_pos_res_sp_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(max_num_sp_srs_pos_res_per_bwp_r16.pack(bref));
  HANDLE_CODE(max_num_sp_srs_pos_res_per_bwp_per_slot_r16.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_pos_res_sp_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(max_num_sp_srs_pos_res_per_bwp_r16.unpack(bref));
  HANDLE_CODE(max_num_sp_srs_pos_res_per_bwp_per_slot_r16.unpack(bref));

  return SRSASN_SUCCESS;
}
void srs_pos_res_sp_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxNumberSP-SRS-PosResourcesPerBWP-r16", max_num_sp_srs_pos_res_per_bwp_r16.to_string());
  j.write_str("maxNumberSP-SRS-PosResourcesPerBWP-PerSlot-r16",
              max_num_sp_srs_pos_res_per_bwp_per_slot_r16.to_string());
  j.end_obj();
}

const char* srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
  return convert_enum_idx(names, 7, value, "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_e_");
}
uint8_t srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_e_");
}

const char* srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_opts::to_string() const
{
  static const char* names[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10", "n12", "n14"};
  return convert_enum_idx(names, 10, value, "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e_");
}
uint8_t srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 14};
  return map_enum_number(numbers, 10, value, "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e_");
}

// SRS-AllPosResources-r16 ::= SEQUENCE
SRSASN_CODE srs_all_pos_res_r16_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(srs_pos_res_ap_r16_present, 1));
  HANDLE_CODE(bref.pack(srs_pos_res_sp_r16_present, 1));

  HANDLE_CODE(srs_pos_res_r16.pack(bref));
  if (srs_pos_res_ap_r16_present) {
    HANDLE_CODE(srs_pos_res_ap_r16.pack(bref));
  }
  if (srs_pos_res_sp_r16_present) {
    HANDLE_CODE(srs_pos_res_sp_r16.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_all_pos_res_r16_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(srs_pos_res_ap_r16_present, 1));
  HANDLE_CODE(bref.unpack(srs_pos_res_sp_r16_present, 1));

  HANDLE_CODE(srs_pos_res_r16.unpack(bref));
  if (srs_pos_res_ap_r16_present) {
    HANDLE_CODE(srs_pos_res_ap_r16.unpack(bref));
  }
  if (srs_pos_res_sp_r16_present) {
    HANDLE_CODE(srs_pos_res_sp_r16.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void srs_all_pos_res_r16_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("srs-PosResources-r16");
  srs_pos_res_r16.to_json(j);
  if (srs_pos_res_ap_r16_present) {
    j.write_fieldname("srs-PosResourceAP-r16");
    srs_pos_res_ap_r16.to_json(j);
  }
  if (srs_pos_res_sp_r16_present) {
    j.write_fieldname("srs-PosResourceSP-r16");
    srs_pos_res_sp_r16.to_json(j);
  }
  j.end_obj();
}
