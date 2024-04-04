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

#include "srsran/asn1/e2ap/e2ap.h"
using namespace asn1;
using namespace asn1::e2ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// ENB-ID-Choice ::= CHOICE
void enb_id_choice_c::destroy_()
{
  switch (type_) {
    case types::enb_id_macro:
      c.destroy<fixed_bitstring<20, false, true>>();
      break;
    case types::enb_id_shortmacro:
      c.destroy<fixed_bitstring<18, false, true>>();
      break;
    case types::enb_id_longmacro:
      c.destroy<fixed_bitstring<21, false, true>>();
      break;
    default:
      break;
  }
}
void enb_id_choice_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::enb_id_macro:
      c.init<fixed_bitstring<20, false, true>>();
      break;
    case types::enb_id_shortmacro:
      c.init<fixed_bitstring<18, false, true>>();
      break;
    case types::enb_id_longmacro:
      c.init<fixed_bitstring<21, false, true>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }
}
enb_id_choice_c::enb_id_choice_c(const enb_id_choice_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::enb_id_macro:
      c.init(other.c.get<fixed_bitstring<20, false, true>>());
      break;
    case types::enb_id_shortmacro:
      c.init(other.c.get<fixed_bitstring<18, false, true>>());
      break;
    case types::enb_id_longmacro:
      c.init(other.c.get<fixed_bitstring<21, false, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }
}
enb_id_choice_c& enb_id_choice_c::operator=(const enb_id_choice_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::enb_id_macro:
      c.set(other.c.get<fixed_bitstring<20, false, true>>());
      break;
    case types::enb_id_shortmacro:
      c.set(other.c.get<fixed_bitstring<18, false, true>>());
      break;
    case types::enb_id_longmacro:
      c.set(other.c.get<fixed_bitstring<21, false, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }

  return *this;
}
fixed_bitstring<20, false, true>& enb_id_choice_c::set_enb_id_macro()
{
  set(types::enb_id_macro);
  return c.get<fixed_bitstring<20, false, true>>();
}
fixed_bitstring<18, false, true>& enb_id_choice_c::set_enb_id_shortmacro()
{
  set(types::enb_id_shortmacro);
  return c.get<fixed_bitstring<18, false, true>>();
}
fixed_bitstring<21, false, true>& enb_id_choice_c::set_enb_id_longmacro()
{
  set(types::enb_id_longmacro);
  return c.get<fixed_bitstring<21, false, true>>();
}
void enb_id_choice_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::enb_id_macro:
      j.write_str("enb-ID-macro", c.get<fixed_bitstring<20, false, true>>().to_string());
      break;
    case types::enb_id_shortmacro:
      j.write_str("enb-ID-shortmacro", c.get<fixed_bitstring<18, false, true>>().to_string());
      break;
    case types::enb_id_longmacro:
      j.write_str("enb-ID-longmacro", c.get<fixed_bitstring<21, false, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }
  j.end_obj();
}
SRSASN_CODE enb_id_choice_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::enb_id_macro:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true>>().pack(bref)));
      break;
    case types::enb_id_shortmacro:
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true>>().pack(bref)));
      break;
    case types::enb_id_longmacro:
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE enb_id_choice_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::enb_id_macro:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true>>().unpack(bref)));
      break;
    case types::enb_id_shortmacro:
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true>>().unpack(bref)));
      break;
    case types::enb_id_longmacro:
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* enb_id_choice_c::types_opts::to_string() const
{
  static const char* names[] = {"enb-ID-macro", "enb-ID-shortmacro", "enb-ID-longmacro"};
  return convert_enum_idx(names, 3, value, "enb_id_choice_c::types");
}

// GNB-ID-Choice ::= CHOICE
void gnb_id_choice_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gnb-ID", c.to_string());
  j.end_obj();
}
SRSASN_CODE gnb_id_choice_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_id_choice_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "gnb_id_choice_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* gnb_id_choice_c::types_opts::to_string() const
{
  static const char* names[] = {"gnb-ID"};
  return convert_enum_idx(names, 1, value, "gnb_id_choice_c::types");
}

// ENGNB-ID ::= CHOICE
void en_gnb_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gNB-ID", c.to_string());
  j.end_obj();
}
SRSASN_CODE en_gnb_id_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE en_gnb_id_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "en_gnb_id_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* en_gnb_id_c::types_opts::to_string() const
{
  static const char* names[] = {"gNB-ID"};
  return convert_enum_idx(names, 1, value, "en_gnb_id_c::types");
}

// ENB-ID ::= CHOICE
void enb_id_c::destroy_()
{
  switch (type_) {
    case types::macro_enb_id:
      c.destroy<fixed_bitstring<20, false, true>>();
      break;
    case types::home_enb_id:
      c.destroy<fixed_bitstring<28, false, true>>();
      break;
    case types::short_macro_enb_id:
      c.destroy<fixed_bitstring<18, false, true>>();
      break;
    case types::long_macro_enb_id:
      c.destroy<fixed_bitstring<21, false, true>>();
      break;
    default:
      break;
  }
}
void enb_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::macro_enb_id:
      c.init<fixed_bitstring<20, false, true>>();
      break;
    case types::home_enb_id:
      c.init<fixed_bitstring<28, false, true>>();
      break;
    case types::short_macro_enb_id:
      c.init<fixed_bitstring<18, false, true>>();
      break;
    case types::long_macro_enb_id:
      c.init<fixed_bitstring<21, false, true>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }
}
enb_id_c::enb_id_c(const enb_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::macro_enb_id:
      c.init(other.c.get<fixed_bitstring<20, false, true>>());
      break;
    case types::home_enb_id:
      c.init(other.c.get<fixed_bitstring<28, false, true>>());
      break;
    case types::short_macro_enb_id:
      c.init(other.c.get<fixed_bitstring<18, false, true>>());
      break;
    case types::long_macro_enb_id:
      c.init(other.c.get<fixed_bitstring<21, false, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }
}
enb_id_c& enb_id_c::operator=(const enb_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::macro_enb_id:
      c.set(other.c.get<fixed_bitstring<20, false, true>>());
      break;
    case types::home_enb_id:
      c.set(other.c.get<fixed_bitstring<28, false, true>>());
      break;
    case types::short_macro_enb_id:
      c.set(other.c.get<fixed_bitstring<18, false, true>>());
      break;
    case types::long_macro_enb_id:
      c.set(other.c.get<fixed_bitstring<21, false, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }

  return *this;
}
fixed_bitstring<20, false, true>& enb_id_c::set_macro_enb_id()
{
  set(types::macro_enb_id);
  return c.get<fixed_bitstring<20, false, true>>();
}
fixed_bitstring<28, false, true>& enb_id_c::set_home_enb_id()
{
  set(types::home_enb_id);
  return c.get<fixed_bitstring<28, false, true>>();
}
fixed_bitstring<18, false, true>& enb_id_c::set_short_macro_enb_id()
{
  set(types::short_macro_enb_id);
  return c.get<fixed_bitstring<18, false, true>>();
}
fixed_bitstring<21, false, true>& enb_id_c::set_long_macro_enb_id()
{
  set(types::long_macro_enb_id);
  return c.get<fixed_bitstring<21, false, true>>();
}
void enb_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::macro_enb_id:
      j.write_str("macro-eNB-ID", c.get<fixed_bitstring<20, false, true>>().to_string());
      break;
    case types::home_enb_id:
      j.write_str("home-eNB-ID", c.get<fixed_bitstring<28, false, true>>().to_string());
      break;
    case types::short_macro_enb_id:
      j.write_str("short-Macro-eNB-ID", c.get<fixed_bitstring<18, false, true>>().to_string());
      break;
    case types::long_macro_enb_id:
      j.write_str("long-Macro-eNB-ID", c.get<fixed_bitstring<21, false, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }
  j.end_obj();
}
SRSASN_CODE enb_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::macro_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true>>().pack(bref)));
      break;
    case types::home_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<28, false, true>>().pack(bref)));
      break;
    case types::short_macro_enb_id: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true>>().pack(bref)));
    } break;
    case types::long_macro_enb_id: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true>>().pack(bref)));
    } break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE enb_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::macro_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true>>().unpack(bref)));
      break;
    case types::home_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<28, false, true>>().unpack(bref)));
      break;
    case types::short_macro_enb_id: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true>>().unpack(bref)));
    } break;
    case types::long_macro_enb_id: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true>>().unpack(bref)));
    } break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* enb_id_c::types_opts::to_string() const
{
  static const char* names[] = {"macro-eNB-ID", "home-eNB-ID", "short-Macro-eNB-ID", "long-Macro-eNB-ID"};
  return convert_enum_idx(names, 4, value, "enb_id_c::types");
}

// GlobalgNB-ID ::= SEQUENCE
SRSASN_CODE global_gnb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(gnb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_gnb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(gnb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_gnb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("plmn-id", plmn_id.to_string());
  j.write_fieldname("gnb-id");
  gnb_id.to_json(j);
  j.end_obj();
}

// GlobalngeNB-ID ::= SEQUENCE
SRSASN_CODE globalng_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(enb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE globalng_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(enb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void globalng_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("plmn-id", plmn_id.to_string());
  j.write_fieldname("enb-id");
  enb_id.to_json(j);
  j.end_obj();
}

// CauseE2node ::= ENUMERATED
const char* cause_e2node_opts::to_string() const
{
  static const char* names[] = {"e2node-component-unknown"};
  return convert_enum_idx(names, 1, value, "cause_e2node_e");
}
uint8_t cause_e2node_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "cause_e2node_e");
}

// CauseMisc ::= ENUMERATED
const char* cause_misc_opts::to_string() const
{
  static const char* names[] = {"control-processing-overload", "hardware-failure", "om-intervention", "unspecified"};
  return convert_enum_idx(names, 4, value, "cause_misc_e");
}

// CauseProtocol ::= ENUMERATED
const char* cause_protocol_opts::to_string() const
{
  static const char* names[] = {"transfer-syntax-error",
                                "abstract-syntax-error-reject",
                                "abstract-syntax-error-ignore-and-notify",
                                "message-not-compatible-with-receiver-state",
                                "semantic-error",
                                "abstract-syntax-error-falsely-constructed-message",
                                "unspecified"};
  return convert_enum_idx(names, 7, value, "cause_protocol_e");
}

// CauseRICrequest ::= ENUMERATED
const char* cause_ric_request_opts::to_string() const
{
  static const char* names[] = {"ran-function-id-invalid",
                                "action-not-supported",
                                "excessive-actions",
                                "duplicate-action",
                                "duplicate-event-trigger",
                                "function-resource-limit",
                                "request-id-unknown",
                                "inconsistent-action-subsequent-action-sequence",
                                "control-message-invalid",
                                "ric-call-process-id-invalid",
                                "control-timer-expired",
                                "control-failed-to-execute",
                                "system-not-ready",
                                "unspecified",
                                "ric-subscription-end-time-expired",
                                "ric-subscription-end-time-invalid",
                                "duplicate-ric-request-id",
                                "eventTriggerNotSupported",
                                "requested-information-unavailable",
                                "invalid-information-request"};
  return convert_enum_idx(names, 20, value, "cause_ric_request_e");
}

// CauseRICservice ::= ENUMERATED
const char* cause_ric_service_opts::to_string() const
{
  static const char* names[] = {"ran-function-not-supported", "excessive-functions", "ric-resource-limit"};
  return convert_enum_idx(names, 3, value, "cause_ric_service_e");
}

// CauseTransport ::= ENUMERATED
const char* cause_transport_opts::to_string() const
{
  static const char* names[] = {"unspecified", "transport-resource-unavailable"};
  return convert_enum_idx(names, 2, value, "cause_transport_e");
}

// GlobalENB-ID ::= SEQUENCE
SRSASN_CODE global_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(enb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(enb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_fieldname("eNB-ID");
  enb_id.to_json(j);
  j.end_obj();
}

// GlobalNG-RANNode-ID ::= CHOICE
void global_ng_ran_node_id_c::destroy_()
{
  switch (type_) {
    case types::gnb:
      c.destroy<global_gnb_id_s>();
      break;
    case types::ng_enb:
      c.destroy<globalng_enb_id_s>();
      break;
    default:
      break;
  }
}
void global_ng_ran_node_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb:
      c.init<global_gnb_id_s>();
      break;
    case types::ng_enb:
      c.init<globalng_enb_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }
}
global_ng_ran_node_id_c::global_ng_ran_node_id_c(const global_ng_ran_node_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb:
      c.init(other.c.get<global_gnb_id_s>());
      break;
    case types::ng_enb:
      c.init(other.c.get<globalng_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }
}
global_ng_ran_node_id_c& global_ng_ran_node_id_c::operator=(const global_ng_ran_node_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb:
      c.set(other.c.get<global_gnb_id_s>());
      break;
    case types::ng_enb:
      c.set(other.c.get<globalng_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }

  return *this;
}
global_gnb_id_s& global_ng_ran_node_id_c::set_gnb()
{
  set(types::gnb);
  return c.get<global_gnb_id_s>();
}
globalng_enb_id_s& global_ng_ran_node_id_c::set_ng_enb()
{
  set(types::ng_enb);
  return c.get<globalng_enb_id_s>();
}
void global_ng_ran_node_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb:
      j.write_fieldname("gNB");
      c.get<global_gnb_id_s>().to_json(j);
      break;
    case types::ng_enb:
      j.write_fieldname("ng-eNB");
      c.get<globalng_enb_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }
  j.end_obj();
}
SRSASN_CODE global_ng_ran_node_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<global_gnb_id_s>().pack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<globalng_enb_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE global_ng_ran_node_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<global_gnb_id_s>().unpack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<globalng_enb_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* global_ng_ran_node_id_c::types_opts::to_string() const
{
  static const char* names[] = {"gNB", "ng-eNB"};
  return convert_enum_idx(names, 2, value, "global_ng_ran_node_id_c::types");
}

// GlobalenGNB-ID ::= SEQUENCE
SRSASN_CODE globalen_gnb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(gnb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE globalen_gnb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(gnb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void globalen_gnb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_fieldname("gNB-ID");
  gnb_id.to_json(j);
  j.end_obj();
}

// RICsubsequentActionType ::= ENUMERATED
const char* ric_subsequent_action_type_opts::to_string() const
{
  static const char* names[] = {"continue", "wait"};
  return convert_enum_idx(names, 2, value, "ric_subsequent_action_type_e");
}

// RICtimeToWait ::= ENUMERATED
const char* ric_time_to_wait_opts::to_string() const
{
  static const char* names[] = {"w1ms",
                                "w2ms",
                                "w5ms",
                                "w10ms",
                                "w20ms",
                                "w30ms",
                                "w40ms",
                                "w50ms",
                                "w100ms",
                                "w200ms",
                                "w500ms",
                                "w1s",
                                "w2s",
                                "w5s",
                                "w10s",
                                "w20s",
                                "w60s"};
  return convert_enum_idx(names, 17, value, "ric_time_to_wait_e");
}

// Cause ::= CHOICE
void cause_c::destroy_() {}
void cause_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
cause_c::cause_c(const cause_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ric_request:
      c.init(other.c.get<cause_ric_request_e>());
      break;
    case types::ric_service:
      c.init(other.c.get<cause_ric_service_e>());
      break;
    case types::e2_node:
      c.init(other.c.get<cause_e2node_e>());
      break;
    case types::transport:
      c.init(other.c.get<cause_transport_e>());
      break;
    case types::protocol:
      c.init(other.c.get<cause_protocol_e>());
      break;
    case types::misc:
      c.init(other.c.get<cause_misc_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }
}
cause_c& cause_c::operator=(const cause_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ric_request:
      c.set(other.c.get<cause_ric_request_e>());
      break;
    case types::ric_service:
      c.set(other.c.get<cause_ric_service_e>());
      break;
    case types::e2_node:
      c.set(other.c.get<cause_e2node_e>());
      break;
    case types::transport:
      c.set(other.c.get<cause_transport_e>());
      break;
    case types::protocol:
      c.set(other.c.get<cause_protocol_e>());
      break;
    case types::misc:
      c.set(other.c.get<cause_misc_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }

  return *this;
}
cause_ric_request_e& cause_c::set_ric_request()
{
  set(types::ric_request);
  return c.get<cause_ric_request_e>();
}
cause_ric_service_e& cause_c::set_ric_service()
{
  set(types::ric_service);
  return c.get<cause_ric_service_e>();
}
cause_e2node_e& cause_c::set_e2_node()
{
  set(types::e2_node);
  return c.get<cause_e2node_e>();
}
cause_transport_e& cause_c::set_transport()
{
  set(types::transport);
  return c.get<cause_transport_e>();
}
cause_protocol_e& cause_c::set_protocol()
{
  set(types::protocol);
  return c.get<cause_protocol_e>();
}
cause_misc_e& cause_c::set_misc()
{
  set(types::misc);
  return c.get<cause_misc_e>();
}
void cause_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request:
      j.write_str("ricRequest", c.get<cause_ric_request_e>().to_string());
      break;
    case types::ric_service:
      j.write_str("ricService", c.get<cause_ric_service_e>().to_string());
      break;
    case types::e2_node:
      j.write_str("e2Node", "e2node-component-unknown");
      break;
    case types::transport:
      j.write_str("transport", c.get<cause_transport_e>().to_string());
      break;
    case types::protocol:
      j.write_str("protocol", c.get<cause_protocol_e>().to_string());
      break;
    case types::misc:
      j.write_str("misc", c.get<cause_misc_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }
  j.end_obj();
}
SRSASN_CODE cause_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ric_request:
      HANDLE_CODE(c.get<cause_ric_request_e>().pack(bref));
      break;
    case types::ric_service:
      HANDLE_CODE(c.get<cause_ric_service_e>().pack(bref));
      break;
    case types::e2_node:
      HANDLE_CODE(c.get<cause_e2node_e>().pack(bref));
      break;
    case types::transport:
      HANDLE_CODE(c.get<cause_transport_e>().pack(bref));
      break;
    case types::protocol:
      HANDLE_CODE(c.get<cause_protocol_e>().pack(bref));
      break;
    case types::misc:
      HANDLE_CODE(c.get<cause_misc_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cause_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ric_request:
      HANDLE_CODE(c.get<cause_ric_request_e>().unpack(bref));
      break;
    case types::ric_service:
      HANDLE_CODE(c.get<cause_ric_service_e>().unpack(bref));
      break;
    case types::e2_node:
      HANDLE_CODE(c.get<cause_e2node_e>().unpack(bref));
      break;
    case types::transport:
      HANDLE_CODE(c.get<cause_transport_e>().unpack(bref));
      break;
    case types::protocol:
      HANDLE_CODE(c.get<cause_protocol_e>().unpack(bref));
      break;
    case types::misc:
      HANDLE_CODE(c.get<cause_misc_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cause_c::types_opts::to_string() const
{
  static const char* names[] = {"ricRequest", "ricService", "e2Node", "transport", "protocol", "misc"};
  return convert_enum_idx(names, 6, value, "cause_c::types");
}
uint8_t cause_c::types_opts::to_number() const
{
  if (value == e2_node) {
    return 2;
  }
  invalid_enum_number(value, "cause_c::types");
  return 0;
}

// E2nodeComponentInterfaceE1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_e1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_e1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_e1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-CU-UP-ID", gnb_cu_up_id);
  j.end_obj();
}

// E2nodeComponentInterfaceF1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_f1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_f1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_f1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-DU-ID", gnb_du_id);
  j.end_obj();
}

// E2nodeComponentInterfaceNG ::= SEQUENCE
SRSASN_CODE e2node_component_interface_ng_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(amf_name.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_ng_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(amf_name.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_ng_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("amf-name", amf_name.to_string());
  j.end_obj();
}

// E2nodeComponentInterfaceS1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_s1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(mme_name.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_s1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(mme_name.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_s1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mme-name", mme_name.to_string());
  j.end_obj();
}

// E2nodeComponentInterfaceW1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_w1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ng_enb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_w1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ng_enb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_w1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ng-eNB-DU-ID", ng_enb_du_id);
  j.end_obj();
}

// E2nodeComponentInterfaceX2 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_x2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(global_enb_id_present, 1));
  HANDLE_CODE(bref.pack(global_en_gnb_id_present, 1));

  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.pack(bref));
  }
  if (global_en_gnb_id_present) {
    HANDLE_CODE(global_en_gnb_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_x2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(global_enb_id_present, 1));
  HANDLE_CODE(bref.unpack(global_en_gnb_id_present, 1));

  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.unpack(bref));
  }
  if (global_en_gnb_id_present) {
    HANDLE_CODE(global_en_gnb_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2node_component_interface_x2_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (global_enb_id_present) {
    j.write_fieldname("global-eNB-ID");
    global_enb_id.to_json(j);
  }
  if (global_en_gnb_id_present) {
    j.write_fieldname("global-en-gNB-ID");
    global_en_gnb_id.to_json(j);
  }
  j.end_obj();
}

// E2nodeComponentInterfaceXn ::= SEQUENCE
SRSASN_CODE e2node_component_interface_xn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_ng_ran_node_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_xn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_ng_ran_node_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_xn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-NG-RAN-Node-ID");
  global_ng_ran_node_id.to_json(j);
  j.end_obj();
}

// RICactionType ::= ENUMERATED
const char* ric_action_type_opts::to_string() const
{
  static const char* names[] = {"report", "insert", "policy"};
  return convert_enum_idx(names, 3, value, "ric_action_type_e");
}

// RICsubsequentAction ::= SEQUENCE
SRSASN_CODE ric_subsequent_action_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ric_subsequent_action_type.pack(bref));
  HANDLE_CODE(ric_time_to_wait.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_subsequent_action_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ric_subsequent_action_type.unpack(bref));
  HANDLE_CODE(ric_time_to_wait.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_subsequent_action_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ricSubsequentActionType", ric_subsequent_action_type.to_string());
  j.write_str("ricTimeToWait", ric_time_to_wait.to_string());
  j.end_obj();
}

// E2nodeComponentConfigurationAck ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_ack_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(fail_cause_present, 1));

  HANDLE_CODE(upd_outcome.pack(bref));
  if (fail_cause_present) {
    HANDLE_CODE(fail_cause.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_ack_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(fail_cause_present, 1));

  HANDLE_CODE(upd_outcome.unpack(bref));
  if (fail_cause_present) {
    HANDLE_CODE(fail_cause.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_ack_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("updateOutcome", upd_outcome.to_string());
  if (fail_cause_present) {
    j.write_fieldname("failureCause");
    fail_cause.to_json(j);
  }
  j.end_obj();
}

const char* e2node_component_cfg_ack_s::upd_outcome_opts::to_string() const
{
  static const char* names[] = {"success", "failure"};
  return convert_enum_idx(names, 2, value, "e2node_component_cfg_ack_s::upd_outcome_e_");
}

// E2nodeComponentConfiguration ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_request_part.pack(bref));
  HANDLE_CODE(e2node_component_resp_part.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_request_part.unpack(bref));
  HANDLE_CODE(e2node_component_resp_part.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentRequestPart", e2node_component_request_part.to_string());
  j.write_str("e2nodeComponentResponsePart", e2node_component_resp_part.to_string());
  j.end_obj();
}

// E2nodeComponentID ::= CHOICE
void e2node_component_id_c::destroy_()
{
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.destroy<e2node_component_interface_ng_s>();
      break;
    case types::e2node_component_interface_type_xn:
      c.destroy<e2node_component_interface_xn_s>();
      break;
    case types::e2node_component_interface_type_e1:
      c.destroy<e2node_component_interface_e1_s>();
      break;
    case types::e2node_component_interface_type_f1:
      c.destroy<e2node_component_interface_f1_s>();
      break;
    case types::e2node_component_interface_type_w1:
      c.destroy<e2node_component_interface_w1_s>();
      break;
    case types::e2node_component_interface_type_s1:
      c.destroy<e2node_component_interface_s1_s>();
      break;
    case types::e2node_component_interface_type_x2:
      c.destroy<e2node_component_interface_x2_s>();
      break;
    default:
      break;
  }
}
void e2node_component_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.init<e2node_component_interface_ng_s>();
      break;
    case types::e2node_component_interface_type_xn:
      c.init<e2node_component_interface_xn_s>();
      break;
    case types::e2node_component_interface_type_e1:
      c.init<e2node_component_interface_e1_s>();
      break;
    case types::e2node_component_interface_type_f1:
      c.init<e2node_component_interface_f1_s>();
      break;
    case types::e2node_component_interface_type_w1:
      c.init<e2node_component_interface_w1_s>();
      break;
    case types::e2node_component_interface_type_s1:
      c.init<e2node_component_interface_s1_s>();
      break;
    case types::e2node_component_interface_type_x2:
      c.init<e2node_component_interface_x2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }
}
e2node_component_id_c::e2node_component_id_c(const e2node_component_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.init(other.c.get<e2node_component_interface_ng_s>());
      break;
    case types::e2node_component_interface_type_xn:
      c.init(other.c.get<e2node_component_interface_xn_s>());
      break;
    case types::e2node_component_interface_type_e1:
      c.init(other.c.get<e2node_component_interface_e1_s>());
      break;
    case types::e2node_component_interface_type_f1:
      c.init(other.c.get<e2node_component_interface_f1_s>());
      break;
    case types::e2node_component_interface_type_w1:
      c.init(other.c.get<e2node_component_interface_w1_s>());
      break;
    case types::e2node_component_interface_type_s1:
      c.init(other.c.get<e2node_component_interface_s1_s>());
      break;
    case types::e2node_component_interface_type_x2:
      c.init(other.c.get<e2node_component_interface_x2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }
}
e2node_component_id_c& e2node_component_id_c::operator=(const e2node_component_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.set(other.c.get<e2node_component_interface_ng_s>());
      break;
    case types::e2node_component_interface_type_xn:
      c.set(other.c.get<e2node_component_interface_xn_s>());
      break;
    case types::e2node_component_interface_type_e1:
      c.set(other.c.get<e2node_component_interface_e1_s>());
      break;
    case types::e2node_component_interface_type_f1:
      c.set(other.c.get<e2node_component_interface_f1_s>());
      break;
    case types::e2node_component_interface_type_w1:
      c.set(other.c.get<e2node_component_interface_w1_s>());
      break;
    case types::e2node_component_interface_type_s1:
      c.set(other.c.get<e2node_component_interface_s1_s>());
      break;
    case types::e2node_component_interface_type_x2:
      c.set(other.c.get<e2node_component_interface_x2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }

  return *this;
}
e2node_component_interface_ng_s& e2node_component_id_c::set_e2node_component_interface_type_ng()
{
  set(types::e2node_component_interface_type_ng);
  return c.get<e2node_component_interface_ng_s>();
}
e2node_component_interface_xn_s& e2node_component_id_c::set_e2node_component_interface_type_xn()
{
  set(types::e2node_component_interface_type_xn);
  return c.get<e2node_component_interface_xn_s>();
}
e2node_component_interface_e1_s& e2node_component_id_c::set_e2node_component_interface_type_e1()
{
  set(types::e2node_component_interface_type_e1);
  return c.get<e2node_component_interface_e1_s>();
}
e2node_component_interface_f1_s& e2node_component_id_c::set_e2node_component_interface_type_f1()
{
  set(types::e2node_component_interface_type_f1);
  return c.get<e2node_component_interface_f1_s>();
}
e2node_component_interface_w1_s& e2node_component_id_c::set_e2node_component_interface_type_w1()
{
  set(types::e2node_component_interface_type_w1);
  return c.get<e2node_component_interface_w1_s>();
}
e2node_component_interface_s1_s& e2node_component_id_c::set_e2node_component_interface_type_s1()
{
  set(types::e2node_component_interface_type_s1);
  return c.get<e2node_component_interface_s1_s>();
}
e2node_component_interface_x2_s& e2node_component_id_c::set_e2node_component_interface_type_x2()
{
  set(types::e2node_component_interface_type_x2);
  return c.get<e2node_component_interface_x2_s>();
}
void e2node_component_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      j.write_fieldname("e2nodeComponentInterfaceTypeNG");
      c.get<e2node_component_interface_ng_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_xn:
      j.write_fieldname("e2nodeComponentInterfaceTypeXn");
      c.get<e2node_component_interface_xn_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_e1:
      j.write_fieldname("e2nodeComponentInterfaceTypeE1");
      c.get<e2node_component_interface_e1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_f1:
      j.write_fieldname("e2nodeComponentInterfaceTypeF1");
      c.get<e2node_component_interface_f1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_w1:
      j.write_fieldname("e2nodeComponentInterfaceTypeW1");
      c.get<e2node_component_interface_w1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_s1:
      j.write_fieldname("e2nodeComponentInterfaceTypeS1");
      c.get<e2node_component_interface_s1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_x2:
      j.write_fieldname("e2nodeComponentInterfaceTypeX2");
      c.get<e2node_component_interface_x2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_component_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      HANDLE_CODE(c.get<e2node_component_interface_ng_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_xn:
      HANDLE_CODE(c.get<e2node_component_interface_xn_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_e1:
      HANDLE_CODE(c.get<e2node_component_interface_e1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_f1:
      HANDLE_CODE(c.get<e2node_component_interface_f1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_w1:
      HANDLE_CODE(c.get<e2node_component_interface_w1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_s1:
      HANDLE_CODE(c.get<e2node_component_interface_s1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_x2:
      HANDLE_CODE(c.get<e2node_component_interface_x2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      HANDLE_CODE(c.get<e2node_component_interface_ng_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_xn:
      HANDLE_CODE(c.get<e2node_component_interface_xn_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_e1:
      HANDLE_CODE(c.get<e2node_component_interface_e1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_f1:
      HANDLE_CODE(c.get<e2node_component_interface_f1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_w1:
      HANDLE_CODE(c.get<e2node_component_interface_w1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_s1:
      HANDLE_CODE(c.get<e2node_component_interface_s1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_x2:
      HANDLE_CODE(c.get<e2node_component_interface_x2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_component_id_c::types_opts::to_string() const
{
  static const char* names[] = {"e2nodeComponentInterfaceTypeNG",
                                "e2nodeComponentInterfaceTypeXn",
                                "e2nodeComponentInterfaceTypeE1",
                                "e2nodeComponentInterfaceTypeF1",
                                "e2nodeComponentInterfaceTypeW1",
                                "e2nodeComponentInterfaceTypeS1",
                                "e2nodeComponentInterfaceTypeX2"};
  return convert_enum_idx(names, 7, value, "e2node_component_id_c::types");
}

// E2nodeComponentInterfaceType ::= ENUMERATED
const char* e2node_component_interface_type_opts::to_string() const
{
  static const char* names[] = {"ng", "xn", "e1", "f1", "w1", "s1", "x2"};
  return convert_enum_idx(names, 7, value, "e2node_component_interface_type_e");
}

// RICaction-ToBeSetup-Item ::= SEQUENCE
SRSASN_CODE ric_action_to_be_setup_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ric_action_definition.size() > 0, 1));
  HANDLE_CODE(bref.pack(ric_subsequent_action_present, 1));

  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_action_type.pack(bref));
  if (ric_action_definition.size() > 0) {
    HANDLE_CODE(ric_action_definition.pack(bref));
  }
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(ric_action_execution_order_present, 1));

    if (ric_action_execution_order_present) {
      HANDLE_CODE(pack_integer(bref, ric_action_execution_order, (uint16_t)0u, (uint16_t)255u, true, true));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_setup_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ric_action_definition_present;
  HANDLE_CODE(bref.unpack(ric_action_definition_present, 1));
  HANDLE_CODE(bref.unpack(ric_subsequent_action_present, 1));

  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_action_type.unpack(bref));
  if (ric_action_definition_present) {
    HANDLE_CODE(ric_action_definition.unpack(bref));
  }
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.unpack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(ric_action_execution_order_present, 1));

    if (ric_action_execution_order_present) {
      HANDLE_CODE(unpack_integer(ric_action_execution_order, bref, (uint16_t)0u, (uint16_t)255u, true, true));
    }
  }
  return SRSASN_SUCCESS;
}
void ric_action_to_be_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_str("ricActionType", ric_action_type.to_string());
  if (ric_action_definition.size() > 0) {
    j.write_str("ricActionDefinition", ric_action_definition.to_string());
  }
  if (ric_subsequent_action_present) {
    j.write_fieldname("ricSubsequentAction");
    ric_subsequent_action.to_json(j);
  }
  if (ext) {
    if (ric_action_execution_order_present) {
      j.write_int("ricActionExecutionOrder", ric_action_execution_order);
    }
  }
  j.end_obj();
}

// TNLinformation ::= SEQUENCE
SRSASN_CODE tnl_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tnl_port_present, 1));

  HANDLE_CODE(tnl_address.pack(bref));
  if (tnl_port_present) {
    HANDLE_CODE(tnl_port.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tnl_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tnl_port_present, 1));

  HANDLE_CODE(tnl_address.unpack(bref));
  if (tnl_port_present) {
    HANDLE_CODE(tnl_port.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tnl_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("tnlAddress", tnl_address.to_string());
  if (tnl_port_present) {
    j.write_str("tnlPort", tnl_port.to_string());
  }
  j.end_obj();
}

// TNLusage ::= ENUMERATED
const char* tnl_usage_opts::to_string() const
{
  static const char* names[] = {"ric-service", "support-function", "both"};
  return convert_enum_idx(names, 3, value, "tnl_usage_e");
}

// E2connectionSetupFailed-Item ::= SEQUENCE
SRSASN_CODE e2conn_setup_failed_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_setup_failed_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2conn_setup_failed_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// E2connectionUpdate-Item ::= SEQUENCE
SRSASN_CODE e2conn_upd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));
  HANDLE_CODE(tnl_usage.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));
  HANDLE_CODE(tnl_usage.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2conn_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.write_str("tnlUsage", tnl_usage.to_string());
  j.end_obj();
}

// E2connectionUpdateRemove-Item ::= SEQUENCE
SRSASN_CODE e2conn_upd_rem_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_rem_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2conn_upd_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigAdditionAck-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_addition_ack_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_ack_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_addition_ack_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfigurationAck");
  e2node_component_cfg_ack.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigAddition-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_addition_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_addition_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfiguration");
  e2node_component_cfg.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigRemovalAck-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_removal_ack_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_ack_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_removal_ack_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfigurationAck");
  e2node_component_cfg_ack.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigRemoval-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_removal_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_removal_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigUpdateAck-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_upd_ack_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_ack_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_upd_ack_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfigurationAck");
  e2node_component_cfg_ack.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigUpdate-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_upd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfiguration");
  e2node_component_cfg.to_json(j);
  j.end_obj();
}

// E2nodeTNLassociationRemoval-Item ::= SEQUENCE
SRSASN_CODE e2node_tnl_assoc_removal_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));
  HANDLE_CODE(tnl_info_ric.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_tnl_assoc_removal_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));
  HANDLE_CODE(tnl_info_ric.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_tnl_assoc_removal_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.write_fieldname("tnlInformationRIC");
  tnl_info_ric.to_json(j);
  j.end_obj();
}

