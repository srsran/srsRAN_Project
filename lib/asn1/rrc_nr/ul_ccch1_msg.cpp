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

#include "srsran/asn1/rrc_nr/ul_ccch1_msg.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

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
