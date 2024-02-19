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

#include "srsran/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "srsran/asn1/rrc_nr/sys_info.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

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

void bcch_dl_sch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::sys_info:
      c = sys_info_s{};
      break;
    case types::sib_type1:
      c = sib1_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_c::c1_c_");
  }
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
