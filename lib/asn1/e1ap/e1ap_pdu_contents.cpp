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

#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// BearerContextInactivityNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_inactivity_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 24};
  return map_enum_number(names, 3, idx, "id");
}
bool bearer_context_inactivity_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 24};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_inactivity_notif_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 24:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_inactivity_notif_ies_o::value_c bearer_context_inactivity_notif_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 24:
      ret.set(value_c::types::activity_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_inactivity_notif_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 24:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_inactivity_notif_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::activity_info:
      c = activity_info_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_inactivity_notif_ies_o::value_c");
  }
}
uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
activity_info_c& bearer_context_inactivity_notif_ies_o::value_c::activity_info()
{
  assert_choice_type(types::activity_info, type_, "Value");
  return c.get<activity_info_c>();
}
const uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_inactivity_notif_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const activity_info_c& bearer_context_inactivity_notif_ies_o::value_c::activity_info() const
{
  assert_choice_type(types::activity_info, type_, "Value");
  return c.get<activity_info_c>();
}
void bearer_context_inactivity_notif_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::activity_info:
      j.write_fieldname("ActivityInformation");
      c.get<activity_info_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_inactivity_notif_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_inactivity_notif_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::activity_info:
      HANDLE_CODE(c.get<activity_info_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_inactivity_notif_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_inactivity_notif_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::activity_info:
      HANDLE_CODE(c.get<activity_info_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_inactivity_notif_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_inactivity_notif_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "ActivityInformation"};
  return convert_enum_idx(names, 3, value, "bearer_context_inactivity_notif_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_inactivity_notif_ies_o>;

SRSASN_CODE bearer_context_inactivity_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)24, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(activity_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_inactivity_notif_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 24: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(activity_info.unpack(bref));
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
void bearer_context_inactivity_notif_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 24);
  j.write_str("criticality", "reject");
  activity_info.to_json(j);
  j.end_obj();
}

// EUTRAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_confirm_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {41};
  return map_enum_number(names, 1, idx, "id");
}
bool eutran_bearer_context_mod_confirm_o::is_id_valid(const uint32_t& id)
{
  return 41 == id;
}
crit_e eutran_bearer_context_mod_confirm_o::get_crit(const uint32_t& id)
{
  if (id == 41) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
eutran_bearer_context_mod_confirm_o::value_c eutran_bearer_context_mod_confirm_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 41) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_bearer_context_mod_confirm_o::get_presence(const uint32_t& id)
{
  if (id == 41) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_mod_confirm_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("DRB-Confirm-Modified-List-EUTRAN");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE eutran_bearer_context_mod_confirm_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 32, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_mod_confirm_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 32, true));
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_mod_confirm_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-Confirm-Modified-List-EUTRAN"};
  return convert_enum_idx(names, 1, value, "eutran_bearer_context_mod_confirm_o::value_c::types");
}

// NG-RAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_confirm_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {50};
  return map_enum_number(names, 1, idx, "id");
}
bool ng_ran_bearer_context_mod_confirm_o::is_id_valid(const uint32_t& id)
{
  return 50 == id;
}
crit_e ng_ran_bearer_context_mod_confirm_o::get_crit(const uint32_t& id)
{
  if (id == 50) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ng_ran_bearer_context_mod_confirm_o::value_c ng_ran_bearer_context_mod_confirm_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 50) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ng_ran_bearer_context_mod_confirm_o::get_presence(const uint32_t& id)
{
  if (id == 50) {
    return presence_e::optional;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_bearer_context_mod_confirm_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("PDU-Session-Resource-Confirm-Modified-List");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE ng_ran_bearer_context_mod_confirm_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 256, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_mod_confirm_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 256, true));
  return SRSASN_SUCCESS;
}

const char* ng_ran_bearer_context_mod_confirm_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDU-Session-Resource-Confirm-Modified-List"};
  return convert_enum_idx(names, 1, value, "ng_ran_bearer_context_mod_confirm_o::value_c::types");
}

// System-BearerContextModificationConfirm ::= CHOICE
void sys_bearer_context_mod_confirm_c::destroy_()
{
  switch (type_) {
    case types::e_utran_bearer_context_mod_confirm:
      c.destroy<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>();
      break;
    case types::ng_ran_bearer_context_mod_confirm:
      c.destroy<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void sys_bearer_context_mod_confirm_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e_utran_bearer_context_mod_confirm:
      c.init<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>();
      break;
    case types::ng_ran_bearer_context_mod_confirm:
      c.init<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_confirm_c");
  }
}
sys_bearer_context_mod_confirm_c::sys_bearer_context_mod_confirm_c(const sys_bearer_context_mod_confirm_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e_utran_bearer_context_mod_confirm:
      c.init(other.c.get<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>());
      break;
    case types::ng_ran_bearer_context_mod_confirm:
      c.init(other.c.get<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_confirm_c");
  }
}
sys_bearer_context_mod_confirm_c&
sys_bearer_context_mod_confirm_c::operator=(const sys_bearer_context_mod_confirm_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e_utran_bearer_context_mod_confirm:
      c.set(other.c.get<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>());
      break;
    case types::ng_ran_bearer_context_mod_confirm:
      c.set(other.c.get<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_confirm_c");
  }

  return *this;
}
protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>&
sys_bearer_context_mod_confirm_c::set_e_utran_bearer_context_mod_confirm()
{
  set(types::e_utran_bearer_context_mod_confirm);
  return c.get<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>();
}
protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>&
sys_bearer_context_mod_confirm_c::set_ng_ran_bearer_context_mod_confirm()
{
  set(types::ng_ran_bearer_context_mod_confirm);
  return c.get<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>&
sys_bearer_context_mod_confirm_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>();
}
void sys_bearer_context_mod_confirm_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_utran_bearer_context_mod_confirm:
      j.write_fieldname("e-UTRAN-BearerContextModificationConfirm");
      break;
    case types::ng_ran_bearer_context_mod_confirm:
      j.write_fieldname("nG-RAN-BearerContextModificationConfirm");
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_confirm_c");
  }
  j.end_obj();
}
SRSASN_CODE sys_bearer_context_mod_confirm_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e_utran_bearer_context_mod_confirm:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>(), 0, 65535, true));
      break;
    case types::ng_ran_bearer_context_mod_confirm:
      HANDLE_CODE(
          pack_dyn_seq_of(bref, c.get<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>(), 0, 65535, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_confirm_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_bearer_context_mod_confirm_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e_utran_bearer_context_mod_confirm:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<protocol_ie_container_l<eutran_bearer_context_mod_confirm_o>>(), bref, 0, 65535, true));
      break;
    case types::ng_ran_bearer_context_mod_confirm:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<protocol_ie_container_l<ng_ran_bearer_context_mod_confirm_o>>(), bref, 0, 65535, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_confirm_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_bearer_context_mod_confirm_c::types_opts::to_string() const
{
  static const char* names[] = {
      "e-UTRAN-BearerContextModificationConfirm", "nG-RAN-BearerContextModificationConfirm", "choice-extension"};
  return convert_enum_idx(names, 3, value, "sys_bearer_context_mod_confirm_c::types");
}

// BearerContextModificationConfirmIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_confirm_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 20};
  return map_enum_number(names, 3, idx, "id");
}
bool bearer_context_mod_confirm_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 20};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_mod_confirm_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 20:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_mod_confirm_ies_o::value_c bearer_context_mod_confirm_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 20:
      ret.set(value_c::types::sys_bearer_context_mod_confirm);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_mod_confirm_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_mod_confirm_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::sys_bearer_context_mod_confirm:
      c = sys_bearer_context_mod_confirm_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_confirm_ies_o::value_c");
  }
}
uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_mod_confirm_c& bearer_context_mod_confirm_ies_o::value_c::sys_bearer_context_mod_confirm()
{
  assert_choice_type(types::sys_bearer_context_mod_confirm, type_, "Value");
  return c.get<sys_bearer_context_mod_confirm_c>();
}
const uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_confirm_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const sys_bearer_context_mod_confirm_c&
bearer_context_mod_confirm_ies_o::value_c::sys_bearer_context_mod_confirm() const
{
  assert_choice_type(types::sys_bearer_context_mod_confirm, type_, "Value");
  return c.get<sys_bearer_context_mod_confirm_c>();
}
void bearer_context_mod_confirm_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_confirm:
      j.write_fieldname("System-BearerContextModificationConfirm");
      c.get<sys_bearer_context_mod_confirm_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_confirm_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_mod_confirm_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_mod_confirm:
      HANDLE_CODE(c.get<sys_bearer_context_mod_confirm_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_confirm_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_confirm_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_mod_confirm:
      HANDLE_CODE(c.get<sys_bearer_context_mod_confirm_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_confirm_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_mod_confirm_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextModificationConfirm"};
  return convert_enum_idx(names, 3, value, "bearer_context_mod_confirm_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_confirm_ies_o>;

SRSASN_CODE bearer_context_mod_confirm_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += sys_bearer_context_mod_confirm_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (sys_bearer_context_mod_confirm_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)20, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sys_bearer_context_mod_confirm.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_confirm_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 20: {
        sys_bearer_context_mod_confirm_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_confirm.unpack(bref));
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
void bearer_context_mod_confirm_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  if (sys_bearer_context_mod_confirm_present) {
    j.write_int("id", 20);
    j.write_str("criticality", "ignore");
    sys_bearer_context_mod_confirm.to_json(j);
  }
  j.end_obj();
}

// BearerContextModificationFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 0, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool bearer_context_mod_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 0, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_mod_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_mod_fail_ies_o::value_c bearer_context_mod_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_mod_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_mod_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
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
      log_invalid_choice_id(type_, "bearer_context_mod_fail_ies_o::value_c");
  }
}
uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& bearer_context_mod_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& bearer_context_mod_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_fail_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& bearer_context_mod_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& bearer_context_mod_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void bearer_context_mod_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
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
      log_invalid_choice_id(type_, "bearer_context_mod_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_mod_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "bearer_context_mod_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_fail_ies_o>;

SRSASN_CODE bearer_context_mod_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_fail_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 1: {
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
void bearer_context_mod_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

// EUTRAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {51, 33, 34, 83, 84};
  return map_enum_number(names, 5, idx, "id");
}
bool eutran_bearer_context_mod_request_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {51, 33, 34, 83, 84};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e eutran_bearer_context_mod_request_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 51:
      return crit_e::reject;
    case 33:
      return crit_e::reject;
    case 34:
      return crit_e::reject;
    case 83:
      return crit_e::ignore;
    case 84:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
eutran_bearer_context_mod_request_o::value_c eutran_bearer_context_mod_request_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 51:
      ret.set(value_c::types::drb_to_setup_mod_list_eutran);
      break;
    case 33:
      ret.set(value_c::types::drb_to_modify_list_eutran);
      break;
    case 34:
      ret.set(value_c::types::drb_to_rem_list_eutran);
      break;
    case 83:
      ret.set(value_c::types::sub_profile_id_for_rfp);
      break;
    case 84:
      ret.set(value_c::types::add_rrm_prio_idx);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_bearer_context_mod_request_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 51:
      return presence_e::optional;
    case 33:
      return presence_e::optional;
    case 34:
      return presence_e::optional;
    case 83:
      return presence_e::optional;
    case 84:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_mod_request_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      c = drb_to_setup_mod_list_eutran_l{};
      break;
    case types::drb_to_modify_list_eutran:
      c = drb_to_modify_list_eutran_l{};
      break;
    case types::drb_to_rem_list_eutran:
      c = drb_to_rem_list_eutran_l{};
      break;
    case types::sub_profile_id_for_rfp:
      c = uint16_t{};
      break;
    case types::add_rrm_prio_idx:
      c = fixed_bitstring<32, false, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
  }
}
drb_to_setup_mod_list_eutran_l& eutran_bearer_context_mod_request_o::value_c::drb_to_setup_mod_list_eutran()
{
  assert_choice_type(types::drb_to_setup_mod_list_eutran, type_, "Value");
  return c.get<drb_to_setup_mod_list_eutran_l>();
}
drb_to_modify_list_eutran_l& eutran_bearer_context_mod_request_o::value_c::drb_to_modify_list_eutran()
{
  assert_choice_type(types::drb_to_modify_list_eutran, type_, "Value");
  return c.get<drb_to_modify_list_eutran_l>();
}
drb_to_rem_list_eutran_l& eutran_bearer_context_mod_request_o::value_c::drb_to_rem_list_eutran()
{
  assert_choice_type(types::drb_to_rem_list_eutran, type_, "Value");
  return c.get<drb_to_rem_list_eutran_l>();
}
uint16_t& eutran_bearer_context_mod_request_o::value_c::sub_profile_id_for_rfp()
{
  assert_choice_type(types::sub_profile_id_for_rfp, type_, "Value");
  return c.get<uint16_t>();
}
fixed_bitstring<32, false, true>& eutran_bearer_context_mod_request_o::value_c::add_rrm_prio_idx()
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
const drb_to_setup_mod_list_eutran_l& eutran_bearer_context_mod_request_o::value_c::drb_to_setup_mod_list_eutran() const
{
  assert_choice_type(types::drb_to_setup_mod_list_eutran, type_, "Value");
  return c.get<drb_to_setup_mod_list_eutran_l>();
}
const drb_to_modify_list_eutran_l& eutran_bearer_context_mod_request_o::value_c::drb_to_modify_list_eutran() const
{
  assert_choice_type(types::drb_to_modify_list_eutran, type_, "Value");
  return c.get<drb_to_modify_list_eutran_l>();
}
const drb_to_rem_list_eutran_l& eutran_bearer_context_mod_request_o::value_c::drb_to_rem_list_eutran() const
{
  assert_choice_type(types::drb_to_rem_list_eutran, type_, "Value");
  return c.get<drb_to_rem_list_eutran_l>();
}
const uint16_t& eutran_bearer_context_mod_request_o::value_c::sub_profile_id_for_rfp() const
{
  assert_choice_type(types::sub_profile_id_for_rfp, type_, "Value");
  return c.get<uint16_t>();
}
const fixed_bitstring<32, false, true>& eutran_bearer_context_mod_request_o::value_c::add_rrm_prio_idx() const
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
void eutran_bearer_context_mod_request_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      j.start_array("DRB-To-Setup-Mod-List-EUTRAN");
      for (const auto& e1 : c.get<drb_to_setup_mod_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_to_modify_list_eutran:
      j.start_array("DRB-To-Modify-List-EUTRAN");
      for (const auto& e1 : c.get<drb_to_modify_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_to_rem_list_eutran:
      j.start_array("DRB-To-Remove-List-EUTRAN");
      for (const auto& e1 : c.get<drb_to_rem_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sub_profile_id_for_rfp:
      j.write_int("INTEGER (1..256,...)", c.get<uint16_t>());
      break;
    case types::add_rrm_prio_idx:
      j.write_str("BIT STRING", c.get<fixed_bitstring<32, false, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE eutran_bearer_context_mod_request_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_to_setup_mod_list_eutran_l>(), 1, 32, true));
      break;
    case types::drb_to_modify_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_to_modify_list_eutran_l>(), 1, 32, true));
      break;
    case types::drb_to_rem_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_to_rem_list_eutran_l>(), 1, 32, true));
      break;
    case types::sub_profile_id_for_rfp:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)256u, true, true));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_mod_request_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_to_setup_mod_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_to_setup_mod_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::drb_to_modify_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_to_modify_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::drb_to_rem_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_to_rem_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::sub_profile_id_for_rfp:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)256u, true, true));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_request_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_mod_request_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-To-Setup-Mod-List-EUTRAN",
                                "DRB-To-Modify-List-EUTRAN",
                                "DRB-To-Remove-List-EUTRAN",
                                "INTEGER (1..256,...)",
                                "BIT STRING"};
  return convert_enum_idx(names, 5, value, "eutran_bearer_context_mod_request_o::value_c::types");
}
uint8_t eutran_bearer_context_mod_request_o::value_c::types_opts::to_number() const
{
  if (value == sub_profile_id_for_rfp) {
    return 1;
  }
  invalid_enum_number(value, "eutran_bearer_context_mod_request_o::value_c::types");
  return 0;
}

// NG-RAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {56, 43, 44};
  return map_enum_number(names, 3, idx, "id");
}
bool ng_ran_bearer_context_mod_request_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {56, 43, 44};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ng_ran_bearer_context_mod_request_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 56:
      return crit_e::reject;
    case 43:
      return crit_e::reject;
    case 44:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ng_ran_bearer_context_mod_request_o::value_c ng_ran_bearer_context_mod_request_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 56:
      ret.set(value_c::types::pdu_session_res_to_setup_mod_list);
      break;
    case 43:
      ret.set(value_c::types::pdu_session_res_to_modify_list);
      break;
    case 44:
      ret.set(value_c::types::pdu_session_res_to_rem_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ng_ran_bearer_context_mod_request_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 56:
      return presence_e::optional;
    case 43:
      return presence_e::optional;
    case 44:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_bearer_context_mod_request_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      c = pdu_session_res_to_setup_mod_list_l{};
      break;
    case types::pdu_session_res_to_modify_list:
      c = pdu_session_res_to_modify_list_l{};
      break;
    case types::pdu_session_res_to_rem_list:
      c = pdu_session_res_to_rem_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_request_o::value_c");
  }
}
pdu_session_res_to_setup_mod_list_l& ng_ran_bearer_context_mod_request_o::value_c::pdu_session_res_to_setup_mod_list()
{
  assert_choice_type(types::pdu_session_res_to_setup_mod_list, type_, "Value");
  return c.get<pdu_session_res_to_setup_mod_list_l>();
}
pdu_session_res_to_modify_list_l& ng_ran_bearer_context_mod_request_o::value_c::pdu_session_res_to_modify_list()
{
  assert_choice_type(types::pdu_session_res_to_modify_list, type_, "Value");
  return c.get<pdu_session_res_to_modify_list_l>();
}
pdu_session_res_to_rem_list_l& ng_ran_bearer_context_mod_request_o::value_c::pdu_session_res_to_rem_list()
{
  assert_choice_type(types::pdu_session_res_to_rem_list, type_, "Value");
  return c.get<pdu_session_res_to_rem_list_l>();
}
const pdu_session_res_to_setup_mod_list_l&
ng_ran_bearer_context_mod_request_o::value_c::pdu_session_res_to_setup_mod_list() const
{
  assert_choice_type(types::pdu_session_res_to_setup_mod_list, type_, "Value");
  return c.get<pdu_session_res_to_setup_mod_list_l>();
}
const pdu_session_res_to_modify_list_l&
ng_ran_bearer_context_mod_request_o::value_c::pdu_session_res_to_modify_list() const
{
  assert_choice_type(types::pdu_session_res_to_modify_list, type_, "Value");
  return c.get<pdu_session_res_to_modify_list_l>();
}
const pdu_session_res_to_rem_list_l& ng_ran_bearer_context_mod_request_o::value_c::pdu_session_res_to_rem_list() const
{
  assert_choice_type(types::pdu_session_res_to_rem_list, type_, "Value");
  return c.get<pdu_session_res_to_rem_list_l>();
}
void ng_ran_bearer_context_mod_request_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      j.start_array("PDU-Session-Resource-To-Setup-Mod-List");
      for (const auto& e1 : c.get<pdu_session_res_to_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_to_modify_list:
      j.start_array("PDU-Session-Resource-To-Modify-List");
      for (const auto& e1 : c.get<pdu_session_res_to_modify_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_to_rem_list:
      j.start_array("PDU-Session-Resource-To-Remove-List");
      for (const auto& e1 : c.get<pdu_session_res_to_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_request_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ng_ran_bearer_context_mod_request_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_to_setup_mod_list_l>(), 1, 256, true));
      break;
    case types::pdu_session_res_to_modify_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_to_modify_list_l>(), 1, 256, true));
      break;
    case types::pdu_session_res_to_rem_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_to_rem_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_request_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_mod_request_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_to_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_to_setup_mod_list_l>(), bref, 1, 256, true));
      break;
    case types::pdu_session_res_to_modify_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_to_modify_list_l>(), bref, 1, 256, true));
      break;
    case types::pdu_session_res_to_rem_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_to_rem_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_request_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ng_ran_bearer_context_mod_request_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDU-Session-Resource-To-Setup-Mod-List",
                                "PDU-Session-Resource-To-Modify-List",
                                "PDU-Session-Resource-To-Remove-List"};
  return convert_enum_idx(names, 3, value, "ng_ran_bearer_context_mod_request_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_mod_request_o>;

SRSASN_CODE eutran_bearer_context_mod_request_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_to_setup_mod_list_eutran_present ? 1 : 0;
  nof_ies += drb_to_modify_list_eutran_present ? 1 : 0;
  nof_ies += drb_to_rem_list_eutran_present ? 1 : 0;
  nof_ies += sub_profile_id_for_rfp_present ? 1 : 0;
  nof_ies += add_rrm_prio_idx_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (drb_to_setup_mod_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)51, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_setup_mod_list_eutran, 1, 32, true));
  }
  if (drb_to_modify_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)33, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_modify_list_eutran, 1, 32, true));
  }
  if (drb_to_rem_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)34, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_rem_list_eutran, 1, 32, true));
  }
  if (sub_profile_id_for_rfp_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)83, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, sub_profile_id_for_rfp, (uint16_t)1u, (uint16_t)256u, true, true));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)84, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_mod_request_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 51: {
        drb_to_setup_mod_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_to_setup_mod_list_eutran, bref, 1, 32, true));
        break;
      }
      case 33: {
        drb_to_modify_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_to_modify_list_eutran, bref, 1, 32, true));
        break;
      }
      case 34: {
        drb_to_rem_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_to_rem_list_eutran, bref, 1, 32, true));
        break;
      }
      case 83: {
        sub_profile_id_for_rfp_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(sub_profile_id_for_rfp, bref, (uint16_t)1u, (uint16_t)256u, true, true));
        break;
      }
      case 84: {
        add_rrm_prio_idx_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void eutran_bearer_context_mod_request_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_to_setup_mod_list_eutran_present) {
    j.write_int("id", 51);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drb_to_setup_mod_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_modify_list_eutran_present) {
    j.write_int("id", 33);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drb_to_modify_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_rem_list_eutran_present) {
    j.write_int("id", 34);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drb_to_rem_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sub_profile_id_for_rfp_present) {
    j.write_int("id", 83);
    j.write_str("criticality", "ignore");
    j.write_int("Value", sub_profile_id_for_rfp);
  }
  if (add_rrm_prio_idx_present) {
    j.write_int("id", 84);
    j.write_str("criticality", "ignore");
    j.write_str("Value", add_rrm_prio_idx.to_string());
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_mod_request_o>;

SRSASN_CODE ng_ran_bearer_context_mod_request_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += pdu_session_res_to_setup_mod_list_present ? 1 : 0;
  nof_ies += pdu_session_res_to_modify_list_present ? 1 : 0;
  nof_ies += pdu_session_res_to_rem_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (pdu_session_res_to_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)56, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_to_setup_mod_list, 1, 256, true));
  }
  if (pdu_session_res_to_modify_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)43, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_to_modify_list, 1, 256, true));
  }
  if (pdu_session_res_to_rem_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)44, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_to_rem_list, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_mod_request_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 56: {
        pdu_session_res_to_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_to_setup_mod_list, bref, 1, 256, true));
        break;
      }
      case 43: {
        pdu_session_res_to_modify_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_to_modify_list, bref, 1, 256, true));
        break;
      }
      case 44: {
        pdu_session_res_to_rem_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_to_rem_list, bref, 1, 256, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void ng_ran_bearer_context_mod_request_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdu_session_res_to_setup_mod_list_present) {
    j.write_int("id", 56);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_to_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdu_session_res_to_modify_list_present) {
    j.write_int("id", 43);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_to_modify_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdu_session_res_to_rem_list_present) {
    j.write_int("id", 44);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_to_rem_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// System-BearerContextModificationRequest ::= CHOICE
void sys_bearer_context_mod_request_c::destroy_()
{
  switch (type_) {
    case types::e_utran_bearer_context_mod_request:
      c.destroy<eutran_bearer_context_mod_request_container>();
      break;
    case types::ng_ran_bearer_context_mod_request:
      c.destroy<ng_ran_bearer_context_mod_request_container>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void sys_bearer_context_mod_request_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e_utran_bearer_context_mod_request:
      c.init<eutran_bearer_context_mod_request_container>();
      break;
    case types::ng_ran_bearer_context_mod_request:
      c.init<ng_ran_bearer_context_mod_request_container>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_request_c");
  }
}
sys_bearer_context_mod_request_c::sys_bearer_context_mod_request_c(const sys_bearer_context_mod_request_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e_utran_bearer_context_mod_request:
      c.init(other.c.get<eutran_bearer_context_mod_request_container>());
      break;
    case types::ng_ran_bearer_context_mod_request:
      c.init(other.c.get<ng_ran_bearer_context_mod_request_container>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_request_c");
  }
}
sys_bearer_context_mod_request_c&
sys_bearer_context_mod_request_c::operator=(const sys_bearer_context_mod_request_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e_utran_bearer_context_mod_request:
      c.set(other.c.get<eutran_bearer_context_mod_request_container>());
      break;
    case types::ng_ran_bearer_context_mod_request:
      c.set(other.c.get<ng_ran_bearer_context_mod_request_container>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_request_c");
  }

  return *this;
}
eutran_bearer_context_mod_request_container& sys_bearer_context_mod_request_c::set_e_utran_bearer_context_mod_request()
{
  set(types::e_utran_bearer_context_mod_request);
  return c.get<eutran_bearer_context_mod_request_container>();
}
ng_ran_bearer_context_mod_request_container& sys_bearer_context_mod_request_c::set_ng_ran_bearer_context_mod_request()
{
  set(types::ng_ran_bearer_context_mod_request);
  return c.get<ng_ran_bearer_context_mod_request_container>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>&
sys_bearer_context_mod_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>();
}
void sys_bearer_context_mod_request_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_utran_bearer_context_mod_request:
      j.write_fieldname("e-UTRAN-BearerContextModificationRequest");
      c.get<eutran_bearer_context_mod_request_container>().to_json(j);
      break;
    case types::ng_ran_bearer_context_mod_request:
      j.write_fieldname("nG-RAN-BearerContextModificationRequest");
      c.get<ng_ran_bearer_context_mod_request_container>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_request_c");
  }
  j.end_obj();
}
SRSASN_CODE sys_bearer_context_mod_request_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e_utran_bearer_context_mod_request:
      HANDLE_CODE(c.get<eutran_bearer_context_mod_request_container>().pack(bref));
      break;
    case types::ng_ran_bearer_context_mod_request:
      HANDLE_CODE(c.get<ng_ran_bearer_context_mod_request_container>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_request_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_bearer_context_mod_request_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e_utran_bearer_context_mod_request:
      HANDLE_CODE(c.get<eutran_bearer_context_mod_request_container>().unpack(bref));
      break;
    case types::ng_ran_bearer_context_mod_request:
      HANDLE_CODE(c.get<ng_ran_bearer_context_mod_request_container>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_request_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_bearer_context_mod_request_c::types_opts::to_string() const
{
  static const char* names[] = {
      "e-UTRAN-BearerContextModificationRequest", "nG-RAN-BearerContextModificationRequest", "choice-extension"};
  return convert_enum_idx(names, 3, value, "sys_bearer_context_mod_request_c::types");
}

// BearerContextModificationRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 13, 14, 66, 17, 26, 59, 70, 18, 76, 77, 23};
  return map_enum_number(names, 13, idx, "id");
}
bool bearer_context_mod_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 13, 14, 66, 17, 26, 59, 70, 18, 76, 77, 23};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_mod_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 66:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 26:
      return crit_e::reject;
    case 59:
      return crit_e::reject;
    case 70:
      return crit_e::ignore;
    case 18:
      return crit_e::reject;
    case 76:
      return crit_e::ignore;
    case 77:
      return crit_e::ignore;
    case 23:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_mod_request_ies_o::value_c bearer_context_mod_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 13:
      ret.set(value_c::types::security_info);
      break;
    case 14:
      ret.set(value_c::types::ue_dl_aggr_max_bit_rate);
      break;
    case 66:
      ret.set(value_c::types::ue_dl_max_integrity_protected_data_rate);
      break;
    case 17:
      ret.set(value_c::types::bearer_context_status_change);
      break;
    case 26:
      ret.set(value_c::types::new_ul_tnl_info_required);
      break;
    case 59:
      ret.set(value_c::types::ue_inactivity_timer);
      break;
    case 70:
      ret.set(value_c::types::data_discard_required);
      break;
    case 18:
      ret.set(value_c::types::sys_bearer_context_mod_request);
      break;
    case 76:
      ret.set(value_c::types::ran_ue_id);
      break;
    case 77:
      ret.set(value_c::types::gnb_du_id);
      break;
    case 23:
      ret.set(value_c::types::activity_notif_level);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_mod_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 13:
      return presence_e::optional;
    case 14:
      return presence_e::optional;
    case 66:
      return presence_e::optional;
    case 17:
      return presence_e::optional;
    case 26:
      return presence_e::optional;
    case 59:
      return presence_e::optional;
    case 70:
      return presence_e::optional;
    case 18:
      return presence_e::optional;
    case 76:
      return presence_e::optional;
    case 77:
      return presence_e::optional;
    case 23:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_mod_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::security_info:
      c = security_info_s{};
      break;
    case types::ue_dl_aggr_max_bit_rate:
      c = uint64_t{};
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      c = uint64_t{};
      break;
    case types::bearer_context_status_change:
      c = bearer_context_status_change_e{};
      break;
    case types::new_ul_tnl_info_required:
      c = new_ul_tnl_info_required_e{};
      break;
    case types::ue_inactivity_timer:
      c = uint16_t{};
      break;
    case types::data_discard_required:
      c = data_discard_required_e{};
      break;
    case types::sys_bearer_context_mod_request:
      c = sys_bearer_context_mod_request_c{};
      break;
    case types::ran_ue_id:
      c = fixed_octstring<8, true>{};
      break;
    case types::gnb_du_id:
      c = uint64_t{};
      break;
    case types::activity_notif_level:
      c = activity_notif_level_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_request_ies_o::value_c");
  }
}
uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
security_info_s& bearer_context_mod_request_ies_o::value_c::security_info()
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
uint64_t& bearer_context_mod_request_ies_o::value_c::ue_dl_aggr_max_bit_rate()
{
  assert_choice_type(types::ue_dl_aggr_max_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_request_ies_o::value_c::ue_dl_max_integrity_protected_data_rate()
{
  assert_choice_type(types::ue_dl_max_integrity_protected_data_rate, type_, "Value");
  return c.get<uint64_t>();
}
bearer_context_status_change_e& bearer_context_mod_request_ies_o::value_c::bearer_context_status_change()
{
  assert_choice_type(types::bearer_context_status_change, type_, "Value");
  return c.get<bearer_context_status_change_e>();
}
new_ul_tnl_info_required_e& bearer_context_mod_request_ies_o::value_c::new_ul_tnl_info_required()
{
  assert_choice_type(types::new_ul_tnl_info_required, type_, "Value");
  return c.get<new_ul_tnl_info_required_e>();
}
uint16_t& bearer_context_mod_request_ies_o::value_c::ue_inactivity_timer()
{
  assert_choice_type(types::ue_inactivity_timer, type_, "Value");
  return c.get<uint16_t>();
}
data_discard_required_e& bearer_context_mod_request_ies_o::value_c::data_discard_required()
{
  assert_choice_type(types::data_discard_required, type_, "Value");
  return c.get<data_discard_required_e>();
}
sys_bearer_context_mod_request_c& bearer_context_mod_request_ies_o::value_c::sys_bearer_context_mod_request()
{
  assert_choice_type(types::sys_bearer_context_mod_request, type_, "Value");
  return c.get<sys_bearer_context_mod_request_c>();
}
fixed_octstring<8, true>& bearer_context_mod_request_ies_o::value_c::ran_ue_id()
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_du_id()
{
  assert_choice_type(types::gnb_du_id, type_, "Value");
  return c.get<uint64_t>();
}
activity_notif_level_e& bearer_context_mod_request_ies_o::value_c::activity_notif_level()
{
  assert_choice_type(types::activity_notif_level, type_, "Value");
  return c.get<activity_notif_level_e>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const security_info_s& bearer_context_mod_request_ies_o::value_c::security_info() const
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::ue_dl_aggr_max_bit_rate() const
{
  assert_choice_type(types::ue_dl_aggr_max_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::ue_dl_max_integrity_protected_data_rate() const
{
  assert_choice_type(types::ue_dl_max_integrity_protected_data_rate, type_, "Value");
  return c.get<uint64_t>();
}
const bearer_context_status_change_e& bearer_context_mod_request_ies_o::value_c::bearer_context_status_change() const
{
  assert_choice_type(types::bearer_context_status_change, type_, "Value");
  return c.get<bearer_context_status_change_e>();
}
const new_ul_tnl_info_required_e& bearer_context_mod_request_ies_o::value_c::new_ul_tnl_info_required() const
{
  assert_choice_type(types::new_ul_tnl_info_required, type_, "Value");
  return c.get<new_ul_tnl_info_required_e>();
}
const uint16_t& bearer_context_mod_request_ies_o::value_c::ue_inactivity_timer() const
{
  assert_choice_type(types::ue_inactivity_timer, type_, "Value");
  return c.get<uint16_t>();
}
const data_discard_required_e& bearer_context_mod_request_ies_o::value_c::data_discard_required() const
{
  assert_choice_type(types::data_discard_required, type_, "Value");
  return c.get<data_discard_required_e>();
}
const sys_bearer_context_mod_request_c&
bearer_context_mod_request_ies_o::value_c::sys_bearer_context_mod_request() const
{
  assert_choice_type(types::sys_bearer_context_mod_request, type_, "Value");
  return c.get<sys_bearer_context_mod_request_c>();
}
const fixed_octstring<8, true>& bearer_context_mod_request_ies_o::value_c::ran_ue_id() const
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
const uint64_t& bearer_context_mod_request_ies_o::value_c::gnb_du_id() const
{
  assert_choice_type(types::gnb_du_id, type_, "Value");
  return c.get<uint64_t>();
}
const activity_notif_level_e& bearer_context_mod_request_ies_o::value_c::activity_notif_level() const
{
  assert_choice_type(types::activity_notif_level, type_, "Value");
  return c.get<activity_notif_level_e>();
}
void bearer_context_mod_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::security_info:
      j.write_fieldname("SecurityInformation");
      c.get<security_info_s>().to_json(j);
      break;
    case types::ue_dl_aggr_max_bit_rate:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::bearer_context_status_change:
      j.write_str("BearerContextStatusChange", c.get<bearer_context_status_change_e>().to_string());
      break;
    case types::new_ul_tnl_info_required:
      j.write_str("New-UL-TNL-Information-Required", "required");
      break;
    case types::ue_inactivity_timer:
      j.write_int("INTEGER (1..7200,...)", c.get<uint16_t>());
      break;
    case types::data_discard_required:
      j.write_str("DataDiscardRequired", "required");
      break;
    case types::sys_bearer_context_mod_request:
      j.write_fieldname("System-BearerContextModificationRequest");
      c.get<sys_bearer_context_mod_request_c>().to_json(j);
      break;
    case types::ran_ue_id:
      j.write_str("OCTET STRING", c.get<fixed_octstring<8, true>>().to_string());
      break;
    case types::gnb_du_id:
      j.write_int("INTEGER (0..68719476735)", c.get<uint64_t>());
      break;
    case types::activity_notif_level:
      j.write_str("ActivityNotificationLevel", c.get<activity_notif_level_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_mod_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().pack(bref));
      break;
    case types::ue_dl_aggr_max_bit_rate:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::bearer_context_status_change:
      HANDLE_CODE(c.get<bearer_context_status_change_e>().pack(bref));
      break;
    case types::new_ul_tnl_info_required:
      HANDLE_CODE(c.get<new_ul_tnl_info_required_e>().pack(bref));
      break;
    case types::ue_inactivity_timer:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)7200u, true, true));
      break;
    case types::data_discard_required:
      HANDLE_CODE(c.get<data_discard_required_e>().pack(bref));
      break;
    case types::sys_bearer_context_mod_request:
      HANDLE_CODE(c.get<sys_bearer_context_mod_request_c>().pack(bref));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().pack(bref)));
      break;
    case types::gnb_du_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::activity_notif_level:
      HANDLE_CODE(c.get<activity_notif_level_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().unpack(bref));
      break;
    case types::ue_dl_aggr_max_bit_rate:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::bearer_context_status_change:
      HANDLE_CODE(c.get<bearer_context_status_change_e>().unpack(bref));
      break;
    case types::new_ul_tnl_info_required:
      HANDLE_CODE(c.get<new_ul_tnl_info_required_e>().unpack(bref));
      break;
    case types::ue_inactivity_timer:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)7200u, true, true));
      break;
    case types::data_discard_required:
      HANDLE_CODE(c.get<data_discard_required_e>().unpack(bref));
      break;
    case types::sys_bearer_context_mod_request:
      HANDLE_CODE(c.get<sys_bearer_context_mod_request_c>().unpack(bref));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().unpack(bref)));
      break;
    case types::gnb_du_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::activity_notif_level:
      HANDLE_CODE(c.get<activity_notif_level_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "SecurityInformation",
                                "INTEGER (0..4000000000000,...)",
                                "INTEGER (0..4000000000000,...)",
                                "BearerContextStatusChange",
                                "New-UL-TNL-Information-Required",
                                "INTEGER (1..7200,...)",
                                "DataDiscardRequired",
                                "System-BearerContextModificationRequest",
                                "OCTET STRING",
                                "INTEGER (0..68719476735)",
                                "ActivityNotificationLevel"};
  return convert_enum_idx(names, 13, value, "bearer_context_mod_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_request_ies_o>;

SRSASN_CODE bearer_context_mod_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += security_info_present ? 1 : 0;
  nof_ies += ue_dl_aggr_max_bit_rate_present ? 1 : 0;
  nof_ies += ue_dl_max_integrity_protected_data_rate_present ? 1 : 0;
  nof_ies += bearer_context_status_change_present ? 1 : 0;
  nof_ies += new_ul_tnl_info_required_present ? 1 : 0;
  nof_ies += ue_inactivity_timer_present ? 1 : 0;
  nof_ies += data_discard_required_present ? 1 : 0;
  nof_ies += sys_bearer_context_mod_request_present ? 1 : 0;
  nof_ies += ran_ue_id_present ? 1 : 0;
  nof_ies += gnb_du_id_present ? 1 : 0;
  nof_ies += activity_notif_level_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (security_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)13, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(security_info.pack(bref));
  }
  if (ue_dl_aggr_max_bit_rate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)14, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ue_dl_aggr_max_bit_rate, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (ue_dl_max_integrity_protected_data_rate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)66, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(
        bref, ue_dl_max_integrity_protected_data_rate, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (bearer_context_status_change_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)17, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(bearer_context_status_change.pack(bref));
  }
  if (new_ul_tnl_info_required_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)26, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(new_ul_tnl_info_required.pack(bref));
  }
  if (ue_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)59, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ue_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (data_discard_required_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)70, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(data_discard_required.pack(bref));
  }
  if (sys_bearer_context_mod_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)18, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sys_bearer_context_mod_request.pack(bref));
  }
  if (ran_ue_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)76, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ran_ue_id.pack(bref));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)77, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (activity_notif_level_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)23, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(activity_notif_level.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_request_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 13: {
        security_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(security_info.unpack(bref));
        break;
      }
      case 14: {
        ue_dl_aggr_max_bit_rate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ue_dl_aggr_max_bit_rate, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 66: {
        ue_dl_max_integrity_protected_data_rate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(
            ue_dl_max_integrity_protected_data_rate, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 17: {
        bearer_context_status_change_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bearer_context_status_change.unpack(bref));
        break;
      }
      case 26: {
        new_ul_tnl_info_required_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(new_ul_tnl_info_required.unpack(bref));
        break;
      }
      case 59: {
        ue_inactivity_timer_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ue_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
        break;
      }
      case 70: {
        data_discard_required_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(data_discard_required.unpack(bref));
        break;
      }
      case 18: {
        sys_bearer_context_mod_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_request.unpack(bref));
        break;
      }
      case 76: {
        ran_ue_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ran_ue_id.unpack(bref));
        break;
      }
      case 77: {
        gnb_du_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
        break;
      }
      case 23: {
        activity_notif_level_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(activity_notif_level.unpack(bref));
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
void bearer_context_mod_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  if (security_info_present) {
    j.write_int("id", 13);
    j.write_str("criticality", "reject");
    security_info.to_json(j);
  }
  if (ue_dl_aggr_max_bit_rate_present) {
    j.write_int("id", 14);
    j.write_str("criticality", "reject");
    j.write_int("Value", ue_dl_aggr_max_bit_rate);
  }
  if (ue_dl_max_integrity_protected_data_rate_present) {
    j.write_int("id", 66);
    j.write_str("criticality", "reject");
    j.write_int("Value", ue_dl_max_integrity_protected_data_rate);
  }
  if (bearer_context_status_change_present) {
    j.write_int("id", 17);
    j.write_str("criticality", "reject");
    j.write_str("Value", bearer_context_status_change.to_string());
  }
  if (new_ul_tnl_info_required_present) {
    j.write_int("id", 26);
    j.write_str("criticality", "reject");
    j.write_str("Value", "required");
  }
  if (ue_inactivity_timer_present) {
    j.write_int("id", 59);
    j.write_str("criticality", "reject");
    j.write_int("Value", ue_inactivity_timer);
  }
  if (data_discard_required_present) {
    j.write_int("id", 70);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "required");
  }
  if (sys_bearer_context_mod_request_present) {
    j.write_int("id", 18);
    j.write_str("criticality", "reject");
    sys_bearer_context_mod_request.to_json(j);
  }
  if (ran_ue_id_present) {
    j.write_int("id", 76);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ran_ue_id.to_string());
  }
  if (gnb_du_id_present) {
    j.write_int("id", 77);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_du_id);
  }
  if (activity_notif_level_present) {
    j.write_int("id", 23);
    j.write_str("criticality", "ignore");
    j.write_str("Value", activity_notif_level.to_string());
  }
  j.end_obj();
}

// EUTRAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_required_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {35, 36};
  return map_enum_number(names, 2, idx, "id");
}
bool eutran_bearer_context_mod_required_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {35, 36};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e eutran_bearer_context_mod_required_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 35:
      return crit_e::reject;
    case 36:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
eutran_bearer_context_mod_required_o::value_c eutran_bearer_context_mod_required_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 35:
      ret.set(value_c::types::drb_required_to_modify_list_eutran);
      break;
    case 36:
      ret.set(value_c::types::drb_required_to_rem_list_eutran);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_bearer_context_mod_required_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 35:
      return presence_e::optional;
    case 36:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_mod_required_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      c = drb_required_to_modify_list_eutran_l{};
      break;
    case types::drb_required_to_rem_list_eutran:
      c = drb_required_to_rem_list_eutran_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_required_o::value_c");
  }
}
drb_required_to_modify_list_eutran_l&
eutran_bearer_context_mod_required_o::value_c::drb_required_to_modify_list_eutran()
{
  assert_choice_type(types::drb_required_to_modify_list_eutran, type_, "Value");
  return c.get<drb_required_to_modify_list_eutran_l>();
}
drb_required_to_rem_list_eutran_l& eutran_bearer_context_mod_required_o::value_c::drb_required_to_rem_list_eutran()
{
  assert_choice_type(types::drb_required_to_rem_list_eutran, type_, "Value");
  return c.get<drb_required_to_rem_list_eutran_l>();
}
const drb_required_to_modify_list_eutran_l&
eutran_bearer_context_mod_required_o::value_c::drb_required_to_modify_list_eutran() const
{
  assert_choice_type(types::drb_required_to_modify_list_eutran, type_, "Value");
  return c.get<drb_required_to_modify_list_eutran_l>();
}
const drb_required_to_rem_list_eutran_l&
eutran_bearer_context_mod_required_o::value_c::drb_required_to_rem_list_eutran() const
{
  assert_choice_type(types::drb_required_to_rem_list_eutran, type_, "Value");
  return c.get<drb_required_to_rem_list_eutran_l>();
}
void eutran_bearer_context_mod_required_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      j.start_array("DRB-Required-To-Modify-List-EUTRAN");
      for (const auto& e1 : c.get<drb_required_to_modify_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_required_to_rem_list_eutran:
      j.start_array("DRB-Required-To-Remove-List-EUTRAN");
      for (const auto& e1 : c.get<drb_required_to_rem_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_required_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE eutran_bearer_context_mod_required_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_required_to_modify_list_eutran_l>(), 1, 32, true));
      break;
    case types::drb_required_to_rem_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_required_to_rem_list_eutran_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_required_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_mod_required_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_required_to_modify_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_required_to_modify_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::drb_required_to_rem_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_required_to_rem_list_eutran_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_required_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_mod_required_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-Required-To-Modify-List-EUTRAN", "DRB-Required-To-Remove-List-EUTRAN"};
  return convert_enum_idx(names, 2, value, "eutran_bearer_context_mod_required_o::value_c::types");
}

// NG-RAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_required_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {45, 44};
  return map_enum_number(names, 2, idx, "id");
}
bool ng_ran_bearer_context_mod_required_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {45, 44};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ng_ran_bearer_context_mod_required_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 45:
      return crit_e::reject;
    case 44:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ng_ran_bearer_context_mod_required_o::value_c ng_ran_bearer_context_mod_required_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 45:
      ret.set(value_c::types::pdu_session_res_required_to_modify_list);
      break;
    case 44:
      ret.set(value_c::types::pdu_session_res_to_rem_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ng_ran_bearer_context_mod_required_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 45:
      return presence_e::optional;
    case 44:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_bearer_context_mod_required_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      c = pdu_session_res_required_to_modify_list_l{};
      break;
    case types::pdu_session_res_to_rem_list:
      c = pdu_session_res_to_rem_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_required_o::value_c");
  }
}
pdu_session_res_required_to_modify_list_l&
ng_ran_bearer_context_mod_required_o::value_c::pdu_session_res_required_to_modify_list()
{
  assert_choice_type(types::pdu_session_res_required_to_modify_list, type_, "Value");
  return c.get<pdu_session_res_required_to_modify_list_l>();
}
pdu_session_res_to_rem_list_l& ng_ran_bearer_context_mod_required_o::value_c::pdu_session_res_to_rem_list()
{
  assert_choice_type(types::pdu_session_res_to_rem_list, type_, "Value");
  return c.get<pdu_session_res_to_rem_list_l>();
}
const pdu_session_res_required_to_modify_list_l&
ng_ran_bearer_context_mod_required_o::value_c::pdu_session_res_required_to_modify_list() const
{
  assert_choice_type(types::pdu_session_res_required_to_modify_list, type_, "Value");
  return c.get<pdu_session_res_required_to_modify_list_l>();
}
const pdu_session_res_to_rem_list_l& ng_ran_bearer_context_mod_required_o::value_c::pdu_session_res_to_rem_list() const
{
  assert_choice_type(types::pdu_session_res_to_rem_list, type_, "Value");
  return c.get<pdu_session_res_to_rem_list_l>();
}
void ng_ran_bearer_context_mod_required_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      j.start_array("PDU-Session-Resource-Required-To-Modify-List");
      for (const auto& e1 : c.get<pdu_session_res_required_to_modify_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_to_rem_list:
      j.start_array("PDU-Session-Resource-To-Remove-List");
      for (const auto& e1 : c.get<pdu_session_res_to_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_required_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ng_ran_bearer_context_mod_required_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_required_to_modify_list_l>(), 1, 256, true));
      break;
    case types::pdu_session_res_to_rem_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_to_rem_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_required_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_mod_required_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_required_to_modify_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_required_to_modify_list_l>(), bref, 1, 256, true));
      break;
    case types::pdu_session_res_to_rem_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_to_rem_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_required_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ng_ran_bearer_context_mod_required_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDU-Session-Resource-Required-To-Modify-List", "PDU-Session-Resource-To-Remove-List"};
  return convert_enum_idx(names, 2, value, "ng_ran_bearer_context_mod_required_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_mod_required_o>;

SRSASN_CODE eutran_bearer_context_mod_required_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_required_to_modify_list_eutran_present ? 1 : 0;
  nof_ies += drb_required_to_rem_list_eutran_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (drb_required_to_modify_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)35, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_required_to_modify_list_eutran, 1, 32, true));
  }
  if (drb_required_to_rem_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)36, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_required_to_rem_list_eutran, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_mod_required_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 35: {
        drb_required_to_modify_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_required_to_modify_list_eutran, bref, 1, 32, true));
        break;
      }
      case 36: {
        drb_required_to_rem_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_required_to_rem_list_eutran, bref, 1, 32, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void eutran_bearer_context_mod_required_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_required_to_modify_list_eutran_present) {
    j.write_int("id", 35);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drb_required_to_modify_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_required_to_rem_list_eutran_present) {
    j.write_int("id", 36);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drb_required_to_rem_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_mod_required_o>;

SRSASN_CODE ng_ran_bearer_context_mod_required_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += pdu_session_res_required_to_modify_list_present ? 1 : 0;
  nof_ies += pdu_session_res_to_rem_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (pdu_session_res_required_to_modify_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)45, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_required_to_modify_list, 1, 256, true));
  }
  if (pdu_session_res_to_rem_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)44, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_to_rem_list, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_mod_required_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 45: {
        pdu_session_res_required_to_modify_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_required_to_modify_list, bref, 1, 256, true));
        break;
      }
      case 44: {
        pdu_session_res_to_rem_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_to_rem_list, bref, 1, 256, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void ng_ran_bearer_context_mod_required_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdu_session_res_required_to_modify_list_present) {
    j.write_int("id", 45);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_required_to_modify_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdu_session_res_to_rem_list_present) {
    j.write_int("id", 44);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_to_rem_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// System-BearerContextModificationRequired ::= CHOICE
void sys_bearer_context_mod_required_c::destroy_()
{
  switch (type_) {
    case types::e_utran_bearer_context_mod_required:
      c.destroy<eutran_bearer_context_mod_required_container>();
      break;
    case types::ng_ran_bearer_context_mod_required:
      c.destroy<ng_ran_bearer_context_mod_required_container>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void sys_bearer_context_mod_required_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e_utran_bearer_context_mod_required:
      c.init<eutran_bearer_context_mod_required_container>();
      break;
    case types::ng_ran_bearer_context_mod_required:
      c.init<ng_ran_bearer_context_mod_required_container>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_required_c");
  }
}
sys_bearer_context_mod_required_c::sys_bearer_context_mod_required_c(const sys_bearer_context_mod_required_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e_utran_bearer_context_mod_required:
      c.init(other.c.get<eutran_bearer_context_mod_required_container>());
      break;
    case types::ng_ran_bearer_context_mod_required:
      c.init(other.c.get<ng_ran_bearer_context_mod_required_container>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_required_c");
  }
}
sys_bearer_context_mod_required_c&
sys_bearer_context_mod_required_c::operator=(const sys_bearer_context_mod_required_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e_utran_bearer_context_mod_required:
      c.set(other.c.get<eutran_bearer_context_mod_required_container>());
      break;
    case types::ng_ran_bearer_context_mod_required:
      c.set(other.c.get<ng_ran_bearer_context_mod_required_container>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_required_c");
  }

  return *this;
}
eutran_bearer_context_mod_required_container&
sys_bearer_context_mod_required_c::set_e_utran_bearer_context_mod_required()
{
  set(types::e_utran_bearer_context_mod_required);
  return c.get<eutran_bearer_context_mod_required_container>();
}
ng_ran_bearer_context_mod_required_container&
sys_bearer_context_mod_required_c::set_ng_ran_bearer_context_mod_required()
{
  set(types::ng_ran_bearer_context_mod_required);
  return c.get<ng_ran_bearer_context_mod_required_container>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>&
sys_bearer_context_mod_required_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>();
}
void sys_bearer_context_mod_required_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_utran_bearer_context_mod_required:
      j.write_fieldname("e-UTRAN-BearerContextModificationRequired");
      c.get<eutran_bearer_context_mod_required_container>().to_json(j);
      break;
    case types::ng_ran_bearer_context_mod_required:
      j.write_fieldname("nG-RAN-BearerContextModificationRequired");
      c.get<ng_ran_bearer_context_mod_required_container>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_required_c");
  }
  j.end_obj();
}
SRSASN_CODE sys_bearer_context_mod_required_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e_utran_bearer_context_mod_required:
      HANDLE_CODE(c.get<eutran_bearer_context_mod_required_container>().pack(bref));
      break;
    case types::ng_ran_bearer_context_mod_required:
      HANDLE_CODE(c.get<ng_ran_bearer_context_mod_required_container>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_required_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_bearer_context_mod_required_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e_utran_bearer_context_mod_required:
      HANDLE_CODE(c.get<eutran_bearer_context_mod_required_container>().unpack(bref));
      break;
    case types::ng_ran_bearer_context_mod_required:
      HANDLE_CODE(c.get<ng_ran_bearer_context_mod_required_container>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_required_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_bearer_context_mod_required_c::types_opts::to_string() const
{
  static const char* names[] = {
      "e-UTRAN-BearerContextModificationRequired", "nG-RAN-BearerContextModificationRequired", "choice-extension"};
  return convert_enum_idx(names, 3, value, "sys_bearer_context_mod_required_c::types");
}

// BearerContextModificationRequiredIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_required_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 21};
  return map_enum_number(names, 3, idx, "id");
}
bool bearer_context_mod_required_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 21};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_mod_required_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 21:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_mod_required_ies_o::value_c bearer_context_mod_required_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 21:
      ret.set(value_c::types::sys_bearer_context_mod_required);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_mod_required_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 21:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_mod_required_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::sys_bearer_context_mod_required:
      c = sys_bearer_context_mod_required_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_required_ies_o::value_c");
  }
}
uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_mod_required_c& bearer_context_mod_required_ies_o::value_c::sys_bearer_context_mod_required()
{
  assert_choice_type(types::sys_bearer_context_mod_required, type_, "Value");
  return c.get<sys_bearer_context_mod_required_c>();
}
const uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_required_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const sys_bearer_context_mod_required_c&
bearer_context_mod_required_ies_o::value_c::sys_bearer_context_mod_required() const
{
  assert_choice_type(types::sys_bearer_context_mod_required, type_, "Value");
  return c.get<sys_bearer_context_mod_required_c>();
}
void bearer_context_mod_required_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_required:
      j.write_fieldname("System-BearerContextModificationRequired");
      c.get<sys_bearer_context_mod_required_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_required_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_mod_required_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_mod_required:
      HANDLE_CODE(c.get<sys_bearer_context_mod_required_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_required_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_required_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_mod_required:
      HANDLE_CODE(c.get<sys_bearer_context_mod_required_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_required_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_mod_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextModificationRequired"};
  return convert_enum_idx(names, 3, value, "bearer_context_mod_required_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_required_ies_o>;

SRSASN_CODE bearer_context_mod_required_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)21, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sys_bearer_context_mod_required.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_required_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 21: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_required.unpack(bref));
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
void bearer_context_mod_required_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 21);
  j.write_str("criticality", "reject");
  sys_bearer_context_mod_required.to_json(j);
  j.end_obj();
}

// EUTRAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_mod_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {52, 53, 39, 40, 85};
  return map_enum_number(names, 5, idx, "id");
}
bool eutran_bearer_context_mod_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {52, 53, 39, 40, 85};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e eutran_bearer_context_mod_resp_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 52:
      return crit_e::ignore;
    case 53:
      return crit_e::ignore;
    case 39:
      return crit_e::ignore;
    case 40:
      return crit_e::ignore;
    case 85:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
eutran_bearer_context_mod_resp_o::value_c eutran_bearer_context_mod_resp_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 52:
      ret.set(value_c::types::drb_setup_mod_list_eutran);
      break;
    case 53:
      ret.set(value_c::types::drb_failed_mod_list_eutran);
      break;
    case 39:
      ret.set(value_c::types::drb_modified_list_eutran);
      break;
    case 40:
      ret.set(value_c::types::drb_failed_to_modify_list_eutran);
      break;
    case 85:
      ret.set(value_c::types::retainability_meass_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_bearer_context_mod_resp_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 52:
      return presence_e::optional;
    case 53:
      return presence_e::optional;
    case 39:
      return presence_e::optional;
    case 40:
      return presence_e::optional;
    case 85:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_mod_resp_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      c = drb_setup_mod_list_eutran_l{};
      break;
    case types::drb_failed_mod_list_eutran:
      c = drb_failed_mod_list_eutran_l{};
      break;
    case types::drb_modified_list_eutran:
      c = drb_modified_list_eutran_l{};
      break;
    case types::drb_failed_to_modify_list_eutran:
      c = drb_failed_to_modify_list_eutran_l{};
      break;
    case types::retainability_meass_info:
      c = retainability_meass_info_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
  }
}
drb_setup_mod_list_eutran_l& eutran_bearer_context_mod_resp_o::value_c::drb_setup_mod_list_eutran()
{
  assert_choice_type(types::drb_setup_mod_list_eutran, type_, "Value");
  return c.get<drb_setup_mod_list_eutran_l>();
}
drb_failed_mod_list_eutran_l& eutran_bearer_context_mod_resp_o::value_c::drb_failed_mod_list_eutran()
{
  assert_choice_type(types::drb_failed_mod_list_eutran, type_, "Value");
  return c.get<drb_failed_mod_list_eutran_l>();
}
drb_modified_list_eutran_l& eutran_bearer_context_mod_resp_o::value_c::drb_modified_list_eutran()
{
  assert_choice_type(types::drb_modified_list_eutran, type_, "Value");
  return c.get<drb_modified_list_eutran_l>();
}
drb_failed_to_modify_list_eutran_l& eutran_bearer_context_mod_resp_o::value_c::drb_failed_to_modify_list_eutran()
{
  assert_choice_type(types::drb_failed_to_modify_list_eutran, type_, "Value");
  return c.get<drb_failed_to_modify_list_eutran_l>();
}
retainability_meass_info_l& eutran_bearer_context_mod_resp_o::value_c::retainability_meass_info()
{
  assert_choice_type(types::retainability_meass_info, type_, "Value");
  return c.get<retainability_meass_info_l>();
}
const drb_setup_mod_list_eutran_l& eutran_bearer_context_mod_resp_o::value_c::drb_setup_mod_list_eutran() const
{
  assert_choice_type(types::drb_setup_mod_list_eutran, type_, "Value");
  return c.get<drb_setup_mod_list_eutran_l>();
}
const drb_failed_mod_list_eutran_l& eutran_bearer_context_mod_resp_o::value_c::drb_failed_mod_list_eutran() const
{
  assert_choice_type(types::drb_failed_mod_list_eutran, type_, "Value");
  return c.get<drb_failed_mod_list_eutran_l>();
}
const drb_modified_list_eutran_l& eutran_bearer_context_mod_resp_o::value_c::drb_modified_list_eutran() const
{
  assert_choice_type(types::drb_modified_list_eutran, type_, "Value");
  return c.get<drb_modified_list_eutran_l>();
}
const drb_failed_to_modify_list_eutran_l&
eutran_bearer_context_mod_resp_o::value_c::drb_failed_to_modify_list_eutran() const
{
  assert_choice_type(types::drb_failed_to_modify_list_eutran, type_, "Value");
  return c.get<drb_failed_to_modify_list_eutran_l>();
}
const retainability_meass_info_l& eutran_bearer_context_mod_resp_o::value_c::retainability_meass_info() const
{
  assert_choice_type(types::retainability_meass_info, type_, "Value");
  return c.get<retainability_meass_info_l>();
}
void eutran_bearer_context_mod_resp_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      j.start_array("DRB-Setup-Mod-List-EUTRAN");
      for (const auto& e1 : c.get<drb_setup_mod_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_failed_mod_list_eutran:
      j.start_array("DRB-Failed-Mod-List-EUTRAN");
      for (const auto& e1 : c.get<drb_failed_mod_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_modified_list_eutran:
      j.start_array("DRB-Modified-List-EUTRAN");
      for (const auto& e1 : c.get<drb_modified_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_failed_to_modify_list_eutran:
      j.start_array("DRB-Failed-To-Modify-List-EUTRAN");
      for (const auto& e1 : c.get<drb_failed_to_modify_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::retainability_meass_info:
      j.start_array("RetainabilityMeasurementsInfo");
      for (const auto& e1 : c.get<retainability_meass_info_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE eutran_bearer_context_mod_resp_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_setup_mod_list_eutran_l>(), 1, 32, true));
      break;
    case types::drb_failed_mod_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_failed_mod_list_eutran_l>(), 1, 32, true));
      break;
    case types::drb_modified_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_modified_list_eutran_l>(), 1, 32, true));
      break;
    case types::drb_failed_to_modify_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_failed_to_modify_list_eutran_l>(), 1, 32, true));
      break;
    case types::retainability_meass_info:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<retainability_meass_info_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_mod_resp_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_setup_mod_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_setup_mod_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::drb_failed_mod_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_failed_mod_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::drb_modified_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_modified_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::drb_failed_to_modify_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_failed_to_modify_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::retainability_meass_info:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<retainability_meass_info_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_mod_resp_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_mod_resp_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-Setup-Mod-List-EUTRAN",
                                "DRB-Failed-Mod-List-EUTRAN",
                                "DRB-Modified-List-EUTRAN",
                                "DRB-Failed-To-Modify-List-EUTRAN",
                                "RetainabilityMeasurementsInfo"};
  return convert_enum_idx(names, 5, value, "eutran_bearer_context_mod_resp_o::value_c::types");
}