// RANfunctionIDcause-Item ::= SEQUENCE
SRSASN_CODE ran_function_id_cause_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_id_cause_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ran_function_id_cause_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ranFunctionID", ran_function_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RANfunctionID-Item ::= SEQUENCE
SRSASN_CODE ran_function_id_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(pack_integer(bref, ran_function_revision, (uint16_t)0u, (uint16_t)4095u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_id_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(unpack_integer(ran_function_revision, bref, (uint16_t)0u, (uint16_t)4095u, false, true));

  return SRSASN_SUCCESS;
}
void ran_function_id_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ranFunctionID", ran_function_id);
  j.write_int("ranFunctionRevision", ran_function_revision);
  j.end_obj();
}

// RANfunction-Item ::= SEQUENCE
SRSASN_CODE ran_function_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_definition.pack(bref));
  HANDLE_CODE(pack_integer(bref, ran_function_revision, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_o_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_definition.unpack(bref));
  HANDLE_CODE(unpack_integer(ran_function_revision, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_o_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void ran_function_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ranFunctionID", ran_function_id);
  j.write_str("ranFunctionDefinition", ran_function_definition.to_string());
  j.write_int("ranFunctionRevision", ran_function_revision);
  j.write_str("ranFunctionOID", ran_function_o_id.to_string());
  j.end_obj();
}

// RICaction-AddedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_added_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_added_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ric_action_added_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-Admitted-Item ::= SEQUENCE
SRSASN_CODE ric_action_admitted_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_admitted_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ric_action_admitted_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-ConfirmedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_confirmed_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_confirmed_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ric_action_confirmed_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-ConfirmedForRemoval-Item ::= SEQUENCE
SRSASN_CODE ric_action_confirmed_for_removal_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_confirmed_for_removal_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ric_action_confirmed_for_removal_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-FailedToBeAddedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_failed_to_be_added_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_failed_to_be_added_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_failed_to_be_added_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-FailedToBeModifiedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_failed_to_be_modified_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_failed_to_be_modified_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_failed_to_be_modified_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-FailedToBeRemovedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_failed_to_be_remd_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_failed_to_be_remd_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_failed_to_be_remd_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-ModifiedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_modified_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_modified_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ric_action_modified_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-NotAdmitted-Item ::= SEQUENCE
SRSASN_CODE ric_action_not_admitted_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_not_admitted_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_not_admitted_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-RefusedToBeModified-Item ::= SEQUENCE
SRSASN_CODE ric_action_refused_to_be_modified_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_refused_to_be_modified_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_refused_to_be_modified_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-RefusedToBeRemoved-Item ::= SEQUENCE
SRSASN_CODE ric_action_refused_to_be_remd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_refused_to_be_remd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_refused_to_be_remd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-RemovedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_remd_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_remd_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ric_action_remd_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-RequiredToBeModified-Item ::= SEQUENCE
SRSASN_CODE ric_action_required_to_be_modified_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_time_to_wait.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_required_to_be_modified_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_time_to_wait.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_required_to_be_modified_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_str("ricTimeToWait", ric_time_to_wait.to_string());
  j.end_obj();
}

// RICaction-RequiredToBeRemoved-Item ::= SEQUENCE
SRSASN_CODE ric_action_required_to_be_remd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_required_to_be_remd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_action_required_to_be_remd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-ToBeAddedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_to_be_added_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ric_subsequent_action_present, 1));

  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_action_type.pack(bref));
  HANDLE_CODE(ric_action_definition.pack(bref));
  HANDLE_CODE(pack_integer(bref, ric_action_execution_order, (uint16_t)0u, (uint16_t)255u, true, true));
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_added_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ric_subsequent_action_present, 1));

  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_action_type.unpack(bref));
  HANDLE_CODE(ric_action_definition.unpack(bref));
  HANDLE_CODE(unpack_integer(ric_action_execution_order, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ric_action_to_be_added_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_str("ricActionType", ric_action_type.to_string());
  j.write_str("ricActionDefinition", ric_action_definition.to_string());
  j.write_int("ricActionExecutionOrder", ric_action_execution_order);
  if (ric_subsequent_action_present) {
    j.write_fieldname("ricSubsequentAction");
    ric_subsequent_action.to_json(j);
  }
  j.end_obj();
}

// RICaction-ToBeModifiedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_to_be_modified_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ric_action_definition.size() > 0, 1));
  HANDLE_CODE(bref.pack(ric_action_execution_order_present, 1));
  HANDLE_CODE(bref.pack(ric_subsequent_action_present, 1));

  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ric_action_definition.size() > 0) {
    HANDLE_CODE(ric_action_definition.pack(bref));
  }
  if (ric_action_execution_order_present) {
    HANDLE_CODE(pack_integer(bref, ric_action_execution_order, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_modified_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ric_action_definition_present;
  HANDLE_CODE(bref.unpack(ric_action_definition_present, 1));
  HANDLE_CODE(bref.unpack(ric_action_execution_order_present, 1));
  HANDLE_CODE(bref.unpack(ric_subsequent_action_present, 1));

  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  if (ric_action_definition_present) {
    HANDLE_CODE(ric_action_definition.unpack(bref));
  }
  if (ric_action_execution_order_present) {
    HANDLE_CODE(unpack_integer(ric_action_execution_order, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ric_action_to_be_modified_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  if (ric_action_definition.size() > 0) {
    j.write_str("ricActionDefinition", ric_action_definition.to_string());
  }
  if (ric_action_execution_order_present) {
    j.write_int("ricActionExecutionOrder", ric_action_execution_order);
  }
  if (ric_subsequent_action_present) {
    j.write_fieldname("ricSubsequentAction");
    ric_subsequent_action.to_json(j);
  }
  j.end_obj();
}

// RICaction-ToBeRemovedForModification-Item ::= SEQUENCE
SRSASN_CODE ric_action_to_be_remd_for_mod_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_remd_for_mod_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ric_action_to_be_remd_for_mod_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-ToBeSetup-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {19};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 19 == id;
}
crit_e ric_action_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 19) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_to_be_setup_item_ies_o::value_c ric_action_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 19) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 19) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_to_be_setup_item_ies_o::value_c::types");
}

// RICrequestID ::= SEQUENCE
SRSASN_CODE ric_request_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_requestor_id, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(pack_integer(bref, ric_instance_id, (uint32_t)0u, (uint32_t)65535u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_request_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_requestor_id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(unpack_integer(ric_instance_id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));

  return SRSASN_SUCCESS;
}
void ric_request_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricRequestorID", ric_requestor_id);
  j.write_int("ricInstanceID", ric_instance_id);
  j.end_obj();
}

// TypeOfError ::= ENUMERATED
const char* type_of_error_opts::to_string() const
{
  static const char* names[] = {"not-understood", "missing"};
  return convert_enum_idx(names, 2, value, "type_of_error_e");
}

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
SRSASN_CODE crit_diagnostics_ie_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ie_crit.pack(bref));
  HANDLE_CODE(pack_integer(bref, ie_id, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(type_of_error.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE crit_diagnostics_ie_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ie_crit.unpack(bref));
  HANDLE_CODE(unpack_integer(ie_id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(type_of_error.unpack(bref));

  return SRSASN_SUCCESS;
}
void crit_diagnostics_ie_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("iECriticality", ie_crit.to_string());
  j.write_int("iE-ID", ie_id);
  j.write_str("typeOfError", type_of_error.to_string());
  j.end_obj();
}

// E2connectionSetupFailed-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_setup_failed_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {41};
  return map_enum_number(names, 1, idx, "id");
}
bool e2conn_setup_failed_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 41 == id;
}
crit_e e2conn_setup_failed_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 41) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2conn_setup_failed_item_ies_o::value_c e2conn_setup_failed_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 41) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2conn_setup_failed_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 41) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2conn_setup_failed_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2connectionSetupFailed-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2conn_setup_failed_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_setup_failed_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2conn_setup_failed_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2connectionSetupFailed-Item"};
  return convert_enum_idx(names, 1, value, "e2conn_setup_failed_item_ies_o::value_c::types");
}
uint8_t e2conn_setup_failed_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2conn_setup_failed_item_ies_o::value_c::types");
}

// E2connectionUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {43};
  return map_enum_number(names, 1, idx, "id");
}
bool e2conn_upd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 43 == id;
}
crit_e e2conn_upd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 43) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2conn_upd_item_ies_o::value_c e2conn_upd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 43) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 43) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2connectionUpdate-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2conn_upd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2connectionUpdate-Item"};
  return convert_enum_idx(names, 1, value, "e2conn_upd_item_ies_o::value_c::types");
}
uint8_t e2conn_upd_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2conn_upd_item_ies_o::value_c::types");
}

// E2connectionUpdateRemove-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_rem_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {47};
  return map_enum_number(names, 1, idx, "id");
}
bool e2conn_upd_rem_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 47 == id;
}
crit_e e2conn_upd_rem_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 47) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2conn_upd_rem_item_ies_o::value_c e2conn_upd_rem_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 47) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_rem_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 47) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_rem_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2connectionUpdateRemove-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2conn_upd_rem_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_rem_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_rem_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2connectionUpdateRemove-Item"};
  return convert_enum_idx(names, 1, value, "e2conn_upd_rem_item_ies_o::value_c::types");
}
uint8_t e2conn_upd_rem_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2conn_upd_rem_item_ies_o::value_c::types");
}

// E2nodeComponentConfigAdditionAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_addition_ack_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {53};
  return map_enum_number(names, 1, idx, "id");
}
bool e2node_component_cfg_addition_ack_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 53 == id;
}
crit_e e2node_component_cfg_addition_ack_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 53) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2node_component_cfg_addition_ack_item_ies_o::value_c
e2node_component_cfg_addition_ack_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 53) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_addition_ack_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 53) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_addition_ack_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigAdditionAck-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_addition_ack_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_ack_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_addition_ack_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2nodeComponentConfigAdditionAck-Item"};
  return convert_enum_idx(names, 1, value, "e2node_component_cfg_addition_ack_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_addition_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_addition_ack_item_ies_o::value_c::types");
}

// E2nodeComponentConfigAddition-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_addition_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {51};
  return map_enum_number(names, 1, idx, "id");
}
bool e2node_component_cfg_addition_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 51 == id;
}
crit_e e2node_component_cfg_addition_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 51) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2node_component_cfg_addition_item_ies_o::value_c
e2node_component_cfg_addition_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 51) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_addition_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 51) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_addition_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigAddition-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_addition_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_addition_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2nodeComponentConfigAddition-Item"};
  return convert_enum_idx(names, 1, value, "e2node_component_cfg_addition_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_addition_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_addition_item_ies_o::value_c::types");
}

// E2nodeComponentConfigRemovalAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_removal_ack_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57};
  return map_enum_number(names, 1, idx, "id");
}
bool e2node_component_cfg_removal_ack_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 57 == id;
}
crit_e e2node_component_cfg_removal_ack_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 57) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2node_component_cfg_removal_ack_item_ies_o::value_c
e2node_component_cfg_removal_ack_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 57) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_removal_ack_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 57) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_removal_ack_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigRemovalAck-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_removal_ack_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_ack_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_removal_ack_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2nodeComponentConfigRemovalAck-Item"};
  return convert_enum_idx(names, 1, value, "e2node_component_cfg_removal_ack_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_removal_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_removal_ack_item_ies_o::value_c::types");
}

// E2nodeComponentConfigRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_removal_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {55};
  return map_enum_number(names, 1, idx, "id");
}
bool e2node_component_cfg_removal_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 55 == id;
}
crit_e e2node_component_cfg_removal_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 55) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2node_component_cfg_removal_item_ies_o::value_c e2node_component_cfg_removal_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 55) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_removal_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 55) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_removal_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigRemoval-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_removal_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_removal_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2nodeComponentConfigRemoval-Item"};
  return convert_enum_idx(names, 1, value, "e2node_component_cfg_removal_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_removal_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_removal_item_ies_o::value_c::types");
}

// E2nodeComponentConfigUpdateAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_upd_ack_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {36};
  return map_enum_number(names, 1, idx, "id");
}
bool e2node_component_cfg_upd_ack_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 36 == id;
}
crit_e e2node_component_cfg_upd_ack_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 36) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2node_component_cfg_upd_ack_item_ies_o::value_c e2node_component_cfg_upd_ack_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 36) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_upd_ack_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 36) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_upd_ack_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigUpdateAck-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_upd_ack_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_ack_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_upd_ack_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2nodeComponentConfigUpdateAck-Item"};
  return convert_enum_idx(names, 1, value, "e2node_component_cfg_upd_ack_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_upd_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_upd_ack_item_ies_o::value_c::types");
}

// E2nodeComponentConfigUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_upd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {34};
  return map_enum_number(names, 1, idx, "id");
}
bool e2node_component_cfg_upd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 34 == id;
}
crit_e e2node_component_cfg_upd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 34) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2node_component_cfg_upd_item_ies_o::value_c e2node_component_cfg_upd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 34) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_upd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 34) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_upd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigUpdate-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_upd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_upd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2nodeComponentConfigUpdate-Item"};
  return convert_enum_idx(names, 1, value, "e2node_component_cfg_upd_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_upd_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_upd_item_ies_o::value_c::types");
}

// E2nodeTNLassociationRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_tnl_assoc_removal_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {59};
  return map_enum_number(names, 1, idx, "id");
}
bool e2node_tnl_assoc_removal_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 59 == id;
}
crit_e e2node_tnl_assoc_removal_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 59) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2node_tnl_assoc_removal_item_ies_o::value_c e2node_tnl_assoc_removal_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 59) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_tnl_assoc_removal_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 59) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_tnl_assoc_removal_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeTNLassociationRemoval-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_tnl_assoc_removal_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_tnl_assoc_removal_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_tnl_assoc_removal_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E2nodeTNLassociationRemoval-Item"};
  return convert_enum_idx(names, 1, value, "e2node_tnl_assoc_removal_item_ies_o::value_c::types");
}
uint8_t e2node_tnl_assoc_removal_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_tnl_assoc_removal_item_ies_o::value_c::types");
}

template struct asn1::protocol_ie_single_container_s<ric_action_to_be_setup_item_ies_o>;

// RANfunctionIDcause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ran_function_id_cause_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {7};
  return map_enum_number(names, 1, idx, "id");
}
bool ran_function_id_cause_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 7 == id;
}
crit_e ran_function_id_cause_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 7) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ran_function_id_cause_item_ies_o::value_c ran_function_id_cause_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 7) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ran_function_id_cause_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 7) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ran_function_id_cause_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RANfunctionIDcause-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ran_function_id_cause_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_id_cause_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ran_function_id_cause_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RANfunctionIDcause-Item"};
  return convert_enum_idx(names, 1, value, "ran_function_id_cause_item_ies_o::value_c::types");
}

// RANfunctionID-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ran_function_id_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {6};
  return map_enum_number(names, 1, idx, "id");
}
bool ran_function_id_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 6 == id;
}
crit_e ran_function_id_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 6) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ran_function_id_item_ies_o::value_c ran_function_id_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 6) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ran_function_id_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 6) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ran_function_id_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RANfunctionID-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ran_function_id_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_id_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ran_function_id_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RANfunctionID-Item"};
  return convert_enum_idx(names, 1, value, "ran_function_id_item_ies_o::value_c::types");
}

// RANfunction-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ran_function_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {8};
  return map_enum_number(names, 1, idx, "id");
}
bool ran_function_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 8 == id;
}
crit_e ran_function_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 8) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ran_function_item_ies_o::value_c ran_function_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 8) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ran_function_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 8) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ran_function_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RANfunction-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ran_function_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ran_function_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ran_function_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RANfunction-Item"};
  return convert_enum_idx(names, 1, value, "ran_function_item_ies_o::value_c::types");
}

// RICaction-AddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_added_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {80};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_added_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 80 == id;
}
crit_e ric_action_added_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 80) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_added_for_mod_item_ies_o::value_c ric_action_added_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 80) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_added_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 80) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_added_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-AddedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_added_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_added_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_added_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-AddedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_added_for_mod_item_ies_o::value_c::types");
}

// RICaction-Admitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_admitted_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {14};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_admitted_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 14 == id;
}
crit_e ric_action_admitted_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 14) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_admitted_item_ies_o::value_c ric_action_admitted_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 14) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_admitted_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 14) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_admitted_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-Admitted-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_admitted_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_admitted_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_admitted_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-Admitted-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_admitted_item_ies_o::value_c::types");
}

// RICaction-ConfirmedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_confirmed_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {88};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_confirmed_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 88 == id;
}
crit_e ric_action_confirmed_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 88) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_confirmed_for_mod_item_ies_o::value_c ric_action_confirmed_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 88) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_confirmed_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 88) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_confirmed_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ConfirmedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_confirmed_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_confirmed_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_confirmed_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-ConfirmedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_confirmed_for_mod_item_ies_o::value_c::types");
}

// RICaction-ConfirmedForRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_confirmed_for_removal_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {92};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_confirmed_for_removal_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 92 == id;
}
crit_e ric_action_confirmed_for_removal_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 92) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_confirmed_for_removal_item_ies_o::value_c
ric_action_confirmed_for_removal_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 92) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_confirmed_for_removal_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 92) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_confirmed_for_removal_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ConfirmedForRemoval-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_confirmed_for_removal_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_confirmed_for_removal_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_confirmed_for_removal_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-ConfirmedForRemoval-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_confirmed_for_removal_item_ies_o::value_c::types");
}

// RICaction-FailedToBeAddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_failed_to_be_added_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {82};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_failed_to_be_added_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 82 == id;
}
crit_e ric_action_failed_to_be_added_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 82) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_failed_to_be_added_for_mod_item_ies_o::value_c
ric_action_failed_to_be_added_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 82) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_failed_to_be_added_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 82) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-FailedToBeAddedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-FailedToBeAddedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types");
}

// RICaction-FailedToBeModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_failed_to_be_modified_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {78};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_failed_to_be_modified_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 78 == id;
}
crit_e ric_action_failed_to_be_modified_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 78) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c
ric_action_failed_to_be_modified_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 78) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_failed_to_be_modified_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 78) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-FailedToBeModifiedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-FailedToBeModifiedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types");
}

// RICaction-FailedToBeRemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_failed_to_be_remd_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {74};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_failed_to_be_remd_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 74 == id;
}
crit_e ric_action_failed_to_be_remd_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 74) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c
ric_action_failed_to_be_remd_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 74) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_failed_to_be_remd_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 74) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-FailedToBeRemovedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-FailedToBeRemovedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types");
}

// RICaction-ModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_modified_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {76};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_modified_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 76 == id;
}
crit_e ric_action_modified_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 76) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_modified_for_mod_item_ies_o::value_c ric_action_modified_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 76) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_modified_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 76) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_modified_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ModifiedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_modified_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_modified_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_modified_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-ModifiedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_modified_for_mod_item_ies_o::value_c::types");
}

// RICaction-NotAdmitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_not_admitted_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {16};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_not_admitted_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 16 == id;
}
crit_e ric_action_not_admitted_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 16) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_not_admitted_item_ies_o::value_c ric_action_not_admitted_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 16) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_not_admitted_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 16) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_not_admitted_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-NotAdmitted-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_not_admitted_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_not_admitted_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_not_admitted_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-NotAdmitted-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_not_admitted_item_ies_o::value_c::types");
}

// RICaction-RefusedToBeModified-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_refused_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {90};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_refused_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 90 == id;
}
crit_e ric_action_refused_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 90) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_refused_to_be_modified_item_ies_o::value_c
ric_action_refused_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 90) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_refused_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 90) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_refused_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-RefusedToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_refused_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_refused_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_refused_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-RefusedToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_refused_to_be_modified_item_ies_o::value_c::types");
}

// RICaction-RefusedToBeRemoved-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_refused_to_be_remd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {94};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_refused_to_be_remd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 94 == id;
}
crit_e ric_action_refused_to_be_remd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 94) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_refused_to_be_remd_item_ies_o::value_c
ric_action_refused_to_be_remd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 94) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_refused_to_be_remd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 94) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_refused_to_be_remd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-RefusedToBeRemoved-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_refused_to_be_remd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_refused_to_be_remd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_refused_to_be_remd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-RefusedToBeRemoved-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_refused_to_be_remd_item_ies_o::value_c::types");
}

// RICaction-RemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_remd_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {72};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_remd_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 72 == id;
}
crit_e ric_action_remd_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 72) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_remd_for_mod_item_ies_o::value_c ric_action_remd_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 72) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_remd_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 72) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_remd_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-RemovedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_remd_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_remd_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_remd_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-RemovedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_remd_for_mod_item_ies_o::value_c::types");
}

// RICaction-RequiredToBeModified-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_required_to_be_modified_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {84};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_required_to_be_modified_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 84 == id;
}
crit_e ric_action_required_to_be_modified_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 84) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_required_to_be_modified_item_ies_o::value_c
ric_action_required_to_be_modified_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 84) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_required_to_be_modified_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 84) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_required_to_be_modified_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-RequiredToBeModified-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_required_to_be_modified_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_required_to_be_modified_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_required_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-RequiredToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_required_to_be_modified_item_ies_o::value_c::types");
}

// RICaction-RequiredToBeRemoved-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_required_to_be_remd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {86};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_required_to_be_remd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 86 == id;
}
crit_e ric_action_required_to_be_remd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 86) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_required_to_be_remd_item_ies_o::value_c
ric_action_required_to_be_remd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 86) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_required_to_be_remd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 86) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_required_to_be_remd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-RequiredToBeRemoved-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_required_to_be_remd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_required_to_be_remd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_required_to_be_remd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-RequiredToBeRemoved-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_required_to_be_remd_item_ies_o::value_c::types");
}

// RICaction-ToBeAddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_to_be_added_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {70};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_to_be_added_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 70 == id;
}
crit_e ric_action_to_be_added_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 70) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_to_be_added_for_mod_item_ies_o::value_c
ric_action_to_be_added_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 70) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_to_be_added_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 70) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_to_be_added_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ToBeAddedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_to_be_added_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_added_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_to_be_added_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-ToBeAddedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_to_be_added_for_mod_item_ies_o::value_c::types");
}

// RICaction-ToBeModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_to_be_modified_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {68};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_to_be_modified_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 68 == id;
}
crit_e ric_action_to_be_modified_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 68) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_to_be_modified_for_mod_item_ies_o::value_c
ric_action_to_be_modified_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 68) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_to_be_modified_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 68) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_to_be_modified_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ToBeModifiedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_to_be_modified_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_modified_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_to_be_modified_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-ToBeModifiedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_to_be_modified_for_mod_item_ies_o::value_c::types");
}

