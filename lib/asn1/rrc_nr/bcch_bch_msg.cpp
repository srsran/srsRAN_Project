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

#include "srsran/asn1/rrc_nr/bcch_bch_msg.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PDCCH-ConfigSIB1 ::= SEQUENCE
SRSASN_CODE pdcch_cfg_sib1_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, coreset_zero, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, search_space_zero, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdcch_cfg_sib1_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(coreset_zero, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(search_space_zero, bref, (uint8_t)0u, (uint8_t)15u));

  return SRSASN_SUCCESS;
}
void pdcch_cfg_sib1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("controlResourceSetZero", coreset_zero);
  j.write_int("searchSpaceZero", search_space_zero);
  j.end_obj();
}

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
