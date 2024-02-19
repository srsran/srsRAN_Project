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

#include "srsran/asn1/rrc_nr/pcch_msg_ies.h"
using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

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