// RICaction-ToBeRemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_action_to_be_remd_for_mod_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {66};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_action_to_be_remd_for_mod_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 66 == id;
}
crit_e ric_action_to_be_remd_for_mod_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 66) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_action_to_be_remd_for_mod_item_ies_o::value_c
ric_action_to_be_remd_for_mod_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 66) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_action_to_be_remd_for_mod_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 66) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_action_to_be_remd_for_mod_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ToBeRemovedForModification-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_action_to_be_remd_for_mod_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_action_to_be_remd_for_mod_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_action_to_be_remd_for_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICaction-ToBeRemovedForModification-Item"};
  return convert_enum_idx(names, 1, value, "ric_action_to_be_remd_for_mod_item_ies_o::value_c::types");
}

// RICsubscription-withCause-Item ::= SEQUENCE
SRSASN_CODE ric_sub_with_cause_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ric_request_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_with_cause_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ric_request_id.unpack(bref));
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ric_sub_with_cause_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ricRequestID");
  ric_request_id.to_json(j);
  j.write_int("ranFunctionID", ran_function_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// GlobalE2node-en-gNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_en_gnb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(en_gnb_cu_up_id_present, 1));
  HANDLE_CODE(bref.pack(en_gnb_du_id_present, 1));

  HANDLE_CODE(global_en_gnb_id.pack(bref));
  if (en_gnb_cu_up_id_present) {
    HANDLE_CODE(pack_integer(bref, en_gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (en_gnb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, en_gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_en_gnb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(en_gnb_cu_up_id_present, 1));
  HANDLE_CODE(bref.unpack(en_gnb_du_id_present, 1));

  HANDLE_CODE(global_en_gnb_id.unpack(bref));
  if (en_gnb_cu_up_id_present) {
    HANDLE_CODE(unpack_integer(en_gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (en_gnb_du_id_present) {
    HANDLE_CODE(unpack_integer(en_gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
void global_e2node_en_gnb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-en-gNB-ID");
  global_en_gnb_id.to_json(j);
  if (en_gnb_cu_up_id_present) {
    j.write_int("en-gNB-CU-UP-ID", en_gnb_cu_up_id);
  }
  if (en_gnb_du_id_present) {
    j.write_int("en-gNB-DU-ID", en_gnb_du_id);
  }
  j.end_obj();
}

// GlobalE2node-eNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_enb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_enb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_e2node_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-eNB-ID");
  global_enb_id.to_json(j);
  j.end_obj();
}

// GlobalE2node-gNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_gnb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(global_en_gnb_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_up_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_du_id_present, 1));

  HANDLE_CODE(global_gnb_id.pack(bref));
  if (global_en_gnb_id_present) {
    HANDLE_CODE(global_en_gnb_id.pack(bref));
  }
  if (gnb_cu_up_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_gnb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(global_en_gnb_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_up_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_du_id_present, 1));

  HANDLE_CODE(global_gnb_id.unpack(bref));
  if (global_en_gnb_id_present) {
    HANDLE_CODE(global_en_gnb_id.unpack(bref));
  }
  if (gnb_cu_up_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
void global_e2node_gnb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-gNB-ID");
  global_gnb_id.to_json(j);
  if (global_en_gnb_id_present) {
    j.write_fieldname("global-en-gNB-ID");
    global_en_gnb_id.to_json(j);
  }
  if (gnb_cu_up_id_present) {
    j.write_int("gNB-CU-UP-ID", gnb_cu_up_id);
  }
  if (gnb_du_id_present) {
    j.write_int("gNB-DU-ID", gnb_du_id);
  }
  j.end_obj();
}

// GlobalE2node-ng-eNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_ng_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(global_enb_id_present, 1));
  HANDLE_CODE(bref.pack(ng_enb_du_id_present, 1));

  HANDLE_CODE(global_ng_enb_id.pack(bref));
  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.pack(bref));
  }
  if (ng_enb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, ng_enb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_ng_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(global_enb_id_present, 1));
  HANDLE_CODE(bref.unpack(ng_enb_du_id_present, 1));

  HANDLE_CODE(global_ng_enb_id.unpack(bref));
  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.unpack(bref));
  }
  if (ng_enb_du_id_present) {
    HANDLE_CODE(unpack_integer(ng_enb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
void global_e2node_ng_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-ng-eNB-ID");
  global_ng_enb_id.to_json(j);
  if (global_enb_id_present) {
    j.write_fieldname("global-eNB-ID");
    global_enb_id.to_json(j);
  }
  if (ng_enb_du_id_present) {
    j.write_int("ngENB-DU-ID", ng_enb_du_id);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_single_container_s<e2conn_setup_failed_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2conn_upd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2conn_upd_rem_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_addition_ack_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_addition_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_removal_ack_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_removal_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_upd_ack_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_upd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_tnl_assoc_removal_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ran_function_id_cause_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ran_function_id_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ran_function_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_added_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_admitted_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_confirmed_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_confirmed_for_removal_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_failed_to_be_added_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_failed_to_be_modified_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_failed_to_be_remd_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_modified_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_not_admitted_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_refused_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_refused_to_be_remd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_remd_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_required_to_be_modified_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_required_to_be_remd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_to_be_added_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_to_be_modified_for_mod_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ric_action_to_be_remd_for_mod_item_ies_o>;

// RICsubscription-withCause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_with_cause_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {61};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_sub_with_cause_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 61 == id;
}
crit_e ric_sub_with_cause_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 61) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_sub_with_cause_item_ies_o::value_c ric_sub_with_cause_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 61) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_with_cause_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 61) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_with_cause_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICsubscription-withCause-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ric_sub_with_cause_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_with_cause_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ric_sub_with_cause_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscription-withCause-Item"};
  return convert_enum_idx(names, 1, value, "ric_sub_with_cause_item_ies_o::value_c::types");
}

// TriggeringMessage ::= ENUMERATED
const char* trigger_msg_opts::to_string() const
{
  static const char* names[] = {"initiating-message", "successful-outcome", "unsuccessfull-outcome"};
  return convert_enum_idx(names, 3, value, "trigger_msg_e");
}

// CriticalityDiagnostics ::= SEQUENCE
SRSASN_CODE crit_diagnostics_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(proc_code_present, 1));
  HANDLE_CODE(bref.pack(trigger_msg_present, 1));
  HANDLE_CODE(bref.pack(proc_crit_present, 1));
  HANDLE_CODE(bref.pack(ric_requestor_id_present, 1));
  HANDLE_CODE(bref.pack(ies_crit_diagnostics.size() > 0, 1));

  if (proc_code_present) {
    HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (trigger_msg_present) {
    HANDLE_CODE(trigger_msg.pack(bref));
  }
  if (proc_crit_present) {
    HANDLE_CODE(proc_crit.pack(bref));
  }
  if (ric_requestor_id_present) {
    HANDLE_CODE(ric_requestor_id.pack(bref));
  }
  if (ies_crit_diagnostics.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ies_crit_diagnostics, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE crit_diagnostics_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(proc_code_present, 1));
  HANDLE_CODE(bref.unpack(trigger_msg_present, 1));
  HANDLE_CODE(bref.unpack(proc_crit_present, 1));
  HANDLE_CODE(bref.unpack(ric_requestor_id_present, 1));
  bool ies_crit_diagnostics_present;
  HANDLE_CODE(bref.unpack(ies_crit_diagnostics_present, 1));

  if (proc_code_present) {
    HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (trigger_msg_present) {
    HANDLE_CODE(trigger_msg.unpack(bref));
  }
  if (proc_crit_present) {
    HANDLE_CODE(proc_crit.unpack(bref));
  }
  if (ric_requestor_id_present) {
    HANDLE_CODE(ric_requestor_id.unpack(bref));
  }
  if (ies_crit_diagnostics_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ies_crit_diagnostics, bref, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
void crit_diagnostics_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (proc_code_present) {
    j.write_int("procedureCode", proc_code);
  }
  if (trigger_msg_present) {
    j.write_str("triggeringMessage", trigger_msg.to_string());
  }
  if (proc_crit_present) {
    j.write_str("procedureCriticality", proc_crit.to_string());
  }
  if (ric_requestor_id_present) {
    j.write_fieldname("ricRequestorID");
    ric_requestor_id.to_json(j);
  }
  if (ies_crit_diagnostics.size() > 0) {
    j.start_array("iEsCriticalityDiagnostics");
    for (const auto& e1 : ies_crit_diagnostics) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// GlobalE2node-ID ::= CHOICE
void global_e2node_id_c::destroy_()
{
  switch (type_) {
    case types::gnb:
      c.destroy<global_e2node_gnb_id_s>();
      break;
    case types::en_gnb:
      c.destroy<global_e2node_en_gnb_id_s>();
      break;
    case types::ng_enb:
      c.destroy<global_e2node_ng_enb_id_s>();
      break;
    case types::enb:
      c.destroy<global_e2node_enb_id_s>();
      break;
    default:
      break;
  }
}
void global_e2node_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb:
      c.init<global_e2node_gnb_id_s>();
      break;
    case types::en_gnb:
      c.init<global_e2node_en_gnb_id_s>();
      break;
    case types::ng_enb:
      c.init<global_e2node_ng_enb_id_s>();
      break;
    case types::enb:
      c.init<global_e2node_enb_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }
}
global_e2node_id_c::global_e2node_id_c(const global_e2node_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb:
      c.init(other.c.get<global_e2node_gnb_id_s>());
      break;
    case types::en_gnb:
      c.init(other.c.get<global_e2node_en_gnb_id_s>());
      break;
    case types::ng_enb:
      c.init(other.c.get<global_e2node_ng_enb_id_s>());
      break;
    case types::enb:
      c.init(other.c.get<global_e2node_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }
}
global_e2node_id_c& global_e2node_id_c::operator=(const global_e2node_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb:
      c.set(other.c.get<global_e2node_gnb_id_s>());
      break;
    case types::en_gnb:
      c.set(other.c.get<global_e2node_en_gnb_id_s>());
      break;
    case types::ng_enb:
      c.set(other.c.get<global_e2node_ng_enb_id_s>());
      break;
    case types::enb:
      c.set(other.c.get<global_e2node_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }

  return *this;
}
global_e2node_gnb_id_s& global_e2node_id_c::set_gnb()
{
  set(types::gnb);
  return c.get<global_e2node_gnb_id_s>();
}
global_e2node_en_gnb_id_s& global_e2node_id_c::set_en_gnb()
{
  set(types::en_gnb);
  return c.get<global_e2node_en_gnb_id_s>();
}
global_e2node_ng_enb_id_s& global_e2node_id_c::set_ng_enb()
{
  set(types::ng_enb);
  return c.get<global_e2node_ng_enb_id_s>();
}
global_e2node_enb_id_s& global_e2node_id_c::set_enb()
{
  set(types::enb);
  return c.get<global_e2node_enb_id_s>();
}
void global_e2node_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb:
      j.write_fieldname("gNB");
      c.get<global_e2node_gnb_id_s>().to_json(j);
      break;
    case types::en_gnb:
      j.write_fieldname("en-gNB");
      c.get<global_e2node_en_gnb_id_s>().to_json(j);
      break;
    case types::ng_enb:
      j.write_fieldname("ng-eNB");
      c.get<global_e2node_ng_enb_id_s>().to_json(j);
      break;
    case types::enb:
      j.write_fieldname("eNB");
      c.get<global_e2node_enb_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }
  j.end_obj();
}
SRSASN_CODE global_e2node_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<global_e2node_gnb_id_s>().pack(bref));
      break;
    case types::en_gnb:
      HANDLE_CODE(c.get<global_e2node_en_gnb_id_s>().pack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<global_e2node_ng_enb_id_s>().pack(bref));
      break;
    case types::enb:
      HANDLE_CODE(c.get<global_e2node_enb_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<global_e2node_gnb_id_s>().unpack(bref));
      break;
    case types::en_gnb:
      HANDLE_CODE(c.get<global_e2node_en_gnb_id_s>().unpack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<global_e2node_ng_enb_id_s>().unpack(bref));
      break;
    case types::enb:
      HANDLE_CODE(c.get<global_e2node_enb_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* global_e2node_id_c::types_opts::to_string() const
{
  static const char* names[] = {"gNB", "en-gNB", "ng-eNB", "eNB"};
  return convert_enum_idx(names, 4, value, "global_e2node_id_c::types");
}

// GlobalRIC-ID ::= SEQUENCE
SRSASN_CODE global_ric_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(ric_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_ric_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(ric_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_ric_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_str("ric-ID", ric_id.to_string());
  j.end_obj();
}

template struct asn1::protocol_ie_single_container_s<ric_sub_with_cause_item_ies_o>;

// RICcontrolAckRequest ::= ENUMERATED
const char* ric_ctrl_ack_request_opts::to_string() const
{
  static const char* names[] = {"noAck", "ack"};
  return convert_enum_idx(names, 2, value, "ric_ctrl_ack_request_e");
}

// RICindicationType ::= ENUMERATED
const char* ric_ind_type_opts::to_string() const
{
  static const char* names[] = {"report", "insert"};
  return convert_enum_idx(names, 2, value, "ric_ind_type_e");
}

// RICsubscriptionDetails ::= SEQUENCE
SRSASN_CODE ric_sub_details_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ric_event_trigger_definition.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, ric_action_to_be_setup_list, 1, 16, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_details_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ric_event_trigger_definition.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(ric_action_to_be_setup_list, bref, 1, 16, true));

  return SRSASN_SUCCESS;
}
void ric_sub_details_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ricEventTriggerDefinition", ric_event_trigger_definition.to_string());
  j.start_array("ricAction-ToBeSetup-List");
  for (const auto& e1 : ric_action_to_be_setup_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// TimeToWait ::= ENUMERATED
const char* time_to_wait_opts::to_string() const
{
  static const char* names[] = {"v1s", "v2s", "v5s", "v10s", "v20s", "v60s"};
  return convert_enum_idx(names, 6, value, "time_to_wait_e");
}
uint8_t time_to_wait_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 5, 10, 20, 60};
  return map_enum_number(numbers, 6, value, "time_to_wait_e");
}

// E2RemovalFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2_removal_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 1, 2};
  return map_enum_number(names, 3, idx, "id");
}
bool e2_removal_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 1, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2_removal_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2_removal_fail_ies_o::value_c e2_removal_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2_removal_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2_removal_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
  }
}
uint16_t& e2_removal_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2_removal_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& e2_removal_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2_removal_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2_removal_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& e2_removal_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2_removal_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_removal_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_removal_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "e2_removal_fail_ies_o::value_c::types");
}
uint8_t e2_removal_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e2_removal_fail_ies_o::value_c::types");
}

// E2RemovalResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2_removal_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 2};
  return map_enum_number(names, 2, idx, "id");
}
bool e2_removal_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2_removal_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2_removal_resp_ies_o::value_c e2_removal_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2_removal_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2_removal_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
  }
}
uint16_t& e2_removal_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& e2_removal_resp_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2_removal_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& e2_removal_resp_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2_removal_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_removal_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_removal_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "e2_removal_resp_ies_o::value_c::types");
}
uint8_t e2_removal_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e2_removal_resp_ies_o::value_c::types");
}

// E2connectionUpdateAck-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 39, 40};
  return map_enum_number(names, 3, idx, "id");
}
bool e2conn_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 39, 40};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2conn_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 39:
      return crit_e::reject;
    case 40:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2conn_upd_ack_ies_o::value_c e2conn_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 39:
      ret.set(value_c::types::e2conn_setup);
      break;
    case 40:
      ret.set(value_c::types::e2conn_setup_failed);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 39:
      return presence_e::optional;
    case 40:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::e2conn_setup:
      c = e2conn_upd_list_l{};
      break;
    case types::e2conn_setup_failed:
      c = e2conn_setup_failed_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
  }
}
uint16_t& e2conn_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
e2conn_upd_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup()
{
  assert_choice_type(types::e2conn_setup, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
e2conn_setup_failed_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup_failed()
{
  assert_choice_type(types::e2conn_setup_failed, type_, "Value");
  return c.get<e2conn_setup_failed_list_l>();
}
const uint16_t& e2conn_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const e2conn_upd_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup() const
{
  assert_choice_type(types::e2conn_setup, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
const e2conn_setup_failed_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup_failed() const
{
  assert_choice_type(types::e2conn_setup_failed, type_, "Value");
  return c.get<e2conn_setup_failed_list_l>();
}
void e2conn_upd_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::e2conn_setup:
      j.start_array("E2connectionUpdate-List");
      for (const auto& e1 : c.get<e2conn_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2conn_setup_failed:
      j.start_array("E2connectionSetupFailed-List");
      for (const auto& e1 : c.get<e2conn_setup_failed_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2conn_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_list_l>(), 1, 32, true));
      break;
    case types::e2conn_setup_failed:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_setup_failed_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_list_l>(), bref, 1, 32, true));
      break;
    case types::e2conn_setup_failed:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_setup_failed_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "E2connectionUpdate-List", "E2connectionSetupFailed-List"};
  return convert_enum_idx(names, 3, value, "e2conn_upd_ack_ies_o::value_c::types");
}

// E2connectionUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 1, 31, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool e2conn_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 1, 31, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2conn_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2conn_upd_fail_ies_o::value_c e2conn_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::time_to_wait:
      c = time_to_wait_e{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
  }
}
uint16_t& e2conn_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2conn_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& e2conn_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& e2conn_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2conn_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2conn_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& e2conn_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& e2conn_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2conn_upd_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2conn_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "e2conn_upd_fail_ies_o::value_c::types");
}
uint8_t e2conn_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e2conn_upd_fail_ies_o::value_c::types");
}

// E2connectionUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 44, 46, 45};
  return map_enum_number(names, 4, idx, "id");
}
bool e2conn_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 44, 46, 45};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2conn_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 44:
      return crit_e::reject;
    case 46:
      return crit_e::reject;
    case 45:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2conn_upd_ies_o::value_c e2conn_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 44:
      ret.set(value_c::types::e2conn_upd_add);
      break;
    case 46:
      ret.set(value_c::types::e2conn_upd_rem);
      break;
    case 45:
      ret.set(value_c::types::e2conn_upd_modify);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 44:
      return presence_e::optional;
    case 46:
      return presence_e::optional;
    case 45:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::e2conn_upd_add:
      c = e2conn_upd_list_l{};
      break;
    case types::e2conn_upd_rem:
      c = e2conn_upd_rem_list_l{};
      break;
    case types::e2conn_upd_modify:
      c = e2conn_upd_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
  }
}
uint16_t& e2conn_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_add()
{
  assert_choice_type(types::e2conn_upd_add, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
e2conn_upd_rem_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_rem()
{
  assert_choice_type(types::e2conn_upd_rem, type_, "Value");
  return c.get<e2conn_upd_rem_list_l>();
}
e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_modify()
{
  assert_choice_type(types::e2conn_upd_modify, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
const uint16_t& e2conn_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_add() const
{
  assert_choice_type(types::e2conn_upd_add, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
const e2conn_upd_rem_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_rem() const
{
  assert_choice_type(types::e2conn_upd_rem, type_, "Value");
  return c.get<e2conn_upd_rem_list_l>();
}
const e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_modify() const
{
  assert_choice_type(types::e2conn_upd_modify, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
void e2conn_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::e2conn_upd_add:
      j.start_array("E2connectionUpdate-List");
      for (const auto& e1 : c.get<e2conn_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2conn_upd_rem:
      j.start_array("E2connectionUpdateRemove-List");
      for (const auto& e1 : c.get<e2conn_upd_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2conn_upd_modify:
      j.start_array("E2connectionUpdate-List");
      for (const auto& e1 : c.get<e2conn_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2conn_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_upd_add:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_list_l>(), 1, 32, true));
      break;
    case types::e2conn_upd_rem:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_rem_list_l>(), 1, 32, true));
      break;
    case types::e2conn_upd_modify:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_upd_add:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_list_l>(), bref, 1, 32, true));
      break;
    case types::e2conn_upd_rem:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_rem_list_l>(), bref, 1, 32, true));
      break;
    case types::e2conn_upd_modify:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "E2connectionUpdate-List", "E2connectionUpdateRemove-List", "E2connectionUpdate-List"};
  return convert_enum_idx(names, 4, value, "e2conn_upd_ies_o::value_c::types");
}

// E2nodeConfigurationUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_cfg_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 52, 35, 56};
  return map_enum_number(names, 4, idx, "id");
}
bool e2node_cfg_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 52, 35, 56};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2node_cfg_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 52:
      return crit_e::reject;
    case 35:
      return crit_e::reject;
    case 56:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2node_cfg_upd_ack_ies_o::value_c e2node_cfg_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 52:
      ret.set(value_c::types::e2node_component_cfg_addition_ack);
      break;
    case 35:
      ret.set(value_c::types::e2node_component_cfg_upd_ack);
      break;
    case 56:
      ret.set(value_c::types::e2node_component_cfg_removal_ack);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_cfg_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 52:
      return presence_e::optional;
    case 35:
      return presence_e::optional;
    case 56:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2node_cfg_upd_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::e2node_component_cfg_addition_ack:
      c = e2node_component_cfg_addition_ack_list_l{};
      break;
    case types::e2node_component_cfg_upd_ack:
      c = e2node_component_cfg_upd_ack_list_l{};
      break;
    case types::e2node_component_cfg_removal_ack:
      c = e2node_component_cfg_removal_ack_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
  }
}
uint16_t& e2node_cfg_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
e2node_component_cfg_addition_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_addition_ack()
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
e2node_component_cfg_upd_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_upd_ack()
{
  assert_choice_type(types::e2node_component_cfg_upd_ack, type_, "Value");
  return c.get<e2node_component_cfg_upd_ack_list_l>();
}
e2node_component_cfg_removal_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_removal_ack()
{
  assert_choice_type(types::e2node_component_cfg_removal_ack, type_, "Value");
  return c.get<e2node_component_cfg_removal_ack_list_l>();
}
const uint16_t& e2node_cfg_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const e2node_component_cfg_addition_ack_list_l&
e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_addition_ack() const
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
const e2node_component_cfg_upd_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_upd_ack() const
{
  assert_choice_type(types::e2node_component_cfg_upd_ack, type_, "Value");
  return c.get<e2node_component_cfg_upd_ack_list_l>();
}
const e2node_component_cfg_removal_ack_list_l&
e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_removal_ack() const
{
  assert_choice_type(types::e2node_component_cfg_removal_ack, type_, "Value");
  return c.get<e2node_component_cfg_removal_ack_list_l>();
}
void e2node_cfg_upd_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::e2node_component_cfg_addition_ack:
      j.start_array("E2nodeComponentConfigAdditionAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_upd_ack:
      j.start_array("E2nodeComponentConfigUpdateAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_upd_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_removal_ack:
      j.start_array("E2nodeComponentConfigRemovalAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_removal_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_cfg_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_ack_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_upd_ack_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_removal_ack_list_l>(), 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_ack_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_upd_ack_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_removal_ack_list_l>(), bref, 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "E2nodeComponentConfigAdditionAck-List",
                                "E2nodeComponentConfigUpdateAck-List",
                                "E2nodeComponentConfigRemovalAck-List"};
  return convert_enum_idx(names, 4, value, "e2node_cfg_upd_ack_ies_o::value_c::types");
}

// E2nodeConfigurationUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_cfg_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 1, 31, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool e2node_cfg_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 1, 31, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2node_cfg_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2node_cfg_upd_fail_ies_o::value_c e2node_cfg_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_cfg_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2node_cfg_upd_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::time_to_wait:
      c = time_to_wait_e{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
  }
}
uint16_t& e2node_cfg_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2node_cfg_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& e2node_cfg_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& e2node_cfg_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2node_cfg_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2node_cfg_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& e2node_cfg_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& e2node_cfg_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2node_cfg_upd_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_cfg_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "e2node_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t e2node_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e2node_cfg_upd_fail_ies_o::value_c::types");
}

// E2nodeConfigurationUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_cfg_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 3, 50, 33, 54, 58};
  return map_enum_number(names, 6, idx, "id");
}
bool e2node_cfg_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 3, 50, 33, 54, 58};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2node_cfg_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 50:
      return crit_e::reject;
    case 33:
      return crit_e::reject;
    case 54:
      return crit_e::reject;
    case 58:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2node_cfg_upd_ies_o::value_c e2node_cfg_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 3:
      ret.set(value_c::types::global_e2node_id);
      break;
    case 50:
      ret.set(value_c::types::e2node_component_cfg_addition);
      break;
    case 33:
      ret.set(value_c::types::e2node_component_cfg_upd);
      break;
    case 54:
      ret.set(value_c::types::e2node_component_cfg_removal);
      break;
    case 58:
      ret.set(value_c::types::e2node_tnl_assoc_removal);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2node_cfg_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 3:
      return presence_e::optional;
    case 50:
      return presence_e::optional;
    case 33:
      return presence_e::optional;
    case 54:
      return presence_e::optional;
    case 58:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2node_cfg_upd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::global_e2node_id:
      c = global_e2node_id_c{};
      break;
    case types::e2node_component_cfg_addition:
      c = e2node_component_cfg_addition_list_l{};
      break;
    case types::e2node_component_cfg_upd:
      c = e2node_component_cfg_upd_list_l{};
      break;
    case types::e2node_component_cfg_removal:
      c = e2node_component_cfg_removal_list_l{};
      break;
    case types::e2node_tnl_assoc_removal:
      c = e2node_tnl_assoc_removal_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
  }
}
uint16_t& e2node_cfg_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
global_e2node_id_c& e2node_cfg_upd_ies_o::value_c::global_e2node_id()
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
e2node_component_cfg_addition_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_addition()
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
e2node_component_cfg_upd_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_upd()
{
  assert_choice_type(types::e2node_component_cfg_upd, type_, "Value");
  return c.get<e2node_component_cfg_upd_list_l>();
}
e2node_component_cfg_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_removal()
{
  assert_choice_type(types::e2node_component_cfg_removal, type_, "Value");
  return c.get<e2node_component_cfg_removal_list_l>();
}
e2node_tnl_assoc_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_tnl_assoc_removal()
{
  assert_choice_type(types::e2node_tnl_assoc_removal, type_, "Value");
  return c.get<e2node_tnl_assoc_removal_list_l>();
}
const uint16_t& e2node_cfg_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const global_e2node_id_c& e2node_cfg_upd_ies_o::value_c::global_e2node_id() const
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
const e2node_component_cfg_addition_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_addition() const
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
const e2node_component_cfg_upd_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_upd() const
{
  assert_choice_type(types::e2node_component_cfg_upd, type_, "Value");
  return c.get<e2node_component_cfg_upd_list_l>();
}
const e2node_component_cfg_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_removal() const
{
  assert_choice_type(types::e2node_component_cfg_removal, type_, "Value");
  return c.get<e2node_component_cfg_removal_list_l>();
}
const e2node_tnl_assoc_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_tnl_assoc_removal() const
{
  assert_choice_type(types::e2node_tnl_assoc_removal, type_, "Value");
  return c.get<e2node_tnl_assoc_removal_list_l>();
}
void e2node_cfg_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      j.write_fieldname("GlobalE2node-ID");
      c.get<global_e2node_id_c>().to_json(j);
      break;
    case types::e2node_component_cfg_addition:
      j.start_array("E2nodeComponentConfigAddition-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_upd:
      j.start_array("E2nodeComponentConfigUpdate-List");
      for (const auto& e1 : c.get<e2node_component_cfg_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_removal:
      j.start_array("E2nodeComponentConfigRemoval-List");
      for (const auto& e1 : c.get<e2node_component_cfg_removal_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_tnl_assoc_removal:
      j.start_array("E2nodeTNLassociationRemoval-List");
      for (const auto& e1 : c.get<e2node_tnl_assoc_removal_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_cfg_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().pack(bref));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_upd_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_removal_list_l>(), 1, 1024, true));
      break;
    case types::e2node_tnl_assoc_removal:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_tnl_assoc_removal_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().unpack(bref));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_upd_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_removal_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_tnl_assoc_removal:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_tnl_assoc_removal_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "GlobalE2node-ID",
                                "E2nodeComponentConfigAddition-List",
                                "E2nodeComponentConfigUpdate-List",
                                "E2nodeComponentConfigRemoval-List",
                                "E2nodeTNLassociationRemoval-List"};
  return convert_enum_idx(names, 6, value, "e2node_cfg_upd_ies_o::value_c::types");
}