// NG-RAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_mod_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {54, 55, 48, 49, 85};
  return map_enum_number(names, 5, idx, "id");
}
bool ng_ran_bearer_context_mod_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {54, 55, 48, 49, 85};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ng_ran_bearer_context_mod_resp_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 54:
      return crit_e::reject;
    case 55:
      return crit_e::reject;
    case 48:
      return crit_e::reject;
    case 49:
      return crit_e::reject;
    case 85:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ng_ran_bearer_context_mod_resp_o::value_c ng_ran_bearer_context_mod_resp_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 54:
      ret.set(value_c::types::pdu_session_res_setup_mod_list);
      break;
    case 55:
      ret.set(value_c::types::pdu_session_res_failed_mod_list);
      break;
    case 48:
      ret.set(value_c::types::pdu_session_res_modified_list);
      break;
    case 49:
      ret.set(value_c::types::pdu_session_res_failed_to_modify_list);
      break;
    case 85:
      ret.set(value_c::types::retainability_meass_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ng_ran_bearer_context_mod_resp_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 54:
      return presence_e::optional;
    case 55:
      return presence_e::optional;
    case 48:
      return presence_e::optional;
    case 49:
      return presence_e::optional;
    case 85:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_bearer_context_mod_resp_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      c = pdu_session_res_setup_mod_list_l{};
      break;
    case types::pdu_session_res_failed_mod_list:
      c = pdu_session_res_failed_mod_list_l{};
      break;
    case types::pdu_session_res_modified_list:
      c = pdu_session_res_modified_list_l{};
      break;
    case types::pdu_session_res_failed_to_modify_list:
      c = pdu_session_res_failed_to_modify_list_l{};
      break;
    case types::retainability_meass_info:
      c = retainability_meass_info_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
  }
}
pdu_session_res_setup_mod_list_l& ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_setup_mod_list()
{
  assert_choice_type(types::pdu_session_res_setup_mod_list, type_, "Value");
  return c.get<pdu_session_res_setup_mod_list_l>();
}
pdu_session_res_failed_mod_list_l& ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_failed_mod_list()
{
  assert_choice_type(types::pdu_session_res_failed_mod_list, type_, "Value");
  return c.get<pdu_session_res_failed_mod_list_l>();
}
pdu_session_res_modified_list_l& ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_modified_list()
{
  assert_choice_type(types::pdu_session_res_modified_list, type_, "Value");
  return c.get<pdu_session_res_modified_list_l>();
}
pdu_session_res_failed_to_modify_list_l&
ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_failed_to_modify_list()
{
  assert_choice_type(types::pdu_session_res_failed_to_modify_list, type_, "Value");
  return c.get<pdu_session_res_failed_to_modify_list_l>();
}
retainability_meass_info_l& ng_ran_bearer_context_mod_resp_o::value_c::retainability_meass_info()
{
  assert_choice_type(types::retainability_meass_info, type_, "Value");
  return c.get<retainability_meass_info_l>();
}
const pdu_session_res_setup_mod_list_l&
ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_setup_mod_list() const
{
  assert_choice_type(types::pdu_session_res_setup_mod_list, type_, "Value");
  return c.get<pdu_session_res_setup_mod_list_l>();
}
const pdu_session_res_failed_mod_list_l&
ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_failed_mod_list() const
{
  assert_choice_type(types::pdu_session_res_failed_mod_list, type_, "Value");
  return c.get<pdu_session_res_failed_mod_list_l>();
}
const pdu_session_res_modified_list_l& ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_modified_list() const
{
  assert_choice_type(types::pdu_session_res_modified_list, type_, "Value");
  return c.get<pdu_session_res_modified_list_l>();
}
const pdu_session_res_failed_to_modify_list_l&
ng_ran_bearer_context_mod_resp_o::value_c::pdu_session_res_failed_to_modify_list() const
{
  assert_choice_type(types::pdu_session_res_failed_to_modify_list, type_, "Value");
  return c.get<pdu_session_res_failed_to_modify_list_l>();
}
const retainability_meass_info_l& ng_ran_bearer_context_mod_resp_o::value_c::retainability_meass_info() const
{
  assert_choice_type(types::retainability_meass_info, type_, "Value");
  return c.get<retainability_meass_info_l>();
}
void ng_ran_bearer_context_mod_resp_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      j.start_array("PDU-Session-Resource-Setup-Mod-List");
      for (const auto& e1 : c.get<pdu_session_res_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_failed_mod_list:
      j.start_array("PDU-Session-Resource-Failed-Mod-List");
      for (const auto& e1 : c.get<pdu_session_res_failed_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_modified_list:
      j.start_array("PDU-Session-Resource-Modified-List");
      for (const auto& e1 : c.get<pdu_session_res_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_failed_to_modify_list:
      j.start_array("PDU-Session-Resource-Failed-To-Modify-List");
      for (const auto& e1 : c.get<pdu_session_res_failed_to_modify_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::retainability_meass_info:
      j.start_array("RetainabilityMeasurementsInfo");
      for (const auto& e1 : c.get<retainability_meass_info_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ng_ran_bearer_context_mod_resp_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_setup_mod_list_l>(), 1, 256, true));
      break;
    case types::pdu_session_res_failed_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_failed_mod_list_l>(), 1, 256, true));
      break;
    case types::pdu_session_res_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_modified_list_l>(), 1, 256, true));
      break;
    case types::pdu_session_res_failed_to_modify_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_failed_to_modify_list_l>(), 1, 256, true));
      break;
    case types::retainability_meass_info:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<retainability_meass_info_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_mod_resp_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_setup_mod_list_l>(), bref, 1, 256, true));
      break;
    case types::pdu_session_res_failed_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_failed_mod_list_l>(), bref, 1, 256, true));
      break;
    case types::pdu_session_res_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_modified_list_l>(), bref, 1, 256, true));
      break;
    case types::pdu_session_res_failed_to_modify_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_failed_to_modify_list_l>(), bref, 1, 256, true));
      break;
    case types::retainability_meass_info:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<retainability_meass_info_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_mod_resp_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ng_ran_bearer_context_mod_resp_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDU-Session-Resource-Setup-Mod-List",
                                "PDU-Session-Resource-Failed-Mod-List",
                                "PDU-Session-Resource-Modified-List",
                                "PDU-Session-Resource-Failed-To-Modify-List",
                                "RetainabilityMeasurementsInfo"};
  return convert_enum_idx(names, 5, value, "ng_ran_bearer_context_mod_resp_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_mod_resp_o>;

SRSASN_CODE eutran_bearer_context_mod_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += drb_setup_mod_list_eutran_present ? 1 : 0;
  nof_ies += drb_failed_mod_list_eutran_present ? 1 : 0;
  nof_ies += drb_modified_list_eutran_present ? 1 : 0;
  nof_ies += drb_failed_to_modify_list_eutran_present ? 1 : 0;
  nof_ies += retainability_meass_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (drb_setup_mod_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)52, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_setup_mod_list_eutran, 1, 32, true));
  }
  if (drb_failed_mod_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)53, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_failed_mod_list_eutran, 1, 32, true));
  }
  if (drb_modified_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)39, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_modified_list_eutran, 1, 32, true));
  }
  if (drb_failed_to_modify_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_failed_to_modify_list_eutran, 1, 32, true));
  }
  if (retainability_meass_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)85, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, retainability_meass_info, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_mod_resp_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 52: {
        drb_setup_mod_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_setup_mod_list_eutran, bref, 1, 32, true));
        break;
      }
      case 53: {
        drb_failed_mod_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_failed_mod_list_eutran, bref, 1, 32, true));
        break;
      }
      case 39: {
        drb_modified_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_modified_list_eutran, bref, 1, 32, true));
        break;
      }
      case 40: {
        drb_failed_to_modify_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_failed_to_modify_list_eutran, bref, 1, 32, true));
        break;
      }
      case 85: {
        retainability_meass_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(retainability_meass_info, bref, 1, 32, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void eutran_bearer_context_mod_resp_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_setup_mod_list_eutran_present) {
    j.write_int("id", 52);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drb_setup_mod_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_failed_mod_list_eutran_present) {
    j.write_int("id", 53);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drb_failed_mod_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_modified_list_eutran_present) {
    j.write_int("id", 39);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drb_modified_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_failed_to_modify_list_eutran_present) {
    j.write_int("id", 40);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drb_failed_to_modify_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (retainability_meass_info_present) {
    j.write_int("id", 85);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : retainability_meass_info) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_mod_resp_o>;

SRSASN_CODE ng_ran_bearer_context_mod_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += pdu_session_res_setup_mod_list_present ? 1 : 0;
  nof_ies += pdu_session_res_failed_mod_list_present ? 1 : 0;
  nof_ies += pdu_session_res_modified_list_present ? 1 : 0;
  nof_ies += pdu_session_res_failed_to_modify_list_present ? 1 : 0;
  nof_ies += retainability_meass_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (pdu_session_res_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)54, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_setup_mod_list, 1, 256, true));
  }
  if (pdu_session_res_failed_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)55, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_failed_mod_list, 1, 256, true));
  }
  if (pdu_session_res_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)48, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_modified_list, 1, 256, true));
  }
  if (pdu_session_res_failed_to_modify_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_failed_to_modify_list, 1, 256, true));
  }
  if (retainability_meass_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)85, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, retainability_meass_info, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_mod_resp_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 54: {
        pdu_session_res_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_setup_mod_list, bref, 1, 256, true));
        break;
      }
      case 55: {
        pdu_session_res_failed_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_failed_mod_list, bref, 1, 256, true));
        break;
      }
      case 48: {
        pdu_session_res_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_modified_list, bref, 1, 256, true));
        break;
      }
      case 49: {
        pdu_session_res_failed_to_modify_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_failed_to_modify_list, bref, 1, 256, true));
        break;
      }
      case 85: {
        retainability_meass_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(retainability_meass_info, bref, 1, 32, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void ng_ran_bearer_context_mod_resp_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdu_session_res_setup_mod_list_present) {
    j.write_int("id", 54);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdu_session_res_failed_mod_list_present) {
    j.write_int("id", 55);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_failed_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdu_session_res_modified_list_present) {
    j.write_int("id", 48);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pdu_session_res_failed_to_modify_list_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_failed_to_modify_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (retainability_meass_info_present) {
    j.write_int("id", 85);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : retainability_meass_info) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// System-BearerContextModificationResponse ::= CHOICE
void sys_bearer_context_mod_resp_c::destroy_()
{
  switch (type_) {
    case types::e_utran_bearer_context_mod_resp:
      c.destroy<eutran_bearer_context_mod_resp_container>();
      break;
    case types::ng_ran_bearer_context_mod_resp:
      c.destroy<ng_ran_bearer_context_mod_resp_container>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void sys_bearer_context_mod_resp_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e_utran_bearer_context_mod_resp:
      c.init<eutran_bearer_context_mod_resp_container>();
      break;
    case types::ng_ran_bearer_context_mod_resp:
      c.init<ng_ran_bearer_context_mod_resp_container>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_resp_c");
  }
}
sys_bearer_context_mod_resp_c::sys_bearer_context_mod_resp_c(const sys_bearer_context_mod_resp_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e_utran_bearer_context_mod_resp:
      c.init(other.c.get<eutran_bearer_context_mod_resp_container>());
      break;
    case types::ng_ran_bearer_context_mod_resp:
      c.init(other.c.get<ng_ran_bearer_context_mod_resp_container>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_resp_c");
  }
}
sys_bearer_context_mod_resp_c& sys_bearer_context_mod_resp_c::operator=(const sys_bearer_context_mod_resp_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e_utran_bearer_context_mod_resp:
      c.set(other.c.get<eutran_bearer_context_mod_resp_container>());
      break;
    case types::ng_ran_bearer_context_mod_resp:
      c.set(other.c.get<ng_ran_bearer_context_mod_resp_container>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_resp_c");
  }

  return *this;
}
eutran_bearer_context_mod_resp_container& sys_bearer_context_mod_resp_c::set_e_utran_bearer_context_mod_resp()
{
  set(types::e_utran_bearer_context_mod_resp);
  return c.get<eutran_bearer_context_mod_resp_container>();
}
ng_ran_bearer_context_mod_resp_container& sys_bearer_context_mod_resp_c::set_ng_ran_bearer_context_mod_resp()
{
  set(types::ng_ran_bearer_context_mod_resp);
  return c.get<ng_ran_bearer_context_mod_resp_container>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>& sys_bearer_context_mod_resp_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>();
}
void sys_bearer_context_mod_resp_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_utran_bearer_context_mod_resp:
      j.write_fieldname("e-UTRAN-BearerContextModificationResponse");
      c.get<eutran_bearer_context_mod_resp_container>().to_json(j);
      break;
    case types::ng_ran_bearer_context_mod_resp:
      j.write_fieldname("nG-RAN-BearerContextModificationResponse");
      c.get<ng_ran_bearer_context_mod_resp_container>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_resp_c");
  }
  j.end_obj();
}
SRSASN_CODE sys_bearer_context_mod_resp_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e_utran_bearer_context_mod_resp:
      HANDLE_CODE(c.get<eutran_bearer_context_mod_resp_container>().pack(bref));
      break;
    case types::ng_ran_bearer_context_mod_resp:
      HANDLE_CODE(c.get<ng_ran_bearer_context_mod_resp_container>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_resp_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_bearer_context_mod_resp_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e_utran_bearer_context_mod_resp:
      HANDLE_CODE(c.get<eutran_bearer_context_mod_resp_container>().unpack(bref));
      break;
    case types::ng_ran_bearer_context_mod_resp:
      HANDLE_CODE(c.get<ng_ran_bearer_context_mod_resp_container>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_mod_resp_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_bearer_context_mod_resp_c::types_opts::to_string() const
{
  static const char* names[] = {
      "e-UTRAN-BearerContextModificationResponse", "nG-RAN-BearerContextModificationResponse", "choice-extension"};
  return convert_enum_idx(names, 3, value, "sys_bearer_context_mod_resp_c::types");
}

// BearerContextModificationResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_mod_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 19};
  return map_enum_number(names, 3, idx, "id");
}
bool bearer_context_mod_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 19};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_mod_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 19:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_mod_resp_ies_o::value_c bearer_context_mod_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 19:
      ret.set(value_c::types::sys_bearer_context_mod_resp);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_mod_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 19:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_mod_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::sys_bearer_context_mod_resp:
      c = sys_bearer_context_mod_resp_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_resp_ies_o::value_c");
  }
}
uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_mod_resp_c& bearer_context_mod_resp_ies_o::value_c::sys_bearer_context_mod_resp()
{
  assert_choice_type(types::sys_bearer_context_mod_resp, type_, "Value");
  return c.get<sys_bearer_context_mod_resp_c>();
}
const uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_mod_resp_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const sys_bearer_context_mod_resp_c& bearer_context_mod_resp_ies_o::value_c::sys_bearer_context_mod_resp() const
{
  assert_choice_type(types::sys_bearer_context_mod_resp, type_, "Value");
  return c.get<sys_bearer_context_mod_resp_c>();
}
void bearer_context_mod_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::sys_bearer_context_mod_resp:
      j.write_fieldname("System-BearerContextModificationResponse");
      c.get<sys_bearer_context_mod_resp_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_mod_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_mod_resp:
      HANDLE_CODE(c.get<sys_bearer_context_mod_resp_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_mod_resp:
      HANDLE_CODE(c.get<sys_bearer_context_mod_resp_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_mod_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextModificationResponse"};
  return convert_enum_idx(names, 3, value, "bearer_context_mod_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_mod_resp_ies_o>;

SRSASN_CODE bearer_context_mod_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += sys_bearer_context_mod_resp_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (sys_bearer_context_mod_resp_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)19, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sys_bearer_context_mod_resp.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_mod_resp_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 19: {
        sys_bearer_context_mod_resp_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_mod_resp.unpack(bref));
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
void bearer_context_mod_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  if (sys_bearer_context_mod_resp_present) {
    j.write_int("id", 19);
    j.write_str("criticality", "ignore");
    sys_bearer_context_mod_resp.to_json(j);
  }
  j.end_obj();
}

// BearerContextReleaseCommandIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_release_cmd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 0};
  return map_enum_number(names, 3, idx, "id");
}
bool bearer_context_release_cmd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 0};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_release_cmd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_release_cmd_ies_o::value_c bearer_context_release_cmd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_release_cmd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_release_cmd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_cmd_ies_o::value_c");
  }
}
uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& bearer_context_release_cmd_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_release_cmd_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& bearer_context_release_cmd_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
void bearer_context_release_cmd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_cmd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_release_cmd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_cmd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_release_cmd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_cmd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(names, 3, value, "bearer_context_release_cmd_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_release_cmd_ies_o>;

SRSASN_CODE bearer_context_release_cmd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_release_cmd_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
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
void bearer_context_release_cmd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  j.end_obj();
}

// BearerContextReleaseCompleteIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_release_complete_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 1, 85};
  return map_enum_number(names, 4, idx, "id");
}
bool bearer_context_release_complete_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 1, 85};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_release_complete_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 85:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_release_complete_ies_o::value_c bearer_context_release_complete_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 85:
      ret.set(value_c::types::retainability_meass_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_release_complete_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    case 85:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_release_complete_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::retainability_meass_info:
      c = retainability_meass_info_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
  }
}
uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
crit_diagnostics_s& bearer_context_release_complete_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
retainability_meass_info_l& bearer_context_release_complete_ies_o::value_c::retainability_meass_info()
{
  assert_choice_type(types::retainability_meass_info, type_, "Value");
  return c.get<retainability_meass_info_l>();
}
const uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_release_complete_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const crit_diagnostics_s& bearer_context_release_complete_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const retainability_meass_info_l& bearer_context_release_complete_ies_o::value_c::retainability_meass_info() const
{
  assert_choice_type(types::retainability_meass_info, type_, "Value");
  return c.get<retainability_meass_info_l>();
}
void bearer_context_release_complete_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    case types::retainability_meass_info:
      j.start_array("RetainabilityMeasurementsInfo");
      for (const auto& e1 : c.get<retainability_meass_info_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_release_complete_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::retainability_meass_info:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<retainability_meass_info_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_release_complete_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::retainability_meass_info:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<retainability_meass_info_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_complete_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "CriticalityDiagnostics", "RetainabilityMeasurementsInfo"};
  return convert_enum_idx(names, 4, value, "bearer_context_release_complete_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_release_complete_ies_o>;

SRSASN_CODE bearer_context_release_complete_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += retainability_meass_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (retainability_meass_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)85, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, retainability_meass_info, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_release_complete_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 1: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 85: {
        retainability_meass_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(retainability_meass_info, bref, 1, 32, true));
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
void bearer_context_release_complete_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (retainability_meass_info_present) {
    j.write_int("id", 85);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : retainability_meass_info) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// BearerContextReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_release_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 22, 0};
  return map_enum_number(names, 4, idx, "id");
}
bool bearer_context_release_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 22, 0};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_release_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 22:
      return crit_e::ignore;
    case 0:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_release_request_ies_o::value_c bearer_context_release_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 22:
      ret.set(value_c::types::drb_status_list);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_release_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 22:
      return presence_e::optional;
    case 0:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_release_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::drb_status_list:
      c = drb_status_list_l{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_request_ies_o::value_c");
  }
}
uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
drb_status_list_l& bearer_context_release_request_ies_o::value_c::drb_status_list()
{
  assert_choice_type(types::drb_status_list, type_, "Value");
  return c.get<drb_status_list_l>();
}
cause_c& bearer_context_release_request_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_release_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const drb_status_list_l& bearer_context_release_request_ies_o::value_c::drb_status_list() const
{
  assert_choice_type(types::drb_status_list, type_, "Value");
  return c.get<drb_status_list_l>();
}
const cause_c& bearer_context_release_request_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
void bearer_context_release_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::drb_status_list:
      j.start_array("DRB-Status-List");
      for (const auto& e1 : c.get<drb_status_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_release_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drb_status_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_status_list_l>(), 1, 32, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_release_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drb_status_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_status_list_l>(), bref, 1, 32, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_release_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "DRB-Status-List", "Cause"};
  return convert_enum_idx(names, 4, value, "bearer_context_release_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_release_request_ies_o>;

SRSASN_CODE bearer_context_release_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += drb_status_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (drb_status_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)22, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_status_list, 1, 32, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_release_request_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 22: {
        drb_status_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_status_list, bref, 1, 32, true));
        break;
      }
      case 0: {
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
void bearer_context_release_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  if (drb_status_list_present) {
    j.write_int("id", 22);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drb_status_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  j.end_obj();
}

// BearerContextSetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 0, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool bearer_context_setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 0, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_setup_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::ignore;
    case 0:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_setup_fail_ies_o::value_c bearer_context_setup_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_setup_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::optional;
    case 0:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_setup_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
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
      log_invalid_choice_id(type_, "bearer_context_setup_fail_ies_o::value_c");
  }
}
uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& bearer_context_setup_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& bearer_context_setup_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_fail_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& bearer_context_setup_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& bearer_context_setup_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void bearer_context_setup_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
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
      log_invalid_choice_id(type_, "bearer_context_setup_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_setup_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_setup_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "bearer_context_setup_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_setup_fail_ies_o>;

SRSASN_CODE bearer_context_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += gnb_cu_up_ue_e1ap_id_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_setup_fail_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        gnb_cu_up_ue_e1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 1: {
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
void bearer_context_setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  if (gnb_cu_up_ue_e1ap_id_present) {
    j.write_int("id", 3);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  }
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

// EUTRAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_setup_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {32, 83, 84};
  return map_enum_number(names, 3, idx, "id");
}
bool eutran_bearer_context_setup_request_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {32, 83, 84};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e eutran_bearer_context_setup_request_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 32:
      return crit_e::reject;
    case 83:
      return crit_e::ignore;
    case 84:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
eutran_bearer_context_setup_request_o::value_c eutran_bearer_context_setup_request_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 32:
      ret.set(value_c::types::drb_to_setup_list_eutran);
      break;
    case 83:
      ret.set(value_c::types::sub_profile_id_for_rfp);
      break;
    case 84:
      ret.set(value_c::types::add_rrm_prio_idx);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_bearer_context_setup_request_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 32:
      return presence_e::mandatory;
    case 83:
      return presence_e::optional;
    case 84:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_setup_request_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::drb_to_setup_list_eutran:
      c = drb_to_setup_list_eutran_l{};
      break;
    case types::sub_profile_id_for_rfp:
      c = uint16_t{};
      break;
    case types::add_rrm_prio_idx:
      c = fixed_bitstring<32, false, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_request_o::value_c");
  }
}
drb_to_setup_list_eutran_l& eutran_bearer_context_setup_request_o::value_c::drb_to_setup_list_eutran()
{
  assert_choice_type(types::drb_to_setup_list_eutran, type_, "Value");
  return c.get<drb_to_setup_list_eutran_l>();
}
uint16_t& eutran_bearer_context_setup_request_o::value_c::sub_profile_id_for_rfp()
{
  assert_choice_type(types::sub_profile_id_for_rfp, type_, "Value");
  return c.get<uint16_t>();
}
fixed_bitstring<32, false, true>& eutran_bearer_context_setup_request_o::value_c::add_rrm_prio_idx()
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
const drb_to_setup_list_eutran_l& eutran_bearer_context_setup_request_o::value_c::drb_to_setup_list_eutran() const
{
  assert_choice_type(types::drb_to_setup_list_eutran, type_, "Value");
  return c.get<drb_to_setup_list_eutran_l>();
}
const uint16_t& eutran_bearer_context_setup_request_o::value_c::sub_profile_id_for_rfp() const
{
  assert_choice_type(types::sub_profile_id_for_rfp, type_, "Value");
  return c.get<uint16_t>();
}
const fixed_bitstring<32, false, true>& eutran_bearer_context_setup_request_o::value_c::add_rrm_prio_idx() const
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
void eutran_bearer_context_setup_request_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_to_setup_list_eutran:
      j.start_array("DRB-To-Setup-List-EUTRAN");
      for (const auto& e1 : c.get<drb_to_setup_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sub_profile_id_for_rfp:
      j.write_int("INTEGER (1..256,...)", c.get<uint16_t>());
      break;
    case types::add_rrm_prio_idx:
      j.write_str("BIT STRING", c.get<fixed_bitstring<32, false, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_request_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE eutran_bearer_context_setup_request_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_to_setup_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_to_setup_list_eutran_l>(), 1, 32, true));
      break;
    case types::sub_profile_id_for_rfp:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)256u, true, true));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_request_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_setup_request_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_to_setup_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_to_setup_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::sub_profile_id_for_rfp:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)256u, true, true));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_request_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_setup_request_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-To-Setup-List-EUTRAN", "INTEGER (1..256,...)", "BIT STRING"};
  return convert_enum_idx(names, 3, value, "eutran_bearer_context_setup_request_o::value_c::types");
}
uint8_t eutran_bearer_context_setup_request_o::value_c::types_opts::to_number() const
{
  if (value == sub_profile_id_for_rfp) {
    return 1;
  }
  invalid_enum_number(value, "eutran_bearer_context_setup_request_o::value_c::types");
  return 0;
}

// NG-RAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_setup_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {42};
  return map_enum_number(names, 1, idx, "id");
}
bool ng_ran_bearer_context_setup_request_o::is_id_valid(const uint32_t& id)
{
  return 42 == id;
}
crit_e ng_ran_bearer_context_setup_request_o::get_crit(const uint32_t& id)
{
  if (id == 42) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ng_ran_bearer_context_setup_request_o::value_c ng_ran_bearer_context_setup_request_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 42) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ng_ran_bearer_context_setup_request_o::get_presence(const uint32_t& id)
{
  if (id == 42) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_bearer_context_setup_request_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("PDU-Session-Resource-To-Setup-List");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE ng_ran_bearer_context_setup_request_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 256, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_setup_request_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 256, true));
  return SRSASN_SUCCESS;
}

const char* ng_ran_bearer_context_setup_request_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDU-Session-Resource-To-Setup-List"};
  return convert_enum_idx(names, 1, value, "ng_ran_bearer_context_setup_request_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_setup_request_o>;

SRSASN_CODE eutran_bearer_context_setup_request_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += sub_profile_id_for_rfp_present ? 1 : 0;
  nof_ies += add_rrm_prio_idx_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)32, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_setup_list_eutran, 1, 32, true));
  }
  if (sub_profile_id_for_rfp_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)83, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, sub_profile_id_for_rfp, (uint16_t)1u, (uint16_t)256u, true, true));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)84, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_setup_request_container::unpack(cbit_ref& bref)
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
      case 32: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_to_setup_list_eutran, bref, 1, 32, true));
        break;
      }
      case 83: {
        sub_profile_id_for_rfp_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(sub_profile_id_for_rfp, bref, (uint16_t)1u, (uint16_t)256u, true, true));
        break;
      }
      case 84: {
        add_rrm_prio_idx_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.unpack(bref));
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
void eutran_bearer_context_setup_request_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 32);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : drb_to_setup_list_eutran) {
    e1.to_json(j);
  }
  j.end_array();
  if (sub_profile_id_for_rfp_present) {
    j.write_int("id", 83);
    j.write_str("criticality", "ignore");
    j.write_int("Value", sub_profile_id_for_rfp);
  }
  if (add_rrm_prio_idx_present) {
    j.write_int("id", 84);
    j.write_str("criticality", "ignore");
    j.write_str("Value", add_rrm_prio_idx.to_string());
  }
  j.end_obj();
}

