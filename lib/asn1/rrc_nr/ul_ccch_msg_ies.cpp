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

#include "srsran/asn1/rrc_nr/ul_ccch_msg_ies.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// EstablishmentCause ::= ENUMERATED
const char* establishment_cause_opts::to_string() const
{
  static const char* names[] = {"emergency",
                                "highPriorityAccess",
                                "mt-Access",
                                "mo-Signalling",
                                "mo-Data",
                                "mo-VoiceCall",
                                "mo-VideoCall",
                                "mo-SMS",
                                "mps-PriorityAccess",
                                "mcs-PriorityAccess",
                                "spare6",
                                "spare5",
                                "spare4",
                                "spare3",
                                "spare2",
                                "spare1"};
  return convert_enum_idx(names, 16, value, "establishment_cause_e");
}

// InitialUE-Identity ::= CHOICE
void init_ue_id_c::destroy_()
{
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.destroy<fixed_bitstring<39>>();
      break;
    case types::random_value:
      c.destroy<fixed_bitstring<39>>();
      break;
    default:
      break;
  }
}
void init_ue_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.init<fixed_bitstring<39>>();
      break;
    case types::random_value:
      c.init<fixed_bitstring<39>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c::init_ue_id_c(const init_ue_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.init(other.c.get<fixed_bitstring<39>>());
      break;
    case types::random_value:
      c.init(other.c.get<fixed_bitstring<39>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c& init_ue_id_c::operator=(const init_ue_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      c.set(other.c.get<fixed_bitstring<39>>());
      break;
    case types::random_value:
      c.set(other.c.get<fixed_bitstring<39>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }

  return *this;
}
fixed_bitstring<39>& init_ue_id_c::set_ng_5_g_s_tmsi_part1()
{
  set(types::ng_5_g_s_tmsi_part1);
  return c.get<fixed_bitstring<39>>();
}
fixed_bitstring<39>& init_ue_id_c::set_random_value()
{
  set(types::random_value);
  return c.get<fixed_bitstring<39>>();
}
void init_ue_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      j.write_str("ng-5G-S-TMSI-Part1", c.get<fixed_bitstring<39>>().to_string());
      break;
    case types::random_value:
      j.write_str("randomValue", c.get<fixed_bitstring<39>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
  j.end_obj();
}
SRSASN_CODE init_ue_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().pack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE init_ue_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().unpack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<39>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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

// ReestabUE-Identity ::= SEQUENCE
SRSASN_CODE reestab_ue_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, c_rnti, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(short_mac_i.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reestab_ue_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(c_rnti, bref, (uint32_t)0u, (uint32_t)65535u));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)1007u));
  HANDLE_CODE(short_mac_i.unpack(bref));

  return SRSASN_SUCCESS;
}
void reestab_ue_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("c-RNTI", c_rnti);
  j.write_int("physCellId", pci);
  j.write_str("shortMAC-I", short_mac_i.to_string());
  j.end_obj();
}

// ReestablishmentCause ::= ENUMERATED
const char* reest_cause_opts::to_string() const
{
  static const char* names[] = {"reconfigurationFailure", "handoverFailure", "otherFailure", "spare1"};
  return convert_enum_idx(names, 4, value, "reest_cause_e");
}

// ResumeCause ::= ENUMERATED
const char* resume_cause_opts::to_string() const
{
  static const char* names[] = {"emergency",
                                "highPriorityAccess",
                                "mt-Access",
                                "mo-Signalling",
                                "mo-Data",
                                "mo-VoiceCall",
                                "mo-VideoCall",
                                "mo-SMS",
                                "rna-Update",
                                "mps-PriorityAccess",
                                "mcs-PriorityAccess",
                                "spare1",
                                "spare2",
                                "spare3",
                                "spare4",
                                "spare5"};
  return convert_enum_idx(names, 16, value, "resume_cause_e");
}