// E2setupFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 1, 31, 2, 48};
  return map_enum_number(names, 5, idx, "id");
}
bool e2setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 1, 31, 2, 48};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2setup_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 48:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2setup_fail_ies_o::value_c e2setup_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 48:
      ret.set(value_c::types::tnl_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2setup_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    case 48:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2setup_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::time_to_wait:
      c = time_to_wait_e{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::tnl_info:
      c = tnl_info_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
  }
}
uint16_t& e2setup_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2setup_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& e2setup_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& e2setup_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
tnl_info_s& e2setup_fail_ies_o::value_c::tnl_info()
{
  assert_choice_type(types::tnl_info, type_, "Value");
  return c.get<tnl_info_s>();
}
const uint16_t& e2setup_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2setup_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& e2setup_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& e2setup_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const tnl_info_s& e2setup_fail_ies_o::value_c::tnl_info() const
{
  assert_choice_type(types::tnl_info, type_, "Value");
  return c.get<tnl_info_s>();
}
void e2setup_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    case types::tnl_info:
      j.write_fieldname("TNLinformation");
      c.get<tnl_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2setup_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::tnl_info:
      HANDLE_CODE(c.get<tnl_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::tnl_info:
      HANDLE_CODE(c.get<tnl_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics", "TNLinformation"};
  return convert_enum_idx(names, 5, value, "e2setup_fail_ies_o::value_c::types");
}
uint8_t e2setup_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e2setup_fail_ies_o::value_c::types");
}

// E2setupRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 3, 10, 50};
  return map_enum_number(names, 4, idx, "id");
}
bool e2setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 3, 10, 50};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2setup_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 50:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2setup_request_ies_o::value_c e2setup_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 3:
      ret.set(value_c::types::global_e2node_id);
      break;
    case 10:
      ret.set(value_c::types::ran_functions_added);
      break;
    case 50:
      ret.set(value_c::types::e2node_component_cfg_addition);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2setup_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 10:
      return presence_e::mandatory;
    case 50:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2setup_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::global_e2node_id:
      c = global_e2node_id_c{};
      break;
    case types::ran_functions_added:
      c = ran_functions_list_l{};
      break;
    case types::e2node_component_cfg_addition:
      c = e2node_component_cfg_addition_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
  }
}
uint16_t& e2setup_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
global_e2node_id_c& e2setup_request_ies_o::value_c::global_e2node_id()
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
ran_functions_list_l& e2setup_request_ies_o::value_c::ran_functions_added()
{
  assert_choice_type(types::ran_functions_added, type_, "Value");
  return c.get<ran_functions_list_l>();
}
e2node_component_cfg_addition_list_l& e2setup_request_ies_o::value_c::e2node_component_cfg_addition()
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
const uint16_t& e2setup_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const global_e2node_id_c& e2setup_request_ies_o::value_c::global_e2node_id() const
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
const ran_functions_list_l& e2setup_request_ies_o::value_c::ran_functions_added() const
{
  assert_choice_type(types::ran_functions_added, type_, "Value");
  return c.get<ran_functions_list_l>();
}
const e2node_component_cfg_addition_list_l& e2setup_request_ies_o::value_c::e2node_component_cfg_addition() const
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
void e2setup_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      j.write_fieldname("GlobalE2node-ID");
      c.get<global_e2node_id_c>().to_json(j);
      break;
    case types::ran_functions_added:
      j.start_array("RANfunctions-List");
      for (const auto& e1 : c.get<ran_functions_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_addition:
      j.start_array("E2nodeComponentConfigAddition-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2setup_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().pack(bref));
      break;
    case types::ran_functions_added:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_list_l>(), 1, 256, true));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_list_l>(), 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().unpack(bref));
      break;
    case types::ran_functions_added:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_list_l>(), bref, 1, 256, true));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_list_l>(), bref, 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "GlobalE2node-ID", "RANfunctions-List", "E2nodeComponentConfigAddition-List"};
  return convert_enum_idx(names, 4, value, "e2setup_request_ies_o::value_c::types");
}

// E2setupResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 4, 9, 13, 52};
  return map_enum_number(names, 5, idx, "id");
}
bool e2setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 4, 9, 13, 52};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2setup_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 52:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e2setup_resp_ies_o::value_c e2setup_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 4:
      ret.set(value_c::types::global_ric_id);
      break;
    case 9:
      ret.set(value_c::types::ran_functions_accepted);
      break;
    case 13:
      ret.set(value_c::types::ran_functions_rejected);
      break;
    case 52:
      ret.set(value_c::types::e2node_component_cfg_addition_ack);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2setup_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 4:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    case 13:
      return presence_e::optional;
    case 52:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2setup_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::global_ric_id:
      c = global_ric_id_s{};
      break;
    case types::ran_functions_accepted:
      c = ran_functions_id_list_l{};
      break;
    case types::ran_functions_rejected:
      c = ran_functions_id_cause_list_l{};
      break;
    case types::e2node_component_cfg_addition_ack:
      c = e2node_component_cfg_addition_ack_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
  }
}
uint16_t& e2setup_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
global_ric_id_s& e2setup_resp_ies_o::value_c::global_ric_id()
{
  assert_choice_type(types::global_ric_id, type_, "Value");
  return c.get<global_ric_id_s>();
}
ran_functions_id_list_l& e2setup_resp_ies_o::value_c::ran_functions_accepted()
{
  assert_choice_type(types::ran_functions_accepted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
ran_functions_id_cause_list_l& e2setup_resp_ies_o::value_c::ran_functions_rejected()
{
  assert_choice_type(types::ran_functions_rejected, type_, "Value");
  return c.get<ran_functions_id_cause_list_l>();
}
e2node_component_cfg_addition_ack_list_l& e2setup_resp_ies_o::value_c::e2node_component_cfg_addition_ack()
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
const uint16_t& e2setup_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const global_ric_id_s& e2setup_resp_ies_o::value_c::global_ric_id() const
{
  assert_choice_type(types::global_ric_id, type_, "Value");
  return c.get<global_ric_id_s>();
}
const ran_functions_id_list_l& e2setup_resp_ies_o::value_c::ran_functions_accepted() const
{
  assert_choice_type(types::ran_functions_accepted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
const ran_functions_id_cause_list_l& e2setup_resp_ies_o::value_c::ran_functions_rejected() const
{
  assert_choice_type(types::ran_functions_rejected, type_, "Value");
  return c.get<ran_functions_id_cause_list_l>();
}
const e2node_component_cfg_addition_ack_list_l& e2setup_resp_ies_o::value_c::e2node_component_cfg_addition_ack() const
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
void e2setup_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::global_ric_id:
      j.write_fieldname("GlobalRIC-ID");
      c.get<global_ric_id_s>().to_json(j);
      break;
    case types::ran_functions_accepted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ran_functions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ran_functions_rejected:
      j.start_array("RANfunctionsIDcause-List");
      for (const auto& e1 : c.get<ran_functions_id_cause_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_addition_ack:
      j.start_array("E2nodeComponentConfigAdditionAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2setup_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_ric_id:
      HANDLE_CODE(c.get<global_ric_id_s>().pack(bref));
      break;
    case types::ran_functions_accepted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_id_list_l>(), 1, 256, true));
      break;
    case types::ran_functions_rejected:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_id_cause_list_l>(), 1, 256, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_ack_list_l>(), 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_ric_id:
      HANDLE_CODE(c.get<global_ric_id_s>().unpack(bref));
      break;
    case types::ran_functions_accepted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_id_list_l>(), bref, 1, 256, true));
      break;
    case types::ran_functions_rejected:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_id_cause_list_l>(), bref, 1, 256, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_ack_list_l>(), bref, 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "GlobalRIC-ID",
                                "RANfunctionsID-List",
                                "RANfunctionsIDcause-List",
                                "E2nodeComponentConfigAdditionAck-List"};
  return convert_enum_idx(names, 5, value, "e2setup_resp_ies_o::value_c::types");
}
uint8_t e2setup_resp_ies_o::value_c::types_opts::to_number() const
{
  switch (value) {
    case transaction_id:
      return 0;
    case e2node_component_cfg_addition_ack:
      return 2;
    default:
      invalid_enum_number(value, "e2setup_resp_ies_o::value_c::types");
  }
  return 0;
}

// ErrorIndication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t error_ind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 29, 5, 1, 2};
  return map_enum_number(names, 5, idx, "id");
}
bool error_ind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 29, 5, 1, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e error_ind_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
error_ind_ies_o::value_c error_ind_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e error_ind_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::optional;
    case 29:
      return presence_e::optional;
    case 5:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void error_ind_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }
}
uint16_t& error_ind_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ric_request_id_s& error_ind_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& error_ind_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& error_ind_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& error_ind_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& error_ind_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_request_id_s& error_ind_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& error_ind_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& error_ind_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& error_ind_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void error_ind_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE error_ind_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE error_ind_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* error_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "error_ind_ies_o::value_c::types");
}

// ResetRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t reset_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 1};
  return map_enum_number(names, 2, idx, "id");
}
bool reset_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e reset_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
reset_request_ies_o::value_c reset_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e reset_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void reset_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
  }
}
uint16_t& reset_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& reset_request_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const uint16_t& reset_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& reset_request_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
void reset_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE reset_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* reset_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause"};
  return convert_enum_idx(names, 2, value, "reset_request_ies_o::value_c::types");
}
uint8_t reset_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "reset_request_ies_o::value_c::types");
}

// ResetResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t reset_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 2};
  return map_enum_number(names, 2, idx, "id");
}
bool reset_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e reset_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
reset_resp_ies_o::value_c reset_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e reset_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void reset_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
  }
}
uint16_t& reset_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& reset_resp_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& reset_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& reset_resp_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void reset_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE reset_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* reset_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "reset_resp_ies_o::value_c::types");
}
uint8_t reset_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "reset_resp_ies_o::value_c::types");
}

// RICcontrolAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_ctrl_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 20, 32};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_ctrl_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 20, 32};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_ctrl_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 32:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_ctrl_ack_ies_o::value_c ric_ctrl_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 20:
      ret.set(value_c::types::ric_call_process_id);
      break;
    case 32:
      ret.set(value_c::types::ric_ctrl_outcome);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_ctrl_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    case 32:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_ctrl_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_call_process_id:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_ctrl_outcome:
      c = unbounded_octstring<true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_ack_ies_o::value_c");
  }
}
ric_request_id_s& ric_ctrl_ack_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_ctrl_ack_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ric_ctrl_ack_ies_o::value_c::ric_call_process_id()
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
unbounded_octstring<true>& ric_ctrl_ack_ies_o::value_c::ric_ctrl_outcome()
{
  assert_choice_type(types::ric_ctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const ric_request_id_s& ric_ctrl_ack_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_ctrl_ack_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ric_ctrl_ack_ies_o::value_c::ric_call_process_id() const
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const unbounded_octstring<true>& ric_ctrl_ack_ies_o::value_c::ric_ctrl_outcome() const
{
  assert_choice_type(types::ric_ctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
void ric_ctrl_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_call_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_ctrl_outcome:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_ctrl_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_ctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_ctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_ctrl_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "ric_ctrl_ack_ies_o::value_c::types");
}
uint8_t ric_ctrl_ack_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_ctrl_ack_ies_o::value_c::types");
  return 0;
}

// RICcontrolFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_ctrl_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 20, 1, 32};
  return map_enum_number(names, 5, idx, "id");
}
bool ric_ctrl_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 20, 1, 32};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_ctrl_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 32:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_ctrl_fail_ies_o::value_c ric_ctrl_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 20:
      ret.set(value_c::types::ric_call_process_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 32:
      ret.set(value_c::types::ric_ctrl_outcome);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_ctrl_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    case 1:
      return presence_e::mandatory;
    case 32:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_ctrl_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_call_process_id:
      c = unbounded_octstring<true>{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::ric_ctrl_outcome:
      c = unbounded_octstring<true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_fail_ies_o::value_c");
  }
}
ric_request_id_s& ric_ctrl_fail_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_ctrl_fail_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ric_ctrl_fail_ies_o::value_c::ric_call_process_id()
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
cause_c& ric_ctrl_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
unbounded_octstring<true>& ric_ctrl_fail_ies_o::value_c::ric_ctrl_outcome()
{
  assert_choice_type(types::ric_ctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const ric_request_id_s& ric_ctrl_fail_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_ctrl_fail_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ric_ctrl_fail_ies_o::value_c::ric_call_process_id() const
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const cause_c& ric_ctrl_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const unbounded_octstring<true>& ric_ctrl_fail_ies_o::value_c::ric_ctrl_outcome() const
{
  assert_choice_type(types::ric_ctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
void ric_ctrl_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_call_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::ric_ctrl_outcome:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_ctrl_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::ric_ctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::ric_ctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_ctrl_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "OCTET STRING", "Cause", "OCTET STRING"};
  return convert_enum_idx(names, 5, value, "ric_ctrl_fail_ies_o::value_c::types");
}
uint8_t ric_ctrl_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_ctrl_fail_ies_o::value_c::types");
  return 0;
}

// RICcontrolRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_ctrl_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 20, 22, 23, 21};
  return map_enum_number(names, 6, idx, "id");
}
bool ric_ctrl_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 20, 22, 23, 21};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_ctrl_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 22:
      return crit_e::reject;
    case 23:
      return crit_e::reject;
    case 21:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_ctrl_request_ies_o::value_c ric_ctrl_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 20:
      ret.set(value_c::types::ric_call_process_id);
      break;
    case 22:
      ret.set(value_c::types::ric_ctrl_hdr);
      break;
    case 23:
      ret.set(value_c::types::ric_ctrl_msg);
      break;
    case 21:
      ret.set(value_c::types::ric_ctrl_ack_request);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_ctrl_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    case 22:
      return presence_e::mandatory;
    case 23:
      return presence_e::mandatory;
    case 21:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_ctrl_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_call_process_id:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_ctrl_hdr:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_ctrl_msg:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_ctrl_ack_request:
      c = ric_ctrl_ack_request_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_request_ies_o::value_c");
  }
}
ric_request_id_s& ric_ctrl_request_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_ctrl_request_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ric_ctrl_request_ies_o::value_c::ric_call_process_id()
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
unbounded_octstring<true>& ric_ctrl_request_ies_o::value_c::ric_ctrl_hdr()
{
  assert_choice_type(types::ric_ctrl_hdr, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
unbounded_octstring<true>& ric_ctrl_request_ies_o::value_c::ric_ctrl_msg()
{
  assert_choice_type(types::ric_ctrl_msg, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
ric_ctrl_ack_request_e& ric_ctrl_request_ies_o::value_c::ric_ctrl_ack_request()
{
  assert_choice_type(types::ric_ctrl_ack_request, type_, "Value");
  return c.get<ric_ctrl_ack_request_e>();
}
const ric_request_id_s& ric_ctrl_request_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_ctrl_request_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ric_ctrl_request_ies_o::value_c::ric_call_process_id() const
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const unbounded_octstring<true>& ric_ctrl_request_ies_o::value_c::ric_ctrl_hdr() const
{
  assert_choice_type(types::ric_ctrl_hdr, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const unbounded_octstring<true>& ric_ctrl_request_ies_o::value_c::ric_ctrl_msg() const
{
  assert_choice_type(types::ric_ctrl_msg, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const ric_ctrl_ack_request_e& ric_ctrl_request_ies_o::value_c::ric_ctrl_ack_request() const
{
  assert_choice_type(types::ric_ctrl_ack_request, type_, "Value");
  return c.get<ric_ctrl_ack_request_e>();
}
void ric_ctrl_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_call_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_ctrl_hdr:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_ctrl_msg:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_ctrl_ack_request:
      j.write_str("RICcontrolAckRequest", c.get<ric_ctrl_ack_request_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_ctrl_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_ctrl_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_ctrl_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_ctrl_ack_request:
      HANDLE_CODE(c.get<ric_ctrl_ack_request_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_ctrl_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_ctrl_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_ctrl_ack_request:
      HANDLE_CODE(c.get<ric_ctrl_ack_request_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ctrl_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_ctrl_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "RICrequestID", "INTEGER (0..4095)", "OCTET STRING", "OCTET STRING", "OCTET STRING", "RICcontrolAckRequest"};
  return convert_enum_idx(names, 6, value, "ric_ctrl_request_ies_o::value_c::types");
}
uint8_t ric_ctrl_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_ctrl_request_ies_o::value_c::types");
  return 0;
}

// RICindication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_ind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 15, 27, 28, 25, 26, 20};
  return map_enum_number(names, 8, idx, "id");
}
bool ric_ind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 15, 27, 28, 25, 26, 20};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_ind_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 27:
      return crit_e::reject;
    case 28:
      return crit_e::reject;
    case 25:
      return crit_e::reject;
    case 26:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_ind_ies_o::value_c ric_ind_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 15:
      ret.set(value_c::types::ric_action_id);
      break;
    case 27:
      ret.set(value_c::types::ric_ind_sn);
      break;
    case 28:
      ret.set(value_c::types::ric_ind_type);
      break;
    case 25:
      ret.set(value_c::types::ric_ind_hdr);
      break;
    case 26:
      ret.set(value_c::types::ric_ind_msg);
      break;
    case 20:
      ret.set(value_c::types::ric_call_process_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_ind_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 15:
      return presence_e::mandatory;
    case 27:
      return presence_e::optional;
    case 28:
      return presence_e::mandatory;
    case 25:
      return presence_e::mandatory;
    case 26:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_ind_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_action_id:
      c = uint16_t{};
      break;
    case types::ric_ind_sn:
      c = uint32_t{};
      break;
    case types::ric_ind_type:
      c = ric_ind_type_e{};
      break;
    case types::ric_ind_hdr:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_ind_msg:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_call_process_id:
      c = unbounded_octstring<true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_ind_ies_o::value_c");
  }
}
ric_request_id_s& ric_ind_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_ind_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& ric_ind_ies_o::value_c::ric_action_id()
{
  assert_choice_type(types::ric_action_id, type_, "Value");
  return c.get<uint16_t>();
}
uint32_t& ric_ind_ies_o::value_c::ric_ind_sn()
{
  assert_choice_type(types::ric_ind_sn, type_, "Value");
  return c.get<uint32_t>();
}
ric_ind_type_e& ric_ind_ies_o::value_c::ric_ind_type()
{
  assert_choice_type(types::ric_ind_type, type_, "Value");
  return c.get<ric_ind_type_e>();
}
unbounded_octstring<true>& ric_ind_ies_o::value_c::ric_ind_hdr()
{
  assert_choice_type(types::ric_ind_hdr, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
unbounded_octstring<true>& ric_ind_ies_o::value_c::ric_ind_msg()
{
  assert_choice_type(types::ric_ind_msg, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
unbounded_octstring<true>& ric_ind_ies_o::value_c::ric_call_process_id()
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const ric_request_id_s& ric_ind_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_ind_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& ric_ind_ies_o::value_c::ric_action_id() const
{
  assert_choice_type(types::ric_action_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint32_t& ric_ind_ies_o::value_c::ric_ind_sn() const
{
  assert_choice_type(types::ric_ind_sn, type_, "Value");
  return c.get<uint32_t>();
}
const ric_ind_type_e& ric_ind_ies_o::value_c::ric_ind_type() const
{
  assert_choice_type(types::ric_ind_type, type_, "Value");
  return c.get<ric_ind_type_e>();
}
const unbounded_octstring<true>& ric_ind_ies_o::value_c::ric_ind_hdr() const
{
  assert_choice_type(types::ric_ind_hdr, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const unbounded_octstring<true>& ric_ind_ies_o::value_c::ric_ind_msg() const
{
  assert_choice_type(types::ric_ind_msg, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const unbounded_octstring<true>& ric_ind_ies_o::value_c::ric_call_process_id() const
{
  assert_choice_type(types::ric_call_process_id, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
void ric_ind_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_action_id:
      j.write_int("INTEGER (0..255)", c.get<uint16_t>());
      break;
    case types::ric_ind_sn:
      j.write_int("INTEGER (0..65535)", c.get<uint32_t>());
      break;
    case types::ric_ind_type:
      j.write_str("RICindicationType", c.get<ric_ind_type_e>().to_string());
      break;
    case types::ric_ind_hdr:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_ind_msg:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_call_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ric_ind_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_ind_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_action_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::ric_ind_sn:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::ric_ind_type:
      HANDLE_CODE(c.get<ric_ind_type_e>().pack(bref));
      break;
    case types::ric_ind_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_ind_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ind_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ind_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_action_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::ric_ind_sn:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::ric_ind_type:
      HANDLE_CODE(c.get<ric_ind_type_e>().unpack(bref));
      break;
    case types::ric_ind_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_ind_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_call_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_ind_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID",
                                "INTEGER (0..4095)",
                                "INTEGER (0..255)",
                                "INTEGER (0..65535)",
                                "RICindicationType",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING"};
  return convert_enum_idx(names, 8, value, "ric_ind_ies_o::value_c::types");
}

// RICQueryFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_query_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_query_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_query_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_query_fail_ies_o::value_c ric_query_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_query_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_query_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_fail_ies_o::value_c");
  }
}
ric_request_id_s& ric_query_fail_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_query_fail_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ric_query_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ric_query_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ric_request_id_s& ric_query_fail_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_query_fail_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ric_query_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ric_query_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ric_query_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_query_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_query_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ric_query_fail_ies_o::value_c::types");
}
uint8_t ric_query_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_query_fail_ies_o::value_c::types");
  return 0;
}

// RICQueryRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_query_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 95, 96};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_query_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 95, 96};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_query_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 95:
      return crit_e::reject;
    case 96:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_query_request_ies_o::value_c ric_query_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 95:
      ret.set(value_c::types::ric_query_hdr);
      break;
    case 96:
      ret.set(value_c::types::ric_query_definition);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_query_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 95:
      return presence_e::mandatory;
    case 96:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_query_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_query_hdr:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_query_definition:
      c = unbounded_octstring<true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_request_ies_o::value_c");
  }
}
ric_request_id_s& ric_query_request_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_query_request_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ric_query_request_ies_o::value_c::ric_query_hdr()
{
  assert_choice_type(types::ric_query_hdr, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
unbounded_octstring<true>& ric_query_request_ies_o::value_c::ric_query_definition()
{
  assert_choice_type(types::ric_query_definition, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const ric_request_id_s& ric_query_request_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_query_request_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ric_query_request_ies_o::value_c::ric_query_hdr() const
{
  assert_choice_type(types::ric_query_hdr, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const unbounded_octstring<true>& ric_query_request_ies_o::value_c::ric_query_definition() const
{
  assert_choice_type(types::ric_query_definition, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
void ric_query_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_query_hdr:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_query_definition:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_query_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_query_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_query_definition:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_query_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_query_definition:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_query_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "ric_query_request_ies_o::value_c::types");
}
uint8_t ric_query_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_query_request_ies_o::value_c::types");
  return 0;
}

// RICQueryResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_query_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 97};
  return map_enum_number(names, 3, idx, "id");
}
bool ric_query_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 97};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_query_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 97:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_query_resp_ies_o::value_c ric_query_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 97:
      ret.set(value_c::types::ric_query_outcome);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_query_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 97:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_query_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_query_outcome:
      c = unbounded_octstring<true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_resp_ies_o::value_c");
  }
}
ric_request_id_s& ric_query_resp_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_query_resp_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ric_query_resp_ies_o::value_c::ric_query_outcome()
{
  assert_choice_type(types::ric_query_outcome, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const ric_request_id_s& ric_query_resp_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_query_resp_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ric_query_resp_ies_o::value_c::ric_query_outcome() const
{
  assert_choice_type(types::ric_query_outcome, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
void ric_query_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_query_outcome:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_query_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_query_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_query_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_query_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_query_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "ric_query_resp_ies_o::value_c::types");
}
uint8_t ric_query_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_query_resp_ies_o::value_c::types");
  return 0;
}

// RICserviceQuery-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_service_query_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 9};
  return map_enum_number(names, 2, idx, "id");
}
bool ric_service_query_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 9};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_service_query_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_service_query_ies_o::value_c ric_service_query_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 9:
      ret.set(value_c::types::ran_functions_accepted);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_service_query_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_service_query_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::ran_functions_accepted:
      c = ran_functions_id_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_query_ies_o::value_c");
  }
}
uint16_t& ric_service_query_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ran_functions_id_list_l& ric_service_query_ies_o::value_c::ran_functions_accepted()
{
  assert_choice_type(types::ran_functions_accepted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
const uint16_t& ric_service_query_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ran_functions_id_list_l& ric_service_query_ies_o::value_c::ran_functions_accepted() const
{
  assert_choice_type(types::ran_functions_accepted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
void ric_service_query_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ran_functions_accepted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ran_functions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_query_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_service_query_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_functions_accepted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_id_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_query_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_query_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_functions_accepted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_id_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_query_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_service_query_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "RANfunctionsID-List"};
  return convert_enum_idx(names, 2, value, "ric_service_query_ies_o::value_c::types");
}
uint8_t ric_service_query_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ric_service_query_ies_o::value_c::types");
}

// RICserviceUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_service_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 9, 13};
  return map_enum_number(names, 3, idx, "id");
}
bool ric_service_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 9, 13};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_service_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_service_upd_ack_ies_o::value_c ric_service_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 9:
      ret.set(value_c::types::ran_functions_accepted);
      break;
    case 13:
      ret.set(value_c::types::ran_functions_rejected);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_service_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    case 13:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_service_upd_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::ran_functions_accepted:
      c = ran_functions_id_list_l{};
      break;
    case types::ran_functions_rejected:
      c = ran_functions_id_cause_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ack_ies_o::value_c");
  }
}
uint16_t& ric_service_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ran_functions_id_list_l& ric_service_upd_ack_ies_o::value_c::ran_functions_accepted()
{
  assert_choice_type(types::ran_functions_accepted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
ran_functions_id_cause_list_l& ric_service_upd_ack_ies_o::value_c::ran_functions_rejected()
{
  assert_choice_type(types::ran_functions_rejected, type_, "Value");
  return c.get<ran_functions_id_cause_list_l>();
}
const uint16_t& ric_service_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ran_functions_id_list_l& ric_service_upd_ack_ies_o::value_c::ran_functions_accepted() const
{
  assert_choice_type(types::ran_functions_accepted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
const ran_functions_id_cause_list_l& ric_service_upd_ack_ies_o::value_c::ran_functions_rejected() const
{
  assert_choice_type(types::ran_functions_rejected, type_, "Value");
  return c.get<ran_functions_id_cause_list_l>();
}
void ric_service_upd_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ran_functions_accepted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ran_functions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ran_functions_rejected:
      j.start_array("RANfunctionsIDcause-List");
      for (const auto& e1 : c.get<ran_functions_id_cause_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_service_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_functions_accepted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_id_list_l>(), 1, 256, true));
      break;
    case types::ran_functions_rejected:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_id_cause_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_functions_accepted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_id_list_l>(), bref, 1, 256, true));
      break;
    case types::ran_functions_rejected:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_id_cause_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_service_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "RANfunctionsID-List", "RANfunctionsIDcause-List"};
  return convert_enum_idx(names, 3, value, "ric_service_upd_ack_ies_o::value_c::types");
}
uint8_t ric_service_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ric_service_upd_ack_ies_o::value_c::types");
}

// RICserviceUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_service_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 1, 31, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_service_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 1, 31, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_service_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_service_upd_fail_ies_o::value_c ric_service_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_service_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_service_upd_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::time_to_wait:
      c = time_to_wait_e{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_fail_ies_o::value_c");
  }
}
uint16_t& ric_service_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ric_service_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& ric_service_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& ric_service_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& ric_service_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ric_service_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& ric_service_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& ric_service_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ric_service_upd_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_service_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_service_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ric_service_upd_fail_ies_o::value_c::types");
}
uint8_t ric_service_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ric_service_upd_fail_ies_o::value_c::types");
}

// RICserviceUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_service_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49, 10, 12, 11};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_service_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {49, 10, 12, 11};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_service_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 12:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_service_upd_ies_o::value_c ric_service_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 10:
      ret.set(value_c::types::ran_functions_added);
      break;
    case 12:
      ret.set(value_c::types::ran_functions_modified);
      break;
    case 11:
      ret.set(value_c::types::ran_functions_deleted);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_service_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 10:
      return presence_e::optional;
    case 12:
      return presence_e::optional;
    case 11:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_service_upd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::ran_functions_added:
      c = ran_functions_list_l{};
      break;
    case types::ran_functions_modified:
      c = ran_functions_list_l{};
      break;
    case types::ran_functions_deleted:
      c = ran_functions_id_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ies_o::value_c");
  }
}
uint16_t& ric_service_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ran_functions_list_l& ric_service_upd_ies_o::value_c::ran_functions_added()
{
  assert_choice_type(types::ran_functions_added, type_, "Value");
  return c.get<ran_functions_list_l>();
}
ran_functions_list_l& ric_service_upd_ies_o::value_c::ran_functions_modified()
{
  assert_choice_type(types::ran_functions_modified, type_, "Value");
  return c.get<ran_functions_list_l>();
}
ran_functions_id_list_l& ric_service_upd_ies_o::value_c::ran_functions_deleted()
{
  assert_choice_type(types::ran_functions_deleted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
const uint16_t& ric_service_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ran_functions_list_l& ric_service_upd_ies_o::value_c::ran_functions_added() const
{
  assert_choice_type(types::ran_functions_added, type_, "Value");
  return c.get<ran_functions_list_l>();
}
const ran_functions_list_l& ric_service_upd_ies_o::value_c::ran_functions_modified() const
{
  assert_choice_type(types::ran_functions_modified, type_, "Value");
  return c.get<ran_functions_list_l>();
}
const ran_functions_id_list_l& ric_service_upd_ies_o::value_c::ran_functions_deleted() const
{
  assert_choice_type(types::ran_functions_deleted, type_, "Value");
  return c.get<ran_functions_id_list_l>();
}
void ric_service_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ran_functions_added:
      j.start_array("RANfunctions-List");
      for (const auto& e1 : c.get<ran_functions_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ran_functions_modified:
      j.start_array("RANfunctions-List");
      for (const auto& e1 : c.get<ran_functions_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ran_functions_deleted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ran_functions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_service_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_functions_added:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_list_l>(), 1, 256, true));
      break;
    case types::ran_functions_modified:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_list_l>(), 1, 256, true));
      break;
    case types::ran_functions_deleted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ran_functions_id_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_functions_added:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_list_l>(), bref, 1, 256, true));
      break;
    case types::ran_functions_modified:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_list_l>(), bref, 1, 256, true));
      break;
    case types::ran_functions_deleted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ran_functions_id_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_service_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_service_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "RANfunctions-List", "RANfunctions-List", "RANfunctionsID-List"};
  return convert_enum_idx(names, 4, value, "ric_service_upd_ies_o::value_c::types");
}
uint8_t ric_service_upd_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ric_service_upd_ies_o::value_c::types");
}

// RICsubscriptionDeleteFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_delete_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_sub_delete_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_delete_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_delete_fail_ies_o::value_c ric_sub_delete_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_delete_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_delete_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_fail_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_delete_fail_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_delete_fail_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ric_sub_delete_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ric_sub_delete_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ric_request_id_s& ric_sub_delete_fail_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_delete_fail_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ric_sub_delete_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ric_sub_delete_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ric_sub_delete_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_delete_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_delete_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_delete_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ric_sub_delete_fail_ies_o::value_c::types");
}
uint8_t ric_sub_delete_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_delete_fail_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionDeleteRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_delete_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5};
  return map_enum_number(names, 2, idx, "id");
}
bool ric_sub_delete_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_delete_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_delete_request_ies_o::value_c ric_sub_delete_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_delete_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_delete_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_request_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_delete_request_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_delete_request_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_request_id_s& ric_sub_delete_request_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_delete_request_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
void ric_sub_delete_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_delete_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_delete_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_delete_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)"};
  return convert_enum_idx(names, 2, value, "ric_sub_delete_request_ies_o::value_c::types");
}
uint8_t ric_sub_delete_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_delete_request_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionDeleteResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_delete_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5};
  return map_enum_number(names, 2, idx, "id");
}
bool ric_sub_delete_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_delete_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_delete_resp_ies_o::value_c ric_sub_delete_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_delete_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_delete_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_resp_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_delete_resp_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_delete_resp_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_request_id_s& ric_sub_delete_resp_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_delete_resp_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
void ric_sub_delete_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_delete_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_delete_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_delete_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_delete_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)"};
  return convert_enum_idx(names, 2, value, "ric_sub_delete_resp_ies_o::value_c::types");
}
uint8_t ric_sub_delete_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_delete_resp_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_sub_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_fail_ies_o::value_c ric_sub_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_fail_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_fail_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_fail_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ric_sub_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ric_sub_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ric_request_id_s& ric_sub_fail_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_fail_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ric_sub_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ric_sub_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ric_sub_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ric_sub_fail_ies_o::value_c::types");
}
uint8_t ric_sub_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_fail_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionModificationConfirm-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_mod_confirm_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 87, 89, 91, 93};
  return map_enum_number(names, 6, idx, "id");
}
bool ric_sub_mod_confirm_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 87, 89, 91, 93};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_mod_confirm_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 87:
      return crit_e::ignore;
    case 89:
      return crit_e::ignore;
    case 91:
      return crit_e::ignore;
    case 93:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_mod_confirm_ies_o::value_c ric_sub_mod_confirm_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 87:
      ret.set(value_c::types::ric_actions_confirmed_for_mod_list);
      break;
    case 89:
      ret.set(value_c::types::ric_actions_refused_to_be_modified_list);
      break;
    case 91:
      ret.set(value_c::types::ric_actions_confirmed_for_removal_list);
      break;
    case 93:
      ret.set(value_c::types::ric_actions_refused_to_be_remd_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_mod_confirm_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 87:
      return presence_e::optional;
    case 89:
      return presence_e::optional;
    case 91:
      return presence_e::optional;
    case 93:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_mod_confirm_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_actions_confirmed_for_mod_list:
      c = ric_actions_confirmed_for_mod_list_l{};
      break;
    case types::ric_actions_refused_to_be_modified_list:
      c = ric_actions_refused_to_be_modified_list_l{};
      break;
    case types::ric_actions_confirmed_for_removal_list:
      c = ric_actions_confirmed_for_removal_list_l{};
      break;
    case types::ric_actions_refused_to_be_remd_list:
      c = ric_actions_refused_to_be_remd_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_confirm_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_mod_confirm_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_mod_confirm_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
ric_actions_confirmed_for_mod_list_l& ric_sub_mod_confirm_ies_o::value_c::ric_actions_confirmed_for_mod_list()
{
  assert_choice_type(types::ric_actions_confirmed_for_mod_list, type_, "Value");
  return c.get<ric_actions_confirmed_for_mod_list_l>();
}
ric_actions_refused_to_be_modified_list_l& ric_sub_mod_confirm_ies_o::value_c::ric_actions_refused_to_be_modified_list()
{
  assert_choice_type(types::ric_actions_refused_to_be_modified_list, type_, "Value");
  return c.get<ric_actions_refused_to_be_modified_list_l>();
}
ric_actions_confirmed_for_removal_list_l& ric_sub_mod_confirm_ies_o::value_c::ric_actions_confirmed_for_removal_list()
{
  assert_choice_type(types::ric_actions_confirmed_for_removal_list, type_, "Value");
  return c.get<ric_actions_confirmed_for_removal_list_l>();
}
ric_actions_refused_to_be_remd_list_l& ric_sub_mod_confirm_ies_o::value_c::ric_actions_refused_to_be_remd_list()
{
  assert_choice_type(types::ric_actions_refused_to_be_remd_list, type_, "Value");
  return c.get<ric_actions_refused_to_be_remd_list_l>();
}
const ric_request_id_s& ric_sub_mod_confirm_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_mod_confirm_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_actions_confirmed_for_mod_list_l&
ric_sub_mod_confirm_ies_o::value_c::ric_actions_confirmed_for_mod_list() const
{
  assert_choice_type(types::ric_actions_confirmed_for_mod_list, type_, "Value");
  return c.get<ric_actions_confirmed_for_mod_list_l>();
}
const ric_actions_refused_to_be_modified_list_l&
ric_sub_mod_confirm_ies_o::value_c::ric_actions_refused_to_be_modified_list() const
{
  assert_choice_type(types::ric_actions_refused_to_be_modified_list, type_, "Value");
  return c.get<ric_actions_refused_to_be_modified_list_l>();
}
const ric_actions_confirmed_for_removal_list_l&
ric_sub_mod_confirm_ies_o::value_c::ric_actions_confirmed_for_removal_list() const
{
  assert_choice_type(types::ric_actions_confirmed_for_removal_list, type_, "Value");
  return c.get<ric_actions_confirmed_for_removal_list_l>();
}
const ric_actions_refused_to_be_remd_list_l&
ric_sub_mod_confirm_ies_o::value_c::ric_actions_refused_to_be_remd_list() const
{
  assert_choice_type(types::ric_actions_refused_to_be_remd_list, type_, "Value");
  return c.get<ric_actions_refused_to_be_remd_list_l>();
}
void ric_sub_mod_confirm_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_actions_confirmed_for_mod_list:
      j.start_array("RICactions-ConfirmedForModification-List");
      for (const auto& e1 : c.get<ric_actions_confirmed_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_refused_to_be_modified_list:
      j.start_array("RICactions-RefusedToBeModified-List");
      for (const auto& e1 : c.get<ric_actions_refused_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_confirmed_for_removal_list:
      j.start_array("RICactions-ConfirmedForRemoval-List");
      for (const auto& e1 : c.get<ric_actions_confirmed_for_removal_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_refused_to_be_remd_list:
      j.start_array("RICactions-RefusedToBeRemoved-List");
      for (const auto& e1 : c.get<ric_actions_refused_to_be_remd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_confirm_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_mod_confirm_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_confirmed_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_confirmed_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_refused_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_refused_to_be_modified_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_confirmed_for_removal_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_confirmed_for_removal_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_refused_to_be_remd_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_refused_to_be_remd_list_l>(), 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_confirm_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_confirm_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_confirmed_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_confirmed_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_refused_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_refused_to_be_modified_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_confirmed_for_removal_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_confirmed_for_removal_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_refused_to_be_remd_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_refused_to_be_remd_list_l>(), bref, 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_confirm_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_mod_confirm_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID",
                                "INTEGER (0..4095)",
                                "RICactions-ConfirmedForModification-List",
                                "RICactions-RefusedToBeModified-List",
                                "RICactions-ConfirmedForRemoval-List",
                                "RICactions-RefusedToBeRemoved-List"};
  return convert_enum_idx(names, 6, value, "ric_sub_mod_confirm_ies_o::value_c::types");
}
uint8_t ric_sub_mod_confirm_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_mod_confirm_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionModificationFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_mod_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_sub_mod_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_mod_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_mod_fail_ies_o::value_c ric_sub_mod_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_mod_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_mod_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_fail_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_mod_fail_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_mod_fail_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ric_sub_mod_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ric_sub_mod_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ric_request_id_s& ric_sub_mod_fail_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_mod_fail_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ric_sub_mod_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ric_sub_mod_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ric_sub_mod_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_mod_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ric_sub_mod_fail_ies_o::value_c::types");
}
uint8_t ric_sub_mod_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_mod_fail_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionModificationRefuse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_mod_refuse_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_sub_mod_refuse_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 1, 2};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_mod_refuse_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_mod_refuse_ies_o::value_c ric_sub_mod_refuse_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_mod_refuse_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_mod_refuse_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_refuse_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_mod_refuse_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_mod_refuse_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ric_sub_mod_refuse_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ric_sub_mod_refuse_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ric_request_id_s& ric_sub_mod_refuse_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_mod_refuse_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ric_sub_mod_refuse_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ric_sub_mod_refuse_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ric_sub_mod_refuse_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_refuse_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_mod_refuse_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_refuse_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_refuse_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_refuse_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_mod_refuse_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ric_sub_mod_refuse_ies_o::value_c::types");
}
uint8_t ric_sub_mod_refuse_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_mod_refuse_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionModificationRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_mod_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 64, 65, 67, 69};
  return map_enum_number(names, 6, idx, "id");
}
bool ric_sub_mod_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 64, 65, 67, 69};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_mod_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 64:
      return crit_e::ignore;
    case 65:
      return crit_e::ignore;
    case 67:
      return crit_e::ignore;
    case 69:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_mod_request_ies_o::value_c ric_sub_mod_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 64:
      ret.set(value_c::types::ric_event_trigger_definition_to_be_modified);
      break;
    case 65:
      ret.set(value_c::types::ric_actions_to_be_remd_for_mod_list);
      break;
    case 67:
      ret.set(value_c::types::ric_actions_to_be_modified_for_mod_list);
      break;
    case 69:
      ret.set(value_c::types::ric_actions_to_be_added_for_mod_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_mod_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 64:
      return presence_e::optional;
    case 65:
      return presence_e::optional;
    case 67:
      return presence_e::optional;
    case 69:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_mod_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_event_trigger_definition_to_be_modified:
      c = unbounded_octstring<true>{};
      break;
    case types::ric_actions_to_be_remd_for_mod_list:
      c = ric_actions_to_be_remd_for_mod_list_l{};
      break;
    case types::ric_actions_to_be_modified_for_mod_list:
      c = ric_actions_to_be_modified_for_mod_list_l{};
      break;
    case types::ric_actions_to_be_added_for_mod_list:
      c = ric_actions_to_be_added_for_mod_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_request_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_mod_request_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_mod_request_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ric_sub_mod_request_ies_o::value_c::ric_event_trigger_definition_to_be_modified()
{
  assert_choice_type(types::ric_event_trigger_definition_to_be_modified, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
ric_actions_to_be_remd_for_mod_list_l& ric_sub_mod_request_ies_o::value_c::ric_actions_to_be_remd_for_mod_list()
{
  assert_choice_type(types::ric_actions_to_be_remd_for_mod_list, type_, "Value");
  return c.get<ric_actions_to_be_remd_for_mod_list_l>();
}
ric_actions_to_be_modified_for_mod_list_l& ric_sub_mod_request_ies_o::value_c::ric_actions_to_be_modified_for_mod_list()
{
  assert_choice_type(types::ric_actions_to_be_modified_for_mod_list, type_, "Value");
  return c.get<ric_actions_to_be_modified_for_mod_list_l>();
}
ric_actions_to_be_added_for_mod_list_l& ric_sub_mod_request_ies_o::value_c::ric_actions_to_be_added_for_mod_list()
{
  assert_choice_type(types::ric_actions_to_be_added_for_mod_list, type_, "Value");
  return c.get<ric_actions_to_be_added_for_mod_list_l>();
}
const ric_request_id_s& ric_sub_mod_request_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_mod_request_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ric_sub_mod_request_ies_o::value_c::ric_event_trigger_definition_to_be_modified() const
{
  assert_choice_type(types::ric_event_trigger_definition_to_be_modified, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const ric_actions_to_be_remd_for_mod_list_l&
ric_sub_mod_request_ies_o::value_c::ric_actions_to_be_remd_for_mod_list() const
{
  assert_choice_type(types::ric_actions_to_be_remd_for_mod_list, type_, "Value");
  return c.get<ric_actions_to_be_remd_for_mod_list_l>();
}
const ric_actions_to_be_modified_for_mod_list_l&
ric_sub_mod_request_ies_o::value_c::ric_actions_to_be_modified_for_mod_list() const
{
  assert_choice_type(types::ric_actions_to_be_modified_for_mod_list, type_, "Value");
  return c.get<ric_actions_to_be_modified_for_mod_list_l>();
}
const ric_actions_to_be_added_for_mod_list_l&
ric_sub_mod_request_ies_o::value_c::ric_actions_to_be_added_for_mod_list() const
{
  assert_choice_type(types::ric_actions_to_be_added_for_mod_list, type_, "Value");
  return c.get<ric_actions_to_be_added_for_mod_list_l>();
}
void ric_sub_mod_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_event_trigger_definition_to_be_modified:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::ric_actions_to_be_remd_for_mod_list:
      j.start_array("RICactions-ToBeRemovedForModification-List");
      for (const auto& e1 : c.get<ric_actions_to_be_remd_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_to_be_modified_for_mod_list:
      j.start_array("RICactions-ToBeModifiedForModification-List");
      for (const auto& e1 : c.get<ric_actions_to_be_modified_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_to_be_added_for_mod_list:
      j.start_array("RICactions-ToBeAddedForModification-List");
      for (const auto& e1 : c.get<ric_actions_to_be_added_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_mod_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_event_trigger_definition_to_be_modified:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::ric_actions_to_be_remd_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_to_be_remd_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_to_be_modified_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_to_be_modified_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_to_be_added_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_to_be_added_for_mod_list_l>(), 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_event_trigger_definition_to_be_modified:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::ric_actions_to_be_remd_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_to_be_remd_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_to_be_modified_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_to_be_modified_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_to_be_added_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_to_be_added_for_mod_list_l>(), bref, 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID",
                                "INTEGER (0..4095)",
                                "OCTET STRING",
                                "RICactions-ToBeRemovedForModification-List",
                                "RICactions-ToBeModifiedForModification-List",
                                "RICactions-ToBeAddedForModification-List"};
  return convert_enum_idx(names, 6, value, "ric_sub_mod_request_ies_o::value_c::types");
}
uint8_t ric_sub_mod_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_mod_request_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionModificationRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_mod_required_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 83, 85};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_sub_mod_required_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 83, 85};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_mod_required_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 83:
      return crit_e::ignore;
    case 85:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_mod_required_ies_o::value_c ric_sub_mod_required_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 83:
      ret.set(value_c::types::ric_actions_required_to_be_modified_list);
      break;
    case 85:
      ret.set(value_c::types::ric_actions_required_to_be_remd_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_mod_required_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 83:
      return presence_e::optional;
    case 85:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_mod_required_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_actions_required_to_be_modified_list:
      c = ric_actions_required_to_be_modified_list_l{};
      break;
    case types::ric_actions_required_to_be_remd_list:
      c = ric_actions_required_to_be_remd_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_required_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_mod_required_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_mod_required_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
ric_actions_required_to_be_modified_list_l&
ric_sub_mod_required_ies_o::value_c::ric_actions_required_to_be_modified_list()
{
  assert_choice_type(types::ric_actions_required_to_be_modified_list, type_, "Value");
  return c.get<ric_actions_required_to_be_modified_list_l>();
}
ric_actions_required_to_be_remd_list_l& ric_sub_mod_required_ies_o::value_c::ric_actions_required_to_be_remd_list()
{
  assert_choice_type(types::ric_actions_required_to_be_remd_list, type_, "Value");
  return c.get<ric_actions_required_to_be_remd_list_l>();
}
const ric_request_id_s& ric_sub_mod_required_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_mod_required_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_actions_required_to_be_modified_list_l&
ric_sub_mod_required_ies_o::value_c::ric_actions_required_to_be_modified_list() const
{
  assert_choice_type(types::ric_actions_required_to_be_modified_list, type_, "Value");
  return c.get<ric_actions_required_to_be_modified_list_l>();
}
const ric_actions_required_to_be_remd_list_l&
ric_sub_mod_required_ies_o::value_c::ric_actions_required_to_be_remd_list() const
{
  assert_choice_type(types::ric_actions_required_to_be_remd_list, type_, "Value");
  return c.get<ric_actions_required_to_be_remd_list_l>();
}
void ric_sub_mod_required_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_actions_required_to_be_modified_list:
      j.start_array("RICactions-RequiredToBeModified-List");
      for (const auto& e1 : c.get<ric_actions_required_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_required_to_be_remd_list:
      j.start_array("RICactions-RequiredToBeRemoved-List");
      for (const auto& e1 : c.get<ric_actions_required_to_be_remd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_required_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_mod_required_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_required_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_required_to_be_modified_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_required_to_be_remd_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_required_to_be_remd_list_l>(), 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_required_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_required_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_required_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_required_to_be_modified_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_required_to_be_remd_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_required_to_be_remd_list_l>(), bref, 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_required_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_mod_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID",
                                "INTEGER (0..4095)",
                                "RICactions-RequiredToBeModified-List",
                                "RICactions-RequiredToBeRemoved-List"};
  return convert_enum_idx(names, 4, value, "ric_sub_mod_required_ies_o::value_c::types");
}
uint8_t ric_sub_mod_required_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_mod_required_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionModificationResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_mod_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 71, 73, 75, 77, 79, 81};
  return map_enum_number(names, 8, idx, "id");
}
bool ric_sub_mod_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 71, 73, 75, 77, 79, 81};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_mod_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 71:
      return crit_e::ignore;
    case 73:
      return crit_e::ignore;
    case 75:
      return crit_e::ignore;
    case 77:
      return crit_e::ignore;
    case 79:
      return crit_e::ignore;
    case 81:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_mod_resp_ies_o::value_c ric_sub_mod_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 71:
      ret.set(value_c::types::ric_actions_remd_for_mod_list);
      break;
    case 73:
      ret.set(value_c::types::ric_actions_failed_to_be_remd_for_mod_list);
      break;
    case 75:
      ret.set(value_c::types::ric_actions_modified_for_mod_list);
      break;
    case 77:
      ret.set(value_c::types::ric_actions_failed_to_be_modified_for_mod_list);
      break;
    case 79:
      ret.set(value_c::types::ric_actions_added_for_mod_list);
      break;
    case 81:
      ret.set(value_c::types::ric_actions_failed_to_be_added_for_mod_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_mod_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 71:
      return presence_e::optional;
    case 73:
      return presence_e::optional;
    case 75:
      return presence_e::optional;
    case 77:
      return presence_e::optional;
    case 79:
      return presence_e::optional;
    case 81:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_mod_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_actions_remd_for_mod_list:
      c = ric_actions_remd_for_mod_list_l{};
      break;
    case types::ric_actions_failed_to_be_remd_for_mod_list:
      c = ric_actions_failed_to_be_remd_for_mod_list_l{};
      break;
    case types::ric_actions_modified_for_mod_list:
      c = ric_actions_modified_for_mod_list_l{};
      break;
    case types::ric_actions_failed_to_be_modified_for_mod_list:
      c = ric_actions_failed_to_be_modified_for_mod_list_l{};
      break;
    case types::ric_actions_added_for_mod_list:
      c = ric_actions_added_for_mod_list_l{};
      break;
    case types::ric_actions_failed_to_be_added_for_mod_list:
      c = ric_actions_failed_to_be_added_for_mod_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_resp_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_mod_resp_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_mod_resp_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
ric_actions_remd_for_mod_list_l& ric_sub_mod_resp_ies_o::value_c::ric_actions_remd_for_mod_list()
{
  assert_choice_type(types::ric_actions_remd_for_mod_list, type_, "Value");
  return c.get<ric_actions_remd_for_mod_list_l>();
}
ric_actions_failed_to_be_remd_for_mod_list_l&
ric_sub_mod_resp_ies_o::value_c::ric_actions_failed_to_be_remd_for_mod_list()
{
  assert_choice_type(types::ric_actions_failed_to_be_remd_for_mod_list, type_, "Value");
  return c.get<ric_actions_failed_to_be_remd_for_mod_list_l>();
}
ric_actions_modified_for_mod_list_l& ric_sub_mod_resp_ies_o::value_c::ric_actions_modified_for_mod_list()
{
  assert_choice_type(types::ric_actions_modified_for_mod_list, type_, "Value");
  return c.get<ric_actions_modified_for_mod_list_l>();
}
ric_actions_failed_to_be_modified_for_mod_list_l&
ric_sub_mod_resp_ies_o::value_c::ric_actions_failed_to_be_modified_for_mod_list()
{
  assert_choice_type(types::ric_actions_failed_to_be_modified_for_mod_list, type_, "Value");
  return c.get<ric_actions_failed_to_be_modified_for_mod_list_l>();
}
ric_actions_added_for_mod_list_l& ric_sub_mod_resp_ies_o::value_c::ric_actions_added_for_mod_list()
{
  assert_choice_type(types::ric_actions_added_for_mod_list, type_, "Value");
  return c.get<ric_actions_added_for_mod_list_l>();
}
ric_actions_failed_to_be_added_for_mod_list_l&
ric_sub_mod_resp_ies_o::value_c::ric_actions_failed_to_be_added_for_mod_list()
{
  assert_choice_type(types::ric_actions_failed_to_be_added_for_mod_list, type_, "Value");
  return c.get<ric_actions_failed_to_be_added_for_mod_list_l>();
}
const ric_request_id_s& ric_sub_mod_resp_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_mod_resp_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_actions_remd_for_mod_list_l& ric_sub_mod_resp_ies_o::value_c::ric_actions_remd_for_mod_list() const
{
  assert_choice_type(types::ric_actions_remd_for_mod_list, type_, "Value");
  return c.get<ric_actions_remd_for_mod_list_l>();
}
const ric_actions_failed_to_be_remd_for_mod_list_l&
ric_sub_mod_resp_ies_o::value_c::ric_actions_failed_to_be_remd_for_mod_list() const
{
  assert_choice_type(types::ric_actions_failed_to_be_remd_for_mod_list, type_, "Value");
  return c.get<ric_actions_failed_to_be_remd_for_mod_list_l>();
}
const ric_actions_modified_for_mod_list_l& ric_sub_mod_resp_ies_o::value_c::ric_actions_modified_for_mod_list() const
{
  assert_choice_type(types::ric_actions_modified_for_mod_list, type_, "Value");
  return c.get<ric_actions_modified_for_mod_list_l>();
}
const ric_actions_failed_to_be_modified_for_mod_list_l&
ric_sub_mod_resp_ies_o::value_c::ric_actions_failed_to_be_modified_for_mod_list() const
{
  assert_choice_type(types::ric_actions_failed_to_be_modified_for_mod_list, type_, "Value");
  return c.get<ric_actions_failed_to_be_modified_for_mod_list_l>();
}
const ric_actions_added_for_mod_list_l& ric_sub_mod_resp_ies_o::value_c::ric_actions_added_for_mod_list() const
{
  assert_choice_type(types::ric_actions_added_for_mod_list, type_, "Value");
  return c.get<ric_actions_added_for_mod_list_l>();
}
const ric_actions_failed_to_be_added_for_mod_list_l&
ric_sub_mod_resp_ies_o::value_c::ric_actions_failed_to_be_added_for_mod_list() const
{
  assert_choice_type(types::ric_actions_failed_to_be_added_for_mod_list, type_, "Value");
  return c.get<ric_actions_failed_to_be_added_for_mod_list_l>();
}
void ric_sub_mod_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_actions_remd_for_mod_list:
      j.start_array("RICactions-RemovedForModification-List");
      for (const auto& e1 : c.get<ric_actions_remd_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_failed_to_be_remd_for_mod_list:
      j.start_array("RICactions-FailedToBeRemovedForModification-List");
      for (const auto& e1 : c.get<ric_actions_failed_to_be_remd_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_modified_for_mod_list:
      j.start_array("RICactions-ModifiedForModification-List");
      for (const auto& e1 : c.get<ric_actions_modified_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_failed_to_be_modified_for_mod_list:
      j.start_array("RICactions-FailedToBeModifiedForModification-List");
      for (const auto& e1 : c.get<ric_actions_failed_to_be_modified_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_added_for_mod_list:
      j.start_array("RICactions-AddedForModification-List");
      for (const auto& e1 : c.get<ric_actions_added_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_failed_to_be_added_for_mod_list:
      j.start_array("RICactions-FailedToBeAddedForModification-List");
      for (const auto& e1 : c.get<ric_actions_failed_to_be_added_for_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_mod_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_remd_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_remd_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_failed_to_be_remd_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_failed_to_be_remd_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_modified_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_modified_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_failed_to_be_modified_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_failed_to_be_modified_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_added_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_added_for_mod_list_l>(), 0, 16, true));
      break;
    case types::ric_actions_failed_to_be_added_for_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_actions_failed_to_be_added_for_mod_list_l>(), 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_remd_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_remd_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_failed_to_be_remd_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_failed_to_be_remd_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_modified_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_modified_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_failed_to_be_modified_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_failed_to_be_modified_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_added_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_added_for_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::ric_actions_failed_to_be_added_for_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_actions_failed_to_be_added_for_mod_list_l>(), bref, 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_mod_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID",
                                "INTEGER (0..4095)",
                                "RICactions-RemovedForModification-List",
                                "RICactions-FailedToBeRemovedForModification-List",
                                "RICactions-ModifiedForModification-List",
                                "RICactions-FailedToBeModifiedForModification-List",
                                "RICactions-AddedForModification-List",
                                "RICactions-FailedToBeAddedForModification-List"};
  return convert_enum_idx(names, 8, value, "ric_sub_mod_resp_ies_o::value_c::types");
}
uint8_t ric_sub_mod_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_mod_resp_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 30};
  return map_enum_number(names, 3, idx, "id");
}
bool ric_sub_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 30};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 30:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_request_ies_o::value_c ric_sub_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 30:
      ret.set(value_c::types::ric_sub_details);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 30:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_sub_details:
      c = ric_sub_details_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_request_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_request_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_request_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
ric_sub_details_s& ric_sub_request_ies_o::value_c::ric_sub_details()
{
  assert_choice_type(types::ric_sub_details, type_, "Value");
  return c.get<ric_sub_details_s>();
}
const ric_request_id_s& ric_sub_request_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_request_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_sub_details_s& ric_sub_request_ies_o::value_c::ric_sub_details() const
{
  assert_choice_type(types::ric_sub_details, type_, "Value");
  return c.get<ric_sub_details_s>();
}
void ric_sub_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_sub_details:
      j.write_fieldname("RICsubscriptionDetails");
      c.get<ric_sub_details_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_sub_details:
      HANDLE_CODE(c.get<ric_sub_details_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_sub_details:
      HANDLE_CODE(c.get<ric_sub_details_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICrequestID", "INTEGER (0..4095)", "RICsubscriptionDetails"};
  return convert_enum_idx(names, 3, value, "ric_sub_request_ies_o::value_c::types");
}
uint8_t ric_sub_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_request_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {29, 5, 17, 18};
  return map_enum_number(names, 4, idx, "id");
}
bool ric_sub_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {29, 5, 17, 18};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ric_sub_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 18:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ric_sub_resp_ies_o::value_c ric_sub_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ric_request_id);
      break;
    case 5:
      ret.set(value_c::types::ran_function_id);
      break;
    case 17:
      ret.set(value_c::types::ric_actions_admitted);
      break;
    case 18:
      ret.set(value_c::types::ric_actions_not_admitted);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 17:
      return presence_e::mandatory;
    case 18:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_request_id:
      c = ric_request_id_s{};
      break;
    case types::ran_function_id:
      c = uint16_t{};
      break;
    case types::ric_actions_admitted:
      c = ric_action_admitted_list_l{};
      break;
    case types::ric_actions_not_admitted:
      c = ric_action_not_admitted_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_resp_ies_o::value_c");
  }
}
ric_request_id_s& ric_sub_resp_ies_o::value_c::ric_request_id()
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
uint16_t& ric_sub_resp_ies_o::value_c::ran_function_id()
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
ric_action_admitted_list_l& ric_sub_resp_ies_o::value_c::ric_actions_admitted()
{
  assert_choice_type(types::ric_actions_admitted, type_, "Value");
  return c.get<ric_action_admitted_list_l>();
}
ric_action_not_admitted_list_l& ric_sub_resp_ies_o::value_c::ric_actions_not_admitted()
{
  assert_choice_type(types::ric_actions_not_admitted, type_, "Value");
  return c.get<ric_action_not_admitted_list_l>();
}
const ric_request_id_s& ric_sub_resp_ies_o::value_c::ric_request_id() const
{
  assert_choice_type(types::ric_request_id, type_, "Value");
  return c.get<ric_request_id_s>();
}
const uint16_t& ric_sub_resp_ies_o::value_c::ran_function_id() const
{
  assert_choice_type(types::ran_function_id, type_, "Value");
  return c.get<uint16_t>();
}
const ric_action_admitted_list_l& ric_sub_resp_ies_o::value_c::ric_actions_admitted() const
{
  assert_choice_type(types::ric_actions_admitted, type_, "Value");
  return c.get<ric_action_admitted_list_l>();
}
const ric_action_not_admitted_list_l& ric_sub_resp_ies_o::value_c::ric_actions_not_admitted() const
{
  assert_choice_type(types::ric_actions_not_admitted, type_, "Value");
  return c.get<ric_action_not_admitted_list_l>();
}
void ric_sub_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request_id:
      j.write_fieldname("RICrequestID");
      c.get<ric_request_id_s>().to_json(j);
      break;
    case types::ran_function_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ric_actions_admitted:
      j.start_array("RICaction-Admitted-List");
      for (const auto& e1 : c.get<ric_action_admitted_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ric_actions_not_admitted:
      j.start_array("RICaction-NotAdmitted-List");
      for (const auto& e1 : c.get<ric_action_not_admitted_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ric_sub_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().pack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_admitted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_action_admitted_list_l>(), 1, 16, true));
      break;
    case types::ric_actions_not_admitted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ric_action_not_admitted_list_l>(), 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_request_id:
      HANDLE_CODE(c.get<ric_request_id_s>().unpack(bref));
      break;
    case types::ran_function_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ric_actions_admitted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_action_admitted_list_l>(), bref, 1, 16, true));
      break;
    case types::ric_actions_not_admitted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ric_action_not_admitted_list_l>(), bref, 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ric_sub_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ric_sub_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "RICrequestID", "INTEGER (0..4095)", "RICaction-Admitted-List", "RICaction-NotAdmitted-List"};
  return convert_enum_idx(names, 4, value, "ric_sub_resp_ies_o::value_c::types");
}
uint8_t ric_sub_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == ran_function_id) {
    return 0;
  }
  invalid_enum_number(value, "ric_sub_resp_ies_o::value_c::types");
  return 0;
}