// System-BearerContextSetupRequest ::= CHOICE
void sys_bearer_context_setup_request_c::destroy_()
{
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      c.destroy<eutran_bearer_context_setup_request_container>();
      break;
    case types::ng_ran_bearer_context_setup_request:
      c.destroy<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void sys_bearer_context_setup_request_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      c.init<eutran_bearer_context_setup_request_container>();
      break;
    case types::ng_ran_bearer_context_setup_request:
      c.init<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_request_c");
  }
}
sys_bearer_context_setup_request_c::sys_bearer_context_setup_request_c(const sys_bearer_context_setup_request_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      c.init(other.c.get<eutran_bearer_context_setup_request_container>());
      break;
    case types::ng_ran_bearer_context_setup_request:
      c.init(other.c.get<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_request_c");
  }
}
sys_bearer_context_setup_request_c&
sys_bearer_context_setup_request_c::operator=(const sys_bearer_context_setup_request_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      c.set(other.c.get<eutran_bearer_context_setup_request_container>());
      break;
    case types::ng_ran_bearer_context_setup_request:
      c.set(other.c.get<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_request_c");
  }

  return *this;
}
eutran_bearer_context_setup_request_container&
sys_bearer_context_setup_request_c::set_e_utran_bearer_context_setup_request()
{
  set(types::e_utran_bearer_context_setup_request);
  return c.get<eutran_bearer_context_setup_request_container>();
}
protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>&
sys_bearer_context_setup_request_c::set_ng_ran_bearer_context_setup_request()
{
  set(types::ng_ran_bearer_context_setup_request);
  return c.get<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>();
}
protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>&
sys_bearer_context_setup_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>();
}
void sys_bearer_context_setup_request_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      j.write_fieldname("e-UTRAN-BearerContextSetupRequest");
      c.get<eutran_bearer_context_setup_request_container>().to_json(j);
      break;
    case types::ng_ran_bearer_context_setup_request:
      j.write_fieldname("nG-RAN-BearerContextSetupRequest");
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_request_c");
  }
  j.end_obj();
}
SRSASN_CODE sys_bearer_context_setup_request_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      HANDLE_CODE(c.get<eutran_bearer_context_setup_request_container>().pack(bref));
      break;
    case types::ng_ran_bearer_context_setup_request:
      HANDLE_CODE(pack_dyn_seq_of(
          bref, c.get<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>(), 0, 65535, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_request_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_bearer_context_setup_request_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e_utran_bearer_context_setup_request:
      HANDLE_CODE(c.get<eutran_bearer_context_setup_request_container>().unpack(bref));
      break;
    case types::ng_ran_bearer_context_setup_request:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<protocol_ie_container_l<ng_ran_bearer_context_setup_request_o>>(), bref, 0, 65535, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_request_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_bearer_context_setup_request_c::types_opts::to_string() const
{
  static const char* names[] = {
      "e-UTRAN-BearerContextSetupRequest", "nG-RAN-BearerContextSetupRequest", "choice-extension"};
  return convert_enum_idx(names, 3, value, "sys_bearer_context_setup_request_c::types");
}

// BearerContextSetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 13, 14, 66, 58, 23, 59, 17, 15, 76, 77, 81, 111, 113, 121, 134, 139, 3};
  return map_enum_number(names, 18, idx, "id");
}
bool bearer_context_setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 13, 14, 66, 58, 23, 59, 17, 15, 76, 77, 81, 111, 113, 121, 134, 139, 3};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_setup_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 66:
      return crit_e::reject;
    case 58:
      return crit_e::ignore;
    case 23:
      return crit_e::reject;
    case 59:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 76:
      return crit_e::ignore;
    case 77:
      return crit_e::ignore;
    case 81:
      return crit_e::ignore;
    case 111:
      return crit_e::reject;
    case 113:
      return crit_e::ignore;
    case 121:
      return crit_e::reject;
    case 134:
      return crit_e::ignore;
    case 139:
      return crit_e::ignore;
    case 3:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_setup_request_ies_o::value_c bearer_context_setup_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 13:
      ret.set(value_c::types::security_info);
      break;
    case 14:
      ret.set(value_c::types::ue_dl_aggr_max_bit_rate);
      break;
    case 66:
      ret.set(value_c::types::ue_dl_max_integrity_protected_data_rate);
      break;
    case 58:
      ret.set(value_c::types::serving_plmn);
      break;
    case 23:
      ret.set(value_c::types::activity_notif_level);
      break;
    case 59:
      ret.set(value_c::types::ue_inactivity_timer);
      break;
    case 17:
      ret.set(value_c::types::bearer_context_status_change);
      break;
    case 15:
      ret.set(value_c::types::sys_bearer_context_setup_request);
      break;
    case 76:
      ret.set(value_c::types::ran_ue_id);
      break;
    case 77:
      ret.set(value_c::types::gnb_du_id);
      break;
    case 81:
      ret.set(value_c::types::trace_activation);
      break;
    case 111:
      ret.set(value_c::types::npn_context_info);
      break;
    case 113:
      ret.set(value_c::types::management_based_mdt_plmn_list);
      break;
    case 121:
      ret.set(value_c::types::cho_initiation);
      break;
    case 134:
      ret.set(value_c::types::add_ho_info);
      break;
    case 139:
      ret.set(value_c::types::direct_forwarding_path_availability);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_setup_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 13:
      return presence_e::mandatory;
    case 14:
      return presence_e::mandatory;
    case 66:
      return presence_e::optional;
    case 58:
      return presence_e::mandatory;
    case 23:
      return presence_e::mandatory;
    case 59:
      return presence_e::optional;
    case 17:
      return presence_e::optional;
    case 15:
      return presence_e::mandatory;
    case 76:
      return presence_e::optional;
    case 77:
      return presence_e::optional;
    case 81:
      return presence_e::optional;
    case 111:
      return presence_e::optional;
    case 113:
      return presence_e::optional;
    case 121:
      return presence_e::optional;
    case 134:
      return presence_e::optional;
    case 139:
      return presence_e::optional;
    case 3:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_setup_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::security_info:
      c = security_info_s{};
      break;
    case types::ue_dl_aggr_max_bit_rate:
      c = uint64_t{};
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      c = uint64_t{};
      break;
    case types::serving_plmn:
      c = fixed_octstring<3, true>{};
      break;
    case types::activity_notif_level:
      c = activity_notif_level_e{};
      break;
    case types::ue_inactivity_timer:
      c = uint16_t{};
      break;
    case types::bearer_context_status_change:
      c = bearer_context_status_change_e{};
      break;
    case types::sys_bearer_context_setup_request:
      c = sys_bearer_context_setup_request_c{};
      break;
    case types::ran_ue_id:
      c = fixed_octstring<8, true>{};
      break;
    case types::gnb_du_id:
      c = uint64_t{};
      break;
    case types::trace_activation:
      c = trace_activation_s{};
      break;
    case types::npn_context_info:
      c = npn_context_info_c{};
      break;
    case types::management_based_mdt_plmn_list:
      c = mdt_plmn_list_l{};
      break;
    case types::cho_initiation:
      c = cho_initiation_e{};
      break;
    case types::add_ho_info:
      c = add_ho_info_e{};
      break;
    case types::direct_forwarding_path_availability:
      c = direct_forwarding_path_availability_e{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
  }
}
uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
security_info_s& bearer_context_setup_request_ies_o::value_c::security_info()
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
uint64_t& bearer_context_setup_request_ies_o::value_c::ue_dl_aggr_max_bit_rate()
{
  assert_choice_type(types::ue_dl_aggr_max_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_setup_request_ies_o::value_c::ue_dl_max_integrity_protected_data_rate()
{
  assert_choice_type(types::ue_dl_max_integrity_protected_data_rate, type_, "Value");
  return c.get<uint64_t>();
}
fixed_octstring<3, true>& bearer_context_setup_request_ies_o::value_c::serving_plmn()
{
  assert_choice_type(types::serving_plmn, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
activity_notif_level_e& bearer_context_setup_request_ies_o::value_c::activity_notif_level()
{
  assert_choice_type(types::activity_notif_level, type_, "Value");
  return c.get<activity_notif_level_e>();
}
uint16_t& bearer_context_setup_request_ies_o::value_c::ue_inactivity_timer()
{
  assert_choice_type(types::ue_inactivity_timer, type_, "Value");
  return c.get<uint16_t>();
}
bearer_context_status_change_e& bearer_context_setup_request_ies_o::value_c::bearer_context_status_change()
{
  assert_choice_type(types::bearer_context_status_change, type_, "Value");
  return c.get<bearer_context_status_change_e>();
}
sys_bearer_context_setup_request_c& bearer_context_setup_request_ies_o::value_c::sys_bearer_context_setup_request()
{
  assert_choice_type(types::sys_bearer_context_setup_request, type_, "Value");
  return c.get<sys_bearer_context_setup_request_c>();
}
fixed_octstring<8, true>& bearer_context_setup_request_ies_o::value_c::ran_ue_id()
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_du_id()
{
  assert_choice_type(types::gnb_du_id, type_, "Value");
  return c.get<uint64_t>();
}
trace_activation_s& bearer_context_setup_request_ies_o::value_c::trace_activation()
{
  assert_choice_type(types::trace_activation, type_, "Value");
  return c.get<trace_activation_s>();
}
npn_context_info_c& bearer_context_setup_request_ies_o::value_c::npn_context_info()
{
  assert_choice_type(types::npn_context_info, type_, "Value");
  return c.get<npn_context_info_c>();
}
mdt_plmn_list_l& bearer_context_setup_request_ies_o::value_c::management_based_mdt_plmn_list()
{
  assert_choice_type(types::management_based_mdt_plmn_list, type_, "Value");
  return c.get<mdt_plmn_list_l>();
}
cho_initiation_e& bearer_context_setup_request_ies_o::value_c::cho_initiation()
{
  assert_choice_type(types::cho_initiation, type_, "Value");
  return c.get<cho_initiation_e>();
}
add_ho_info_e& bearer_context_setup_request_ies_o::value_c::add_ho_info()
{
  assert_choice_type(types::add_ho_info, type_, "Value");
  return c.get<add_ho_info_e>();
}
direct_forwarding_path_availability_e&
bearer_context_setup_request_ies_o::value_c::direct_forwarding_path_availability()
{
  assert_choice_type(types::direct_forwarding_path_availability, type_, "Value");
  return c.get<direct_forwarding_path_availability_e>();
}
uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const security_info_s& bearer_context_setup_request_ies_o::value_c::security_info() const
{
  assert_choice_type(types::security_info, type_, "Value");
  return c.get<security_info_s>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::ue_dl_aggr_max_bit_rate() const
{
  assert_choice_type(types::ue_dl_aggr_max_bit_rate, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::ue_dl_max_integrity_protected_data_rate() const
{
  assert_choice_type(types::ue_dl_max_integrity_protected_data_rate, type_, "Value");
  return c.get<uint64_t>();
}
const fixed_octstring<3, true>& bearer_context_setup_request_ies_o::value_c::serving_plmn() const
{
  assert_choice_type(types::serving_plmn, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
const activity_notif_level_e& bearer_context_setup_request_ies_o::value_c::activity_notif_level() const
{
  assert_choice_type(types::activity_notif_level, type_, "Value");
  return c.get<activity_notif_level_e>();
}
const uint16_t& bearer_context_setup_request_ies_o::value_c::ue_inactivity_timer() const
{
  assert_choice_type(types::ue_inactivity_timer, type_, "Value");
  return c.get<uint16_t>();
}
const bearer_context_status_change_e& bearer_context_setup_request_ies_o::value_c::bearer_context_status_change() const
{
  assert_choice_type(types::bearer_context_status_change, type_, "Value");
  return c.get<bearer_context_status_change_e>();
}
const sys_bearer_context_setup_request_c&
bearer_context_setup_request_ies_o::value_c::sys_bearer_context_setup_request() const
{
  assert_choice_type(types::sys_bearer_context_setup_request, type_, "Value");
  return c.get<sys_bearer_context_setup_request_c>();
}
const fixed_octstring<8, true>& bearer_context_setup_request_ies_o::value_c::ran_ue_id() const
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_du_id() const
{
  assert_choice_type(types::gnb_du_id, type_, "Value");
  return c.get<uint64_t>();
}
const trace_activation_s& bearer_context_setup_request_ies_o::value_c::trace_activation() const
{
  assert_choice_type(types::trace_activation, type_, "Value");
  return c.get<trace_activation_s>();
}
const npn_context_info_c& bearer_context_setup_request_ies_o::value_c::npn_context_info() const
{
  assert_choice_type(types::npn_context_info, type_, "Value");
  return c.get<npn_context_info_c>();
}
const mdt_plmn_list_l& bearer_context_setup_request_ies_o::value_c::management_based_mdt_plmn_list() const
{
  assert_choice_type(types::management_based_mdt_plmn_list, type_, "Value");
  return c.get<mdt_plmn_list_l>();
}
const cho_initiation_e& bearer_context_setup_request_ies_o::value_c::cho_initiation() const
{
  assert_choice_type(types::cho_initiation, type_, "Value");
  return c.get<cho_initiation_e>();
}
const add_ho_info_e& bearer_context_setup_request_ies_o::value_c::add_ho_info() const
{
  assert_choice_type(types::add_ho_info, type_, "Value");
  return c.get<add_ho_info_e>();
}
const direct_forwarding_path_availability_e&
bearer_context_setup_request_ies_o::value_c::direct_forwarding_path_availability() const
{
  assert_choice_type(types::direct_forwarding_path_availability, type_, "Value");
  return c.get<direct_forwarding_path_availability_e>();
}
const uint64_t& bearer_context_setup_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
void bearer_context_setup_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::security_info:
      j.write_fieldname("SecurityInformation");
      c.get<security_info_s>().to_json(j);
      break;
    case types::ue_dl_aggr_max_bit_rate:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::serving_plmn:
      j.write_str("OCTET STRING", c.get<fixed_octstring<3, true>>().to_string());
      break;
    case types::activity_notif_level:
      j.write_str("ActivityNotificationLevel", c.get<activity_notif_level_e>().to_string());
      break;
    case types::ue_inactivity_timer:
      j.write_int("INTEGER (1..7200,...)", c.get<uint16_t>());
      break;
    case types::bearer_context_status_change:
      j.write_str("BearerContextStatusChange", c.get<bearer_context_status_change_e>().to_string());
      break;
    case types::sys_bearer_context_setup_request:
      j.write_fieldname("System-BearerContextSetupRequest");
      c.get<sys_bearer_context_setup_request_c>().to_json(j);
      break;
    case types::ran_ue_id:
      j.write_str("OCTET STRING", c.get<fixed_octstring<8, true>>().to_string());
      break;
    case types::gnb_du_id:
      j.write_int("INTEGER (0..68719476735)", c.get<uint64_t>());
      break;
    case types::trace_activation:
      j.write_fieldname("TraceActivation");
      c.get<trace_activation_s>().to_json(j);
      break;
    case types::npn_context_info:
      j.write_fieldname("NPNContextInfo");
      c.get<npn_context_info_c>().to_json(j);
      break;
    case types::management_based_mdt_plmn_list:
      j.start_array("MDTPLMNList");
      for (const auto& e1 : c.get<mdt_plmn_list_l>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    case types::cho_initiation:
      j.write_str("CHOInitiation", "true");
      break;
    case types::add_ho_info:
      j.write_str("AdditionalHandoverInfo", "discard-pdpc-SN");
      break;
    case types::direct_forwarding_path_availability:
      j.write_str("DirectForwardingPathAvailability", c.get<direct_forwarding_path_availability_e>().to_string());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_setup_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().pack(bref));
      break;
    case types::ue_dl_aggr_max_bit_rate:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::serving_plmn:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().pack(bref)));
      break;
    case types::activity_notif_level:
      HANDLE_CODE(c.get<activity_notif_level_e>().pack(bref));
      break;
    case types::ue_inactivity_timer:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)7200u, true, true));
      break;
    case types::bearer_context_status_change:
      HANDLE_CODE(c.get<bearer_context_status_change_e>().pack(bref));
      break;
    case types::sys_bearer_context_setup_request:
      HANDLE_CODE(c.get<sys_bearer_context_setup_request_c>().pack(bref));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().pack(bref)));
      break;
    case types::gnb_du_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::trace_activation:
      HANDLE_CODE(c.get<trace_activation_s>().pack(bref));
      break;
    case types::npn_context_info:
      HANDLE_CODE(c.get<npn_context_info_c>().pack(bref));
      break;
    case types::management_based_mdt_plmn_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<mdt_plmn_list_l>(), 1, 16, true));
      break;
    case types::cho_initiation:
      HANDLE_CODE(c.get<cho_initiation_e>().pack(bref));
      break;
    case types::add_ho_info:
      HANDLE_CODE(c.get<add_ho_info_e>().pack(bref));
      break;
    case types::direct_forwarding_path_availability:
      HANDLE_CODE(c.get<direct_forwarding_path_availability_e>().pack(bref));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_setup_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::security_info:
      HANDLE_CODE(c.get<security_info_s>().unpack(bref));
      break;
    case types::ue_dl_aggr_max_bit_rate:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::ue_dl_max_integrity_protected_data_rate:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::serving_plmn:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().unpack(bref)));
      break;
    case types::activity_notif_level:
      HANDLE_CODE(c.get<activity_notif_level_e>().unpack(bref));
      break;
    case types::ue_inactivity_timer:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)7200u, true, true));
      break;
    case types::bearer_context_status_change:
      HANDLE_CODE(c.get<bearer_context_status_change_e>().unpack(bref));
      break;
    case types::sys_bearer_context_setup_request:
      HANDLE_CODE(c.get<sys_bearer_context_setup_request_c>().unpack(bref));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().unpack(bref)));
      break;
    case types::gnb_du_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::trace_activation:
      HANDLE_CODE(c.get<trace_activation_s>().unpack(bref));
      break;
    case types::npn_context_info:
      HANDLE_CODE(c.get<npn_context_info_c>().unpack(bref));
      break;
    case types::management_based_mdt_plmn_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<mdt_plmn_list_l>(), bref, 1, 16, true));
      break;
    case types::cho_initiation:
      HANDLE_CODE(c.get<cho_initiation_e>().unpack(bref));
      break;
    case types::add_ho_info:
      HANDLE_CODE(c.get<add_ho_info_e>().unpack(bref));
      break;
    case types::direct_forwarding_path_availability:
      HANDLE_CODE(c.get<direct_forwarding_path_availability_e>().unpack(bref));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "SecurityInformation",
                                "INTEGER (0..4000000000000,...)",
                                "INTEGER (0..4000000000000,...)",
                                "OCTET STRING",
                                "ActivityNotificationLevel",
                                "INTEGER (1..7200,...)",
                                "BearerContextStatusChange",
                                "System-BearerContextSetupRequest",
                                "OCTET STRING",
                                "INTEGER (0..68719476735)",
                                "TraceActivation",
                                "NPNContextInfo",
                                "MDTPLMNList",
                                "CHOInitiation",
                                "AdditionalHandoverInfo",
                                "DirectForwardingPathAvailability",
                                "INTEGER (0..4294967295)"};
  return convert_enum_idx(names, 18, value, "bearer_context_setup_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_setup_request_ies_o>;

SRSASN_CODE bearer_context_setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 6;
  nof_ies += ue_dl_max_integrity_protected_data_rate_present ? 1 : 0;
  nof_ies += ue_inactivity_timer_present ? 1 : 0;
  nof_ies += bearer_context_status_change_present ? 1 : 0;
  nof_ies += ran_ue_id_present ? 1 : 0;
  nof_ies += gnb_du_id_present ? 1 : 0;
  nof_ies += trace_activation_present ? 1 : 0;
  nof_ies += npn_context_info_present ? 1 : 0;
  nof_ies += management_based_mdt_plmn_list_present ? 1 : 0;
  nof_ies += cho_initiation_present ? 1 : 0;
  nof_ies += add_ho_info_present ? 1 : 0;
  nof_ies += direct_forwarding_path_availability_present ? 1 : 0;
  nof_ies += gnb_cu_up_ue_e1ap_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)13, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(security_info.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)14, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ue_dl_aggr_max_bit_rate, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (ue_dl_max_integrity_protected_data_rate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)66, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(
        bref, ue_dl_max_integrity_protected_data_rate, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)58, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(serving_plmn.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)23, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(activity_notif_level.pack(bref));
  }
  if (ue_inactivity_timer_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)59, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ue_inactivity_timer, (uint16_t)1u, (uint16_t)7200u, true, true));
  }
  if (bearer_context_status_change_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)17, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(bearer_context_status_change.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)15, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sys_bearer_context_setup_request.pack(bref));
  }
  if (ran_ue_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)76, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ran_ue_id.pack(bref));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)77, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (trace_activation_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)81, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(trace_activation.pack(bref));
  }
  if (npn_context_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)111, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(npn_context_info.pack(bref));
  }
  if (management_based_mdt_plmn_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)113, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, management_based_mdt_plmn_list, 1, 16, true));
  }
  if (cho_initiation_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)121, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cho_initiation.pack(bref));
  }
  if (add_ho_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)134, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(add_ho_info.pack(bref));
  }
  if (direct_forwarding_path_availability_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)139, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(direct_forwarding_path_availability.pack(bref));
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_setup_request_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 13: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(security_info.unpack(bref));
        break;
      }
      case 14: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ue_dl_aggr_max_bit_rate, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 66: {
        ue_dl_max_integrity_protected_data_rate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(
            ue_dl_max_integrity_protected_data_rate, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 58: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_plmn.unpack(bref));
        break;
      }
      case 23: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(activity_notif_level.unpack(bref));
        break;
      }
      case 59: {
        ue_inactivity_timer_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ue_inactivity_timer, bref, (uint16_t)1u, (uint16_t)7200u, true, true));
        break;
      }
      case 17: {
        bearer_context_status_change_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bearer_context_status_change.unpack(bref));
        break;
      }
      case 15: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_setup_request.unpack(bref));
        break;
      }
      case 76: {
        ran_ue_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ran_ue_id.unpack(bref));
        break;
      }
      case 77: {
        gnb_du_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
        break;
      }
      case 81: {
        trace_activation_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_activation.unpack(bref));
        break;
      }
      case 111: {
        npn_context_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(npn_context_info.unpack(bref));
        break;
      }
      case 113: {
        management_based_mdt_plmn_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(management_based_mdt_plmn_list, bref, 1, 16, true));
        break;
      }
      case 121: {
        cho_initiation_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cho_initiation.unpack(bref));
        break;
      }
      case 134: {
        add_ho_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_ho_info.unpack(bref));
        break;
      }
      case 139: {
        direct_forwarding_path_availability_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(direct_forwarding_path_availability.unpack(bref));
        break;
      }
      case 3: {
        gnb_cu_up_ue_e1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
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
void bearer_context_setup_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 13);
  j.write_str("criticality", "reject");
  security_info.to_json(j);
  j.write_int("id", 14);
  j.write_str("criticality", "reject");
  j.write_int("Value", ue_dl_aggr_max_bit_rate);
  if (ue_dl_max_integrity_protected_data_rate_present) {
    j.write_int("id", 66);
    j.write_str("criticality", "reject");
    j.write_int("Value", ue_dl_max_integrity_protected_data_rate);
  }
  j.write_int("id", 58);
  j.write_str("criticality", "ignore");
  j.write_str("Value", serving_plmn.to_string());
  j.write_int("id", 23);
  j.write_str("criticality", "reject");
  j.write_str("Value", activity_notif_level.to_string());
  if (ue_inactivity_timer_present) {
    j.write_int("id", 59);
    j.write_str("criticality", "reject");
    j.write_int("Value", ue_inactivity_timer);
  }
  if (bearer_context_status_change_present) {
    j.write_int("id", 17);
    j.write_str("criticality", "reject");
    j.write_str("Value", bearer_context_status_change.to_string());
  }
  j.write_int("id", 15);
  j.write_str("criticality", "reject");
  sys_bearer_context_setup_request.to_json(j);
  if (ran_ue_id_present) {
    j.write_int("id", 76);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ran_ue_id.to_string());
  }
  if (gnb_du_id_present) {
    j.write_int("id", 77);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_du_id);
  }
  if (trace_activation_present) {
    j.write_int("id", 81);
    j.write_str("criticality", "ignore");
    trace_activation.to_json(j);
  }
  if (npn_context_info_present) {
    j.write_int("id", 111);
    j.write_str("criticality", "reject");
    npn_context_info.to_json(j);
  }
  if (management_based_mdt_plmn_list_present) {
    j.write_int("id", 113);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : management_based_mdt_plmn_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (cho_initiation_present) {
    j.write_int("id", 121);
    j.write_str("criticality", "reject");
    j.write_str("Value", "true");
  }
  if (add_ho_info_present) {
    j.write_int("id", 134);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "discard-pdpc-SN");
  }
  if (direct_forwarding_path_availability_present) {
    j.write_int("id", 139);
    j.write_str("criticality", "ignore");
    j.write_str("Value", direct_forwarding_path_availability.to_string());
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    j.write_int("id", 3);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  }
  j.end_obj();
}

// EUTRAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_bearer_context_setup_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {37, 38};
  return map_enum_number(names, 2, idx, "id");
}
bool eutran_bearer_context_setup_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {37, 38};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e eutran_bearer_context_setup_resp_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 37:
      return crit_e::ignore;
    case 38:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
eutran_bearer_context_setup_resp_o::value_c eutran_bearer_context_setup_resp_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 37:
      ret.set(value_c::types::drb_setup_list_eutran);
      break;
    case 38:
      ret.set(value_c::types::drb_failed_list_eutran);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_bearer_context_setup_resp_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 37:
      return presence_e::mandatory;
    case 38:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void eutran_bearer_context_setup_resp_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::drb_setup_list_eutran:
      c = drb_setup_list_eutran_l{};
      break;
    case types::drb_failed_list_eutran:
      c = drb_failed_list_eutran_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_resp_o::value_c");
  }
}
drb_setup_list_eutran_l& eutran_bearer_context_setup_resp_o::value_c::drb_setup_list_eutran()
{
  assert_choice_type(types::drb_setup_list_eutran, type_, "Value");
  return c.get<drb_setup_list_eutran_l>();
}
drb_failed_list_eutran_l& eutran_bearer_context_setup_resp_o::value_c::drb_failed_list_eutran()
{
  assert_choice_type(types::drb_failed_list_eutran, type_, "Value");
  return c.get<drb_failed_list_eutran_l>();
}
const drb_setup_list_eutran_l& eutran_bearer_context_setup_resp_o::value_c::drb_setup_list_eutran() const
{
  assert_choice_type(types::drb_setup_list_eutran, type_, "Value");
  return c.get<drb_setup_list_eutran_l>();
}
const drb_failed_list_eutran_l& eutran_bearer_context_setup_resp_o::value_c::drb_failed_list_eutran() const
{
  assert_choice_type(types::drb_failed_list_eutran, type_, "Value");
  return c.get<drb_failed_list_eutran_l>();
}
void eutran_bearer_context_setup_resp_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::drb_setup_list_eutran:
      j.start_array("DRB-Setup-List-EUTRAN");
      for (const auto& e1 : c.get<drb_setup_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drb_failed_list_eutran:
      j.start_array("DRB-Failed-List-EUTRAN");
      for (const auto& e1 : c.get<drb_failed_list_eutran_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_resp_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE eutran_bearer_context_setup_resp_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_setup_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_setup_list_eutran_l>(), 1, 32, true));
      break;
    case types::drb_failed_list_eutran:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_failed_list_eutran_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_resp_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_setup_resp_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::drb_setup_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_setup_list_eutran_l>(), bref, 1, 32, true));
      break;
    case types::drb_failed_list_eutran:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_failed_list_eutran_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "eutran_bearer_context_setup_resp_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* eutran_bearer_context_setup_resp_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-Setup-List-EUTRAN", "DRB-Failed-List-EUTRAN"};
  return convert_enum_idx(names, 2, value, "eutran_bearer_context_setup_resp_o::value_c::types");
}

// NG-RAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_bearer_context_setup_resp_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {46, 47};
  return map_enum_number(names, 2, idx, "id");
}
bool ng_ran_bearer_context_setup_resp_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {46, 47};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ng_ran_bearer_context_setup_resp_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 46:
      return crit_e::ignore;
    case 47:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ng_ran_bearer_context_setup_resp_o::value_c ng_ran_bearer_context_setup_resp_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 46:
      ret.set(value_c::types::pdu_session_res_setup_list);
      break;
    case 47:
      ret.set(value_c::types::pdu_session_res_failed_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ng_ran_bearer_context_setup_resp_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 46:
      return presence_e::mandatory;
    case 47:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_bearer_context_setup_resp_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::pdu_session_res_setup_list:
      c = pdu_session_res_setup_list_l{};
      break;
    case types::pdu_session_res_failed_list:
      c = pdu_session_res_failed_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_setup_resp_o::value_c");
  }
}
pdu_session_res_setup_list_l& ng_ran_bearer_context_setup_resp_o::value_c::pdu_session_res_setup_list()
{
  assert_choice_type(types::pdu_session_res_setup_list, type_, "Value");
  return c.get<pdu_session_res_setup_list_l>();
}
pdu_session_res_failed_list_l& ng_ran_bearer_context_setup_resp_o::value_c::pdu_session_res_failed_list()
{
  assert_choice_type(types::pdu_session_res_failed_list, type_, "Value");
  return c.get<pdu_session_res_failed_list_l>();
}
const pdu_session_res_setup_list_l& ng_ran_bearer_context_setup_resp_o::value_c::pdu_session_res_setup_list() const
{
  assert_choice_type(types::pdu_session_res_setup_list, type_, "Value");
  return c.get<pdu_session_res_setup_list_l>();
}
const pdu_session_res_failed_list_l& ng_ran_bearer_context_setup_resp_o::value_c::pdu_session_res_failed_list() const
{
  assert_choice_type(types::pdu_session_res_failed_list, type_, "Value");
  return c.get<pdu_session_res_failed_list_l>();
}
void ng_ran_bearer_context_setup_resp_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pdu_session_res_setup_list:
      j.start_array("PDU-Session-Resource-Setup-List");
      for (const auto& e1 : c.get<pdu_session_res_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pdu_session_res_failed_list:
      j.start_array("PDU-Session-Resource-Failed-List");
      for (const auto& e1 : c.get<pdu_session_res_failed_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_setup_resp_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ng_ran_bearer_context_setup_resp_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_setup_list_l>(), 1, 256, true));
      break;
    case types::pdu_session_res_failed_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_failed_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_setup_resp_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_setup_resp_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::pdu_session_res_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_setup_list_l>(), bref, 1, 256, true));
      break;
    case types::pdu_session_res_failed_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_failed_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ng_ran_bearer_context_setup_resp_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ng_ran_bearer_context_setup_resp_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDU-Session-Resource-Setup-List", "PDU-Session-Resource-Failed-List"};
  return convert_enum_idx(names, 2, value, "ng_ran_bearer_context_setup_resp_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<eutran_bearer_context_setup_resp_o>;

SRSASN_CODE eutran_bearer_context_setup_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += drb_failed_list_eutran_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)37, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_setup_list_eutran, 1, 32, true));
  }
  if (drb_failed_list_eutran_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)38, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_failed_list_eutran, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_bearer_context_setup_resp_container::unpack(cbit_ref& bref)
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
      case 37: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_setup_list_eutran, bref, 1, 32, true));
        break;
      }
      case 38: {
        drb_failed_list_eutran_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_failed_list_eutran, bref, 1, 32, true));
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
void eutran_bearer_context_setup_resp_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 37);
  j.write_str("criticality", "ignore");
  j.start_array("Value");
  for (const auto& e1 : drb_setup_list_eutran) {
    e1.to_json(j);
  }
  j.end_array();
  if (drb_failed_list_eutran_present) {
    j.write_int("id", 38);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drb_failed_list_eutran) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ng_ran_bearer_context_setup_resp_o>;

SRSASN_CODE ng_ran_bearer_context_setup_resp_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += pdu_session_res_failed_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)46, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_setup_list, 1, 256, true));
  }
  if (pdu_session_res_failed_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)47, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_failed_list, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_bearer_context_setup_resp_container::unpack(cbit_ref& bref)
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
      case 46: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_setup_list, bref, 1, 256, true));
        break;
      }
      case 47: {
        pdu_session_res_failed_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_failed_list, bref, 1, 256, true));
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
void ng_ran_bearer_context_setup_resp_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 46);
  j.write_str("criticality", "ignore");
  j.start_array("Value");
  for (const auto& e1 : pdu_session_res_setup_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (pdu_session_res_failed_list_present) {
    j.write_int("id", 47);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_res_failed_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// System-BearerContextSetupResponse ::= CHOICE
void sys_bearer_context_setup_resp_c::destroy_()
{
  switch (type_) {
    case types::e_utran_bearer_context_setup_resp:
      c.destroy<eutran_bearer_context_setup_resp_container>();
      break;
    case types::ng_ran_bearer_context_setup_resp:
      c.destroy<ng_ran_bearer_context_setup_resp_container>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void sys_bearer_context_setup_resp_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e_utran_bearer_context_setup_resp:
      c.init<eutran_bearer_context_setup_resp_container>();
      break;
    case types::ng_ran_bearer_context_setup_resp:
      c.init<ng_ran_bearer_context_setup_resp_container>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_resp_c");
  }
}
sys_bearer_context_setup_resp_c::sys_bearer_context_setup_resp_c(const sys_bearer_context_setup_resp_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e_utran_bearer_context_setup_resp:
      c.init(other.c.get<eutran_bearer_context_setup_resp_container>());
      break;
    case types::ng_ran_bearer_context_setup_resp:
      c.init(other.c.get<ng_ran_bearer_context_setup_resp_container>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_resp_c");
  }
}
sys_bearer_context_setup_resp_c&
sys_bearer_context_setup_resp_c::operator=(const sys_bearer_context_setup_resp_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e_utran_bearer_context_setup_resp:
      c.set(other.c.get<eutran_bearer_context_setup_resp_container>());
      break;
    case types::ng_ran_bearer_context_setup_resp:
      c.set(other.c.get<ng_ran_bearer_context_setup_resp_container>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_resp_c");
  }

  return *this;
}
eutran_bearer_context_setup_resp_container& sys_bearer_context_setup_resp_c::set_e_utran_bearer_context_setup_resp()
{
  set(types::e_utran_bearer_context_setup_resp);
  return c.get<eutran_bearer_context_setup_resp_container>();
}
ng_ran_bearer_context_setup_resp_container& sys_bearer_context_setup_resp_c::set_ng_ran_bearer_context_setup_resp()
{
  set(types::ng_ran_bearer_context_setup_resp);
  return c.get<ng_ran_bearer_context_setup_resp_container>();
}
protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>&
sys_bearer_context_setup_resp_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>();
}
void sys_bearer_context_setup_resp_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_utran_bearer_context_setup_resp:
      j.write_fieldname("e-UTRAN-BearerContextSetupResponse");
      c.get<eutran_bearer_context_setup_resp_container>().to_json(j);
      break;
    case types::ng_ran_bearer_context_setup_resp:
      j.write_fieldname("nG-RAN-BearerContextSetupResponse");
      c.get<ng_ran_bearer_context_setup_resp_container>().to_json(j);
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_resp_c");
  }
  j.end_obj();
}
SRSASN_CODE sys_bearer_context_setup_resp_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e_utran_bearer_context_setup_resp:
      HANDLE_CODE(c.get<eutran_bearer_context_setup_resp_container>().pack(bref));
      break;
    case types::ng_ran_bearer_context_setup_resp:
      HANDLE_CODE(c.get<ng_ran_bearer_context_setup_resp_container>().pack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_resp_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_bearer_context_setup_resp_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e_utran_bearer_context_setup_resp:
      HANDLE_CODE(c.get<eutran_bearer_context_setup_resp_container>().unpack(bref));
      break;
    case types::ng_ran_bearer_context_setup_resp:
      HANDLE_CODE(c.get<ng_ran_bearer_context_setup_resp_container>().unpack(bref));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_bearer_context_setup_resp_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_bearer_context_setup_resp_c::types_opts::to_string() const
{
  static const char* names[] = {
      "e-UTRAN-BearerContextSetupResponse", "nG-RAN-BearerContextSetupResponse", "choice-extension"};
  return convert_enum_idx(names, 3, value, "sys_bearer_context_setup_resp_c::types");
}

// BearerContextSetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t bearer_context_setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 16};
  return map_enum_number(names, 3, idx, "id");
}
bool bearer_context_setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 16};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e bearer_context_setup_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 16:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
bearer_context_setup_resp_ies_o::value_c bearer_context_setup_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 16:
      ret.set(value_c::types::sys_bearer_context_setup_resp);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e bearer_context_setup_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 16:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void bearer_context_setup_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::sys_bearer_context_setup_resp:
      c = sys_bearer_context_setup_resp_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_resp_ies_o::value_c");
  }
}
uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_bearer_context_setup_resp_c& bearer_context_setup_resp_ies_o::value_c::sys_bearer_context_setup_resp()
{
  assert_choice_type(types::sys_bearer_context_setup_resp, type_, "Value");
  return c.get<sys_bearer_context_setup_resp_c>();
}
const uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& bearer_context_setup_resp_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const sys_bearer_context_setup_resp_c& bearer_context_setup_resp_ies_o::value_c::sys_bearer_context_setup_resp() const
{
  assert_choice_type(types::sys_bearer_context_setup_resp, type_, "Value");
  return c.get<sys_bearer_context_setup_resp_c>();
}
void bearer_context_setup_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::sys_bearer_context_setup_resp:
      j.write_fieldname("System-BearerContextSetupResponse");
      c.get<sys_bearer_context_setup_resp_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE bearer_context_setup_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_setup_resp:
      HANDLE_CODE(c.get<sys_bearer_context_setup_resp_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_setup_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_bearer_context_setup_resp:
      HANDLE_CODE(c.get<sys_bearer_context_setup_resp_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bearer_context_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bearer_context_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-BearerContextSetupResponse"};
  return convert_enum_idx(names, 3, value, "bearer_context_setup_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bearer_context_setup_resp_ies_o>;

SRSASN_CODE bearer_context_setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)16, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sys_bearer_context_setup_resp.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bearer_context_setup_resp_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 16: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_bearer_context_setup_resp.unpack(bref));
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
void bearer_context_setup_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 16);
  j.write_str("criticality", "ignore");
  sys_bearer_context_setup_resp.to_json(j);
  j.end_obj();
}

// CellTrafficTraceIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t cell_traffic_trace_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 82, 114, 115, 117};
  return map_enum_number(names, 6, idx, "id");
}
bool cell_traffic_trace_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 82, 114, 115, 117};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e cell_traffic_trace_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 82:
      return crit_e::ignore;
    case 114:
      return crit_e::ignore;
    case 115:
      return crit_e::ignore;
    case 117:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
cell_traffic_trace_ies_o::value_c cell_traffic_trace_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 82:
      ret.set(value_c::types::trace_id);
      break;
    case 114:
      ret.set(value_c::types::trace_collection_entity_ip_address);
      break;
    case 115:
      ret.set(value_c::types::privacy_ind);
      break;
    case 117:
      ret.set(value_c::types::ur_iaddress);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e cell_traffic_trace_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 82:
      return presence_e::mandatory;
    case 114:
      return presence_e::mandatory;
    case 115:
      return presence_e::optional;
    case 117:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void cell_traffic_trace_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::trace_id:
      c = fixed_octstring<8, true>{};
      break;
    case types::trace_collection_entity_ip_address:
      c = bounded_bitstring<1, 160, true, true>{};
      break;
    case types::privacy_ind:
      c = privacy_ind_e{};
      break;
    case types::ur_iaddress:
      c = visible_string<0, MAX_ASN_STRING_LENGTH, false, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_traffic_trace_ies_o::value_c");
  }
}
uint64_t& cell_traffic_trace_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& cell_traffic_trace_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
fixed_octstring<8, true>& cell_traffic_trace_ies_o::value_c::trace_id()
{
  assert_choice_type(types::trace_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
bounded_bitstring<1, 160, true, true>& cell_traffic_trace_ies_o::value_c::trace_collection_entity_ip_address()
{
  assert_choice_type(types::trace_collection_entity_ip_address, type_, "Value");
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
privacy_ind_e& cell_traffic_trace_ies_o::value_c::privacy_ind()
{
  assert_choice_type(types::privacy_ind, type_, "Value");
  return c.get<privacy_ind_e>();
}
visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& cell_traffic_trace_ies_o::value_c::ur_iaddress()
{
  assert_choice_type(types::ur_iaddress, type_, "Value");
  return c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
}
const uint64_t& cell_traffic_trace_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& cell_traffic_trace_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const fixed_octstring<8, true>& cell_traffic_trace_ies_o::value_c::trace_id() const
{
  assert_choice_type(types::trace_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
const bounded_bitstring<1, 160, true, true>&
cell_traffic_trace_ies_o::value_c::trace_collection_entity_ip_address() const
{
  assert_choice_type(types::trace_collection_entity_ip_address, type_, "Value");
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
const privacy_ind_e& cell_traffic_trace_ies_o::value_c::privacy_ind() const
{
  assert_choice_type(types::privacy_ind, type_, "Value");
  return c.get<privacy_ind_e>();
}
const visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& cell_traffic_trace_ies_o::value_c::ur_iaddress() const
{
  assert_choice_type(types::ur_iaddress, type_, "Value");
  return c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
}
void cell_traffic_trace_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::trace_id:
      j.write_str("OCTET STRING", c.get<fixed_octstring<8, true>>().to_string());
      break;
    case types::trace_collection_entity_ip_address:
      j.write_str("BIT STRING", c.get<bounded_bitstring<1, 160, true, true>>().to_string());
      break;
    case types::privacy_ind:
      j.write_str("PrivacyIndicator", c.get<privacy_ind_e>().to_string());
      break;
    case types::ur_iaddress:
      j.write_str("VisibleString", c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "cell_traffic_trace_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE cell_traffic_trace_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::trace_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().pack(bref)));
      break;
    case types::trace_collection_entity_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().pack(bref)));
      break;
    case types::privacy_ind:
      HANDLE_CODE(c.get<privacy_ind_e>().pack(bref));
      break;
    case types::ur_iaddress:
      HANDLE_CODE((c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "cell_traffic_trace_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_traffic_trace_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::trace_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().unpack(bref)));
      break;
    case types::trace_collection_entity_ip_address:
      HANDLE_CODE((c.get<bounded_bitstring<1, 160, true, true>>().unpack(bref)));
      break;
    case types::privacy_ind:
      HANDLE_CODE(c.get<privacy_ind_e>().unpack(bref));
      break;
    case types::ur_iaddress:
      HANDLE_CODE((c.get<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "cell_traffic_trace_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cell_traffic_trace_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "BIT STRING",
                                "PrivacyIndicator",
                                "VisibleString"};
  return convert_enum_idx(names, 6, value, "cell_traffic_trace_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<cell_traffic_trace_ies_o>;

SRSASN_CODE cell_traffic_trace_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += privacy_ind_present ? 1 : 0;
  nof_ies += ur_iaddress_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)82, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(trace_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)114, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(trace_collection_entity_ip_address.pack(bref));
  }
  if (privacy_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)115, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(privacy_ind.pack(bref));
  }
  if (ur_iaddress_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)117, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ur_iaddress.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_traffic_trace_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 82: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_id.unpack(bref));
        break;
      }
      case 114: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_collection_entity_ip_address.unpack(bref));
        break;
      }
      case 115: {
        privacy_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(privacy_ind.unpack(bref));
        break;
      }
      case 117: {
        ur_iaddress_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ur_iaddress.unpack(bref));
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
void cell_traffic_trace_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 82);
  j.write_str("criticality", "ignore");
  j.write_str("Value", trace_id.to_string());
  j.write_int("id", 114);
  j.write_str("criticality", "ignore");
  j.write_str("Value", trace_collection_entity_ip_address.to_string());
  if (privacy_ind_present) {
    j.write_int("id", 115);
    j.write_str("criticality", "ignore");
    j.write_str("Value", privacy_ind.to_string());
  }
  if (ur_iaddress_present) {
    j.write_int("id", 117);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ur_iaddress.to_string());
  }
  j.end_obj();
}

// DLDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t dl_data_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 63, 67};
  return map_enum_number(names, 4, idx, "id");
}
bool dl_data_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 63, 67};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e dl_data_notif_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 63:
      return crit_e::ignore;
    case 67:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
dl_data_notif_ies_o::value_c dl_data_notif_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 63:
      ret.set(value_c::types::ppi);
      break;
    case 67:
      ret.set(value_c::types::pdu_session_to_notify_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e dl_data_notif_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 63:
      return presence_e::optional;
    case 67:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void dl_data_notif_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::ppi:
      c = uint8_t{};
      break;
    case types::pdu_session_to_notify_list:
      c = pdu_session_to_notify_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_data_notif_ies_o::value_c");
  }
}
uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint8_t& dl_data_notif_ies_o::value_c::ppi()
{
  assert_choice_type(types::ppi, type_, "Value");
  return c.get<uint8_t>();
}
pdu_session_to_notify_list_l& dl_data_notif_ies_o::value_c::pdu_session_to_notify_list()
{
  assert_choice_type(types::pdu_session_to_notify_list, type_, "Value");
  return c.get<pdu_session_to_notify_list_l>();
}
const uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& dl_data_notif_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint8_t& dl_data_notif_ies_o::value_c::ppi() const
{
  assert_choice_type(types::ppi, type_, "Value");
  return c.get<uint8_t>();
}
const pdu_session_to_notify_list_l& dl_data_notif_ies_o::value_c::pdu_session_to_notify_list() const
{
  assert_choice_type(types::pdu_session_to_notify_list, type_, "Value");
  return c.get<pdu_session_to_notify_list_l>();
}
void dl_data_notif_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::ppi:
      j.write_int("INTEGER (0..7,...)", c.get<uint8_t>());
      break;
    case types::pdu_session_to_notify_list:
      j.start_array("PDU-Session-To-Notify-List");
      for (const auto& e1 : c.get<pdu_session_to_notify_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "dl_data_notif_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_data_notif_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::ppi:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)7u, true, true));
      break;
    case types::pdu_session_to_notify_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_to_notify_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "dl_data_notif_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_data_notif_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::ppi:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)7u, true, true));
      break;
    case types::pdu_session_to_notify_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_to_notify_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "dl_data_notif_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* dl_data_notif_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (0..7,...)", "PDU-Session-To-Notify-List"};
  return convert_enum_idx(names, 4, value, "dl_data_notif_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_data_notif_ies_o>;

SRSASN_CODE dl_data_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ppi_present ? 1 : 0;
  nof_ies += pdu_session_to_notify_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (ppi_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)63, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, ppi, (uint8_t)0u, (uint8_t)7u, true, true));
  }
  if (pdu_session_to_notify_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)67, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_to_notify_list, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_data_notif_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 63: {
        ppi_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(ppi, bref, (uint8_t)0u, (uint8_t)7u, true, true));
        break;
      }
      case 67: {
        pdu_session_to_notify_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_to_notify_list, bref, 1, 256, true));
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
void dl_data_notif_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  if (ppi_present) {
    j.write_int("id", 63);
    j.write_str("criticality", "ignore");
    j.write_int("Value", ppi);
  }
  if (pdu_session_to_notify_list_present) {
    j.write_int("id", 67);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pdu_session_to_notify_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t data_usage_report_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 25};
  return map_enum_number(names, 3, idx, "id");
}
bool data_usage_report_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 25};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e data_usage_report_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 25:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
data_usage_report_ies_o::value_c data_usage_report_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 25:
      ret.set(value_c::types::data_usage_report_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e data_usage_report_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 25:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void data_usage_report_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::data_usage_report_list:
      c = data_usage_report_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "data_usage_report_ies_o::value_c");
  }
}
uint64_t& data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
data_usage_report_list_l& data_usage_report_ies_o::value_c::data_usage_report_list()
{
  assert_choice_type(types::data_usage_report_list, type_, "Value");
  return c.get<data_usage_report_list_l>();
}
const uint64_t& data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const data_usage_report_list_l& data_usage_report_ies_o::value_c::data_usage_report_list() const
{
  assert_choice_type(types::data_usage_report_list, type_, "Value");
  return c.get<data_usage_report_list_l>();
}
void data_usage_report_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::data_usage_report_list:
      j.start_array("Data-Usage-Report-List");
      for (const auto& e1 : c.get<data_usage_report_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "data_usage_report_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE data_usage_report_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::data_usage_report_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<data_usage_report_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "data_usage_report_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE data_usage_report_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::data_usage_report_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<data_usage_report_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "data_usage_report_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* data_usage_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Data-Usage-Report-List"};
  return convert_enum_idx(names, 3, value, "data_usage_report_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<data_usage_report_ies_o>;

SRSASN_CODE data_usage_report_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)25, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, data_usage_report_list, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE data_usage_report_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 25: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(data_usage_report_list, bref, 1, 32, true));
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
void data_usage_report_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 25);
  j.write_str("criticality", "ignore");
  j.start_array("Value");
  for (const auto& e1 : data_usage_report_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// DeactivateTraceIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t deactiv_trace_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 82};
  return map_enum_number(names, 3, idx, "id");
}
bool deactiv_trace_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 82};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e deactiv_trace_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 82:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
deactiv_trace_ies_o::value_c deactiv_trace_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 82:
      ret.set(value_c::types::trace_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e deactiv_trace_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 82:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void deactiv_trace_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::trace_id:
      c = fixed_octstring<8, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "deactiv_trace_ies_o::value_c");
  }
}
uint64_t& deactiv_trace_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& deactiv_trace_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
fixed_octstring<8, true>& deactiv_trace_ies_o::value_c::trace_id()
{
  assert_choice_type(types::trace_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
const uint64_t& deactiv_trace_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& deactiv_trace_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const fixed_octstring<8, true>& deactiv_trace_ies_o::value_c::trace_id() const
{
  assert_choice_type(types::trace_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
void deactiv_trace_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::trace_id:
      j.write_str("OCTET STRING", c.get<fixed_octstring<8, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "deactiv_trace_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE deactiv_trace_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::trace_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "deactiv_trace_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE deactiv_trace_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::trace_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "deactiv_trace_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* deactiv_trace_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "deactiv_trace_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<deactiv_trace_ies_o>;

SRSASN_CODE deactiv_trace_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)82, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(trace_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE deactiv_trace_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 82: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_id.unpack(bref));
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
void deactiv_trace_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 82);
  j.write_str("criticality", "ignore");
  j.write_str("Value", trace_id.to_string());
  j.end_obj();
}

// SupportedPLMNs-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
uint32_t supported_plmns_ext_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {110, 125, 135};
  return map_enum_number(names, 3, idx, "id");
}
bool supported_plmns_ext_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {110, 125, 135};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e supported_plmns_ext_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 110:
      return crit_e::reject;
    case 125:
      return crit_e::reject;
    case 135:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
supported_plmns_ext_ies_o::ext_c supported_plmns_ext_ies_o::get_ext(const uint32_t& id)
{
  ext_c ret{};
  switch (id) {
    case 110:
      ret.set(ext_c::types::npn_support_info);
      break;
    case 125:
      ret.set(ext_c::types::extended_slice_support_list);
      break;
    case 135:
      ret.set(ext_c::types::extended_nr_cgi_support_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e supported_plmns_ext_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 110:
      return presence_e::optional;
    case 125:
      return presence_e::optional;
    case 135:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Extension ::= OPEN TYPE
void supported_plmns_ext_ies_o::ext_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::npn_support_info:
      c = npn_support_info_c{};
      break;
    case types::extended_slice_support_list:
      c = extended_slice_support_list_l{};
      break;
    case types::extended_nr_cgi_support_list:
      c = extended_nr_cgi_support_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "supported_plmns_ext_ies_o::ext_c");
  }
}
npn_support_info_c& supported_plmns_ext_ies_o::ext_c::npn_support_info()
{
  assert_choice_type(types::npn_support_info, type_, "Extension");
  return c.get<npn_support_info_c>();
}
extended_slice_support_list_l& supported_plmns_ext_ies_o::ext_c::extended_slice_support_list()
{
  assert_choice_type(types::extended_slice_support_list, type_, "Extension");
  return c.get<extended_slice_support_list_l>();
}
extended_nr_cgi_support_list_l& supported_plmns_ext_ies_o::ext_c::extended_nr_cgi_support_list()
{
  assert_choice_type(types::extended_nr_cgi_support_list, type_, "Extension");
  return c.get<extended_nr_cgi_support_list_l>();
}
const npn_support_info_c& supported_plmns_ext_ies_o::ext_c::npn_support_info() const
{
  assert_choice_type(types::npn_support_info, type_, "Extension");
  return c.get<npn_support_info_c>();
}
const extended_slice_support_list_l& supported_plmns_ext_ies_o::ext_c::extended_slice_support_list() const
{
  assert_choice_type(types::extended_slice_support_list, type_, "Extension");
  return c.get<extended_slice_support_list_l>();
}
const extended_nr_cgi_support_list_l& supported_plmns_ext_ies_o::ext_c::extended_nr_cgi_support_list() const
{
  assert_choice_type(types::extended_nr_cgi_support_list, type_, "Extension");
  return c.get<extended_nr_cgi_support_list_l>();
}
void supported_plmns_ext_ies_o::ext_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::npn_support_info:
      j.write_fieldname("NPNSupportInfo");
      c.get<npn_support_info_c>().to_json(j);
      break;
    case types::extended_slice_support_list:
      j.start_array("ExtendedSliceSupportList");
      for (const auto& e1 : c.get<extended_slice_support_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::extended_nr_cgi_support_list:
      j.start_array("Extended-NR-CGI-Support-List");
      for (const auto& e1 : c.get<extended_nr_cgi_support_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "supported_plmns_ext_ies_o::ext_c");
  }
  j.end_obj();
}
SRSASN_CODE supported_plmns_ext_ies_o::ext_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::npn_support_info:
      HANDLE_CODE(c.get<npn_support_info_c>().pack(bref));
      break;
    case types::extended_slice_support_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<extended_slice_support_list_l>(), 1, 65535, true));
      break;
    case types::extended_nr_cgi_support_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<extended_nr_cgi_support_list_l>(), 1, 16384, true));
      break;
    default:
      log_invalid_choice_id(type_, "supported_plmns_ext_ies_o::ext_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_plmns_ext_ies_o::ext_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::npn_support_info:
      HANDLE_CODE(c.get<npn_support_info_c>().unpack(bref));
      break;
    case types::extended_slice_support_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<extended_slice_support_list_l>(), bref, 1, 65535, true));
      break;
    case types::extended_nr_cgi_support_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<extended_nr_cgi_support_list_l>(), bref, 1, 16384, true));
      break;
    default:
      log_invalid_choice_id(type_, "supported_plmns_ext_ies_o::ext_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* supported_plmns_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NPNSupportInfo", "ExtendedSliceSupportList", "Extended-NR-CGI-Support-List"};
  return convert_enum_idx(names, 3, value, "supported_plmns_ext_ies_o::ext_c::types");
}

// UE-associatedLogicalE1-ConnectionItemRes ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ue_associated_lc_e1_conn_item_res_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {5};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_associated_lc_e1_conn_item_res_o::is_id_valid(const uint32_t& id)
{
  return 5 == id;
}
crit_e ue_associated_lc_e1_conn_item_res_o::get_crit(const uint32_t& id)
{
  if (id == 5) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_associated_lc_e1_conn_item_res_o::value_c ue_associated_lc_e1_conn_item_res_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 5) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_associated_lc_e1_conn_item_res_o::get_presence(const uint32_t& id)
{
  if (id == 5) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_associated_lc_e1_conn_item_res_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-associatedLogicalE1-ConnectionItem");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_associated_lc_e1_conn_item_res_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_associated_lc_e1_conn_item_res_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-associatedLogicalE1-ConnectionItem"};
  return convert_enum_idx(names, 1, value, "ue_associated_lc_e1_conn_item_res_o::value_c::types");
}
uint8_t ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_associated_lc_e1_conn_item_res_o::value_c::types");
}

// EUTRAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t eutran_gnb_cu_up_counter_check_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {61};
  return map_enum_number(names, 1, idx, "id");
}
bool eutran_gnb_cu_up_counter_check_request_o::is_id_valid(const uint32_t& id)
{
  return 61 == id;
}
crit_e eutran_gnb_cu_up_counter_check_request_o::get_crit(const uint32_t& id)
{
  if (id == 61) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
eutran_gnb_cu_up_counter_check_request_o::value_c
eutran_gnb_cu_up_counter_check_request_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 61) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e eutran_gnb_cu_up_counter_check_request_o::get_presence(const uint32_t& id)
{
  if (id == 61) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void eutran_gnb_cu_up_counter_check_request_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("DRBs-Subject-To-Counter-Check-List-EUTRAN");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE eutran_gnb_cu_up_counter_check_request_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 32, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE eutran_gnb_cu_up_counter_check_request_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 32, true));
  return SRSASN_SUCCESS;
}

const char* eutran_gnb_cu_up_counter_check_request_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-Subject-To-Counter-Check-List-EUTRAN"};
  return convert_enum_idx(names, 1, value, "eutran_gnb_cu_up_counter_check_request_o::value_c::types");
}

// NG-RAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ng_ran_gnb_cu_up_counter_check_request_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {62};
  return map_enum_number(names, 1, idx, "id");
}
bool ng_ran_gnb_cu_up_counter_check_request_o::is_id_valid(const uint32_t& id)
{
  return 62 == id;
}
crit_e ng_ran_gnb_cu_up_counter_check_request_o::get_crit(const uint32_t& id)
{
  if (id == 62) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ng_ran_gnb_cu_up_counter_check_request_o::value_c
ng_ran_gnb_cu_up_counter_check_request_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 62) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ng_ran_gnb_cu_up_counter_check_request_o::get_presence(const uint32_t& id)
{
  if (id == 62) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ng_ran_gnb_cu_up_counter_check_request_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("DRBs-Subject-To-Counter-Check-List-NG-RAN");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE ng_ran_gnb_cu_up_counter_check_request_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 32, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_ran_gnb_cu_up_counter_check_request_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 32, true));
  return SRSASN_SUCCESS;
}

const char* ng_ran_gnb_cu_up_counter_check_request_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-Subject-To-Counter-Check-List-NG-RAN"};
  return convert_enum_idx(names, 1, value, "ng_ran_gnb_cu_up_counter_check_request_o::value_c::types");
}

// ResetAll ::= ENUMERATED
const char* reset_all_opts::to_string() const
{
  static const char* names[] = {"reset-all"};
  return convert_enum_idx(names, 1, value, "reset_all_e");
}

template struct asn1::protocol_ext_field_s<supported_plmns_ext_ies_o>;

SRSASN_CODE supported_plmns_ext_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += npn_support_info_present ? 1 : 0;
  nof_ies += extended_slice_support_list_present ? 1 : 0;
  nof_ies += extended_nr_cgi_support_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  if (npn_support_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)110, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(npn_support_info.pack(bref));
  }
  if (extended_slice_support_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)125, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, extended_slice_support_list, 1, 65535, true));
  }
  if (extended_nr_cgi_support_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)135, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, extended_nr_cgi_support_list, 1, 16384, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_plmns_ext_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 110: {
        npn_support_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(npn_support_info.unpack(bref));
        break;
      }
      case 125: {
        extended_slice_support_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(extended_slice_support_list, bref, 1, 65535, true));
        break;
      }
      case 135: {
        extended_nr_cgi_support_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(extended_nr_cgi_support_list, bref, 1, 16384, true));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID={} is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void supported_plmns_ext_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (npn_support_info_present) {
    j.write_int("id", 110);
    j.write_str("criticality", "reject");
    npn_support_info.to_json(j);
  }
  if (extended_slice_support_list_present) {
    j.write_int("id", 125);
    j.write_str("criticality", "reject");
    j.start_array("Extension");
    for (const auto& e1 : extended_slice_support_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (extended_nr_cgi_support_list_present) {
    j.write_int("id", 135);
    j.write_str("criticality", "ignore");
    j.start_array("Extension");
    for (const auto& e1 : extended_nr_cgi_support_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SupportedPLMNs-Item ::= SEQUENCE
SRSASN_CODE supported_plmns_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(slice_support_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(nr_cgi_support_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(qos_params_support_list_present, 1));
  HANDLE_CODE(bref.pack(ie_exts_present, 1));

  HANDLE_CODE(plmn_id.pack(bref));
  if (slice_support_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, slice_support_list, 1, 1024, true));
  }
  if (nr_cgi_support_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, nr_cgi_support_list, 1, 512, true));
  }
  if (qos_params_support_list_present) {
    HANDLE_CODE(qos_params_support_list.pack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE supported_plmns_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool slice_support_list_present;
  HANDLE_CODE(bref.unpack(slice_support_list_present, 1));
  bool nr_cgi_support_list_present;
  HANDLE_CODE(bref.unpack(nr_cgi_support_list_present, 1));
  HANDLE_CODE(bref.unpack(qos_params_support_list_present, 1));
  HANDLE_CODE(bref.unpack(ie_exts_present, 1));

  HANDLE_CODE(plmn_id.unpack(bref));
  if (slice_support_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(slice_support_list, bref, 1, 1024, true));
  }
  if (nr_cgi_support_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(nr_cgi_support_list, bref, 1, 512, true));
  }
  if (qos_params_support_list_present) {
    HANDLE_CODE(qos_params_support_list.unpack(bref));
  }
  if (ie_exts_present) {
    HANDLE_CODE(ie_exts.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void supported_plmns_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  if (slice_support_list.size() > 0) {
    j.start_array("slice-Support-List");
    for (const auto& e1 : slice_support_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nr_cgi_support_list.size() > 0) {
    j.start_array("nR-CGI-Support-List");
    for (const auto& e1 : nr_cgi_support_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (qos_params_support_list_present) {
    j.write_fieldname("qoS-Parameters-Support-List");
    qos_params_support_list.to_json(j);
  }
  if (ie_exts_present) {
    j.write_fieldname("iE-Extensions");
    ie_exts.to_json(j);
  }
  j.end_obj();
}

// UE-associatedLogicalE1-ConnectionItemResAck ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ue_associated_lc_e1_conn_item_res_ack_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {5};
  return map_enum_number(names, 1, idx, "id");
}
bool ue_associated_lc_e1_conn_item_res_ack_o::is_id_valid(const uint32_t& id)
{
  return 5 == id;
}
crit_e ue_associated_lc_e1_conn_item_res_ack_o::get_crit(const uint32_t& id)
{
  if (id == 5) {
    return crit_e::ignore;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
ue_associated_lc_e1_conn_item_res_ack_o::value_c ue_associated_lc_e1_conn_item_res_ack_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 5) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_associated_lc_e1_conn_item_res_ack_o::get_presence(const uint32_t& id)
{
  if (id == 5) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ue_associated_lc_e1_conn_item_res_ack_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("UE-associatedLogicalE1-ConnectionItem");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ue_associated_lc_e1_conn_item_res_ack_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_associated_lc_e1_conn_item_res_ack_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ue_associated_lc_e1_conn_item_res_ack_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-associatedLogicalE1-ConnectionItem"};
  return convert_enum_idx(names, 1, value, "ue_associated_lc_e1_conn_item_res_ack_o::value_c::types");
}
uint8_t ue_associated_lc_e1_conn_item_res_ack_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_associated_lc_e1_conn_item_res_ack_o::value_c::types");
}

template struct asn1::protocol_ie_single_container_s<ue_associated_lc_e1_conn_item_res_o>;

// ResetType ::= CHOICE
void reset_type_c::destroy_()
{
  switch (type_) {
    case types::part_of_e1_interface:
      c.destroy<ue_associated_lc_e1_conn_list_res_l>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void reset_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e1_interface:
      break;
    case types::part_of_e1_interface:
      c.init<ue_associated_lc_e1_conn_list_res_l>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_type_c");
  }
}
reset_type_c::reset_type_c(const reset_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e1_interface:
      c.init(other.c.get<reset_all_e>());
      break;
    case types::part_of_e1_interface:
      c.init(other.c.get<ue_associated_lc_e1_conn_list_res_l>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_type_c");
  }
}
reset_type_c& reset_type_c::operator=(const reset_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e1_interface:
      c.set(other.c.get<reset_all_e>());
      break;
    case types::part_of_e1_interface:
      c.set(other.c.get<ue_associated_lc_e1_conn_list_res_l>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_type_c");
  }

  return *this;
}
reset_all_e& reset_type_c::set_e1_interface()
{
  set(types::e1_interface);
  return c.get<reset_all_e>();
}
ue_associated_lc_e1_conn_list_res_l& reset_type_c::set_part_of_e1_interface()
{
  set(types::part_of_e1_interface);
  return c.get<ue_associated_lc_e1_conn_list_res_l>();
}
protocol_ie_single_container_s<reset_type_ext_ies_o>& reset_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
}
void reset_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e1_interface:
      j.write_str("e1-Interface", "reset-all");
      break;
    case types::part_of_e1_interface:
      j.start_array("partOfE1-Interface");
      for (const auto& e1 : c.get<ue_associated_lc_e1_conn_list_res_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "reset_type_c");
  }
  j.end_obj();
}
SRSASN_CODE reset_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e1_interface:
      HANDLE_CODE(c.get<reset_all_e>().pack(bref));
      break;
    case types::part_of_e1_interface:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_associated_lc_e1_conn_list_res_l>(), 1, 65536, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e1_interface:
      HANDLE_CODE(c.get<reset_all_e>().unpack(bref));
      break;
    case types::part_of_e1_interface:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_associated_lc_e1_conn_list_res_l>(), bref, 1, 65536, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* reset_type_c::types_opts::to_string() const
{
  static const char* names[] = {"e1-Interface", "partOfE1-Interface", "choice-extension"};
  return convert_enum_idx(names, 3, value, "reset_type_c::types");
}

// System-GNB-CU-UP-CounterCheckRequest ::= CHOICE
void sys_gnb_cu_up_counter_check_request_c::destroy_()
{
  switch (type_) {
    case types::e_utran_gnb_cu_up_counter_check_request:
      c.destroy<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>();
      break;
    case types::ng_ran_gnb_cu_up_counter_check_request:
      c.destroy<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>();
      break;
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void sys_gnb_cu_up_counter_check_request_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e_utran_gnb_cu_up_counter_check_request:
      c.init<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>();
      break;
    case types::ng_ran_gnb_cu_up_counter_check_request:
      c.init<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>();
      break;
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_gnb_cu_up_counter_check_request_c");
  }
}
sys_gnb_cu_up_counter_check_request_c::sys_gnb_cu_up_counter_check_request_c(
    const sys_gnb_cu_up_counter_check_request_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e_utran_gnb_cu_up_counter_check_request:
      c.init(other.c.get<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>());
      break;
    case types::ng_ran_gnb_cu_up_counter_check_request:
      c.init(other.c.get<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>());
      break;
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_gnb_cu_up_counter_check_request_c");
  }
}
sys_gnb_cu_up_counter_check_request_c&
sys_gnb_cu_up_counter_check_request_c::operator=(const sys_gnb_cu_up_counter_check_request_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e_utran_gnb_cu_up_counter_check_request:
      c.set(other.c.get<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>());
      break;
    case types::ng_ran_gnb_cu_up_counter_check_request:
      c.set(other.c.get<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>());
      break;
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sys_gnb_cu_up_counter_check_request_c");
  }

  return *this;
}
protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>&
sys_gnb_cu_up_counter_check_request_c::set_e_utran_gnb_cu_up_counter_check_request()
{
  set(types::e_utran_gnb_cu_up_counter_check_request);
  return c.get<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>();
}
protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>&
sys_gnb_cu_up_counter_check_request_c::set_ng_ran_gnb_cu_up_counter_check_request()
{
  set(types::ng_ran_gnb_cu_up_counter_check_request);
  return c.get<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>();
}
protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>&
sys_gnb_cu_up_counter_check_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>();
}
void sys_gnb_cu_up_counter_check_request_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e_utran_gnb_cu_up_counter_check_request:
      j.write_fieldname("e-UTRAN-GNB-CU-UP-CounterCheckRequest");
      break;
    case types::ng_ran_gnb_cu_up_counter_check_request:
      j.write_fieldname("nG-RAN-GNB-CU-UP-CounterCheckRequest");
      break;
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sys_gnb_cu_up_counter_check_request_c");
  }
  j.end_obj();
}
SRSASN_CODE sys_gnb_cu_up_counter_check_request_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e_utran_gnb_cu_up_counter_check_request:
      HANDLE_CODE(pack_dyn_seq_of(
          bref, c.get<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>(), 0, 65535, true));
      break;
    case types::ng_ran_gnb_cu_up_counter_check_request:
      HANDLE_CODE(pack_dyn_seq_of(
          bref, c.get<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>(), 0, 65535, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_gnb_cu_up_counter_check_request_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sys_gnb_cu_up_counter_check_request_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e_utran_gnb_cu_up_counter_check_request:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<protocol_ie_container_l<eutran_gnb_cu_up_counter_check_request_o>>(), bref, 0, 65535, true));
      break;
    case types::ng_ran_gnb_cu_up_counter_check_request:
      HANDLE_CODE(unpack_dyn_seq_of(
          c.get<protocol_ie_container_l<ng_ran_gnb_cu_up_counter_check_request_o>>(), bref, 0, 65535, true));
      break;
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sys_gnb_cu_up_counter_check_request_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sys_gnb_cu_up_counter_check_request_c::types_opts::to_string() const
{
  static const char* names[] = {
      "e-UTRAN-GNB-CU-UP-CounterCheckRequest", "nG-RAN-GNB-CU-UP-CounterCheckRequest", "choice-extension"};
  return convert_enum_idx(names, 3, value, "sys_gnb_cu_up_counter_check_request_c::types");
}

template struct asn1::protocol_ie_single_container_s<ue_associated_lc_e1_conn_item_res_ack_o>;

// E1ReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t e1_release_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 0};
  return map_enum_number(names, 2, idx, "id");
}
bool e1_release_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 0};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e1_release_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
e1_release_request_ies_o::value_c e1_release_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e1_release_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e1_release_request_ies_o::value_c::set(types::options e)
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
      log_invalid_choice_id(type_, "e1_release_request_ies_o::value_c");
  }
}
uint16_t& e1_release_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e1_release_request_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const uint16_t& e1_release_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e1_release_request_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
void e1_release_request_ies_o::value_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "e1_release_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e1_release_request_ies_o::value_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "e1_release_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_release_request_ies_o::value_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "e1_release_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause"};
  return convert_enum_idx(names, 2, value, "e1_release_request_ies_o::value_c::types");
}
uint8_t e1_release_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e1_release_request_ies_o::value_c::types");
}

// E1ReleaseResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t e1_release_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57};
  return map_enum_number(names, 1, idx, "id");
}
bool e1_release_resp_ies_o::is_id_valid(const uint32_t& id)
{
  return 57 == id;
}
crit_e e1_release_resp_ies_o::get_crit(const uint32_t& id)
{
  if (id == 57) {
    return crit_e::reject;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}
e1_release_resp_ies_o::value_c e1_release_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 57) {
    asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e e1_release_resp_ies_o::get_presence(const uint32_t& id)
{
  if (id == 57) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id={} is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e1_release_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("INTEGER (0..255,...)", c);
  j.end_obj();
}
SRSASN_CODE e1_release_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_integer(bref, c, (uint16_t)0u, (uint16_t)255u, true, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_release_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_integer(c, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  return SRSASN_SUCCESS;
}

const char* e1_release_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)"};
  return convert_enum_idx(names, 1, value, "e1_release_resp_ies_o::value_c::types");
}
uint8_t e1_release_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e1_release_resp_ies_o::value_c::types");
}

// EarlyForwardingSNTransferIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t early_forwarding_sn_transfer_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 119};
  return map_enum_number(names, 3, idx, "id");
}
bool early_forwarding_sn_transfer_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 119};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e early_forwarding_sn_transfer_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 119:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
early_forwarding_sn_transfer_ies_o::value_c early_forwarding_sn_transfer_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 119:
      ret.set(value_c::types::drbs_subject_to_early_forwarding_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e early_forwarding_sn_transfer_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 119:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void early_forwarding_sn_transfer_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::drbs_subject_to_early_forwarding_list:
      c = drbs_subject_to_early_forwarding_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_sn_transfer_ies_o::value_c");
  }
}
uint64_t& early_forwarding_sn_transfer_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& early_forwarding_sn_transfer_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
drbs_subject_to_early_forwarding_list_l&
early_forwarding_sn_transfer_ies_o::value_c::drbs_subject_to_early_forwarding_list()
{
  assert_choice_type(types::drbs_subject_to_early_forwarding_list, type_, "Value");
  return c.get<drbs_subject_to_early_forwarding_list_l>();
}
const uint64_t& early_forwarding_sn_transfer_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& early_forwarding_sn_transfer_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const drbs_subject_to_early_forwarding_list_l&
early_forwarding_sn_transfer_ies_o::value_c::drbs_subject_to_early_forwarding_list() const
{
  assert_choice_type(types::drbs_subject_to_early_forwarding_list, type_, "Value");
  return c.get<drbs_subject_to_early_forwarding_list_l>();
}
void early_forwarding_sn_transfer_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::drbs_subject_to_early_forwarding_list:
      j.start_array("DRBs-Subject-To-Early-Forwarding-List");
      for (const auto& e1 : c.get<drbs_subject_to_early_forwarding_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_sn_transfer_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE early_forwarding_sn_transfer_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drbs_subject_to_early_forwarding_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_subject_to_early_forwarding_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_sn_transfer_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE early_forwarding_sn_transfer_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drbs_subject_to_early_forwarding_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_subject_to_early_forwarding_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "early_forwarding_sn_transfer_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* early_forwarding_sn_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "DRBs-Subject-To-Early-Forwarding-List"};
  return convert_enum_idx(names, 3, value, "early_forwarding_sn_transfer_ies_o::value_c::types");
}