// RRC-PosSystemInfoRequest-r16-IEs ::= SEQUENCE
SRSASN_CODE rrc_pos_sys_info_request_r16_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(requested_pos_si_list.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_pos_sys_info_request_r16_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(requested_pos_si_list.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_pos_sys_info_request_r16_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("requestedPosSI-List", requested_pos_si_list.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCReestablishmentRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_reest_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(reest_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(reest_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("reestablishmentCause", reest_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCResumeRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_resume_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id.pack(bref));
  HANDLE_CODE(resume_mac_i.pack(bref));
  HANDLE_CODE(resume_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id.unpack(bref));
  HANDLE_CODE(resume_mac_i.unpack(bref));
  HANDLE_CODE(resume_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("resumeIdentity", resume_id.to_string());
  j.write_str("resumeMAC-I", resume_mac_i.to_string());
  j.write_str("resumeCause", resume_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCSetupRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_setup_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(establishment_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(establishment_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("establishmentCause", establishment_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCSystemInfoRequest-IEs ::= SEQUENCE
SRSASN_CODE rrc_sys_info_request_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(requested_si_list.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(requested_si_list.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_sys_info_request_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("requested-SI-List", requested_si_list.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCReestablishmentRequest ::= SEQUENCE
SRSASN_CODE rrc_reest_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_reest_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_reest_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_reest_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_reest_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcReestablishmentRequest");
  rrc_reest_request.to_json(j);
  j.end_obj();
}

// RRCResumeRequest ::= SEQUENCE
SRSASN_CODE rrc_resume_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_resume_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_resume_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_resume_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_resume_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcResumeRequest");
  rrc_resume_request.to_json(j);
  j.end_obj();
}

// RRCSetupRequest ::= SEQUENCE
SRSASN_CODE rrc_setup_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rrc_setup_request.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_setup_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rrc_setup_request.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_setup_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcSetupRequest");
  rrc_setup_request.to_json(j);
  j.end_obj();
}

// RRCSystemInfoRequest ::= SEQUENCE
SRSASN_CODE rrc_sys_info_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_sys_info_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_sys_info_request_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_sys_info_request:
      c.destroy<rrc_sys_info_request_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.destroy<crit_exts_future_r16_c_>();
      break;
    default:
      break;
  }
}
void rrc_sys_info_request_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_sys_info_request:
      c.init<rrc_sys_info_request_ies_s>();
      break;
    case types::crit_exts_future_r16:
      c.init<crit_exts_future_r16_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }
}
rrc_sys_info_request_s::crit_exts_c_::crit_exts_c_(const rrc_sys_info_request_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_sys_info_request:
      c.init(other.c.get<rrc_sys_info_request_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.init(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }
}
rrc_sys_info_request_s::crit_exts_c_&
rrc_sys_info_request_s::crit_exts_c_::operator=(const rrc_sys_info_request_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_sys_info_request:
      c.set(other.c.get<rrc_sys_info_request_ies_s>());
      break;
    case types::crit_exts_future_r16:
      c.set(other.c.get<crit_exts_future_r16_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }

  return *this;
}
rrc_sys_info_request_ies_s& rrc_sys_info_request_s::crit_exts_c_::set_rrc_sys_info_request()
{
  set(types::rrc_sys_info_request);
  return c.get<rrc_sys_info_request_ies_s>();
}
rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_&
rrc_sys_info_request_s::crit_exts_c_::set_crit_exts_future_r16()
{
  set(types::crit_exts_future_r16);
  return c.get<crit_exts_future_r16_c_>();
}
void rrc_sys_info_request_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_sys_info_request:
      j.write_fieldname("rrcSystemInfoRequest");
      c.get<rrc_sys_info_request_ies_s>().to_json(j);
      break;
    case types::crit_exts_future_r16:
      j.write_fieldname("criticalExtensionsFuture-r16");
      c.get<crit_exts_future_r16_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_ies_s>().pack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_sys_info_request:
      HANDLE_CODE(c.get<rrc_sys_info_request_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future_r16:
      HANDLE_CODE(c.get<crit_exts_future_r16_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set(types::options e)
{
  type_ = e;
}
rrc_pos_sys_info_request_r16_ies_s&
rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set_rrc_pos_sys_info_request_r16()
{
  set(types::rrc_pos_sys_info_request_r16);
  return c;
}
void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_pos_sys_info_request_r16:
      j.write_fieldname("rrcPosSystemInfoRequest-r16");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_pos_sys_info_request_r16:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_pos_sys_info_request_r16:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
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