// E2RemovalRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2_removal_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {49};
  return map_enum_number(names, 1, idx, "id");
}
bool e2_removal_request_ies_o::is_id_valid(const uint32_t& id)
{
  return 49 == id;
}
crit_e e2_removal_request_ies_o::get_crit(const uint32_t& id)
{
  if (id == 49) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e2_removal_request_ies_o::value_c e2_removal_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 49) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e2_removal_request_ies_o::get_presence(const uint32_t& id)
{
  if (id == 49) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2_removal_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("INTEGER (0..255,...)", c);
  j.end_obj();
}
SRSASN_CODE e2_removal_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_integer(bref, c, (uint16_t)0u, (uint16_t)255u, true, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_integer(c, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  return SRSASN_SUCCESS;
}

const char* e2_removal_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)"};
  return convert_enum_idx(names, 1, value, "e2_removal_request_ies_o::value_c::types");
}
uint8_t e2_removal_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e2_removal_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e2_removal_fail_ies_o>;

template struct asn1::protocol_ie_field_s<e2_removal_resp_ies_o>;

template struct asn1::protocol_ie_field_s<e2conn_upd_ack_ies_o>;

template struct asn1::protocol_ie_field_s<e2conn_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<e2conn_upd_ies_o>;

template struct asn1::protocol_ie_field_s<e2node_cfg_upd_ack_ies_o>;

template struct asn1::protocol_ie_field_s<e2node_cfg_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<e2node_cfg_upd_ies_o>;

template struct asn1::protocol_ie_field_s<e2setup_fail_ies_o>;

template struct asn1::protocol_ie_field_s<e2setup_request_ies_o>;

template struct asn1::protocol_ie_field_s<e2setup_resp_ies_o>;

template struct asn1::protocol_ie_field_s<error_ind_ies_o>;

template struct asn1::protocol_ie_field_s<reset_request_ies_o>;

template struct asn1::protocol_ie_field_s<reset_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ric_ctrl_ack_ies_o>;

template struct asn1::protocol_ie_field_s<ric_ctrl_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ric_ctrl_request_ies_o>;

template struct asn1::protocol_ie_field_s<ric_ind_ies_o>;

template struct asn1::protocol_ie_field_s<ric_query_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ric_query_request_ies_o>;

template struct asn1::protocol_ie_field_s<ric_query_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ric_service_query_ies_o>;

template struct asn1::protocol_ie_field_s<ric_service_upd_ack_ies_o>;

template struct asn1::protocol_ie_field_s<ric_service_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ric_service_upd_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_delete_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_delete_request_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_delete_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_mod_confirm_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_mod_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_mod_refuse_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_mod_request_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_mod_required_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_mod_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_request_ies_o>;

template struct asn1::protocol_ie_field_s<ric_sub_resp_ies_o>;

// RICsubscriptionDeleteRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ric_sub_delete_required_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {60};
  return map_enum_number(names, 1, idx, "id");
}
bool ric_sub_delete_required_ies_o::is_id_valid(const uint32_t& id)
{
  return 60 == id;
}
crit_e ric_sub_delete_required_ies_o::get_crit(const uint32_t& id)
{
  if (id == 60) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ric_sub_delete_required_ies_o::value_c ric_sub_delete_required_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 60) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ric_sub_delete_required_ies_o::get_presence(const uint32_t& id)
{
  if (id == 60) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ric_sub_delete_required_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("RICsubscription-List-withCause");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE ric_sub_delete_required_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 1024, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_delete_required_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 1024, true));
  return SRSASN_SUCCESS;
}

const char* ric_sub_delete_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscription-List-withCause"};
  return convert_enum_idx(names, 1, value, "ric_sub_delete_required_ies_o::value_c::types");
}

SRSASN_CODE e2_removal_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2_removal_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 1);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE e2_removal_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2_removal_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE e2conn_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += e2conn_setup_present ? 1 : 0;
  nof_ies += e2conn_setup_failed_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (e2conn_setup_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)39, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2conn_setup, 1, 32, true));
  }
  if (e2conn_setup_failed_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2conn_setup_failed, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 39: {
        e2conn_setup_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2conn_setup, bref, 1, 32, true));
        break;
      }
      case 40: {
        e2conn_setup_failed_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2conn_setup_failed, bref, 1, 32, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2conn_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (e2conn_setup_present) {
    j.write_int("id", 39);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2conn_setup) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2conn_setup_failed_present) {
    j.write_int("id", 40);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2conn_setup_failed) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE e2conn_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += cause_present ? 1 : 0;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (cause_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)31, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        cause_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2conn_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (cause_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "reject");
    cause.to_json(j);
  }
  if (time_to_wait_present) {
    j.write_int("id", 31);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE e2conn_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += e2conn_upd_add_present ? 1 : 0;
  nof_ies += e2conn_upd_rem_present ? 1 : 0;
  nof_ies += e2conn_upd_modify_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (e2conn_upd_add_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)44, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2conn_upd_add, 1, 32, true));
  }
  if (e2conn_upd_rem_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)46, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2conn_upd_rem, 1, 32, true));
  }
  if (e2conn_upd_modify_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)45, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2conn_upd_modify, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 44: {
        e2conn_upd_add_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2conn_upd_add, bref, 1, 32, true));
        break;
      }
      case 46: {
        e2conn_upd_rem_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2conn_upd_rem, bref, 1, 32, true));
        break;
      }
      case 45: {
        e2conn_upd_modify_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2conn_upd_modify, bref, 1, 32, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2conn_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (e2conn_upd_add_present) {
    j.write_int("id", 44);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2conn_upd_add) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2conn_upd_rem_present) {
    j.write_int("id", 46);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2conn_upd_rem) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2conn_upd_modify_present) {
    j.write_int("id", 45);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2conn_upd_modify) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE e2node_cfg_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += e2node_component_cfg_addition_ack_present ? 1 : 0;
  nof_ies += e2node_component_cfg_upd_ack_present ? 1 : 0;
  nof_ies += e2node_component_cfg_removal_ack_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (e2node_component_cfg_addition_ack_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)52, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_addition_ack, 1, 1024, true));
  }
  if (e2node_component_cfg_upd_ack_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)35, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_upd_ack, 1, 1024, true));
  }
  if (e2node_component_cfg_removal_ack_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)56, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_removal_ack, 1, 1024, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 52: {
        e2node_component_cfg_addition_ack_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_addition_ack, bref, 1, 1024, true));
        break;
      }
      case 35: {
        e2node_component_cfg_upd_ack_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_upd_ack, bref, 1, 1024, true));
        break;
      }
      case 56: {
        e2node_component_cfg_removal_ack_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_removal_ack, bref, 1, 1024, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2node_cfg_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (e2node_component_cfg_addition_ack_present) {
    j.write_int("id", 52);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2node_component_cfg_addition_ack) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2node_component_cfg_upd_ack_present) {
    j.write_int("id", 35);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2node_component_cfg_upd_ack) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2node_component_cfg_removal_ack_present) {
    j.write_int("id", 56);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2node_component_cfg_removal_ack) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE e2node_cfg_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)31, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2node_cfg_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 1);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 31);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE e2node_cfg_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += global_e2node_id_present ? 1 : 0;
  nof_ies += e2node_component_cfg_addition_present ? 1 : 0;
  nof_ies += e2node_component_cfg_upd_present ? 1 : 0;
  nof_ies += e2node_component_cfg_removal_present ? 1 : 0;
  nof_ies += e2node_tnl_assoc_removal_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (global_e2node_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(global_e2node_id.pack(bref));
  }
  if (e2node_component_cfg_addition_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_addition, 1, 1024, true));
  }
  if (e2node_component_cfg_upd_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)33, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_upd, 1, 1024, true));
  }
  if (e2node_component_cfg_removal_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)54, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_removal, 1, 1024, true));
  }
  if (e2node_tnl_assoc_removal_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)58, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_tnl_assoc_removal, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 3: {
        global_e2node_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(global_e2node_id.unpack(bref));
        break;
      }
      case 50: {
        e2node_component_cfg_addition_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_addition, bref, 1, 1024, true));
        break;
      }
      case 33: {
        e2node_component_cfg_upd_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_upd, bref, 1, 1024, true));
        break;
      }
      case 54: {
        e2node_component_cfg_removal_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_removal, bref, 1, 1024, true));
        break;
      }
      case 58: {
        e2node_tnl_assoc_removal_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_tnl_assoc_removal, bref, 1, 32, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2node_cfg_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (global_e2node_id_present) {
    j.write_int("id", 3);
    j.write_str("criticality", "reject");
    global_e2node_id.to_json(j);
  }
  if (e2node_component_cfg_addition_present) {
    j.write_int("id", 50);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2node_component_cfg_addition) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2node_component_cfg_upd_present) {
    j.write_int("id", 33);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2node_component_cfg_upd) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2node_component_cfg_removal_present) {
    j.write_int("id", 54);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2node_component_cfg_removal) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (e2node_tnl_assoc_removal_present) {
    j.write_int("id", 58);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : e2node_tnl_assoc_removal) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE e2setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += tnl_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)31, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (tnl_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)48, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(tnl_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 48: {
        tnl_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(tnl_info.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 1);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 31);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (tnl_info_present) {
    j.write_int("id", 48);
    j.write_str("criticality", "ignore");
    tnl_info.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE e2setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(global_e2node_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)10, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_added, 1, 256, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_addition, 1, 1024, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(global_e2node_id.unpack(bref));
        break;
      }
      case 10: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_added, bref, 1, 256, true));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_addition, bref, 1, 1024, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2setup_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  global_e2node_id.to_json(j);
  j.write_int("id", 10);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : ran_functions_added) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("id", 50);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : e2node_component_cfg_addition) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

SRSASN_CODE e2setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += ran_functions_accepted_present ? 1 : 0;
  nof_ies += ran_functions_rejected_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)4, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(global_ric_id.pack(bref));
  }
  if (ran_functions_accepted_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_accepted, 1, 256, true));
  }
  if (ran_functions_rejected_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)13, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_rejected, 1, 256, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)52, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, e2node_component_cfg_addition_ack, 1, 1024, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 4: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(global_ric_id.unpack(bref));
        break;
      }
      case 9: {
        ran_functions_accepted_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_accepted, bref, 1, 256, true));
        break;
      }
      case 13: {
        ran_functions_rejected_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_rejected, bref, 1, 256, true));
        break;
      }
      case 52: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(e2node_component_cfg_addition_ack, bref, 1, 1024, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2setup_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 4);
  j.write_str("criticality", "reject");
  global_ric_id.to_json(j);
  if (ran_functions_accepted_present) {
    j.write_int("id", 9);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_accepted) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_functions_rejected_present) {
    j.write_int("id", 13);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_rejected) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("id", 52);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : e2node_component_cfg_addition_ack) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

SRSASN_CODE error_ind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += transaction_id_present ? 1 : 0;
  nof_ies += ric_request_id_present ? 1 : 0;
  nof_ies += ran_function_id_present ? 1 : 0;
  nof_ies += cause_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (transaction_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ric_request_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  if (ran_function_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (cause_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE error_ind_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        transaction_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 29: {
        ric_request_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        ran_function_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 1: {
        cause_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void error_ind_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (transaction_id_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "reject");
    j.write_int("Value", transaction_id);
  }
  if (ric_request_id_present) {
    j.write_int("id", 29);
    j.write_str("criticality", "reject");
    ric_request_id.to_json(j);
  }
  if (ran_function_id_present) {
    j.write_int("id", 5);
    j.write_str("criticality", "reject");
    j.write_int("Value", ran_function_id);
  }
  if (cause_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    cause.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE reset_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void reset_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 1);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  j.end_obj();
}

SRSASN_CODE reset_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void reset_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE ric_ctrl_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ric_call_process_id_present ? 1 : 0;
  nof_ies += ric_ctrl_outcome_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (ric_call_process_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)20, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_call_process_id.pack(bref));
  }
  if (ric_ctrl_outcome_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)32, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ctrl_outcome.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 20: {
        ric_call_process_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_call_process_id.unpack(bref));
        break;
      }
      case 32: {
        ric_ctrl_outcome_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ctrl_outcome.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_ctrl_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  if (ric_call_process_id_present) {
    j.write_int("id", 20);
    j.write_str("criticality", "reject");
    j.write_str("Value", ric_call_process_id.to_string());
  }
  if (ric_ctrl_outcome_present) {
    j.write_int("id", 32);
    j.write_str("criticality", "reject");
    j.write_str("Value", ric_ctrl_outcome.to_string());
  }
  j.end_obj();
}

SRSASN_CODE ric_ctrl_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += ric_call_process_id_present ? 1 : 0;
  nof_ies += ric_ctrl_outcome_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (ric_call_process_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)20, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_call_process_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (ric_ctrl_outcome_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)32, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ctrl_outcome.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 20: {
        ric_call_process_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_call_process_id.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 32: {
        ric_ctrl_outcome_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ctrl_outcome.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_ctrl_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  if (ric_call_process_id_present) {
    j.write_int("id", 20);
    j.write_str("criticality", "reject");
    j.write_str("Value", ric_call_process_id.to_string());
  }
  j.write_int("id", 1);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (ric_ctrl_outcome_present) {
    j.write_int("id", 32);
    j.write_str("criticality", "reject");
    j.write_str("Value", ric_ctrl_outcome.to_string());
  }
  j.end_obj();
}

SRSASN_CODE ric_ctrl_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += ric_call_process_id_present ? 1 : 0;
  nof_ies += ric_ctrl_ack_request_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (ric_call_process_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)20, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_call_process_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)22, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ctrl_hdr.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)23, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ctrl_msg.pack(bref));
  }
  if (ric_ctrl_ack_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)21, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ctrl_ack_request.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ctrl_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 20: {
        ric_call_process_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_call_process_id.unpack(bref));
        break;
      }
      case 22: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ctrl_hdr.unpack(bref));
        break;
      }
      case 23: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ctrl_msg.unpack(bref));
        break;
      }
      case 21: {
        ric_ctrl_ack_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ctrl_ack_request.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_ctrl_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  if (ric_call_process_id_present) {
    j.write_int("id", 20);
    j.write_str("criticality", "reject");
    j.write_str("Value", ric_call_process_id.to_string());
  }
  j.write_int("id", 22);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_ctrl_hdr.to_string());
  j.write_int("id", 23);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_ctrl_msg.to_string());
  if (ric_ctrl_ack_request_present) {
    j.write_int("id", 21);
    j.write_str("criticality", "reject");
    j.write_str("Value", ric_ctrl_ack_request.to_string());
  }
  j.end_obj();
}

SRSASN_CODE ric_ind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 6;
  nof_ies += ric_ind_sn_present ? 1 : 0;
  nof_ies += ric_call_process_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)15, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (ric_ind_sn_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)27, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ric_ind_sn, (uint32_t)0u, (uint32_t)65535u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)28, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ind_type.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)25, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ind_hdr.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)26, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_ind_msg.pack(bref));
  }
  if (ric_call_process_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)20, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_call_process_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_ind_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 6;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 15: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
        break;
      }
      case 27: {
        ric_ind_sn_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ric_ind_sn, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
        break;
      }
      case 28: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ind_type.unpack(bref));
        break;
      }
      case 25: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ind_hdr.unpack(bref));
        break;
      }
      case 26: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_ind_msg.unpack(bref));
        break;
      }
      case 20: {
        ric_call_process_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_call_process_id.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_ind_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 15);
  j.write_str("criticality", "reject");
  j.write_int("Value", ric_action_id);
  if (ric_ind_sn_present) {
    j.write_int("id", 27);
    j.write_str("criticality", "reject");
    j.write_int("Value", ric_ind_sn);
  }
  j.write_int("id", 28);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_ind_type.to_string());
  j.write_int("id", 25);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_ind_hdr.to_string());
  j.write_int("id", 26);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_ind_msg.to_string());
  if (ric_call_process_id_present) {
    j.write_int("id", 20);
    j.write_str("criticality", "reject");
    j.write_str("Value", ric_call_process_id.to_string());
  }
  j.end_obj();
}

SRSASN_CODE ric_query_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_query_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 1);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE ric_query_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)95, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_query_hdr.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)96, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_query_definition.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 95: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_query_hdr.unpack(bref));
        break;
      }
      case 96: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_query_definition.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_query_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 95);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_query_hdr.to_string());
  j.write_int("id", 96);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_query_definition.to_string());
  j.end_obj();
}

SRSASN_CODE ric_query_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)97, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_query_outcome.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_query_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 97: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_query_outcome.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_query_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 97);
  j.write_str("criticality", "reject");
  j.write_str("Value", ric_query_outcome.to_string());
  j.end_obj();
}

SRSASN_CODE ric_service_query_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ran_functions_accepted_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ran_functions_accepted_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_accepted, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_query_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 9: {
        ran_functions_accepted_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_accepted, bref, 1, 256, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_service_query_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (ran_functions_accepted_present) {
    j.write_int("id", 9);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_accepted) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE ric_service_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ran_functions_accepted_present ? 1 : 0;
  nof_ies += ran_functions_rejected_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ran_functions_accepted_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_accepted, 1, 256, true));
  }
  if (ran_functions_rejected_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)13, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_rejected, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_upd_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 9: {
        ran_functions_accepted_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_accepted, bref, 1, 256, true));
        break;
      }
      case 13: {
        ran_functions_rejected_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_rejected, bref, 1, 256, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_service_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (ran_functions_accepted_present) {
    j.write_int("id", 9);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_accepted) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_functions_rejected_present) {
    j.write_int("id", 13);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_rejected) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE ric_service_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)31, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_upd_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_service_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 1);
  j.write_str("criticality", "reject");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 31);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE ric_service_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ran_functions_added_present ? 1 : 0;
  nof_ies += ran_functions_modified_present ? 1 : 0;
  nof_ies += ran_functions_deleted_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ran_functions_added_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)10, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_added, 1, 256, true));
  }
  if (ran_functions_modified_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)12, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_modified, 1, 256, true));
  }
  if (ran_functions_deleted_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)11, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ran_functions_deleted, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_service_upd_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 10: {
        ran_functions_added_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_added, bref, 1, 256, true));
        break;
      }
      case 12: {
        ran_functions_modified_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_modified, bref, 1, 256, true));
        break;
      }
      case 11: {
        ran_functions_deleted_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ran_functions_deleted, bref, 1, 256, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_service_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 49);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (ran_functions_added_present) {
    j.write_int("id", 10);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_added) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_functions_modified_present) {
    j.write_int("id", 12);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_modified) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_functions_deleted_present) {
    j.write_int("id", 11);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ran_functions_deleted) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_delete_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_delete_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_delete_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 1);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_delete_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_delete_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_delete_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.end_obj();
}

SRSASN_CODE ric_sub_delete_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_delete_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_delete_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.end_obj();
}

SRSASN_CODE ric_sub_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 1);
  j.write_str("criticality", "reject");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_mod_confirm_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ric_actions_confirmed_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_refused_to_be_modified_list_present ? 1 : 0;
  nof_ies += ric_actions_confirmed_for_removal_list_present ? 1 : 0;
  nof_ies += ric_actions_refused_to_be_remd_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (ric_actions_confirmed_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)87, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_confirmed_for_mod_list, 0, 16, true));
  }
  if (ric_actions_refused_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)89, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_refused_to_be_modified_list, 0, 16, true));
  }
  if (ric_actions_confirmed_for_removal_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)91, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_confirmed_for_removal_list, 0, 16, true));
  }
  if (ric_actions_refused_to_be_remd_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)93, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_refused_to_be_remd_list, 0, 16, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_confirm_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 87: {
        ric_actions_confirmed_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_confirmed_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 89: {
        ric_actions_refused_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_refused_to_be_modified_list, bref, 0, 16, true));
        break;
      }
      case 91: {
        ric_actions_confirmed_for_removal_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_confirmed_for_removal_list, bref, 0, 16, true));
        break;
      }
      case 93: {
        ric_actions_refused_to_be_remd_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_refused_to_be_remd_list, bref, 0, 16, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_mod_confirm_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  if (ric_actions_confirmed_for_mod_list_present) {
    j.write_int("id", 87);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_confirmed_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_refused_to_be_modified_list_present) {
    j.write_int("id", 89);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_refused_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_confirmed_for_removal_list_present) {
    j.write_int("id", 91);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_confirmed_for_removal_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_refused_to_be_remd_list_present) {
    j.write_int("id", 93);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_refused_to_be_remd_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_mod_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_mod_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 1);
  j.write_str("criticality", "reject");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_mod_refuse_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_refuse_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_mod_refuse_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 1);
  j.write_str("criticality", "reject");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_mod_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ric_event_trigger_definition_to_be_modified_present ? 1 : 0;
  nof_ies += ric_actions_to_be_remd_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_to_be_modified_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_to_be_added_for_mod_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (ric_event_trigger_definition_to_be_modified_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)64, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_event_trigger_definition_to_be_modified.pack(bref));
  }
  if (ric_actions_to_be_remd_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)65, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_to_be_remd_for_mod_list, 0, 16, true));
  }
  if (ric_actions_to_be_modified_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)67, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_to_be_modified_for_mod_list, 0, 16, true));
  }
  if (ric_actions_to_be_added_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)69, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_to_be_added_for_mod_list, 0, 16, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 64: {
        ric_event_trigger_definition_to_be_modified_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_event_trigger_definition_to_be_modified.unpack(bref));
        break;
      }
      case 65: {
        ric_actions_to_be_remd_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_to_be_remd_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 67: {
        ric_actions_to_be_modified_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_to_be_modified_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 69: {
        ric_actions_to_be_added_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_to_be_added_for_mod_list, bref, 0, 16, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_mod_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  if (ric_event_trigger_definition_to_be_modified_present) {
    j.write_int("id", 64);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ric_event_trigger_definition_to_be_modified.to_string());
  }
  if (ric_actions_to_be_remd_for_mod_list_present) {
    j.write_int("id", 65);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_to_be_remd_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_to_be_modified_for_mod_list_present) {
    j.write_int("id", 67);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_to_be_modified_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_to_be_added_for_mod_list_present) {
    j.write_int("id", 69);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_to_be_added_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_mod_required_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ric_actions_required_to_be_modified_list_present ? 1 : 0;
  nof_ies += ric_actions_required_to_be_remd_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (ric_actions_required_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)83, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_required_to_be_modified_list, 0, 16, true));
  }
  if (ric_actions_required_to_be_remd_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)85, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_required_to_be_remd_list, 0, 16, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_required_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 83: {
        ric_actions_required_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_required_to_be_modified_list, bref, 0, 16, true));
        break;
      }
      case 85: {
        ric_actions_required_to_be_remd_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_required_to_be_remd_list, bref, 0, 16, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_mod_required_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  if (ric_actions_required_to_be_modified_list_present) {
    j.write_int("id", 83);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_required_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_required_to_be_remd_list_present) {
    j.write_int("id", 85);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_required_to_be_remd_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_mod_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ric_actions_remd_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_failed_to_be_remd_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_modified_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_failed_to_be_modified_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_added_for_mod_list_present ? 1 : 0;
  nof_ies += ric_actions_failed_to_be_added_for_mod_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (ric_actions_remd_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)71, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_remd_for_mod_list, 0, 16, true));
  }
  if (ric_actions_failed_to_be_remd_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)73, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_failed_to_be_remd_for_mod_list, 0, 16, true));
  }
  if (ric_actions_modified_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)75, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_modified_for_mod_list, 0, 16, true));
  }
  if (ric_actions_failed_to_be_modified_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)77, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_failed_to_be_modified_for_mod_list, 0, 16, true));
  }
  if (ric_actions_added_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)79, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_added_for_mod_list, 0, 16, true));
  }
  if (ric_actions_failed_to_be_added_for_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)81, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_failed_to_be_added_for_mod_list, 0, 16, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_mod_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 71: {
        ric_actions_remd_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_remd_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 73: {
        ric_actions_failed_to_be_remd_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_failed_to_be_remd_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 75: {
        ric_actions_modified_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_modified_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 77: {
        ric_actions_failed_to_be_modified_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_failed_to_be_modified_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 79: {
        ric_actions_added_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_added_for_mod_list, bref, 0, 16, true));
        break;
      }
      case 81: {
        ric_actions_failed_to_be_added_for_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_failed_to_be_added_for_mod_list, bref, 0, 16, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_mod_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  if (ric_actions_remd_for_mod_list_present) {
    j.write_int("id", 71);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_remd_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_failed_to_be_remd_for_mod_list_present) {
    j.write_int("id", 73);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_failed_to_be_remd_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_modified_for_mod_list_present) {
    j.write_int("id", 75);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_modified_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_failed_to_be_modified_for_mod_list_present) {
    j.write_int("id", 77);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_failed_to_be_modified_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_added_for_mod_list_present) {
    j.write_int("id", 79);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_added_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_actions_failed_to_be_added_for_mod_list_present) {
    j.write_int("id", 81);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_failed_to_be_added_for_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

SRSASN_CODE ric_sub_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)30, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_sub_details.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 30: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_sub_details.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 30);
  j.write_str("criticality", "reject");
  ric_sub_details.to_json(j);
  j.end_obj();
}

SRSASN_CODE ric_sub_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += ric_actions_not_admitted_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ric_request_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)5, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)17, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_admitted, 1, 16, true));
  }
  if (ric_actions_not_admitted_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)18, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_actions_not_admitted, 0, 16, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_sub_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ric_request_id.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
        break;
      }
      case 17: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_admitted, bref, 1, 16, true));
        break;
      }
      case 18: {
        ric_actions_not_admitted_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ric_actions_not_admitted, bref, 0, 16, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ric_sub_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 29);
  j.write_str("criticality", "reject");
  ric_request_id.to_json(j);
  j.write_int("id", 5);
  j.write_str("criticality", "reject");
  j.write_int("Value", ran_function_id);
  j.write_int("id", 17);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : ric_actions_admitted) {
    e1.to_json(j);
  }
  j.end_array();
  if (ric_actions_not_admitted_present) {
    j.write_int("id", 18);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ric_actions_not_admitted) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// E2AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