// ErrorIndication-IEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t error_ind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 2, 3, 0, 1};
  return map_enum_number(names, 5, idx, "id");
}
bool error_ind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 2, 3, 0, 1};
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
    case 57:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    case 3:
      return crit_e::ignore;
    case 0:
      return crit_e::ignore;
    case 1:
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
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 1:
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
    case 57:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    case 3:
      return presence_e::optional;
    case 0:
      return presence_e::optional;
    case 1:
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
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
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
uint64_t& error_ind_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& error_ind_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
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
const uint64_t& error_ind_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& error_ind_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
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
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
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
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
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
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
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
      "INTEGER (0..255,...)", "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "error_ind_ies_o::value_c::types");
}

// GNB-CU-CP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_cfg_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 1, 30, 31, 86};
  return map_enum_number(names, 5, idx, "id");
}
bool gnb_cu_cp_cfg_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 1, 30, 31, 86};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_cp_cfg_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 30:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 86:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_cp_cfg_upd_ack_ies_o::value_c gnb_cu_cp_cfg_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 30:
      ret.set(value_c::types::gnb_cu_cp_tnl_a_setup_list);
      break;
    case 31:
      ret.set(value_c::types::gnb_cu_cp_tnl_a_failed_to_setup_list);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_cfg_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    case 30:
      return presence_e::optional;
    case 31:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_cfg_upd_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::gnb_cu_cp_tnl_a_setup_list:
      c = gnb_cu_cp_tnl_a_setup_list_l{};
      break;
    case types::gnb_cu_cp_tnl_a_failed_to_setup_list:
      c = gnb_cu_cp_tnl_a_failed_to_setup_list_l{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
  }
}
uint16_t& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
gnb_cu_cp_tnl_a_setup_list_l& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnl_a_setup_list()
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_setup_list_l>();
}
gnb_cu_cp_tnl_a_failed_to_setup_list_l& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnl_a_failed_to_setup_list()
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_failed_to_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_failed_to_setup_list_l>();
}
transport_layer_address_info_s& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const uint16_t& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const gnb_cu_cp_tnl_a_setup_list_l& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnl_a_setup_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_setup_list_l>();
}
const gnb_cu_cp_tnl_a_failed_to_setup_list_l&
gnb_cu_cp_cfg_upd_ack_ies_o::value_c::gnb_cu_cp_tnl_a_failed_to_setup_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_failed_to_setup_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_failed_to_setup_list_l>();
}
const transport_layer_address_info_s& gnb_cu_cp_cfg_upd_ack_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
void gnb_cu_cp_cfg_upd_ack_ies_o::value_c::to_json(json_writer& j) const
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
    case types::gnb_cu_cp_tnl_a_setup_list:
      j.start_array("GNB-CU-CP-TNLA-Setup-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnl_a_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_cp_tnl_a_failed_to_setup_list:
      j.start_array("GNB-CU-CP-TNLA-Failed-To-Setup-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnl_a_failed_to_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::gnb_cu_cp_tnl_a_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnl_a_setup_list_l>(), 1, 32, true));
      break;
    case types::gnb_cu_cp_tnl_a_failed_to_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnl_a_failed_to_setup_list_l>(), 1, 32, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_tnl_a_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnl_a_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::gnb_cu_cp_tnl_a_failed_to_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnl_a_failed_to_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "CriticalityDiagnostics",
                                "GNB-CU-CP-TNLA-Setup-List",
                                "GNB-CU-CP-TNLA-Failed-To-Setup-List",
                                "Transport-Layer-Address-Info"};
  return convert_enum_idx(names, 5, value, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cp_cfg_upd_ack_ies_o::value_c::types");
}

// GNB-CU-CP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_cfg_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool gnb_cu_cp_cfg_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_cp_cfg_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_cp_cfg_upd_fail_ies_o::value_c gnb_cu_cp_cfg_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 12:
      ret.set(value_c::types::time_to_wait);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_cfg_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 12:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_cfg_upd_fail_ies_o::value_c::set(types::options e)
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
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c");
  }
}
uint16_t& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& gnb_cu_cp_cfg_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void gnb_cu_cp_cfg_upd_fail_ies_o::value_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_cfg_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_cfg_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cp_cfg_upd_fail_ies_o::value_c::types");
}

// GNB-CU-CP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_cfg_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 9, 27, 28, 29, 86, 129};
  return map_enum_number(names, 7, idx, "id");
}
bool gnb_cu_cp_cfg_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 9, 27, 28, 29, 86, 129};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_cp_cfg_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 9:
      return crit_e::ignore;
    case 27:
      return crit_e::ignore;
    case 28:
      return crit_e::ignore;
    case 29:
      return crit_e::ignore;
    case 86:
      return crit_e::ignore;
    case 129:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_cp_cfg_upd_ies_o::value_c gnb_cu_cp_cfg_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 9:
      ret.set(value_c::types::gnb_cu_cp_name);
      break;
    case 27:
      ret.set(value_c::types::gnb_cu_cp_tnl_a_to_add_list);
      break;
    case 28:
      ret.set(value_c::types::gnb_cu_cp_tnl_a_to_rem_list);
      break;
    case 29:
      ret.set(value_c::types::gnb_cu_cp_tnl_a_to_upd_list);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    case 129:
      ret.set(value_c::types::extended_gnb_cu_cp_name);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_cfg_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    case 27:
      return presence_e::optional;
    case 28:
      return presence_e::optional;
    case 29:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    case 129:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_cfg_upd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_cp_name:
      c = printable_string<1, 150, true, true>{};
      break;
    case types::gnb_cu_cp_tnl_a_to_add_list:
      c = gnb_cu_cp_tnl_a_to_add_list_l{};
      break;
    case types::gnb_cu_cp_tnl_a_to_rem_list:
      c = gnb_cu_cp_tnl_a_to_rem_list_l{};
      break;
    case types::gnb_cu_cp_tnl_a_to_upd_list:
      c = gnb_cu_cp_tnl_a_to_upd_list_l{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::extended_gnb_cu_cp_name:
      c = extended_gnb_cu_cp_name_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
  }
}
uint16_t& gnb_cu_cp_cfg_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
printable_string<1, 150, true, true>& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_name()
{
  assert_choice_type(types::gnb_cu_cp_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
gnb_cu_cp_tnl_a_to_add_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnl_a_to_add_list()
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_to_add_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_to_add_list_l>();
}
gnb_cu_cp_tnl_a_to_rem_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnl_a_to_rem_list()
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_to_rem_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_to_rem_list_l>();
}
gnb_cu_cp_tnl_a_to_upd_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnl_a_to_upd_list()
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_to_upd_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_to_upd_list_l>();
}
transport_layer_address_info_s& gnb_cu_cp_cfg_upd_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
extended_gnb_cu_cp_name_s& gnb_cu_cp_cfg_upd_ies_o::value_c::extended_gnb_cu_cp_name()
{
  assert_choice_type(types::extended_gnb_cu_cp_name, type_, "Value");
  return c.get<extended_gnb_cu_cp_name_s>();
}
const uint16_t& gnb_cu_cp_cfg_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const printable_string<1, 150, true, true>& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_name() const
{
  assert_choice_type(types::gnb_cu_cp_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
const gnb_cu_cp_tnl_a_to_add_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnl_a_to_add_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_to_add_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_to_add_list_l>();
}
const gnb_cu_cp_tnl_a_to_rem_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnl_a_to_rem_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_to_rem_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_to_rem_list_l>();
}
const gnb_cu_cp_tnl_a_to_upd_list_l& gnb_cu_cp_cfg_upd_ies_o::value_c::gnb_cu_cp_tnl_a_to_upd_list() const
{
  assert_choice_type(types::gnb_cu_cp_tnl_a_to_upd_list, type_, "Value");
  return c.get<gnb_cu_cp_tnl_a_to_upd_list_l>();
}
const transport_layer_address_info_s& gnb_cu_cp_cfg_upd_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const extended_gnb_cu_cp_name_s& gnb_cu_cp_cfg_upd_ies_o::value_c::extended_gnb_cu_cp_name() const
{
  assert_choice_type(types::extended_gnb_cu_cp_name, type_, "Value");
  return c.get<extended_gnb_cu_cp_name_s>();
}
void gnb_cu_cp_cfg_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      j.write_str("PrintableString", c.get<printable_string<1, 150, true, true>>().to_string());
      break;
    case types::gnb_cu_cp_tnl_a_to_add_list:
      j.start_array("GNB-CU-CP-TNLA-To-Add-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnl_a_to_add_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_cp_tnl_a_to_rem_list:
      j.start_array("GNB-CU-CP-TNLA-To-Remove-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnl_a_to_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_cp_tnl_a_to_upd_list:
      j.start_array("GNB-CU-CP-TNLA-To-Update-List");
      for (const auto& e1 : c.get<gnb_cu_cp_tnl_a_to_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    case types::extended_gnb_cu_cp_name:
      j.write_fieldname("Extended-GNB-CU-CP-Name");
      c.get<extended_gnb_cu_cp_name_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().pack(bref)));
      break;
    case types::gnb_cu_cp_tnl_a_to_add_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnl_a_to_add_list_l>(), 1, 32, true));
      break;
    case types::gnb_cu_cp_tnl_a_to_rem_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnl_a_to_rem_list_l>(), 1, 32, true));
      break;
    case types::gnb_cu_cp_tnl_a_to_upd_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_cp_tnl_a_to_upd_list_l>(), 1, 32, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    case types::extended_gnb_cu_cp_name:
      HANDLE_CODE(c.get<extended_gnb_cu_cp_name_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().unpack(bref)));
      break;
    case types::gnb_cu_cp_tnl_a_to_add_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnl_a_to_add_list_l>(), bref, 1, 32, true));
      break;
    case types::gnb_cu_cp_tnl_a_to_rem_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnl_a_to_rem_list_l>(), bref, 1, 32, true));
      break;
    case types::gnb_cu_cp_tnl_a_to_upd_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_cp_tnl_a_to_upd_list_l>(), bref, 1, 32, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    case types::extended_gnb_cu_cp_name:
      HANDLE_CODE(c.get<extended_gnb_cu_cp_name_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "PrintableString",
                                "GNB-CU-CP-TNLA-To-Add-List",
                                "GNB-CU-CP-TNLA-To-Remove-List",
                                "GNB-CU-CP-TNLA-To-Update-List",
                                "Transport-Layer-Address-Info",
                                "Extended-GNB-CU-CP-Name"};
  return convert_enum_idx(names, 7, value, "gnb_cu_cp_cfg_upd_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_cfg_upd_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cp_cfg_upd_ies_o::value_c::types");
}

// GNB-CU-CP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_e1_setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool gnb_cu_cp_e1_setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_cp_e1_setup_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_cp_e1_setup_fail_ies_o::value_c gnb_cu_cp_e1_setup_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 12:
      ret.set(value_c::types::time_to_wait);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_e1_setup_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 12:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_e1_setup_fail_ies_o::value_c::set(types::options e)
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
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_fail_ies_o::value_c");
  }
}
uint16_t& gnb_cu_cp_e1_setup_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& gnb_cu_cp_e1_setup_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& gnb_cu_cp_e1_setup_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& gnb_cu_cp_e1_setup_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& gnb_cu_cp_e1_setup_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& gnb_cu_cp_e1_setup_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& gnb_cu_cp_e1_setup_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& gnb_cu_cp_e1_setup_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void gnb_cu_cp_e1_setup_fail_ies_o::value_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_e1_setup_fail_ies_o::value_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_e1_setup_fail_ies_o::value_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_e1_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "gnb_cu_cp_e1_setup_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_e1_setup_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cp_e1_setup_fail_ies_o::value_c::types");
}

// GNB-CU-CP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_e1_setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 9, 86, 129};
  return map_enum_number(names, 4, idx, "id");
}
bool gnb_cu_cp_e1_setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 9, 86, 129};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_cp_e1_setup_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 9:
      return crit_e::ignore;
    case 86:
      return crit_e::ignore;
    case 129:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_cp_e1_setup_request_ies_o::value_c gnb_cu_cp_e1_setup_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 9:
      ret.set(value_c::types::gnb_cu_cp_name);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    case 129:
      ret.set(value_c::types::extended_gnb_cu_cp_name);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_e1_setup_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    case 129:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_e1_setup_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_cp_name:
      c = printable_string<1, 150, true, true>{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::extended_gnb_cu_cp_name:
      c = extended_gnb_cu_cp_name_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
  }
}
uint16_t& gnb_cu_cp_e1_setup_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
printable_string<1, 150, true, true>& gnb_cu_cp_e1_setup_request_ies_o::value_c::gnb_cu_cp_name()
{
  assert_choice_type(types::gnb_cu_cp_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
transport_layer_address_info_s& gnb_cu_cp_e1_setup_request_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
extended_gnb_cu_cp_name_s& gnb_cu_cp_e1_setup_request_ies_o::value_c::extended_gnb_cu_cp_name()
{
  assert_choice_type(types::extended_gnb_cu_cp_name, type_, "Value");
  return c.get<extended_gnb_cu_cp_name_s>();
}
const uint16_t& gnb_cu_cp_e1_setup_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const printable_string<1, 150, true, true>& gnb_cu_cp_e1_setup_request_ies_o::value_c::gnb_cu_cp_name() const
{
  assert_choice_type(types::gnb_cu_cp_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
const transport_layer_address_info_s& gnb_cu_cp_e1_setup_request_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const extended_gnb_cu_cp_name_s& gnb_cu_cp_e1_setup_request_ies_o::value_c::extended_gnb_cu_cp_name() const
{
  assert_choice_type(types::extended_gnb_cu_cp_name, type_, "Value");
  return c.get<extended_gnb_cu_cp_name_s>();
}
void gnb_cu_cp_e1_setup_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      j.write_str("PrintableString", c.get<printable_string<1, 150, true, true>>().to_string());
      break;
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    case types::extended_gnb_cu_cp_name:
      j.write_fieldname("Extended-GNB-CU-CP-Name");
      c.get<extended_gnb_cu_cp_name_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_e1_setup_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().pack(bref)));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    case types::extended_gnb_cu_cp_name:
      HANDLE_CODE(c.get<extended_gnb_cu_cp_name_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_e1_setup_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().unpack(bref)));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    case types::extended_gnb_cu_cp_name:
      HANDLE_CODE(c.get<extended_gnb_cu_cp_name_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_e1_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "PrintableString", "Transport-Layer-Address-Info", "Extended-GNB-CU-CP-Name"};
  return convert_enum_idx(names, 4, value, "gnb_cu_cp_e1_setup_request_ies_o::value_c::types");
}
uint8_t gnb_cu_cp_e1_setup_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cp_e1_setup_request_ies_o::value_c::types");
}

// GNB-CU-CP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_e1_setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 7, 8, 10, 11, 64, 86, 130};
  return map_enum_number(names, 8, idx, "id");
}
bool gnb_cu_cp_e1_setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 7, 8, 10, 11, 64, 86, 130};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_cp_e1_setup_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 8:
      return crit_e::ignore;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 64:
      return crit_e::ignore;
    case 86:
      return crit_e::ignore;
    case 130:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_cp_e1_setup_resp_ies_o::value_c gnb_cu_cp_e1_setup_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 7:
      ret.set(value_c::types::gnb_cu_up_id);
      break;
    case 8:
      ret.set(value_c::types::gnb_cu_up_name);
      break;
    case 10:
      ret.set(value_c::types::cn_support);
      break;
    case 11:
      ret.set(value_c::types::supported_plmns);
      break;
    case 64:
      ret.set(value_c::types::gnb_cu_up_capacity);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    case 130:
      ret.set(value_c::types::extended_gnb_cu_up_name);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_e1_setup_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 7:
      return presence_e::mandatory;
    case 8:
      return presence_e::optional;
    case 10:
      return presence_e::mandatory;
    case 11:
      return presence_e::mandatory;
    case 64:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    case 130:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_e1_setup_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_name:
      c = printable_string<1, 150, true, true>{};
      break;
    case types::cn_support:
      c = cn_support_e{};
      break;
    case types::supported_plmns:
      c = supported_plmns_list_l{};
      break;
    case types::gnb_cu_up_capacity:
      c = uint16_t{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::extended_gnb_cu_up_name:
      c = extended_gnb_cu_up_name_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
  }
}
uint16_t& gnb_cu_cp_e1_setup_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint64_t& gnb_cu_cp_e1_setup_resp_ies_o::value_c::gnb_cu_up_id()
{
  assert_choice_type(types::gnb_cu_up_id, type_, "Value");
  return c.get<uint64_t>();
}
printable_string<1, 150, true, true>& gnb_cu_cp_e1_setup_resp_ies_o::value_c::gnb_cu_up_name()
{
  assert_choice_type(types::gnb_cu_up_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
cn_support_e& gnb_cu_cp_e1_setup_resp_ies_o::value_c::cn_support()
{
  assert_choice_type(types::cn_support, type_, "Value");
  return c.get<cn_support_e>();
}
supported_plmns_list_l& gnb_cu_cp_e1_setup_resp_ies_o::value_c::supported_plmns()
{
  assert_choice_type(types::supported_plmns, type_, "Value");
  return c.get<supported_plmns_list_l>();
}
uint16_t& gnb_cu_cp_e1_setup_resp_ies_o::value_c::gnb_cu_up_capacity()
{
  assert_choice_type(types::gnb_cu_up_capacity, type_, "Value");
  return c.get<uint16_t>();
}
transport_layer_address_info_s& gnb_cu_cp_e1_setup_resp_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
extended_gnb_cu_up_name_s& gnb_cu_cp_e1_setup_resp_ies_o::value_c::extended_gnb_cu_up_name()
{
  assert_choice_type(types::extended_gnb_cu_up_name, type_, "Value");
  return c.get<extended_gnb_cu_up_name_s>();
}
const uint16_t& gnb_cu_cp_e1_setup_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint64_t& gnb_cu_cp_e1_setup_resp_ies_o::value_c::gnb_cu_up_id() const
{
  assert_choice_type(types::gnb_cu_up_id, type_, "Value");
  return c.get<uint64_t>();
}
const printable_string<1, 150, true, true>& gnb_cu_cp_e1_setup_resp_ies_o::value_c::gnb_cu_up_name() const
{
  assert_choice_type(types::gnb_cu_up_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
const cn_support_e& gnb_cu_cp_e1_setup_resp_ies_o::value_c::cn_support() const
{
  assert_choice_type(types::cn_support, type_, "Value");
  return c.get<cn_support_e>();
}
const supported_plmns_list_l& gnb_cu_cp_e1_setup_resp_ies_o::value_c::supported_plmns() const
{
  assert_choice_type(types::supported_plmns, type_, "Value");
  return c.get<supported_plmns_list_l>();
}
const uint16_t& gnb_cu_cp_e1_setup_resp_ies_o::value_c::gnb_cu_up_capacity() const
{
  assert_choice_type(types::gnb_cu_up_capacity, type_, "Value");
  return c.get<uint16_t>();
}
const transport_layer_address_info_s& gnb_cu_cp_e1_setup_resp_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const extended_gnb_cu_up_name_s& gnb_cu_cp_e1_setup_resp_ies_o::value_c::extended_gnb_cu_up_name() const
{
  assert_choice_type(types::extended_gnb_cu_up_name, type_, "Value");
  return c.get<extended_gnb_cu_up_name_s>();
}
void gnb_cu_cp_e1_setup_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      j.write_int("INTEGER (0..68719476735)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      j.write_str("PrintableString", c.get<printable_string<1, 150, true, true>>().to_string());
      break;
    case types::cn_support:
      j.write_str("CNSupport", c.get<cn_support_e>().to_string());
      break;
    case types::supported_plmns:
      j.start_array("SupportedPLMNs-List");
      for (const auto& e1 : c.get<supported_plmns_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_up_capacity:
      j.write_int("INTEGER (0..255)", c.get<uint16_t>());
      break;
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    case types::extended_gnb_cu_up_name:
      j.write_fieldname("Extended-GNB-CU-UP-Name");
      c.get<extended_gnb_cu_up_name_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_e1_setup_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::gnb_cu_up_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().pack(bref)));
      break;
    case types::cn_support:
      HANDLE_CODE(c.get<cn_support_e>().pack(bref));
      break;
    case types::supported_plmns:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<supported_plmns_list_l>(), 1, 12, true));
      break;
    case types::gnb_cu_up_capacity:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    case types::extended_gnb_cu_up_name:
      HANDLE_CODE(c.get<extended_gnb_cu_up_name_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_e1_setup_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::gnb_cu_up_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().unpack(bref)));
      break;
    case types::cn_support:
      HANDLE_CODE(c.get<cn_support_e>().unpack(bref));
      break;
    case types::supported_plmns:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<supported_plmns_list_l>(), bref, 1, 12, true));
      break;
    case types::gnb_cu_up_capacity:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    case types::extended_gnb_cu_up_name:
      HANDLE_CODE(c.get<extended_gnb_cu_up_name_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_e1_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_e1_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (0..68719476735)",
                                "PrintableString",
                                "CNSupport",
                                "SupportedPLMNs-List",
                                "INTEGER (0..255)",
                                "Transport-Layer-Address-Info",
                                "Extended-GNB-CU-UP-Name"};
  return convert_enum_idx(names, 8, value, "gnb_cu_cp_e1_setup_resp_ies_o::value_c::types");
}

// GNB-CU-CPMeasurementResultsInformationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_cp_meas_results_info_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 128};
  return map_enum_number(names, 3, idx, "id");
}
bool gnb_cu_cp_meas_results_info_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 128};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_cp_meas_results_info_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 128:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_cp_meas_results_info_ies_o::value_c gnb_cu_cp_meas_results_info_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 128:
      ret.set(value_c::types::drb_meas_results_info_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_cp_meas_results_info_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 128:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_cp_meas_results_info_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::drb_meas_results_info_list:
      c = drb_meas_results_info_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_meas_results_info_ies_o::value_c");
  }
}
uint64_t& gnb_cu_cp_meas_results_info_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& gnb_cu_cp_meas_results_info_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
drb_meas_results_info_list_l& gnb_cu_cp_meas_results_info_ies_o::value_c::drb_meas_results_info_list()
{
  assert_choice_type(types::drb_meas_results_info_list, type_, "Value");
  return c.get<drb_meas_results_info_list_l>();
}
const uint64_t& gnb_cu_cp_meas_results_info_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& gnb_cu_cp_meas_results_info_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const drb_meas_results_info_list_l& gnb_cu_cp_meas_results_info_ies_o::value_c::drb_meas_results_info_list() const
{
  assert_choice_type(types::drb_meas_results_info_list, type_, "Value");
  return c.get<drb_meas_results_info_list_l>();
}
void gnb_cu_cp_meas_results_info_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::drb_meas_results_info_list:
      j.start_array("DRB-Measurement-Results-Information-List");
      for (const auto& e1 : c.get<drb_meas_results_info_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_meas_results_info_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_cp_meas_results_info_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drb_meas_results_info_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_meas_results_info_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_meas_results_info_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_meas_results_info_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drb_meas_results_info_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_meas_results_info_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_cp_meas_results_info_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_cp_meas_results_info_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "DRB-Measurement-Results-Information-List"};
  return convert_enum_idx(names, 3, value, "gnb_cu_cp_meas_results_info_ies_o::value_c::types");
}

// GNB-CU-UP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_cfg_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 1, 86};
  return map_enum_number(names, 3, idx, "id");
}
bool gnb_cu_up_cfg_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 1, 86};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_cfg_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 86:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_cfg_upd_ack_ies_o::value_c gnb_cu_up_cfg_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_cfg_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_cfg_upd_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
  }
}
uint16_t& gnb_cu_up_cfg_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& gnb_cu_up_cfg_upd_ack_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
transport_layer_address_info_s& gnb_cu_up_cfg_upd_ack_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const uint16_t& gnb_cu_up_cfg_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& gnb_cu_up_cfg_upd_ack_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const transport_layer_address_info_s& gnb_cu_up_cfg_upd_ack_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
void gnb_cu_up_cfg_upd_ack_ies_o::value_c::to_json(json_writer& j) const
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
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_cfg_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_cfg_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics", "Transport-Layer-Address-Info"};
  return convert_enum_idx(names, 3, value, "gnb_cu_up_cfg_upd_ack_ies_o::value_c::types");
}
uint8_t gnb_cu_up_cfg_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_up_cfg_upd_ack_ies_o::value_c::types");
}

// GNB-CU-UP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_cfg_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool gnb_cu_up_cfg_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_cfg_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_cfg_upd_fail_ies_o::value_c gnb_cu_up_cfg_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 12:
      ret.set(value_c::types::time_to_wait);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_cfg_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 12:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_cfg_upd_fail_ies_o::value_c::set(types::options e)
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
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_fail_ies_o::value_c");
  }
}
uint16_t& gnb_cu_up_cfg_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& gnb_cu_up_cfg_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& gnb_cu_up_cfg_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& gnb_cu_up_cfg_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& gnb_cu_up_cfg_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& gnb_cu_up_cfg_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& gnb_cu_up_cfg_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& gnb_cu_up_cfg_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void gnb_cu_up_cfg_upd_fail_ies_o::value_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_cfg_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_cfg_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "gnb_cu_up_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_up_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_up_cfg_upd_fail_ies_o::value_c::types");
}

// GNB-CU-UP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_cfg_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 7, 8, 11, 64, 73, 86, 130};
  return map_enum_number(names, 8, idx, "id");
}
bool gnb_cu_up_cfg_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 7, 8, 11, 64, 73, 86, 130};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_cfg_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 8:
      return crit_e::ignore;
    case 11:
      return crit_e::reject;
    case 64:
      return crit_e::ignore;
    case 73:
      return crit_e::reject;
    case 86:
      return crit_e::ignore;
    case 130:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_cfg_upd_ies_o::value_c gnb_cu_up_cfg_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 7:
      ret.set(value_c::types::gnb_cu_up_id);
      break;
    case 8:
      ret.set(value_c::types::gnb_cu_up_name);
      break;
    case 11:
      ret.set(value_c::types::supported_plmns);
      break;
    case 64:
      ret.set(value_c::types::gnb_cu_up_capacity);
      break;
    case 73:
      ret.set(value_c::types::gnb_cu_up_tnl_a_to_rem_list);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    case 130:
      ret.set(value_c::types::extended_gnb_cu_up_name);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_cfg_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 7:
      return presence_e::mandatory;
    case 8:
      return presence_e::optional;
    case 11:
      return presence_e::optional;
    case 64:
      return presence_e::optional;
    case 73:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    case 130:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_cfg_upd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_name:
      c = printable_string<1, 150, true, true>{};
      break;
    case types::supported_plmns:
      c = supported_plmns_list_l{};
      break;
    case types::gnb_cu_up_capacity:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_tnl_a_to_rem_list:
      c = gnb_cu_up_tnl_a_to_rem_list_l{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::extended_gnb_cu_up_name:
      c = extended_gnb_cu_up_name_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
  }
}
uint16_t& gnb_cu_up_cfg_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint64_t& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_id()
{
  assert_choice_type(types::gnb_cu_up_id, type_, "Value");
  return c.get<uint64_t>();
}
printable_string<1, 150, true, true>& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_name()
{
  assert_choice_type(types::gnb_cu_up_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
supported_plmns_list_l& gnb_cu_up_cfg_upd_ies_o::value_c::supported_plmns()
{
  assert_choice_type(types::supported_plmns, type_, "Value");
  return c.get<supported_plmns_list_l>();
}
uint16_t& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_capacity()
{
  assert_choice_type(types::gnb_cu_up_capacity, type_, "Value");
  return c.get<uint16_t>();
}
gnb_cu_up_tnl_a_to_rem_list_l& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_tnl_a_to_rem_list()
{
  assert_choice_type(types::gnb_cu_up_tnl_a_to_rem_list, type_, "Value");
  return c.get<gnb_cu_up_tnl_a_to_rem_list_l>();
}
transport_layer_address_info_s& gnb_cu_up_cfg_upd_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
extended_gnb_cu_up_name_s& gnb_cu_up_cfg_upd_ies_o::value_c::extended_gnb_cu_up_name()
{
  assert_choice_type(types::extended_gnb_cu_up_name, type_, "Value");
  return c.get<extended_gnb_cu_up_name_s>();
}
const uint16_t& gnb_cu_up_cfg_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint64_t& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_id() const
{
  assert_choice_type(types::gnb_cu_up_id, type_, "Value");
  return c.get<uint64_t>();
}
const printable_string<1, 150, true, true>& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_name() const
{
  assert_choice_type(types::gnb_cu_up_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
const supported_plmns_list_l& gnb_cu_up_cfg_upd_ies_o::value_c::supported_plmns() const
{
  assert_choice_type(types::supported_plmns, type_, "Value");
  return c.get<supported_plmns_list_l>();
}
const uint16_t& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_capacity() const
{
  assert_choice_type(types::gnb_cu_up_capacity, type_, "Value");
  return c.get<uint16_t>();
}
const gnb_cu_up_tnl_a_to_rem_list_l& gnb_cu_up_cfg_upd_ies_o::value_c::gnb_cu_up_tnl_a_to_rem_list() const
{
  assert_choice_type(types::gnb_cu_up_tnl_a_to_rem_list, type_, "Value");
  return c.get<gnb_cu_up_tnl_a_to_rem_list_l>();
}
const transport_layer_address_info_s& gnb_cu_up_cfg_upd_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const extended_gnb_cu_up_name_s& gnb_cu_up_cfg_upd_ies_o::value_c::extended_gnb_cu_up_name() const
{
  assert_choice_type(types::extended_gnb_cu_up_name, type_, "Value");
  return c.get<extended_gnb_cu_up_name_s>();
}
void gnb_cu_up_cfg_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      j.write_int("INTEGER (0..68719476735)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      j.write_str("PrintableString", c.get<printable_string<1, 150, true, true>>().to_string());
      break;
    case types::supported_plmns:
      j.start_array("SupportedPLMNs-List");
      for (const auto& e1 : c.get<supported_plmns_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_up_capacity:
      j.write_int("INTEGER (0..255)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_tnl_a_to_rem_list:
      j.start_array("GNB-CU-UP-TNLA-To-Remove-List");
      for (const auto& e1 : c.get<gnb_cu_up_tnl_a_to_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    case types::extended_gnb_cu_up_name:
      j.write_fieldname("Extended-GNB-CU-UP-Name");
      c.get<extended_gnb_cu_up_name_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_cfg_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::gnb_cu_up_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().pack(bref)));
      break;
    case types::supported_plmns:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<supported_plmns_list_l>(), 1, 12, true));
      break;
    case types::gnb_cu_up_capacity:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::gnb_cu_up_tnl_a_to_rem_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_cu_up_tnl_a_to_rem_list_l>(), 1, 32, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    case types::extended_gnb_cu_up_name:
      HANDLE_CODE(c.get<extended_gnb_cu_up_name_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_cfg_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::gnb_cu_up_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().unpack(bref)));
      break;
    case types::supported_plmns:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<supported_plmns_list_l>(), bref, 1, 12, true));
      break;
    case types::gnb_cu_up_capacity:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::gnb_cu_up_tnl_a_to_rem_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_cu_up_tnl_a_to_rem_list_l>(), bref, 1, 32, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    case types::extended_gnb_cu_up_name:
      HANDLE_CODE(c.get<extended_gnb_cu_up_name_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (0..68719476735)",
                                "PrintableString",
                                "SupportedPLMNs-List",
                                "INTEGER (0..255)",
                                "GNB-CU-UP-TNLA-To-Remove-List",
                                "Transport-Layer-Address-Info",
                                "Extended-GNB-CU-UP-Name"};
  return convert_enum_idx(names, 8, value, "gnb_cu_up_cfg_upd_ies_o::value_c::types");
}

// GNB-CU-UP-CounterCheckRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_counter_check_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 60};
  return map_enum_number(names, 3, idx, "id");
}
bool gnb_cu_up_counter_check_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 60};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_counter_check_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 60:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_counter_check_request_ies_o::value_c gnb_cu_up_counter_check_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 60:
      ret.set(value_c::types::sys_gnb_cu_up_counter_check_request);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_counter_check_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 60:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_counter_check_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::sys_gnb_cu_up_counter_check_request:
      c = sys_gnb_cu_up_counter_check_request_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_counter_check_request_ies_o::value_c");
  }
}
uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
sys_gnb_cu_up_counter_check_request_c&
gnb_cu_up_counter_check_request_ies_o::value_c::sys_gnb_cu_up_counter_check_request()
{
  assert_choice_type(types::sys_gnb_cu_up_counter_check_request, type_, "Value");
  return c.get<sys_gnb_cu_up_counter_check_request_c>();
}
const uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& gnb_cu_up_counter_check_request_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const sys_gnb_cu_up_counter_check_request_c&
gnb_cu_up_counter_check_request_ies_o::value_c::sys_gnb_cu_up_counter_check_request() const
{
  assert_choice_type(types::sys_gnb_cu_up_counter_check_request, type_, "Value");
  return c.get<sys_gnb_cu_up_counter_check_request_c>();
}
void gnb_cu_up_counter_check_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::sys_gnb_cu_up_counter_check_request:
      j.write_fieldname("System-GNB-CU-UP-CounterCheckRequest");
      c.get<sys_gnb_cu_up_counter_check_request_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_counter_check_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_counter_check_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_gnb_cu_up_counter_check_request:
      HANDLE_CODE(c.get<sys_gnb_cu_up_counter_check_request_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_counter_check_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_counter_check_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sys_gnb_cu_up_counter_check_request:
      HANDLE_CODE(c.get<sys_gnb_cu_up_counter_check_request_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_counter_check_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_counter_check_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "System-GNB-CU-UP-CounterCheckRequest"};
  return convert_enum_idx(names, 3, value, "gnb_cu_up_counter_check_request_ies_o::value_c::types");
}

