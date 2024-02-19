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

#include "srsran/asn1/rrc_nr/pcch_msg.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

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