uint16_t e2ap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {8, 9, 14, 15, 16, 7, 4, 1, 10, 11, 3, 13, 5, 6, 2, 12};
  return map_enum_number(names, 16, idx, "proc_code");
}
bool e2ap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {8, 9, 14, 15, 16, 7, 4, 1, 10, 11, 3, 13, 5, 6, 2, 12};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
e2ap_elem_procs_o::init_msg_c e2ap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(init_msg_c::types::ric_sub_request);
      break;
    case 9:
      ret.set(init_msg_c::types::ric_sub_delete_request);
      break;
    case 14:
      ret.set(init_msg_c::types::ric_sub_mod_request);
      break;
    case 15:
      ret.set(init_msg_c::types::ric_sub_mod_required);
      break;
    case 16:
      ret.set(init_msg_c::types::ric_query_request);
      break;
    case 7:
      ret.set(init_msg_c::types::ric_service_upd);
      break;
    case 4:
      ret.set(init_msg_c::types::ric_ctrl_request);
      break;
    case 1:
      ret.set(init_msg_c::types::e2setup_request);
      break;
    case 10:
      ret.set(init_msg_c::types::e2node_cfg_upd);
      break;
    case 11:
      ret.set(init_msg_c::types::e2conn_upd);
      break;
    case 3:
      ret.set(init_msg_c::types::reset_request);
      break;
    case 13:
      ret.set(init_msg_c::types::e2_removal_request);
      break;
    case 5:
      ret.set(init_msg_c::types::ric_ind);
      break;
    case 6:
      ret.set(init_msg_c::types::ric_service_query);
      break;
    case 2:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 12:
      ret.set(init_msg_c::types::ric_sub_delete_required);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_o::successful_outcome_c e2ap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(successful_outcome_c::types::ric_sub_resp);
      break;
    case 9:
      ret.set(successful_outcome_c::types::ric_sub_delete_resp);
      break;
    case 14:
      ret.set(successful_outcome_c::types::ric_sub_mod_resp);
      break;
    case 15:
      ret.set(successful_outcome_c::types::ric_sub_mod_confirm);
      break;
    case 16:
      ret.set(successful_outcome_c::types::ric_query_resp);
      break;
    case 7:
      ret.set(successful_outcome_c::types::ric_service_upd_ack);
      break;
    case 4:
      ret.set(successful_outcome_c::types::ric_ctrl_ack);
      break;
    case 1:
      ret.set(successful_outcome_c::types::e2setup_resp);
      break;
    case 10:
      ret.set(successful_outcome_c::types::e2node_cfg_upd_ack);
      break;
    case 11:
      ret.set(successful_outcome_c::types::e2conn_upd_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::reset_resp);
      break;
    case 13:
      ret.set(successful_outcome_c::types::e2_removal_resp);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_o::unsuccessful_outcome_c e2ap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(unsuccessful_outcome_c::types::ric_sub_fail);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::ric_sub_delete_fail);
      break;
    case 14:
      ret.set(unsuccessful_outcome_c::types::ric_sub_mod_fail);
      break;
    case 15:
      ret.set(unsuccessful_outcome_c::types::ric_sub_mod_refuse);
      break;
    case 16:
      ret.set(unsuccessful_outcome_c::types::ric_query_fail);
      break;
    case 7:
      ret.set(unsuccessful_outcome_c::types::ric_service_upd_fail);
      break;
    case 4:
      ret.set(unsuccessful_outcome_c::types::ric_ctrl_fail);
      break;
    case 1:
      ret.set(unsuccessful_outcome_c::types::e2setup_fail);
      break;
    case 10:
      ret.set(unsuccessful_outcome_c::types::e2node_cfg_upd_fail);
      break;
    case 11:
      ret.set(unsuccessful_outcome_c::types::e2conn_upd_fail);
      break;
    case 13:
      ret.set(unsuccessful_outcome_c::types::e2_removal_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e e2ap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 8:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 16:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 5:
      return crit_e::ignore;
    case 6:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void e2ap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_request:
      c = ric_sub_request_s{};
      break;
    case types::ric_sub_delete_request:
      c = ric_sub_delete_request_s{};
      break;
    case types::ric_sub_mod_request:
      c = ric_sub_mod_request_s{};
      break;
    case types::ric_sub_mod_required:
      c = ric_sub_mod_required_s{};
      break;
    case types::ric_query_request:
      c = ric_query_request_s{};
      break;
    case types::ric_service_upd:
      c = ric_service_upd_s{};
      break;
    case types::ric_ctrl_request:
      c = ric_ctrl_request_s{};
      break;
    case types::e2setup_request:
      c = e2setup_request_s{};
      break;
    case types::e2node_cfg_upd:
      c = e2node_cfg_upd_s{};
      break;
    case types::e2conn_upd:
      c = e2conn_upd_s{};
      break;
    case types::reset_request:
      c = reset_request_s{};
      break;
    case types::e2_removal_request:
      c = e2_removal_request_s{};
      break;
    case types::ric_ind:
      c = ric_ind_s{};
      break;
    case types::ric_service_query:
      c = ric_service_query_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::ric_sub_delete_required:
      c = ric_sub_delete_required_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
  }
}
ric_sub_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_request()
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
ric_sub_delete_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_request()
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
ric_sub_mod_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_request()
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
ric_sub_mod_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_required()
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
ric_query_request_s& e2ap_elem_procs_o::init_msg_c::ric_query_request()
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
ric_service_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_upd()
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
ric_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_ctrl_request()
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
e2setup_request_s& e2ap_elem_procs_o::init_msg_c::e2setup_request()
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
e2node_cfg_upd_s& e2ap_elem_procs_o::init_msg_c::e2node_cfg_upd()
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
e2conn_upd_s& e2ap_elem_procs_o::init_msg_c::e2conn_upd()
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
reset_request_s& e2ap_elem_procs_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
e2_removal_request_s& e2ap_elem_procs_o::init_msg_c::e2_removal_request()
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
ric_ind_s& e2ap_elem_procs_o::init_msg_c::ric_ind()
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
ric_service_query_s& e2ap_elem_procs_o::init_msg_c::ric_service_query()
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
error_ind_s& e2ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ric_sub_delete_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_required()
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
const ric_sub_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_request() const
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
const ric_sub_delete_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_request() const
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
const ric_sub_mod_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_request() const
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
const ric_sub_mod_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_required() const
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
const ric_query_request_s& e2ap_elem_procs_o::init_msg_c::ric_query_request() const
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
const ric_service_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_upd() const
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
const ric_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_ctrl_request() const
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
const e2setup_request_s& e2ap_elem_procs_o::init_msg_c::e2setup_request() const
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
const e2node_cfg_upd_s& e2ap_elem_procs_o::init_msg_c::e2node_cfg_upd() const
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
const e2conn_upd_s& e2ap_elem_procs_o::init_msg_c::e2conn_upd() const
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
const reset_request_s& e2ap_elem_procs_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const e2_removal_request_s& e2ap_elem_procs_o::init_msg_c::e2_removal_request() const
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
const ric_ind_s& e2ap_elem_procs_o::init_msg_c::ric_ind() const
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
const ric_service_query_s& e2ap_elem_procs_o::init_msg_c::ric_service_query() const
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
const error_ind_s& e2ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ric_sub_delete_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_required() const
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
void e2ap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_request:
      j.write_fieldname("RICsubscriptionRequest");
      c.get<ric_sub_request_s>().to_json(j);
      break;
    case types::ric_sub_delete_request:
      j.write_fieldname("RICsubscriptionDeleteRequest");
      c.get<ric_sub_delete_request_s>().to_json(j);
      break;
    case types::ric_sub_mod_request:
      j.write_fieldname("RICsubscriptionModificationRequest");
      c.get<ric_sub_mod_request_s>().to_json(j);
      break;
    case types::ric_sub_mod_required:
      j.write_fieldname("RICsubscriptionModificationRequired");
      c.get<ric_sub_mod_required_s>().to_json(j);
      break;
    case types::ric_query_request:
      j.write_fieldname("RICQueryRequest");
      c.get<ric_query_request_s>().to_json(j);
      break;
    case types::ric_service_upd:
      j.write_fieldname("RICserviceUpdate");
      c.get<ric_service_upd_s>().to_json(j);
      break;
    case types::ric_ctrl_request:
      j.write_fieldname("RICcontrolRequest");
      c.get<ric_ctrl_request_s>().to_json(j);
      break;
    case types::e2setup_request:
      j.write_fieldname("E2setupRequest");
      c.get<e2setup_request_s>().to_json(j);
      break;
    case types::e2node_cfg_upd:
      j.write_fieldname("E2nodeConfigurationUpdate");
      c.get<e2node_cfg_upd_s>().to_json(j);
      break;
    case types::e2conn_upd:
      j.write_fieldname("E2connectionUpdate");
      c.get<e2conn_upd_s>().to_json(j);
      break;
    case types::reset_request:
      j.write_fieldname("ResetRequest");
      c.get<reset_request_s>().to_json(j);
      break;
    case types::e2_removal_request:
      j.write_fieldname("E2RemovalRequest");
      c.get<e2_removal_request_s>().to_json(j);
      break;
    case types::ric_ind:
      j.write_fieldname("RICindication");
      c.get<ric_ind_s>().to_json(j);
      break;
    case types::ric_service_query:
      j.write_fieldname("RICserviceQuery");
      c.get<ric_service_query_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::ric_sub_delete_required:
      j.write_fieldname("RICsubscriptionDeleteRequired");
      c.get<ric_sub_delete_required_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_request:
      HANDLE_CODE(c.get<ric_sub_request_s>().pack(bref));
      break;
    case types::ric_sub_delete_request:
      HANDLE_CODE(c.get<ric_sub_delete_request_s>().pack(bref));
      break;
    case types::ric_sub_mod_request:
      HANDLE_CODE(c.get<ric_sub_mod_request_s>().pack(bref));
      break;
    case types::ric_sub_mod_required:
      HANDLE_CODE(c.get<ric_sub_mod_required_s>().pack(bref));
      break;
    case types::ric_query_request:
      HANDLE_CODE(c.get<ric_query_request_s>().pack(bref));
      break;
    case types::ric_service_upd:
      HANDLE_CODE(c.get<ric_service_upd_s>().pack(bref));
      break;
    case types::ric_ctrl_request:
      HANDLE_CODE(c.get<ric_ctrl_request_s>().pack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().pack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().pack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().pack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().pack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().pack(bref));
      break;
    case types::ric_ind:
      HANDLE_CODE(c.get<ric_ind_s>().pack(bref));
      break;
    case types::ric_service_query:
      HANDLE_CODE(c.get<ric_service_query_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::ric_sub_delete_required:
      HANDLE_CODE(c.get<ric_sub_delete_required_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_request:
      HANDLE_CODE(c.get<ric_sub_request_s>().unpack(bref));
      break;
    case types::ric_sub_delete_request:
      HANDLE_CODE(c.get<ric_sub_delete_request_s>().unpack(bref));
      break;
    case types::ric_sub_mod_request:
      HANDLE_CODE(c.get<ric_sub_mod_request_s>().unpack(bref));
      break;
    case types::ric_sub_mod_required:
      HANDLE_CODE(c.get<ric_sub_mod_required_s>().unpack(bref));
      break;
    case types::ric_query_request:
      HANDLE_CODE(c.get<ric_query_request_s>().unpack(bref));
      break;
    case types::ric_service_upd:
      HANDLE_CODE(c.get<ric_service_upd_s>().unpack(bref));
      break;
    case types::ric_ctrl_request:
      HANDLE_CODE(c.get<ric_ctrl_request_s>().unpack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().unpack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().unpack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().unpack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().unpack(bref));
      break;
    case types::ric_ind:
      HANDLE_CODE(c.get<ric_ind_s>().unpack(bref));
      break;
    case types::ric_service_query:
      HANDLE_CODE(c.get<ric_service_query_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::ric_sub_delete_required:
      HANDLE_CODE(c.get<ric_sub_delete_required_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionRequest",
                                "RICsubscriptionDeleteRequest",
                                "RICsubscriptionModificationRequest",
                                "RICsubscriptionModificationRequired",
                                "RICQueryRequest",
                                "RICserviceUpdate",
                                "RICcontrolRequest",
                                "E2setupRequest",
                                "E2nodeConfigurationUpdate",
                                "E2connectionUpdate",
                                "ResetRequest",
                                "E2RemovalRequest",
                                "RICindication",
                                "RICserviceQuery",
                                "ErrorIndication",
                                "RICsubscriptionDeleteRequired"};
  return convert_enum_idx(names, 16, value, "e2ap_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_resp:
      c = ric_sub_resp_s{};
      break;
    case types::ric_sub_delete_resp:
      c = ric_sub_delete_resp_s{};
      break;
    case types::ric_sub_mod_resp:
      c = ric_sub_mod_resp_s{};
      break;
    case types::ric_sub_mod_confirm:
      c = ric_sub_mod_confirm_s{};
      break;
    case types::ric_query_resp:
      c = ric_query_resp_s{};
      break;
    case types::ric_service_upd_ack:
      c = ric_service_upd_ack_s{};
      break;
    case types::ric_ctrl_ack:
      c = ric_ctrl_ack_s{};
      break;
    case types::e2setup_resp:
      c = e2setup_resp_s{};
      break;
    case types::e2node_cfg_upd_ack:
      c = e2node_cfg_upd_ack_s{};
      break;
    case types::e2conn_upd_ack:
      c = e2conn_upd_ack_s{};
      break;
    case types::reset_resp:
      c = reset_resp_s{};
      break;
    case types::e2_removal_resp:
      c = e2_removal_resp_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
  }
}
ric_sub_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_resp()
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
ric_sub_delete_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_delete_resp()
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
ric_sub_mod_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_resp()
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
ric_sub_mod_confirm_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_confirm()
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
ric_query_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_query_resp()
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
ric_service_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_upd_ack()
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
ric_ctrl_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_ctrl_ack()
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
e2setup_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2setup_resp()
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
e2node_cfg_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack()
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
e2conn_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack()
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
reset_resp_s& e2ap_elem_procs_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
e2_removal_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_removal_resp()
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
const ric_sub_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_resp() const
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
const ric_sub_delete_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_delete_resp() const
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
const ric_sub_mod_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_resp() const
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
const ric_sub_mod_confirm_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_confirm() const
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
const ric_query_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_query_resp() const
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
const ric_service_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_upd_ack() const
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
const ric_ctrl_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_ctrl_ack() const
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
const e2setup_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2setup_resp() const
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
const e2node_cfg_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack() const
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
const e2conn_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack() const
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
const reset_resp_s& e2ap_elem_procs_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const e2_removal_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_removal_resp() const
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
void e2ap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_resp:
      j.write_fieldname("RICsubscriptionResponse");
      c.get<ric_sub_resp_s>().to_json(j);
      break;
    case types::ric_sub_delete_resp:
      j.write_fieldname("RICsubscriptionDeleteResponse");
      c.get<ric_sub_delete_resp_s>().to_json(j);
      break;
    case types::ric_sub_mod_resp:
      j.write_fieldname("RICsubscriptionModificationResponse");
      c.get<ric_sub_mod_resp_s>().to_json(j);
      break;
    case types::ric_sub_mod_confirm:
      j.write_fieldname("RICsubscriptionModificationConfirm");
      c.get<ric_sub_mod_confirm_s>().to_json(j);
      break;
    case types::ric_query_resp:
      j.write_fieldname("RICQueryResponse");
      c.get<ric_query_resp_s>().to_json(j);
      break;
    case types::ric_service_upd_ack:
      j.write_fieldname("RICserviceUpdateAcknowledge");
      c.get<ric_service_upd_ack_s>().to_json(j);
      break;
    case types::ric_ctrl_ack:
      j.write_fieldname("RICcontrolAcknowledge");
      c.get<ric_ctrl_ack_s>().to_json(j);
      break;
    case types::e2setup_resp:
      j.write_fieldname("E2setupResponse");
      c.get<e2setup_resp_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_ack:
      j.write_fieldname("E2nodeConfigurationUpdateAcknowledge");
      c.get<e2node_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e2conn_upd_ack:
      j.write_fieldname("E2connectionUpdateAcknowledge");
      c.get<e2conn_upd_ack_s>().to_json(j);
      break;
    case types::reset_resp:
      j.write_fieldname("ResetResponse");
      c.get<reset_resp_s>().to_json(j);
      break;
    case types::e2_removal_resp:
      j.write_fieldname("E2RemovalResponse");
      c.get<e2_removal_resp_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_resp:
      HANDLE_CODE(c.get<ric_sub_resp_s>().pack(bref));
      break;
    case types::ric_sub_delete_resp:
      HANDLE_CODE(c.get<ric_sub_delete_resp_s>().pack(bref));
      break;
    case types::ric_sub_mod_resp:
      HANDLE_CODE(c.get<ric_sub_mod_resp_s>().pack(bref));
      break;
    case types::ric_sub_mod_confirm:
      HANDLE_CODE(c.get<ric_sub_mod_confirm_s>().pack(bref));
      break;
    case types::ric_query_resp:
      HANDLE_CODE(c.get<ric_query_resp_s>().pack(bref));
      break;
    case types::ric_service_upd_ack:
      HANDLE_CODE(c.get<ric_service_upd_ack_s>().pack(bref));
      break;
    case types::ric_ctrl_ack:
      HANDLE_CODE(c.get<ric_ctrl_ack_s>().pack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().pack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().pack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_resp:
      HANDLE_CODE(c.get<ric_sub_resp_s>().unpack(bref));
      break;
    case types::ric_sub_delete_resp:
      HANDLE_CODE(c.get<ric_sub_delete_resp_s>().unpack(bref));
      break;
    case types::ric_sub_mod_resp:
      HANDLE_CODE(c.get<ric_sub_mod_resp_s>().unpack(bref));
      break;
    case types::ric_sub_mod_confirm:
      HANDLE_CODE(c.get<ric_sub_mod_confirm_s>().unpack(bref));
      break;
    case types::ric_query_resp:
      HANDLE_CODE(c.get<ric_query_resp_s>().unpack(bref));
      break;
    case types::ric_service_upd_ack:
      HANDLE_CODE(c.get<ric_service_upd_ack_s>().unpack(bref));
      break;
    case types::ric_ctrl_ack:
      HANDLE_CODE(c.get<ric_ctrl_ack_s>().unpack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().unpack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().unpack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionResponse",
                                "RICsubscriptionDeleteResponse",
                                "RICsubscriptionModificationResponse",
                                "RICsubscriptionModificationConfirm",
                                "RICQueryResponse",
                                "RICserviceUpdateAcknowledge",
                                "RICcontrolAcknowledge",
                                "E2setupResponse",
                                "E2nodeConfigurationUpdateAcknowledge",
                                "E2connectionUpdateAcknowledge",
                                "ResetResponse",
                                "E2RemovalResponse"};
  return convert_enum_idx(names, 12, value, "e2ap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_fail:
      c = ric_sub_fail_s{};
      break;
    case types::ric_sub_delete_fail:
      c = ric_sub_delete_fail_s{};
      break;
    case types::ric_sub_mod_fail:
      c = ric_sub_mod_fail_s{};
      break;
    case types::ric_sub_mod_refuse:
      c = ric_sub_mod_refuse_s{};
      break;
    case types::ric_query_fail:
      c = ric_query_fail_s{};
      break;
    case types::ric_service_upd_fail:
      c = ric_service_upd_fail_s{};
      break;
    case types::ric_ctrl_fail:
      c = ric_ctrl_fail_s{};
      break;
    case types::e2setup_fail:
      c = e2setup_fail_s{};
      break;
    case types::e2node_cfg_upd_fail:
      c = e2node_cfg_upd_fail_s{};
      break;
    case types::e2conn_upd_fail:
      c = e2conn_upd_fail_s{};
      break;
    case types::e2_removal_fail:
      c = e2_removal_fail_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
ric_sub_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_fail()
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
ric_sub_delete_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_delete_fail()
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
ric_sub_mod_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_fail()
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
ric_sub_mod_refuse_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_refuse()
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
ric_query_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_query_fail()
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
ric_service_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_upd_fail()
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
ric_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_ctrl_fail()
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
e2setup_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail()
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
e2node_cfg_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail()
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
e2conn_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail()
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
e2_removal_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail()
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
const ric_sub_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_fail() const
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
const ric_sub_delete_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_delete_fail() const
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
const ric_sub_mod_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_fail() const
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
const ric_sub_mod_refuse_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_refuse() const
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
const ric_query_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_query_fail() const
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
const ric_service_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_upd_fail() const
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
const ric_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_ctrl_fail() const
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
const e2setup_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail() const
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
const e2node_cfg_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail() const
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
const e2conn_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail() const
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
const e2_removal_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail() const
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
void e2ap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_fail:
      j.write_fieldname("RICsubscriptionFailure");
      c.get<ric_sub_fail_s>().to_json(j);
      break;
    case types::ric_sub_delete_fail:
      j.write_fieldname("RICsubscriptionDeleteFailure");
      c.get<ric_sub_delete_fail_s>().to_json(j);
      break;
    case types::ric_sub_mod_fail:
      j.write_fieldname("RICsubscriptionModificationFailure");
      c.get<ric_sub_mod_fail_s>().to_json(j);
      break;
    case types::ric_sub_mod_refuse:
      j.write_fieldname("RICsubscriptionModificationRefuse");
      c.get<ric_sub_mod_refuse_s>().to_json(j);
      break;
    case types::ric_query_fail:
      j.write_fieldname("RICQueryFailure");
      c.get<ric_query_fail_s>().to_json(j);
      break;
    case types::ric_service_upd_fail:
      j.write_fieldname("RICserviceUpdateFailure");
      c.get<ric_service_upd_fail_s>().to_json(j);
      break;
    case types::ric_ctrl_fail:
      j.write_fieldname("RICcontrolFailure");
      c.get<ric_ctrl_fail_s>().to_json(j);
      break;
    case types::e2setup_fail:
      j.write_fieldname("E2setupFailure");
      c.get<e2setup_fail_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_fail:
      j.write_fieldname("E2nodeConfigurationUpdateFailure");
      c.get<e2node_cfg_upd_fail_s>().to_json(j);
      break;
    case types::e2conn_upd_fail:
      j.write_fieldname("E2connectionUpdateFailure");
      c.get<e2conn_upd_fail_s>().to_json(j);
      break;
    case types::e2_removal_fail:
      j.write_fieldname("E2RemovalFailure");
      c.get<e2_removal_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_fail:
      HANDLE_CODE(c.get<ric_sub_fail_s>().pack(bref));
      break;
    case types::ric_sub_delete_fail:
      HANDLE_CODE(c.get<ric_sub_delete_fail_s>().pack(bref));
      break;
    case types::ric_sub_mod_fail:
      HANDLE_CODE(c.get<ric_sub_mod_fail_s>().pack(bref));
      break;
    case types::ric_sub_mod_refuse:
      HANDLE_CODE(c.get<ric_sub_mod_refuse_s>().pack(bref));
      break;
    case types::ric_query_fail:
      HANDLE_CODE(c.get<ric_query_fail_s>().pack(bref));
      break;
    case types::ric_service_upd_fail:
      HANDLE_CODE(c.get<ric_service_upd_fail_s>().pack(bref));
      break;
    case types::ric_ctrl_fail:
      HANDLE_CODE(c.get<ric_ctrl_fail_s>().pack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().pack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().pack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_fail:
      HANDLE_CODE(c.get<ric_sub_fail_s>().unpack(bref));
      break;
    case types::ric_sub_delete_fail:
      HANDLE_CODE(c.get<ric_sub_delete_fail_s>().unpack(bref));
      break;
    case types::ric_sub_mod_fail:
      HANDLE_CODE(c.get<ric_sub_mod_fail_s>().unpack(bref));
      break;
    case types::ric_sub_mod_refuse:
      HANDLE_CODE(c.get<ric_sub_mod_refuse_s>().unpack(bref));
      break;
    case types::ric_query_fail:
      HANDLE_CODE(c.get<ric_query_fail_s>().unpack(bref));
      break;
    case types::ric_service_upd_fail:
      HANDLE_CODE(c.get<ric_service_upd_fail_s>().unpack(bref));
      break;
    case types::ric_ctrl_fail:
      HANDLE_CODE(c.get<ric_ctrl_fail_s>().unpack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().unpack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionFailure",
                                "RICsubscriptionDeleteFailure",
                                "RICsubscriptionModificationFailure",
                                "RICsubscriptionModificationRefuse",
                                "RICQueryFailure",
                                "RICserviceUpdateFailure",
                                "RICcontrolFailure",
                                "E2setupFailure",
                                "E2nodeConfigurationUpdateFailure",
                                "E2connectionUpdateFailure",
                                "E2RemovalFailure"};
  return convert_enum_idx(names, 11, value, "e2ap_elem_procs_o::unsuccessful_outcome_c::types");
}

// ProtocolIE-FieldPair{E2AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{E2AP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
SRSASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, id, (uint32_t)0u, (uint32_t)65535u, false, true));
  warn_assert(first_crit != ies_set_paramT_::get_first_crit(id), __func__, __LINE__);
  HANDLE_CODE(first_crit.pack(bref));
  HANDLE_CODE(first_value.pack(bref));
  warn_assert(second_crit != ies_set_paramT_::get_second_crit(id), __func__, __LINE__);
  HANDLE_CODE(second_crit.pack(bref));
  HANDLE_CODE(second_value.pack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
SRSASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(first_crit.unpack(bref));
  first_value = ies_set_paramT_::get_first_value(id);
  HANDLE_CODE(first_value.unpack(bref));
  HANDLE_CODE(second_crit.unpack(bref));
  second_value = ies_set_paramT_::get_second_value(id);
  HANDLE_CODE(second_value.unpack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
void protocol_ie_field_pair_s<ies_set_paramT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", id);
  j.write_str("firstCriticality", first_crit.to_string());
  j.write_str("secondCriticality", second_crit.to_string());
  j.end_obj();
}
template <class ies_set_paramT_>
bool protocol_ie_field_pair_s<ies_set_paramT_>::load_info_obj(const uint32_t& id_)
{
  if (not ies_set_paramT_::is_id_valid(id_)) {
    return false;
  }
  id           = id_;
  first_crit   = ies_set_paramT_::get_first_crit(id);
  first_value  = ies_set_paramT_::get_first_value(id);
  second_crit  = ies_set_paramT_::get_second_crit(id);
  second_value = ies_set_paramT_::get_second_value(id);
  return first_value.type().value != ies_set_paramT_::first_value_c::types_opts::nulltype and
         second_value.type().value != ies_set_paramT_::second_value_c::types_opts::nulltype;
}

// InitiatingMessage ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2ap_elem_procs_o::get_init_msg(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void init_msg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2ap_elem_procs_o::get_crit(proc_code);
  value     = e2ap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != e2ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2ap_elem_procs_o::get_successful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void successful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2ap_elem_procs_o::get_crit(proc_code);
  value     = e2ap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != e2ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void unsuccessful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2ap_elem_procs_o::get_crit(proc_code);
  value     = e2ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// E2AP-ELEMENTARY-PROCEDURES-CLASS-1 ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
uint16_t e2ap_elem_procs_class_1_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {8, 9, 14, 15, 16, 7, 4, 1, 10, 11, 3, 13};
  return map_enum_number(names, 12, idx, "proc_code");
}
bool e2ap_elem_procs_class_1_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {8, 9, 14, 15, 16, 7, 4, 1, 10, 11, 3, 13};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
e2ap_elem_procs_class_1_o::init_msg_c e2ap_elem_procs_class_1_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(init_msg_c::types::ric_sub_request);
      break;
    case 9:
      ret.set(init_msg_c::types::ric_sub_delete_request);
      break;
    case 14:
      ret.set(init_msg_c::types::ric_sub_mod_request);
      break;
    case 15:
      ret.set(init_msg_c::types::ric_sub_mod_required);
      break;
    case 16:
      ret.set(init_msg_c::types::ric_query_request);
      break;
    case 7:
      ret.set(init_msg_c::types::ric_service_upd);
      break;
    case 4:
      ret.set(init_msg_c::types::ric_ctrl_request);
      break;
    case 1:
      ret.set(init_msg_c::types::e2setup_request);
      break;
    case 10:
      ret.set(init_msg_c::types::e2node_cfg_upd);
      break;
    case 11:
      ret.set(init_msg_c::types::e2conn_upd);
      break;
    case 3:
      ret.set(init_msg_c::types::reset_request);
      break;
    case 13:
      ret.set(init_msg_c::types::e2_removal_request);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_class_1_o::successful_outcome_c
e2ap_elem_procs_class_1_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(successful_outcome_c::types::ric_sub_resp);
      break;
    case 9:
      ret.set(successful_outcome_c::types::ric_sub_delete_resp);
      break;
    case 14:
      ret.set(successful_outcome_c::types::ric_sub_mod_resp);
      break;
    case 15:
      ret.set(successful_outcome_c::types::ric_sub_mod_confirm);
      break;
    case 16:
      ret.set(successful_outcome_c::types::ric_query_resp);
      break;
    case 7:
      ret.set(successful_outcome_c::types::ric_service_upd_ack);
      break;
    case 4:
      ret.set(successful_outcome_c::types::ric_ctrl_ack);
      break;
    case 1:
      ret.set(successful_outcome_c::types::e2setup_resp);
      break;
    case 10:
      ret.set(successful_outcome_c::types::e2node_cfg_upd_ack);
      break;
    case 11:
      ret.set(successful_outcome_c::types::e2conn_upd_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::reset_resp);
      break;
    case 13:
      ret.set(successful_outcome_c::types::e2_removal_resp);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_class_1_o::unsuccessful_outcome_c
e2ap_elem_procs_class_1_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(unsuccessful_outcome_c::types::ric_sub_fail);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::ric_sub_delete_fail);
      break;
    case 14:
      ret.set(unsuccessful_outcome_c::types::ric_sub_mod_fail);
      break;
    case 15:
      ret.set(unsuccessful_outcome_c::types::ric_sub_mod_refuse);
      break;
    case 16:
      ret.set(unsuccessful_outcome_c::types::ric_query_fail);
      break;
    case 7:
      ret.set(unsuccessful_outcome_c::types::ric_service_upd_fail);
      break;
    case 4:
      ret.set(unsuccessful_outcome_c::types::ric_ctrl_fail);
      break;
    case 1:
      ret.set(unsuccessful_outcome_c::types::e2setup_fail);
      break;
    case 10:
      ret.set(unsuccessful_outcome_c::types::e2node_cfg_upd_fail);
      break;
    case 11:
      ret.set(unsuccessful_outcome_c::types::e2conn_upd_fail);
      break;
    case 13:
      ret.set(unsuccessful_outcome_c::types::e2_removal_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e e2ap_elem_procs_class_1_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 8:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 16:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void e2ap_elem_procs_class_1_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_request:
      c = ric_sub_request_s{};
      break;
    case types::ric_sub_delete_request:
      c = ric_sub_delete_request_s{};
      break;
    case types::ric_sub_mod_request:
      c = ric_sub_mod_request_s{};
      break;
    case types::ric_sub_mod_required:
      c = ric_sub_mod_required_s{};
      break;
    case types::ric_query_request:
      c = ric_query_request_s{};
      break;
    case types::ric_service_upd:
      c = ric_service_upd_s{};
      break;
    case types::ric_ctrl_request:
      c = ric_ctrl_request_s{};
      break;
    case types::e2setup_request:
      c = e2setup_request_s{};
      break;
    case types::e2node_cfg_upd:
      c = e2node_cfg_upd_s{};
      break;
    case types::e2conn_upd:
      c = e2conn_upd_s{};
      break;
    case types::reset_request:
      c = reset_request_s{};
      break;
    case types::e2_removal_request:
      c = e2_removal_request_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::init_msg_c");
  }
}
ric_sub_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_request()
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
ric_sub_delete_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_delete_request()
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
ric_sub_mod_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_mod_request()
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
ric_sub_mod_required_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_mod_required()
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
ric_query_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_query_request()
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
ric_service_upd_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_service_upd()
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
ric_ctrl_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_ctrl_request()
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
e2setup_request_s& e2ap_elem_procs_class_1_o::init_msg_c::e2setup_request()
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
e2node_cfg_upd_s& e2ap_elem_procs_class_1_o::init_msg_c::e2node_cfg_upd()
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
e2conn_upd_s& e2ap_elem_procs_class_1_o::init_msg_c::e2conn_upd()
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
reset_request_s& e2ap_elem_procs_class_1_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
e2_removal_request_s& e2ap_elem_procs_class_1_o::init_msg_c::e2_removal_request()
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
const ric_sub_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_request() const
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
const ric_sub_delete_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_delete_request() const
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
const ric_sub_mod_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_mod_request() const
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
const ric_sub_mod_required_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_sub_mod_required() const
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
const ric_query_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_query_request() const
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
const ric_service_upd_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_service_upd() const
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
const ric_ctrl_request_s& e2ap_elem_procs_class_1_o::init_msg_c::ric_ctrl_request() const
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
const e2setup_request_s& e2ap_elem_procs_class_1_o::init_msg_c::e2setup_request() const
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
const e2node_cfg_upd_s& e2ap_elem_procs_class_1_o::init_msg_c::e2node_cfg_upd() const
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
const e2conn_upd_s& e2ap_elem_procs_class_1_o::init_msg_c::e2conn_upd() const
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
const reset_request_s& e2ap_elem_procs_class_1_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const e2_removal_request_s& e2ap_elem_procs_class_1_o::init_msg_c::e2_removal_request() const
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
void e2ap_elem_procs_class_1_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_request:
      j.write_fieldname("RICsubscriptionRequest");
      c.get<ric_sub_request_s>().to_json(j);
      break;
    case types::ric_sub_delete_request:
      j.write_fieldname("RICsubscriptionDeleteRequest");
      c.get<ric_sub_delete_request_s>().to_json(j);
      break;
    case types::ric_sub_mod_request:
      j.write_fieldname("RICsubscriptionModificationRequest");
      c.get<ric_sub_mod_request_s>().to_json(j);
      break;
    case types::ric_sub_mod_required:
      j.write_fieldname("RICsubscriptionModificationRequired");
      c.get<ric_sub_mod_required_s>().to_json(j);
      break;
    case types::ric_query_request:
      j.write_fieldname("RICQueryRequest");
      c.get<ric_query_request_s>().to_json(j);
      break;
    case types::ric_service_upd:
      j.write_fieldname("RICserviceUpdate");
      c.get<ric_service_upd_s>().to_json(j);
      break;
    case types::ric_ctrl_request:
      j.write_fieldname("RICcontrolRequest");
      c.get<ric_ctrl_request_s>().to_json(j);
      break;
    case types::e2setup_request:
      j.write_fieldname("E2setupRequest");
      c.get<e2setup_request_s>().to_json(j);
      break;
    case types::e2node_cfg_upd:
      j.write_fieldname("E2nodeConfigurationUpdate");
      c.get<e2node_cfg_upd_s>().to_json(j);
      break;
    case types::e2conn_upd:
      j.write_fieldname("E2connectionUpdate");
      c.get<e2conn_upd_s>().to_json(j);
      break;
    case types::reset_request:
      j.write_fieldname("ResetRequest");
      c.get<reset_request_s>().to_json(j);
      break;
    case types::e2_removal_request:
      j.write_fieldname("E2RemovalRequest");
      c.get<e2_removal_request_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_class_1_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_request:
      HANDLE_CODE(c.get<ric_sub_request_s>().pack(bref));
      break;
    case types::ric_sub_delete_request:
      HANDLE_CODE(c.get<ric_sub_delete_request_s>().pack(bref));
      break;
    case types::ric_sub_mod_request:
      HANDLE_CODE(c.get<ric_sub_mod_request_s>().pack(bref));
      break;
    case types::ric_sub_mod_required:
      HANDLE_CODE(c.get<ric_sub_mod_required_s>().pack(bref));
      break;
    case types::ric_query_request:
      HANDLE_CODE(c.get<ric_query_request_s>().pack(bref));
      break;
    case types::ric_service_upd:
      HANDLE_CODE(c.get<ric_service_upd_s>().pack(bref));
      break;
    case types::ric_ctrl_request:
      HANDLE_CODE(c.get<ric_ctrl_request_s>().pack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().pack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().pack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().pack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().pack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_class_1_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_request:
      HANDLE_CODE(c.get<ric_sub_request_s>().unpack(bref));
      break;
    case types::ric_sub_delete_request:
      HANDLE_CODE(c.get<ric_sub_delete_request_s>().unpack(bref));
      break;
    case types::ric_sub_mod_request:
      HANDLE_CODE(c.get<ric_sub_mod_request_s>().unpack(bref));
      break;
    case types::ric_sub_mod_required:
      HANDLE_CODE(c.get<ric_sub_mod_required_s>().unpack(bref));
      break;
    case types::ric_query_request:
      HANDLE_CODE(c.get<ric_query_request_s>().unpack(bref));
      break;
    case types::ric_service_upd:
      HANDLE_CODE(c.get<ric_service_upd_s>().unpack(bref));
      break;
    case types::ric_ctrl_request:
      HANDLE_CODE(c.get<ric_ctrl_request_s>().unpack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().unpack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().unpack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().unpack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_class_1_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionRequest",
                                "RICsubscriptionDeleteRequest",
                                "RICsubscriptionModificationRequest",
                                "RICsubscriptionModificationRequired",
                                "RICQueryRequest",
                                "RICserviceUpdate",
                                "RICcontrolRequest",
                                "E2setupRequest",
                                "E2nodeConfigurationUpdate",
                                "E2connectionUpdate",
                                "ResetRequest",
                                "E2RemovalRequest"};
  return convert_enum_idx(names, 12, value, "e2ap_elem_procs_class_1_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_class_1_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_resp:
      c = ric_sub_resp_s{};
      break;
    case types::ric_sub_delete_resp:
      c = ric_sub_delete_resp_s{};
      break;
    case types::ric_sub_mod_resp:
      c = ric_sub_mod_resp_s{};
      break;
    case types::ric_sub_mod_confirm:
      c = ric_sub_mod_confirm_s{};
      break;
    case types::ric_query_resp:
      c = ric_query_resp_s{};
      break;
    case types::ric_service_upd_ack:
      c = ric_service_upd_ack_s{};
      break;
    case types::ric_ctrl_ack:
      c = ric_ctrl_ack_s{};
      break;
    case types::e2setup_resp:
      c = e2setup_resp_s{};
      break;
    case types::e2node_cfg_upd_ack:
      c = e2node_cfg_upd_ack_s{};
      break;
    case types::e2conn_upd_ack:
      c = e2conn_upd_ack_s{};
      break;
    case types::reset_resp:
      c = reset_resp_s{};
      break;
    case types::e2_removal_resp:
      c = e2_removal_resp_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::successful_outcome_c");
  }
}
ric_sub_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_resp()
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
ric_sub_delete_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_delete_resp()
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
ric_sub_mod_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_mod_resp()
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
ric_sub_mod_confirm_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_mod_confirm()
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
ric_query_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_query_resp()
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
ric_service_upd_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_service_upd_ack()
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
ric_ctrl_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_ctrl_ack()
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
e2setup_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2setup_resp()
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
e2node_cfg_upd_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2node_cfg_upd_ack()
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
e2conn_upd_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2conn_upd_ack()
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
reset_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
e2_removal_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2_removal_resp()
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
const ric_sub_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_resp() const
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
const ric_sub_delete_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_delete_resp() const
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
const ric_sub_mod_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_mod_resp() const
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
const ric_sub_mod_confirm_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_sub_mod_confirm() const
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
const ric_query_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_query_resp() const
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
const ric_service_upd_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_service_upd_ack() const
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
const ric_ctrl_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::ric_ctrl_ack() const
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
const e2setup_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2setup_resp() const
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
const e2node_cfg_upd_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2node_cfg_upd_ack() const
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
const e2conn_upd_ack_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2conn_upd_ack() const
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
const reset_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const e2_removal_resp_s& e2ap_elem_procs_class_1_o::successful_outcome_c::e2_removal_resp() const
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
void e2ap_elem_procs_class_1_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_resp:
      j.write_fieldname("RICsubscriptionResponse");
      c.get<ric_sub_resp_s>().to_json(j);
      break;
    case types::ric_sub_delete_resp:
      j.write_fieldname("RICsubscriptionDeleteResponse");
      c.get<ric_sub_delete_resp_s>().to_json(j);
      break;
    case types::ric_sub_mod_resp:
      j.write_fieldname("RICsubscriptionModificationResponse");
      c.get<ric_sub_mod_resp_s>().to_json(j);
      break;
    case types::ric_sub_mod_confirm:
      j.write_fieldname("RICsubscriptionModificationConfirm");
      c.get<ric_sub_mod_confirm_s>().to_json(j);
      break;
    case types::ric_query_resp:
      j.write_fieldname("RICQueryResponse");
      c.get<ric_query_resp_s>().to_json(j);
      break;
    case types::ric_service_upd_ack:
      j.write_fieldname("RICserviceUpdateAcknowledge");
      c.get<ric_service_upd_ack_s>().to_json(j);
      break;
    case types::ric_ctrl_ack:
      j.write_fieldname("RICcontrolAcknowledge");
      c.get<ric_ctrl_ack_s>().to_json(j);
      break;
    case types::e2setup_resp:
      j.write_fieldname("E2setupResponse");
      c.get<e2setup_resp_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_ack:
      j.write_fieldname("E2nodeConfigurationUpdateAcknowledge");
      c.get<e2node_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e2conn_upd_ack:
      j.write_fieldname("E2connectionUpdateAcknowledge");
      c.get<e2conn_upd_ack_s>().to_json(j);
      break;
    case types::reset_resp:
      j.write_fieldname("ResetResponse");
      c.get<reset_resp_s>().to_json(j);
      break;
    case types::e2_removal_resp:
      j.write_fieldname("E2RemovalResponse");
      c.get<e2_removal_resp_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_class_1_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_resp:
      HANDLE_CODE(c.get<ric_sub_resp_s>().pack(bref));
      break;
    case types::ric_sub_delete_resp:
      HANDLE_CODE(c.get<ric_sub_delete_resp_s>().pack(bref));
      break;
    case types::ric_sub_mod_resp:
      HANDLE_CODE(c.get<ric_sub_mod_resp_s>().pack(bref));
      break;
    case types::ric_sub_mod_confirm:
      HANDLE_CODE(c.get<ric_sub_mod_confirm_s>().pack(bref));
      break;
    case types::ric_query_resp:
      HANDLE_CODE(c.get<ric_query_resp_s>().pack(bref));
      break;
    case types::ric_service_upd_ack:
      HANDLE_CODE(c.get<ric_service_upd_ack_s>().pack(bref));
      break;
    case types::ric_ctrl_ack:
      HANDLE_CODE(c.get<ric_ctrl_ack_s>().pack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().pack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().pack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_class_1_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_resp:
      HANDLE_CODE(c.get<ric_sub_resp_s>().unpack(bref));
      break;
    case types::ric_sub_delete_resp:
      HANDLE_CODE(c.get<ric_sub_delete_resp_s>().unpack(bref));
      break;
    case types::ric_sub_mod_resp:
      HANDLE_CODE(c.get<ric_sub_mod_resp_s>().unpack(bref));
      break;
    case types::ric_sub_mod_confirm:
      HANDLE_CODE(c.get<ric_sub_mod_confirm_s>().unpack(bref));
      break;
    case types::ric_query_resp:
      HANDLE_CODE(c.get<ric_query_resp_s>().unpack(bref));
      break;
    case types::ric_service_upd_ack:
      HANDLE_CODE(c.get<ric_service_upd_ack_s>().unpack(bref));
      break;
    case types::ric_ctrl_ack:
      HANDLE_CODE(c.get<ric_ctrl_ack_s>().unpack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().unpack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().unpack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_class_1_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionResponse",
                                "RICsubscriptionDeleteResponse",
                                "RICsubscriptionModificationResponse",
                                "RICsubscriptionModificationConfirm",
                                "RICQueryResponse",
                                "RICserviceUpdateAcknowledge",
                                "RICcontrolAcknowledge",
                                "E2setupResponse",
                                "E2nodeConfigurationUpdateAcknowledge",
                                "E2connectionUpdateAcknowledge",
                                "ResetResponse",
                                "E2RemovalResponse"};
  return convert_enum_idx(names, 12, value, "e2ap_elem_procs_class_1_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_sub_fail:
      c = ric_sub_fail_s{};
      break;
    case types::ric_sub_delete_fail:
      c = ric_sub_delete_fail_s{};
      break;
    case types::ric_sub_mod_fail:
      c = ric_sub_mod_fail_s{};
      break;
    case types::ric_sub_mod_refuse:
      c = ric_sub_mod_refuse_s{};
      break;
    case types::ric_query_fail:
      c = ric_query_fail_s{};
      break;
    case types::ric_service_upd_fail:
      c = ric_service_upd_fail_s{};
      break;
    case types::ric_ctrl_fail:
      c = ric_ctrl_fail_s{};
      break;
    case types::e2setup_fail:
      c = e2setup_fail_s{};
      break;
    case types::e2node_cfg_upd_fail:
      c = e2node_cfg_upd_fail_s{};
      break;
    case types::e2conn_upd_fail:
      c = e2conn_upd_fail_s{};
      break;
    case types::e2_removal_fail:
      c = e2_removal_fail_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::unsuccessful_outcome_c");
  }
}
ric_sub_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_fail()
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
ric_sub_delete_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_delete_fail()
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
ric_sub_mod_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_mod_fail()
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
ric_sub_mod_refuse_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_mod_refuse()
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
ric_query_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_query_fail()
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
ric_service_upd_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_service_upd_fail()
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
ric_ctrl_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_ctrl_fail()
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
e2setup_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2setup_fail()
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
e2node_cfg_upd_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2node_cfg_upd_fail()
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
e2conn_upd_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2conn_upd_fail()
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
e2_removal_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2_removal_fail()
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
const ric_sub_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_fail() const
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
const ric_sub_delete_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_delete_fail() const
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
const ric_sub_mod_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_mod_fail() const
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
const ric_sub_mod_refuse_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_sub_mod_refuse() const
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
const ric_query_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_query_fail() const
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
const ric_service_upd_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_service_upd_fail() const
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
const ric_ctrl_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::ric_ctrl_fail() const
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
const e2setup_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2setup_fail() const
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
const e2node_cfg_upd_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2node_cfg_upd_fail() const
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
const e2conn_upd_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2conn_upd_fail() const
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
const e2_removal_fail_s& e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::e2_removal_fail() const
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
void e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_sub_fail:
      j.write_fieldname("RICsubscriptionFailure");
      c.get<ric_sub_fail_s>().to_json(j);
      break;
    case types::ric_sub_delete_fail:
      j.write_fieldname("RICsubscriptionDeleteFailure");
      c.get<ric_sub_delete_fail_s>().to_json(j);
      break;
    case types::ric_sub_mod_fail:
      j.write_fieldname("RICsubscriptionModificationFailure");
      c.get<ric_sub_mod_fail_s>().to_json(j);
      break;
    case types::ric_sub_mod_refuse:
      j.write_fieldname("RICsubscriptionModificationRefuse");
      c.get<ric_sub_mod_refuse_s>().to_json(j);
      break;
    case types::ric_query_fail:
      j.write_fieldname("RICQueryFailure");
      c.get<ric_query_fail_s>().to_json(j);
      break;
    case types::ric_service_upd_fail:
      j.write_fieldname("RICserviceUpdateFailure");
      c.get<ric_service_upd_fail_s>().to_json(j);
      break;
    case types::ric_ctrl_fail:
      j.write_fieldname("RICcontrolFailure");
      c.get<ric_ctrl_fail_s>().to_json(j);
      break;
    case types::e2setup_fail:
      j.write_fieldname("E2setupFailure");
      c.get<e2setup_fail_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_fail:
      j.write_fieldname("E2nodeConfigurationUpdateFailure");
      c.get<e2node_cfg_upd_fail_s>().to_json(j);
      break;
    case types::e2conn_upd_fail:
      j.write_fieldname("E2connectionUpdateFailure");
      c.get<e2conn_upd_fail_s>().to_json(j);
      break;
    case types::e2_removal_fail:
      j.write_fieldname("E2RemovalFailure");
      c.get<e2_removal_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_fail:
      HANDLE_CODE(c.get<ric_sub_fail_s>().pack(bref));
      break;
    case types::ric_sub_delete_fail:
      HANDLE_CODE(c.get<ric_sub_delete_fail_s>().pack(bref));
      break;
    case types::ric_sub_mod_fail:
      HANDLE_CODE(c.get<ric_sub_mod_fail_s>().pack(bref));
      break;
    case types::ric_sub_mod_refuse:
      HANDLE_CODE(c.get<ric_sub_mod_refuse_s>().pack(bref));
      break;
    case types::ric_query_fail:
      HANDLE_CODE(c.get<ric_query_fail_s>().pack(bref));
      break;
    case types::ric_service_upd_fail:
      HANDLE_CODE(c.get<ric_service_upd_fail_s>().pack(bref));
      break;
    case types::ric_ctrl_fail:
      HANDLE_CODE(c.get<ric_ctrl_fail_s>().pack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().pack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().pack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_sub_fail:
      HANDLE_CODE(c.get<ric_sub_fail_s>().unpack(bref));
      break;
    case types::ric_sub_delete_fail:
      HANDLE_CODE(c.get<ric_sub_delete_fail_s>().unpack(bref));
      break;
    case types::ric_sub_mod_fail:
      HANDLE_CODE(c.get<ric_sub_mod_fail_s>().unpack(bref));
      break;
    case types::ric_sub_mod_refuse:
      HANDLE_CODE(c.get<ric_sub_mod_refuse_s>().unpack(bref));
      break;
    case types::ric_query_fail:
      HANDLE_CODE(c.get<ric_query_fail_s>().unpack(bref));
      break;
    case types::ric_service_upd_fail:
      HANDLE_CODE(c.get<ric_service_upd_fail_s>().unpack(bref));
      break;
    case types::ric_ctrl_fail:
      HANDLE_CODE(c.get<ric_ctrl_fail_s>().unpack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().unpack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_1_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"RICsubscriptionFailure",
                                "RICsubscriptionDeleteFailure",
                                "RICsubscriptionModificationFailure",
                                "RICsubscriptionModificationRefuse",
                                "RICQueryFailure",
                                "RICserviceUpdateFailure",
                                "RICcontrolFailure",
                                "E2setupFailure",
                                "E2nodeConfigurationUpdateFailure",
                                "E2connectionUpdateFailure",
                                "E2RemovalFailure"};
  return convert_enum_idx(names, 11, value, "e2ap_elem_procs_class_1_o::unsuccessful_outcome_c::types");
}