// GNB-CU-UP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_e1_setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool gnb_cu_up_e1_setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_e1_setup_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_e1_setup_fail_ies_o::value_c gnb_cu_up_e1_setup_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 12:
      ret.set(value_c::types::time_to_wait);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_e1_setup_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 12:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_e1_setup_fail_ies_o::value_c::set(types::options e)
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
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_fail_ies_o::value_c");
  }
}
uint16_t& gnb_cu_up_e1_setup_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& gnb_cu_up_e1_setup_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& gnb_cu_up_e1_setup_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& gnb_cu_up_e1_setup_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& gnb_cu_up_e1_setup_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& gnb_cu_up_e1_setup_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& gnb_cu_up_e1_setup_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& gnb_cu_up_e1_setup_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void gnb_cu_up_e1_setup_fail_ies_o::value_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_e1_setup_fail_ies_o::value_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_e1_setup_fail_ies_o::value_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_e1_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "gnb_cu_up_e1_setup_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_up_e1_setup_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_up_e1_setup_fail_ies_o::value_c::types");
}

// GNB-CU-UP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_e1_setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 7, 8, 10, 11, 64, 86, 130};
  return map_enum_number(names, 8, idx, "id");
}
bool gnb_cu_up_e1_setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 7, 8, 10, 11, 64, 86, 130};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_e1_setup_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 8:
      return crit_e::ignore;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 64:
      return crit_e::ignore;
    case 86:
      return crit_e::ignore;
    case 130:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_e1_setup_request_ies_o::value_c gnb_cu_up_e1_setup_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 7:
      ret.set(value_c::types::gnb_cu_up_id);
      break;
    case 8:
      ret.set(value_c::types::gnb_cu_up_name);
      break;
    case 10:
      ret.set(value_c::types::cn_support);
      break;
    case 11:
      ret.set(value_c::types::supported_plmns);
      break;
    case 64:
      ret.set(value_c::types::gnb_cu_up_capacity);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    case 130:
      ret.set(value_c::types::extended_gnb_cu_up_name);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_e1_setup_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 7:
      return presence_e::mandatory;
    case 8:
      return presence_e::optional;
    case 10:
      return presence_e::mandatory;
    case 11:
      return presence_e::mandatory;
    case 64:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    case 130:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_e1_setup_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_name:
      c = printable_string<1, 150, true, true>{};
      break;
    case types::cn_support:
      c = cn_support_e{};
      break;
    case types::supported_plmns:
      c = supported_plmns_list_l{};
      break;
    case types::gnb_cu_up_capacity:
      c = uint16_t{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::extended_gnb_cu_up_name:
      c = extended_gnb_cu_up_name_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
  }
}
uint16_t& gnb_cu_up_e1_setup_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint64_t& gnb_cu_up_e1_setup_request_ies_o::value_c::gnb_cu_up_id()
{
  assert_choice_type(types::gnb_cu_up_id, type_, "Value");
  return c.get<uint64_t>();
}
printable_string<1, 150, true, true>& gnb_cu_up_e1_setup_request_ies_o::value_c::gnb_cu_up_name()
{
  assert_choice_type(types::gnb_cu_up_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
cn_support_e& gnb_cu_up_e1_setup_request_ies_o::value_c::cn_support()
{
  assert_choice_type(types::cn_support, type_, "Value");
  return c.get<cn_support_e>();
}
supported_plmns_list_l& gnb_cu_up_e1_setup_request_ies_o::value_c::supported_plmns()
{
  assert_choice_type(types::supported_plmns, type_, "Value");
  return c.get<supported_plmns_list_l>();
}
uint16_t& gnb_cu_up_e1_setup_request_ies_o::value_c::gnb_cu_up_capacity()
{
  assert_choice_type(types::gnb_cu_up_capacity, type_, "Value");
  return c.get<uint16_t>();
}
transport_layer_address_info_s& gnb_cu_up_e1_setup_request_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
extended_gnb_cu_up_name_s& gnb_cu_up_e1_setup_request_ies_o::value_c::extended_gnb_cu_up_name()
{
  assert_choice_type(types::extended_gnb_cu_up_name, type_, "Value");
  return c.get<extended_gnb_cu_up_name_s>();
}
const uint16_t& gnb_cu_up_e1_setup_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint64_t& gnb_cu_up_e1_setup_request_ies_o::value_c::gnb_cu_up_id() const
{
  assert_choice_type(types::gnb_cu_up_id, type_, "Value");
  return c.get<uint64_t>();
}
const printable_string<1, 150, true, true>& gnb_cu_up_e1_setup_request_ies_o::value_c::gnb_cu_up_name() const
{
  assert_choice_type(types::gnb_cu_up_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
const cn_support_e& gnb_cu_up_e1_setup_request_ies_o::value_c::cn_support() const
{
  assert_choice_type(types::cn_support, type_, "Value");
  return c.get<cn_support_e>();
}
const supported_plmns_list_l& gnb_cu_up_e1_setup_request_ies_o::value_c::supported_plmns() const
{
  assert_choice_type(types::supported_plmns, type_, "Value");
  return c.get<supported_plmns_list_l>();
}
const uint16_t& gnb_cu_up_e1_setup_request_ies_o::value_c::gnb_cu_up_capacity() const
{
  assert_choice_type(types::gnb_cu_up_capacity, type_, "Value");
  return c.get<uint16_t>();
}
const transport_layer_address_info_s& gnb_cu_up_e1_setup_request_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const extended_gnb_cu_up_name_s& gnb_cu_up_e1_setup_request_ies_o::value_c::extended_gnb_cu_up_name() const
{
  assert_choice_type(types::extended_gnb_cu_up_name, type_, "Value");
  return c.get<extended_gnb_cu_up_name_s>();
}
void gnb_cu_up_e1_setup_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_id:
      j.write_int("INTEGER (0..68719476735)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_name:
      j.write_str("PrintableString", c.get<printable_string<1, 150, true, true>>().to_string());
      break;
    case types::cn_support:
      j.write_str("CNSupport", c.get<cn_support_e>().to_string());
      break;
    case types::supported_plmns:
      j.start_array("SupportedPLMNs-List");
      for (const auto& e1 : c.get<supported_plmns_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::gnb_cu_up_capacity:
      j.write_int("INTEGER (0..255)", c.get<uint16_t>());
      break;
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    case types::extended_gnb_cu_up_name:
      j.write_fieldname("Extended-GNB-CU-UP-Name");
      c.get<extended_gnb_cu_up_name_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_e1_setup_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::gnb_cu_up_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().pack(bref)));
      break;
    case types::cn_support:
      HANDLE_CODE(c.get<cn_support_e>().pack(bref));
      break;
    case types::supported_plmns:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<supported_plmns_list_l>(), 1, 12, true));
      break;
    case types::gnb_cu_up_capacity:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    case types::extended_gnb_cu_up_name:
      HANDLE_CODE(c.get<extended_gnb_cu_up_name_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_e1_setup_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
      break;
    case types::gnb_cu_up_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().unpack(bref)));
      break;
    case types::cn_support:
      HANDLE_CODE(c.get<cn_support_e>().unpack(bref));
      break;
    case types::supported_plmns:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<supported_plmns_list_l>(), bref, 1, 12, true));
      break;
    case types::gnb_cu_up_capacity:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    case types::extended_gnb_cu_up_name:
      HANDLE_CODE(c.get<extended_gnb_cu_up_name_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_e1_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (0..68719476735)",
                                "PrintableString",
                                "CNSupport",
                                "SupportedPLMNs-List",
                                "INTEGER (0..255)",
                                "Transport-Layer-Address-Info",
                                "Extended-GNB-CU-UP-Name"};
  return convert_enum_idx(names, 8, value, "gnb_cu_up_e1_setup_request_ies_o::value_c::types");
}

// GNB-CU-UP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_e1_setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 9, 86, 129};
  return map_enum_number(names, 4, idx, "id");
}
bool gnb_cu_up_e1_setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 9, 86, 129};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_e1_setup_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 9:
      return crit_e::ignore;
    case 86:
      return crit_e::ignore;
    case 129:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_e1_setup_resp_ies_o::value_c gnb_cu_up_e1_setup_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 9:
      ret.set(value_c::types::gnb_cu_cp_name);
      break;
    case 86:
      ret.set(value_c::types::transport_layer_address_info);
      break;
    case 129:
      ret.set(value_c::types::extended_gnb_cu_cp_name);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_e1_setup_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    case 86:
      return presence_e::optional;
    case 129:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_e1_setup_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_cp_name:
      c = printable_string<1, 150, true, true>{};
      break;
    case types::transport_layer_address_info:
      c = transport_layer_address_info_s{};
      break;
    case types::extended_gnb_cu_cp_name:
      c = extended_gnb_cu_cp_name_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
  }
}
uint16_t& gnb_cu_up_e1_setup_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
printable_string<1, 150, true, true>& gnb_cu_up_e1_setup_resp_ies_o::value_c::gnb_cu_cp_name()
{
  assert_choice_type(types::gnb_cu_cp_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
transport_layer_address_info_s& gnb_cu_up_e1_setup_resp_ies_o::value_c::transport_layer_address_info()
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
extended_gnb_cu_cp_name_s& gnb_cu_up_e1_setup_resp_ies_o::value_c::extended_gnb_cu_cp_name()
{
  assert_choice_type(types::extended_gnb_cu_cp_name, type_, "Value");
  return c.get<extended_gnb_cu_cp_name_s>();
}
const uint16_t& gnb_cu_up_e1_setup_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const printable_string<1, 150, true, true>& gnb_cu_up_e1_setup_resp_ies_o::value_c::gnb_cu_cp_name() const
{
  assert_choice_type(types::gnb_cu_cp_name, type_, "Value");
  return c.get<printable_string<1, 150, true, true>>();
}
const transport_layer_address_info_s& gnb_cu_up_e1_setup_resp_ies_o::value_c::transport_layer_address_info() const
{
  assert_choice_type(types::transport_layer_address_info, type_, "Value");
  return c.get<transport_layer_address_info_s>();
}
const extended_gnb_cu_cp_name_s& gnb_cu_up_e1_setup_resp_ies_o::value_c::extended_gnb_cu_cp_name() const
{
  assert_choice_type(types::extended_gnb_cu_cp_name, type_, "Value");
  return c.get<extended_gnb_cu_cp_name_s>();
}
void gnb_cu_up_e1_setup_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_name:
      j.write_str("PrintableString", c.get<printable_string<1, 150, true, true>>().to_string());
      break;
    case types::transport_layer_address_info:
      j.write_fieldname("Transport-Layer-Address-Info");
      c.get<transport_layer_address_info_s>().to_json(j);
      break;
    case types::extended_gnb_cu_cp_name:
      j.write_fieldname("Extended-GNB-CU-CP-Name");
      c.get<extended_gnb_cu_cp_name_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_e1_setup_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().pack(bref)));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().pack(bref));
      break;
    case types::extended_gnb_cu_cp_name:
      HANDLE_CODE(c.get<extended_gnb_cu_cp_name_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_e1_setup_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true>>().unpack(bref)));
      break;
    case types::transport_layer_address_info:
      HANDLE_CODE(c.get<transport_layer_address_info_s>().unpack(bref));
      break;
    case types::extended_gnb_cu_cp_name:
      HANDLE_CODE(c.get<extended_gnb_cu_cp_name_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_e1_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_e1_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "PrintableString", "Transport-Layer-Address-Info", "Extended-GNB-CU-CP-Name"};
  return convert_enum_idx(names, 4, value, "gnb_cu_up_e1_setup_resp_ies_o::value_c::types");
}
uint8_t gnb_cu_up_e1_setup_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_up_e1_setup_resp_ies_o::value_c::types");
}

// GNB-CU-UP-StatusIndicationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t gnb_cu_up_status_ind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 65};
  return map_enum_number(names, 2, idx, "id");
}
bool gnb_cu_up_status_ind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 65};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e gnb_cu_up_status_ind_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 65:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
gnb_cu_up_status_ind_ies_o::value_c gnb_cu_up_status_ind_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 65:
      ret.set(value_c::types::gnb_cu_up_overload_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e gnb_cu_up_status_ind_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 65:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void gnb_cu_up_status_ind_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_overload_info:
      c = gnb_cu_up_overload_info_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_status_ind_ies_o::value_c");
  }
}
uint16_t& gnb_cu_up_status_ind_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
gnb_cu_up_overload_info_e& gnb_cu_up_status_ind_ies_o::value_c::gnb_cu_up_overload_info()
{
  assert_choice_type(types::gnb_cu_up_overload_info, type_, "Value");
  return c.get<gnb_cu_up_overload_info_e>();
}
const uint16_t& gnb_cu_up_status_ind_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const gnb_cu_up_overload_info_e& gnb_cu_up_status_ind_ies_o::value_c::gnb_cu_up_overload_info() const
{
  assert_choice_type(types::gnb_cu_up_overload_info, type_, "Value");
  return c.get<gnb_cu_up_overload_info_e>();
}
void gnb_cu_up_status_ind_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_overload_info:
      j.write_str("GNB-CU-UP-OverloadInformation", c.get<gnb_cu_up_overload_info_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_status_ind_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE gnb_cu_up_status_ind_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_overload_info:
      HANDLE_CODE(c.get<gnb_cu_up_overload_info_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_status_ind_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_status_ind_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_up_overload_info:
      HANDLE_CODE(c.get<gnb_cu_up_overload_info_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "gnb_cu_up_status_ind_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* gnb_cu_up_status_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "GNB-CU-UP-OverloadInformation"};
  return convert_enum_idx(names, 2, value, "gnb_cu_up_status_ind_ies_o::value_c::types");
}
uint8_t gnb_cu_up_status_ind_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_up_status_ind_ies_o::value_c::types");
}

// IAB-UPTNLAddressUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t iab_up_tnl_address_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 1, 109};
  return map_enum_number(names, 3, idx, "id");
}
bool iab_up_tnl_address_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 1, 109};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e iab_up_tnl_address_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 109:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
iab_up_tnl_address_upd_ack_ies_o::value_c iab_up_tnl_address_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 109:
      ret.set(value_c::types::ul_up_tnl_address_to_upd_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e iab_up_tnl_address_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    case 109:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void iab_up_tnl_address_upd_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::ul_up_tnl_address_to_upd_list:
      c = ul_up_tnl_address_to_upd_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ack_ies_o::value_c");
  }
}
uint16_t& iab_up_tnl_address_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& iab_up_tnl_address_upd_ack_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
ul_up_tnl_address_to_upd_list_l& iab_up_tnl_address_upd_ack_ies_o::value_c::ul_up_tnl_address_to_upd_list()
{
  assert_choice_type(types::ul_up_tnl_address_to_upd_list, type_, "Value");
  return c.get<ul_up_tnl_address_to_upd_list_l>();
}
const uint16_t& iab_up_tnl_address_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& iab_up_tnl_address_upd_ack_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ul_up_tnl_address_to_upd_list_l& iab_up_tnl_address_upd_ack_ies_o::value_c::ul_up_tnl_address_to_upd_list() const
{
  assert_choice_type(types::ul_up_tnl_address_to_upd_list, type_, "Value");
  return c.get<ul_up_tnl_address_to_upd_list_l>();
}
void iab_up_tnl_address_upd_ack_ies_o::value_c::to_json(json_writer& j) const
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
    case types::ul_up_tnl_address_to_upd_list:
      j.start_array("ULUPTNLAddressToUpdateList");
      for (const auto& e1 : c.get<ul_up_tnl_address_to_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE iab_up_tnl_address_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::ul_up_tnl_address_to_upd_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ul_up_tnl_address_to_upd_list_l>(), 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_up_tnl_address_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::ul_up_tnl_address_to_upd_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ul_up_tnl_address_to_upd_list_l>(), bref, 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* iab_up_tnl_address_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics", "ULUPTNLAddressToUpdateList"};
  return convert_enum_idx(names, 3, value, "iab_up_tnl_address_upd_ack_ies_o::value_c::types");
}
uint8_t iab_up_tnl_address_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_up_tnl_address_upd_ack_ies_o::value_c::types");
}

// IAB-UPTNLAddressUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t iab_up_tnl_address_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool iab_up_tnl_address_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 0, 12, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e iab_up_tnl_address_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
iab_up_tnl_address_upd_fail_ies_o::value_c iab_up_tnl_address_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 12:
      ret.set(value_c::types::time_to_wait);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e iab_up_tnl_address_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 12:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void iab_up_tnl_address_upd_fail_ies_o::value_c::set(types::options e)
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
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_fail_ies_o::value_c");
  }
}
uint16_t& iab_up_tnl_address_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& iab_up_tnl_address_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& iab_up_tnl_address_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& iab_up_tnl_address_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& iab_up_tnl_address_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& iab_up_tnl_address_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& iab_up_tnl_address_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& iab_up_tnl_address_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void iab_up_tnl_address_upd_fail_ies_o::value_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE iab_up_tnl_address_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_up_tnl_address_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* iab_up_tnl_address_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "iab_up_tnl_address_upd_fail_ies_o::value_c::types");
}
uint8_t iab_up_tnl_address_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_up_tnl_address_upd_fail_ies_o::value_c::types");
}

// IAB-UPTNLAddressUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t iab_up_tnl_address_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 108};
  return map_enum_number(names, 2, idx, "id");
}
bool iab_up_tnl_address_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 108};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e iab_up_tnl_address_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 108:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
iab_up_tnl_address_upd_ies_o::value_c iab_up_tnl_address_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 108:
      ret.set(value_c::types::dl_up_tnl_address_to_upd_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e iab_up_tnl_address_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 108:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void iab_up_tnl_address_upd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::dl_up_tnl_address_to_upd_list:
      c = dl_up_tnl_address_to_upd_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ies_o::value_c");
  }
}
uint16_t& iab_up_tnl_address_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
dl_up_tnl_address_to_upd_list_l& iab_up_tnl_address_upd_ies_o::value_c::dl_up_tnl_address_to_upd_list()
{
  assert_choice_type(types::dl_up_tnl_address_to_upd_list, type_, "Value");
  return c.get<dl_up_tnl_address_to_upd_list_l>();
}
const uint16_t& iab_up_tnl_address_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const dl_up_tnl_address_to_upd_list_l& iab_up_tnl_address_upd_ies_o::value_c::dl_up_tnl_address_to_upd_list() const
{
  assert_choice_type(types::dl_up_tnl_address_to_upd_list, type_, "Value");
  return c.get<dl_up_tnl_address_to_upd_list_l>();
}
void iab_up_tnl_address_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::dl_up_tnl_address_to_upd_list:
      j.start_array("DLUPTNLAddressToUpdateList");
      for (const auto& e1 : c.get<dl_up_tnl_address_to_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE iab_up_tnl_address_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::dl_up_tnl_address_to_upd_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<dl_up_tnl_address_to_upd_list_l>(), 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_up_tnl_address_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::dl_up_tnl_address_to_upd_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<dl_up_tnl_address_to_upd_list_l>(), bref, 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "iab_up_tnl_address_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* iab_up_tnl_address_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "DLUPTNLAddressToUpdateList"};
  return convert_enum_idx(names, 2, value, "iab_up_tnl_address_upd_ies_o::value_c::types");
}
uint8_t iab_up_tnl_address_upd_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_up_tnl_address_upd_ies_o::value_c::types");
}

// IABPSKNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t iabpsk_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 144};
  return map_enum_number(names, 2, idx, "id");
}
bool iabpsk_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 144};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e iabpsk_notif_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 144:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
iabpsk_notif_ies_o::value_c iabpsk_notif_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 144:
      ret.set(value_c::types::iab_donor_cu_up_psk_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e iabpsk_notif_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 144:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void iabpsk_notif_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::iab_donor_cu_up_psk_info:
      c = iab_donor_cu_up_psk_info_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "iabpsk_notif_ies_o::value_c");
  }
}
uint16_t& iabpsk_notif_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
iab_donor_cu_up_psk_info_l& iabpsk_notif_ies_o::value_c::iab_donor_cu_up_psk_info()
{
  assert_choice_type(types::iab_donor_cu_up_psk_info, type_, "Value");
  return c.get<iab_donor_cu_up_psk_info_l>();
}
const uint16_t& iabpsk_notif_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const iab_donor_cu_up_psk_info_l& iabpsk_notif_ies_o::value_c::iab_donor_cu_up_psk_info() const
{
  assert_choice_type(types::iab_donor_cu_up_psk_info, type_, "Value");
  return c.get<iab_donor_cu_up_psk_info_l>();
}
void iabpsk_notif_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::iab_donor_cu_up_psk_info:
      j.start_array("IAB-Donor-CU-UPPSKInfo");
      for (const auto& e1 : c.get<iab_donor_cu_up_psk_info_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "iabpsk_notif_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE iabpsk_notif_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::iab_donor_cu_up_psk_info:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<iab_donor_cu_up_psk_info_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "iabpsk_notif_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE iabpsk_notif_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::iab_donor_cu_up_psk_info:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<iab_donor_cu_up_psk_info_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "iabpsk_notif_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* iabpsk_notif_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "IAB-Donor-CU-UPPSKInfo"};
  return convert_enum_idx(names, 2, value, "iabpsk_notif_ies_o::value_c::types");
}
uint8_t iabpsk_notif_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iabpsk_notif_ies_o::value_c::types");
}

// MRDC-DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t mrdc_data_usage_report_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 68};
  return map_enum_number(names, 3, idx, "id");
}
bool mrdc_data_usage_report_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 68};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e mrdc_data_usage_report_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 68:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
mrdc_data_usage_report_ies_o::value_c mrdc_data_usage_report_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 68:
      ret.set(value_c::types::pdu_session_res_data_usage_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e mrdc_data_usage_report_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 68:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void mrdc_data_usage_report_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::pdu_session_res_data_usage_list:
      c = pdu_session_res_data_usage_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_data_usage_report_ies_o::value_c");
  }
}
uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
pdu_session_res_data_usage_list_l& mrdc_data_usage_report_ies_o::value_c::pdu_session_res_data_usage_list()
{
  assert_choice_type(types::pdu_session_res_data_usage_list, type_, "Value");
  return c.get<pdu_session_res_data_usage_list_l>();
}
const uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& mrdc_data_usage_report_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const pdu_session_res_data_usage_list_l& mrdc_data_usage_report_ies_o::value_c::pdu_session_res_data_usage_list() const
{
  assert_choice_type(types::pdu_session_res_data_usage_list, type_, "Value");
  return c.get<pdu_session_res_data_usage_list_l>();
}
void mrdc_data_usage_report_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::pdu_session_res_data_usage_list:
      j.start_array("PDU-Session-Resource-Data-Usage-List");
      for (const auto& e1 : c.get<pdu_session_res_data_usage_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_data_usage_report_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE mrdc_data_usage_report_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::pdu_session_res_data_usage_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_res_data_usage_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_data_usage_report_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_data_usage_report_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::pdu_session_res_data_usage_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_res_data_usage_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "mrdc_data_usage_report_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* mrdc_data_usage_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "PDU-Session-Resource-Data-Usage-List"};
  return convert_enum_idx(names, 3, value, "mrdc_data_usage_report_ies_o::value_c::types");
}

// Value ::= OPEN TYPE
void e1ap_private_ies_empty_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}
SRSASN_CODE e1ap_private_ies_empty_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1ap_private_ies_empty_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return SRSASN_SUCCESS;
}

const char* e1ap_private_ies_empty_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {};
  return convert_enum_idx(names, 0, value, "e1ap_private_ies_empty_o::value_c::types");
}

// ResetAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t reset_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 6, 1};
  return map_enum_number(names, 3, idx, "id");
}
bool reset_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 6, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e reset_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 6:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
reset_ack_ies_o::value_c reset_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 6:
      ret.set(value_c::types::ue_associated_lc_e1_conn_list_res_ack);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e reset_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 6:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void reset_ack_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::ue_associated_lc_e1_conn_list_res_ack:
      c = ue_associated_lc_e1_conn_list_res_ack_l{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ack_ies_o::value_c");
  }
}
uint16_t& reset_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ue_associated_lc_e1_conn_list_res_ack_l& reset_ack_ies_o::value_c::ue_associated_lc_e1_conn_list_res_ack()
{
  assert_choice_type(types::ue_associated_lc_e1_conn_list_res_ack, type_, "Value");
  return c.get<ue_associated_lc_e1_conn_list_res_ack_l>();
}
crit_diagnostics_s& reset_ack_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& reset_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ue_associated_lc_e1_conn_list_res_ack_l& reset_ack_ies_o::value_c::ue_associated_lc_e1_conn_list_res_ack() const
{
  assert_choice_type(types::ue_associated_lc_e1_conn_list_res_ack, type_, "Value");
  return c.get<ue_associated_lc_e1_conn_list_res_ack_l>();
}
const crit_diagnostics_s& reset_ack_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void reset_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ue_associated_lc_e1_conn_list_res_ack:
      j.start_array("UE-associatedLogicalE1-ConnectionListResAck");
      for (const auto& e1 : c.get<ue_associated_lc_e1_conn_list_res_ack_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "reset_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE reset_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ue_associated_lc_e1_conn_list_res_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_associated_lc_e1_conn_list_res_ack_l>(), 1, 65536, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ue_associated_lc_e1_conn_list_res_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_associated_lc_e1_conn_list_res_ack_l>(), bref, 1, 65536, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* reset_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "UE-associatedLogicalE1-ConnectionListResAck", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "reset_ack_ies_o::value_c::types");
}
uint8_t reset_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "reset_ack_ies_o::value_c::types");
}

// ResetIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t reset_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 0, 4};
  return map_enum_number(names, 3, idx, "id");
}
bool reset_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 0, 4};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e reset_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 4:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
reset_ies_o::value_c reset_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 4:
      ret.set(value_c::types::reset_type);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e reset_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 4:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void reset_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::reset_type:
      c = reset_type_c{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_ies_o::value_c");
  }
}
uint16_t& reset_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& reset_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
reset_type_c& reset_ies_o::value_c::reset_type()
{
  assert_choice_type(types::reset_type, type_, "Value");
  return c.get<reset_type_c>();
}
const uint16_t& reset_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& reset_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const reset_type_c& reset_ies_o::value_c::reset_type() const
{
  assert_choice_type(types::reset_type, type_, "Value");
  return c.get<reset_type_c>();
}
void reset_ies_o::value_c::to_json(json_writer& j) const
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
    case types::reset_type:
      j.write_fieldname("ResetType");
      c.get<reset_type_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "reset_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE reset_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::reset_type:
      HANDLE_CODE(c.get<reset_type_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::reset_type:
      HANDLE_CODE(c.get<reset_type_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* reset_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "ResetType"};
  return convert_enum_idx(names, 3, value, "reset_ies_o::value_c::types");
}
uint8_t reset_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "reset_ies_o::value_c::types");
}

// ResourceStatusFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t res_status_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 89, 90, 0, 1};
  return map_enum_number(names, 5, idx, "id");
}
bool res_status_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 89, 90, 0, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e res_status_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 89:
      return crit_e::reject;
    case 90:
      return crit_e::ignore;
    case 0:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
res_status_fail_ies_o::value_c res_status_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 89:
      ret.set(value_c::types::gnb_cu_cp_meas_id);
      break;
    case 90:
      ret.set(value_c::types::gnb_cu_up_meas_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e res_status_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 89:
      return presence_e::mandatory;
    case 90:
      return presence_e::optional;
    case 0:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void res_status_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_cp_meas_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_meas_id:
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
      log_invalid_choice_id(type_, "res_status_fail_ies_o::value_c");
  }
}
uint16_t& res_status_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_fail_ies_o::value_c::gnb_cu_cp_meas_id()
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_fail_ies_o::value_c::gnb_cu_up_meas_id()
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& res_status_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& res_status_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& res_status_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_fail_ies_o::value_c::gnb_cu_cp_meas_id() const
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_fail_ies_o::value_c::gnb_cu_up_meas_id() const
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& res_status_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& res_status_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void res_status_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
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
      log_invalid_choice_id(type_, "res_status_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE res_status_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* res_status_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "INTEGER (1..4095,...)", "INTEGER (1..4095,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "res_status_fail_ies_o::value_c::types");
}

// ResourceStatusRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t res_status_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 89, 90, 91, 92, 93};
  return map_enum_number(names, 6, idx, "id");
}
bool res_status_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 89, 90, 91, 92, 93};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e res_status_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 89:
      return crit_e::reject;
    case 90:
      return crit_e::ignore;
    case 91:
      return crit_e::reject;
    case 92:
      return crit_e::reject;
    case 93:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
res_status_request_ies_o::value_c res_status_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 89:
      ret.set(value_c::types::gnb_cu_cp_meas_id);
      break;
    case 90:
      ret.set(value_c::types::gnb_cu_up_meas_id);
      break;
    case 91:
      ret.set(value_c::types::regist_request);
      break;
    case 92:
      ret.set(value_c::types::report_characteristics);
      break;
    case 93:
      ret.set(value_c::types::report_periodicity);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e res_status_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 89:
      return presence_e::mandatory;
    case 90:
      return presence_e::optional;
    case 91:
      return presence_e::mandatory;
    case 92:
      return presence_e::conditional;
    case 93:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void res_status_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_cp_meas_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_meas_id:
      c = uint16_t{};
      break;
    case types::regist_request:
      c = regist_request_e{};
      break;
    case types::report_characteristics:
      c = fixed_bitstring<36, false, true>{};
      break;
    case types::report_periodicity:
      c = report_periodicity_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_status_request_ies_o::value_c");
  }
}
uint16_t& res_status_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_request_ies_o::value_c::gnb_cu_cp_meas_id()
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_request_ies_o::value_c::gnb_cu_up_meas_id()
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
regist_request_e& res_status_request_ies_o::value_c::regist_request()
{
  assert_choice_type(types::regist_request, type_, "Value");
  return c.get<regist_request_e>();
}
fixed_bitstring<36, false, true>& res_status_request_ies_o::value_c::report_characteristics()
{
  assert_choice_type(types::report_characteristics, type_, "Value");
  return c.get<fixed_bitstring<36, false, true>>();
}
report_periodicity_e& res_status_request_ies_o::value_c::report_periodicity()
{
  assert_choice_type(types::report_periodicity, type_, "Value");
  return c.get<report_periodicity_e>();
}
const uint16_t& res_status_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_request_ies_o::value_c::gnb_cu_cp_meas_id() const
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_request_ies_o::value_c::gnb_cu_up_meas_id() const
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const regist_request_e& res_status_request_ies_o::value_c::regist_request() const
{
  assert_choice_type(types::regist_request, type_, "Value");
  return c.get<regist_request_e>();
}
const fixed_bitstring<36, false, true>& res_status_request_ies_o::value_c::report_characteristics() const
{
  assert_choice_type(types::report_characteristics, type_, "Value");
  return c.get<fixed_bitstring<36, false, true>>();
}
const report_periodicity_e& res_status_request_ies_o::value_c::report_periodicity() const
{
  assert_choice_type(types::report_periodicity, type_, "Value");
  return c.get<report_periodicity_e>();
}
void res_status_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
      break;
    case types::regist_request:
      j.write_str("RegistrationRequest", c.get<regist_request_e>().to_string());
      break;
    case types::report_characteristics:
      j.write_str("BIT STRING", c.get<fixed_bitstring<36, false, true>>().to_string());
      break;
    case types::report_periodicity:
      j.write_str("ReportingPeriodicity", c.get<report_periodicity_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "res_status_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE res_status_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::regist_request:
      HANDLE_CODE(c.get<regist_request_e>().pack(bref));
      break;
    case types::report_characteristics:
      HANDLE_CODE((c.get<fixed_bitstring<36, false, true>>().pack(bref)));
      break;
    case types::report_periodicity:
      HANDLE_CODE(c.get<report_periodicity_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::regist_request:
      HANDLE_CODE(c.get<regist_request_e>().unpack(bref));
      break;
    case types::report_characteristics:
      HANDLE_CODE((c.get<fixed_bitstring<36, false, true>>().unpack(bref)));
      break;
    case types::report_periodicity:
      HANDLE_CODE(c.get<report_periodicity_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* res_status_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (1..4095,...)",
                                "INTEGER (1..4095,...)",
                                "RegistrationRequest",
                                "BIT STRING",
                                "ReportingPeriodicity"};
  return convert_enum_idx(names, 6, value, "res_status_request_ies_o::value_c::types");
}

// ResourceStatusResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t res_status_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 89, 90, 1};
  return map_enum_number(names, 4, idx, "id");
}
bool res_status_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 89, 90, 1};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e res_status_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 89:
      return crit_e::reject;
    case 90:
      return crit_e::ignore;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