// E2AP-ELEMENTARY-PROCEDURES-CLASS-2 ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
uint16_t e2ap_elem_procs_class_2_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {5, 6, 2, 12};
  return map_enum_number(names, 4, idx, "proc_code");
}
bool e2ap_elem_procs_class_2_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {5, 6, 2, 12};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
e2ap_elem_procs_class_2_o::init_msg_c e2ap_elem_procs_class_2_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 5:
      ret.set(init_msg_c::types::ric_ind);
      break;
    case 6:
      ret.set(init_msg_c::types::ric_service_query);
      break;
    case 2:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 12:
      ret.set(init_msg_c::types::ric_sub_delete_required);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_class_2_o::successful_outcome_c
e2ap_elem_procs_class_2_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e2ap_elem_procs_class_2_o::unsuccessful_outcome_c
e2ap_elem_procs_class_2_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e e2ap_elem_procs_class_2_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 5:
      return crit_e::ignore;
    case 6:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void e2ap_elem_procs_class_2_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::ric_ind:
      c = ric_ind_s{};
      break;
    case types::ric_service_query:
      c = ric_service_query_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::ric_sub_delete_required:
      c = ric_sub_delete_required_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_2_o::init_msg_c");
  }
}
ric_ind_s& e2ap_elem_procs_class_2_o::init_msg_c::ric_ind()
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
ric_service_query_s& e2ap_elem_procs_class_2_o::init_msg_c::ric_service_query()
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
error_ind_s& e2ap_elem_procs_class_2_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ric_sub_delete_required_s& e2ap_elem_procs_class_2_o::init_msg_c::ric_sub_delete_required()
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
const ric_ind_s& e2ap_elem_procs_class_2_o::init_msg_c::ric_ind() const
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
const ric_service_query_s& e2ap_elem_procs_class_2_o::init_msg_c::ric_service_query() const
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
const error_ind_s& e2ap_elem_procs_class_2_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ric_sub_delete_required_s& e2ap_elem_procs_class_2_o::init_msg_c::ric_sub_delete_required() const
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
void e2ap_elem_procs_class_2_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_ind:
      j.write_fieldname("RICindication");
      c.get<ric_ind_s>().to_json(j);
      break;
    case types::ric_service_query:
      j.write_fieldname("RICserviceQuery");
      c.get<ric_service_query_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::ric_sub_delete_required:
      j.write_fieldname("RICsubscriptionDeleteRequired");
      c.get<ric_sub_delete_required_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_2_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_class_2_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_ind:
      HANDLE_CODE(c.get<ric_ind_s>().pack(bref));
      break;
    case types::ric_service_query:
      HANDLE_CODE(c.get<ric_service_query_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::ric_sub_delete_required:
      HANDLE_CODE(c.get<ric_sub_delete_required_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_2_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_class_2_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ric_ind:
      HANDLE_CODE(c.get<ric_ind_s>().unpack(bref));
      break;
    case types::ric_service_query:
      HANDLE_CODE(c.get<ric_service_query_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::ric_sub_delete_required:
      HANDLE_CODE(c.get<ric_sub_delete_required_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_elem_procs_class_2_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_class_2_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"RICindication", "RICserviceQuery", "ErrorIndication", "RICsubscriptionDeleteRequired"};
  return convert_enum_idx(names, 4, value, "e2ap_elem_procs_class_2_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_class_2_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_class_2_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_class_2_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_class_2_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {};
  return convert_enum_idx(names, 0, value, "e2ap_elem_procs_class_2_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void e2ap_elem_procs_class_2_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE e2ap_elem_procs_class_2_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_elem_procs_class_2_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}

const char* e2ap_elem_procs_class_2_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {};
  return convert_enum_idx(names, 0, value, "e2ap_elem_procs_class_2_o::unsuccessful_outcome_c::types");
}

// E2AP-PDU ::= CHOICE
void e2ap_pdu_c::destroy_()
{
  switch (type_) {
    case types::init_msg:
      c.destroy<init_msg_s>();
      break;
    case types::successful_outcome:
      c.destroy<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.destroy<unsuccessful_outcome_s>();
      break;
    default:
      break;
  }
}
void e2ap_pdu_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::init_msg:
      c.init<init_msg_s>();
      break;
    case types::successful_outcome:
      c.init<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.init<unsuccessful_outcome_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }
}
e2ap_pdu_c::e2ap_pdu_c(const e2ap_pdu_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::init_msg:
      c.init(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.init(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.init(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }
}
e2ap_pdu_c& e2ap_pdu_c::operator=(const e2ap_pdu_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::init_msg:
      c.set(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.set(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.set(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }

  return *this;
}
init_msg_s& e2ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& e2ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& e2ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void e2ap_pdu_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::init_msg:
      j.write_fieldname("initiatingMessage");
      c.get<init_msg_s>().to_json(j);
      break;
    case types::successful_outcome:
      j.write_fieldname("successfulOutcome");
      c.get<successful_outcome_s>().to_json(j);
      break;
    case types::unsuccessful_outcome:
      j.write_fieldname("unsuccessfulOutcome");
      c.get<unsuccessful_outcome_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
  }
  j.end_obj();
}
SRSASN_CODE e2ap_pdu_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().pack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().pack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2ap_pdu_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().unpack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().unpack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2ap_pdu_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2ap_pdu_c::types_opts::to_string() const
{
  static const char* names[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(names, 3, value, "e2ap_pdu_c::types");
}