res_status_resp_ies_o::value_c res_status_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 89:
      ret.set(value_c::types::gnb_cu_cp_meas_id);
      break;
    case 90:
      ret.set(value_c::types::gnb_cu_up_meas_id);
      break;
    case 1:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e res_status_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 89:
      return presence_e::mandatory;
    case 90:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void res_status_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_cp_meas_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_meas_id:
      c = uint16_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_status_resp_ies_o::value_c");
  }
}
uint16_t& res_status_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_resp_ies_o::value_c::gnb_cu_cp_meas_id()
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_resp_ies_o::value_c::gnb_cu_up_meas_id()
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& res_status_resp_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& res_status_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_resp_ies_o::value_c::gnb_cu_cp_meas_id() const
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_resp_ies_o::value_c::gnb_cu_up_meas_id() const
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& res_status_resp_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void res_status_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "res_status_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE res_status_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* res_status_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "INTEGER (1..4095,...)", "INTEGER (1..4095,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "res_status_resp_ies_o::value_c::types");
}

// ResourceStatusUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t res_status_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {57, 89, 90, 94, 95};
  return map_enum_number(names, 5, idx, "id");
}
bool res_status_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {57, 89, 90, 94, 95};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e res_status_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 57:
      return crit_e::reject;
    case 89:
      return crit_e::reject;
    case 90:
      return crit_e::ignore;
    case 94:
      return crit_e::ignore;
    case 95:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
res_status_upd_ies_o::value_c res_status_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 57:
      ret.set(value_c::types::transaction_id);
      break;
    case 89:
      ret.set(value_c::types::gnb_cu_cp_meas_id);
      break;
    case 90:
      ret.set(value_c::types::gnb_cu_up_meas_id);
      break;
    case 94:
      ret.set(value_c::types::tnl_available_capacity_ind);
      break;
    case 95:
      ret.set(value_c::types::hw_capacity_ind);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e res_status_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 57:
      return presence_e::mandatory;
    case 89:
      return presence_e::mandatory;
    case 90:
      return presence_e::optional;
    case 94:
      return presence_e::optional;
    case 95:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void res_status_upd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_cp_meas_id:
      c = uint16_t{};
      break;
    case types::gnb_cu_up_meas_id:
      c = uint16_t{};
      break;
    case types::tnl_available_capacity_ind:
      c = tnl_available_capacity_ind_s{};
      break;
    case types::hw_capacity_ind:
      c = hw_capacity_ind_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "res_status_upd_ies_o::value_c");
  }
}
uint16_t& res_status_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_upd_ies_o::value_c::gnb_cu_cp_meas_id()
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& res_status_upd_ies_o::value_c::gnb_cu_up_meas_id()
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
tnl_available_capacity_ind_s& res_status_upd_ies_o::value_c::tnl_available_capacity_ind()
{
  assert_choice_type(types::tnl_available_capacity_ind, type_, "Value");
  return c.get<tnl_available_capacity_ind_s>();
}
hw_capacity_ind_s& res_status_upd_ies_o::value_c::hw_capacity_ind()
{
  assert_choice_type(types::hw_capacity_ind, type_, "Value");
  return c.get<hw_capacity_ind_s>();
}
const uint16_t& res_status_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_upd_ies_o::value_c::gnb_cu_cp_meas_id() const
{
  assert_choice_type(types::gnb_cu_cp_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& res_status_upd_ies_o::value_c::gnb_cu_up_meas_id() const
{
  assert_choice_type(types::gnb_cu_up_meas_id, type_, "Value");
  return c.get<uint16_t>();
}
const tnl_available_capacity_ind_s& res_status_upd_ies_o::value_c::tnl_available_capacity_ind() const
{
  assert_choice_type(types::tnl_available_capacity_ind, type_, "Value");
  return c.get<tnl_available_capacity_ind_s>();
}
const hw_capacity_ind_s& res_status_upd_ies_o::value_c::hw_capacity_ind() const
{
  assert_choice_type(types::hw_capacity_ind, type_, "Value");
  return c.get<hw_capacity_ind_s>();
}
void res_status_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_cp_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
      break;
    case types::gnb_cu_up_meas_id:
      j.write_int("INTEGER (1..4095,...)", c.get<uint16_t>());
      break;
    case types::tnl_available_capacity_ind:
      j.write_fieldname("TNL-AvailableCapacityIndicator");
      c.get<tnl_available_capacity_ind_s>().to_json(j);
      break;
    case types::hw_capacity_ind:
      j.write_fieldname("HW-CapacityIndicator");
      c.get<hw_capacity_ind_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "res_status_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE res_status_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::tnl_available_capacity_ind:
      HANDLE_CODE(c.get<tnl_available_capacity_ind_s>().pack(bref));
      break;
    case types::hw_capacity_ind:
      HANDLE_CODE(c.get<hw_capacity_ind_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::gnb_cu_cp_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::gnb_cu_up_meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)1u, (uint16_t)4095u, true, true));
      break;
    case types::tnl_available_capacity_ind:
      HANDLE_CODE(c.get<tnl_available_capacity_ind_s>().unpack(bref));
      break;
    case types::hw_capacity_ind:
      HANDLE_CODE(c.get<hw_capacity_ind_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "res_status_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* res_status_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (1..4095,...)",
                                "INTEGER (1..4095,...)",
                                "TNL-AvailableCapacityIndicator",
                                "HW-CapacityIndicator"};
  return convert_enum_idx(names, 5, value, "res_status_upd_ies_o::value_c::types");
}

// TraceStartIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t trace_start_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 81};
  return map_enum_number(names, 3, idx, "id");
}
bool trace_start_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 81};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e trace_start_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 81:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
trace_start_ies_o::value_c trace_start_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 81:
      ret.set(value_c::types::trace_activation);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e trace_start_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 81:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void trace_start_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::trace_activation:
      c = trace_activation_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "trace_start_ies_o::value_c");
  }
}
uint64_t& trace_start_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& trace_start_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
trace_activation_s& trace_start_ies_o::value_c::trace_activation()
{
  assert_choice_type(types::trace_activation, type_, "Value");
  return c.get<trace_activation_s>();
}
const uint64_t& trace_start_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& trace_start_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const trace_activation_s& trace_start_ies_o::value_c::trace_activation() const
{
  assert_choice_type(types::trace_activation, type_, "Value");
  return c.get<trace_activation_s>();
}
void trace_start_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::trace_activation:
      j.write_fieldname("TraceActivation");
      c.get<trace_activation_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "trace_start_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE trace_start_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::trace_activation:
      HANDLE_CODE(c.get<trace_activation_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "trace_start_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE trace_start_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::trace_activation:
      HANDLE_CODE(c.get<trace_activation_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "trace_start_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* trace_start_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "TraceActivation"};
  return convert_enum_idx(names, 3, value, "trace_start_ies_o::value_c::types");
}

// ULDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
uint32_t ul_data_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {2, 3, 67};
  return map_enum_number(names, 3, idx, "id");
}
bool ul_data_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {2, 3, 67};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ul_data_notif_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 2:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 67:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ul_data_notif_ies_o::value_c ul_data_notif_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 2:
      ret.set(value_c::types::gnb_cu_cp_ue_e1ap_id);
      break;
    case 3:
      ret.set(value_c::types::gnb_cu_up_ue_e1ap_id);
      break;
    case 67:
      ret.set(value_c::types::pdu_session_to_notify_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ul_data_notif_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 2:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 67:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ul_data_notif_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      c = uint64_t{};
      break;
    case types::pdu_session_to_notify_list:
      c = pdu_session_to_notify_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_notif_ies_o::value_c");
  }
}
uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_up_ue_e1ap_id()
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
pdu_session_to_notify_list_l& ul_data_notif_ies_o::value_c::pdu_session_to_notify_list()
{
  assert_choice_type(types::pdu_session_to_notify_list, type_, "Value");
  return c.get<pdu_session_to_notify_list_l>();
}
const uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_cp_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_cp_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ul_data_notif_ies_o::value_c::gnb_cu_up_ue_e1ap_id() const
{
  assert_choice_type(types::gnb_cu_up_ue_e1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const pdu_session_to_notify_list_l& ul_data_notif_ies_o::value_c::pdu_session_to_notify_list() const
{
  assert_choice_type(types::pdu_session_to_notify_list, type_, "Value");
  return c.get<pdu_session_to_notify_list_l>();
}
void ul_data_notif_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::pdu_session_to_notify_list:
      j.start_array("PDU-Session-To-Notify-List");
      for (const auto& e1 : c.get<pdu_session_to_notify_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_notif_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_data_notif_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::pdu_session_to_notify_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pdu_session_to_notify_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_notif_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_data_notif_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_cp_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_cu_up_ue_e1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::pdu_session_to_notify_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pdu_session_to_notify_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ul_data_notif_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ul_data_notif_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "PDU-Session-To-Notify-List"};
  return convert_enum_idx(names, 3, value, "ul_data_notif_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e1_release_request_ies_o>;

SRSASN_CODE e1_release_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e1_release_request_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 0: {
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
void e1_release_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<early_forwarding_sn_transfer_ies_o>;

SRSASN_CODE early_forwarding_sn_transfer_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)119, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_subject_to_early_forwarding_list, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE early_forwarding_sn_transfer_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 119: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_subject_to_early_forwarding_list, bref, 1, 32, true));
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
void early_forwarding_sn_transfer_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 119);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : drbs_subject_to_early_forwarding_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<error_ind_ies_o>;

SRSASN_CODE error_ind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_ue_e1ap_id_present ? 1 : 0;
  nof_ies += gnb_cu_up_ue_e1ap_id_present ? 1 : 0;
  nof_ies += cause_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (gnb_cu_cp_ue_e1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (cause_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
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

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 2: {
        gnb_cu_cp_ue_e1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        gnb_cu_up_ue_e1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        cause_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 1: {
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
void error_ind_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (gnb_cu_cp_ue_e1ap_id_present) {
    j.write_int("id", 2);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  }
  if (gnb_cu_up_ue_e1ap_id_present) {
    j.write_int("id", 3);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  }
  if (cause_present) {
    j.write_int("id", 0);
    j.write_str("criticality", "ignore");
    cause.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_cfg_upd_ies_o>;

SRSASN_CODE gnb_cu_cp_cfg_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_name_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnl_a_to_add_list_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnl_a_to_rem_list_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnl_a_to_upd_list_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  nof_ies += extended_gnb_cu_cp_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (gnb_cu_cp_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_cu_cp_name.pack(bref));
  }
  if (gnb_cu_cp_tnl_a_to_add_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)27, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_cp_tnl_a_to_add_list, 1, 32, true));
  }
  if (gnb_cu_cp_tnl_a_to_rem_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)28, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_cp_tnl_a_to_rem_list, 1, 32, true));
  }
  if (gnb_cu_cp_tnl_a_to_upd_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_cp_tnl_a_to_upd_list, 1, 32, true));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }
  if (extended_gnb_cu_cp_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)129, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(extended_gnb_cu_cp_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 9: {
        gnb_cu_cp_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_name.unpack(bref));
        break;
      }
      case 27: {
        gnb_cu_cp_tnl_a_to_add_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_cp_tnl_a_to_add_list, bref, 1, 32, true));
        break;
      }
      case 28: {
        gnb_cu_cp_tnl_a_to_rem_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_cp_tnl_a_to_rem_list, bref, 1, 32, true));
        break;
      }
      case 29: {
        gnb_cu_cp_tnl_a_to_upd_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_cp_tnl_a_to_upd_list, bref, 1, 32, true));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
        break;
      }
      case 129: {
        extended_gnb_cu_cp_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(extended_gnb_cu_cp_name.unpack(bref));
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
void gnb_cu_cp_cfg_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (gnb_cu_cp_name_present) {
    j.write_int("id", 9);
    j.write_str("criticality", "ignore");
    j.write_str("Value", gnb_cu_cp_name.to_string());
  }
  if (gnb_cu_cp_tnl_a_to_add_list_present) {
    j.write_int("id", 27);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : gnb_cu_cp_tnl_a_to_add_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (gnb_cu_cp_tnl_a_to_rem_list_present) {
    j.write_int("id", 28);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : gnb_cu_cp_tnl_a_to_rem_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (gnb_cu_cp_tnl_a_to_upd_list_present) {
    j.write_int("id", 29);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : gnb_cu_cp_tnl_a_to_upd_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  if (extended_gnb_cu_cp_name_present) {
    j.write_int("id", 129);
    j.write_str("criticality", "ignore");
    extended_gnb_cu_cp_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_cfg_upd_ack_ies_o>;

SRSASN_CODE gnb_cu_cp_cfg_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnl_a_setup_list_present ? 1 : 0;
  nof_ies += gnb_cu_cp_tnl_a_failed_to_setup_list_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (gnb_cu_cp_tnl_a_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)30, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_cp_tnl_a_setup_list, 1, 32, true));
  }
  if (gnb_cu_cp_tnl_a_failed_to_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)31, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_cp_tnl_a_failed_to_setup_list, 1, 32, true));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_cfg_upd_ack_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 30: {
        gnb_cu_cp_tnl_a_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_cp_tnl_a_setup_list, bref, 1, 32, true));
        break;
      }
      case 31: {
        gnb_cu_cp_tnl_a_failed_to_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_cp_tnl_a_failed_to_setup_list, bref, 1, 32, true));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
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
void gnb_cu_cp_cfg_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (gnb_cu_cp_tnl_a_setup_list_present) {
    j.write_int("id", 30);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : gnb_cu_cp_tnl_a_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (gnb_cu_cp_tnl_a_failed_to_setup_list_present) {
    j.write_int("id", 31);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : gnb_cu_cp_tnl_a_failed_to_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_cfg_upd_fail_ies_o>;

SRSASN_CODE gnb_cu_cp_cfg_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)12, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_cfg_upd_fail_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 12: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_cp_cfg_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 12);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_e1_setup_fail_ies_o>;

SRSASN_CODE gnb_cu_cp_e1_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)12, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_e1_setup_fail_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 12: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_cp_e1_setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 12);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_e1_setup_request_ies_o>;

SRSASN_CODE gnb_cu_cp_e1_setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_name_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  nof_ies += extended_gnb_cu_cp_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (gnb_cu_cp_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_cu_cp_name.pack(bref));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }
  if (extended_gnb_cu_cp_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)129, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(extended_gnb_cu_cp_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_e1_setup_request_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 9: {
        gnb_cu_cp_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_name.unpack(bref));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
        break;
      }
      case 129: {
        extended_gnb_cu_cp_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(extended_gnb_cu_cp_name.unpack(bref));
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
void gnb_cu_cp_e1_setup_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (gnb_cu_cp_name_present) {
    j.write_int("id", 9);
    j.write_str("criticality", "ignore");
    j.write_str("Value", gnb_cu_cp_name.to_string());
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  if (extended_gnb_cu_cp_name_present) {
    j.write_int("id", 129);
    j.write_str("criticality", "ignore");
    extended_gnb_cu_cp_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_e1_setup_resp_ies_o>;

SRSASN_CODE gnb_cu_cp_e1_setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += gnb_cu_up_name_present ? 1 : 0;
  nof_ies += gnb_cu_up_capacity_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  nof_ies += extended_gnb_cu_up_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_cu_up_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)8, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_cu_up_name.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)10, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cn_support.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)11, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_plmns, 1, 12, true));
  }
  if (gnb_cu_up_capacity_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)64, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_capacity, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }
  if (extended_gnb_cu_up_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)130, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(extended_gnb_cu_up_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_e1_setup_resp_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 7: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
        break;
      }
      case 8: {
        gnb_cu_up_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_name.unpack(bref));
        break;
      }
      case 10: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cn_support.unpack(bref));
        break;
      }
      case 11: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(supported_plmns, bref, 1, 12, true));
        break;
      }
      case 64: {
        gnb_cu_up_capacity_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_capacity, bref, (uint16_t)0u, (uint16_t)255u, false, true));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
        break;
      }
      case 130: {
        extended_gnb_cu_up_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(extended_gnb_cu_up_name.unpack(bref));
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
void gnb_cu_cp_e1_setup_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 7);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_id);
  if (gnb_cu_up_name_present) {
    j.write_int("id", 8);
    j.write_str("criticality", "ignore");
    j.write_str("Value", gnb_cu_up_name.to_string());
  }
  j.write_int("id", 10);
  j.write_str("criticality", "reject");
  j.write_str("Value", cn_support.to_string());
  j.write_int("id", 11);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : supported_plmns) {
    e1.to_json(j);
  }
  j.end_array();
  if (gnb_cu_up_capacity_present) {
    j.write_int("id", 64);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_capacity);
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  if (extended_gnb_cu_up_name_present) {
    j.write_int("id", 130);
    j.write_str("criticality", "ignore");
    extended_gnb_cu_up_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_cp_meas_results_info_ies_o>;

SRSASN_CODE gnb_cu_cp_meas_results_info_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)128, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_meas_results_info_list, 1, 32, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_cp_meas_results_info_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 128: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_meas_results_info_list, bref, 1, 32, true));
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
void gnb_cu_cp_meas_results_info_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 128);
  j.write_str("criticality", "ignore");
  j.start_array("Value");
  for (const auto& e1 : drb_meas_results_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_cfg_upd_ies_o>;

SRSASN_CODE gnb_cu_up_cfg_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += gnb_cu_up_name_present ? 1 : 0;
  nof_ies += supported_plmns_present ? 1 : 0;
  nof_ies += gnb_cu_up_capacity_present ? 1 : 0;
  nof_ies += gnb_cu_up_tnl_a_to_rem_list_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  nof_ies += extended_gnb_cu_up_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_cu_up_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)8, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_cu_up_name.pack(bref));
  }
  if (supported_plmns_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)11, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_plmns, 1, 12, true));
  }
  if (gnb_cu_up_capacity_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)64, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_capacity, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (gnb_cu_up_tnl_a_to_rem_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)73, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_up_tnl_a_to_rem_list, 1, 32, true));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }
  if (extended_gnb_cu_up_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)130, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(extended_gnb_cu_up_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_cfg_upd_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 7: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
        break;
      }
      case 8: {
        gnb_cu_up_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_name.unpack(bref));
        break;
      }
      case 11: {
        supported_plmns_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(supported_plmns, bref, 1, 12, true));
        break;
      }
      case 64: {
        gnb_cu_up_capacity_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_capacity, bref, (uint16_t)0u, (uint16_t)255u, false, true));
        break;
      }
      case 73: {
        gnb_cu_up_tnl_a_to_rem_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_up_tnl_a_to_rem_list, bref, 1, 32, true));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
        break;
      }
      case 130: {
        extended_gnb_cu_up_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(extended_gnb_cu_up_name.unpack(bref));
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
void gnb_cu_up_cfg_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 7);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_id);
  if (gnb_cu_up_name_present) {
    j.write_int("id", 8);
    j.write_str("criticality", "ignore");
    j.write_str("Value", gnb_cu_up_name.to_string());
  }
  if (supported_plmns_present) {
    j.write_int("id", 11);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : supported_plmns) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (gnb_cu_up_capacity_present) {
    j.write_int("id", 64);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_capacity);
  }
  if (gnb_cu_up_tnl_a_to_rem_list_present) {
    j.write_int("id", 73);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : gnb_cu_up_tnl_a_to_rem_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  if (extended_gnb_cu_up_name_present) {
    j.write_int("id", 130);
    j.write_str("criticality", "ignore");
    extended_gnb_cu_up_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_cfg_upd_ack_ies_o>;

SRSASN_CODE gnb_cu_up_cfg_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_cfg_upd_ack_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
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
void gnb_cu_up_cfg_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_cfg_upd_fail_ies_o>;

SRSASN_CODE gnb_cu_up_cfg_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)12, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_cfg_upd_fail_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 12: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_up_cfg_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 12);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_counter_check_request_ies_o>;

SRSASN_CODE gnb_cu_up_counter_check_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)60, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sys_gnb_cu_up_counter_check_request.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_counter_check_request_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 60: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sys_gnb_cu_up_counter_check_request.unpack(bref));
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
void gnb_cu_up_counter_check_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 60);
  j.write_str("criticality", "reject");
  sys_gnb_cu_up_counter_check_request.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_e1_setup_fail_ies_o>;

SRSASN_CODE gnb_cu_up_e1_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)12, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_e1_setup_fail_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 12: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 1: {
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
void gnb_cu_up_e1_setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 12);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_e1_setup_request_ies_o>;

SRSASN_CODE gnb_cu_up_e1_setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += gnb_cu_up_name_present ? 1 : 0;
  nof_ies += gnb_cu_up_capacity_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  nof_ies += extended_gnb_cu_up_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_cu_up_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)8, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_cu_up_name.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)10, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cn_support.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)11, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, supported_plmns, 1, 12, true));
  }
  if (gnb_cu_up_capacity_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)64, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_capacity, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }
  if (extended_gnb_cu_up_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)130, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(extended_gnb_cu_up_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_e1_setup_request_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 7: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
        break;
      }
      case 8: {
        gnb_cu_up_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_name.unpack(bref));
        break;
      }
      case 10: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cn_support.unpack(bref));
        break;
      }
      case 11: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(supported_plmns, bref, 1, 12, true));
        break;
      }
      case 64: {
        gnb_cu_up_capacity_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_capacity, bref, (uint16_t)0u, (uint16_t)255u, false, true));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
        break;
      }
      case 130: {
        extended_gnb_cu_up_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(extended_gnb_cu_up_name.unpack(bref));
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
void gnb_cu_up_e1_setup_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 7);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_id);
  if (gnb_cu_up_name_present) {
    j.write_int("id", 8);
    j.write_str("criticality", "ignore");
    j.write_str("Value", gnb_cu_up_name.to_string());
  }
  j.write_int("id", 10);
  j.write_str("criticality", "reject");
  j.write_str("Value", cn_support.to_string());
  j.write_int("id", 11);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : supported_plmns) {
    e1.to_json(j);
  }
  j.end_array();
  if (gnb_cu_up_capacity_present) {
    j.write_int("id", 64);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_capacity);
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  if (extended_gnb_cu_up_name_present) {
    j.write_int("id", 130);
    j.write_str("criticality", "ignore");
    extended_gnb_cu_up_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_e1_setup_resp_ies_o>;

SRSASN_CODE gnb_cu_up_e1_setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += gnb_cu_cp_name_present ? 1 : 0;
  nof_ies += transport_layer_address_info_present ? 1 : 0;
  nof_ies += extended_gnb_cu_cp_name_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (gnb_cu_cp_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_cu_cp_name.pack(bref));
  }
  if (transport_layer_address_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)86, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(transport_layer_address_info.pack(bref));
  }
  if (extended_gnb_cu_cp_name_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)129, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(extended_gnb_cu_cp_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_e1_setup_resp_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 9: {
        gnb_cu_cp_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_cp_name.unpack(bref));
        break;
      }
      case 86: {
        transport_layer_address_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transport_layer_address_info.unpack(bref));
        break;
      }
      case 129: {
        extended_gnb_cu_cp_name_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(extended_gnb_cu_cp_name.unpack(bref));
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
void gnb_cu_up_e1_setup_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (gnb_cu_cp_name_present) {
    j.write_int("id", 9);
    j.write_str("criticality", "ignore");
    j.write_str("Value", gnb_cu_cp_name.to_string());
  }
  if (transport_layer_address_info_present) {
    j.write_int("id", 86);
    j.write_str("criticality", "ignore");
    transport_layer_address_info.to_json(j);
  }
  if (extended_gnb_cu_cp_name_present) {
    j.write_int("id", 129);
    j.write_str("criticality", "ignore");
    extended_gnb_cu_cp_name.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<gnb_cu_up_status_ind_ies_o>;

SRSASN_CODE gnb_cu_up_status_ind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)65, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_cu_up_overload_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_cu_up_status_ind_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 65: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_up_overload_info.unpack(bref));
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
void gnb_cu_up_status_ind_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 65);
  j.write_str("criticality", "reject");
  j.write_str("Value", gnb_cu_up_overload_info.to_string());
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<iab_up_tnl_address_upd_ies_o>;

SRSASN_CODE iab_up_tnl_address_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += dl_up_tnl_address_to_upd_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (dl_up_tnl_address_to_upd_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)108, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, dl_up_tnl_address_to_upd_list, 1, 8, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_up_tnl_address_upd_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 108: {
        dl_up_tnl_address_to_upd_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(dl_up_tnl_address_to_upd_list, bref, 1, 8, true));
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
void iab_up_tnl_address_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (dl_up_tnl_address_to_upd_list_present) {
    j.write_int("id", 108);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : dl_up_tnl_address_to_upd_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<iab_up_tnl_address_upd_ack_ies_o>;

SRSASN_CODE iab_up_tnl_address_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += ul_up_tnl_address_to_upd_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (ul_up_tnl_address_to_upd_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)109, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ul_up_tnl_address_to_upd_list, 1, 8, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_up_tnl_address_upd_ack_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 1: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 109: {
        ul_up_tnl_address_to_upd_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ul_up_tnl_address_to_upd_list, bref, 1, 8, true));
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
void iab_up_tnl_address_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (ul_up_tnl_address_to_upd_list_present) {
    j.write_int("id", 109);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ul_up_tnl_address_to_upd_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<iab_up_tnl_address_upd_fail_ies_o>;

SRSASN_CODE iab_up_tnl_address_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)12, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iab_up_tnl_address_upd_fail_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 12: {
        time_to_wait_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.unpack(bref));
        break;
      }
      case 1: {
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
void iab_up_tnl_address_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_int("id", 12);
    j.write_str("criticality", "ignore");
    j.write_str("Value", time_to_wait.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<iabpsk_notif_ies_o>;

SRSASN_CODE iabpsk_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)144, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, iab_donor_cu_up_psk_info, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE iabpsk_notif_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 144: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(iab_donor_cu_up_psk_info, bref, 1, 256, true));
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
void iabpsk_notif_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 144);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : iab_donor_cu_up_psk_info) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<mrdc_data_usage_report_ies_o>;

SRSASN_CODE mrdc_data_usage_report_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)68, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_res_data_usage_list, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mrdc_data_usage_report_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 68: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_res_data_usage_list, bref, 1, 256, true));
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
void mrdc_data_usage_report_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 68);
  j.write_str("criticality", "ignore");
  j.start_array("Value");
  for (const auto& e1 : pdu_session_res_data_usage_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

SRSASN_CODE private_ie_container_empty_l::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  pack_length(bref, nof_ies, 1u, 65535u, true);

  return SRSASN_SUCCESS;
}
SRSASN_CODE private_ie_container_empty_l::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 1u, 65535u, true);
  if (nof_ies > 0) {
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void private_ie_container_empty_l::to_json(json_writer& j) const
{
  j.start_obj();
  j.end_obj();
}

// PrivateMessage ::= SEQUENCE
SRSASN_CODE private_msg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(private_ies.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE private_msg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(private_ies.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void private_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("PrivateMessage");
  j.write_fieldname("privateIEs");
  private_ies.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

template struct asn1::protocol_ie_field_s<reset_ies_o>;

SRSASN_CODE reset_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)4, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(reset_type.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 4: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(reset_type.unpack(bref));
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
void reset_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  j.write_int("id", 4);
  j.write_str("criticality", "reject");
  reset_type.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<reset_ack_ies_o>;

SRSASN_CODE reset_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ue_associated_lc_e1_conn_list_res_ack_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ue_associated_lc_e1_conn_list_res_ack_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)6, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_associated_lc_e1_conn_list_res_ack, 1, 65536, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_ack_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 6: {
        ue_associated_lc_e1_conn_list_res_ack_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_associated_lc_e1_conn_list_res_ack, bref, 1, 65536, true));
        break;
      }
      case 1: {
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
void reset_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  if (ue_associated_lc_e1_conn_list_res_ack_present) {
    j.write_int("id", 6);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : ue_associated_lc_e1_conn_list_res_ack) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<res_status_fail_ies_o>;

SRSASN_CODE res_status_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += gnb_cu_up_meas_id_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)89, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  if (gnb_cu_up_meas_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)90, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_fail_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 89: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 90: {
        gnb_cu_up_meas_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 1: {
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
void res_status_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 89);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_meas_id);
  if (gnb_cu_up_meas_id_present) {
    j.write_int("id", 90);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_meas_id);
  }
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<res_status_request_ies_o>;

SRSASN_CODE res_status_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += gnb_cu_up_meas_id_present ? 1 : 0;
  nof_ies += report_characteristics_present ? 1 : 0;
  nof_ies += report_periodicity_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)89, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  if (gnb_cu_up_meas_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)90, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)91, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(regist_request.pack(bref));
  }
  if (report_characteristics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)92, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(report_characteristics.pack(bref));
  }
  if (report_periodicity_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)93, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(report_periodicity.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_request_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 89: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 90: {
        gnb_cu_up_meas_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 91: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(regist_request.unpack(bref));
        break;
      }
      case 92: {
        report_characteristics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(report_characteristics.unpack(bref));
        break;
      }
      case 93: {
        report_periodicity_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(report_periodicity.unpack(bref));
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
void res_status_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 89);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_meas_id);
  if (gnb_cu_up_meas_id_present) {
    j.write_int("id", 90);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_meas_id);
  }
  j.write_int("id", 91);
  j.write_str("criticality", "reject");
  j.write_str("Value", regist_request.to_string());
  if (report_characteristics_present) {
    j.write_int("id", 92);
    j.write_str("criticality", "reject");
    j.write_str("Value", report_characteristics.to_string());
  }
  if (report_periodicity_present) {
    j.write_int("id", 93);
    j.write_str("criticality", "reject");
    j.write_str("Value", report_periodicity.to_string());
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<res_status_resp_ies_o>;

SRSASN_CODE res_status_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)89, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)90, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)1, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_resp_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 89: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 90: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 1: {
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
void res_status_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 89);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_meas_id);
  j.write_int("id", 90);
  j.write_str("criticality", "ignore");
  j.write_int("Value", gnb_cu_up_meas_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 1);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<res_status_upd_ies_o>;

SRSASN_CODE res_status_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += gnb_cu_up_meas_id_present ? 1 : 0;
  nof_ies += tnl_available_capacity_ind_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)57, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)89, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  if (gnb_cu_up_meas_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)90, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_meas_id, (uint16_t)1u, (uint16_t)4095u, true, true));
  }
  if (tnl_available_capacity_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)94, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(tnl_available_capacity_ind.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)95, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(hw_capacity_ind.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE res_status_upd_ies_container::unpack(cbit_ref& bref)
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
      case 57: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 89: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 90: {
        gnb_cu_up_meas_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_meas_id, bref, (uint16_t)1u, (uint16_t)4095u, true, true));
        break;
      }
      case 94: {
        tnl_available_capacity_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(tnl_available_capacity_ind.unpack(bref));
        break;
      }
      case 95: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(hw_capacity_ind.unpack(bref));
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
void res_status_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 57);
  j.write_str("criticality", "reject");
  j.write_int("Value", transaction_id);
  j.write_int("id", 89);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_meas_id);
  if (gnb_cu_up_meas_id_present) {
    j.write_int("id", 90);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_cu_up_meas_id);
  }
  if (tnl_available_capacity_ind_present) {
    j.write_int("id", 94);
    j.write_str("criticality", "ignore");
    tnl_available_capacity_ind.to_json(j);
  }
  j.write_int("id", 95);
  j.write_str("criticality", "ignore");
  hw_capacity_ind.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<trace_start_ies_o>;

SRSASN_CODE trace_start_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)81, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(trace_activation.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE trace_start_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 81: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_activation.unpack(bref));
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
void trace_start_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 81);
  j.write_str("criticality", "ignore");
  trace_activation.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ul_data_notif_ies_o>;

SRSASN_CODE ul_data_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)2, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)3, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_ue_e1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)67, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pdu_session_to_notify_list, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_data_notif_ies_container::unpack(cbit_ref& bref)
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
      case 2: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_up_ue_e1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 67: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pdu_session_to_notify_list, bref, 1, 256, true));
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
void ul_data_notif_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 2);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_cp_ue_e1ap_id);
  j.write_int("id", 3);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_up_ue_e1ap_id);
  j.write_int("id", 67);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : pdu_session_to_notify_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
