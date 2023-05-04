/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include <sstream>

using namespace asn1;
using namespace asn1::f1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// DLRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t dl_rrc_msg_transfer_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 47, 64, 109, 50, 108, 184, 222, 218, 221, 217, 248, 597};
  return map_enum_number(names, 14, idx, "id");
}
bool dl_rrc_msg_transfer_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 47, 64, 109, 50, 108, 184, 222, 218, 221, 217, 248, 597};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e dl_rrc_msg_transfer_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 47:
      return crit_e::reject;
    case 64:
      return crit_e::reject;
    case 109:
      return crit_e::ignore;
    case 50:
      return crit_e::reject;
    case 108:
      return crit_e::reject;
    case 184:
      return crit_e::ignore;
    case 222:
      return crit_e::reject;
    case 218:
      return crit_e::reject;
    case 221:
      return crit_e::ignore;
    case 217:
      return crit_e::reject;
    case 248:
      return crit_e::ignore;
    case 597:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
dl_rrc_msg_transfer_ies_o::value_c dl_rrc_msg_transfer_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 47:
      ret.set(value_c::types::old_gnb_du_ue_f1ap_id);
      break;
    case 64:
      ret.set(value_c::types::srb_id);
      break;
    case 109:
      ret.set(value_c::types::execute_dupl);
      break;
    case 50:
      ret.set(value_c::types::rrc_container);
      break;
    case 108:
      ret.set(value_c::types::rat_freq_prio_info);
      break;
    case 184:
      ret.set(value_c::types::rrc_delivery_status_request);
      break;
    case 222:
      ret.set(value_c::types::ue_context_not_retrievable);
      break;
    case 218:
      ret.set(value_c::types::redirected_rrc_msg);
      break;
    case 221:
      ret.set(value_c::types::plmn_assist_info_for_net_shar);
      break;
    case 217:
      ret.set(value_c::types::new_gnb_cu_ue_f1ap_id);
      break;
    case 248:
      ret.set(value_c::types::add_rrm_prio_idx);
      break;
    case 597:
      ret.set(value_c::types::srb_map_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e dl_rrc_msg_transfer_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 47:
      return presence_e::optional;
    case 64:
      return presence_e::mandatory;
    case 109:
      return presence_e::optional;
    case 50:
      return presence_e::mandatory;
    case 108:
      return presence_e::optional;
    case 184:
      return presence_e::optional;
    case 222:
      return presence_e::optional;
    case 218:
      return presence_e::optional;
    case 221:
      return presence_e::optional;
    case 217:
      return presence_e::optional;
    case 248:
      return presence_e::optional;
    case 597:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void dl_rrc_msg_transfer_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::rat_freq_prio_info:
      c.destroy<rat_freq_prio_info_c>();
      break;
    case types::redirected_rrc_msg:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::plmn_assist_info_for_net_shar:
      c.destroy<fixed_octstring<3, true>>();
      break;
    case types::add_rrm_prio_idx:
      c.destroy<fixed_bitstring<32, false, true>>();
      break;
    case types::srb_map_info:
      c.destroy<fixed_bitstring<16, false, true>>();
      break;
    default:
      break;
  }
}
void dl_rrc_msg_transfer_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::old_gnb_du_ue_f1ap_id:
      break;
    case types::srb_id:
      break;
    case types::execute_dupl:
      break;
    case types::rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::rat_freq_prio_info:
      c.init<rat_freq_prio_info_c>();
      break;
    case types::rrc_delivery_status_request:
      break;
    case types::ue_context_not_retrievable:
      break;
    case types::redirected_rrc_msg:
      c.init<unbounded_octstring<true>>();
      break;
    case types::plmn_assist_info_for_net_shar:
      c.init<fixed_octstring<3, true>>();
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      break;
    case types::add_rrm_prio_idx:
      c.init<fixed_bitstring<32, false, true>>();
      break;
    case types::srb_map_info:
      c.init<fixed_bitstring<16, false, true>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_rrc_msg_transfer_ies_o::value_c");
  }
}
dl_rrc_msg_transfer_ies_o::value_c::value_c(const dl_rrc_msg_transfer_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::old_gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::srb_id:
      c.init(other.c.get<uint8_t>());
      break;
    case types::execute_dupl:
      c.init(other.c.get<execute_dupl_e>());
      break;
    case types::rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::rat_freq_prio_info:
      c.init(other.c.get<rat_freq_prio_info_c>());
      break;
    case types::rrc_delivery_status_request:
      c.init(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::ue_context_not_retrievable:
      c.init(other.c.get<ue_context_not_retrievable_e>());
      break;
    case types::redirected_rrc_msg:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::plmn_assist_info_for_net_shar:
      c.init(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::add_rrm_prio_idx:
      c.init(other.c.get<fixed_bitstring<32, false, true>>());
      break;
    case types::srb_map_info:
      c.init(other.c.get<fixed_bitstring<16, false, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_rrc_msg_transfer_ies_o::value_c");
  }
}
dl_rrc_msg_transfer_ies_o::value_c&
dl_rrc_msg_transfer_ies_o::value_c::operator=(const dl_rrc_msg_transfer_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::old_gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::srb_id:
      c.set(other.c.get<uint8_t>());
      break;
    case types::execute_dupl:
      c.set(other.c.get<execute_dupl_e>());
      break;
    case types::rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::rat_freq_prio_info:
      c.set(other.c.get<rat_freq_prio_info_c>());
      break;
    case types::rrc_delivery_status_request:
      c.set(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::ue_context_not_retrievable:
      c.set(other.c.get<ue_context_not_retrievable_e>());
      break;
    case types::redirected_rrc_msg:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::plmn_assist_info_for_net_shar:
      c.set(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::add_rrm_prio_idx:
      c.set(other.c.get<fixed_bitstring<32, false, true>>());
      break;
    case types::srb_map_info:
      c.set(other.c.get<fixed_bitstring<16, false, true>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_rrc_msg_transfer_ies_o::value_c");
  }

  return *this;
}
uint64_t& dl_rrc_msg_transfer_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& dl_rrc_msg_transfer_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& dl_rrc_msg_transfer_ies_o::value_c::old_gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::old_gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint8_t& dl_rrc_msg_transfer_ies_o::value_c::srb_id()
{
  assert_choice_type(types::srb_id, type_, "Value");
  return c.get<uint8_t>();
}
execute_dupl_e& dl_rrc_msg_transfer_ies_o::value_c::execute_dupl()
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
unbounded_octstring<true>& dl_rrc_msg_transfer_ies_o::value_c::rrc_container()
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
rat_freq_prio_info_c& dl_rrc_msg_transfer_ies_o::value_c::rat_freq_prio_info()
{
  assert_choice_type(types::rat_freq_prio_info, type_, "Value");
  return c.get<rat_freq_prio_info_c>();
}
rrc_delivery_status_request_e& dl_rrc_msg_transfer_ies_o::value_c::rrc_delivery_status_request()
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
ue_context_not_retrievable_e& dl_rrc_msg_transfer_ies_o::value_c::ue_context_not_retrievable()
{
  assert_choice_type(types::ue_context_not_retrievable, type_, "Value");
  return c.get<ue_context_not_retrievable_e>();
}
unbounded_octstring<true>& dl_rrc_msg_transfer_ies_o::value_c::redirected_rrc_msg()
{
  assert_choice_type(types::redirected_rrc_msg, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
fixed_octstring<3, true>& dl_rrc_msg_transfer_ies_o::value_c::plmn_assist_info_for_net_shar()
{
  assert_choice_type(types::plmn_assist_info_for_net_shar, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
uint64_t& dl_rrc_msg_transfer_ies_o::value_c::new_gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::new_gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
fixed_bitstring<32, false, true>& dl_rrc_msg_transfer_ies_o::value_c::add_rrm_prio_idx()
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
fixed_bitstring<16, false, true>& dl_rrc_msg_transfer_ies_o::value_c::srb_map_info()
{
  assert_choice_type(types::srb_map_info, type_, "Value");
  return c.get<fixed_bitstring<16, false, true>>();
}
const uint64_t& dl_rrc_msg_transfer_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& dl_rrc_msg_transfer_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& dl_rrc_msg_transfer_ies_o::value_c::old_gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::old_gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint8_t& dl_rrc_msg_transfer_ies_o::value_c::srb_id() const
{
  assert_choice_type(types::srb_id, type_, "Value");
  return c.get<uint8_t>();
}
const execute_dupl_e& dl_rrc_msg_transfer_ies_o::value_c::execute_dupl() const
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
const unbounded_octstring<true>& dl_rrc_msg_transfer_ies_o::value_c::rrc_container() const
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const rat_freq_prio_info_c& dl_rrc_msg_transfer_ies_o::value_c::rat_freq_prio_info() const
{
  assert_choice_type(types::rat_freq_prio_info, type_, "Value");
  return c.get<rat_freq_prio_info_c>();
}
const rrc_delivery_status_request_e& dl_rrc_msg_transfer_ies_o::value_c::rrc_delivery_status_request() const
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
const ue_context_not_retrievable_e& dl_rrc_msg_transfer_ies_o::value_c::ue_context_not_retrievable() const
{
  assert_choice_type(types::ue_context_not_retrievable, type_, "Value");
  return c.get<ue_context_not_retrievable_e>();
}
const unbounded_octstring<true>& dl_rrc_msg_transfer_ies_o::value_c::redirected_rrc_msg() const
{
  assert_choice_type(types::redirected_rrc_msg, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const fixed_octstring<3, true>& dl_rrc_msg_transfer_ies_o::value_c::plmn_assist_info_for_net_shar() const
{
  assert_choice_type(types::plmn_assist_info_for_net_shar, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
const uint64_t& dl_rrc_msg_transfer_ies_o::value_c::new_gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::new_gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const fixed_bitstring<32, false, true>& dl_rrc_msg_transfer_ies_o::value_c::add_rrm_prio_idx() const
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
const fixed_bitstring<16, false, true>& dl_rrc_msg_transfer_ies_o::value_c::srb_map_info() const
{
  assert_choice_type(types::srb_map_info, type_, "Value");
  return c.get<fixed_bitstring<16, false, true>>();
}
void dl_rrc_msg_transfer_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::old_gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::srb_id:
      j.write_int("INTEGER (0..3,...)", c.get<uint8_t>());
      break;
    case types::execute_dupl:
      j.write_str("ExecuteDuplication", "true");
      break;
    case types::rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::rat_freq_prio_info:
      j.write_fieldname("RAT-FrequencyPriorityInformation");
      c.get<rat_freq_prio_info_c>().to_json(j);
      break;
    case types::rrc_delivery_status_request:
      j.write_str("RRCDeliveryStatusRequest", "true");
      break;
    case types::ue_context_not_retrievable:
      j.write_str("UEContextNotRetrievable", "true");
      break;
    case types::redirected_rrc_msg:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::plmn_assist_info_for_net_shar:
      j.write_str("OCTET STRING", c.get<fixed_octstring<3, true>>().to_string());
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::add_rrm_prio_idx:
      j.write_str("BIT STRING", c.get<fixed_bitstring<32, false, true>>().to_string());
      break;
    case types::srb_map_info:
      j.write_str("BIT STRING", c.get<fixed_bitstring<16, false, true>>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "dl_rrc_msg_transfer_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE dl_rrc_msg_transfer_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::old_gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::srb_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u, true, true));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().pack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::rat_freq_prio_info:
      HANDLE_CODE(c.get<rat_freq_prio_info_c>().pack(bref));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().pack(bref));
      break;
    case types::ue_context_not_retrievable:
      HANDLE_CODE(c.get<ue_context_not_retrievable_e>().pack(bref));
      break;
    case types::redirected_rrc_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::plmn_assist_info_for_net_shar:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().pack(bref)));
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().pack(bref)));
      break;
    case types::srb_map_info:
      HANDLE_CODE((c.get<fixed_bitstring<16, false, true>>().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "dl_rrc_msg_transfer_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_rrc_msg_transfer_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::old_gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::srb_id:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u, true, true));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().unpack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::rat_freq_prio_info:
      HANDLE_CODE(c.get<rat_freq_prio_info_c>().unpack(bref));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().unpack(bref));
      break;
    case types::ue_context_not_retrievable:
      HANDLE_CODE(c.get<ue_context_not_retrievable_e>().unpack(bref));
      break;
    case types::redirected_rrc_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::plmn_assist_info_for_net_shar:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().unpack(bref)));
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().unpack(bref)));
      break;
    case types::srb_map_info:
      HANDLE_CODE((c.get<fixed_bitstring<16, false, true>>().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "dl_rrc_msg_transfer_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

dl_rrc_msg_transfer_ies_container::dl_rrc_msg_transfer_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  old_gnb_du_ue_f1ap_id(47, crit_e::reject),
  srb_id(64, crit_e::reject),
  execute_dupl(109, crit_e::ignore),
  rrc_container(50, crit_e::reject),
  rat_freq_prio_info(108, crit_e::reject),
  rrc_delivery_status_request(184, crit_e::ignore),
  ue_context_not_retrievable(222, crit_e::reject),
  redirected_rrc_msg(218, crit_e::reject),
  plmn_assist_info_for_net_shar(221, crit_e::ignore),
  new_gnb_cu_ue_f1ap_id(217, crit_e::reject),
  add_rrm_prio_idx(248, crit_e::ignore),
  srb_map_info(597, crit_e::ignore)
{
}
SRSASN_CODE dl_rrc_msg_transfer_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += old_gnb_du_ue_f1ap_id_present ? 1 : 0;
  nof_ies += execute_dupl_present ? 1 : 0;
  nof_ies += rat_freq_prio_info_present ? 1 : 0;
  nof_ies += rrc_delivery_status_request_present ? 1 : 0;
  nof_ies += ue_context_not_retrievable_present ? 1 : 0;
  nof_ies += redirected_rrc_msg_present ? 1 : 0;
  nof_ies += plmn_assist_info_for_net_shar_present ? 1 : 0;
  nof_ies += new_gnb_cu_ue_f1ap_id_present ? 1 : 0;
  nof_ies += add_rrm_prio_idx_present ? 1 : 0;
  nof_ies += srb_map_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  if (old_gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(old_gnb_du_ue_f1ap_id.pack(bref));
  }
  HANDLE_CODE(srb_id.pack(bref));
  if (execute_dupl_present) {
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  HANDLE_CODE(rrc_container.pack(bref));
  if (rat_freq_prio_info_present) {
    HANDLE_CODE(rat_freq_prio_info.pack(bref));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (ue_context_not_retrievable_present) {
    HANDLE_CODE(ue_context_not_retrievable.pack(bref));
  }
  if (redirected_rrc_msg_present) {
    HANDLE_CODE(redirected_rrc_msg.pack(bref));
  }
  if (plmn_assist_info_for_net_shar_present) {
    HANDLE_CODE(plmn_assist_info_for_net_shar.pack(bref));
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    HANDLE_CODE(new_gnb_cu_ue_f1ap_id.pack(bref));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }
  if (srb_map_info_present) {
    HANDLE_CODE(srb_map_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE dl_rrc_msg_transfer_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 47: {
        old_gnb_du_ue_f1ap_id_present = true;
        old_gnb_du_ue_f1ap_id.id      = id;
        HANDLE_CODE(old_gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(old_gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 64: {
        nof_mandatory_ies--;
        srb_id.id = id;
        HANDLE_CODE(srb_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srb_id.value.unpack(bref));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        execute_dupl.id      = id;
        HANDLE_CODE(execute_dupl.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.value.unpack(bref));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        rrc_container.id = id;
        HANDLE_CODE(rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.value.unpack(bref));
        break;
      }
      case 108: {
        rat_freq_prio_info_present = true;
        rat_freq_prio_info.id      = id;
        HANDLE_CODE(rat_freq_prio_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rat_freq_prio_info.value.unpack(bref));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        rrc_delivery_status_request.id      = id;
        HANDLE_CODE(rrc_delivery_status_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.value.unpack(bref));
        break;
      }
      case 222: {
        ue_context_not_retrievable_present = true;
        ue_context_not_retrievable.id      = id;
        HANDLE_CODE(ue_context_not_retrievable.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_context_not_retrievable.value.unpack(bref));
        break;
      }
      case 218: {
        redirected_rrc_msg_present = true;
        redirected_rrc_msg.id      = id;
        HANDLE_CODE(redirected_rrc_msg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redirected_rrc_msg.value.unpack(bref));
        break;
      }
      case 221: {
        plmn_assist_info_for_net_shar_present = true;
        plmn_assist_info_for_net_shar.id      = id;
        HANDLE_CODE(plmn_assist_info_for_net_shar.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(plmn_assist_info_for_net_shar.value.unpack(bref));
        break;
      }
      case 217: {
        new_gnb_cu_ue_f1ap_id_present = true;
        new_gnb_cu_ue_f1ap_id.id      = id;
        HANDLE_CODE(new_gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(new_gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 248: {
        add_rrm_prio_idx_present = true;
        add_rrm_prio_idx.id      = id;
        HANDLE_CODE(add_rrm_prio_idx.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.value.unpack(bref));
        break;
      }
      case 597: {
        srb_map_info_present = true;
        srb_map_info.id      = id;
        HANDLE_CODE(srb_map_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srb_map_info.value.unpack(bref));
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
void dl_rrc_msg_transfer_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  if (old_gnb_du_ue_f1ap_id_present) {
    j.write_fieldname("");
    old_gnb_du_ue_f1ap_id.to_json(j);
  }
  j.write_fieldname("");
  srb_id.to_json(j);
  if (execute_dupl_present) {
    j.write_fieldname("");
    execute_dupl.to_json(j);
  }
  j.write_fieldname("");
  rrc_container.to_json(j);
  if (rat_freq_prio_info_present) {
    j.write_fieldname("");
    rat_freq_prio_info.to_json(j);
  }
  if (rrc_delivery_status_request_present) {
    j.write_fieldname("");
    rrc_delivery_status_request.to_json(j);
  }
  if (ue_context_not_retrievable_present) {
    j.write_fieldname("");
    ue_context_not_retrievable.to_json(j);
  }
  if (redirected_rrc_msg_present) {
    j.write_fieldname("");
    redirected_rrc_msg.to_json(j);
  }
  if (plmn_assist_info_for_net_shar_present) {
    j.write_fieldname("");
    plmn_assist_info_for_net_shar.to_json(j);
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    j.write_fieldname("");
    new_gnb_cu_ue_f1ap_id.to_json(j);
  }
  if (add_rrm_prio_idx_present) {
    j.write_fieldname("");
    add_rrm_prio_idx.to_json(j);
  }
  if (srb_map_info_present) {
    j.write_fieldname("");
    srb_map_info.to_json(j);
  }
  j.end_obj();
}

// InitialULRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t init_ul_rrc_msg_transfer_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {41, 111, 95, 50, 128, 178, 78, 226, 241, 578, 592, 617};
  return map_enum_number(names, 12, idx, "id");
}
bool init_ul_rrc_msg_transfer_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {41, 111, 95, 50, 128, 178, 78, 226, 241, 578, 592, 617};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e init_ul_rrc_msg_transfer_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 41:
      return crit_e::reject;
    case 111:
      return crit_e::reject;
    case 95:
      return crit_e::reject;
    case 50:
      return crit_e::reject;
    case 128:
      return crit_e::reject;
    case 178:
      return crit_e::ignore;
    case 78:
      return crit_e::ignore;
    case 226:
      return crit_e::ignore;
    case 241:
      return crit_e::ignore;
    case 578:
      return crit_e::ignore;
    case 592:
      return crit_e::ignore;
    case 617:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
init_ul_rrc_msg_transfer_ies_o::value_c init_ul_rrc_msg_transfer_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 111:
      ret.set(value_c::types::nr_cgi);
      break;
    case 95:
      ret.set(value_c::types::c_rnti);
      break;
    case 50:
      ret.set(value_c::types::rrc_container);
      break;
    case 128:
      ret.set(value_c::types::du_to_cu_rrc_container);
      break;
    case 178:
      ret.set(value_c::types::sul_access_ind);
      break;
    case 78:
      ret.set(value_c::types::transaction_id);
      break;
    case 226:
      ret.set(value_c::types::ran_ue_id);
      break;
    case 241:
      ret.set(value_c::types::rrc_container_rrc_setup_complete);
      break;
    case 578:
      ret.set(value_c::types::nr_red_cap_ue_ind);
      break;
    case 592:
      ret.set(value_c::types::sdt_info);
      break;
    case 617:
      ret.set(value_c::types::sidelink_relay_cfg);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e init_ul_rrc_msg_transfer_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 41:
      return presence_e::mandatory;
    case 111:
      return presence_e::mandatory;
    case 95:
      return presence_e::mandatory;
    case 50:
      return presence_e::mandatory;
    case 128:
      return presence_e::optional;
    case 178:
      return presence_e::optional;
    case 78:
      return presence_e::mandatory;
    case 226:
      return presence_e::optional;
    case 241:
      return presence_e::optional;
    case 578:
      return presence_e::optional;
    case 592:
      return presence_e::optional;
    case 617:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void init_ul_rrc_msg_transfer_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::nr_cgi:
      c.destroy<nr_cgi_s>();
      break;
    case types::rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::du_to_cu_rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::ran_ue_id:
      c.destroy<fixed_octstring<8, true>>();
      break;
    case types::rrc_container_rrc_setup_complete:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::sdt_info:
      c.destroy<sdt_info_s>();
      break;
    case types::sidelink_relay_cfg:
      c.destroy<sidelink_relay_cfg_s>();
      break;
    default:
      break;
  }
}
void init_ul_rrc_msg_transfer_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::nr_cgi:
      c.init<nr_cgi_s>();
      break;
    case types::c_rnti:
      break;
    case types::rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::du_to_cu_rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::sul_access_ind:
      break;
    case types::transaction_id:
      break;
    case types::ran_ue_id:
      c.init<fixed_octstring<8, true>>();
      break;
    case types::rrc_container_rrc_setup_complete:
      c.init<unbounded_octstring<true>>();
      break;
    case types::nr_red_cap_ue_ind:
      break;
    case types::sdt_info:
      c.init<sdt_info_s>();
      break;
    case types::sidelink_relay_cfg:
      c.init<sidelink_relay_cfg_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ul_rrc_msg_transfer_ies_o::value_c");
  }
}
init_ul_rrc_msg_transfer_ies_o::value_c::value_c(const init_ul_rrc_msg_transfer_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::nr_cgi:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::c_rnti:
      c.init(other.c.get<uint32_t>());
      break;
    case types::rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::du_to_cu_rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::sul_access_ind:
      c.init(other.c.get<sul_access_ind_e>());
      break;
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ran_ue_id:
      c.init(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::rrc_container_rrc_setup_complete:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::nr_red_cap_ue_ind:
      c.init(other.c.get<nr_red_cap_ue_ind_e>());
      break;
    case types::sdt_info:
      c.init(other.c.get<sdt_info_s>());
      break;
    case types::sidelink_relay_cfg:
      c.init(other.c.get<sidelink_relay_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ul_rrc_msg_transfer_ies_o::value_c");
  }
}
init_ul_rrc_msg_transfer_ies_o::value_c&
init_ul_rrc_msg_transfer_ies_o::value_c::operator=(const init_ul_rrc_msg_transfer_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::nr_cgi:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::c_rnti:
      c.set(other.c.get<uint32_t>());
      break;
    case types::rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::du_to_cu_rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::sul_access_ind:
      c.set(other.c.get<sul_access_ind_e>());
      break;
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ran_ue_id:
      c.set(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::rrc_container_rrc_setup_complete:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::nr_red_cap_ue_ind:
      c.set(other.c.get<nr_red_cap_ue_ind_e>());
      break;
    case types::sdt_info:
      c.set(other.c.get<sdt_info_s>());
      break;
    case types::sidelink_relay_cfg:
      c.set(other.c.get<sidelink_relay_cfg_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ul_rrc_msg_transfer_ies_o::value_c");
  }

  return *this;
}
uint64_t& init_ul_rrc_msg_transfer_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
nr_cgi_s& init_ul_rrc_msg_transfer_ies_o::value_c::nr_cgi()
{
  assert_choice_type(types::nr_cgi, type_, "Value");
  return c.get<nr_cgi_s>();
}
uint32_t& init_ul_rrc_msg_transfer_ies_o::value_c::c_rnti()
{
  assert_choice_type(types::c_rnti, type_, "Value");
  return c.get<uint32_t>();
}
unbounded_octstring<true>& init_ul_rrc_msg_transfer_ies_o::value_c::rrc_container()
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
unbounded_octstring<true>& init_ul_rrc_msg_transfer_ies_o::value_c::du_to_cu_rrc_container()
{
  assert_choice_type(types::du_to_cu_rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
sul_access_ind_e& init_ul_rrc_msg_transfer_ies_o::value_c::sul_access_ind()
{
  assert_choice_type(types::sul_access_ind, type_, "Value");
  return c.get<sul_access_ind_e>();
}
uint16_t& init_ul_rrc_msg_transfer_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
fixed_octstring<8, true>& init_ul_rrc_msg_transfer_ies_o::value_c::ran_ue_id()
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
unbounded_octstring<true>& init_ul_rrc_msg_transfer_ies_o::value_c::rrc_container_rrc_setup_complete()
{
  assert_choice_type(types::rrc_container_rrc_setup_complete, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
nr_red_cap_ue_ind_e& init_ul_rrc_msg_transfer_ies_o::value_c::nr_red_cap_ue_ind()
{
  assert_choice_type(types::nr_red_cap_ue_ind, type_, "Value");
  return c.get<nr_red_cap_ue_ind_e>();
}
sdt_info_s& init_ul_rrc_msg_transfer_ies_o::value_c::sdt_info()
{
  assert_choice_type(types::sdt_info, type_, "Value");
  return c.get<sdt_info_s>();
}
sidelink_relay_cfg_s& init_ul_rrc_msg_transfer_ies_o::value_c::sidelink_relay_cfg()
{
  assert_choice_type(types::sidelink_relay_cfg, type_, "Value");
  return c.get<sidelink_relay_cfg_s>();
}
const uint64_t& init_ul_rrc_msg_transfer_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const nr_cgi_s& init_ul_rrc_msg_transfer_ies_o::value_c::nr_cgi() const
{
  assert_choice_type(types::nr_cgi, type_, "Value");
  return c.get<nr_cgi_s>();
}
const uint32_t& init_ul_rrc_msg_transfer_ies_o::value_c::c_rnti() const
{
  assert_choice_type(types::c_rnti, type_, "Value");
  return c.get<uint32_t>();
}
const unbounded_octstring<true>& init_ul_rrc_msg_transfer_ies_o::value_c::rrc_container() const
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const unbounded_octstring<true>& init_ul_rrc_msg_transfer_ies_o::value_c::du_to_cu_rrc_container() const
{
  assert_choice_type(types::du_to_cu_rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const sul_access_ind_e& init_ul_rrc_msg_transfer_ies_o::value_c::sul_access_ind() const
{
  assert_choice_type(types::sul_access_ind, type_, "Value");
  return c.get<sul_access_ind_e>();
}
const uint16_t& init_ul_rrc_msg_transfer_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const fixed_octstring<8, true>& init_ul_rrc_msg_transfer_ies_o::value_c::ran_ue_id() const
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
const unbounded_octstring<true>& init_ul_rrc_msg_transfer_ies_o::value_c::rrc_container_rrc_setup_complete() const
{
  assert_choice_type(types::rrc_container_rrc_setup_complete, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const nr_red_cap_ue_ind_e& init_ul_rrc_msg_transfer_ies_o::value_c::nr_red_cap_ue_ind() const
{
  assert_choice_type(types::nr_red_cap_ue_ind, type_, "Value");
  return c.get<nr_red_cap_ue_ind_e>();
}
const sdt_info_s& init_ul_rrc_msg_transfer_ies_o::value_c::sdt_info() const
{
  assert_choice_type(types::sdt_info, type_, "Value");
  return c.get<sdt_info_s>();
}
const sidelink_relay_cfg_s& init_ul_rrc_msg_transfer_ies_o::value_c::sidelink_relay_cfg() const
{
  assert_choice_type(types::sidelink_relay_cfg, type_, "Value");
  return c.get<sidelink_relay_cfg_s>();
}
void init_ul_rrc_msg_transfer_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::nr_cgi:
      j.write_fieldname("NRCGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    case types::c_rnti:
      j.write_int("INTEGER (0..65535,...)", c.get<uint32_t>());
      break;
    case types::rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::du_to_cu_rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::sul_access_ind:
      j.write_str("SULAccessIndication", "true");
      break;
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ran_ue_id:
      j.write_str("OCTET STRING", c.get<fixed_octstring<8, true>>().to_string());
      break;
    case types::rrc_container_rrc_setup_complete:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::nr_red_cap_ue_ind:
      j.write_str("NRRedCapUEIndication", "true");
      break;
    case types::sdt_info:
      j.write_fieldname("SDTInformation");
      c.get<sdt_info_s>().to_json(j);
      break;
    case types::sidelink_relay_cfg:
      j.write_fieldname("SidelinkRelayConfiguration");
      c.get<sidelink_relay_cfg_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "init_ul_rrc_msg_transfer_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE init_ul_rrc_msg_transfer_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::nr_cgi:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    case types::c_rnti:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u, true, true));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::du_to_cu_rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::sul_access_ind:
      HANDLE_CODE(c.get<sul_access_ind_e>().pack(bref));
      break;
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().pack(bref)));
      break;
    case types::rrc_container_rrc_setup_complete:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::nr_red_cap_ue_ind:
      HANDLE_CODE(c.get<nr_red_cap_ue_ind_e>().pack(bref));
      break;
    case types::sdt_info:
      HANDLE_CODE(c.get<sdt_info_s>().pack(bref));
      break;
    case types::sidelink_relay_cfg:
      HANDLE_CODE(c.get<sidelink_relay_cfg_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ul_rrc_msg_transfer_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE init_ul_rrc_msg_transfer_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::nr_cgi:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    case types::c_rnti:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u, true, true));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::du_to_cu_rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::sul_access_ind:
      HANDLE_CODE(c.get<sul_access_ind_e>().unpack(bref));
      break;
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().unpack(bref)));
      break;
    case types::rrc_container_rrc_setup_complete:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::nr_red_cap_ue_ind:
      HANDLE_CODE(c.get<nr_red_cap_ue_ind_e>().unpack(bref));
      break;
    case types::sdt_info:
      HANDLE_CODE(c.get<sdt_info_s>().unpack(bref));
      break;
    case types::sidelink_relay_cfg:
      HANDLE_CODE(c.get<sidelink_relay_cfg_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ul_rrc_msg_transfer_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextModificationConfirmIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_mod_confirm_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 49, 19, 50, 7, 109, 195, 337, 604, 616, 653};
  return map_enum_number(names, 12, idx, "id");
}
bool ue_context_mod_confirm_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 49, 19, 50, 7, 109, 195, 337, 604, 616, 653};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_mod_confirm_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 49:
      return crit_e::ignore;
    case 19:
      return crit_e::ignore;
    case 50:
      return crit_e::ignore;
    case 7:
      return crit_e::ignore;
    case 109:
      return crit_e::ignore;
    case 195:
      return crit_e::ignore;
    case 337:
      return crit_e::ignore;
    case 604:
      return crit_e::reject;
    case 616:
      return crit_e::reject;
    case 653:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_mod_confirm_ies_o::value_c ue_context_mod_confirm_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 49:
      ret.set(value_c::types::res_coordination_transfer_container);
      break;
    case 19:
      ret.set(value_c::types::drbs_modified_conf_list);
      break;
    case 50:
      ret.set(value_c::types::rrc_container);
      break;
    case 7:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 109:
      ret.set(value_c::types::execute_dupl);
      break;
    case 195:
      ret.set(value_c::types::res_coordination_transfer_info);
      break;
    case 337:
      ret.set(value_c::types::sl_drbs_modified_conf_list);
      break;
    case 604:
      ret.set(value_c::types::uu_rlc_ch_modified_list);
      break;
    case 616:
      ret.set(value_c::types::pc5_rlc_ch_modified_list);
      break;
    case 653:
      ret.set(value_c::types::ue_multicast_m_rbs_confirmed_to_be_modified_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_mod_confirm_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 49:
      return presence_e::optional;
    case 19:
      return presence_e::optional;
    case 50:
      return presence_e::optional;
    case 7:
      return presence_e::optional;
    case 109:
      return presence_e::optional;
    case 195:
      return presence_e::optional;
    case 337:
      return presence_e::optional;
    case 604:
      return presence_e::optional;
    case 616:
      return presence_e::optional;
    case 653:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_mod_confirm_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::res_coordination_transfer_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::drbs_modified_conf_list:
      c.destroy<drbs_modified_conf_list_l>();
      break;
    case types::rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    case types::res_coordination_transfer_info:
      c.destroy<res_coordination_transfer_info_s>();
      break;
    case types::sl_drbs_modified_conf_list:
      c.destroy<sl_drbs_modified_conf_list_l>();
      break;
    case types::uu_rlc_ch_modified_list:
      c.destroy<uu_rlc_ch_modified_list_l>();
      break;
    case types::pc5_rlc_ch_modified_list:
      c.destroy<pc5_rlc_ch_modified_list_l>();
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      c.destroy<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>();
      break;
    default:
      break;
  }
}
void ue_context_mod_confirm_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::res_coordination_transfer_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::drbs_modified_conf_list:
      c.init<drbs_modified_conf_list_l>();
      break;
    case types::rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::execute_dupl:
      break;
    case types::res_coordination_transfer_info:
      c.init<res_coordination_transfer_info_s>();
      break;
    case types::sl_drbs_modified_conf_list:
      c.init<sl_drbs_modified_conf_list_l>();
      break;
    case types::uu_rlc_ch_modified_list:
      c.init<uu_rlc_ch_modified_list_l>();
      break;
    case types::pc5_rlc_ch_modified_list:
      c.init<pc5_rlc_ch_modified_list_l>();
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      c.init<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_confirm_ies_o::value_c");
  }
}
ue_context_mod_confirm_ies_o::value_c::value_c(const ue_context_mod_confirm_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::drbs_modified_conf_list:
      c.init(other.c.get<drbs_modified_conf_list_l>());
      break;
    case types::rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::execute_dupl:
      c.init(other.c.get<execute_dupl_e>());
      break;
    case types::res_coordination_transfer_info:
      c.init(other.c.get<res_coordination_transfer_info_s>());
      break;
    case types::sl_drbs_modified_conf_list:
      c.init(other.c.get<sl_drbs_modified_conf_list_l>());
      break;
    case types::uu_rlc_ch_modified_list:
      c.init(other.c.get<uu_rlc_ch_modified_list_l>());
      break;
    case types::pc5_rlc_ch_modified_list:
      c.init(other.c.get<pc5_rlc_ch_modified_list_l>());
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      c.init(other.c.get<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_confirm_ies_o::value_c");
  }
}
ue_context_mod_confirm_ies_o::value_c&
ue_context_mod_confirm_ies_o::value_c::operator=(const ue_context_mod_confirm_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::drbs_modified_conf_list:
      c.set(other.c.get<drbs_modified_conf_list_l>());
      break;
    case types::rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::execute_dupl:
      c.set(other.c.get<execute_dupl_e>());
      break;
    case types::res_coordination_transfer_info:
      c.set(other.c.get<res_coordination_transfer_info_s>());
      break;
    case types::sl_drbs_modified_conf_list:
      c.set(other.c.get<sl_drbs_modified_conf_list_l>());
      break;
    case types::uu_rlc_ch_modified_list:
      c.set(other.c.get<uu_rlc_ch_modified_list_l>());
      break;
    case types::pc5_rlc_ch_modified_list:
      c.set(other.c.get<pc5_rlc_ch_modified_list_l>());
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      c.set(other.c.get<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_confirm_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_mod_confirm_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_mod_confirm_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
unbounded_octstring<true>& ue_context_mod_confirm_ies_o::value_c::res_coordination_transfer_container()
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
drbs_modified_conf_list_l& ue_context_mod_confirm_ies_o::value_c::drbs_modified_conf_list()
{
  assert_choice_type(types::drbs_modified_conf_list, type_, "Value");
  return c.get<drbs_modified_conf_list_l>();
}
unbounded_octstring<true>& ue_context_mod_confirm_ies_o::value_c::rrc_container()
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
crit_diagnostics_s& ue_context_mod_confirm_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
execute_dupl_e& ue_context_mod_confirm_ies_o::value_c::execute_dupl()
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
res_coordination_transfer_info_s& ue_context_mod_confirm_ies_o::value_c::res_coordination_transfer_info()
{
  assert_choice_type(types::res_coordination_transfer_info, type_, "Value");
  return c.get<res_coordination_transfer_info_s>();
}
sl_drbs_modified_conf_list_l& ue_context_mod_confirm_ies_o::value_c::sl_drbs_modified_conf_list()
{
  assert_choice_type(types::sl_drbs_modified_conf_list, type_, "Value");
  return c.get<sl_drbs_modified_conf_list_l>();
}
uu_rlc_ch_modified_list_l& ue_context_mod_confirm_ies_o::value_c::uu_rlc_ch_modified_list()
{
  assert_choice_type(types::uu_rlc_ch_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_modified_list_l>();
}
pc5_rlc_ch_modified_list_l& ue_context_mod_confirm_ies_o::value_c::pc5_rlc_ch_modified_list()
{
  assert_choice_type(types::pc5_rlc_ch_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_modified_list_l>();
}
ue_multicast_m_rbs_confirmed_to_be_modified_list_l&
ue_context_mod_confirm_ies_o::value_c::ue_multicast_m_rbs_confirmed_to_be_modified_list()
{
  assert_choice_type(types::ue_multicast_m_rbs_confirmed_to_be_modified_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>();
}
const uint64_t& ue_context_mod_confirm_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_mod_confirm_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const unbounded_octstring<true>& ue_context_mod_confirm_ies_o::value_c::res_coordination_transfer_container() const
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const drbs_modified_conf_list_l& ue_context_mod_confirm_ies_o::value_c::drbs_modified_conf_list() const
{
  assert_choice_type(types::drbs_modified_conf_list, type_, "Value");
  return c.get<drbs_modified_conf_list_l>();
}
const unbounded_octstring<true>& ue_context_mod_confirm_ies_o::value_c::rrc_container() const
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const crit_diagnostics_s& ue_context_mod_confirm_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const execute_dupl_e& ue_context_mod_confirm_ies_o::value_c::execute_dupl() const
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
const res_coordination_transfer_info_s& ue_context_mod_confirm_ies_o::value_c::res_coordination_transfer_info() const
{
  assert_choice_type(types::res_coordination_transfer_info, type_, "Value");
  return c.get<res_coordination_transfer_info_s>();
}
const sl_drbs_modified_conf_list_l& ue_context_mod_confirm_ies_o::value_c::sl_drbs_modified_conf_list() const
{
  assert_choice_type(types::sl_drbs_modified_conf_list, type_, "Value");
  return c.get<sl_drbs_modified_conf_list_l>();
}
const uu_rlc_ch_modified_list_l& ue_context_mod_confirm_ies_o::value_c::uu_rlc_ch_modified_list() const
{
  assert_choice_type(types::uu_rlc_ch_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_modified_list_l>();
}
const pc5_rlc_ch_modified_list_l& ue_context_mod_confirm_ies_o::value_c::pc5_rlc_ch_modified_list() const
{
  assert_choice_type(types::pc5_rlc_ch_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_modified_list_l>();
}
const ue_multicast_m_rbs_confirmed_to_be_modified_list_l&
ue_context_mod_confirm_ies_o::value_c::ue_multicast_m_rbs_confirmed_to_be_modified_list() const
{
  assert_choice_type(types::ue_multicast_m_rbs_confirmed_to_be_modified_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>();
}
void ue_context_mod_confirm_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::drbs_modified_conf_list:
      j.start_array("DRBs-ModifiedConf-List");
      for (const auto& e1 : c.get<drbs_modified_conf_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    case types::execute_dupl:
      j.write_str("ExecuteDuplication", "true");
      break;
    case types::res_coordination_transfer_info:
      j.write_fieldname("ResourceCoordinationTransferInformation");
      c.get<res_coordination_transfer_info_s>().to_json(j);
      break;
    case types::sl_drbs_modified_conf_list:
      j.start_array("SLDRBs-ModifiedConf-List");
      for (const auto& e1 : c.get<sl_drbs_modified_conf_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_modified_list:
      j.start_array("UuRLCChannelModifiedList");
      for (const auto& e1 : c.get<uu_rlc_ch_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_modified_list:
      j.start_array("PC5RLCChannelModifiedList");
      for (const auto& e1 : c.get<pc5_rlc_ch_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      j.start_array("UE-MulticastMRBs-ConfirmedToBeModified-List");
      for (const auto& e1 : c.get<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_confirm_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_mod_confirm_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::drbs_modified_conf_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_modified_conf_list_l>(), 1, 64, true));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().pack(bref));
      break;
    case types::res_coordination_transfer_info:
      HANDLE_CODE(c.get<res_coordination_transfer_info_s>().pack(bref));
      break;
    case types::sl_drbs_modified_conf_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_modified_conf_list_l>(), 1, 512, true));
      break;
    case types::uu_rlc_ch_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_modified_list_l>(), 1, 32, true));
      break;
    case types::pc5_rlc_ch_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_modified_list_l>(), 1, 512, true));
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>(), 1, 64, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_confirm_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_confirm_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::drbs_modified_conf_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_modified_conf_list_l>(), bref, 1, 64, true));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().unpack(bref));
      break;
    case types::res_coordination_transfer_info:
      HANDLE_CODE(c.get<res_coordination_transfer_info_s>().unpack(bref));
      break;
    case types::sl_drbs_modified_conf_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_modified_conf_list_l>(), bref, 1, 512, true));
      break;
    case types::uu_rlc_ch_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_modified_list_l>(), bref, 1, 32, true));
      break;
    case types::pc5_rlc_ch_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_multicast_m_rbs_confirmed_to_be_modified_list_l>(), bref, 1, 64, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_confirm_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_mod_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 0, 7, 376};
  return map_enum_number(names, 5, idx, "id");
}
bool ue_context_mod_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 0, 7, 376};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_mod_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 7:
      return crit_e::ignore;
    case 376:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_mod_fail_ies_o::value_c ue_context_mod_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 7:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 376:
      ret.set(value_c::types::requested_target_cell_global_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_mod_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 7:
      return presence_e::optional;
    case 376:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_mod_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    case types::requested_target_cell_global_id:
      c.destroy<nr_cgi_s>();
      break;
    default:
      break;
  }
}
void ue_context_mod_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::requested_target_cell_global_id:
      c.init<nr_cgi_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_fail_ies_o::value_c");
  }
}
ue_context_mod_fail_ies_o::value_c::value_c(const ue_context_mod_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::requested_target_cell_global_id:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_fail_ies_o::value_c");
  }
}
ue_context_mod_fail_ies_o::value_c&
ue_context_mod_fail_ies_o::value_c::operator=(const ue_context_mod_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::requested_target_cell_global_id:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_fail_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_mod_fail_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_mod_fail_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& ue_context_mod_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ue_context_mod_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
nr_cgi_s& ue_context_mod_fail_ies_o::value_c::requested_target_cell_global_id()
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
const uint64_t& ue_context_mod_fail_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_mod_fail_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& ue_context_mod_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ue_context_mod_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const nr_cgi_s& ue_context_mod_fail_ies_o::value_c::requested_target_cell_global_id() const
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
void ue_context_mod_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
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
    case types::requested_target_cell_global_id:
      j.write_fieldname("NRCGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_mod_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextModificationRefuseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_mod_refuse_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 0, 7};
  return map_enum_number(names, 4, idx, "id");
}
bool ue_context_mod_refuse_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 0, 7};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_mod_refuse_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 7:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_mod_refuse_ies_o::value_c ue_context_mod_refuse_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 7:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_mod_refuse_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 7:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_mod_refuse_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void ue_context_mod_refuse_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_refuse_ies_o::value_c");
  }
}
ue_context_mod_refuse_ies_o::value_c::value_c(const ue_context_mod_refuse_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_refuse_ies_o::value_c");
  }
}
ue_context_mod_refuse_ies_o::value_c&
ue_context_mod_refuse_ies_o::value_c::operator=(const ue_context_mod_refuse_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_refuse_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_mod_refuse_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_mod_refuse_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& ue_context_mod_refuse_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ue_context_mod_refuse_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint64_t& ue_context_mod_refuse_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_mod_refuse_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& ue_context_mod_refuse_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ue_context_mod_refuse_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ue_context_mod_refuse_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
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
      log_invalid_choice_id(type_, "ue_context_mod_refuse_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_mod_refuse_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_refuse_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_refuse_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_refuse_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextModificationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_mod_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40,  41,  63,  107, 96,  38,  9,   79,  49,  87,  50,  56,  52,  76,  37,  31,
                                   72,  33,  97,  108, 159, 174, 175, 162, 158, 109, 184, 195, 182, 215, 94,  248,
                                   253, 267, 263, 265, 306, 307, 308, 309, 340, 332, 326, 328, 374, 428, 432, 684,
                                   510, 511, 536, 547, 586, 594, 595, 596, 618, 599, 600, 601, 608, 609, 610, 619,
                                   626, 632, 633, 685, 628, 643, 647, 675, 683, 695, 690, 691};
  return map_enum_number(names, 76, idx, "id");
}
bool ue_context_mod_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40,  41,  63,  107, 96,  38,  9,   79,  49,  87,  50,  56,  52,  76,  37,  31,
                                   72,  33,  97,  108, 159, 174, 175, 162, 158, 109, 184, 195, 182, 215, 94,  248,
                                   253, 267, 263, 265, 306, 307, 308, 309, 340, 332, 326, 328, 374, 428, 432, 684,
                                   510, 511, 536, 547, 586, 594, 595, 596, 618, 599, 600, 601, 608, 609, 610, 619,
                                   626, 632, 633, 685, 628, 643, 647, 675, 683, 695, 690, 691};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_mod_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 63:
      return crit_e::ignore;
    case 107:
      return crit_e::reject;
    case 96:
      return crit_e::ignore;
    case 38:
      return crit_e::ignore;
    case 9:
      return crit_e::reject;
    case 79:
      return crit_e::ignore;
    case 49:
      return crit_e::ignore;
    case 87:
      return crit_e::ignore;
    case 50:
      return crit_e::reject;
    case 56:
      return crit_e::ignore;
    case 52:
      return crit_e::ignore;
    case 76:
      return crit_e::reject;
    case 37:
      return crit_e::reject;
    case 31:
      return crit_e::reject;
    case 72:
      return crit_e::reject;
    case 33:
      return crit_e::reject;
    case 97:
      return crit_e::reject;
    case 108:
      return crit_e::reject;
    case 159:
      return crit_e::ignore;
    case 174:
      return crit_e::ignore;
    case 175:
      return crit_e::ignore;
    case 162:
      return crit_e::reject;
    case 158:
      return crit_e::ignore;
    case 109:
      return crit_e::ignore;
    case 184:
      return crit_e::ignore;
    case 195:
      return crit_e::ignore;
    case 182:
      return crit_e::ignore;
    case 215:
      return crit_e::ignore;
    case 94:
      return crit_e::reject;
    case 248:
      return crit_e::ignore;
    case 253:
      return crit_e::ignore;
    case 267:
      return crit_e::reject;
    case 263:
      return crit_e::reject;
    case 265:
      return crit_e::reject;
    case 306:
      return crit_e::ignore;
    case 307:
      return crit_e::ignore;
    case 308:
      return crit_e::ignore;
    case 309:
      return crit_e::ignore;
    case 340:
      return crit_e::ignore;
    case 332:
      return crit_e::reject;
    case 326:
      return crit_e::reject;
    case 328:
      return crit_e::reject;
    case 374:
      return crit_e::reject;
    case 428:
      return crit_e::reject;
    case 432:
      return crit_e::ignore;
    case 684:
      return crit_e::ignore;
    case 510:
      return crit_e::reject;
    case 511:
      return crit_e::reject;
    case 536:
      return crit_e::ignore;
    case 547:
      return crit_e::ignore;
    case 586:
      return crit_e::ignore;
    case 594:
      return crit_e::ignore;
    case 595:
      return crit_e::ignore;
    case 596:
      return crit_e::ignore;
    case 618:
      return crit_e::ignore;
    case 599:
      return crit_e::reject;
    case 600:
      return crit_e::reject;
    case 601:
      return crit_e::reject;
    case 608:
      return crit_e::reject;
    case 609:
      return crit_e::reject;
    case 610:
      return crit_e::reject;
    case 619:
      return crit_e::ignore;
    case 626:
      return crit_e::ignore;
    case 632:
      return crit_e::reject;
    case 633:
      return crit_e::reject;
    case 685:
      return crit_e::reject;
    case 628:
      return crit_e::reject;
    case 643:
      return crit_e::ignore;
    case 647:
      return crit_e::ignore;
    case 675:
      return crit_e::ignore;
    case 683:
      return crit_e::ignore;
    case 695:
      return crit_e::ignore;
    case 690:
      return crit_e::ignore;
    case 691:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_mod_request_ies_o::value_c ue_context_mod_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 63:
      ret.set(value_c::types::sp_cell_id);
      break;
    case 107:
      ret.set(value_c::types::serv_cell_idx);
      break;
    case 96:
      ret.set(value_c::types::sp_cell_ul_cfg);
      break;
    case 38:
      ret.set(value_c::types::drx_cycle);
      break;
    case 9:
      ret.set(value_c::types::cu_to_du_rrc_info);
      break;
    case 79:
      ret.set(value_c::types::tx_action_ind);
      break;
    case 49:
      ret.set(value_c::types::res_coordination_transfer_container);
      break;
    case 87:
      ret.set(value_c::types::rrc_recfg_complete_ind);
      break;
    case 50:
      ret.set(value_c::types::rrc_container);
      break;
    case 56:
      ret.set(value_c::types::scell_to_be_setup_mod_list);
      break;
    case 52:
      ret.set(value_c::types::scell_to_be_remd_list);
      break;
    case 76:
      ret.set(value_c::types::srbs_to_be_setup_mod_list);
      break;
    case 37:
      ret.set(value_c::types::drbs_to_be_setup_mod_list);
      break;
    case 31:
      ret.set(value_c::types::drbs_to_be_modified_list);
      break;
    case 72:
      ret.set(value_c::types::srbs_to_be_released_list);
      break;
    case 33:
      ret.set(value_c::types::drbs_to_be_released_list);
      break;
    case 97:
      ret.set(value_c::types::inactivity_monitoring_request);
      break;
    case 108:
      ret.set(value_c::types::rat_freq_prio_info);
      break;
    case 159:
      ret.set(value_c::types::drx_cfg_ind);
      break;
    case 174:
      ret.set(value_c::types::rlc_fail_ind);
      break;
    case 175:
      ret.set(value_c::types::ul_tx_direct_current_list_info);
      break;
    case 162:
      ret.set(value_c::types::gnb_du_cfg_query);
      break;
    case 158:
      ret.set(value_c::types::gnb_du_ue_ambr_ul);
      break;
    case 109:
      ret.set(value_c::types::execute_dupl);
      break;
    case 184:
      ret.set(value_c::types::rrc_delivery_status_request);
      break;
    case 195:
      ret.set(value_c::types::res_coordination_transfer_info);
      break;
    case 182:
      ret.set(value_c::types::serving_cell_mo);
      break;
    case 215:
      ret.set(value_c::types::needfor_gap);
      break;
    case 94:
      ret.set(value_c::types::full_cfg);
      break;
    case 248:
      ret.set(value_c::types::add_rrm_prio_idx);
      break;
    case 253:
      ret.set(value_c::types::lower_layer_presence_status_change);
      break;
    case 267:
      ret.set(value_c::types::bh_chs_to_be_setup_mod_list);
      break;
    case 263:
      ret.set(value_c::types::bh_chs_to_be_modified_list);
      break;
    case 265:
      ret.set(value_c::types::bh_chs_to_be_released_list);
      break;
    case 306:
      ret.set(value_c::types::nr_v2x_services_authorized);
      break;
    case 307:
      ret.set(value_c::types::ltev2x_services_authorized);
      break;
    case 308:
      ret.set(value_c::types::nr_ue_sidelink_aggr_max_bitrate);
      break;
    case 309:
      ret.set(value_c::types::lte_ue_sidelink_aggr_max_bitrate);
      break;
    case 340:
      ret.set(value_c::types::pc5_link_ambr);
      break;
    case 332:
      ret.set(value_c::types::sl_drbs_to_be_setup_mod_list);
      break;
    case 326:
      ret.set(value_c::types::sl_drbs_to_be_modified_list);
      break;
    case 328:
      ret.set(value_c::types::sl_drbs_to_be_released_list);
      break;
    case 374:
      ret.set(value_c::types::conditional_intra_du_mob_info);
      break;
    case 428:
      ret.set(value_c::types::f1_c_transfer_path);
      break;
    case 432:
      ret.set(value_c::types::scg_ind);
      break;
    case 684:
      ret.set(value_c::types::ul_tx_direct_current_two_carrier_list_info);
      break;
    case 510:
      ret.set(value_c::types::iab_conditional_rrc_msg_delivery_ind);
      break;
    case 511:
      ret.set(value_c::types::f1_c_transfer_path_nr_dc);
      break;
    case 536:
      ret.set(value_c::types::mdt_polluted_meas_ind);
      break;
    case 547:
      ret.set(value_c::types::scg_activation_request);
      break;
    case 586:
      ret.set(value_c::types::cg_sdt_query_ind);
      break;
    case 594:
      ret.set(value_c::types::five_g_pro_se_authorized);
      break;
    case 595:
      ret.set(value_c::types::five_g_pro_se_ue_pc5_aggr_max_bitrate);
      break;
    case 596:
      ret.set(value_c::types::five_g_pro_se_pc5_link_ambr);
      break;
    case 618:
      ret.set(value_c::types::upd_remote_ue_local_id);
      break;
    case 599:
      ret.set(value_c::types::uu_rlc_ch_to_be_setup_list);
      break;
    case 600:
      ret.set(value_c::types::uu_rlc_ch_to_be_modified_list);
      break;
    case 601:
      ret.set(value_c::types::uu_rlc_ch_to_be_released_list);
      break;
    case 608:
      ret.set(value_c::types::pc5_rlc_ch_to_be_setup_list);
      break;
    case 609:
      ret.set(value_c::types::pc5_rlc_ch_to_be_modified_list);
      break;
    case 610:
      ret.set(value_c::types::pc5_rlc_ch_to_be_released_list);
      break;
    case 619:
      ret.set(value_c::types::path_switch_cfg);
      break;
    case 626:
      ret.set(value_c::types::gnb_du_ue_slice_max_bit_rate_list);
      break;
    case 632:
      ret.set(value_c::types::multicast_mbs_session_setup_list);
      break;
    case 633:
      ret.set(value_c::types::multicast_mbs_session_rem_list);
      break;
    case 685:
      ret.set(value_c::types::ue_multicast_m_rbs_to_be_setup_at_modify_list);
      break;
    case 628:
      ret.set(value_c::types::ue_multicast_m_rbs_to_be_released_list);
      break;
    case 643:
      ret.set(value_c::types::sldrx_cycle_list);
      break;
    case 647:
      ret.set(value_c::types::management_based_mdt_plmn_mod_list);
      break;
    case 675:
      ret.set(value_c::types::sdt_bearer_cfg_query_ind);
      break;
    case 683:
      ret.set(value_c::types::daps_ho_status);
      break;
    case 695:
      ret.set(value_c::types::serving_cell_mo_list);
      break;
    case 690:
      ret.set(value_c::types::ul_tx_direct_current_more_carrier_info);
      break;
    case 691:
      ret.set(value_c::types::cp_acmcg_info);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_mod_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 63:
      return presence_e::optional;
    case 107:
      return presence_e::optional;
    case 96:
      return presence_e::optional;
    case 38:
      return presence_e::optional;
    case 9:
      return presence_e::optional;
    case 79:
      return presence_e::optional;
    case 49:
      return presence_e::optional;
    case 87:
      return presence_e::optional;
    case 50:
      return presence_e::optional;
    case 56:
      return presence_e::optional;
    case 52:
      return presence_e::optional;
    case 76:
      return presence_e::optional;
    case 37:
      return presence_e::optional;
    case 31:
      return presence_e::optional;
    case 72:
      return presence_e::optional;
    case 33:
      return presence_e::optional;
    case 97:
      return presence_e::optional;
    case 108:
      return presence_e::optional;
    case 159:
      return presence_e::optional;
    case 174:
      return presence_e::optional;
    case 175:
      return presence_e::optional;
    case 162:
      return presence_e::optional;
    case 158:
      return presence_e::optional;
    case 109:
      return presence_e::optional;
    case 184:
      return presence_e::optional;
    case 195:
      return presence_e::optional;
    case 182:
      return presence_e::optional;
    case 215:
      return presence_e::optional;
    case 94:
      return presence_e::optional;
    case 248:
      return presence_e::optional;
    case 253:
      return presence_e::optional;
    case 267:
      return presence_e::optional;
    case 263:
      return presence_e::optional;
    case 265:
      return presence_e::optional;
    case 306:
      return presence_e::optional;
    case 307:
      return presence_e::optional;
    case 308:
      return presence_e::optional;
    case 309:
      return presence_e::optional;
    case 340:
      return presence_e::optional;
    case 332:
      return presence_e::optional;
    case 326:
      return presence_e::optional;
    case 328:
      return presence_e::optional;
    case 374:
      return presence_e::optional;
    case 428:
      return presence_e::optional;
    case 432:
      return presence_e::optional;
    case 684:
      return presence_e::optional;
    case 510:
      return presence_e::optional;
    case 511:
      return presence_e::optional;
    case 536:
      return presence_e::optional;
    case 547:
      return presence_e::optional;
    case 586:
      return presence_e::optional;
    case 594:
      return presence_e::optional;
    case 595:
      return presence_e::optional;
    case 596:
      return presence_e::optional;
    case 618:
      return presence_e::optional;
    case 599:
      return presence_e::optional;
    case 600:
      return presence_e::optional;
    case 601:
      return presence_e::optional;
    case 608:
      return presence_e::optional;
    case 609:
      return presence_e::optional;
    case 610:
      return presence_e::optional;
    case 619:
      return presence_e::optional;
    case 626:
      return presence_e::optional;
    case 632:
      return presence_e::optional;
    case 633:
      return presence_e::optional;
    case 685:
      return presence_e::optional;
    case 628:
      return presence_e::optional;
    case 643:
      return presence_e::optional;
    case 647:
      return presence_e::optional;
    case 675:
      return presence_e::optional;
    case 683:
      return presence_e::optional;
    case 695:
      return presence_e::optional;
    case 690:
      return presence_e::optional;
    case 691:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_mod_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::sp_cell_id:
      c.destroy<nr_cgi_s>();
      break;
    case types::drx_cycle:
      c.destroy<drx_cycle_s>();
      break;
    case types::cu_to_du_rrc_info:
      c.destroy<cu_to_du_rrc_info_s>();
      break;
    case types::res_coordination_transfer_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::scell_to_be_setup_mod_list:
      c.destroy<scell_to_be_setup_mod_list_l>();
      break;
    case types::scell_to_be_remd_list:
      c.destroy<scell_to_be_remd_list_l>();
      break;
    case types::srbs_to_be_setup_mod_list:
      c.destroy<srbs_to_be_setup_mod_list_l>();
      break;
    case types::drbs_to_be_setup_mod_list:
      c.destroy<drbs_to_be_setup_mod_list_l>();
      break;
    case types::drbs_to_be_modified_list:
      c.destroy<drbs_to_be_modified_list_l>();
      break;
    case types::srbs_to_be_released_list:
      c.destroy<srbs_to_be_released_list_l>();
      break;
    case types::drbs_to_be_released_list:
      c.destroy<drbs_to_be_released_list_l>();
      break;
    case types::rat_freq_prio_info:
      c.destroy<rat_freq_prio_info_c>();
      break;
    case types::rlc_fail_ind:
      c.destroy<rlc_fail_ind_s>();
      break;
    case types::ul_tx_direct_current_list_info:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::res_coordination_transfer_info:
      c.destroy<res_coordination_transfer_info_s>();
      break;
    case types::add_rrm_prio_idx:
      c.destroy<fixed_bitstring<32, false, true>>();
      break;
    case types::bh_chs_to_be_setup_mod_list:
      c.destroy<bh_chs_to_be_setup_mod_list_l>();
      break;
    case types::bh_chs_to_be_modified_list:
      c.destroy<bh_chs_to_be_modified_list_l>();
      break;
    case types::bh_chs_to_be_released_list:
      c.destroy<bh_chs_to_be_released_list_l>();
      break;
    case types::nr_v2x_services_authorized:
      c.destroy<nr_v2x_services_authorized_s>();
      break;
    case types::ltev2x_services_authorized:
      c.destroy<ltev2x_services_authorized_s>();
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.destroy<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.destroy<lte_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      c.destroy<sl_drbs_to_be_setup_mod_list_l>();
      break;
    case types::sl_drbs_to_be_modified_list:
      c.destroy<sl_drbs_to_be_modified_list_l>();
      break;
    case types::sl_drbs_to_be_released_list:
      c.destroy<sl_drbs_to_be_released_list_l>();
      break;
    case types::conditional_intra_du_mob_info:
      c.destroy<conditional_intra_du_mob_info_s>();
      break;
    case types::f1_c_transfer_path:
      c.destroy<f1_c_transfer_path_s>();
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.destroy<f1_c_transfer_path_nr_dc_s>();
      break;
    case types::five_g_pro_se_authorized:
      c.destroy<five_g_pro_se_authorized_s>();
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.destroy<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.destroy<uu_rlc_ch_to_be_setup_list_l>();
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      c.destroy<uu_rlc_ch_to_be_modified_list_l>();
      break;
    case types::uu_rlc_ch_to_be_released_list:
      c.destroy<uu_rlc_ch_to_be_released_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.destroy<pc5_rlc_ch_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      c.destroy<pc5_rlc_ch_to_be_modified_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      c.destroy<pc5_rlc_ch_to_be_released_list_l>();
      break;
    case types::path_switch_cfg:
      c.destroy<path_switch_cfg_s>();
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.destroy<gnb_du_ue_slice_max_bit_rate_list_l>();
      break;
    case types::multicast_mbs_session_setup_list:
      c.destroy<multicast_mbs_session_list_l>();
      break;
    case types::multicast_mbs_session_rem_list:
      c.destroy<multicast_mbs_session_list_l>();
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      c.destroy<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>();
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      c.destroy<ue_multicast_m_rbs_to_be_released_list_l>();
      break;
    case types::sldrx_cycle_list:
      c.destroy<sldrx_cycle_list_l>();
      break;
    case types::management_based_mdt_plmn_mod_list:
      c.destroy<mdt_plmn_mod_list_l>();
      break;
    case types::serving_cell_mo_list:
      c.destroy<serving_cell_mo_list_l>();
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::cp_acmcg_info:
      c.destroy<cp_acmcg_info_s>();
      break;
    default:
      break;
  }
}
void ue_context_mod_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::sp_cell_id:
      c.init<nr_cgi_s>();
      break;
    case types::serv_cell_idx:
      break;
    case types::sp_cell_ul_cfg:
      break;
    case types::drx_cycle:
      c.init<drx_cycle_s>();
      break;
    case types::cu_to_du_rrc_info:
      c.init<cu_to_du_rrc_info_s>();
      break;
    case types::tx_action_ind:
      break;
    case types::res_coordination_transfer_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::rrc_recfg_complete_ind:
      break;
    case types::rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::scell_to_be_setup_mod_list:
      c.init<scell_to_be_setup_mod_list_l>();
      break;
    case types::scell_to_be_remd_list:
      c.init<scell_to_be_remd_list_l>();
      break;
    case types::srbs_to_be_setup_mod_list:
      c.init<srbs_to_be_setup_mod_list_l>();
      break;
    case types::drbs_to_be_setup_mod_list:
      c.init<drbs_to_be_setup_mod_list_l>();
      break;
    case types::drbs_to_be_modified_list:
      c.init<drbs_to_be_modified_list_l>();
      break;
    case types::srbs_to_be_released_list:
      c.init<srbs_to_be_released_list_l>();
      break;
    case types::drbs_to_be_released_list:
      c.init<drbs_to_be_released_list_l>();
      break;
    case types::inactivity_monitoring_request:
      break;
    case types::rat_freq_prio_info:
      c.init<rat_freq_prio_info_c>();
      break;
    case types::drx_cfg_ind:
      break;
    case types::rlc_fail_ind:
      c.init<rlc_fail_ind_s>();
      break;
    case types::ul_tx_direct_current_list_info:
      c.init<unbounded_octstring<true>>();
      break;
    case types::gnb_du_cfg_query:
      break;
    case types::gnb_du_ue_ambr_ul:
      break;
    case types::execute_dupl:
      break;
    case types::rrc_delivery_status_request:
      break;
    case types::res_coordination_transfer_info:
      c.init<res_coordination_transfer_info_s>();
      break;
    case types::serving_cell_mo:
      break;
    case types::needfor_gap:
      break;
    case types::full_cfg:
      break;
    case types::add_rrm_prio_idx:
      c.init<fixed_bitstring<32, false, true>>();
      break;
    case types::lower_layer_presence_status_change:
      break;
    case types::bh_chs_to_be_setup_mod_list:
      c.init<bh_chs_to_be_setup_mod_list_l>();
      break;
    case types::bh_chs_to_be_modified_list:
      c.init<bh_chs_to_be_modified_list_l>();
      break;
    case types::bh_chs_to_be_released_list:
      c.init<bh_chs_to_be_released_list_l>();
      break;
    case types::nr_v2x_services_authorized:
      c.init<nr_v2x_services_authorized_s>();
      break;
    case types::ltev2x_services_authorized:
      c.init<ltev2x_services_authorized_s>();
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.init<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.init<lte_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::pc5_link_ambr:
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      c.init<sl_drbs_to_be_setup_mod_list_l>();
      break;
    case types::sl_drbs_to_be_modified_list:
      c.init<sl_drbs_to_be_modified_list_l>();
      break;
    case types::sl_drbs_to_be_released_list:
      c.init<sl_drbs_to_be_released_list_l>();
      break;
    case types::conditional_intra_du_mob_info:
      c.init<conditional_intra_du_mob_info_s>();
      break;
    case types::f1_c_transfer_path:
      c.init<f1_c_transfer_path_s>();
      break;
    case types::scg_ind:
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      c.init<unbounded_octstring<true>>();
      break;
    case types::iab_conditional_rrc_msg_delivery_ind:
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.init<f1_c_transfer_path_nr_dc_s>();
      break;
    case types::mdt_polluted_meas_ind:
      break;
    case types::scg_activation_request:
      break;
    case types::cg_sdt_query_ind:
      break;
    case types::five_g_pro_se_authorized:
      c.init<five_g_pro_se_authorized_s>();
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.init<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      break;
    case types::upd_remote_ue_local_id:
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.init<uu_rlc_ch_to_be_setup_list_l>();
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      c.init<uu_rlc_ch_to_be_modified_list_l>();
      break;
    case types::uu_rlc_ch_to_be_released_list:
      c.init<uu_rlc_ch_to_be_released_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.init<pc5_rlc_ch_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      c.init<pc5_rlc_ch_to_be_modified_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      c.init<pc5_rlc_ch_to_be_released_list_l>();
      break;
    case types::path_switch_cfg:
      c.init<path_switch_cfg_s>();
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.init<gnb_du_ue_slice_max_bit_rate_list_l>();
      break;
    case types::multicast_mbs_session_setup_list:
      c.init<multicast_mbs_session_list_l>();
      break;
    case types::multicast_mbs_session_rem_list:
      c.init<multicast_mbs_session_list_l>();
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      c.init<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>();
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      c.init<ue_multicast_m_rbs_to_be_released_list_l>();
      break;
    case types::sldrx_cycle_list:
      c.init<sldrx_cycle_list_l>();
      break;
    case types::management_based_mdt_plmn_mod_list:
      c.init<mdt_plmn_mod_list_l>();
      break;
    case types::sdt_bearer_cfg_query_ind:
      break;
    case types::daps_ho_status:
      break;
    case types::serving_cell_mo_list:
      c.init<serving_cell_mo_list_l>();
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      c.init<unbounded_octstring<true>>();
      break;
    case types::cp_acmcg_info:
      c.init<cp_acmcg_info_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_request_ies_o::value_c");
  }
}
ue_context_mod_request_ies_o::value_c::value_c(const ue_context_mod_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sp_cell_id:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::serv_cell_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sp_cell_ul_cfg:
      c.init(other.c.get<cell_ul_cfg_e>());
      break;
    case types::drx_cycle:
      c.init(other.c.get<drx_cycle_s>());
      break;
    case types::cu_to_du_rrc_info:
      c.init(other.c.get<cu_to_du_rrc_info_s>());
      break;
    case types::tx_action_ind:
      c.init(other.c.get<tx_action_ind_e>());
      break;
    case types::res_coordination_transfer_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::rrc_recfg_complete_ind:
      c.init(other.c.get<rrc_recfg_complete_ind_e>());
      break;
    case types::rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::scell_to_be_setup_mod_list:
      c.init(other.c.get<scell_to_be_setup_mod_list_l>());
      break;
    case types::scell_to_be_remd_list:
      c.init(other.c.get<scell_to_be_remd_list_l>());
      break;
    case types::srbs_to_be_setup_mod_list:
      c.init(other.c.get<srbs_to_be_setup_mod_list_l>());
      break;
    case types::drbs_to_be_setup_mod_list:
      c.init(other.c.get<drbs_to_be_setup_mod_list_l>());
      break;
    case types::drbs_to_be_modified_list:
      c.init(other.c.get<drbs_to_be_modified_list_l>());
      break;
    case types::srbs_to_be_released_list:
      c.init(other.c.get<srbs_to_be_released_list_l>());
      break;
    case types::drbs_to_be_released_list:
      c.init(other.c.get<drbs_to_be_released_list_l>());
      break;
    case types::inactivity_monitoring_request:
      c.init(other.c.get<inactivity_monitoring_request_e>());
      break;
    case types::rat_freq_prio_info:
      c.init(other.c.get<rat_freq_prio_info_c>());
      break;
    case types::drx_cfg_ind:
      c.init(other.c.get<drx_cfg_ind_e>());
      break;
    case types::rlc_fail_ind:
      c.init(other.c.get<rlc_fail_ind_s>());
      break;
    case types::ul_tx_direct_current_list_info:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::gnb_du_cfg_query:
      c.init(other.c.get<gnb_du_cfg_query_e>());
      break;
    case types::gnb_du_ue_ambr_ul:
      c.init(other.c.get<uint64_t>());
      break;
    case types::execute_dupl:
      c.init(other.c.get<execute_dupl_e>());
      break;
    case types::rrc_delivery_status_request:
      c.init(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::res_coordination_transfer_info:
      c.init(other.c.get<res_coordination_transfer_info_s>());
      break;
    case types::serving_cell_mo:
      c.init(other.c.get<uint8_t>());
      break;
    case types::needfor_gap:
      c.init(other.c.get<needfor_gap_e>());
      break;
    case types::full_cfg:
      c.init(other.c.get<full_cfg_e>());
      break;
    case types::add_rrm_prio_idx:
      c.init(other.c.get<fixed_bitstring<32, false, true>>());
      break;
    case types::lower_layer_presence_status_change:
      c.init(other.c.get<lower_layer_presence_status_change_e>());
      break;
    case types::bh_chs_to_be_setup_mod_list:
      c.init(other.c.get<bh_chs_to_be_setup_mod_list_l>());
      break;
    case types::bh_chs_to_be_modified_list:
      c.init(other.c.get<bh_chs_to_be_modified_list_l>());
      break;
    case types::bh_chs_to_be_released_list:
      c.init(other.c.get<bh_chs_to_be_released_list_l>());
      break;
    case types::nr_v2x_services_authorized:
      c.init(other.c.get<nr_v2x_services_authorized_s>());
      break;
    case types::ltev2x_services_authorized:
      c.init(other.c.get<ltev2x_services_authorized_s>());
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.init(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.init(other.c.get<lte_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::pc5_link_ambr:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      c.init(other.c.get<sl_drbs_to_be_setup_mod_list_l>());
      break;
    case types::sl_drbs_to_be_modified_list:
      c.init(other.c.get<sl_drbs_to_be_modified_list_l>());
      break;
    case types::sl_drbs_to_be_released_list:
      c.init(other.c.get<sl_drbs_to_be_released_list_l>());
      break;
    case types::conditional_intra_du_mob_info:
      c.init(other.c.get<conditional_intra_du_mob_info_s>());
      break;
    case types::f1_c_transfer_path:
      c.init(other.c.get<f1_c_transfer_path_s>());
      break;
    case types::scg_ind:
      c.init(other.c.get<scg_ind_e>());
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::iab_conditional_rrc_msg_delivery_ind:
      c.init(other.c.get<iab_conditional_rrc_msg_delivery_ind_e>());
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.init(other.c.get<f1_c_transfer_path_nr_dc_s>());
      break;
    case types::mdt_polluted_meas_ind:
      c.init(other.c.get<mdt_polluted_meas_ind_e>());
      break;
    case types::scg_activation_request:
      c.init(other.c.get<scg_activation_request_e>());
      break;
    case types::cg_sdt_query_ind:
      c.init(other.c.get<cg_sdt_query_ind_e>());
      break;
    case types::five_g_pro_se_authorized:
      c.init(other.c.get<five_g_pro_se_authorized_s>());
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.init(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      c.init(other.c.get<uint64_t>());
      break;
    case types::upd_remote_ue_local_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.init(other.c.get<uu_rlc_ch_to_be_setup_list_l>());
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      c.init(other.c.get<uu_rlc_ch_to_be_modified_list_l>());
      break;
    case types::uu_rlc_ch_to_be_released_list:
      c.init(other.c.get<uu_rlc_ch_to_be_released_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.init(other.c.get<pc5_rlc_ch_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      c.init(other.c.get<pc5_rlc_ch_to_be_modified_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      c.init(other.c.get<pc5_rlc_ch_to_be_released_list_l>());
      break;
    case types::path_switch_cfg:
      c.init(other.c.get<path_switch_cfg_s>());
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.init(other.c.get<gnb_du_ue_slice_max_bit_rate_list_l>());
      break;
    case types::multicast_mbs_session_setup_list:
      c.init(other.c.get<multicast_mbs_session_list_l>());
      break;
    case types::multicast_mbs_session_rem_list:
      c.init(other.c.get<multicast_mbs_session_list_l>());
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      c.init(other.c.get<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>());
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      c.init(other.c.get<ue_multicast_m_rbs_to_be_released_list_l>());
      break;
    case types::sldrx_cycle_list:
      c.init(other.c.get<sldrx_cycle_list_l>());
      break;
    case types::management_based_mdt_plmn_mod_list:
      c.init(other.c.get<mdt_plmn_mod_list_l>());
      break;
    case types::sdt_bearer_cfg_query_ind:
      c.init(other.c.get<sdt_bearer_cfg_query_ind_e>());
      break;
    case types::daps_ho_status:
      c.init(other.c.get<daps_ho_status_e>());
      break;
    case types::serving_cell_mo_list:
      c.init(other.c.get<serving_cell_mo_list_l>());
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::cp_acmcg_info:
      c.init(other.c.get<cp_acmcg_info_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_request_ies_o::value_c");
  }
}
ue_context_mod_request_ies_o::value_c&
ue_context_mod_request_ies_o::value_c::operator=(const ue_context_mod_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sp_cell_id:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::serv_cell_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sp_cell_ul_cfg:
      c.set(other.c.get<cell_ul_cfg_e>());
      break;
    case types::drx_cycle:
      c.set(other.c.get<drx_cycle_s>());
      break;
    case types::cu_to_du_rrc_info:
      c.set(other.c.get<cu_to_du_rrc_info_s>());
      break;
    case types::tx_action_ind:
      c.set(other.c.get<tx_action_ind_e>());
      break;
    case types::res_coordination_transfer_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::rrc_recfg_complete_ind:
      c.set(other.c.get<rrc_recfg_complete_ind_e>());
      break;
    case types::rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::scell_to_be_setup_mod_list:
      c.set(other.c.get<scell_to_be_setup_mod_list_l>());
      break;
    case types::scell_to_be_remd_list:
      c.set(other.c.get<scell_to_be_remd_list_l>());
      break;
    case types::srbs_to_be_setup_mod_list:
      c.set(other.c.get<srbs_to_be_setup_mod_list_l>());
      break;
    case types::drbs_to_be_setup_mod_list:
      c.set(other.c.get<drbs_to_be_setup_mod_list_l>());
      break;
    case types::drbs_to_be_modified_list:
      c.set(other.c.get<drbs_to_be_modified_list_l>());
      break;
    case types::srbs_to_be_released_list:
      c.set(other.c.get<srbs_to_be_released_list_l>());
      break;
    case types::drbs_to_be_released_list:
      c.set(other.c.get<drbs_to_be_released_list_l>());
      break;
    case types::inactivity_monitoring_request:
      c.set(other.c.get<inactivity_monitoring_request_e>());
      break;
    case types::rat_freq_prio_info:
      c.set(other.c.get<rat_freq_prio_info_c>());
      break;
    case types::drx_cfg_ind:
      c.set(other.c.get<drx_cfg_ind_e>());
      break;
    case types::rlc_fail_ind:
      c.set(other.c.get<rlc_fail_ind_s>());
      break;
    case types::ul_tx_direct_current_list_info:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::gnb_du_cfg_query:
      c.set(other.c.get<gnb_du_cfg_query_e>());
      break;
    case types::gnb_du_ue_ambr_ul:
      c.set(other.c.get<uint64_t>());
      break;
    case types::execute_dupl:
      c.set(other.c.get<execute_dupl_e>());
      break;
    case types::rrc_delivery_status_request:
      c.set(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::res_coordination_transfer_info:
      c.set(other.c.get<res_coordination_transfer_info_s>());
      break;
    case types::serving_cell_mo:
      c.set(other.c.get<uint8_t>());
      break;
    case types::needfor_gap:
      c.set(other.c.get<needfor_gap_e>());
      break;
    case types::full_cfg:
      c.set(other.c.get<full_cfg_e>());
      break;
    case types::add_rrm_prio_idx:
      c.set(other.c.get<fixed_bitstring<32, false, true>>());
      break;
    case types::lower_layer_presence_status_change:
      c.set(other.c.get<lower_layer_presence_status_change_e>());
      break;
    case types::bh_chs_to_be_setup_mod_list:
      c.set(other.c.get<bh_chs_to_be_setup_mod_list_l>());
      break;
    case types::bh_chs_to_be_modified_list:
      c.set(other.c.get<bh_chs_to_be_modified_list_l>());
      break;
    case types::bh_chs_to_be_released_list:
      c.set(other.c.get<bh_chs_to_be_released_list_l>());
      break;
    case types::nr_v2x_services_authorized:
      c.set(other.c.get<nr_v2x_services_authorized_s>());
      break;
    case types::ltev2x_services_authorized:
      c.set(other.c.get<ltev2x_services_authorized_s>());
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.set(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.set(other.c.get<lte_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::pc5_link_ambr:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      c.set(other.c.get<sl_drbs_to_be_setup_mod_list_l>());
      break;
    case types::sl_drbs_to_be_modified_list:
      c.set(other.c.get<sl_drbs_to_be_modified_list_l>());
      break;
    case types::sl_drbs_to_be_released_list:
      c.set(other.c.get<sl_drbs_to_be_released_list_l>());
      break;
    case types::conditional_intra_du_mob_info:
      c.set(other.c.get<conditional_intra_du_mob_info_s>());
      break;
    case types::f1_c_transfer_path:
      c.set(other.c.get<f1_c_transfer_path_s>());
      break;
    case types::scg_ind:
      c.set(other.c.get<scg_ind_e>());
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::iab_conditional_rrc_msg_delivery_ind:
      c.set(other.c.get<iab_conditional_rrc_msg_delivery_ind_e>());
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.set(other.c.get<f1_c_transfer_path_nr_dc_s>());
      break;
    case types::mdt_polluted_meas_ind:
      c.set(other.c.get<mdt_polluted_meas_ind_e>());
      break;
    case types::scg_activation_request:
      c.set(other.c.get<scg_activation_request_e>());
      break;
    case types::cg_sdt_query_ind:
      c.set(other.c.get<cg_sdt_query_ind_e>());
      break;
    case types::five_g_pro_se_authorized:
      c.set(other.c.get<five_g_pro_se_authorized_s>());
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.set(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      c.set(other.c.get<uint64_t>());
      break;
    case types::upd_remote_ue_local_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.set(other.c.get<uu_rlc_ch_to_be_setup_list_l>());
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      c.set(other.c.get<uu_rlc_ch_to_be_modified_list_l>());
      break;
    case types::uu_rlc_ch_to_be_released_list:
      c.set(other.c.get<uu_rlc_ch_to_be_released_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.set(other.c.get<pc5_rlc_ch_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      c.set(other.c.get<pc5_rlc_ch_to_be_modified_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      c.set(other.c.get<pc5_rlc_ch_to_be_released_list_l>());
      break;
    case types::path_switch_cfg:
      c.set(other.c.get<path_switch_cfg_s>());
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.set(other.c.get<gnb_du_ue_slice_max_bit_rate_list_l>());
      break;
    case types::multicast_mbs_session_setup_list:
      c.set(other.c.get<multicast_mbs_session_list_l>());
      break;
    case types::multicast_mbs_session_rem_list:
      c.set(other.c.get<multicast_mbs_session_list_l>());
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      c.set(other.c.get<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>());
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      c.set(other.c.get<ue_multicast_m_rbs_to_be_released_list_l>());
      break;
    case types::sldrx_cycle_list:
      c.set(other.c.get<sldrx_cycle_list_l>());
      break;
    case types::management_based_mdt_plmn_mod_list:
      c.set(other.c.get<mdt_plmn_mod_list_l>());
      break;
    case types::sdt_bearer_cfg_query_ind:
      c.set(other.c.get<sdt_bearer_cfg_query_ind_e>());
      break;
    case types::daps_ho_status:
      c.set(other.c.get<daps_ho_status_e>());
      break;
    case types::serving_cell_mo_list:
      c.set(other.c.get<serving_cell_mo_list_l>());
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::cp_acmcg_info:
      c.set(other.c.get<cp_acmcg_info_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_request_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_mod_request_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_mod_request_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
nr_cgi_s& ue_context_mod_request_ies_o::value_c::sp_cell_id()
{
  assert_choice_type(types::sp_cell_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
uint8_t& ue_context_mod_request_ies_o::value_c::serv_cell_idx()
{
  assert_choice_type(types::serv_cell_idx, type_, "Value");
  return c.get<uint8_t>();
}
cell_ul_cfg_e& ue_context_mod_request_ies_o::value_c::sp_cell_ul_cfg()
{
  assert_choice_type(types::sp_cell_ul_cfg, type_, "Value");
  return c.get<cell_ul_cfg_e>();
}
drx_cycle_s& ue_context_mod_request_ies_o::value_c::drx_cycle()
{
  assert_choice_type(types::drx_cycle, type_, "Value");
  return c.get<drx_cycle_s>();
}
cu_to_du_rrc_info_s& ue_context_mod_request_ies_o::value_c::cu_to_du_rrc_info()
{
  assert_choice_type(types::cu_to_du_rrc_info, type_, "Value");
  return c.get<cu_to_du_rrc_info_s>();
}
tx_action_ind_e& ue_context_mod_request_ies_o::value_c::tx_action_ind()
{
  assert_choice_type(types::tx_action_ind, type_, "Value");
  return c.get<tx_action_ind_e>();
}
unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::res_coordination_transfer_container()
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
rrc_recfg_complete_ind_e& ue_context_mod_request_ies_o::value_c::rrc_recfg_complete_ind()
{
  assert_choice_type(types::rrc_recfg_complete_ind, type_, "Value");
  return c.get<rrc_recfg_complete_ind_e>();
}
unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::rrc_container()
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
scell_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::scell_to_be_setup_mod_list()
{
  assert_choice_type(types::scell_to_be_setup_mod_list, type_, "Value");
  return c.get<scell_to_be_setup_mod_list_l>();
}
scell_to_be_remd_list_l& ue_context_mod_request_ies_o::value_c::scell_to_be_remd_list()
{
  assert_choice_type(types::scell_to_be_remd_list, type_, "Value");
  return c.get<scell_to_be_remd_list_l>();
}
srbs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::srbs_to_be_setup_mod_list()
{
  assert_choice_type(types::srbs_to_be_setup_mod_list, type_, "Value");
  return c.get<srbs_to_be_setup_mod_list_l>();
}
drbs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::drbs_to_be_setup_mod_list()
{
  assert_choice_type(types::drbs_to_be_setup_mod_list, type_, "Value");
  return c.get<drbs_to_be_setup_mod_list_l>();
}
drbs_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::drbs_to_be_modified_list()
{
  assert_choice_type(types::drbs_to_be_modified_list, type_, "Value");
  return c.get<drbs_to_be_modified_list_l>();
}
srbs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::srbs_to_be_released_list()
{
  assert_choice_type(types::srbs_to_be_released_list, type_, "Value");
  return c.get<srbs_to_be_released_list_l>();
}
drbs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::drbs_to_be_released_list()
{
  assert_choice_type(types::drbs_to_be_released_list, type_, "Value");
  return c.get<drbs_to_be_released_list_l>();
}
inactivity_monitoring_request_e& ue_context_mod_request_ies_o::value_c::inactivity_monitoring_request()
{
  assert_choice_type(types::inactivity_monitoring_request, type_, "Value");
  return c.get<inactivity_monitoring_request_e>();
}
rat_freq_prio_info_c& ue_context_mod_request_ies_o::value_c::rat_freq_prio_info()
{
  assert_choice_type(types::rat_freq_prio_info, type_, "Value");
  return c.get<rat_freq_prio_info_c>();
}
drx_cfg_ind_e& ue_context_mod_request_ies_o::value_c::drx_cfg_ind()
{
  assert_choice_type(types::drx_cfg_ind, type_, "Value");
  return c.get<drx_cfg_ind_e>();
}
rlc_fail_ind_s& ue_context_mod_request_ies_o::value_c::rlc_fail_ind()
{
  assert_choice_type(types::rlc_fail_ind, type_, "Value");
  return c.get<rlc_fail_ind_s>();
}
unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::ul_tx_direct_current_list_info()
{
  assert_choice_type(types::ul_tx_direct_current_list_info, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
gnb_du_cfg_query_e& ue_context_mod_request_ies_o::value_c::gnb_du_cfg_query()
{
  assert_choice_type(types::gnb_du_cfg_query, type_, "Value");
  return c.get<gnb_du_cfg_query_e>();
}
uint64_t& ue_context_mod_request_ies_o::value_c::gnb_du_ue_ambr_ul()
{
  assert_choice_type(types::gnb_du_ue_ambr_ul, type_, "Value");
  return c.get<uint64_t>();
}
execute_dupl_e& ue_context_mod_request_ies_o::value_c::execute_dupl()
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
rrc_delivery_status_request_e& ue_context_mod_request_ies_o::value_c::rrc_delivery_status_request()
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
res_coordination_transfer_info_s& ue_context_mod_request_ies_o::value_c::res_coordination_transfer_info()
{
  assert_choice_type(types::res_coordination_transfer_info, type_, "Value");
  return c.get<res_coordination_transfer_info_s>();
}
uint8_t& ue_context_mod_request_ies_o::value_c::serving_cell_mo()
{
  assert_choice_type(types::serving_cell_mo, type_, "Value");
  return c.get<uint8_t>();
}
needfor_gap_e& ue_context_mod_request_ies_o::value_c::needfor_gap()
{
  assert_choice_type(types::needfor_gap, type_, "Value");
  return c.get<needfor_gap_e>();
}
full_cfg_e& ue_context_mod_request_ies_o::value_c::full_cfg()
{
  assert_choice_type(types::full_cfg, type_, "Value");
  return c.get<full_cfg_e>();
}
fixed_bitstring<32, false, true>& ue_context_mod_request_ies_o::value_c::add_rrm_prio_idx()
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
lower_layer_presence_status_change_e& ue_context_mod_request_ies_o::value_c::lower_layer_presence_status_change()
{
  assert_choice_type(types::lower_layer_presence_status_change, type_, "Value");
  return c.get<lower_layer_presence_status_change_e>();
}
bh_chs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::bh_chs_to_be_setup_mod_list()
{
  assert_choice_type(types::bh_chs_to_be_setup_mod_list, type_, "Value");
  return c.get<bh_chs_to_be_setup_mod_list_l>();
}
bh_chs_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::bh_chs_to_be_modified_list()
{
  assert_choice_type(types::bh_chs_to_be_modified_list, type_, "Value");
  return c.get<bh_chs_to_be_modified_list_l>();
}
bh_chs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::bh_chs_to_be_released_list()
{
  assert_choice_type(types::bh_chs_to_be_released_list, type_, "Value");
  return c.get<bh_chs_to_be_released_list_l>();
}
nr_v2x_services_authorized_s& ue_context_mod_request_ies_o::value_c::nr_v2x_services_authorized()
{
  assert_choice_type(types::nr_v2x_services_authorized, type_, "Value");
  return c.get<nr_v2x_services_authorized_s>();
}
ltev2x_services_authorized_s& ue_context_mod_request_ies_o::value_c::ltev2x_services_authorized()
{
  assert_choice_type(types::ltev2x_services_authorized, type_, "Value");
  return c.get<ltev2x_services_authorized_s>();
}
nr_ue_sidelink_aggr_max_bitrate_s& ue_context_mod_request_ies_o::value_c::nr_ue_sidelink_aggr_max_bitrate()
{
  assert_choice_type(types::nr_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
lte_ue_sidelink_aggr_max_bitrate_s& ue_context_mod_request_ies_o::value_c::lte_ue_sidelink_aggr_max_bitrate()
{
  assert_choice_type(types::lte_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<lte_ue_sidelink_aggr_max_bitrate_s>();
}
uint64_t& ue_context_mod_request_ies_o::value_c::pc5_link_ambr()
{
  assert_choice_type(types::pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
sl_drbs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::sl_drbs_to_be_setup_mod_list()
{
  assert_choice_type(types::sl_drbs_to_be_setup_mod_list, type_, "Value");
  return c.get<sl_drbs_to_be_setup_mod_list_l>();
}
sl_drbs_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::sl_drbs_to_be_modified_list()
{
  assert_choice_type(types::sl_drbs_to_be_modified_list, type_, "Value");
  return c.get<sl_drbs_to_be_modified_list_l>();
}
sl_drbs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::sl_drbs_to_be_released_list()
{
  assert_choice_type(types::sl_drbs_to_be_released_list, type_, "Value");
  return c.get<sl_drbs_to_be_released_list_l>();
}
conditional_intra_du_mob_info_s& ue_context_mod_request_ies_o::value_c::conditional_intra_du_mob_info()
{
  assert_choice_type(types::conditional_intra_du_mob_info, type_, "Value");
  return c.get<conditional_intra_du_mob_info_s>();
}
f1_c_transfer_path_s& ue_context_mod_request_ies_o::value_c::f1_c_transfer_path()
{
  assert_choice_type(types::f1_c_transfer_path, type_, "Value");
  return c.get<f1_c_transfer_path_s>();
}
scg_ind_e& ue_context_mod_request_ies_o::value_c::scg_ind()
{
  assert_choice_type(types::scg_ind, type_, "Value");
  return c.get<scg_ind_e>();
}
unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::ul_tx_direct_current_two_carrier_list_info()
{
  assert_choice_type(types::ul_tx_direct_current_two_carrier_list_info, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
iab_conditional_rrc_msg_delivery_ind_e& ue_context_mod_request_ies_o::value_c::iab_conditional_rrc_msg_delivery_ind()
{
  assert_choice_type(types::iab_conditional_rrc_msg_delivery_ind, type_, "Value");
  return c.get<iab_conditional_rrc_msg_delivery_ind_e>();
}
f1_c_transfer_path_nr_dc_s& ue_context_mod_request_ies_o::value_c::f1_c_transfer_path_nr_dc()
{
  assert_choice_type(types::f1_c_transfer_path_nr_dc, type_, "Value");
  return c.get<f1_c_transfer_path_nr_dc_s>();
}
mdt_polluted_meas_ind_e& ue_context_mod_request_ies_o::value_c::mdt_polluted_meas_ind()
{
  assert_choice_type(types::mdt_polluted_meas_ind, type_, "Value");
  return c.get<mdt_polluted_meas_ind_e>();
}
scg_activation_request_e& ue_context_mod_request_ies_o::value_c::scg_activation_request()
{
  assert_choice_type(types::scg_activation_request, type_, "Value");
  return c.get<scg_activation_request_e>();
}
cg_sdt_query_ind_e& ue_context_mod_request_ies_o::value_c::cg_sdt_query_ind()
{
  assert_choice_type(types::cg_sdt_query_ind, type_, "Value");
  return c.get<cg_sdt_query_ind_e>();
}
five_g_pro_se_authorized_s& ue_context_mod_request_ies_o::value_c::five_g_pro_se_authorized()
{
  assert_choice_type(types::five_g_pro_se_authorized, type_, "Value");
  return c.get<five_g_pro_se_authorized_s>();
}
nr_ue_sidelink_aggr_max_bitrate_s& ue_context_mod_request_ies_o::value_c::five_g_pro_se_ue_pc5_aggr_max_bitrate()
{
  assert_choice_type(types::five_g_pro_se_ue_pc5_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
uint64_t& ue_context_mod_request_ies_o::value_c::five_g_pro_se_pc5_link_ambr()
{
  assert_choice_type(types::five_g_pro_se_pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
uint16_t& ue_context_mod_request_ies_o::value_c::upd_remote_ue_local_id()
{
  assert_choice_type(types::upd_remote_ue_local_id, type_, "Value");
  return c.get<uint16_t>();
}
uu_rlc_ch_to_be_setup_list_l& ue_context_mod_request_ies_o::value_c::uu_rlc_ch_to_be_setup_list()
{
  assert_choice_type(types::uu_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_setup_list_l>();
}
uu_rlc_ch_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::uu_rlc_ch_to_be_modified_list()
{
  assert_choice_type(types::uu_rlc_ch_to_be_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_modified_list_l>();
}
uu_rlc_ch_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::uu_rlc_ch_to_be_released_list()
{
  assert_choice_type(types::uu_rlc_ch_to_be_released_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_released_list_l>();
}
pc5_rlc_ch_to_be_setup_list_l& ue_context_mod_request_ies_o::value_c::pc5_rlc_ch_to_be_setup_list()
{
  assert_choice_type(types::pc5_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_setup_list_l>();
}
pc5_rlc_ch_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::pc5_rlc_ch_to_be_modified_list()
{
  assert_choice_type(types::pc5_rlc_ch_to_be_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_modified_list_l>();
}
pc5_rlc_ch_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::pc5_rlc_ch_to_be_released_list()
{
  assert_choice_type(types::pc5_rlc_ch_to_be_released_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_released_list_l>();
}
path_switch_cfg_s& ue_context_mod_request_ies_o::value_c::path_switch_cfg()
{
  assert_choice_type(types::path_switch_cfg, type_, "Value");
  return c.get<path_switch_cfg_s>();
}
gnb_du_ue_slice_max_bit_rate_list_l& ue_context_mod_request_ies_o::value_c::gnb_du_ue_slice_max_bit_rate_list()
{
  assert_choice_type(types::gnb_du_ue_slice_max_bit_rate_list, type_, "Value");
  return c.get<gnb_du_ue_slice_max_bit_rate_list_l>();
}
multicast_mbs_session_list_l& ue_context_mod_request_ies_o::value_c::multicast_mbs_session_setup_list()
{
  assert_choice_type(types::multicast_mbs_session_setup_list, type_, "Value");
  return c.get<multicast_mbs_session_list_l>();
}
multicast_mbs_session_list_l& ue_context_mod_request_ies_o::value_c::multicast_mbs_session_rem_list()
{
  assert_choice_type(types::multicast_mbs_session_rem_list, type_, "Value");
  return c.get<multicast_mbs_session_list_l>();
}
ue_multicast_m_rbs_to_be_setup_at_modify_list_l&
ue_context_mod_request_ies_o::value_c::ue_multicast_m_rbs_to_be_setup_at_modify_list()
{
  assert_choice_type(types::ue_multicast_m_rbs_to_be_setup_at_modify_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>();
}
ue_multicast_m_rbs_to_be_released_list_l&
ue_context_mod_request_ies_o::value_c::ue_multicast_m_rbs_to_be_released_list()
{
  assert_choice_type(types::ue_multicast_m_rbs_to_be_released_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_to_be_released_list_l>();
}
sldrx_cycle_list_l& ue_context_mod_request_ies_o::value_c::sldrx_cycle_list()
{
  assert_choice_type(types::sldrx_cycle_list, type_, "Value");
  return c.get<sldrx_cycle_list_l>();
}
mdt_plmn_mod_list_l& ue_context_mod_request_ies_o::value_c::management_based_mdt_plmn_mod_list()
{
  assert_choice_type(types::management_based_mdt_plmn_mod_list, type_, "Value");
  return c.get<mdt_plmn_mod_list_l>();
}
sdt_bearer_cfg_query_ind_e& ue_context_mod_request_ies_o::value_c::sdt_bearer_cfg_query_ind()
{
  assert_choice_type(types::sdt_bearer_cfg_query_ind, type_, "Value");
  return c.get<sdt_bearer_cfg_query_ind_e>();
}
daps_ho_status_e& ue_context_mod_request_ies_o::value_c::daps_ho_status()
{
  assert_choice_type(types::daps_ho_status, type_, "Value");
  return c.get<daps_ho_status_e>();
}
serving_cell_mo_list_l& ue_context_mod_request_ies_o::value_c::serving_cell_mo_list()
{
  assert_choice_type(types::serving_cell_mo_list, type_, "Value");
  return c.get<serving_cell_mo_list_l>();
}
unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::ul_tx_direct_current_more_carrier_info()
{
  assert_choice_type(types::ul_tx_direct_current_more_carrier_info, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
cp_acmcg_info_s& ue_context_mod_request_ies_o::value_c::cp_acmcg_info()
{
  assert_choice_type(types::cp_acmcg_info, type_, "Value");
  return c.get<cp_acmcg_info_s>();
}
const uint64_t& ue_context_mod_request_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_mod_request_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const nr_cgi_s& ue_context_mod_request_ies_o::value_c::sp_cell_id() const
{
  assert_choice_type(types::sp_cell_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
const uint8_t& ue_context_mod_request_ies_o::value_c::serv_cell_idx() const
{
  assert_choice_type(types::serv_cell_idx, type_, "Value");
  return c.get<uint8_t>();
}
const cell_ul_cfg_e& ue_context_mod_request_ies_o::value_c::sp_cell_ul_cfg() const
{
  assert_choice_type(types::sp_cell_ul_cfg, type_, "Value");
  return c.get<cell_ul_cfg_e>();
}
const drx_cycle_s& ue_context_mod_request_ies_o::value_c::drx_cycle() const
{
  assert_choice_type(types::drx_cycle, type_, "Value");
  return c.get<drx_cycle_s>();
}
const cu_to_du_rrc_info_s& ue_context_mod_request_ies_o::value_c::cu_to_du_rrc_info() const
{
  assert_choice_type(types::cu_to_du_rrc_info, type_, "Value");
  return c.get<cu_to_du_rrc_info_s>();
}
const tx_action_ind_e& ue_context_mod_request_ies_o::value_c::tx_action_ind() const
{
  assert_choice_type(types::tx_action_ind, type_, "Value");
  return c.get<tx_action_ind_e>();
}
const unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::res_coordination_transfer_container() const
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const rrc_recfg_complete_ind_e& ue_context_mod_request_ies_o::value_c::rrc_recfg_complete_ind() const
{
  assert_choice_type(types::rrc_recfg_complete_ind, type_, "Value");
  return c.get<rrc_recfg_complete_ind_e>();
}
const unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::rrc_container() const
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const scell_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::scell_to_be_setup_mod_list() const
{
  assert_choice_type(types::scell_to_be_setup_mod_list, type_, "Value");
  return c.get<scell_to_be_setup_mod_list_l>();
}
const scell_to_be_remd_list_l& ue_context_mod_request_ies_o::value_c::scell_to_be_remd_list() const
{
  assert_choice_type(types::scell_to_be_remd_list, type_, "Value");
  return c.get<scell_to_be_remd_list_l>();
}
const srbs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::srbs_to_be_setup_mod_list() const
{
  assert_choice_type(types::srbs_to_be_setup_mod_list, type_, "Value");
  return c.get<srbs_to_be_setup_mod_list_l>();
}
const drbs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::drbs_to_be_setup_mod_list() const
{
  assert_choice_type(types::drbs_to_be_setup_mod_list, type_, "Value");
  return c.get<drbs_to_be_setup_mod_list_l>();
}
const drbs_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::drbs_to_be_modified_list() const
{
  assert_choice_type(types::drbs_to_be_modified_list, type_, "Value");
  return c.get<drbs_to_be_modified_list_l>();
}
const srbs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::srbs_to_be_released_list() const
{
  assert_choice_type(types::srbs_to_be_released_list, type_, "Value");
  return c.get<srbs_to_be_released_list_l>();
}
const drbs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::drbs_to_be_released_list() const
{
  assert_choice_type(types::drbs_to_be_released_list, type_, "Value");
  return c.get<drbs_to_be_released_list_l>();
}
const inactivity_monitoring_request_e& ue_context_mod_request_ies_o::value_c::inactivity_monitoring_request() const
{
  assert_choice_type(types::inactivity_monitoring_request, type_, "Value");
  return c.get<inactivity_monitoring_request_e>();
}
const rat_freq_prio_info_c& ue_context_mod_request_ies_o::value_c::rat_freq_prio_info() const
{
  assert_choice_type(types::rat_freq_prio_info, type_, "Value");
  return c.get<rat_freq_prio_info_c>();
}
const drx_cfg_ind_e& ue_context_mod_request_ies_o::value_c::drx_cfg_ind() const
{
  assert_choice_type(types::drx_cfg_ind, type_, "Value");
  return c.get<drx_cfg_ind_e>();
}
const rlc_fail_ind_s& ue_context_mod_request_ies_o::value_c::rlc_fail_ind() const
{
  assert_choice_type(types::rlc_fail_ind, type_, "Value");
  return c.get<rlc_fail_ind_s>();
}
const unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::ul_tx_direct_current_list_info() const
{
  assert_choice_type(types::ul_tx_direct_current_list_info, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const gnb_du_cfg_query_e& ue_context_mod_request_ies_o::value_c::gnb_du_cfg_query() const
{
  assert_choice_type(types::gnb_du_cfg_query, type_, "Value");
  return c.get<gnb_du_cfg_query_e>();
}
const uint64_t& ue_context_mod_request_ies_o::value_c::gnb_du_ue_ambr_ul() const
{
  assert_choice_type(types::gnb_du_ue_ambr_ul, type_, "Value");
  return c.get<uint64_t>();
}
const execute_dupl_e& ue_context_mod_request_ies_o::value_c::execute_dupl() const
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
const rrc_delivery_status_request_e& ue_context_mod_request_ies_o::value_c::rrc_delivery_status_request() const
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
const res_coordination_transfer_info_s& ue_context_mod_request_ies_o::value_c::res_coordination_transfer_info() const
{
  assert_choice_type(types::res_coordination_transfer_info, type_, "Value");
  return c.get<res_coordination_transfer_info_s>();
}
const uint8_t& ue_context_mod_request_ies_o::value_c::serving_cell_mo() const
{
  assert_choice_type(types::serving_cell_mo, type_, "Value");
  return c.get<uint8_t>();
}
const needfor_gap_e& ue_context_mod_request_ies_o::value_c::needfor_gap() const
{
  assert_choice_type(types::needfor_gap, type_, "Value");
  return c.get<needfor_gap_e>();
}
const full_cfg_e& ue_context_mod_request_ies_o::value_c::full_cfg() const
{
  assert_choice_type(types::full_cfg, type_, "Value");
  return c.get<full_cfg_e>();
}
const fixed_bitstring<32, false, true>& ue_context_mod_request_ies_o::value_c::add_rrm_prio_idx() const
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
const lower_layer_presence_status_change_e&
ue_context_mod_request_ies_o::value_c::lower_layer_presence_status_change() const
{
  assert_choice_type(types::lower_layer_presence_status_change, type_, "Value");
  return c.get<lower_layer_presence_status_change_e>();
}
const bh_chs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::bh_chs_to_be_setup_mod_list() const
{
  assert_choice_type(types::bh_chs_to_be_setup_mod_list, type_, "Value");
  return c.get<bh_chs_to_be_setup_mod_list_l>();
}
const bh_chs_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::bh_chs_to_be_modified_list() const
{
  assert_choice_type(types::bh_chs_to_be_modified_list, type_, "Value");
  return c.get<bh_chs_to_be_modified_list_l>();
}
const bh_chs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::bh_chs_to_be_released_list() const
{
  assert_choice_type(types::bh_chs_to_be_released_list, type_, "Value");
  return c.get<bh_chs_to_be_released_list_l>();
}
const nr_v2x_services_authorized_s& ue_context_mod_request_ies_o::value_c::nr_v2x_services_authorized() const
{
  assert_choice_type(types::nr_v2x_services_authorized, type_, "Value");
  return c.get<nr_v2x_services_authorized_s>();
}
const ltev2x_services_authorized_s& ue_context_mod_request_ies_o::value_c::ltev2x_services_authorized() const
{
  assert_choice_type(types::ltev2x_services_authorized, type_, "Value");
  return c.get<ltev2x_services_authorized_s>();
}
const nr_ue_sidelink_aggr_max_bitrate_s& ue_context_mod_request_ies_o::value_c::nr_ue_sidelink_aggr_max_bitrate() const
{
  assert_choice_type(types::nr_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
const lte_ue_sidelink_aggr_max_bitrate_s&
ue_context_mod_request_ies_o::value_c::lte_ue_sidelink_aggr_max_bitrate() const
{
  assert_choice_type(types::lte_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<lte_ue_sidelink_aggr_max_bitrate_s>();
}
const uint64_t& ue_context_mod_request_ies_o::value_c::pc5_link_ambr() const
{
  assert_choice_type(types::pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
const sl_drbs_to_be_setup_mod_list_l& ue_context_mod_request_ies_o::value_c::sl_drbs_to_be_setup_mod_list() const
{
  assert_choice_type(types::sl_drbs_to_be_setup_mod_list, type_, "Value");
  return c.get<sl_drbs_to_be_setup_mod_list_l>();
}
const sl_drbs_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::sl_drbs_to_be_modified_list() const
{
  assert_choice_type(types::sl_drbs_to_be_modified_list, type_, "Value");
  return c.get<sl_drbs_to_be_modified_list_l>();
}
const sl_drbs_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::sl_drbs_to_be_released_list() const
{
  assert_choice_type(types::sl_drbs_to_be_released_list, type_, "Value");
  return c.get<sl_drbs_to_be_released_list_l>();
}
const conditional_intra_du_mob_info_s& ue_context_mod_request_ies_o::value_c::conditional_intra_du_mob_info() const
{
  assert_choice_type(types::conditional_intra_du_mob_info, type_, "Value");
  return c.get<conditional_intra_du_mob_info_s>();
}
const f1_c_transfer_path_s& ue_context_mod_request_ies_o::value_c::f1_c_transfer_path() const
{
  assert_choice_type(types::f1_c_transfer_path, type_, "Value");
  return c.get<f1_c_transfer_path_s>();
}
const scg_ind_e& ue_context_mod_request_ies_o::value_c::scg_ind() const
{
  assert_choice_type(types::scg_ind, type_, "Value");
  return c.get<scg_ind_e>();
}
const unbounded_octstring<true>&
ue_context_mod_request_ies_o::value_c::ul_tx_direct_current_two_carrier_list_info() const
{
  assert_choice_type(types::ul_tx_direct_current_two_carrier_list_info, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const iab_conditional_rrc_msg_delivery_ind_e&
ue_context_mod_request_ies_o::value_c::iab_conditional_rrc_msg_delivery_ind() const
{
  assert_choice_type(types::iab_conditional_rrc_msg_delivery_ind, type_, "Value");
  return c.get<iab_conditional_rrc_msg_delivery_ind_e>();
}
const f1_c_transfer_path_nr_dc_s& ue_context_mod_request_ies_o::value_c::f1_c_transfer_path_nr_dc() const
{
  assert_choice_type(types::f1_c_transfer_path_nr_dc, type_, "Value");
  return c.get<f1_c_transfer_path_nr_dc_s>();
}
const mdt_polluted_meas_ind_e& ue_context_mod_request_ies_o::value_c::mdt_polluted_meas_ind() const
{
  assert_choice_type(types::mdt_polluted_meas_ind, type_, "Value");
  return c.get<mdt_polluted_meas_ind_e>();
}
const scg_activation_request_e& ue_context_mod_request_ies_o::value_c::scg_activation_request() const
{
  assert_choice_type(types::scg_activation_request, type_, "Value");
  return c.get<scg_activation_request_e>();
}
const cg_sdt_query_ind_e& ue_context_mod_request_ies_o::value_c::cg_sdt_query_ind() const
{
  assert_choice_type(types::cg_sdt_query_ind, type_, "Value");
  return c.get<cg_sdt_query_ind_e>();
}
const five_g_pro_se_authorized_s& ue_context_mod_request_ies_o::value_c::five_g_pro_se_authorized() const
{
  assert_choice_type(types::five_g_pro_se_authorized, type_, "Value");
  return c.get<five_g_pro_se_authorized_s>();
}
const nr_ue_sidelink_aggr_max_bitrate_s&
ue_context_mod_request_ies_o::value_c::five_g_pro_se_ue_pc5_aggr_max_bitrate() const
{
  assert_choice_type(types::five_g_pro_se_ue_pc5_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
const uint64_t& ue_context_mod_request_ies_o::value_c::five_g_pro_se_pc5_link_ambr() const
{
  assert_choice_type(types::five_g_pro_se_pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
const uint16_t& ue_context_mod_request_ies_o::value_c::upd_remote_ue_local_id() const
{
  assert_choice_type(types::upd_remote_ue_local_id, type_, "Value");
  return c.get<uint16_t>();
}
const uu_rlc_ch_to_be_setup_list_l& ue_context_mod_request_ies_o::value_c::uu_rlc_ch_to_be_setup_list() const
{
  assert_choice_type(types::uu_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_setup_list_l>();
}
const uu_rlc_ch_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::uu_rlc_ch_to_be_modified_list() const
{
  assert_choice_type(types::uu_rlc_ch_to_be_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_modified_list_l>();
}
const uu_rlc_ch_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::uu_rlc_ch_to_be_released_list() const
{
  assert_choice_type(types::uu_rlc_ch_to_be_released_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_released_list_l>();
}
const pc5_rlc_ch_to_be_setup_list_l& ue_context_mod_request_ies_o::value_c::pc5_rlc_ch_to_be_setup_list() const
{
  assert_choice_type(types::pc5_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_setup_list_l>();
}
const pc5_rlc_ch_to_be_modified_list_l& ue_context_mod_request_ies_o::value_c::pc5_rlc_ch_to_be_modified_list() const
{
  assert_choice_type(types::pc5_rlc_ch_to_be_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_modified_list_l>();
}
const pc5_rlc_ch_to_be_released_list_l& ue_context_mod_request_ies_o::value_c::pc5_rlc_ch_to_be_released_list() const
{
  assert_choice_type(types::pc5_rlc_ch_to_be_released_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_released_list_l>();
}
const path_switch_cfg_s& ue_context_mod_request_ies_o::value_c::path_switch_cfg() const
{
  assert_choice_type(types::path_switch_cfg, type_, "Value");
  return c.get<path_switch_cfg_s>();
}
const gnb_du_ue_slice_max_bit_rate_list_l&
ue_context_mod_request_ies_o::value_c::gnb_du_ue_slice_max_bit_rate_list() const
{
  assert_choice_type(types::gnb_du_ue_slice_max_bit_rate_list, type_, "Value");
  return c.get<gnb_du_ue_slice_max_bit_rate_list_l>();
}
const multicast_mbs_session_list_l& ue_context_mod_request_ies_o::value_c::multicast_mbs_session_setup_list() const
{
  assert_choice_type(types::multicast_mbs_session_setup_list, type_, "Value");
  return c.get<multicast_mbs_session_list_l>();
}
const multicast_mbs_session_list_l& ue_context_mod_request_ies_o::value_c::multicast_mbs_session_rem_list() const
{
  assert_choice_type(types::multicast_mbs_session_rem_list, type_, "Value");
  return c.get<multicast_mbs_session_list_l>();
}
const ue_multicast_m_rbs_to_be_setup_at_modify_list_l&
ue_context_mod_request_ies_o::value_c::ue_multicast_m_rbs_to_be_setup_at_modify_list() const
{
  assert_choice_type(types::ue_multicast_m_rbs_to_be_setup_at_modify_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>();
}
const ue_multicast_m_rbs_to_be_released_list_l&
ue_context_mod_request_ies_o::value_c::ue_multicast_m_rbs_to_be_released_list() const
{
  assert_choice_type(types::ue_multicast_m_rbs_to_be_released_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_to_be_released_list_l>();
}
const sldrx_cycle_list_l& ue_context_mod_request_ies_o::value_c::sldrx_cycle_list() const
{
  assert_choice_type(types::sldrx_cycle_list, type_, "Value");
  return c.get<sldrx_cycle_list_l>();
}
const mdt_plmn_mod_list_l& ue_context_mod_request_ies_o::value_c::management_based_mdt_plmn_mod_list() const
{
  assert_choice_type(types::management_based_mdt_plmn_mod_list, type_, "Value");
  return c.get<mdt_plmn_mod_list_l>();
}
const sdt_bearer_cfg_query_ind_e& ue_context_mod_request_ies_o::value_c::sdt_bearer_cfg_query_ind() const
{
  assert_choice_type(types::sdt_bearer_cfg_query_ind, type_, "Value");
  return c.get<sdt_bearer_cfg_query_ind_e>();
}
const daps_ho_status_e& ue_context_mod_request_ies_o::value_c::daps_ho_status() const
{
  assert_choice_type(types::daps_ho_status, type_, "Value");
  return c.get<daps_ho_status_e>();
}
const serving_cell_mo_list_l& ue_context_mod_request_ies_o::value_c::serving_cell_mo_list() const
{
  assert_choice_type(types::serving_cell_mo_list, type_, "Value");
  return c.get<serving_cell_mo_list_l>();
}
const unbounded_octstring<true>& ue_context_mod_request_ies_o::value_c::ul_tx_direct_current_more_carrier_info() const
{
  assert_choice_type(types::ul_tx_direct_current_more_carrier_info, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const cp_acmcg_info_s& ue_context_mod_request_ies_o::value_c::cp_acmcg_info() const
{
  assert_choice_type(types::cp_acmcg_info, type_, "Value");
  return c.get<cp_acmcg_info_s>();
}
void ue_context_mod_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::sp_cell_id:
      j.write_fieldname("NRCGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    case types::serv_cell_idx:
      j.write_int("INTEGER (0..31,...)", c.get<uint8_t>());
      break;
    case types::sp_cell_ul_cfg:
      j.write_str("CellULConfigured", c.get<cell_ul_cfg_e>().to_string());
      break;
    case types::drx_cycle:
      j.write_fieldname("DRXCycle");
      c.get<drx_cycle_s>().to_json(j);
      break;
    case types::cu_to_du_rrc_info:
      j.write_fieldname("CUtoDURRCInformation");
      c.get<cu_to_du_rrc_info_s>().to_json(j);
      break;
    case types::tx_action_ind:
      j.write_str("TransmissionActionIndicator", c.get<tx_action_ind_e>().to_string());
      break;
    case types::res_coordination_transfer_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::rrc_recfg_complete_ind:
      j.write_str("RRCReconfigurationCompleteIndicator", c.get<rrc_recfg_complete_ind_e>().to_string());
      break;
    case types::rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::scell_to_be_setup_mod_list:
      j.start_array("SCell-ToBeSetupMod-List");
      for (const auto& e1 : c.get<scell_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::scell_to_be_remd_list:
      j.start_array("SCell-ToBeRemoved-List");
      for (const auto& e1 : c.get<scell_to_be_remd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_to_be_setup_mod_list:
      j.start_array("SRBs-ToBeSetupMod-List");
      for (const auto& e1 : c.get<srbs_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_to_be_setup_mod_list:
      j.start_array("DRBs-ToBeSetupMod-List");
      for (const auto& e1 : c.get<drbs_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_to_be_modified_list:
      j.start_array("DRBs-ToBeModified-List");
      for (const auto& e1 : c.get<drbs_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_to_be_released_list:
      j.start_array("SRBs-ToBeReleased-List");
      for (const auto& e1 : c.get<srbs_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_to_be_released_list:
      j.start_array("DRBs-ToBeReleased-List");
      for (const auto& e1 : c.get<drbs_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::inactivity_monitoring_request:
      j.write_str("InactivityMonitoringRequest", "true");
      break;
    case types::rat_freq_prio_info:
      j.write_fieldname("RAT-FrequencyPriorityInformation");
      c.get<rat_freq_prio_info_c>().to_json(j);
      break;
    case types::drx_cfg_ind:
      j.write_str("DRXConfigurationIndicator", "release");
      break;
    case types::rlc_fail_ind:
      j.write_fieldname("RLCFailureIndication");
      c.get<rlc_fail_ind_s>().to_json(j);
      break;
    case types::ul_tx_direct_current_list_info:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::gnb_du_cfg_query:
      j.write_str("GNB-DUConfigurationQuery", "true");
      break;
    case types::gnb_du_ue_ambr_ul:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::execute_dupl:
      j.write_str("ExecuteDuplication", "true");
      break;
    case types::rrc_delivery_status_request:
      j.write_str("RRCDeliveryStatusRequest", "true");
      break;
    case types::res_coordination_transfer_info:
      j.write_fieldname("ResourceCoordinationTransferInformation");
      c.get<res_coordination_transfer_info_s>().to_json(j);
      break;
    case types::serving_cell_mo:
      j.write_int("INTEGER (1..64,...)", c.get<uint8_t>());
      break;
    case types::needfor_gap:
      j.write_str("NeedforGap", "true");
      break;
    case types::full_cfg:
      j.write_str("FullConfiguration", "full");
      break;
    case types::add_rrm_prio_idx:
      j.write_str("BIT STRING", c.get<fixed_bitstring<32, false, true>>().to_string());
      break;
    case types::lower_layer_presence_status_change:
      j.write_str("LowerLayerPresenceStatusChange", c.get<lower_layer_presence_status_change_e>().to_string());
      break;
    case types::bh_chs_to_be_setup_mod_list:
      j.start_array("BHChannels-ToBeSetupMod-List");
      for (const auto& e1 : c.get<bh_chs_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::bh_chs_to_be_modified_list:
      j.start_array("BHChannels-ToBeModified-List");
      for (const auto& e1 : c.get<bh_chs_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::bh_chs_to_be_released_list:
      j.start_array("BHChannels-ToBeReleased-List");
      for (const auto& e1 : c.get<bh_chs_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::nr_v2x_services_authorized:
      j.write_fieldname("NRV2XServicesAuthorized");
      c.get<nr_v2x_services_authorized_s>().to_json(j);
      break;
    case types::ltev2x_services_authorized:
      j.write_fieldname("LTEV2XServicesAuthorized");
      c.get<ltev2x_services_authorized_s>().to_json(j);
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      j.write_fieldname("NRUESidelinkAggregateMaximumBitrate");
      c.get<nr_ue_sidelink_aggr_max_bitrate_s>().to_json(j);
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      j.write_fieldname("LTEUESidelinkAggregateMaximumBitrate");
      c.get<lte_ue_sidelink_aggr_max_bitrate_s>().to_json(j);
      break;
    case types::pc5_link_ambr:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      j.start_array("SLDRBs-ToBeSetupMod-List");
      for (const auto& e1 : c.get<sl_drbs_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_to_be_modified_list:
      j.start_array("SLDRBs-ToBeModified-List");
      for (const auto& e1 : c.get<sl_drbs_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_to_be_released_list:
      j.start_array("SLDRBs-ToBeReleased-List");
      for (const auto& e1 : c.get<sl_drbs_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::conditional_intra_du_mob_info:
      j.write_fieldname("ConditionalIntraDUMobilityInformation");
      c.get<conditional_intra_du_mob_info_s>().to_json(j);
      break;
    case types::f1_c_transfer_path:
      j.write_fieldname("F1CTransferPath");
      c.get<f1_c_transfer_path_s>().to_json(j);
      break;
    case types::scg_ind:
      j.write_str("SCGIndicator", "released");
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::iab_conditional_rrc_msg_delivery_ind:
      j.write_str("IABConditionalRRCMessageDeliveryIndication", "true");
      break;
    case types::f1_c_transfer_path_nr_dc:
      j.write_fieldname("F1CTransferPathNRDC");
      c.get<f1_c_transfer_path_nr_dc_s>().to_json(j);
      break;
    case types::mdt_polluted_meas_ind:
      j.write_str("MDTPollutedMeasurementIndicator", c.get<mdt_polluted_meas_ind_e>().to_string());
      break;
    case types::scg_activation_request:
      j.write_str("SCGActivationRequest", c.get<scg_activation_request_e>().to_string());
      break;
    case types::cg_sdt_query_ind:
      j.write_str("CG-SDTQueryIndication", "true");
      break;
    case types::five_g_pro_se_authorized:
      j.write_fieldname("FiveG-ProSeAuthorized");
      c.get<five_g_pro_se_authorized_s>().to_json(j);
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      j.write_fieldname("NRUESidelinkAggregateMaximumBitrate");
      c.get<nr_ue_sidelink_aggr_max_bitrate_s>().to_json(j);
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::upd_remote_ue_local_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      j.start_array("UuRLCChannelToBeSetupList");
      for (const auto& e1 : c.get<uu_rlc_ch_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      j.start_array("UuRLCChannelToBeModifiedList");
      for (const auto& e1 : c.get<uu_rlc_ch_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_to_be_released_list:
      j.start_array("UuRLCChannelToBeReleasedList");
      for (const auto& e1 : c.get<uu_rlc_ch_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      j.start_array("PC5RLCChannelToBeSetupList");
      for (const auto& e1 : c.get<pc5_rlc_ch_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      j.start_array("PC5RLCChannelToBeModifiedList");
      for (const auto& e1 : c.get<pc5_rlc_ch_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      j.start_array("PC5RLCChannelToBeReleasedList");
      for (const auto& e1 : c.get<pc5_rlc_ch_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::path_switch_cfg:
      j.write_fieldname("PathSwitchConfiguration");
      c.get<path_switch_cfg_s>().to_json(j);
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      j.start_array("GNBDUUESliceMaximumBitRateList");
      for (const auto& e1 : c.get<gnb_du_ue_slice_max_bit_rate_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::multicast_mbs_session_setup_list:
      j.start_array("MulticastMBSSessionList");
      for (const auto& e1 : c.get<multicast_mbs_session_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::multicast_mbs_session_rem_list:
      j.start_array("MulticastMBSSessionList");
      for (const auto& e1 : c.get<multicast_mbs_session_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      j.start_array("UE-MulticastMRBs-ToBeSetup-atModify-List");
      for (const auto& e1 : c.get<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      j.start_array("UE-MulticastMRBs-ToBeReleased-List");
      for (const auto& e1 : c.get<ue_multicast_m_rbs_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sldrx_cycle_list:
      j.start_array("SLDRXCycleList");
      for (const auto& e1 : c.get<sldrx_cycle_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::management_based_mdt_plmn_mod_list:
      j.start_array("MDTPLMNModificationList");
      for (const auto& e1 : c.get<mdt_plmn_mod_list_l>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    case types::sdt_bearer_cfg_query_ind:
      j.write_str("SDTBearerConfigurationQueryIndication", "true");
      break;
    case types::daps_ho_status:
      j.write_str("DAPS-HO-Status", "initiation");
      break;
    case types::serving_cell_mo_list:
      j.start_array("ServingCellMO-List");
      for (const auto& e1 : c.get<serving_cell_mo_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::cp_acmcg_info:
      j.write_fieldname("CPACMCGInformation");
      c.get<cp_acmcg_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_mod_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sp_cell_id:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    case types::serv_cell_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u, true, true));
      break;
    case types::sp_cell_ul_cfg:
      HANDLE_CODE(c.get<cell_ul_cfg_e>().pack(bref));
      break;
    case types::drx_cycle:
      HANDLE_CODE(c.get<drx_cycle_s>().pack(bref));
      break;
    case types::cu_to_du_rrc_info:
      HANDLE_CODE(c.get<cu_to_du_rrc_info_s>().pack(bref));
      break;
    case types::tx_action_ind:
      HANDLE_CODE(c.get<tx_action_ind_e>().pack(bref));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::rrc_recfg_complete_ind:
      HANDLE_CODE(c.get<rrc_recfg_complete_ind_e>().pack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::scell_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scell_to_be_setup_mod_list_l>(), 1, 32, true));
      break;
    case types::scell_to_be_remd_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scell_to_be_remd_list_l>(), 1, 32, true));
      break;
    case types::srbs_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_to_be_setup_mod_list_l>(), 1, 8, true));
      break;
    case types::drbs_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_to_be_setup_mod_list_l>(), 1, 64, true));
      break;
    case types::drbs_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_to_be_modified_list_l>(), 1, 64, true));
      break;
    case types::srbs_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_to_be_released_list_l>(), 1, 8, true));
      break;
    case types::drbs_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_to_be_released_list_l>(), 1, 64, true));
      break;
    case types::inactivity_monitoring_request:
      HANDLE_CODE(c.get<inactivity_monitoring_request_e>().pack(bref));
      break;
    case types::rat_freq_prio_info:
      HANDLE_CODE(c.get<rat_freq_prio_info_c>().pack(bref));
      break;
    case types::drx_cfg_ind:
      HANDLE_CODE(c.get<drx_cfg_ind_e>().pack(bref));
      break;
    case types::rlc_fail_ind:
      HANDLE_CODE(c.get<rlc_fail_ind_s>().pack(bref));
      break;
    case types::ul_tx_direct_current_list_info:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::gnb_du_cfg_query:
      HANDLE_CODE(c.get<gnb_du_cfg_query_e>().pack(bref));
      break;
    case types::gnb_du_ue_ambr_ul:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().pack(bref));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().pack(bref));
      break;
    case types::res_coordination_transfer_info:
      HANDLE_CODE(c.get<res_coordination_transfer_info_s>().pack(bref));
      break;
    case types::serving_cell_mo:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)64u, true, true));
      break;
    case types::needfor_gap:
      HANDLE_CODE(c.get<needfor_gap_e>().pack(bref));
      break;
    case types::full_cfg:
      HANDLE_CODE(c.get<full_cfg_e>().pack(bref));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().pack(bref)));
      break;
    case types::lower_layer_presence_status_change:
      HANDLE_CODE(c.get<lower_layer_presence_status_change_e>().pack(bref));
      break;
    case types::bh_chs_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_to_be_setup_mod_list_l>(), 1, 65536, true));
      break;
    case types::bh_chs_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_to_be_modified_list_l>(), 1, 65536, true));
      break;
    case types::bh_chs_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_to_be_released_list_l>(), 1, 65536, true));
      break;
    case types::nr_v2x_services_authorized:
      HANDLE_CODE(c.get<nr_v2x_services_authorized_s>().pack(bref));
      break;
    case types::ltev2x_services_authorized:
      HANDLE_CODE(c.get<ltev2x_services_authorized_s>().pack(bref));
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().pack(bref));
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<lte_ue_sidelink_aggr_max_bitrate_s>().pack(bref));
      break;
    case types::pc5_link_ambr:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_to_be_setup_mod_list_l>(), 1, 512, true));
      break;
    case types::sl_drbs_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_to_be_modified_list_l>(), 1, 512, true));
      break;
    case types::sl_drbs_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_to_be_released_list_l>(), 1, 512, true));
      break;
    case types::conditional_intra_du_mob_info:
      HANDLE_CODE(c.get<conditional_intra_du_mob_info_s>().pack(bref));
      break;
    case types::f1_c_transfer_path:
      HANDLE_CODE(c.get<f1_c_transfer_path_s>().pack(bref));
      break;
    case types::scg_ind:
      HANDLE_CODE(c.get<scg_ind_e>().pack(bref));
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::iab_conditional_rrc_msg_delivery_ind:
      HANDLE_CODE(c.get<iab_conditional_rrc_msg_delivery_ind_e>().pack(bref));
      break;
    case types::f1_c_transfer_path_nr_dc:
      HANDLE_CODE(c.get<f1_c_transfer_path_nr_dc_s>().pack(bref));
      break;
    case types::mdt_polluted_meas_ind:
      HANDLE_CODE(c.get<mdt_polluted_meas_ind_e>().pack(bref));
      break;
    case types::scg_activation_request:
      HANDLE_CODE(c.get<scg_activation_request_e>().pack(bref));
      break;
    case types::cg_sdt_query_ind:
      HANDLE_CODE(c.get<cg_sdt_query_ind_e>().pack(bref));
      break;
    case types::five_g_pro_se_authorized:
      HANDLE_CODE(c.get<five_g_pro_se_authorized_s>().pack(bref));
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().pack(bref));
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::upd_remote_ue_local_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_to_be_setup_list_l>(), 1, 32, true));
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_to_be_modified_list_l>(), 1, 32, true));
      break;
    case types::uu_rlc_ch_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_to_be_released_list_l>(), 1, 32, true));
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_to_be_setup_list_l>(), 1, 512, true));
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_to_be_modified_list_l>(), 1, 512, true));
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_to_be_released_list_l>(), 1, 512, true));
      break;
    case types::path_switch_cfg:
      HANDLE_CODE(c.get<path_switch_cfg_s>().pack(bref));
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_du_ue_slice_max_bit_rate_list_l>(), 1, 8, true));
      break;
    case types::multicast_mbs_session_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<multicast_mbs_session_list_l>(), 1, 256, true));
      break;
    case types::multicast_mbs_session_rem_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<multicast_mbs_session_list_l>(), 1, 256, true));
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>(), 1, 64, true));
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_multicast_m_rbs_to_be_released_list_l>(), 1, 64, true));
      break;
    case types::sldrx_cycle_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sldrx_cycle_list_l>(), 1, 32, true));
      break;
    case types::management_based_mdt_plmn_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<mdt_plmn_mod_list_l>(), 0, 16, true));
      break;
    case types::sdt_bearer_cfg_query_ind:
      HANDLE_CODE(c.get<sdt_bearer_cfg_query_ind_e>().pack(bref));
      break;
    case types::daps_ho_status:
      HANDLE_CODE(c.get<daps_ho_status_e>().pack(bref));
      break;
    case types::serving_cell_mo_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<serving_cell_mo_list_l>(), 1, 16, true));
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::cp_acmcg_info:
      HANDLE_CODE(c.get<cp_acmcg_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sp_cell_id:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    case types::serv_cell_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u, true, true));
      break;
    case types::sp_cell_ul_cfg:
      HANDLE_CODE(c.get<cell_ul_cfg_e>().unpack(bref));
      break;
    case types::drx_cycle:
      HANDLE_CODE(c.get<drx_cycle_s>().unpack(bref));
      break;
    case types::cu_to_du_rrc_info:
      HANDLE_CODE(c.get<cu_to_du_rrc_info_s>().unpack(bref));
      break;
    case types::tx_action_ind:
      HANDLE_CODE(c.get<tx_action_ind_e>().unpack(bref));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::rrc_recfg_complete_ind:
      HANDLE_CODE(c.get<rrc_recfg_complete_ind_e>().unpack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::scell_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scell_to_be_setup_mod_list_l>(), bref, 1, 32, true));
      break;
    case types::scell_to_be_remd_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scell_to_be_remd_list_l>(), bref, 1, 32, true));
      break;
    case types::srbs_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_to_be_setup_mod_list_l>(), bref, 1, 8, true));
      break;
    case types::drbs_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_to_be_setup_mod_list_l>(), bref, 1, 64, true));
      break;
    case types::drbs_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_to_be_modified_list_l>(), bref, 1, 64, true));
      break;
    case types::srbs_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_to_be_released_list_l>(), bref, 1, 8, true));
      break;
    case types::drbs_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_to_be_released_list_l>(), bref, 1, 64, true));
      break;
    case types::inactivity_monitoring_request:
      HANDLE_CODE(c.get<inactivity_monitoring_request_e>().unpack(bref));
      break;
    case types::rat_freq_prio_info:
      HANDLE_CODE(c.get<rat_freq_prio_info_c>().unpack(bref));
      break;
    case types::drx_cfg_ind:
      HANDLE_CODE(c.get<drx_cfg_ind_e>().unpack(bref));
      break;
    case types::rlc_fail_ind:
      HANDLE_CODE(c.get<rlc_fail_ind_s>().unpack(bref));
      break;
    case types::ul_tx_direct_current_list_info:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::gnb_du_cfg_query:
      HANDLE_CODE(c.get<gnb_du_cfg_query_e>().unpack(bref));
      break;
    case types::gnb_du_ue_ambr_ul:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().unpack(bref));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().unpack(bref));
      break;
    case types::res_coordination_transfer_info:
      HANDLE_CODE(c.get<res_coordination_transfer_info_s>().unpack(bref));
      break;
    case types::serving_cell_mo:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)64u, true, true));
      break;
    case types::needfor_gap:
      HANDLE_CODE(c.get<needfor_gap_e>().unpack(bref));
      break;
    case types::full_cfg:
      HANDLE_CODE(c.get<full_cfg_e>().unpack(bref));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().unpack(bref)));
      break;
    case types::lower_layer_presence_status_change:
      HANDLE_CODE(c.get<lower_layer_presence_status_change_e>().unpack(bref));
      break;
    case types::bh_chs_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_to_be_setup_mod_list_l>(), bref, 1, 65536, true));
      break;
    case types::bh_chs_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_to_be_modified_list_l>(), bref, 1, 65536, true));
      break;
    case types::bh_chs_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_to_be_released_list_l>(), bref, 1, 65536, true));
      break;
    case types::nr_v2x_services_authorized:
      HANDLE_CODE(c.get<nr_v2x_services_authorized_s>().unpack(bref));
      break;
    case types::ltev2x_services_authorized:
      HANDLE_CODE(c.get<ltev2x_services_authorized_s>().unpack(bref));
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().unpack(bref));
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<lte_ue_sidelink_aggr_max_bitrate_s>().unpack(bref));
      break;
    case types::pc5_link_ambr:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_to_be_setup_mod_list_l>(), bref, 1, 512, true));
      break;
    case types::sl_drbs_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_to_be_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::sl_drbs_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_to_be_released_list_l>(), bref, 1, 512, true));
      break;
    case types::conditional_intra_du_mob_info:
      HANDLE_CODE(c.get<conditional_intra_du_mob_info_s>().unpack(bref));
      break;
    case types::f1_c_transfer_path:
      HANDLE_CODE(c.get<f1_c_transfer_path_s>().unpack(bref));
      break;
    case types::scg_ind:
      HANDLE_CODE(c.get<scg_ind_e>().unpack(bref));
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::iab_conditional_rrc_msg_delivery_ind:
      HANDLE_CODE(c.get<iab_conditional_rrc_msg_delivery_ind_e>().unpack(bref));
      break;
    case types::f1_c_transfer_path_nr_dc:
      HANDLE_CODE(c.get<f1_c_transfer_path_nr_dc_s>().unpack(bref));
      break;
    case types::mdt_polluted_meas_ind:
      HANDLE_CODE(c.get<mdt_polluted_meas_ind_e>().unpack(bref));
      break;
    case types::scg_activation_request:
      HANDLE_CODE(c.get<scg_activation_request_e>().unpack(bref));
      break;
    case types::cg_sdt_query_ind:
      HANDLE_CODE(c.get<cg_sdt_query_ind_e>().unpack(bref));
      break;
    case types::five_g_pro_se_authorized:
      HANDLE_CODE(c.get<five_g_pro_se_authorized_s>().unpack(bref));
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().unpack(bref));
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::upd_remote_ue_local_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_to_be_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_to_be_modified_list_l>(), bref, 1, 32, true));
      break;
    case types::uu_rlc_ch_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_to_be_released_list_l>(), bref, 1, 32, true));
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_to_be_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_to_be_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_to_be_released_list_l>(), bref, 1, 512, true));
      break;
    case types::path_switch_cfg:
      HANDLE_CODE(c.get<path_switch_cfg_s>().unpack(bref));
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_du_ue_slice_max_bit_rate_list_l>(), bref, 1, 8, true));
      break;
    case types::multicast_mbs_session_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<multicast_mbs_session_list_l>(), bref, 1, 256, true));
      break;
    case types::multicast_mbs_session_rem_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<multicast_mbs_session_list_l>(), bref, 1, 256, true));
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_multicast_m_rbs_to_be_setup_at_modify_list_l>(), bref, 1, 64, true));
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_multicast_m_rbs_to_be_released_list_l>(), bref, 1, 64, true));
      break;
    case types::sldrx_cycle_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sldrx_cycle_list_l>(), bref, 1, 32, true));
      break;
    case types::management_based_mdt_plmn_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<mdt_plmn_mod_list_l>(), bref, 0, 16, true));
      break;
    case types::sdt_bearer_cfg_query_ind:
      HANDLE_CODE(c.get<sdt_bearer_cfg_query_ind_e>().unpack(bref));
      break;
    case types::daps_ho_status:
      HANDLE_CODE(c.get<daps_ho_status_e>().unpack(bref));
      break;
    case types::serving_cell_mo_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<serving_cell_mo_list_l>(), bref, 1, 16, true));
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::cp_acmcg_info:
      HANDLE_CODE(c.get<cp_acmcg_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextModificationRequiredIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_mod_required_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 49, 39, 23, 70, 25, 0, 277, 320, 322, 375, 606, 607, 614, 615, 655, 657};
  return map_enum_number(names, 18, idx, "id");
}
bool ue_context_mod_required_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 49, 39, 23, 70, 25, 0, 277, 320, 322, 375, 606, 607, 614, 615, 655, 657};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_mod_required_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 49:
      return crit_e::ignore;
    case 39:
      return crit_e::reject;
    case 23:
      return crit_e::reject;
    case 70:
      return crit_e::reject;
    case 25:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 277:
      return crit_e::reject;
    case 320:
      return crit_e::reject;
    case 322:
      return crit_e::reject;
    case 375:
      return crit_e::reject;
    case 606:
      return crit_e::reject;
    case 607:
      return crit_e::reject;
    case 614:
      return crit_e::reject;
    case 615:
      return crit_e::reject;
    case 655:
      return crit_e::reject;
    case 657:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_mod_required_ies_o::value_c ue_context_mod_required_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 49:
      ret.set(value_c::types::res_coordination_transfer_container);
      break;
    case 39:
      ret.set(value_c::types::du_to_cu_rrc_info);
      break;
    case 23:
      ret.set(value_c::types::drbs_required_to_be_modified_list);
      break;
    case 70:
      ret.set(value_c::types::srbs_required_to_be_released_list);
      break;
    case 25:
      ret.set(value_c::types::drbs_required_to_be_released_list);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 277:
      ret.set(value_c::types::bh_chs_required_to_be_released_list);
      break;
    case 320:
      ret.set(value_c::types::sl_drbs_required_to_be_modified_list);
      break;
    case 322:
      ret.set(value_c::types::sl_drbs_required_to_be_released_list);
      break;
    case 375:
      ret.set(value_c::types::target_cells_to_cancel);
      break;
    case 606:
      ret.set(value_c::types::uu_rlc_ch_required_to_be_modified_list);
      break;
    case 607:
      ret.set(value_c::types::uu_rlc_ch_required_to_be_released_list);
      break;
    case 614:
      ret.set(value_c::types::pc5_rlc_ch_required_to_be_modified_list);
      break;
    case 615:
      ret.set(value_c::types::pc5_rlc_ch_required_to_be_released_list);
      break;
    case 655:
      ret.set(value_c::types::ue_multicast_m_rbs_required_to_be_modified_list);
      break;
    case 657:
      ret.set(value_c::types::ue_multicast_m_rbs_required_to_be_released_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_mod_required_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 49:
      return presence_e::optional;
    case 39:
      return presence_e::optional;
    case 23:
      return presence_e::optional;
    case 70:
      return presence_e::optional;
    case 25:
      return presence_e::optional;
    case 0:
      return presence_e::mandatory;
    case 277:
      return presence_e::optional;
    case 320:
      return presence_e::optional;
    case 322:
      return presence_e::optional;
    case 375:
      return presence_e::optional;
    case 606:
      return presence_e::optional;
    case 607:
      return presence_e::optional;
    case 614:
      return presence_e::optional;
    case 615:
      return presence_e::optional;
    case 655:
      return presence_e::optional;
    case 657:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_mod_required_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::res_coordination_transfer_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::du_to_cu_rrc_info:
      c.destroy<du_to_cu_rrc_info_s>();
      break;
    case types::drbs_required_to_be_modified_list:
      c.destroy<drbs_required_to_be_modified_list_l>();
      break;
    case types::srbs_required_to_be_released_list:
      c.destroy<srbs_required_to_be_released_list_l>();
      break;
    case types::drbs_required_to_be_released_list:
      c.destroy<drbs_required_to_be_released_list_l>();
      break;
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::bh_chs_required_to_be_released_list:
      c.destroy<bh_chs_required_to_be_released_list_l>();
      break;
    case types::sl_drbs_required_to_be_modified_list:
      c.destroy<sl_drbs_required_to_be_modified_list_l>();
      break;
    case types::sl_drbs_required_to_be_released_list:
      c.destroy<sl_drbs_required_to_be_released_list_l>();
      break;
    case types::target_cells_to_cancel:
      c.destroy<target_cell_list_l>();
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      c.destroy<uu_rlc_ch_required_to_be_modified_list_l>();
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      c.destroy<uu_rlc_ch_required_to_be_released_list_l>();
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      c.destroy<pc5_rlc_ch_required_to_be_modified_list_l>();
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      c.destroy<pc5_rlc_ch_required_to_be_released_list_l>();
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      c.destroy<ue_multicast_m_rbs_required_to_be_modified_list_l>();
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      c.destroy<ue_multicast_m_rbs_required_to_be_released_list_l>();
      break;
    default:
      break;
  }
}
void ue_context_mod_required_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::res_coordination_transfer_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::du_to_cu_rrc_info:
      c.init<du_to_cu_rrc_info_s>();
      break;
    case types::drbs_required_to_be_modified_list:
      c.init<drbs_required_to_be_modified_list_l>();
      break;
    case types::srbs_required_to_be_released_list:
      c.init<srbs_required_to_be_released_list_l>();
      break;
    case types::drbs_required_to_be_released_list:
      c.init<drbs_required_to_be_released_list_l>();
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::bh_chs_required_to_be_released_list:
      c.init<bh_chs_required_to_be_released_list_l>();
      break;
    case types::sl_drbs_required_to_be_modified_list:
      c.init<sl_drbs_required_to_be_modified_list_l>();
      break;
    case types::sl_drbs_required_to_be_released_list:
      c.init<sl_drbs_required_to_be_released_list_l>();
      break;
    case types::target_cells_to_cancel:
      c.init<target_cell_list_l>();
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      c.init<uu_rlc_ch_required_to_be_modified_list_l>();
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      c.init<uu_rlc_ch_required_to_be_released_list_l>();
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      c.init<pc5_rlc_ch_required_to_be_modified_list_l>();
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      c.init<pc5_rlc_ch_required_to_be_released_list_l>();
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      c.init<ue_multicast_m_rbs_required_to_be_modified_list_l>();
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      c.init<ue_multicast_m_rbs_required_to_be_released_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_required_ies_o::value_c");
  }
}
ue_context_mod_required_ies_o::value_c::value_c(const ue_context_mod_required_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::du_to_cu_rrc_info:
      c.init(other.c.get<du_to_cu_rrc_info_s>());
      break;
    case types::drbs_required_to_be_modified_list:
      c.init(other.c.get<drbs_required_to_be_modified_list_l>());
      break;
    case types::srbs_required_to_be_released_list:
      c.init(other.c.get<srbs_required_to_be_released_list_l>());
      break;
    case types::drbs_required_to_be_released_list:
      c.init(other.c.get<drbs_required_to_be_released_list_l>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::bh_chs_required_to_be_released_list:
      c.init(other.c.get<bh_chs_required_to_be_released_list_l>());
      break;
    case types::sl_drbs_required_to_be_modified_list:
      c.init(other.c.get<sl_drbs_required_to_be_modified_list_l>());
      break;
    case types::sl_drbs_required_to_be_released_list:
      c.init(other.c.get<sl_drbs_required_to_be_released_list_l>());
      break;
    case types::target_cells_to_cancel:
      c.init(other.c.get<target_cell_list_l>());
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      c.init(other.c.get<uu_rlc_ch_required_to_be_modified_list_l>());
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      c.init(other.c.get<uu_rlc_ch_required_to_be_released_list_l>());
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      c.init(other.c.get<pc5_rlc_ch_required_to_be_modified_list_l>());
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      c.init(other.c.get<pc5_rlc_ch_required_to_be_released_list_l>());
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      c.init(other.c.get<ue_multicast_m_rbs_required_to_be_modified_list_l>());
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      c.init(other.c.get<ue_multicast_m_rbs_required_to_be_released_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_required_ies_o::value_c");
  }
}
ue_context_mod_required_ies_o::value_c&
ue_context_mod_required_ies_o::value_c::operator=(const ue_context_mod_required_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::du_to_cu_rrc_info:
      c.set(other.c.get<du_to_cu_rrc_info_s>());
      break;
    case types::drbs_required_to_be_modified_list:
      c.set(other.c.get<drbs_required_to_be_modified_list_l>());
      break;
    case types::srbs_required_to_be_released_list:
      c.set(other.c.get<srbs_required_to_be_released_list_l>());
      break;
    case types::drbs_required_to_be_released_list:
      c.set(other.c.get<drbs_required_to_be_released_list_l>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::bh_chs_required_to_be_released_list:
      c.set(other.c.get<bh_chs_required_to_be_released_list_l>());
      break;
    case types::sl_drbs_required_to_be_modified_list:
      c.set(other.c.get<sl_drbs_required_to_be_modified_list_l>());
      break;
    case types::sl_drbs_required_to_be_released_list:
      c.set(other.c.get<sl_drbs_required_to_be_released_list_l>());
      break;
    case types::target_cells_to_cancel:
      c.set(other.c.get<target_cell_list_l>());
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      c.set(other.c.get<uu_rlc_ch_required_to_be_modified_list_l>());
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      c.set(other.c.get<uu_rlc_ch_required_to_be_released_list_l>());
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      c.set(other.c.get<pc5_rlc_ch_required_to_be_modified_list_l>());
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      c.set(other.c.get<pc5_rlc_ch_required_to_be_released_list_l>());
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      c.set(other.c.get<ue_multicast_m_rbs_required_to_be_modified_list_l>());
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      c.set(other.c.get<ue_multicast_m_rbs_required_to_be_released_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_required_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_mod_required_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_mod_required_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
unbounded_octstring<true>& ue_context_mod_required_ies_o::value_c::res_coordination_transfer_container()
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
du_to_cu_rrc_info_s& ue_context_mod_required_ies_o::value_c::du_to_cu_rrc_info()
{
  assert_choice_type(types::du_to_cu_rrc_info, type_, "Value");
  return c.get<du_to_cu_rrc_info_s>();
}
drbs_required_to_be_modified_list_l& ue_context_mod_required_ies_o::value_c::drbs_required_to_be_modified_list()
{
  assert_choice_type(types::drbs_required_to_be_modified_list, type_, "Value");
  return c.get<drbs_required_to_be_modified_list_l>();
}
srbs_required_to_be_released_list_l& ue_context_mod_required_ies_o::value_c::srbs_required_to_be_released_list()
{
  assert_choice_type(types::srbs_required_to_be_released_list, type_, "Value");
  return c.get<srbs_required_to_be_released_list_l>();
}
drbs_required_to_be_released_list_l& ue_context_mod_required_ies_o::value_c::drbs_required_to_be_released_list()
{
  assert_choice_type(types::drbs_required_to_be_released_list, type_, "Value");
  return c.get<drbs_required_to_be_released_list_l>();
}
cause_c& ue_context_mod_required_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
bh_chs_required_to_be_released_list_l& ue_context_mod_required_ies_o::value_c::bh_chs_required_to_be_released_list()
{
  assert_choice_type(types::bh_chs_required_to_be_released_list, type_, "Value");
  return c.get<bh_chs_required_to_be_released_list_l>();
}
sl_drbs_required_to_be_modified_list_l& ue_context_mod_required_ies_o::value_c::sl_drbs_required_to_be_modified_list()
{
  assert_choice_type(types::sl_drbs_required_to_be_modified_list, type_, "Value");
  return c.get<sl_drbs_required_to_be_modified_list_l>();
}
sl_drbs_required_to_be_released_list_l& ue_context_mod_required_ies_o::value_c::sl_drbs_required_to_be_released_list()
{
  assert_choice_type(types::sl_drbs_required_to_be_released_list, type_, "Value");
  return c.get<sl_drbs_required_to_be_released_list_l>();
}
target_cell_list_l& ue_context_mod_required_ies_o::value_c::target_cells_to_cancel()
{
  assert_choice_type(types::target_cells_to_cancel, type_, "Value");
  return c.get<target_cell_list_l>();
}
uu_rlc_ch_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::uu_rlc_ch_required_to_be_modified_list()
{
  assert_choice_type(types::uu_rlc_ch_required_to_be_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_required_to_be_modified_list_l>();
}
uu_rlc_ch_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::uu_rlc_ch_required_to_be_released_list()
{
  assert_choice_type(types::uu_rlc_ch_required_to_be_released_list, type_, "Value");
  return c.get<uu_rlc_ch_required_to_be_released_list_l>();
}
pc5_rlc_ch_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::pc5_rlc_ch_required_to_be_modified_list()
{
  assert_choice_type(types::pc5_rlc_ch_required_to_be_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_required_to_be_modified_list_l>();
}
pc5_rlc_ch_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::pc5_rlc_ch_required_to_be_released_list()
{
  assert_choice_type(types::pc5_rlc_ch_required_to_be_released_list, type_, "Value");
  return c.get<pc5_rlc_ch_required_to_be_released_list_l>();
}
ue_multicast_m_rbs_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::ue_multicast_m_rbs_required_to_be_modified_list()
{
  assert_choice_type(types::ue_multicast_m_rbs_required_to_be_modified_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_required_to_be_modified_list_l>();
}
ue_multicast_m_rbs_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::ue_multicast_m_rbs_required_to_be_released_list()
{
  assert_choice_type(types::ue_multicast_m_rbs_required_to_be_released_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_required_to_be_released_list_l>();
}
const uint64_t& ue_context_mod_required_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_mod_required_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const unbounded_octstring<true>& ue_context_mod_required_ies_o::value_c::res_coordination_transfer_container() const
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const du_to_cu_rrc_info_s& ue_context_mod_required_ies_o::value_c::du_to_cu_rrc_info() const
{
  assert_choice_type(types::du_to_cu_rrc_info, type_, "Value");
  return c.get<du_to_cu_rrc_info_s>();
}
const drbs_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::drbs_required_to_be_modified_list() const
{
  assert_choice_type(types::drbs_required_to_be_modified_list, type_, "Value");
  return c.get<drbs_required_to_be_modified_list_l>();
}
const srbs_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::srbs_required_to_be_released_list() const
{
  assert_choice_type(types::srbs_required_to_be_released_list, type_, "Value");
  return c.get<srbs_required_to_be_released_list_l>();
}
const drbs_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::drbs_required_to_be_released_list() const
{
  assert_choice_type(types::drbs_required_to_be_released_list, type_, "Value");
  return c.get<drbs_required_to_be_released_list_l>();
}
const cause_c& ue_context_mod_required_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const bh_chs_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::bh_chs_required_to_be_released_list() const
{
  assert_choice_type(types::bh_chs_required_to_be_released_list, type_, "Value");
  return c.get<bh_chs_required_to_be_released_list_l>();
}
const sl_drbs_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::sl_drbs_required_to_be_modified_list() const
{
  assert_choice_type(types::sl_drbs_required_to_be_modified_list, type_, "Value");
  return c.get<sl_drbs_required_to_be_modified_list_l>();
}
const sl_drbs_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::sl_drbs_required_to_be_released_list() const
{
  assert_choice_type(types::sl_drbs_required_to_be_released_list, type_, "Value");
  return c.get<sl_drbs_required_to_be_released_list_l>();
}
const target_cell_list_l& ue_context_mod_required_ies_o::value_c::target_cells_to_cancel() const
{
  assert_choice_type(types::target_cells_to_cancel, type_, "Value");
  return c.get<target_cell_list_l>();
}
const uu_rlc_ch_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::uu_rlc_ch_required_to_be_modified_list() const
{
  assert_choice_type(types::uu_rlc_ch_required_to_be_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_required_to_be_modified_list_l>();
}
const uu_rlc_ch_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::uu_rlc_ch_required_to_be_released_list() const
{
  assert_choice_type(types::uu_rlc_ch_required_to_be_released_list, type_, "Value");
  return c.get<uu_rlc_ch_required_to_be_released_list_l>();
}
const pc5_rlc_ch_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::pc5_rlc_ch_required_to_be_modified_list() const
{
  assert_choice_type(types::pc5_rlc_ch_required_to_be_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_required_to_be_modified_list_l>();
}
const pc5_rlc_ch_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::pc5_rlc_ch_required_to_be_released_list() const
{
  assert_choice_type(types::pc5_rlc_ch_required_to_be_released_list, type_, "Value");
  return c.get<pc5_rlc_ch_required_to_be_released_list_l>();
}
const ue_multicast_m_rbs_required_to_be_modified_list_l&
ue_context_mod_required_ies_o::value_c::ue_multicast_m_rbs_required_to_be_modified_list() const
{
  assert_choice_type(types::ue_multicast_m_rbs_required_to_be_modified_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_required_to_be_modified_list_l>();
}
const ue_multicast_m_rbs_required_to_be_released_list_l&
ue_context_mod_required_ies_o::value_c::ue_multicast_m_rbs_required_to_be_released_list() const
{
  assert_choice_type(types::ue_multicast_m_rbs_required_to_be_released_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_required_to_be_released_list_l>();
}
void ue_context_mod_required_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::du_to_cu_rrc_info:
      j.write_fieldname("DUtoCURRCInformation");
      c.get<du_to_cu_rrc_info_s>().to_json(j);
      break;
    case types::drbs_required_to_be_modified_list:
      j.start_array("DRBs-Required-ToBeModified-List");
      for (const auto& e1 : c.get<drbs_required_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_required_to_be_released_list:
      j.start_array("SRBs-Required-ToBeReleased-List");
      for (const auto& e1 : c.get<srbs_required_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_required_to_be_released_list:
      j.start_array("DRBs-Required-ToBeReleased-List");
      for (const auto& e1 : c.get<drbs_required_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::bh_chs_required_to_be_released_list:
      j.start_array("BHChannels-Required-ToBeReleased-List");
      for (const auto& e1 : c.get<bh_chs_required_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_required_to_be_modified_list:
      j.start_array("SLDRBs-Required-ToBeModified-List");
      for (const auto& e1 : c.get<sl_drbs_required_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_required_to_be_released_list:
      j.start_array("SLDRBs-Required-ToBeReleased-List");
      for (const auto& e1 : c.get<sl_drbs_required_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::target_cells_to_cancel:
      j.start_array("TargetCellList");
      for (const auto& e1 : c.get<target_cell_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      j.start_array("UuRLCChannelRequiredToBeModifiedList");
      for (const auto& e1 : c.get<uu_rlc_ch_required_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      j.start_array("UuRLCChannelRequiredToBeReleasedList");
      for (const auto& e1 : c.get<uu_rlc_ch_required_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      j.start_array("PC5RLCChannelRequiredToBeModifiedList");
      for (const auto& e1 : c.get<pc5_rlc_ch_required_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      j.start_array("PC5RLCChannelRequiredToBeReleasedList");
      for (const auto& e1 : c.get<pc5_rlc_ch_required_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      j.start_array("UE-MulticastMRBs-RequiredToBeModified-List");
      for (const auto& e1 : c.get<ue_multicast_m_rbs_required_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      j.start_array("UE-MulticastMRBs-RequiredToBeReleased-List");
      for (const auto& e1 : c.get<ue_multicast_m_rbs_required_to_be_released_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_required_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_mod_required_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::du_to_cu_rrc_info:
      HANDLE_CODE(c.get<du_to_cu_rrc_info_s>().pack(bref));
      break;
    case types::drbs_required_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_required_to_be_modified_list_l>(), 1, 64, true));
      break;
    case types::srbs_required_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_required_to_be_released_list_l>(), 1, 8, true));
      break;
    case types::drbs_required_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_required_to_be_released_list_l>(), 1, 64, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::bh_chs_required_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_required_to_be_released_list_l>(), 1, 65536, true));
      break;
    case types::sl_drbs_required_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_required_to_be_modified_list_l>(), 1, 512, true));
      break;
    case types::sl_drbs_required_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_required_to_be_released_list_l>(), 1, 512, true));
      break;
    case types::target_cells_to_cancel:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<target_cell_list_l>(), 1, 8, true));
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_required_to_be_modified_list_l>(), 1, 32, true));
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_required_to_be_released_list_l>(), 1, 32, true));
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_required_to_be_modified_list_l>(), 1, 512, true));
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_required_to_be_released_list_l>(), 1, 512, true));
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_multicast_m_rbs_required_to_be_modified_list_l>(), 1, 64, true));
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_multicast_m_rbs_required_to_be_released_list_l>(), 1, 64, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_required_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_required_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::du_to_cu_rrc_info:
      HANDLE_CODE(c.get<du_to_cu_rrc_info_s>().unpack(bref));
      break;
    case types::drbs_required_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_required_to_be_modified_list_l>(), bref, 1, 64, true));
      break;
    case types::srbs_required_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_required_to_be_released_list_l>(), bref, 1, 8, true));
      break;
    case types::drbs_required_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_required_to_be_released_list_l>(), bref, 1, 64, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::bh_chs_required_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_required_to_be_released_list_l>(), bref, 1, 65536, true));
      break;
    case types::sl_drbs_required_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_required_to_be_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::sl_drbs_required_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_required_to_be_released_list_l>(), bref, 1, 512, true));
      break;
    case types::target_cells_to_cancel:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<target_cell_list_l>(), bref, 1, 8, true));
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_required_to_be_modified_list_l>(), bref, 1, 32, true));
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_required_to_be_released_list_l>(), bref, 1, 32, true));
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_required_to_be_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_required_to_be_released_list_l>(), bref, 1, 512, true));
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_multicast_m_rbs_required_to_be_modified_list_l>(), bref, 1, 64, true));
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_multicast_m_rbs_required_to_be_released_list_l>(), bref, 1, 64, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_required_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextModificationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_mod_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40,  41,  49,  39,  29,  21,  68,  17,  85,  13,  98,  7,   95,
                                   198, 204, 206, 94,  275, 273, 271, 269, 333, 318, 334, 314, 376,
                                   548, 602, 603, 604, 605, 611, 612, 616, 613, 676, 679, 697};
  return map_enum_number(names, 38, idx, "id");
}
bool ue_context_mod_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40,  41,  49,  39,  29,  21,  68,  17,  85,  13,  98,  7,   95,
                                   198, 204, 206, 94,  275, 273, 271, 269, 333, 318, 334, 314, 376,
                                   548, 602, 603, 604, 605, 611, 612, 616, 613, 676, 679, 697};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_mod_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 49:
      return crit_e::ignore;
    case 39:
      return crit_e::reject;
    case 29:
      return crit_e::ignore;
    case 21:
      return crit_e::ignore;
    case 68:
      return crit_e::ignore;
    case 17:
      return crit_e::ignore;
    case 85:
      return crit_e::ignore;
    case 13:
      return crit_e::ignore;
    case 98:
      return crit_e::reject;
    case 7:
      return crit_e::ignore;
    case 95:
      return crit_e::ignore;
    case 198:
      return crit_e::ignore;
    case 204:
      return crit_e::ignore;
    case 206:
      return crit_e::ignore;
    case 94:
      return crit_e::reject;
    case 275:
      return crit_e::ignore;
    case 273:
      return crit_e::ignore;
    case 271:
      return crit_e::ignore;
    case 269:
      return crit_e::ignore;
    case 333:
      return crit_e::ignore;
    case 318:
      return crit_e::ignore;
    case 334:
      return crit_e::ignore;
    case 314:
      return crit_e::ignore;
    case 376:
      return crit_e::reject;
    case 548:
      return crit_e::ignore;
    case 602:
      return crit_e::ignore;
    case 603:
      return crit_e::ignore;
    case 604:
      return crit_e::ignore;
    case 605:
      return crit_e::ignore;
    case 611:
      return crit_e::ignore;
    case 612:
      return crit_e::ignore;
    case 616:
      return crit_e::ignore;
    case 613:
      return crit_e::ignore;
    case 676:
      return crit_e::ignore;
    case 679:
      return crit_e::reject;
    case 697:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_mod_resp_ies_o::value_c ue_context_mod_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 49:
      ret.set(value_c::types::res_coordination_transfer_container);
      break;
    case 39:
      ret.set(value_c::types::du_to_cu_rrc_info);
      break;
    case 29:
      ret.set(value_c::types::drbs_setup_mod_list);
      break;
    case 21:
      ret.set(value_c::types::drbs_modified_list);
      break;
    case 68:
      ret.set(value_c::types::srbs_failed_to_be_setup_mod_list);
      break;
    case 17:
      ret.set(value_c::types::drbs_failed_to_be_setup_mod_list);
      break;
    case 85:
      ret.set(value_c::types::scell_failedto_setup_mod_list);
      break;
    case 13:
      ret.set(value_c::types::drbs_failed_to_be_modified_list);
      break;
    case 98:
      ret.set(value_c::types::inactivity_monitoring_resp);
      break;
    case 7:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 95:
      ret.set(value_c::types::c_rnti);
      break;
    case 198:
      ret.set(value_c::types::associated_scell_list);
      break;
    case 204:
      ret.set(value_c::types::srbs_setup_mod_list);
      break;
    case 206:
      ret.set(value_c::types::srbs_modified_list);
      break;
    case 94:
      ret.set(value_c::types::full_cfg);
      break;
    case 275:
      ret.set(value_c::types::bh_chs_setup_mod_list);
      break;
    case 273:
      ret.set(value_c::types::bh_chs_modified_list);
      break;
    case 271:
      ret.set(value_c::types::bh_chs_failed_to_be_setup_mod_list);
      break;
    case 269:
      ret.set(value_c::types::bh_chs_failed_to_be_modified_list);
      break;
    case 333:
      ret.set(value_c::types::sl_drbs_setup_mod_list);
      break;
    case 318:
      ret.set(value_c::types::sl_drbs_modified_list);
      break;
    case 334:
      ret.set(value_c::types::sl_drbs_failed_to_be_setup_mod_list);
      break;
    case 314:
      ret.set(value_c::types::sl_drbs_failed_to_be_modified_list);
      break;
    case 376:
      ret.set(value_c::types::requested_target_cell_global_id);
      break;
    case 548:
      ret.set(value_c::types::scg_activation_status);
      break;
    case 602:
      ret.set(value_c::types::uu_rlc_ch_setup_list);
      break;
    case 603:
      ret.set(value_c::types::uu_rlc_ch_failed_to_be_setup_list);
      break;
    case 604:
      ret.set(value_c::types::uu_rlc_ch_modified_list);
      break;
    case 605:
      ret.set(value_c::types::uu_rlc_ch_failed_to_be_modified_list);
      break;
    case 611:
      ret.set(value_c::types::pc5_rlc_ch_setup_list);
      break;
    case 612:
      ret.set(value_c::types::pc5_rlc_ch_failed_to_be_setup_list);
      break;
    case 616:
      ret.set(value_c::types::pc5_rlc_ch_modified_list);
      break;
    case 613:
      ret.set(value_c::types::pc5_rlc_ch_failed_to_be_modified_list);
      break;
    case 676:
      ret.set(value_c::types::sdt_bearer_cfg_info);
      break;
    case 679:
      ret.set(value_c::types::ue_multicast_m_rbs_setup_list);
      break;
    case 697:
      ret.set(value_c::types::serving_cell_mo_encoded_in_cgc_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_mod_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 49:
      return presence_e::optional;
    case 39:
      return presence_e::optional;
    case 29:
      return presence_e::optional;
    case 21:
      return presence_e::optional;
    case 68:
      return presence_e::optional;
    case 17:
      return presence_e::optional;
    case 85:
      return presence_e::optional;
    case 13:
      return presence_e::optional;
    case 98:
      return presence_e::optional;
    case 7:
      return presence_e::optional;
    case 95:
      return presence_e::optional;
    case 198:
      return presence_e::optional;
    case 204:
      return presence_e::optional;
    case 206:
      return presence_e::optional;
    case 94:
      return presence_e::optional;
    case 275:
      return presence_e::optional;
    case 273:
      return presence_e::optional;
    case 271:
      return presence_e::optional;
    case 269:
      return presence_e::optional;
    case 333:
      return presence_e::optional;
    case 318:
      return presence_e::optional;
    case 334:
      return presence_e::optional;
    case 314:
      return presence_e::optional;
    case 376:
      return presence_e::optional;
    case 548:
      return presence_e::optional;
    case 602:
      return presence_e::optional;
    case 603:
      return presence_e::optional;
    case 604:
      return presence_e::optional;
    case 605:
      return presence_e::optional;
    case 611:
      return presence_e::optional;
    case 612:
      return presence_e::optional;
    case 616:
      return presence_e::optional;
    case 613:
      return presence_e::optional;
    case 676:
      return presence_e::optional;
    case 679:
      return presence_e::optional;
    case 697:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_mod_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::res_coordination_transfer_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::du_to_cu_rrc_info:
      c.destroy<du_to_cu_rrc_info_s>();
      break;
    case types::drbs_setup_mod_list:
      c.destroy<drbs_setup_mod_list_l>();
      break;
    case types::drbs_modified_list:
      c.destroy<drbs_modified_list_l>();
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      c.destroy<srbs_failed_to_be_setup_mod_list_l>();
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      c.destroy<drbs_failed_to_be_setup_mod_list_l>();
      break;
    case types::scell_failedto_setup_mod_list:
      c.destroy<scell_failedto_setup_mod_list_l>();
      break;
    case types::drbs_failed_to_be_modified_list:
      c.destroy<drbs_failed_to_be_modified_list_l>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    case types::associated_scell_list:
      c.destroy<associated_scell_list_l>();
      break;
    case types::srbs_setup_mod_list:
      c.destroy<srbs_setup_mod_list_l>();
      break;
    case types::srbs_modified_list:
      c.destroy<srbs_modified_list_l>();
      break;
    case types::bh_chs_setup_mod_list:
      c.destroy<bh_chs_setup_mod_list_l>();
      break;
    case types::bh_chs_modified_list:
      c.destroy<bh_chs_modified_list_l>();
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      c.destroy<bh_chs_failed_to_be_setup_mod_list_l>();
      break;
    case types::bh_chs_failed_to_be_modified_list:
      c.destroy<bh_chs_failed_to_be_modified_list_l>();
      break;
    case types::sl_drbs_setup_mod_list:
      c.destroy<sl_drbs_setup_mod_list_l>();
      break;
    case types::sl_drbs_modified_list:
      c.destroy<sl_drbs_modified_list_l>();
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      c.destroy<sl_drbs_failed_to_be_setup_mod_list_l>();
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      c.destroy<sl_drbs_failed_to_be_modified_list_l>();
      break;
    case types::requested_target_cell_global_id:
      c.destroy<nr_cgi_s>();
      break;
    case types::uu_rlc_ch_setup_list:
      c.destroy<uu_rlc_ch_setup_list_l>();
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.destroy<uu_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::uu_rlc_ch_modified_list:
      c.destroy<uu_rlc_ch_modified_list_l>();
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      c.destroy<uu_rlc_ch_failed_to_be_modified_list_l>();
      break;
    case types::pc5_rlc_ch_setup_list:
      c.destroy<pc5_rlc_ch_setup_list_l>();
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.destroy<pc5_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_modified_list:
      c.destroy<pc5_rlc_ch_modified_list_l>();
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      c.destroy<pc5_rlc_ch_failed_to_be_modified_list_l>();
      break;
    case types::sdt_bearer_cfg_info:
      c.destroy<sdt_bearer_cfg_info_s>();
      break;
    case types::ue_multicast_m_rbs_setup_list:
      c.destroy<ue_multicast_m_rbs_setup_list_l>();
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.destroy<serving_cell_mo_encoded_in_cgc_list_l>();
      break;
    default:
      break;
  }
}
void ue_context_mod_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::res_coordination_transfer_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::du_to_cu_rrc_info:
      c.init<du_to_cu_rrc_info_s>();
      break;
    case types::drbs_setup_mod_list:
      c.init<drbs_setup_mod_list_l>();
      break;
    case types::drbs_modified_list:
      c.init<drbs_modified_list_l>();
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      c.init<srbs_failed_to_be_setup_mod_list_l>();
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      c.init<drbs_failed_to_be_setup_mod_list_l>();
      break;
    case types::scell_failedto_setup_mod_list:
      c.init<scell_failedto_setup_mod_list_l>();
      break;
    case types::drbs_failed_to_be_modified_list:
      c.init<drbs_failed_to_be_modified_list_l>();
      break;
    case types::inactivity_monitoring_resp:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::c_rnti:
      break;
    case types::associated_scell_list:
      c.init<associated_scell_list_l>();
      break;
    case types::srbs_setup_mod_list:
      c.init<srbs_setup_mod_list_l>();
      break;
    case types::srbs_modified_list:
      c.init<srbs_modified_list_l>();
      break;
    case types::full_cfg:
      break;
    case types::bh_chs_setup_mod_list:
      c.init<bh_chs_setup_mod_list_l>();
      break;
    case types::bh_chs_modified_list:
      c.init<bh_chs_modified_list_l>();
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      c.init<bh_chs_failed_to_be_setup_mod_list_l>();
      break;
    case types::bh_chs_failed_to_be_modified_list:
      c.init<bh_chs_failed_to_be_modified_list_l>();
      break;
    case types::sl_drbs_setup_mod_list:
      c.init<sl_drbs_setup_mod_list_l>();
      break;
    case types::sl_drbs_modified_list:
      c.init<sl_drbs_modified_list_l>();
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      c.init<sl_drbs_failed_to_be_setup_mod_list_l>();
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      c.init<sl_drbs_failed_to_be_modified_list_l>();
      break;
    case types::requested_target_cell_global_id:
      c.init<nr_cgi_s>();
      break;
    case types::scg_activation_status:
      break;
    case types::uu_rlc_ch_setup_list:
      c.init<uu_rlc_ch_setup_list_l>();
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.init<uu_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::uu_rlc_ch_modified_list:
      c.init<uu_rlc_ch_modified_list_l>();
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      c.init<uu_rlc_ch_failed_to_be_modified_list_l>();
      break;
    case types::pc5_rlc_ch_setup_list:
      c.init<pc5_rlc_ch_setup_list_l>();
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.init<pc5_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_modified_list:
      c.init<pc5_rlc_ch_modified_list_l>();
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      c.init<pc5_rlc_ch_failed_to_be_modified_list_l>();
      break;
    case types::sdt_bearer_cfg_info:
      c.init<sdt_bearer_cfg_info_s>();
      break;
    case types::ue_multicast_m_rbs_setup_list:
      c.init<ue_multicast_m_rbs_setup_list_l>();
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.init<serving_cell_mo_encoded_in_cgc_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_resp_ies_o::value_c");
  }
}
ue_context_mod_resp_ies_o::value_c::value_c(const ue_context_mod_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::du_to_cu_rrc_info:
      c.init(other.c.get<du_to_cu_rrc_info_s>());
      break;
    case types::drbs_setup_mod_list:
      c.init(other.c.get<drbs_setup_mod_list_l>());
      break;
    case types::drbs_modified_list:
      c.init(other.c.get<drbs_modified_list_l>());
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      c.init(other.c.get<srbs_failed_to_be_setup_mod_list_l>());
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      c.init(other.c.get<drbs_failed_to_be_setup_mod_list_l>());
      break;
    case types::scell_failedto_setup_mod_list:
      c.init(other.c.get<scell_failedto_setup_mod_list_l>());
      break;
    case types::drbs_failed_to_be_modified_list:
      c.init(other.c.get<drbs_failed_to_be_modified_list_l>());
      break;
    case types::inactivity_monitoring_resp:
      c.init(other.c.get<inactivity_monitoring_resp_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::c_rnti:
      c.init(other.c.get<uint32_t>());
      break;
    case types::associated_scell_list:
      c.init(other.c.get<associated_scell_list_l>());
      break;
    case types::srbs_setup_mod_list:
      c.init(other.c.get<srbs_setup_mod_list_l>());
      break;
    case types::srbs_modified_list:
      c.init(other.c.get<srbs_modified_list_l>());
      break;
    case types::full_cfg:
      c.init(other.c.get<full_cfg_e>());
      break;
    case types::bh_chs_setup_mod_list:
      c.init(other.c.get<bh_chs_setup_mod_list_l>());
      break;
    case types::bh_chs_modified_list:
      c.init(other.c.get<bh_chs_modified_list_l>());
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      c.init(other.c.get<bh_chs_failed_to_be_setup_mod_list_l>());
      break;
    case types::bh_chs_failed_to_be_modified_list:
      c.init(other.c.get<bh_chs_failed_to_be_modified_list_l>());
      break;
    case types::sl_drbs_setup_mod_list:
      c.init(other.c.get<sl_drbs_setup_mod_list_l>());
      break;
    case types::sl_drbs_modified_list:
      c.init(other.c.get<sl_drbs_modified_list_l>());
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      c.init(other.c.get<sl_drbs_failed_to_be_setup_mod_list_l>());
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      c.init(other.c.get<sl_drbs_failed_to_be_modified_list_l>());
      break;
    case types::requested_target_cell_global_id:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::scg_activation_status:
      c.init(other.c.get<scg_activation_status_e>());
      break;
    case types::uu_rlc_ch_setup_list:
      c.init(other.c.get<uu_rlc_ch_setup_list_l>());
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.init(other.c.get<uu_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::uu_rlc_ch_modified_list:
      c.init(other.c.get<uu_rlc_ch_modified_list_l>());
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      c.init(other.c.get<uu_rlc_ch_failed_to_be_modified_list_l>());
      break;
    case types::pc5_rlc_ch_setup_list:
      c.init(other.c.get<pc5_rlc_ch_setup_list_l>());
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.init(other.c.get<pc5_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_modified_list:
      c.init(other.c.get<pc5_rlc_ch_modified_list_l>());
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      c.init(other.c.get<pc5_rlc_ch_failed_to_be_modified_list_l>());
      break;
    case types::sdt_bearer_cfg_info:
      c.init(other.c.get<sdt_bearer_cfg_info_s>());
      break;
    case types::ue_multicast_m_rbs_setup_list:
      c.init(other.c.get<ue_multicast_m_rbs_setup_list_l>());
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.init(other.c.get<serving_cell_mo_encoded_in_cgc_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_resp_ies_o::value_c");
  }
}
ue_context_mod_resp_ies_o::value_c&
ue_context_mod_resp_ies_o::value_c::operator=(const ue_context_mod_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::du_to_cu_rrc_info:
      c.set(other.c.get<du_to_cu_rrc_info_s>());
      break;
    case types::drbs_setup_mod_list:
      c.set(other.c.get<drbs_setup_mod_list_l>());
      break;
    case types::drbs_modified_list:
      c.set(other.c.get<drbs_modified_list_l>());
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      c.set(other.c.get<srbs_failed_to_be_setup_mod_list_l>());
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      c.set(other.c.get<drbs_failed_to_be_setup_mod_list_l>());
      break;
    case types::scell_failedto_setup_mod_list:
      c.set(other.c.get<scell_failedto_setup_mod_list_l>());
      break;
    case types::drbs_failed_to_be_modified_list:
      c.set(other.c.get<drbs_failed_to_be_modified_list_l>());
      break;
    case types::inactivity_monitoring_resp:
      c.set(other.c.get<inactivity_monitoring_resp_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::c_rnti:
      c.set(other.c.get<uint32_t>());
      break;
    case types::associated_scell_list:
      c.set(other.c.get<associated_scell_list_l>());
      break;
    case types::srbs_setup_mod_list:
      c.set(other.c.get<srbs_setup_mod_list_l>());
      break;
    case types::srbs_modified_list:
      c.set(other.c.get<srbs_modified_list_l>());
      break;
    case types::full_cfg:
      c.set(other.c.get<full_cfg_e>());
      break;
    case types::bh_chs_setup_mod_list:
      c.set(other.c.get<bh_chs_setup_mod_list_l>());
      break;
    case types::bh_chs_modified_list:
      c.set(other.c.get<bh_chs_modified_list_l>());
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      c.set(other.c.get<bh_chs_failed_to_be_setup_mod_list_l>());
      break;
    case types::bh_chs_failed_to_be_modified_list:
      c.set(other.c.get<bh_chs_failed_to_be_modified_list_l>());
      break;
    case types::sl_drbs_setup_mod_list:
      c.set(other.c.get<sl_drbs_setup_mod_list_l>());
      break;
    case types::sl_drbs_modified_list:
      c.set(other.c.get<sl_drbs_modified_list_l>());
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      c.set(other.c.get<sl_drbs_failed_to_be_setup_mod_list_l>());
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      c.set(other.c.get<sl_drbs_failed_to_be_modified_list_l>());
      break;
    case types::requested_target_cell_global_id:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::scg_activation_status:
      c.set(other.c.get<scg_activation_status_e>());
      break;
    case types::uu_rlc_ch_setup_list:
      c.set(other.c.get<uu_rlc_ch_setup_list_l>());
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.set(other.c.get<uu_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::uu_rlc_ch_modified_list:
      c.set(other.c.get<uu_rlc_ch_modified_list_l>());
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      c.set(other.c.get<uu_rlc_ch_failed_to_be_modified_list_l>());
      break;
    case types::pc5_rlc_ch_setup_list:
      c.set(other.c.get<pc5_rlc_ch_setup_list_l>());
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.set(other.c.get<pc5_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_modified_list:
      c.set(other.c.get<pc5_rlc_ch_modified_list_l>());
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      c.set(other.c.get<pc5_rlc_ch_failed_to_be_modified_list_l>());
      break;
    case types::sdt_bearer_cfg_info:
      c.set(other.c.get<sdt_bearer_cfg_info_s>());
      break;
    case types::ue_multicast_m_rbs_setup_list:
      c.set(other.c.get<ue_multicast_m_rbs_setup_list_l>());
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.set(other.c.get<serving_cell_mo_encoded_in_cgc_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_resp_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_mod_resp_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_mod_resp_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
unbounded_octstring<true>& ue_context_mod_resp_ies_o::value_c::res_coordination_transfer_container()
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
du_to_cu_rrc_info_s& ue_context_mod_resp_ies_o::value_c::du_to_cu_rrc_info()
{
  assert_choice_type(types::du_to_cu_rrc_info, type_, "Value");
  return c.get<du_to_cu_rrc_info_s>();
}
drbs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::drbs_setup_mod_list()
{
  assert_choice_type(types::drbs_setup_mod_list, type_, "Value");
  return c.get<drbs_setup_mod_list_l>();
}
drbs_modified_list_l& ue_context_mod_resp_ies_o::value_c::drbs_modified_list()
{
  assert_choice_type(types::drbs_modified_list, type_, "Value");
  return c.get<drbs_modified_list_l>();
}
srbs_failed_to_be_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::srbs_failed_to_be_setup_mod_list()
{
  assert_choice_type(types::srbs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<srbs_failed_to_be_setup_mod_list_l>();
}
drbs_failed_to_be_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::drbs_failed_to_be_setup_mod_list()
{
  assert_choice_type(types::drbs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<drbs_failed_to_be_setup_mod_list_l>();
}
scell_failedto_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::scell_failedto_setup_mod_list()
{
  assert_choice_type(types::scell_failedto_setup_mod_list, type_, "Value");
  return c.get<scell_failedto_setup_mod_list_l>();
}
drbs_failed_to_be_modified_list_l& ue_context_mod_resp_ies_o::value_c::drbs_failed_to_be_modified_list()
{
  assert_choice_type(types::drbs_failed_to_be_modified_list, type_, "Value");
  return c.get<drbs_failed_to_be_modified_list_l>();
}
inactivity_monitoring_resp_e& ue_context_mod_resp_ies_o::value_c::inactivity_monitoring_resp()
{
  assert_choice_type(types::inactivity_monitoring_resp, type_, "Value");
  return c.get<inactivity_monitoring_resp_e>();
}
crit_diagnostics_s& ue_context_mod_resp_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
uint32_t& ue_context_mod_resp_ies_o::value_c::c_rnti()
{
  assert_choice_type(types::c_rnti, type_, "Value");
  return c.get<uint32_t>();
}
associated_scell_list_l& ue_context_mod_resp_ies_o::value_c::associated_scell_list()
{
  assert_choice_type(types::associated_scell_list, type_, "Value");
  return c.get<associated_scell_list_l>();
}
srbs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::srbs_setup_mod_list()
{
  assert_choice_type(types::srbs_setup_mod_list, type_, "Value");
  return c.get<srbs_setup_mod_list_l>();
}
srbs_modified_list_l& ue_context_mod_resp_ies_o::value_c::srbs_modified_list()
{
  assert_choice_type(types::srbs_modified_list, type_, "Value");
  return c.get<srbs_modified_list_l>();
}
full_cfg_e& ue_context_mod_resp_ies_o::value_c::full_cfg()
{
  assert_choice_type(types::full_cfg, type_, "Value");
  return c.get<full_cfg_e>();
}
bh_chs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::bh_chs_setup_mod_list()
{
  assert_choice_type(types::bh_chs_setup_mod_list, type_, "Value");
  return c.get<bh_chs_setup_mod_list_l>();
}
bh_chs_modified_list_l& ue_context_mod_resp_ies_o::value_c::bh_chs_modified_list()
{
  assert_choice_type(types::bh_chs_modified_list, type_, "Value");
  return c.get<bh_chs_modified_list_l>();
}
bh_chs_failed_to_be_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::bh_chs_failed_to_be_setup_mod_list()
{
  assert_choice_type(types::bh_chs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<bh_chs_failed_to_be_setup_mod_list_l>();
}
bh_chs_failed_to_be_modified_list_l& ue_context_mod_resp_ies_o::value_c::bh_chs_failed_to_be_modified_list()
{
  assert_choice_type(types::bh_chs_failed_to_be_modified_list, type_, "Value");
  return c.get<bh_chs_failed_to_be_modified_list_l>();
}
sl_drbs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::sl_drbs_setup_mod_list()
{
  assert_choice_type(types::sl_drbs_setup_mod_list, type_, "Value");
  return c.get<sl_drbs_setup_mod_list_l>();
}
sl_drbs_modified_list_l& ue_context_mod_resp_ies_o::value_c::sl_drbs_modified_list()
{
  assert_choice_type(types::sl_drbs_modified_list, type_, "Value");
  return c.get<sl_drbs_modified_list_l>();
}
sl_drbs_failed_to_be_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::sl_drbs_failed_to_be_setup_mod_list()
{
  assert_choice_type(types::sl_drbs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<sl_drbs_failed_to_be_setup_mod_list_l>();
}
sl_drbs_failed_to_be_modified_list_l& ue_context_mod_resp_ies_o::value_c::sl_drbs_failed_to_be_modified_list()
{
  assert_choice_type(types::sl_drbs_failed_to_be_modified_list, type_, "Value");
  return c.get<sl_drbs_failed_to_be_modified_list_l>();
}
nr_cgi_s& ue_context_mod_resp_ies_o::value_c::requested_target_cell_global_id()
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
scg_activation_status_e& ue_context_mod_resp_ies_o::value_c::scg_activation_status()
{
  assert_choice_type(types::scg_activation_status, type_, "Value");
  return c.get<scg_activation_status_e>();
}
uu_rlc_ch_setup_list_l& ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_setup_list()
{
  assert_choice_type(types::uu_rlc_ch_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_setup_list_l>();
}
uu_rlc_ch_failed_to_be_setup_list_l& ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_failed_to_be_setup_list()
{
  assert_choice_type(types::uu_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_failed_to_be_setup_list_l>();
}
uu_rlc_ch_modified_list_l& ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_modified_list()
{
  assert_choice_type(types::uu_rlc_ch_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_modified_list_l>();
}
uu_rlc_ch_failed_to_be_modified_list_l& ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_failed_to_be_modified_list()
{
  assert_choice_type(types::uu_rlc_ch_failed_to_be_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_failed_to_be_modified_list_l>();
}
pc5_rlc_ch_setup_list_l& ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_setup_list()
{
  assert_choice_type(types::pc5_rlc_ch_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_setup_list_l>();
}
pc5_rlc_ch_failed_to_be_setup_list_l& ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_failed_to_be_setup_list()
{
  assert_choice_type(types::pc5_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_failed_to_be_setup_list_l>();
}
pc5_rlc_ch_modified_list_l& ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_modified_list()
{
  assert_choice_type(types::pc5_rlc_ch_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_modified_list_l>();
}
pc5_rlc_ch_failed_to_be_modified_list_l& ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_failed_to_be_modified_list()
{
  assert_choice_type(types::pc5_rlc_ch_failed_to_be_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_failed_to_be_modified_list_l>();
}
sdt_bearer_cfg_info_s& ue_context_mod_resp_ies_o::value_c::sdt_bearer_cfg_info()
{
  assert_choice_type(types::sdt_bearer_cfg_info, type_, "Value");
  return c.get<sdt_bearer_cfg_info_s>();
}
ue_multicast_m_rbs_setup_list_l& ue_context_mod_resp_ies_o::value_c::ue_multicast_m_rbs_setup_list()
{
  assert_choice_type(types::ue_multicast_m_rbs_setup_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_setup_list_l>();
}
serving_cell_mo_encoded_in_cgc_list_l& ue_context_mod_resp_ies_o::value_c::serving_cell_mo_encoded_in_cgc_list()
{
  assert_choice_type(types::serving_cell_mo_encoded_in_cgc_list, type_, "Value");
  return c.get<serving_cell_mo_encoded_in_cgc_list_l>();
}
const uint64_t& ue_context_mod_resp_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_mod_resp_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const unbounded_octstring<true>& ue_context_mod_resp_ies_o::value_c::res_coordination_transfer_container() const
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const du_to_cu_rrc_info_s& ue_context_mod_resp_ies_o::value_c::du_to_cu_rrc_info() const
{
  assert_choice_type(types::du_to_cu_rrc_info, type_, "Value");
  return c.get<du_to_cu_rrc_info_s>();
}
const drbs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::drbs_setup_mod_list() const
{
  assert_choice_type(types::drbs_setup_mod_list, type_, "Value");
  return c.get<drbs_setup_mod_list_l>();
}
const drbs_modified_list_l& ue_context_mod_resp_ies_o::value_c::drbs_modified_list() const
{
  assert_choice_type(types::drbs_modified_list, type_, "Value");
  return c.get<drbs_modified_list_l>();
}
const srbs_failed_to_be_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::srbs_failed_to_be_setup_mod_list() const
{
  assert_choice_type(types::srbs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<srbs_failed_to_be_setup_mod_list_l>();
}
const drbs_failed_to_be_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::drbs_failed_to_be_setup_mod_list() const
{
  assert_choice_type(types::drbs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<drbs_failed_to_be_setup_mod_list_l>();
}
const scell_failedto_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::scell_failedto_setup_mod_list() const
{
  assert_choice_type(types::scell_failedto_setup_mod_list, type_, "Value");
  return c.get<scell_failedto_setup_mod_list_l>();
}
const drbs_failed_to_be_modified_list_l& ue_context_mod_resp_ies_o::value_c::drbs_failed_to_be_modified_list() const
{
  assert_choice_type(types::drbs_failed_to_be_modified_list, type_, "Value");
  return c.get<drbs_failed_to_be_modified_list_l>();
}
const inactivity_monitoring_resp_e& ue_context_mod_resp_ies_o::value_c::inactivity_monitoring_resp() const
{
  assert_choice_type(types::inactivity_monitoring_resp, type_, "Value");
  return c.get<inactivity_monitoring_resp_e>();
}
const crit_diagnostics_s& ue_context_mod_resp_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint32_t& ue_context_mod_resp_ies_o::value_c::c_rnti() const
{
  assert_choice_type(types::c_rnti, type_, "Value");
  return c.get<uint32_t>();
}
const associated_scell_list_l& ue_context_mod_resp_ies_o::value_c::associated_scell_list() const
{
  assert_choice_type(types::associated_scell_list, type_, "Value");
  return c.get<associated_scell_list_l>();
}
const srbs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::srbs_setup_mod_list() const
{
  assert_choice_type(types::srbs_setup_mod_list, type_, "Value");
  return c.get<srbs_setup_mod_list_l>();
}
const srbs_modified_list_l& ue_context_mod_resp_ies_o::value_c::srbs_modified_list() const
{
  assert_choice_type(types::srbs_modified_list, type_, "Value");
  return c.get<srbs_modified_list_l>();
}
const full_cfg_e& ue_context_mod_resp_ies_o::value_c::full_cfg() const
{
  assert_choice_type(types::full_cfg, type_, "Value");
  return c.get<full_cfg_e>();
}
const bh_chs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::bh_chs_setup_mod_list() const
{
  assert_choice_type(types::bh_chs_setup_mod_list, type_, "Value");
  return c.get<bh_chs_setup_mod_list_l>();
}
const bh_chs_modified_list_l& ue_context_mod_resp_ies_o::value_c::bh_chs_modified_list() const
{
  assert_choice_type(types::bh_chs_modified_list, type_, "Value");
  return c.get<bh_chs_modified_list_l>();
}
const bh_chs_failed_to_be_setup_mod_list_l&
ue_context_mod_resp_ies_o::value_c::bh_chs_failed_to_be_setup_mod_list() const
{
  assert_choice_type(types::bh_chs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<bh_chs_failed_to_be_setup_mod_list_l>();
}
const bh_chs_failed_to_be_modified_list_l& ue_context_mod_resp_ies_o::value_c::bh_chs_failed_to_be_modified_list() const
{
  assert_choice_type(types::bh_chs_failed_to_be_modified_list, type_, "Value");
  return c.get<bh_chs_failed_to_be_modified_list_l>();
}
const sl_drbs_setup_mod_list_l& ue_context_mod_resp_ies_o::value_c::sl_drbs_setup_mod_list() const
{
  assert_choice_type(types::sl_drbs_setup_mod_list, type_, "Value");
  return c.get<sl_drbs_setup_mod_list_l>();
}
const sl_drbs_modified_list_l& ue_context_mod_resp_ies_o::value_c::sl_drbs_modified_list() const
{
  assert_choice_type(types::sl_drbs_modified_list, type_, "Value");
  return c.get<sl_drbs_modified_list_l>();
}
const sl_drbs_failed_to_be_setup_mod_list_l&
ue_context_mod_resp_ies_o::value_c::sl_drbs_failed_to_be_setup_mod_list() const
{
  assert_choice_type(types::sl_drbs_failed_to_be_setup_mod_list, type_, "Value");
  return c.get<sl_drbs_failed_to_be_setup_mod_list_l>();
}
const sl_drbs_failed_to_be_modified_list_l&
ue_context_mod_resp_ies_o::value_c::sl_drbs_failed_to_be_modified_list() const
{
  assert_choice_type(types::sl_drbs_failed_to_be_modified_list, type_, "Value");
  return c.get<sl_drbs_failed_to_be_modified_list_l>();
}
const nr_cgi_s& ue_context_mod_resp_ies_o::value_c::requested_target_cell_global_id() const
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
const scg_activation_status_e& ue_context_mod_resp_ies_o::value_c::scg_activation_status() const
{
  assert_choice_type(types::scg_activation_status, type_, "Value");
  return c.get<scg_activation_status_e>();
}
const uu_rlc_ch_setup_list_l& ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_setup_list() const
{
  assert_choice_type(types::uu_rlc_ch_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_setup_list_l>();
}
const uu_rlc_ch_failed_to_be_setup_list_l& ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_failed_to_be_setup_list() const
{
  assert_choice_type(types::uu_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_failed_to_be_setup_list_l>();
}
const uu_rlc_ch_modified_list_l& ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_modified_list() const
{
  assert_choice_type(types::uu_rlc_ch_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_modified_list_l>();
}
const uu_rlc_ch_failed_to_be_modified_list_l&
ue_context_mod_resp_ies_o::value_c::uu_rlc_ch_failed_to_be_modified_list() const
{
  assert_choice_type(types::uu_rlc_ch_failed_to_be_modified_list, type_, "Value");
  return c.get<uu_rlc_ch_failed_to_be_modified_list_l>();
}
const pc5_rlc_ch_setup_list_l& ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_setup_list() const
{
  assert_choice_type(types::pc5_rlc_ch_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_setup_list_l>();
}
const pc5_rlc_ch_failed_to_be_setup_list_l&
ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_failed_to_be_setup_list() const
{
  assert_choice_type(types::pc5_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_failed_to_be_setup_list_l>();
}
const pc5_rlc_ch_modified_list_l& ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_modified_list() const
{
  assert_choice_type(types::pc5_rlc_ch_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_modified_list_l>();
}
const pc5_rlc_ch_failed_to_be_modified_list_l&
ue_context_mod_resp_ies_o::value_c::pc5_rlc_ch_failed_to_be_modified_list() const
{
  assert_choice_type(types::pc5_rlc_ch_failed_to_be_modified_list, type_, "Value");
  return c.get<pc5_rlc_ch_failed_to_be_modified_list_l>();
}
const sdt_bearer_cfg_info_s& ue_context_mod_resp_ies_o::value_c::sdt_bearer_cfg_info() const
{
  assert_choice_type(types::sdt_bearer_cfg_info, type_, "Value");
  return c.get<sdt_bearer_cfg_info_s>();
}
const ue_multicast_m_rbs_setup_list_l& ue_context_mod_resp_ies_o::value_c::ue_multicast_m_rbs_setup_list() const
{
  assert_choice_type(types::ue_multicast_m_rbs_setup_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_setup_list_l>();
}
const serving_cell_mo_encoded_in_cgc_list_l&
ue_context_mod_resp_ies_o::value_c::serving_cell_mo_encoded_in_cgc_list() const
{
  assert_choice_type(types::serving_cell_mo_encoded_in_cgc_list, type_, "Value");
  return c.get<serving_cell_mo_encoded_in_cgc_list_l>();
}
void ue_context_mod_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::res_coordination_transfer_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::du_to_cu_rrc_info:
      j.write_fieldname("DUtoCURRCInformation");
      c.get<du_to_cu_rrc_info_s>().to_json(j);
      break;
    case types::drbs_setup_mod_list:
      j.start_array("DRBs-SetupMod-List");
      for (const auto& e1 : c.get<drbs_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_modified_list:
      j.start_array("DRBs-Modified-List");
      for (const auto& e1 : c.get<drbs_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      j.start_array("SRBs-FailedToBeSetupMod-List");
      for (const auto& e1 : c.get<srbs_failed_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      j.start_array("DRBs-FailedToBeSetupMod-List");
      for (const auto& e1 : c.get<drbs_failed_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::scell_failedto_setup_mod_list:
      j.start_array("SCell-FailedtoSetupMod-List");
      for (const auto& e1 : c.get<scell_failedto_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_failed_to_be_modified_list:
      j.start_array("DRBs-FailedToBeModified-List");
      for (const auto& e1 : c.get<drbs_failed_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::inactivity_monitoring_resp:
      j.write_str("InactivityMonitoringResponse", "not-supported");
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    case types::c_rnti:
      j.write_int("INTEGER (0..65535,...)", c.get<uint32_t>());
      break;
    case types::associated_scell_list:
      j.start_array("Associated-SCell-List");
      for (const auto& e1 : c.get<associated_scell_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_setup_mod_list:
      j.start_array("SRBs-SetupMod-List");
      for (const auto& e1 : c.get<srbs_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_modified_list:
      j.start_array("SRBs-Modified-List");
      for (const auto& e1 : c.get<srbs_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::full_cfg:
      j.write_str("FullConfiguration", "full");
      break;
    case types::bh_chs_setup_mod_list:
      j.start_array("BHChannels-SetupMod-List");
      for (const auto& e1 : c.get<bh_chs_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::bh_chs_modified_list:
      j.start_array("BHChannels-Modified-List");
      for (const auto& e1 : c.get<bh_chs_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      j.start_array("BHChannels-FailedToBeSetupMod-List");
      for (const auto& e1 : c.get<bh_chs_failed_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::bh_chs_failed_to_be_modified_list:
      j.start_array("BHChannels-FailedToBeModified-List");
      for (const auto& e1 : c.get<bh_chs_failed_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_setup_mod_list:
      j.start_array("SLDRBs-SetupMod-List");
      for (const auto& e1 : c.get<sl_drbs_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_modified_list:
      j.start_array("SLDRBs-Modified-List");
      for (const auto& e1 : c.get<sl_drbs_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      j.start_array("SLDRBs-FailedToBeSetupMod-List");
      for (const auto& e1 : c.get<sl_drbs_failed_to_be_setup_mod_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      j.start_array("SLDRBs-FailedToBeModified-List");
      for (const auto& e1 : c.get<sl_drbs_failed_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::requested_target_cell_global_id:
      j.write_fieldname("NRCGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    case types::scg_activation_status:
      j.write_str("SCGActivationStatus", c.get<scg_activation_status_e>().to_string());
      break;
    case types::uu_rlc_ch_setup_list:
      j.start_array("UuRLCChannelSetupList");
      for (const auto& e1 : c.get<uu_rlc_ch_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      j.start_array("UuRLCChannelFailedToBeSetupList");
      for (const auto& e1 : c.get<uu_rlc_ch_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_modified_list:
      j.start_array("UuRLCChannelModifiedList");
      for (const auto& e1 : c.get<uu_rlc_ch_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      j.start_array("UuRLCChannelFailedToBeModifiedList");
      for (const auto& e1 : c.get<uu_rlc_ch_failed_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_setup_list:
      j.start_array("PC5RLCChannelSetupList");
      for (const auto& e1 : c.get<pc5_rlc_ch_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      j.start_array("PC5RLCChannelFailedToBeSetupList");
      for (const auto& e1 : c.get<pc5_rlc_ch_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_modified_list:
      j.start_array("PC5RLCChannelModifiedList");
      for (const auto& e1 : c.get<pc5_rlc_ch_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      j.start_array("PC5RLCChannelFailedToBeModifiedList");
      for (const auto& e1 : c.get<pc5_rlc_ch_failed_to_be_modified_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sdt_bearer_cfg_info:
      j.write_fieldname("SDTBearerConfigurationInfo");
      c.get<sdt_bearer_cfg_info_s>().to_json(j);
      break;
    case types::ue_multicast_m_rbs_setup_list:
      j.start_array("UE-MulticastMRBs-Setup-List");
      for (const auto& e1 : c.get<ue_multicast_m_rbs_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      j.start_array("ServingCellMO-encoded-in-CGC-List");
      for (const auto& e1 : c.get<serving_cell_mo_encoded_in_cgc_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_mod_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::du_to_cu_rrc_info:
      HANDLE_CODE(c.get<du_to_cu_rrc_info_s>().pack(bref));
      break;
    case types::drbs_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_setup_mod_list_l>(), 1, 64, true));
      break;
    case types::drbs_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_modified_list_l>(), 1, 64, true));
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_failed_to_be_setup_mod_list_l>(), 1, 8, true));
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_failed_to_be_setup_mod_list_l>(), 1, 64, true));
      break;
    case types::scell_failedto_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scell_failedto_setup_mod_list_l>(), 1, 32, true));
      break;
    case types::drbs_failed_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_failed_to_be_modified_list_l>(), 1, 64, true));
      break;
    case types::inactivity_monitoring_resp:
      HANDLE_CODE(c.get<inactivity_monitoring_resp_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::c_rnti:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u, true, true));
      break;
    case types::associated_scell_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<associated_scell_list_l>(), 1, 32, true));
      break;
    case types::srbs_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_setup_mod_list_l>(), 1, 8, true));
      break;
    case types::srbs_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_modified_list_l>(), 1, 8, true));
      break;
    case types::full_cfg:
      HANDLE_CODE(c.get<full_cfg_e>().pack(bref));
      break;
    case types::bh_chs_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_setup_mod_list_l>(), 1, 65536, true));
      break;
    case types::bh_chs_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_modified_list_l>(), 1, 65536, true));
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_failed_to_be_setup_mod_list_l>(), 1, 65536, true));
      break;
    case types::bh_chs_failed_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_failed_to_be_modified_list_l>(), 1, 65536, true));
      break;
    case types::sl_drbs_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_setup_mod_list_l>(), 1, 512, true));
      break;
    case types::sl_drbs_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_modified_list_l>(), 1, 512, true));
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_failed_to_be_setup_mod_list_l>(), 1, 512, true));
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_failed_to_be_modified_list_l>(), 1, 512, true));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    case types::scg_activation_status:
      HANDLE_CODE(c.get<scg_activation_status_e>().pack(bref));
      break;
    case types::uu_rlc_ch_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_setup_list_l>(), 1, 32, true));
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_failed_to_be_setup_list_l>(), 1, 32, true));
      break;
    case types::uu_rlc_ch_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_modified_list_l>(), 1, 32, true));
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_failed_to_be_modified_list_l>(), 1, 32, true));
      break;
    case types::pc5_rlc_ch_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_setup_list_l>(), 1, 512, true));
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_failed_to_be_setup_list_l>(), 1, 512, true));
      break;
    case types::pc5_rlc_ch_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_modified_list_l>(), 1, 512, true));
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_failed_to_be_modified_list_l>(), 1, 512, true));
      break;
    case types::sdt_bearer_cfg_info:
      HANDLE_CODE(c.get<sdt_bearer_cfg_info_s>().pack(bref));
      break;
    case types::ue_multicast_m_rbs_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_multicast_m_rbs_setup_list_l>(), 1, 64, true));
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<serving_cell_mo_encoded_in_cgc_list_l>(), 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::du_to_cu_rrc_info:
      HANDLE_CODE(c.get<du_to_cu_rrc_info_s>().unpack(bref));
      break;
    case types::drbs_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_setup_mod_list_l>(), bref, 1, 64, true));
      break;
    case types::drbs_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_modified_list_l>(), bref, 1, 64, true));
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_failed_to_be_setup_mod_list_l>(), bref, 1, 8, true));
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_failed_to_be_setup_mod_list_l>(), bref, 1, 64, true));
      break;
    case types::scell_failedto_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scell_failedto_setup_mod_list_l>(), bref, 1, 32, true));
      break;
    case types::drbs_failed_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_failed_to_be_modified_list_l>(), bref, 1, 64, true));
      break;
    case types::inactivity_monitoring_resp:
      HANDLE_CODE(c.get<inactivity_monitoring_resp_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::c_rnti:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u, true, true));
      break;
    case types::associated_scell_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<associated_scell_list_l>(), bref, 1, 32, true));
      break;
    case types::srbs_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_setup_mod_list_l>(), bref, 1, 8, true));
      break;
    case types::srbs_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_modified_list_l>(), bref, 1, 8, true));
      break;
    case types::full_cfg:
      HANDLE_CODE(c.get<full_cfg_e>().unpack(bref));
      break;
    case types::bh_chs_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_setup_mod_list_l>(), bref, 1, 65536, true));
      break;
    case types::bh_chs_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_modified_list_l>(), bref, 1, 65536, true));
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_failed_to_be_setup_mod_list_l>(), bref, 1, 65536, true));
      break;
    case types::bh_chs_failed_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_failed_to_be_modified_list_l>(), bref, 1, 65536, true));
      break;
    case types::sl_drbs_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_setup_mod_list_l>(), bref, 1, 512, true));
      break;
    case types::sl_drbs_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_failed_to_be_setup_mod_list_l>(), bref, 1, 512, true));
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_failed_to_be_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    case types::scg_activation_status:
      HANDLE_CODE(c.get<scg_activation_status_e>().unpack(bref));
      break;
    case types::uu_rlc_ch_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_failed_to_be_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::uu_rlc_ch_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_modified_list_l>(), bref, 1, 32, true));
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_failed_to_be_modified_list_l>(), bref, 1, 32, true));
      break;
    case types::pc5_rlc_ch_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_failed_to_be_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::pc5_rlc_ch_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_failed_to_be_modified_list_l>(), bref, 1, 512, true));
      break;
    case types::sdt_bearer_cfg_info:
      HANDLE_CODE(c.get<sdt_bearer_cfg_info_s>().unpack(bref));
      break;
    case types::ue_multicast_m_rbs_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_multicast_m_rbs_setup_list_l>(), bref, 1, 64, true));
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<serving_cell_mo_encoded_in_cgc_list_l>(), bref, 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_release_cmd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 0, 50, 64, 47, 109, 184, 375, 576, 588};
  return map_enum_number(names, 11, idx, "id");
}
bool ue_context_release_cmd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 0, 50, 64, 47, 109, 184, 375, 576, 588};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_release_cmd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 50:
      return crit_e::ignore;
    case 64:
      return crit_e::ignore;
    case 47:
      return crit_e::ignore;
    case 109:
      return crit_e::ignore;
    case 184:
      return crit_e::ignore;
    case 375:
      return crit_e::reject;
    case 576:
      return crit_e::reject;
    case 588:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_release_cmd_ies_o::value_c ue_context_release_cmd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 50:
      ret.set(value_c::types::rrc_container);
      break;
    case 64:
      ret.set(value_c::types::srb_id);
      break;
    case 47:
      ret.set(value_c::types::old_gnb_du_ue_f1ap_id);
      break;
    case 109:
      ret.set(value_c::types::execute_dupl);
      break;
    case 184:
      ret.set(value_c::types::rrc_delivery_status_request);
      break;
    case 375:
      ret.set(value_c::types::target_cells_to_cancel);
      break;
    case 576:
      ret.set(value_c::types::pos_conext_rev_ind);
      break;
    case 588:
      ret.set(value_c::types::cg_sdt_kept_ind);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_release_cmd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 50:
      return presence_e::optional;
    case 64:
      return presence_e::conditional;
    case 47:
      return presence_e::optional;
    case 109:
      return presence_e::optional;
    case 184:
      return presence_e::optional;
    case 375:
      return presence_e::optional;
    case 576:
      return presence_e::optional;
    case 588:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_release_cmd_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::target_cells_to_cancel:
      c.destroy<target_cell_list_l>();
      break;
    default:
      break;
  }
}
void ue_context_release_cmd_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::srb_id:
      break;
    case types::old_gnb_du_ue_f1ap_id:
      break;
    case types::execute_dupl:
      break;
    case types::rrc_delivery_status_request:
      break;
    case types::target_cells_to_cancel:
      c.init<target_cell_list_l>();
      break;
    case types::pos_conext_rev_ind:
      break;
    case types::cg_sdt_kept_ind:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_cmd_ies_o::value_c");
  }
}
ue_context_release_cmd_ies_o::value_c::value_c(const ue_context_release_cmd_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::srb_id:
      c.init(other.c.get<uint8_t>());
      break;
    case types::old_gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::execute_dupl:
      c.init(other.c.get<execute_dupl_e>());
      break;
    case types::rrc_delivery_status_request:
      c.init(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::target_cells_to_cancel:
      c.init(other.c.get<target_cell_list_l>());
      break;
    case types::pos_conext_rev_ind:
      c.init(other.c.get<pos_conext_rev_ind_e>());
      break;
    case types::cg_sdt_kept_ind:
      c.init(other.c.get<cg_sdt_kept_ind_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_cmd_ies_o::value_c");
  }
}
ue_context_release_cmd_ies_o::value_c&
ue_context_release_cmd_ies_o::value_c::operator=(const ue_context_release_cmd_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::srb_id:
      c.set(other.c.get<uint8_t>());
      break;
    case types::old_gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::execute_dupl:
      c.set(other.c.get<execute_dupl_e>());
      break;
    case types::rrc_delivery_status_request:
      c.set(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::target_cells_to_cancel:
      c.set(other.c.get<target_cell_list_l>());
      break;
    case types::pos_conext_rev_ind:
      c.set(other.c.get<pos_conext_rev_ind_e>());
      break;
    case types::cg_sdt_kept_ind:
      c.set(other.c.get<cg_sdt_kept_ind_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_cmd_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_release_cmd_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_release_cmd_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& ue_context_release_cmd_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
unbounded_octstring<true>& ue_context_release_cmd_ies_o::value_c::rrc_container()
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
uint8_t& ue_context_release_cmd_ies_o::value_c::srb_id()
{
  assert_choice_type(types::srb_id, type_, "Value");
  return c.get<uint8_t>();
}
uint64_t& ue_context_release_cmd_ies_o::value_c::old_gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::old_gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
execute_dupl_e& ue_context_release_cmd_ies_o::value_c::execute_dupl()
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
rrc_delivery_status_request_e& ue_context_release_cmd_ies_o::value_c::rrc_delivery_status_request()
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
target_cell_list_l& ue_context_release_cmd_ies_o::value_c::target_cells_to_cancel()
{
  assert_choice_type(types::target_cells_to_cancel, type_, "Value");
  return c.get<target_cell_list_l>();
}
pos_conext_rev_ind_e& ue_context_release_cmd_ies_o::value_c::pos_conext_rev_ind()
{
  assert_choice_type(types::pos_conext_rev_ind, type_, "Value");
  return c.get<pos_conext_rev_ind_e>();
}
cg_sdt_kept_ind_e& ue_context_release_cmd_ies_o::value_c::cg_sdt_kept_ind()
{
  assert_choice_type(types::cg_sdt_kept_ind, type_, "Value");
  return c.get<cg_sdt_kept_ind_e>();
}
const uint64_t& ue_context_release_cmd_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_release_cmd_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& ue_context_release_cmd_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const unbounded_octstring<true>& ue_context_release_cmd_ies_o::value_c::rrc_container() const
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const uint8_t& ue_context_release_cmd_ies_o::value_c::srb_id() const
{
  assert_choice_type(types::srb_id, type_, "Value");
  return c.get<uint8_t>();
}
const uint64_t& ue_context_release_cmd_ies_o::value_c::old_gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::old_gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const execute_dupl_e& ue_context_release_cmd_ies_o::value_c::execute_dupl() const
{
  assert_choice_type(types::execute_dupl, type_, "Value");
  return c.get<execute_dupl_e>();
}
const rrc_delivery_status_request_e& ue_context_release_cmd_ies_o::value_c::rrc_delivery_status_request() const
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
const target_cell_list_l& ue_context_release_cmd_ies_o::value_c::target_cells_to_cancel() const
{
  assert_choice_type(types::target_cells_to_cancel, type_, "Value");
  return c.get<target_cell_list_l>();
}
const pos_conext_rev_ind_e& ue_context_release_cmd_ies_o::value_c::pos_conext_rev_ind() const
{
  assert_choice_type(types::pos_conext_rev_ind, type_, "Value");
  return c.get<pos_conext_rev_ind_e>();
}
const cg_sdt_kept_ind_e& ue_context_release_cmd_ies_o::value_c::cg_sdt_kept_ind() const
{
  assert_choice_type(types::cg_sdt_kept_ind, type_, "Value");
  return c.get<cg_sdt_kept_ind_e>();
}
void ue_context_release_cmd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::srb_id:
      j.write_int("INTEGER (0..3,...)", c.get<uint8_t>());
      break;
    case types::old_gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::execute_dupl:
      j.write_str("ExecuteDuplication", "true");
      break;
    case types::rrc_delivery_status_request:
      j.write_str("RRCDeliveryStatusRequest", "true");
      break;
    case types::target_cells_to_cancel:
      j.start_array("TargetCellList");
      for (const auto& e1 : c.get<target_cell_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pos_conext_rev_ind:
      j.write_str("PosConextRevIndication", "true");
      break;
    case types::cg_sdt_kept_ind:
      j.write_str("CG-SDTKeptIndicator", "true");
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_cmd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_release_cmd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::srb_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u, true, true));
      break;
    case types::old_gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().pack(bref));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().pack(bref));
      break;
    case types::target_cells_to_cancel:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<target_cell_list_l>(), 1, 8, true));
      break;
    case types::pos_conext_rev_ind:
      HANDLE_CODE(c.get<pos_conext_rev_ind_e>().pack(bref));
      break;
    case types::cg_sdt_kept_ind:
      HANDLE_CODE(c.get<cg_sdt_kept_ind_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_cmd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_release_cmd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::srb_id:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u, true, true));
      break;
    case types::old_gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::execute_dupl:
      HANDLE_CODE(c.get<execute_dupl_e>().unpack(bref));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().unpack(bref));
      break;
    case types::target_cells_to_cancel:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<target_cell_list_l>(), bref, 1, 8, true));
      break;
    case types::pos_conext_rev_ind:
      HANDLE_CODE(c.get<pos_conext_rev_ind_e>().unpack(bref));
      break;
    case types::cg_sdt_kept_ind:
      HANDLE_CODE(c.get<cg_sdt_kept_ind_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_cmd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_release_complete_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 7};
  return map_enum_number(names, 3, idx, "id");
}
bool ue_context_release_complete_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 7};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_release_complete_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 7:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_release_complete_ies_o::value_c ue_context_release_complete_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 7:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_release_complete_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 7:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_release_complete_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void ue_context_release_complete_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_complete_ies_o::value_c");
  }
}
ue_context_release_complete_ies_o::value_c::value_c(const ue_context_release_complete_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_complete_ies_o::value_c");
  }
}
ue_context_release_complete_ies_o::value_c&
ue_context_release_complete_ies_o::value_c::operator=(const ue_context_release_complete_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_complete_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_release_complete_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_release_complete_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
crit_diagnostics_s& ue_context_release_complete_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint64_t& ue_context_release_complete_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_release_complete_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const crit_diagnostics_s& ue_context_release_complete_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ue_context_release_complete_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_complete_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_release_complete_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_complete_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_release_complete_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_complete_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextReleaseRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_release_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 0, 375};
  return map_enum_number(names, 4, idx, "id");
}
bool ue_context_release_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 0, 375};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_release_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 0:
      return crit_e::ignore;
    case 375:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_release_request_ies_o::value_c ue_context_release_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 375:
      ret.set(value_c::types::target_cells_to_cancel);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_release_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 0:
      return presence_e::mandatory;
    case 375:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_release_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::target_cells_to_cancel:
      c.destroy<target_cell_list_l>();
      break;
    default:
      break;
  }
}
void ue_context_release_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::target_cells_to_cancel:
      c.init<target_cell_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_request_ies_o::value_c");
  }
}
ue_context_release_request_ies_o::value_c::value_c(const ue_context_release_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::target_cells_to_cancel:
      c.init(other.c.get<target_cell_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_request_ies_o::value_c");
  }
}
ue_context_release_request_ies_o::value_c&
ue_context_release_request_ies_o::value_c::operator=(const ue_context_release_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::target_cells_to_cancel:
      c.set(other.c.get<target_cell_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_request_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_release_request_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_release_request_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& ue_context_release_request_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
target_cell_list_l& ue_context_release_request_ies_o::value_c::target_cells_to_cancel()
{
  assert_choice_type(types::target_cells_to_cancel, type_, "Value");
  return c.get<target_cell_list_l>();
}
const uint64_t& ue_context_release_request_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_release_request_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& ue_context_release_request_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const target_cell_list_l& ue_context_release_request_ies_o::value_c::target_cells_to_cancel() const
{
  assert_choice_type(types::target_cells_to_cancel, type_, "Value");
  return c.get<target_cell_list_l>();
}
void ue_context_release_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::target_cells_to_cancel:
      j.start_array("TargetCellList");
      for (const auto& e1 : c.get<target_cell_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_release_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::target_cells_to_cancel:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<target_cell_list_l>(), 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_release_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::target_cells_to_cancel:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<target_cell_list_l>(), bref, 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 0, 7, 92, 376};
  return map_enum_number(names, 6, idx, "id");
}
bool ue_context_setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 0, 7, 92, 376};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_setup_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::ignore;
    case 0:
      return crit_e::ignore;
    case 7:
      return crit_e::ignore;
    case 92:
      return crit_e::ignore;
    case 376:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_setup_fail_ies_o::value_c ue_context_setup_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 0:
      ret.set(value_c::types::cause);
      break;
    case 7:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 92:
      ret.set(value_c::types::potential_sp_cell_list);
      break;
    case 376:
      ret.set(value_c::types::requested_target_cell_global_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_setup_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::optional;
    case 0:
      return presence_e::mandatory;
    case 7:
      return presence_e::optional;
    case 92:
      return presence_e::optional;
    case 376:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_setup_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    case types::potential_sp_cell_list:
      c.destroy<potential_sp_cell_list_l>();
      break;
    case types::requested_target_cell_global_id:
      c.destroy<nr_cgi_s>();
      break;
    default:
      break;
  }
}
void ue_context_setup_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::potential_sp_cell_list:
      c.init<potential_sp_cell_list_l>();
      break;
    case types::requested_target_cell_global_id:
      c.init<nr_cgi_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_fail_ies_o::value_c");
  }
}
ue_context_setup_fail_ies_o::value_c::value_c(const ue_context_setup_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::potential_sp_cell_list:
      c.init(other.c.get<potential_sp_cell_list_l>());
      break;
    case types::requested_target_cell_global_id:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_fail_ies_o::value_c");
  }
}
ue_context_setup_fail_ies_o::value_c&
ue_context_setup_fail_ies_o::value_c::operator=(const ue_context_setup_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::potential_sp_cell_list:
      c.set(other.c.get<potential_sp_cell_list_l>());
      break;
    case types::requested_target_cell_global_id:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_fail_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_setup_fail_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_setup_fail_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
cause_c& ue_context_setup_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ue_context_setup_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
potential_sp_cell_list_l& ue_context_setup_fail_ies_o::value_c::potential_sp_cell_list()
{
  assert_choice_type(types::potential_sp_cell_list, type_, "Value");
  return c.get<potential_sp_cell_list_l>();
}
nr_cgi_s& ue_context_setup_fail_ies_o::value_c::requested_target_cell_global_id()
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
const uint64_t& ue_context_setup_fail_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_setup_fail_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const cause_c& ue_context_setup_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ue_context_setup_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const potential_sp_cell_list_l& ue_context_setup_fail_ies_o::value_c::potential_sp_cell_list() const
{
  assert_choice_type(types::potential_sp_cell_list, type_, "Value");
  return c.get<potential_sp_cell_list_l>();
}
const nr_cgi_s& ue_context_setup_fail_ies_o::value_c::requested_target_cell_global_id() const
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
void ue_context_setup_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
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
    case types::potential_sp_cell_list:
      j.start_array("Potential-SpCell-List");
      for (const auto& e1 : c.get<potential_sp_cell_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::requested_target_cell_global_id:
      j.write_fieldname("NRCGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_setup_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::potential_sp_cell_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<potential_sp_cell_list_l>(), 0, 64, true));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_setup_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::potential_sp_cell_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<potential_sp_cell_list_l>(), bref, 0, 64, true));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40,  41,  63,  107, 96,  9,   90,  38,  49,  54,  74,  35,  97,  108, 50,  126, 165,
                                   158, 184, 195, 182, 217, 226, 242, 248, 258, 282, 306, 307, 308, 309, 340, 330, 373,
                                   377, 382, 428, 511, 536, 547, 591, 594, 595, 596, 599, 608, 619, 626, 632, 630, 695};
  return map_enum_number(names, 51, idx, "id");
}
bool ue_context_setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40,  41,  63,  107, 96,  9,   90,  38,  49,  54,  74,  35,  97,  108, 50,  126, 165,
                                   158, 184, 195, 182, 217, 226, 242, 248, 258, 282, 306, 307, 308, 309, 340, 330, 373,
                                   377, 382, 428, 511, 536, 547, 591, 594, 595, 596, 599, 608, 619, 626, 632, 630, 695};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_setup_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::ignore;
    case 63:
      return crit_e::reject;
    case 107:
      return crit_e::reject;
    case 96:
      return crit_e::ignore;
    case 9:
      return crit_e::reject;
    case 90:
      return crit_e::ignore;
    case 38:
      return crit_e::ignore;
    case 49:
      return crit_e::ignore;
    case 54:
      return crit_e::ignore;
    case 74:
      return crit_e::reject;
    case 35:
      return crit_e::reject;
    case 97:
      return crit_e::reject;
    case 108:
      return crit_e::reject;
    case 50:
      return crit_e::ignore;
    case 126:
      return crit_e::ignore;
    case 165:
      return crit_e::ignore;
    case 158:
      return crit_e::ignore;
    case 184:
      return crit_e::ignore;
    case 195:
      return crit_e::ignore;
    case 182:
      return crit_e::ignore;
    case 217:
      return crit_e::reject;
    case 226:
      return crit_e::ignore;
    case 242:
      return crit_e::ignore;
    case 248:
      return crit_e::ignore;
    case 258:
      return crit_e::reject;
    case 282:
      return crit_e::reject;
    case 306:
      return crit_e::ignore;
    case 307:
      return crit_e::ignore;
    case 308:
      return crit_e::ignore;
    case 309:
      return crit_e::ignore;
    case 340:
      return crit_e::ignore;
    case 330:
      return crit_e::reject;
    case 373:
      return crit_e::reject;
    case 377:
      return crit_e::ignore;
    case 382:
      return crit_e::reject;
    case 428:
      return crit_e::reject;
    case 511:
      return crit_e::reject;
    case 536:
      return crit_e::ignore;
    case 547:
      return crit_e::ignore;
    case 591:
      return crit_e::ignore;
    case 594:
      return crit_e::ignore;
    case 595:
      return crit_e::ignore;
    case 596:
      return crit_e::ignore;
    case 599:
      return crit_e::reject;
    case 608:
      return crit_e::reject;
    case 619:
      return crit_e::ignore;
    case 626:
      return crit_e::ignore;
    case 632:
      return crit_e::reject;
    case 630:
      return crit_e::reject;
    case 695:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_setup_request_ies_o::value_c ue_context_setup_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 63:
      ret.set(value_c::types::sp_cell_id);
      break;
    case 107:
      ret.set(value_c::types::serv_cell_idx);
      break;
    case 96:
      ret.set(value_c::types::sp_cell_ul_cfg);
      break;
    case 9:
      ret.set(value_c::types::cu_to_du_rrc_info);
      break;
    case 90:
      ret.set(value_c::types::candidate_sp_cell_list);
      break;
    case 38:
      ret.set(value_c::types::drx_cycle);
      break;
    case 49:
      ret.set(value_c::types::res_coordination_transfer_container);
      break;
    case 54:
      ret.set(value_c::types::scell_to_be_setup_list);
      break;
    case 74:
      ret.set(value_c::types::srbs_to_be_setup_list);
      break;
    case 35:
      ret.set(value_c::types::drbs_to_be_setup_list);
      break;
    case 97:
      ret.set(value_c::types::inactivity_monitoring_request);
      break;
    case 108:
      ret.set(value_c::types::rat_freq_prio_info);
      break;
    case 50:
      ret.set(value_c::types::rrc_container);
      break;
    case 126:
      ret.set(value_c::types::masked_imeisv);
      break;
    case 165:
      ret.set(value_c::types::serving_plmn);
      break;
    case 158:
      ret.set(value_c::types::gnb_du_ue_ambr_ul);
      break;
    case 184:
      ret.set(value_c::types::rrc_delivery_status_request);
      break;
    case 195:
      ret.set(value_c::types::res_coordination_transfer_info);
      break;
    case 182:
      ret.set(value_c::types::serving_cell_mo);
      break;
    case 217:
      ret.set(value_c::types::new_gnb_cu_ue_f1ap_id);
      break;
    case 226:
      ret.set(value_c::types::ran_ue_id);
      break;
    case 242:
      ret.set(value_c::types::trace_activation);
      break;
    case 248:
      ret.set(value_c::types::add_rrm_prio_idx);
      break;
    case 258:
      ret.set(value_c::types::bh_chs_to_be_setup_list);
      break;
    case 282:
      ret.set(value_c::types::cfg_bap_address);
      break;
    case 306:
      ret.set(value_c::types::nr_v2x_services_authorized);
      break;
    case 307:
      ret.set(value_c::types::ltev2x_services_authorized);
      break;
    case 308:
      ret.set(value_c::types::nr_ue_sidelink_aggr_max_bitrate);
      break;
    case 309:
      ret.set(value_c::types::lte_ue_sidelink_aggr_max_bitrate);
      break;
    case 340:
      ret.set(value_c::types::pc5_link_ambr);
      break;
    case 330:
      ret.set(value_c::types::sl_drbs_to_be_setup_list);
      break;
    case 373:
      ret.set(value_c::types::conditional_inter_du_mob_info);
      break;
    case 377:
      ret.set(value_c::types::management_based_mdt_plmn_list);
      break;
    case 382:
      ret.set(value_c::types::serving_n_id);
      break;
    case 428:
      ret.set(value_c::types::f1_c_transfer_path);
      break;
    case 511:
      ret.set(value_c::types::f1_c_transfer_path_nr_dc);
      break;
    case 536:
      ret.set(value_c::types::mdt_polluted_meas_ind);
      break;
    case 547:
      ret.set(value_c::types::scg_activation_request);
      break;
    case 591:
      ret.set(value_c::types::cg_sdt_session_info_old);
      break;
    case 594:
      ret.set(value_c::types::five_g_pro_se_authorized);
      break;
    case 595:
      ret.set(value_c::types::five_g_pro_se_ue_pc5_aggr_max_bitrate);
      break;
    case 596:
      ret.set(value_c::types::five_g_pro_se_pc5_link_ambr);
      break;
    case 599:
      ret.set(value_c::types::uu_rlc_ch_to_be_setup_list);
      break;
    case 608:
      ret.set(value_c::types::pc5_rlc_ch_to_be_setup_list);
      break;
    case 619:
      ret.set(value_c::types::path_switch_cfg);
      break;
    case 626:
      ret.set(value_c::types::gnb_du_ue_slice_max_bit_rate_list);
      break;
    case 632:
      ret.set(value_c::types::multicast_mbs_session_setup_list);
      break;
    case 630:
      ret.set(value_c::types::ue_multicast_m_rbs_to_be_setup_list);
      break;
    case 695:
      ret.set(value_c::types::serving_cell_mo_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_setup_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::optional;
    case 63:
      return presence_e::mandatory;
    case 107:
      return presence_e::mandatory;
    case 96:
      return presence_e::optional;
    case 9:
      return presence_e::mandatory;
    case 90:
      return presence_e::optional;
    case 38:
      return presence_e::optional;
    case 49:
      return presence_e::optional;
    case 54:
      return presence_e::optional;
    case 74:
      return presence_e::optional;
    case 35:
      return presence_e::optional;
    case 97:
      return presence_e::optional;
    case 108:
      return presence_e::optional;
    case 50:
      return presence_e::optional;
    case 126:
      return presence_e::optional;
    case 165:
      return presence_e::optional;
    case 158:
      return presence_e::conditional;
    case 184:
      return presence_e::optional;
    case 195:
      return presence_e::optional;
    case 182:
      return presence_e::optional;
    case 217:
      return presence_e::optional;
    case 226:
      return presence_e::optional;
    case 242:
      return presence_e::optional;
    case 248:
      return presence_e::optional;
    case 258:
      return presence_e::optional;
    case 282:
      return presence_e::optional;
    case 306:
      return presence_e::optional;
    case 307:
      return presence_e::optional;
    case 308:
      return presence_e::optional;
    case 309:
      return presence_e::optional;
    case 340:
      return presence_e::optional;
    case 330:
      return presence_e::optional;
    case 373:
      return presence_e::optional;
    case 377:
      return presence_e::optional;
    case 382:
      return presence_e::optional;
    case 428:
      return presence_e::optional;
    case 511:
      return presence_e::optional;
    case 536:
      return presence_e::optional;
    case 547:
      return presence_e::optional;
    case 591:
      return presence_e::optional;
    case 594:
      return presence_e::optional;
    case 595:
      return presence_e::optional;
    case 596:
      return presence_e::optional;
    case 599:
      return presence_e::optional;
    case 608:
      return presence_e::optional;
    case 619:
      return presence_e::optional;
    case 626:
      return presence_e::optional;
    case 632:
      return presence_e::optional;
    case 630:
      return presence_e::optional;
    case 695:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_setup_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::sp_cell_id:
      c.destroy<nr_cgi_s>();
      break;
    case types::cu_to_du_rrc_info:
      c.destroy<cu_to_du_rrc_info_s>();
      break;
    case types::candidate_sp_cell_list:
      c.destroy<candidate_sp_cell_list_l>();
      break;
    case types::drx_cycle:
      c.destroy<drx_cycle_s>();
      break;
    case types::res_coordination_transfer_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::scell_to_be_setup_list:
      c.destroy<scell_to_be_setup_list_l>();
      break;
    case types::srbs_to_be_setup_list:
      c.destroy<srbs_to_be_setup_list_l>();
      break;
    case types::drbs_to_be_setup_list:
      c.destroy<drbs_to_be_setup_list_l>();
      break;
    case types::rat_freq_prio_info:
      c.destroy<rat_freq_prio_info_c>();
      break;
    case types::rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::masked_imeisv:
      c.destroy<fixed_bitstring<64, false, true>>();
      break;
    case types::serving_plmn:
      c.destroy<fixed_octstring<3, true>>();
      break;
    case types::res_coordination_transfer_info:
      c.destroy<res_coordination_transfer_info_s>();
      break;
    case types::ran_ue_id:
      c.destroy<fixed_octstring<8, true>>();
      break;
    case types::trace_activation:
      c.destroy<trace_activation_s>();
      break;
    case types::add_rrm_prio_idx:
      c.destroy<fixed_bitstring<32, false, true>>();
      break;
    case types::bh_chs_to_be_setup_list:
      c.destroy<bh_chs_to_be_setup_list_l>();
      break;
    case types::cfg_bap_address:
      c.destroy<fixed_bitstring<10, false, true>>();
      break;
    case types::nr_v2x_services_authorized:
      c.destroy<nr_v2x_services_authorized_s>();
      break;
    case types::ltev2x_services_authorized:
      c.destroy<ltev2x_services_authorized_s>();
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.destroy<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.destroy<lte_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::sl_drbs_to_be_setup_list:
      c.destroy<sl_drbs_to_be_setup_list_l>();
      break;
    case types::conditional_inter_du_mob_info:
      c.destroy<conditional_inter_du_mob_info_s>();
      break;
    case types::management_based_mdt_plmn_list:
      c.destroy<mdt_plmn_list_l>();
      break;
    case types::serving_n_id:
      c.destroy<fixed_bitstring<44, false, true>>();
      break;
    case types::f1_c_transfer_path:
      c.destroy<f1_c_transfer_path_s>();
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.destroy<f1_c_transfer_path_nr_dc_s>();
      break;
    case types::cg_sdt_session_info_old:
      c.destroy<cg_sdt_session_info_s>();
      break;
    case types::five_g_pro_se_authorized:
      c.destroy<five_g_pro_se_authorized_s>();
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.destroy<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.destroy<uu_rlc_ch_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.destroy<pc5_rlc_ch_to_be_setup_list_l>();
      break;
    case types::path_switch_cfg:
      c.destroy<path_switch_cfg_s>();
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.destroy<gnb_du_ue_slice_max_bit_rate_list_l>();
      break;
    case types::multicast_mbs_session_setup_list:
      c.destroy<multicast_mbs_session_list_l>();
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      c.destroy<ue_multicast_m_rbs_to_be_setup_list_l>();
      break;
    case types::serving_cell_mo_list:
      c.destroy<serving_cell_mo_list_l>();
      break;
    default:
      break;
  }
}
void ue_context_setup_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::sp_cell_id:
      c.init<nr_cgi_s>();
      break;
    case types::serv_cell_idx:
      break;
    case types::sp_cell_ul_cfg:
      break;
    case types::cu_to_du_rrc_info:
      c.init<cu_to_du_rrc_info_s>();
      break;
    case types::candidate_sp_cell_list:
      c.init<candidate_sp_cell_list_l>();
      break;
    case types::drx_cycle:
      c.init<drx_cycle_s>();
      break;
    case types::res_coordination_transfer_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::scell_to_be_setup_list:
      c.init<scell_to_be_setup_list_l>();
      break;
    case types::srbs_to_be_setup_list:
      c.init<srbs_to_be_setup_list_l>();
      break;
    case types::drbs_to_be_setup_list:
      c.init<drbs_to_be_setup_list_l>();
      break;
    case types::inactivity_monitoring_request:
      break;
    case types::rat_freq_prio_info:
      c.init<rat_freq_prio_info_c>();
      break;
    case types::rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::masked_imeisv:
      c.init<fixed_bitstring<64, false, true>>();
      break;
    case types::serving_plmn:
      c.init<fixed_octstring<3, true>>();
      break;
    case types::gnb_du_ue_ambr_ul:
      break;
    case types::rrc_delivery_status_request:
      break;
    case types::res_coordination_transfer_info:
      c.init<res_coordination_transfer_info_s>();
      break;
    case types::serving_cell_mo:
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      break;
    case types::ran_ue_id:
      c.init<fixed_octstring<8, true>>();
      break;
    case types::trace_activation:
      c.init<trace_activation_s>();
      break;
    case types::add_rrm_prio_idx:
      c.init<fixed_bitstring<32, false, true>>();
      break;
    case types::bh_chs_to_be_setup_list:
      c.init<bh_chs_to_be_setup_list_l>();
      break;
    case types::cfg_bap_address:
      c.init<fixed_bitstring<10, false, true>>();
      break;
    case types::nr_v2x_services_authorized:
      c.init<nr_v2x_services_authorized_s>();
      break;
    case types::ltev2x_services_authorized:
      c.init<ltev2x_services_authorized_s>();
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.init<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.init<lte_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::pc5_link_ambr:
      break;
    case types::sl_drbs_to_be_setup_list:
      c.init<sl_drbs_to_be_setup_list_l>();
      break;
    case types::conditional_inter_du_mob_info:
      c.init<conditional_inter_du_mob_info_s>();
      break;
    case types::management_based_mdt_plmn_list:
      c.init<mdt_plmn_list_l>();
      break;
    case types::serving_n_id:
      c.init<fixed_bitstring<44, false, true>>();
      break;
    case types::f1_c_transfer_path:
      c.init<f1_c_transfer_path_s>();
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.init<f1_c_transfer_path_nr_dc_s>();
      break;
    case types::mdt_polluted_meas_ind:
      break;
    case types::scg_activation_request:
      break;
    case types::cg_sdt_session_info_old:
      c.init<cg_sdt_session_info_s>();
      break;
    case types::five_g_pro_se_authorized:
      c.init<five_g_pro_se_authorized_s>();
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.init<nr_ue_sidelink_aggr_max_bitrate_s>();
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.init<uu_rlc_ch_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.init<pc5_rlc_ch_to_be_setup_list_l>();
      break;
    case types::path_switch_cfg:
      c.init<path_switch_cfg_s>();
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.init<gnb_du_ue_slice_max_bit_rate_list_l>();
      break;
    case types::multicast_mbs_session_setup_list:
      c.init<multicast_mbs_session_list_l>();
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      c.init<ue_multicast_m_rbs_to_be_setup_list_l>();
      break;
    case types::serving_cell_mo_list:
      c.init<serving_cell_mo_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_request_ies_o::value_c");
  }
}
ue_context_setup_request_ies_o::value_c::value_c(const ue_context_setup_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sp_cell_id:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::serv_cell_idx:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sp_cell_ul_cfg:
      c.init(other.c.get<cell_ul_cfg_e>());
      break;
    case types::cu_to_du_rrc_info:
      c.init(other.c.get<cu_to_du_rrc_info_s>());
      break;
    case types::candidate_sp_cell_list:
      c.init(other.c.get<candidate_sp_cell_list_l>());
      break;
    case types::drx_cycle:
      c.init(other.c.get<drx_cycle_s>());
      break;
    case types::res_coordination_transfer_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::scell_to_be_setup_list:
      c.init(other.c.get<scell_to_be_setup_list_l>());
      break;
    case types::srbs_to_be_setup_list:
      c.init(other.c.get<srbs_to_be_setup_list_l>());
      break;
    case types::drbs_to_be_setup_list:
      c.init(other.c.get<drbs_to_be_setup_list_l>());
      break;
    case types::inactivity_monitoring_request:
      c.init(other.c.get<inactivity_monitoring_request_e>());
      break;
    case types::rat_freq_prio_info:
      c.init(other.c.get<rat_freq_prio_info_c>());
      break;
    case types::rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::masked_imeisv:
      c.init(other.c.get<fixed_bitstring<64, false, true>>());
      break;
    case types::serving_plmn:
      c.init(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::gnb_du_ue_ambr_ul:
      c.init(other.c.get<uint64_t>());
      break;
    case types::rrc_delivery_status_request:
      c.init(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::res_coordination_transfer_info:
      c.init(other.c.get<res_coordination_transfer_info_s>());
      break;
    case types::serving_cell_mo:
      c.init(other.c.get<uint8_t>());
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::ran_ue_id:
      c.init(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::trace_activation:
      c.init(other.c.get<trace_activation_s>());
      break;
    case types::add_rrm_prio_idx:
      c.init(other.c.get<fixed_bitstring<32, false, true>>());
      break;
    case types::bh_chs_to_be_setup_list:
      c.init(other.c.get<bh_chs_to_be_setup_list_l>());
      break;
    case types::cfg_bap_address:
      c.init(other.c.get<fixed_bitstring<10, false, true>>());
      break;
    case types::nr_v2x_services_authorized:
      c.init(other.c.get<nr_v2x_services_authorized_s>());
      break;
    case types::ltev2x_services_authorized:
      c.init(other.c.get<ltev2x_services_authorized_s>());
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.init(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.init(other.c.get<lte_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::pc5_link_ambr:
      c.init(other.c.get<uint64_t>());
      break;
    case types::sl_drbs_to_be_setup_list:
      c.init(other.c.get<sl_drbs_to_be_setup_list_l>());
      break;
    case types::conditional_inter_du_mob_info:
      c.init(other.c.get<conditional_inter_du_mob_info_s>());
      break;
    case types::management_based_mdt_plmn_list:
      c.init(other.c.get<mdt_plmn_list_l>());
      break;
    case types::serving_n_id:
      c.init(other.c.get<fixed_bitstring<44, false, true>>());
      break;
    case types::f1_c_transfer_path:
      c.init(other.c.get<f1_c_transfer_path_s>());
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.init(other.c.get<f1_c_transfer_path_nr_dc_s>());
      break;
    case types::mdt_polluted_meas_ind:
      c.init(other.c.get<mdt_polluted_meas_ind_e>());
      break;
    case types::scg_activation_request:
      c.init(other.c.get<scg_activation_request_e>());
      break;
    case types::cg_sdt_session_info_old:
      c.init(other.c.get<cg_sdt_session_info_s>());
      break;
    case types::five_g_pro_se_authorized:
      c.init(other.c.get<five_g_pro_se_authorized_s>());
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.init(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      c.init(other.c.get<uint64_t>());
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.init(other.c.get<uu_rlc_ch_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.init(other.c.get<pc5_rlc_ch_to_be_setup_list_l>());
      break;
    case types::path_switch_cfg:
      c.init(other.c.get<path_switch_cfg_s>());
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.init(other.c.get<gnb_du_ue_slice_max_bit_rate_list_l>());
      break;
    case types::multicast_mbs_session_setup_list:
      c.init(other.c.get<multicast_mbs_session_list_l>());
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      c.init(other.c.get<ue_multicast_m_rbs_to_be_setup_list_l>());
      break;
    case types::serving_cell_mo_list:
      c.init(other.c.get<serving_cell_mo_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_request_ies_o::value_c");
  }
}
ue_context_setup_request_ies_o::value_c&
ue_context_setup_request_ies_o::value_c::operator=(const ue_context_setup_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sp_cell_id:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::serv_cell_idx:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sp_cell_ul_cfg:
      c.set(other.c.get<cell_ul_cfg_e>());
      break;
    case types::cu_to_du_rrc_info:
      c.set(other.c.get<cu_to_du_rrc_info_s>());
      break;
    case types::candidate_sp_cell_list:
      c.set(other.c.get<candidate_sp_cell_list_l>());
      break;
    case types::drx_cycle:
      c.set(other.c.get<drx_cycle_s>());
      break;
    case types::res_coordination_transfer_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::scell_to_be_setup_list:
      c.set(other.c.get<scell_to_be_setup_list_l>());
      break;
    case types::srbs_to_be_setup_list:
      c.set(other.c.get<srbs_to_be_setup_list_l>());
      break;
    case types::drbs_to_be_setup_list:
      c.set(other.c.get<drbs_to_be_setup_list_l>());
      break;
    case types::inactivity_monitoring_request:
      c.set(other.c.get<inactivity_monitoring_request_e>());
      break;
    case types::rat_freq_prio_info:
      c.set(other.c.get<rat_freq_prio_info_c>());
      break;
    case types::rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::masked_imeisv:
      c.set(other.c.get<fixed_bitstring<64, false, true>>());
      break;
    case types::serving_plmn:
      c.set(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::gnb_du_ue_ambr_ul:
      c.set(other.c.get<uint64_t>());
      break;
    case types::rrc_delivery_status_request:
      c.set(other.c.get<rrc_delivery_status_request_e>());
      break;
    case types::res_coordination_transfer_info:
      c.set(other.c.get<res_coordination_transfer_info_s>());
      break;
    case types::serving_cell_mo:
      c.set(other.c.get<uint8_t>());
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::ran_ue_id:
      c.set(other.c.get<fixed_octstring<8, true>>());
      break;
    case types::trace_activation:
      c.set(other.c.get<trace_activation_s>());
      break;
    case types::add_rrm_prio_idx:
      c.set(other.c.get<fixed_bitstring<32, false, true>>());
      break;
    case types::bh_chs_to_be_setup_list:
      c.set(other.c.get<bh_chs_to_be_setup_list_l>());
      break;
    case types::cfg_bap_address:
      c.set(other.c.get<fixed_bitstring<10, false, true>>());
      break;
    case types::nr_v2x_services_authorized:
      c.set(other.c.get<nr_v2x_services_authorized_s>());
      break;
    case types::ltev2x_services_authorized:
      c.set(other.c.get<ltev2x_services_authorized_s>());
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c.set(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c.set(other.c.get<lte_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::pc5_link_ambr:
      c.set(other.c.get<uint64_t>());
      break;
    case types::sl_drbs_to_be_setup_list:
      c.set(other.c.get<sl_drbs_to_be_setup_list_l>());
      break;
    case types::conditional_inter_du_mob_info:
      c.set(other.c.get<conditional_inter_du_mob_info_s>());
      break;
    case types::management_based_mdt_plmn_list:
      c.set(other.c.get<mdt_plmn_list_l>());
      break;
    case types::serving_n_id:
      c.set(other.c.get<fixed_bitstring<44, false, true>>());
      break;
    case types::f1_c_transfer_path:
      c.set(other.c.get<f1_c_transfer_path_s>());
      break;
    case types::f1_c_transfer_path_nr_dc:
      c.set(other.c.get<f1_c_transfer_path_nr_dc_s>());
      break;
    case types::mdt_polluted_meas_ind:
      c.set(other.c.get<mdt_polluted_meas_ind_e>());
      break;
    case types::scg_activation_request:
      c.set(other.c.get<scg_activation_request_e>());
      break;
    case types::cg_sdt_session_info_old:
      c.set(other.c.get<cg_sdt_session_info_s>());
      break;
    case types::five_g_pro_se_authorized:
      c.set(other.c.get<five_g_pro_se_authorized_s>());
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c.set(other.c.get<nr_ue_sidelink_aggr_max_bitrate_s>());
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      c.set(other.c.get<uint64_t>());
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c.set(other.c.get<uu_rlc_ch_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c.set(other.c.get<pc5_rlc_ch_to_be_setup_list_l>());
      break;
    case types::path_switch_cfg:
      c.set(other.c.get<path_switch_cfg_s>());
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c.set(other.c.get<gnb_du_ue_slice_max_bit_rate_list_l>());
      break;
    case types::multicast_mbs_session_setup_list:
      c.set(other.c.get<multicast_mbs_session_list_l>());
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      c.set(other.c.get<ue_multicast_m_rbs_to_be_setup_list_l>());
      break;
    case types::serving_cell_mo_list:
      c.set(other.c.get<serving_cell_mo_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_request_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_setup_request_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_setup_request_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
nr_cgi_s& ue_context_setup_request_ies_o::value_c::sp_cell_id()
{
  assert_choice_type(types::sp_cell_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
uint8_t& ue_context_setup_request_ies_o::value_c::serv_cell_idx()
{
  assert_choice_type(types::serv_cell_idx, type_, "Value");
  return c.get<uint8_t>();
}
cell_ul_cfg_e& ue_context_setup_request_ies_o::value_c::sp_cell_ul_cfg()
{
  assert_choice_type(types::sp_cell_ul_cfg, type_, "Value");
  return c.get<cell_ul_cfg_e>();
}
cu_to_du_rrc_info_s& ue_context_setup_request_ies_o::value_c::cu_to_du_rrc_info()
{
  assert_choice_type(types::cu_to_du_rrc_info, type_, "Value");
  return c.get<cu_to_du_rrc_info_s>();
}
candidate_sp_cell_list_l& ue_context_setup_request_ies_o::value_c::candidate_sp_cell_list()
{
  assert_choice_type(types::candidate_sp_cell_list, type_, "Value");
  return c.get<candidate_sp_cell_list_l>();
}
drx_cycle_s& ue_context_setup_request_ies_o::value_c::drx_cycle()
{
  assert_choice_type(types::drx_cycle, type_, "Value");
  return c.get<drx_cycle_s>();
}
unbounded_octstring<true>& ue_context_setup_request_ies_o::value_c::res_coordination_transfer_container()
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
scell_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::scell_to_be_setup_list()
{
  assert_choice_type(types::scell_to_be_setup_list, type_, "Value");
  return c.get<scell_to_be_setup_list_l>();
}
srbs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::srbs_to_be_setup_list()
{
  assert_choice_type(types::srbs_to_be_setup_list, type_, "Value");
  return c.get<srbs_to_be_setup_list_l>();
}
drbs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::drbs_to_be_setup_list()
{
  assert_choice_type(types::drbs_to_be_setup_list, type_, "Value");
  return c.get<drbs_to_be_setup_list_l>();
}
inactivity_monitoring_request_e& ue_context_setup_request_ies_o::value_c::inactivity_monitoring_request()
{
  assert_choice_type(types::inactivity_monitoring_request, type_, "Value");
  return c.get<inactivity_monitoring_request_e>();
}
rat_freq_prio_info_c& ue_context_setup_request_ies_o::value_c::rat_freq_prio_info()
{
  assert_choice_type(types::rat_freq_prio_info, type_, "Value");
  return c.get<rat_freq_prio_info_c>();
}
unbounded_octstring<true>& ue_context_setup_request_ies_o::value_c::rrc_container()
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
fixed_bitstring<64, false, true>& ue_context_setup_request_ies_o::value_c::masked_imeisv()
{
  assert_choice_type(types::masked_imeisv, type_, "Value");
  return c.get<fixed_bitstring<64, false, true>>();
}
fixed_octstring<3, true>& ue_context_setup_request_ies_o::value_c::serving_plmn()
{
  assert_choice_type(types::serving_plmn, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
uint64_t& ue_context_setup_request_ies_o::value_c::gnb_du_ue_ambr_ul()
{
  assert_choice_type(types::gnb_du_ue_ambr_ul, type_, "Value");
  return c.get<uint64_t>();
}
rrc_delivery_status_request_e& ue_context_setup_request_ies_o::value_c::rrc_delivery_status_request()
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
res_coordination_transfer_info_s& ue_context_setup_request_ies_o::value_c::res_coordination_transfer_info()
{
  assert_choice_type(types::res_coordination_transfer_info, type_, "Value");
  return c.get<res_coordination_transfer_info_s>();
}
uint8_t& ue_context_setup_request_ies_o::value_c::serving_cell_mo()
{
  assert_choice_type(types::serving_cell_mo, type_, "Value");
  return c.get<uint8_t>();
}
uint64_t& ue_context_setup_request_ies_o::value_c::new_gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::new_gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
fixed_octstring<8, true>& ue_context_setup_request_ies_o::value_c::ran_ue_id()
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
trace_activation_s& ue_context_setup_request_ies_o::value_c::trace_activation()
{
  assert_choice_type(types::trace_activation, type_, "Value");
  return c.get<trace_activation_s>();
}
fixed_bitstring<32, false, true>& ue_context_setup_request_ies_o::value_c::add_rrm_prio_idx()
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
bh_chs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::bh_chs_to_be_setup_list()
{
  assert_choice_type(types::bh_chs_to_be_setup_list, type_, "Value");
  return c.get<bh_chs_to_be_setup_list_l>();
}
fixed_bitstring<10, false, true>& ue_context_setup_request_ies_o::value_c::cfg_bap_address()
{
  assert_choice_type(types::cfg_bap_address, type_, "Value");
  return c.get<fixed_bitstring<10, false, true>>();
}
nr_v2x_services_authorized_s& ue_context_setup_request_ies_o::value_c::nr_v2x_services_authorized()
{
  assert_choice_type(types::nr_v2x_services_authorized, type_, "Value");
  return c.get<nr_v2x_services_authorized_s>();
}
ltev2x_services_authorized_s& ue_context_setup_request_ies_o::value_c::ltev2x_services_authorized()
{
  assert_choice_type(types::ltev2x_services_authorized, type_, "Value");
  return c.get<ltev2x_services_authorized_s>();
}
nr_ue_sidelink_aggr_max_bitrate_s& ue_context_setup_request_ies_o::value_c::nr_ue_sidelink_aggr_max_bitrate()
{
  assert_choice_type(types::nr_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
lte_ue_sidelink_aggr_max_bitrate_s& ue_context_setup_request_ies_o::value_c::lte_ue_sidelink_aggr_max_bitrate()
{
  assert_choice_type(types::lte_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<lte_ue_sidelink_aggr_max_bitrate_s>();
}
uint64_t& ue_context_setup_request_ies_o::value_c::pc5_link_ambr()
{
  assert_choice_type(types::pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
sl_drbs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::sl_drbs_to_be_setup_list()
{
  assert_choice_type(types::sl_drbs_to_be_setup_list, type_, "Value");
  return c.get<sl_drbs_to_be_setup_list_l>();
}
conditional_inter_du_mob_info_s& ue_context_setup_request_ies_o::value_c::conditional_inter_du_mob_info()
{
  assert_choice_type(types::conditional_inter_du_mob_info, type_, "Value");
  return c.get<conditional_inter_du_mob_info_s>();
}
mdt_plmn_list_l& ue_context_setup_request_ies_o::value_c::management_based_mdt_plmn_list()
{
  assert_choice_type(types::management_based_mdt_plmn_list, type_, "Value");
  return c.get<mdt_plmn_list_l>();
}
fixed_bitstring<44, false, true>& ue_context_setup_request_ies_o::value_c::serving_n_id()
{
  assert_choice_type(types::serving_n_id, type_, "Value");
  return c.get<fixed_bitstring<44, false, true>>();
}
f1_c_transfer_path_s& ue_context_setup_request_ies_o::value_c::f1_c_transfer_path()
{
  assert_choice_type(types::f1_c_transfer_path, type_, "Value");
  return c.get<f1_c_transfer_path_s>();
}
f1_c_transfer_path_nr_dc_s& ue_context_setup_request_ies_o::value_c::f1_c_transfer_path_nr_dc()
{
  assert_choice_type(types::f1_c_transfer_path_nr_dc, type_, "Value");
  return c.get<f1_c_transfer_path_nr_dc_s>();
}
mdt_polluted_meas_ind_e& ue_context_setup_request_ies_o::value_c::mdt_polluted_meas_ind()
{
  assert_choice_type(types::mdt_polluted_meas_ind, type_, "Value");
  return c.get<mdt_polluted_meas_ind_e>();
}
scg_activation_request_e& ue_context_setup_request_ies_o::value_c::scg_activation_request()
{
  assert_choice_type(types::scg_activation_request, type_, "Value");
  return c.get<scg_activation_request_e>();
}
cg_sdt_session_info_s& ue_context_setup_request_ies_o::value_c::cg_sdt_session_info_old()
{
  assert_choice_type(types::cg_sdt_session_info_old, type_, "Value");
  return c.get<cg_sdt_session_info_s>();
}
five_g_pro_se_authorized_s& ue_context_setup_request_ies_o::value_c::five_g_pro_se_authorized()
{
  assert_choice_type(types::five_g_pro_se_authorized, type_, "Value");
  return c.get<five_g_pro_se_authorized_s>();
}
nr_ue_sidelink_aggr_max_bitrate_s& ue_context_setup_request_ies_o::value_c::five_g_pro_se_ue_pc5_aggr_max_bitrate()
{
  assert_choice_type(types::five_g_pro_se_ue_pc5_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
uint64_t& ue_context_setup_request_ies_o::value_c::five_g_pro_se_pc5_link_ambr()
{
  assert_choice_type(types::five_g_pro_se_pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
uu_rlc_ch_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::uu_rlc_ch_to_be_setup_list()
{
  assert_choice_type(types::uu_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_setup_list_l>();
}
pc5_rlc_ch_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::pc5_rlc_ch_to_be_setup_list()
{
  assert_choice_type(types::pc5_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_setup_list_l>();
}
path_switch_cfg_s& ue_context_setup_request_ies_o::value_c::path_switch_cfg()
{
  assert_choice_type(types::path_switch_cfg, type_, "Value");
  return c.get<path_switch_cfg_s>();
}
gnb_du_ue_slice_max_bit_rate_list_l& ue_context_setup_request_ies_o::value_c::gnb_du_ue_slice_max_bit_rate_list()
{
  assert_choice_type(types::gnb_du_ue_slice_max_bit_rate_list, type_, "Value");
  return c.get<gnb_du_ue_slice_max_bit_rate_list_l>();
}
multicast_mbs_session_list_l& ue_context_setup_request_ies_o::value_c::multicast_mbs_session_setup_list()
{
  assert_choice_type(types::multicast_mbs_session_setup_list, type_, "Value");
  return c.get<multicast_mbs_session_list_l>();
}
ue_multicast_m_rbs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::ue_multicast_m_rbs_to_be_setup_list()
{
  assert_choice_type(types::ue_multicast_m_rbs_to_be_setup_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_to_be_setup_list_l>();
}
serving_cell_mo_list_l& ue_context_setup_request_ies_o::value_c::serving_cell_mo_list()
{
  assert_choice_type(types::serving_cell_mo_list, type_, "Value");
  return c.get<serving_cell_mo_list_l>();
}
const uint64_t& ue_context_setup_request_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_setup_request_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const nr_cgi_s& ue_context_setup_request_ies_o::value_c::sp_cell_id() const
{
  assert_choice_type(types::sp_cell_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
const uint8_t& ue_context_setup_request_ies_o::value_c::serv_cell_idx() const
{
  assert_choice_type(types::serv_cell_idx, type_, "Value");
  return c.get<uint8_t>();
}
const cell_ul_cfg_e& ue_context_setup_request_ies_o::value_c::sp_cell_ul_cfg() const
{
  assert_choice_type(types::sp_cell_ul_cfg, type_, "Value");
  return c.get<cell_ul_cfg_e>();
}
const cu_to_du_rrc_info_s& ue_context_setup_request_ies_o::value_c::cu_to_du_rrc_info() const
{
  assert_choice_type(types::cu_to_du_rrc_info, type_, "Value");
  return c.get<cu_to_du_rrc_info_s>();
}
const candidate_sp_cell_list_l& ue_context_setup_request_ies_o::value_c::candidate_sp_cell_list() const
{
  assert_choice_type(types::candidate_sp_cell_list, type_, "Value");
  return c.get<candidate_sp_cell_list_l>();
}
const drx_cycle_s& ue_context_setup_request_ies_o::value_c::drx_cycle() const
{
  assert_choice_type(types::drx_cycle, type_, "Value");
  return c.get<drx_cycle_s>();
}
const unbounded_octstring<true>& ue_context_setup_request_ies_o::value_c::res_coordination_transfer_container() const
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const scell_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::scell_to_be_setup_list() const
{
  assert_choice_type(types::scell_to_be_setup_list, type_, "Value");
  return c.get<scell_to_be_setup_list_l>();
}
const srbs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::srbs_to_be_setup_list() const
{
  assert_choice_type(types::srbs_to_be_setup_list, type_, "Value");
  return c.get<srbs_to_be_setup_list_l>();
}
const drbs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::drbs_to_be_setup_list() const
{
  assert_choice_type(types::drbs_to_be_setup_list, type_, "Value");
  return c.get<drbs_to_be_setup_list_l>();
}
const inactivity_monitoring_request_e& ue_context_setup_request_ies_o::value_c::inactivity_monitoring_request() const
{
  assert_choice_type(types::inactivity_monitoring_request, type_, "Value");
  return c.get<inactivity_monitoring_request_e>();
}
const rat_freq_prio_info_c& ue_context_setup_request_ies_o::value_c::rat_freq_prio_info() const
{
  assert_choice_type(types::rat_freq_prio_info, type_, "Value");
  return c.get<rat_freq_prio_info_c>();
}
const unbounded_octstring<true>& ue_context_setup_request_ies_o::value_c::rrc_container() const
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const fixed_bitstring<64, false, true>& ue_context_setup_request_ies_o::value_c::masked_imeisv() const
{
  assert_choice_type(types::masked_imeisv, type_, "Value");
  return c.get<fixed_bitstring<64, false, true>>();
}
const fixed_octstring<3, true>& ue_context_setup_request_ies_o::value_c::serving_plmn() const
{
  assert_choice_type(types::serving_plmn, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
const uint64_t& ue_context_setup_request_ies_o::value_c::gnb_du_ue_ambr_ul() const
{
  assert_choice_type(types::gnb_du_ue_ambr_ul, type_, "Value");
  return c.get<uint64_t>();
}
const rrc_delivery_status_request_e& ue_context_setup_request_ies_o::value_c::rrc_delivery_status_request() const
{
  assert_choice_type(types::rrc_delivery_status_request, type_, "Value");
  return c.get<rrc_delivery_status_request_e>();
}
const res_coordination_transfer_info_s& ue_context_setup_request_ies_o::value_c::res_coordination_transfer_info() const
{
  assert_choice_type(types::res_coordination_transfer_info, type_, "Value");
  return c.get<res_coordination_transfer_info_s>();
}
const uint8_t& ue_context_setup_request_ies_o::value_c::serving_cell_mo() const
{
  assert_choice_type(types::serving_cell_mo, type_, "Value");
  return c.get<uint8_t>();
}
const uint64_t& ue_context_setup_request_ies_o::value_c::new_gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::new_gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const fixed_octstring<8, true>& ue_context_setup_request_ies_o::value_c::ran_ue_id() const
{
  assert_choice_type(types::ran_ue_id, type_, "Value");
  return c.get<fixed_octstring<8, true>>();
}
const trace_activation_s& ue_context_setup_request_ies_o::value_c::trace_activation() const
{
  assert_choice_type(types::trace_activation, type_, "Value");
  return c.get<trace_activation_s>();
}
const fixed_bitstring<32, false, true>& ue_context_setup_request_ies_o::value_c::add_rrm_prio_idx() const
{
  assert_choice_type(types::add_rrm_prio_idx, type_, "Value");
  return c.get<fixed_bitstring<32, false, true>>();
}
const bh_chs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::bh_chs_to_be_setup_list() const
{
  assert_choice_type(types::bh_chs_to_be_setup_list, type_, "Value");
  return c.get<bh_chs_to_be_setup_list_l>();
}
const fixed_bitstring<10, false, true>& ue_context_setup_request_ies_o::value_c::cfg_bap_address() const
{
  assert_choice_type(types::cfg_bap_address, type_, "Value");
  return c.get<fixed_bitstring<10, false, true>>();
}
const nr_v2x_services_authorized_s& ue_context_setup_request_ies_o::value_c::nr_v2x_services_authorized() const
{
  assert_choice_type(types::nr_v2x_services_authorized, type_, "Value");
  return c.get<nr_v2x_services_authorized_s>();
}
const ltev2x_services_authorized_s& ue_context_setup_request_ies_o::value_c::ltev2x_services_authorized() const
{
  assert_choice_type(types::ltev2x_services_authorized, type_, "Value");
  return c.get<ltev2x_services_authorized_s>();
}
const nr_ue_sidelink_aggr_max_bitrate_s&
ue_context_setup_request_ies_o::value_c::nr_ue_sidelink_aggr_max_bitrate() const
{
  assert_choice_type(types::nr_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
const lte_ue_sidelink_aggr_max_bitrate_s&
ue_context_setup_request_ies_o::value_c::lte_ue_sidelink_aggr_max_bitrate() const
{
  assert_choice_type(types::lte_ue_sidelink_aggr_max_bitrate, type_, "Value");
  return c.get<lte_ue_sidelink_aggr_max_bitrate_s>();
}
const uint64_t& ue_context_setup_request_ies_o::value_c::pc5_link_ambr() const
{
  assert_choice_type(types::pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
const sl_drbs_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::sl_drbs_to_be_setup_list() const
{
  assert_choice_type(types::sl_drbs_to_be_setup_list, type_, "Value");
  return c.get<sl_drbs_to_be_setup_list_l>();
}
const conditional_inter_du_mob_info_s& ue_context_setup_request_ies_o::value_c::conditional_inter_du_mob_info() const
{
  assert_choice_type(types::conditional_inter_du_mob_info, type_, "Value");
  return c.get<conditional_inter_du_mob_info_s>();
}
const mdt_plmn_list_l& ue_context_setup_request_ies_o::value_c::management_based_mdt_plmn_list() const
{
  assert_choice_type(types::management_based_mdt_plmn_list, type_, "Value");
  return c.get<mdt_plmn_list_l>();
}
const fixed_bitstring<44, false, true>& ue_context_setup_request_ies_o::value_c::serving_n_id() const
{
  assert_choice_type(types::serving_n_id, type_, "Value");
  return c.get<fixed_bitstring<44, false, true>>();
}
const f1_c_transfer_path_s& ue_context_setup_request_ies_o::value_c::f1_c_transfer_path() const
{
  assert_choice_type(types::f1_c_transfer_path, type_, "Value");
  return c.get<f1_c_transfer_path_s>();
}
const f1_c_transfer_path_nr_dc_s& ue_context_setup_request_ies_o::value_c::f1_c_transfer_path_nr_dc() const
{
  assert_choice_type(types::f1_c_transfer_path_nr_dc, type_, "Value");
  return c.get<f1_c_transfer_path_nr_dc_s>();
}
const mdt_polluted_meas_ind_e& ue_context_setup_request_ies_o::value_c::mdt_polluted_meas_ind() const
{
  assert_choice_type(types::mdt_polluted_meas_ind, type_, "Value");
  return c.get<mdt_polluted_meas_ind_e>();
}
const scg_activation_request_e& ue_context_setup_request_ies_o::value_c::scg_activation_request() const
{
  assert_choice_type(types::scg_activation_request, type_, "Value");
  return c.get<scg_activation_request_e>();
}
const cg_sdt_session_info_s& ue_context_setup_request_ies_o::value_c::cg_sdt_session_info_old() const
{
  assert_choice_type(types::cg_sdt_session_info_old, type_, "Value");
  return c.get<cg_sdt_session_info_s>();
}
const five_g_pro_se_authorized_s& ue_context_setup_request_ies_o::value_c::five_g_pro_se_authorized() const
{
  assert_choice_type(types::five_g_pro_se_authorized, type_, "Value");
  return c.get<five_g_pro_se_authorized_s>();
}
const nr_ue_sidelink_aggr_max_bitrate_s&
ue_context_setup_request_ies_o::value_c::five_g_pro_se_ue_pc5_aggr_max_bitrate() const
{
  assert_choice_type(types::five_g_pro_se_ue_pc5_aggr_max_bitrate, type_, "Value");
  return c.get<nr_ue_sidelink_aggr_max_bitrate_s>();
}
const uint64_t& ue_context_setup_request_ies_o::value_c::five_g_pro_se_pc5_link_ambr() const
{
  assert_choice_type(types::five_g_pro_se_pc5_link_ambr, type_, "Value");
  return c.get<uint64_t>();
}
const uu_rlc_ch_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::uu_rlc_ch_to_be_setup_list() const
{
  assert_choice_type(types::uu_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_to_be_setup_list_l>();
}
const pc5_rlc_ch_to_be_setup_list_l& ue_context_setup_request_ies_o::value_c::pc5_rlc_ch_to_be_setup_list() const
{
  assert_choice_type(types::pc5_rlc_ch_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_to_be_setup_list_l>();
}
const path_switch_cfg_s& ue_context_setup_request_ies_o::value_c::path_switch_cfg() const
{
  assert_choice_type(types::path_switch_cfg, type_, "Value");
  return c.get<path_switch_cfg_s>();
}
const gnb_du_ue_slice_max_bit_rate_list_l&
ue_context_setup_request_ies_o::value_c::gnb_du_ue_slice_max_bit_rate_list() const
{
  assert_choice_type(types::gnb_du_ue_slice_max_bit_rate_list, type_, "Value");
  return c.get<gnb_du_ue_slice_max_bit_rate_list_l>();
}
const multicast_mbs_session_list_l& ue_context_setup_request_ies_o::value_c::multicast_mbs_session_setup_list() const
{
  assert_choice_type(types::multicast_mbs_session_setup_list, type_, "Value");
  return c.get<multicast_mbs_session_list_l>();
}
const ue_multicast_m_rbs_to_be_setup_list_l&
ue_context_setup_request_ies_o::value_c::ue_multicast_m_rbs_to_be_setup_list() const
{
  assert_choice_type(types::ue_multicast_m_rbs_to_be_setup_list, type_, "Value");
  return c.get<ue_multicast_m_rbs_to_be_setup_list_l>();
}
const serving_cell_mo_list_l& ue_context_setup_request_ies_o::value_c::serving_cell_mo_list() const
{
  assert_choice_type(types::serving_cell_mo_list, type_, "Value");
  return c.get<serving_cell_mo_list_l>();
}
void ue_context_setup_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::sp_cell_id:
      j.write_fieldname("NRCGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    case types::serv_cell_idx:
      j.write_int("INTEGER (0..31,...)", c.get<uint8_t>());
      break;
    case types::sp_cell_ul_cfg:
      j.write_str("CellULConfigured", c.get<cell_ul_cfg_e>().to_string());
      break;
    case types::cu_to_du_rrc_info:
      j.write_fieldname("CUtoDURRCInformation");
      c.get<cu_to_du_rrc_info_s>().to_json(j);
      break;
    case types::candidate_sp_cell_list:
      j.start_array("Candidate-SpCell-List");
      for (const auto& e1 : c.get<candidate_sp_cell_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drx_cycle:
      j.write_fieldname("DRXCycle");
      c.get<drx_cycle_s>().to_json(j);
      break;
    case types::res_coordination_transfer_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::scell_to_be_setup_list:
      j.start_array("SCell-ToBeSetup-List");
      for (const auto& e1 : c.get<scell_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_to_be_setup_list:
      j.start_array("SRBs-ToBeSetup-List");
      for (const auto& e1 : c.get<srbs_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_to_be_setup_list:
      j.start_array("DRBs-ToBeSetup-List");
      for (const auto& e1 : c.get<drbs_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::inactivity_monitoring_request:
      j.write_str("InactivityMonitoringRequest", "true");
      break;
    case types::rat_freq_prio_info:
      j.write_fieldname("RAT-FrequencyPriorityInformation");
      c.get<rat_freq_prio_info_c>().to_json(j);
      break;
    case types::rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::masked_imeisv:
      j.write_str("BIT STRING", c.get<fixed_bitstring<64, false, true>>().to_string());
      break;
    case types::serving_plmn:
      j.write_str("OCTET STRING", c.get<fixed_octstring<3, true>>().to_string());
      break;
    case types::gnb_du_ue_ambr_ul:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::rrc_delivery_status_request:
      j.write_str("RRCDeliveryStatusRequest", "true");
      break;
    case types::res_coordination_transfer_info:
      j.write_fieldname("ResourceCoordinationTransferInformation");
      c.get<res_coordination_transfer_info_s>().to_json(j);
      break;
    case types::serving_cell_mo:
      j.write_int("INTEGER (1..64,...)", c.get<uint8_t>());
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::ran_ue_id:
      j.write_str("OCTET STRING", c.get<fixed_octstring<8, true>>().to_string());
      break;
    case types::trace_activation:
      j.write_fieldname("TraceActivation");
      c.get<trace_activation_s>().to_json(j);
      break;
    case types::add_rrm_prio_idx:
      j.write_str("BIT STRING", c.get<fixed_bitstring<32, false, true>>().to_string());
      break;
    case types::bh_chs_to_be_setup_list:
      j.start_array("BHChannels-ToBeSetup-List");
      for (const auto& e1 : c.get<bh_chs_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::cfg_bap_address:
      j.write_str("BIT STRING", c.get<fixed_bitstring<10, false, true>>().to_string());
      break;
    case types::nr_v2x_services_authorized:
      j.write_fieldname("NRV2XServicesAuthorized");
      c.get<nr_v2x_services_authorized_s>().to_json(j);
      break;
    case types::ltev2x_services_authorized:
      j.write_fieldname("LTEV2XServicesAuthorized");
      c.get<ltev2x_services_authorized_s>().to_json(j);
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      j.write_fieldname("NRUESidelinkAggregateMaximumBitrate");
      c.get<nr_ue_sidelink_aggr_max_bitrate_s>().to_json(j);
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      j.write_fieldname("LTEUESidelinkAggregateMaximumBitrate");
      c.get<lte_ue_sidelink_aggr_max_bitrate_s>().to_json(j);
      break;
    case types::pc5_link_ambr:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::sl_drbs_to_be_setup_list:
      j.start_array("SLDRBs-ToBeSetup-List");
      for (const auto& e1 : c.get<sl_drbs_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::conditional_inter_du_mob_info:
      j.write_fieldname("ConditionalInterDUMobilityInformation");
      c.get<conditional_inter_du_mob_info_s>().to_json(j);
      break;
    case types::management_based_mdt_plmn_list:
      j.start_array("MDTPLMNList");
      for (const auto& e1 : c.get<mdt_plmn_list_l>()) {
        j.write_str(e1.to_string());
      }
      j.end_array();
      break;
    case types::serving_n_id:
      j.write_str("BIT STRING", c.get<fixed_bitstring<44, false, true>>().to_string());
      break;
    case types::f1_c_transfer_path:
      j.write_fieldname("F1CTransferPath");
      c.get<f1_c_transfer_path_s>().to_json(j);
      break;
    case types::f1_c_transfer_path_nr_dc:
      j.write_fieldname("F1CTransferPathNRDC");
      c.get<f1_c_transfer_path_nr_dc_s>().to_json(j);
      break;
    case types::mdt_polluted_meas_ind:
      j.write_str("MDTPollutedMeasurementIndicator", c.get<mdt_polluted_meas_ind_e>().to_string());
      break;
    case types::scg_activation_request:
      j.write_str("SCGActivationRequest", c.get<scg_activation_request_e>().to_string());
      break;
    case types::cg_sdt_session_info_old:
      j.write_fieldname("CG-SDTSessionInfo");
      c.get<cg_sdt_session_info_s>().to_json(j);
      break;
    case types::five_g_pro_se_authorized:
      j.write_fieldname("FiveG-ProSeAuthorized");
      c.get<five_g_pro_se_authorized_s>().to_json(j);
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      j.write_fieldname("NRUESidelinkAggregateMaximumBitrate");
      c.get<nr_ue_sidelink_aggr_max_bitrate_s>().to_json(j);
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      j.write_int("INTEGER (0..4000000000000,...)", c.get<uint64_t>());
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      j.start_array("UuRLCChannelToBeSetupList");
      for (const auto& e1 : c.get<uu_rlc_ch_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      j.start_array("PC5RLCChannelToBeSetupList");
      for (const auto& e1 : c.get<pc5_rlc_ch_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::path_switch_cfg:
      j.write_fieldname("PathSwitchConfiguration");
      c.get<path_switch_cfg_s>().to_json(j);
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      j.start_array("GNBDUUESliceMaximumBitRateList");
      for (const auto& e1 : c.get<gnb_du_ue_slice_max_bit_rate_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::multicast_mbs_session_setup_list:
      j.start_array("MulticastMBSSessionList");
      for (const auto& e1 : c.get<multicast_mbs_session_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      j.start_array("UE-MulticastMRBs-ToBeSetup-List");
      for (const auto& e1 : c.get<ue_multicast_m_rbs_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::serving_cell_mo_list:
      j.start_array("ServingCellMO-List");
      for (const auto& e1 : c.get<serving_cell_mo_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_setup_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sp_cell_id:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    case types::serv_cell_idx:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u, true, true));
      break;
    case types::sp_cell_ul_cfg:
      HANDLE_CODE(c.get<cell_ul_cfg_e>().pack(bref));
      break;
    case types::cu_to_du_rrc_info:
      HANDLE_CODE(c.get<cu_to_du_rrc_info_s>().pack(bref));
      break;
    case types::candidate_sp_cell_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<candidate_sp_cell_list_l>(), 1, 64, true));
      break;
    case types::drx_cycle:
      HANDLE_CODE(c.get<drx_cycle_s>().pack(bref));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::scell_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scell_to_be_setup_list_l>(), 1, 32, true));
      break;
    case types::srbs_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_to_be_setup_list_l>(), 1, 8, true));
      break;
    case types::drbs_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_to_be_setup_list_l>(), 1, 64, true));
      break;
    case types::inactivity_monitoring_request:
      HANDLE_CODE(c.get<inactivity_monitoring_request_e>().pack(bref));
      break;
    case types::rat_freq_prio_info:
      HANDLE_CODE(c.get<rat_freq_prio_info_c>().pack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::masked_imeisv:
      HANDLE_CODE((c.get<fixed_bitstring<64, false, true>>().pack(bref)));
      break;
    case types::serving_plmn:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().pack(bref)));
      break;
    case types::gnb_du_ue_ambr_ul:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().pack(bref));
      break;
    case types::res_coordination_transfer_info:
      HANDLE_CODE(c.get<res_coordination_transfer_info_s>().pack(bref));
      break;
    case types::serving_cell_mo:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)64u, true, true));
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().pack(bref)));
      break;
    case types::trace_activation:
      HANDLE_CODE(c.get<trace_activation_s>().pack(bref));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().pack(bref)));
      break;
    case types::bh_chs_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_to_be_setup_list_l>(), 1, 65536, true));
      break;
    case types::cfg_bap_address:
      HANDLE_CODE((c.get<fixed_bitstring<10, false, true>>().pack(bref)));
      break;
    case types::nr_v2x_services_authorized:
      HANDLE_CODE(c.get<nr_v2x_services_authorized_s>().pack(bref));
      break;
    case types::ltev2x_services_authorized:
      HANDLE_CODE(c.get<ltev2x_services_authorized_s>().pack(bref));
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().pack(bref));
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<lte_ue_sidelink_aggr_max_bitrate_s>().pack(bref));
      break;
    case types::pc5_link_ambr:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::sl_drbs_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_to_be_setup_list_l>(), 1, 512, true));
      break;
    case types::conditional_inter_du_mob_info:
      HANDLE_CODE(c.get<conditional_inter_du_mob_info_s>().pack(bref));
      break;
    case types::management_based_mdt_plmn_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<mdt_plmn_list_l>(), 1, 16, true));
      break;
    case types::serving_n_id:
      HANDLE_CODE((c.get<fixed_bitstring<44, false, true>>().pack(bref)));
      break;
    case types::f1_c_transfer_path:
      HANDLE_CODE(c.get<f1_c_transfer_path_s>().pack(bref));
      break;
    case types::f1_c_transfer_path_nr_dc:
      HANDLE_CODE(c.get<f1_c_transfer_path_nr_dc_s>().pack(bref));
      break;
    case types::mdt_polluted_meas_ind:
      HANDLE_CODE(c.get<mdt_polluted_meas_ind_e>().pack(bref));
      break;
    case types::scg_activation_request:
      HANDLE_CODE(c.get<scg_activation_request_e>().pack(bref));
      break;
    case types::cg_sdt_session_info_old:
      HANDLE_CODE(c.get<cg_sdt_session_info_s>().pack(bref));
      break;
    case types::five_g_pro_se_authorized:
      HANDLE_CODE(c.get<five_g_pro_se_authorized_s>().pack(bref));
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().pack(bref));
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_to_be_setup_list_l>(), 1, 32, true));
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_to_be_setup_list_l>(), 1, 512, true));
      break;
    case types::path_switch_cfg:
      HANDLE_CODE(c.get<path_switch_cfg_s>().pack(bref));
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<gnb_du_ue_slice_max_bit_rate_list_l>(), 1, 8, true));
      break;
    case types::multicast_mbs_session_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<multicast_mbs_session_list_l>(), 1, 256, true));
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ue_multicast_m_rbs_to_be_setup_list_l>(), 1, 64, true));
      break;
    case types::serving_cell_mo_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<serving_cell_mo_list_l>(), 1, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_setup_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::sp_cell_id:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    case types::serv_cell_idx:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u, true, true));
      break;
    case types::sp_cell_ul_cfg:
      HANDLE_CODE(c.get<cell_ul_cfg_e>().unpack(bref));
      break;
    case types::cu_to_du_rrc_info:
      HANDLE_CODE(c.get<cu_to_du_rrc_info_s>().unpack(bref));
      break;
    case types::candidate_sp_cell_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<candidate_sp_cell_list_l>(), bref, 1, 64, true));
      break;
    case types::drx_cycle:
      HANDLE_CODE(c.get<drx_cycle_s>().unpack(bref));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::scell_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scell_to_be_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::srbs_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_to_be_setup_list_l>(), bref, 1, 8, true));
      break;
    case types::drbs_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_to_be_setup_list_l>(), bref, 1, 64, true));
      break;
    case types::inactivity_monitoring_request:
      HANDLE_CODE(c.get<inactivity_monitoring_request_e>().unpack(bref));
      break;
    case types::rat_freq_prio_info:
      HANDLE_CODE(c.get<rat_freq_prio_info_c>().unpack(bref));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::masked_imeisv:
      HANDLE_CODE((c.get<fixed_bitstring<64, false, true>>().unpack(bref)));
      break;
    case types::serving_plmn:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().unpack(bref)));
      break;
    case types::gnb_du_ue_ambr_ul:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::rrc_delivery_status_request:
      HANDLE_CODE(c.get<rrc_delivery_status_request_e>().unpack(bref));
      break;
    case types::res_coordination_transfer_info:
      HANDLE_CODE(c.get<res_coordination_transfer_info_s>().unpack(bref));
      break;
    case types::serving_cell_mo:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)64u, true, true));
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::ran_ue_id:
      HANDLE_CODE((c.get<fixed_octstring<8, true>>().unpack(bref)));
      break;
    case types::trace_activation:
      HANDLE_CODE(c.get<trace_activation_s>().unpack(bref));
      break;
    case types::add_rrm_prio_idx:
      HANDLE_CODE((c.get<fixed_bitstring<32, false, true>>().unpack(bref)));
      break;
    case types::bh_chs_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_to_be_setup_list_l>(), bref, 1, 65536, true));
      break;
    case types::cfg_bap_address:
      HANDLE_CODE((c.get<fixed_bitstring<10, false, true>>().unpack(bref)));
      break;
    case types::nr_v2x_services_authorized:
      HANDLE_CODE(c.get<nr_v2x_services_authorized_s>().unpack(bref));
      break;
    case types::ltev2x_services_authorized:
      HANDLE_CODE(c.get<ltev2x_services_authorized_s>().unpack(bref));
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().unpack(bref));
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      HANDLE_CODE(c.get<lte_ue_sidelink_aggr_max_bitrate_s>().unpack(bref));
      break;
    case types::pc5_link_ambr:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::sl_drbs_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_to_be_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::conditional_inter_du_mob_info:
      HANDLE_CODE(c.get<conditional_inter_du_mob_info_s>().unpack(bref));
      break;
    case types::management_based_mdt_plmn_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<mdt_plmn_list_l>(), bref, 1, 16, true));
      break;
    case types::serving_n_id:
      HANDLE_CODE((c.get<fixed_bitstring<44, false, true>>().unpack(bref)));
      break;
    case types::f1_c_transfer_path:
      HANDLE_CODE(c.get<f1_c_transfer_path_s>().unpack(bref));
      break;
    case types::f1_c_transfer_path_nr_dc:
      HANDLE_CODE(c.get<f1_c_transfer_path_nr_dc_s>().unpack(bref));
      break;
    case types::mdt_polluted_meas_ind:
      HANDLE_CODE(c.get<mdt_polluted_meas_ind_e>().unpack(bref));
      break;
    case types::scg_activation_request:
      HANDLE_CODE(c.get<scg_activation_request_e>().unpack(bref));
      break;
    case types::cg_sdt_session_info_old:
      HANDLE_CODE(c.get<cg_sdt_session_info_s>().unpack(bref));
      break;
    case types::five_g_pro_se_authorized:
      HANDLE_CODE(c.get<five_g_pro_se_authorized_s>().unpack(bref));
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      HANDLE_CODE(c.get<nr_ue_sidelink_aggr_max_bitrate_s>().unpack(bref));
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_to_be_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_to_be_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::path_switch_cfg:
      HANDLE_CODE(c.get<path_switch_cfg_s>().unpack(bref));
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<gnb_du_ue_slice_max_bit_rate_list_l>(), bref, 1, 8, true));
      break;
    case types::multicast_mbs_session_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<multicast_mbs_session_list_l>(), bref, 1, 256, true));
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ue_multicast_m_rbs_to_be_setup_list_l>(), bref, 1, 64, true));
      break;
    case types::serving_cell_mo_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<serving_cell_mo_list_l>(), bref, 1, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEContextSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_context_setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40,  41,  39,  95,  49,  94,  27,  66,  15,  83,  98,  7,
                                   202, 260, 279, 324, 316, 376, 548, 602, 603, 611, 612, 697};
  return map_enum_number(names, 24, idx, "id");
}
bool ue_context_setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40,  41,  39,  95,  49,  94,  27,  66,  15,  83,  98,  7,
                                   202, 260, 279, 324, 316, 376, 548, 602, 603, 611, 612, 697};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_context_setup_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 39:
      return crit_e::reject;
    case 95:
      return crit_e::ignore;
    case 49:
      return crit_e::ignore;
    case 94:
      return crit_e::reject;
    case 27:
      return crit_e::ignore;
    case 66:
      return crit_e::ignore;
    case 15:
      return crit_e::ignore;
    case 83:
      return crit_e::ignore;
    case 98:
      return crit_e::reject;
    case 7:
      return crit_e::ignore;
    case 202:
      return crit_e::ignore;
    case 260:
      return crit_e::ignore;
    case 279:
      return crit_e::ignore;
    case 324:
      return crit_e::ignore;
    case 316:
      return crit_e::ignore;
    case 376:
      return crit_e::reject;
    case 548:
      return crit_e::ignore;
    case 602:
      return crit_e::ignore;
    case 603:
      return crit_e::ignore;
    case 611:
      return crit_e::ignore;
    case 612:
      return crit_e::ignore;
    case 697:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_context_setup_resp_ies_o::value_c ue_context_setup_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 39:
      ret.set(value_c::types::du_to_cu_rrc_info);
      break;
    case 95:
      ret.set(value_c::types::c_rnti);
      break;
    case 49:
      ret.set(value_c::types::res_coordination_transfer_container);
      break;
    case 94:
      ret.set(value_c::types::full_cfg);
      break;
    case 27:
      ret.set(value_c::types::drbs_setup_list);
      break;
    case 66:
      ret.set(value_c::types::srbs_failed_to_be_setup_list);
      break;
    case 15:
      ret.set(value_c::types::drbs_failed_to_be_setup_list);
      break;
    case 83:
      ret.set(value_c::types::scell_failedto_setup_list);
      break;
    case 98:
      ret.set(value_c::types::inactivity_monitoring_resp);
      break;
    case 7:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 202:
      ret.set(value_c::types::srbs_setup_list);
      break;
    case 260:
      ret.set(value_c::types::bh_chs_setup_list);
      break;
    case 279:
      ret.set(value_c::types::bh_chs_failed_to_be_setup_list);
      break;
    case 324:
      ret.set(value_c::types::sl_drbs_setup_list);
      break;
    case 316:
      ret.set(value_c::types::sl_drbs_failed_to_be_setup_list);
      break;
    case 376:
      ret.set(value_c::types::requested_target_cell_global_id);
      break;
    case 548:
      ret.set(value_c::types::scg_activation_status);
      break;
    case 602:
      ret.set(value_c::types::uu_rlc_ch_setup_list);
      break;
    case 603:
      ret.set(value_c::types::uu_rlc_ch_failed_to_be_setup_list);
      break;
    case 611:
      ret.set(value_c::types::pc5_rlc_ch_setup_list);
      break;
    case 612:
      ret.set(value_c::types::pc5_rlc_ch_failed_to_be_setup_list);
      break;
    case 697:
      ret.set(value_c::types::serving_cell_mo_encoded_in_cgc_list);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_context_setup_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 39:
      return presence_e::mandatory;
    case 95:
      return presence_e::optional;
    case 49:
      return presence_e::optional;
    case 94:
      return presence_e::optional;
    case 27:
      return presence_e::optional;
    case 66:
      return presence_e::optional;
    case 15:
      return presence_e::optional;
    case 83:
      return presence_e::optional;
    case 98:
      return presence_e::optional;
    case 7:
      return presence_e::optional;
    case 202:
      return presence_e::optional;
    case 260:
      return presence_e::optional;
    case 279:
      return presence_e::optional;
    case 324:
      return presence_e::optional;
    case 316:
      return presence_e::optional;
    case 376:
      return presence_e::optional;
    case 548:
      return presence_e::optional;
    case 602:
      return presence_e::optional;
    case 603:
      return presence_e::optional;
    case 611:
      return presence_e::optional;
    case 612:
      return presence_e::optional;
    case 697:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_context_setup_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::du_to_cu_rrc_info:
      c.destroy<du_to_cu_rrc_info_s>();
      break;
    case types::res_coordination_transfer_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::drbs_setup_list:
      c.destroy<drbs_setup_list_l>();
      break;
    case types::srbs_failed_to_be_setup_list:
      c.destroy<srbs_failed_to_be_setup_list_l>();
      break;
    case types::drbs_failed_to_be_setup_list:
      c.destroy<drbs_failed_to_be_setup_list_l>();
      break;
    case types::scell_failedto_setup_list:
      c.destroy<scell_failedto_setup_list_l>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    case types::srbs_setup_list:
      c.destroy<srbs_setup_list_l>();
      break;
    case types::bh_chs_setup_list:
      c.destroy<bh_chs_setup_list_l>();
      break;
    case types::bh_chs_failed_to_be_setup_list:
      c.destroy<bh_chs_failed_to_be_setup_list_l>();
      break;
    case types::sl_drbs_setup_list:
      c.destroy<sl_drbs_setup_list_l>();
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      c.destroy<sl_drbs_failed_to_be_setup_list_l>();
      break;
    case types::requested_target_cell_global_id:
      c.destroy<nr_cgi_s>();
      break;
    case types::uu_rlc_ch_setup_list:
      c.destroy<uu_rlc_ch_setup_list_l>();
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.destroy<uu_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_setup_list:
      c.destroy<pc5_rlc_ch_setup_list_l>();
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.destroy<pc5_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.destroy<serving_cell_mo_encoded_in_cgc_list_l>();
      break;
    default:
      break;
  }
}
void ue_context_setup_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::du_to_cu_rrc_info:
      c.init<du_to_cu_rrc_info_s>();
      break;
    case types::c_rnti:
      break;
    case types::res_coordination_transfer_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::full_cfg:
      break;
    case types::drbs_setup_list:
      c.init<drbs_setup_list_l>();
      break;
    case types::srbs_failed_to_be_setup_list:
      c.init<srbs_failed_to_be_setup_list_l>();
      break;
    case types::drbs_failed_to_be_setup_list:
      c.init<drbs_failed_to_be_setup_list_l>();
      break;
    case types::scell_failedto_setup_list:
      c.init<scell_failedto_setup_list_l>();
      break;
    case types::inactivity_monitoring_resp:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::srbs_setup_list:
      c.init<srbs_setup_list_l>();
      break;
    case types::bh_chs_setup_list:
      c.init<bh_chs_setup_list_l>();
      break;
    case types::bh_chs_failed_to_be_setup_list:
      c.init<bh_chs_failed_to_be_setup_list_l>();
      break;
    case types::sl_drbs_setup_list:
      c.init<sl_drbs_setup_list_l>();
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      c.init<sl_drbs_failed_to_be_setup_list_l>();
      break;
    case types::requested_target_cell_global_id:
      c.init<nr_cgi_s>();
      break;
    case types::scg_activation_status:
      break;
    case types::uu_rlc_ch_setup_list:
      c.init<uu_rlc_ch_setup_list_l>();
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.init<uu_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::pc5_rlc_ch_setup_list:
      c.init<pc5_rlc_ch_setup_list_l>();
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.init<pc5_rlc_ch_failed_to_be_setup_list_l>();
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.init<serving_cell_mo_encoded_in_cgc_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_resp_ies_o::value_c");
  }
}
ue_context_setup_resp_ies_o::value_c::value_c(const ue_context_setup_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::du_to_cu_rrc_info:
      c.init(other.c.get<du_to_cu_rrc_info_s>());
      break;
    case types::c_rnti:
      c.init(other.c.get<uint32_t>());
      break;
    case types::res_coordination_transfer_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::full_cfg:
      c.init(other.c.get<full_cfg_e>());
      break;
    case types::drbs_setup_list:
      c.init(other.c.get<drbs_setup_list_l>());
      break;
    case types::srbs_failed_to_be_setup_list:
      c.init(other.c.get<srbs_failed_to_be_setup_list_l>());
      break;
    case types::drbs_failed_to_be_setup_list:
      c.init(other.c.get<drbs_failed_to_be_setup_list_l>());
      break;
    case types::scell_failedto_setup_list:
      c.init(other.c.get<scell_failedto_setup_list_l>());
      break;
    case types::inactivity_monitoring_resp:
      c.init(other.c.get<inactivity_monitoring_resp_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::srbs_setup_list:
      c.init(other.c.get<srbs_setup_list_l>());
      break;
    case types::bh_chs_setup_list:
      c.init(other.c.get<bh_chs_setup_list_l>());
      break;
    case types::bh_chs_failed_to_be_setup_list:
      c.init(other.c.get<bh_chs_failed_to_be_setup_list_l>());
      break;
    case types::sl_drbs_setup_list:
      c.init(other.c.get<sl_drbs_setup_list_l>());
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      c.init(other.c.get<sl_drbs_failed_to_be_setup_list_l>());
      break;
    case types::requested_target_cell_global_id:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::scg_activation_status:
      c.init(other.c.get<scg_activation_status_e>());
      break;
    case types::uu_rlc_ch_setup_list:
      c.init(other.c.get<uu_rlc_ch_setup_list_l>());
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.init(other.c.get<uu_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_setup_list:
      c.init(other.c.get<pc5_rlc_ch_setup_list_l>());
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.init(other.c.get<pc5_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.init(other.c.get<serving_cell_mo_encoded_in_cgc_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_resp_ies_o::value_c");
  }
}
ue_context_setup_resp_ies_o::value_c&
ue_context_setup_resp_ies_o::value_c::operator=(const ue_context_setup_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::du_to_cu_rrc_info:
      c.set(other.c.get<du_to_cu_rrc_info_s>());
      break;
    case types::c_rnti:
      c.set(other.c.get<uint32_t>());
      break;
    case types::res_coordination_transfer_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::full_cfg:
      c.set(other.c.get<full_cfg_e>());
      break;
    case types::drbs_setup_list:
      c.set(other.c.get<drbs_setup_list_l>());
      break;
    case types::srbs_failed_to_be_setup_list:
      c.set(other.c.get<srbs_failed_to_be_setup_list_l>());
      break;
    case types::drbs_failed_to_be_setup_list:
      c.set(other.c.get<drbs_failed_to_be_setup_list_l>());
      break;
    case types::scell_failedto_setup_list:
      c.set(other.c.get<scell_failedto_setup_list_l>());
      break;
    case types::inactivity_monitoring_resp:
      c.set(other.c.get<inactivity_monitoring_resp_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::srbs_setup_list:
      c.set(other.c.get<srbs_setup_list_l>());
      break;
    case types::bh_chs_setup_list:
      c.set(other.c.get<bh_chs_setup_list_l>());
      break;
    case types::bh_chs_failed_to_be_setup_list:
      c.set(other.c.get<bh_chs_failed_to_be_setup_list_l>());
      break;
    case types::sl_drbs_setup_list:
      c.set(other.c.get<sl_drbs_setup_list_l>());
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      c.set(other.c.get<sl_drbs_failed_to_be_setup_list_l>());
      break;
    case types::requested_target_cell_global_id:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::scg_activation_status:
      c.set(other.c.get<scg_activation_status_e>());
      break;
    case types::uu_rlc_ch_setup_list:
      c.set(other.c.get<uu_rlc_ch_setup_list_l>());
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c.set(other.c.get<uu_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::pc5_rlc_ch_setup_list:
      c.set(other.c.get<pc5_rlc_ch_setup_list_l>());
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c.set(other.c.get<pc5_rlc_ch_failed_to_be_setup_list_l>());
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c.set(other.c.get<serving_cell_mo_encoded_in_cgc_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_resp_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_context_setup_resp_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_context_setup_resp_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
du_to_cu_rrc_info_s& ue_context_setup_resp_ies_o::value_c::du_to_cu_rrc_info()
{
  assert_choice_type(types::du_to_cu_rrc_info, type_, "Value");
  return c.get<du_to_cu_rrc_info_s>();
}
uint32_t& ue_context_setup_resp_ies_o::value_c::c_rnti()
{
  assert_choice_type(types::c_rnti, type_, "Value");
  return c.get<uint32_t>();
}
unbounded_octstring<true>& ue_context_setup_resp_ies_o::value_c::res_coordination_transfer_container()
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
full_cfg_e& ue_context_setup_resp_ies_o::value_c::full_cfg()
{
  assert_choice_type(types::full_cfg, type_, "Value");
  return c.get<full_cfg_e>();
}
drbs_setup_list_l& ue_context_setup_resp_ies_o::value_c::drbs_setup_list()
{
  assert_choice_type(types::drbs_setup_list, type_, "Value");
  return c.get<drbs_setup_list_l>();
}
srbs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::srbs_failed_to_be_setup_list()
{
  assert_choice_type(types::srbs_failed_to_be_setup_list, type_, "Value");
  return c.get<srbs_failed_to_be_setup_list_l>();
}
drbs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::drbs_failed_to_be_setup_list()
{
  assert_choice_type(types::drbs_failed_to_be_setup_list, type_, "Value");
  return c.get<drbs_failed_to_be_setup_list_l>();
}
scell_failedto_setup_list_l& ue_context_setup_resp_ies_o::value_c::scell_failedto_setup_list()
{
  assert_choice_type(types::scell_failedto_setup_list, type_, "Value");
  return c.get<scell_failedto_setup_list_l>();
}
inactivity_monitoring_resp_e& ue_context_setup_resp_ies_o::value_c::inactivity_monitoring_resp()
{
  assert_choice_type(types::inactivity_monitoring_resp, type_, "Value");
  return c.get<inactivity_monitoring_resp_e>();
}
crit_diagnostics_s& ue_context_setup_resp_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
srbs_setup_list_l& ue_context_setup_resp_ies_o::value_c::srbs_setup_list()
{
  assert_choice_type(types::srbs_setup_list, type_, "Value");
  return c.get<srbs_setup_list_l>();
}
bh_chs_setup_list_l& ue_context_setup_resp_ies_o::value_c::bh_chs_setup_list()
{
  assert_choice_type(types::bh_chs_setup_list, type_, "Value");
  return c.get<bh_chs_setup_list_l>();
}
bh_chs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::bh_chs_failed_to_be_setup_list()
{
  assert_choice_type(types::bh_chs_failed_to_be_setup_list, type_, "Value");
  return c.get<bh_chs_failed_to_be_setup_list_l>();
}
sl_drbs_setup_list_l& ue_context_setup_resp_ies_o::value_c::sl_drbs_setup_list()
{
  assert_choice_type(types::sl_drbs_setup_list, type_, "Value");
  return c.get<sl_drbs_setup_list_l>();
}
sl_drbs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::sl_drbs_failed_to_be_setup_list()
{
  assert_choice_type(types::sl_drbs_failed_to_be_setup_list, type_, "Value");
  return c.get<sl_drbs_failed_to_be_setup_list_l>();
}
nr_cgi_s& ue_context_setup_resp_ies_o::value_c::requested_target_cell_global_id()
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
scg_activation_status_e& ue_context_setup_resp_ies_o::value_c::scg_activation_status()
{
  assert_choice_type(types::scg_activation_status, type_, "Value");
  return c.get<scg_activation_status_e>();
}
uu_rlc_ch_setup_list_l& ue_context_setup_resp_ies_o::value_c::uu_rlc_ch_setup_list()
{
  assert_choice_type(types::uu_rlc_ch_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_setup_list_l>();
}
uu_rlc_ch_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::uu_rlc_ch_failed_to_be_setup_list()
{
  assert_choice_type(types::uu_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_failed_to_be_setup_list_l>();
}
pc5_rlc_ch_setup_list_l& ue_context_setup_resp_ies_o::value_c::pc5_rlc_ch_setup_list()
{
  assert_choice_type(types::pc5_rlc_ch_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_setup_list_l>();
}
pc5_rlc_ch_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::pc5_rlc_ch_failed_to_be_setup_list()
{
  assert_choice_type(types::pc5_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_failed_to_be_setup_list_l>();
}
serving_cell_mo_encoded_in_cgc_list_l& ue_context_setup_resp_ies_o::value_c::serving_cell_mo_encoded_in_cgc_list()
{
  assert_choice_type(types::serving_cell_mo_encoded_in_cgc_list, type_, "Value");
  return c.get<serving_cell_mo_encoded_in_cgc_list_l>();
}
const uint64_t& ue_context_setup_resp_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_context_setup_resp_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const du_to_cu_rrc_info_s& ue_context_setup_resp_ies_o::value_c::du_to_cu_rrc_info() const
{
  assert_choice_type(types::du_to_cu_rrc_info, type_, "Value");
  return c.get<du_to_cu_rrc_info_s>();
}
const uint32_t& ue_context_setup_resp_ies_o::value_c::c_rnti() const
{
  assert_choice_type(types::c_rnti, type_, "Value");
  return c.get<uint32_t>();
}
const unbounded_octstring<true>& ue_context_setup_resp_ies_o::value_c::res_coordination_transfer_container() const
{
  assert_choice_type(types::res_coordination_transfer_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const full_cfg_e& ue_context_setup_resp_ies_o::value_c::full_cfg() const
{
  assert_choice_type(types::full_cfg, type_, "Value");
  return c.get<full_cfg_e>();
}
const drbs_setup_list_l& ue_context_setup_resp_ies_o::value_c::drbs_setup_list() const
{
  assert_choice_type(types::drbs_setup_list, type_, "Value");
  return c.get<drbs_setup_list_l>();
}
const srbs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::srbs_failed_to_be_setup_list() const
{
  assert_choice_type(types::srbs_failed_to_be_setup_list, type_, "Value");
  return c.get<srbs_failed_to_be_setup_list_l>();
}
const drbs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::drbs_failed_to_be_setup_list() const
{
  assert_choice_type(types::drbs_failed_to_be_setup_list, type_, "Value");
  return c.get<drbs_failed_to_be_setup_list_l>();
}
const scell_failedto_setup_list_l& ue_context_setup_resp_ies_o::value_c::scell_failedto_setup_list() const
{
  assert_choice_type(types::scell_failedto_setup_list, type_, "Value");
  return c.get<scell_failedto_setup_list_l>();
}
const inactivity_monitoring_resp_e& ue_context_setup_resp_ies_o::value_c::inactivity_monitoring_resp() const
{
  assert_choice_type(types::inactivity_monitoring_resp, type_, "Value");
  return c.get<inactivity_monitoring_resp_e>();
}
const crit_diagnostics_s& ue_context_setup_resp_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const srbs_setup_list_l& ue_context_setup_resp_ies_o::value_c::srbs_setup_list() const
{
  assert_choice_type(types::srbs_setup_list, type_, "Value");
  return c.get<srbs_setup_list_l>();
}
const bh_chs_setup_list_l& ue_context_setup_resp_ies_o::value_c::bh_chs_setup_list() const
{
  assert_choice_type(types::bh_chs_setup_list, type_, "Value");
  return c.get<bh_chs_setup_list_l>();
}
const bh_chs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::bh_chs_failed_to_be_setup_list() const
{
  assert_choice_type(types::bh_chs_failed_to_be_setup_list, type_, "Value");
  return c.get<bh_chs_failed_to_be_setup_list_l>();
}
const sl_drbs_setup_list_l& ue_context_setup_resp_ies_o::value_c::sl_drbs_setup_list() const
{
  assert_choice_type(types::sl_drbs_setup_list, type_, "Value");
  return c.get<sl_drbs_setup_list_l>();
}
const sl_drbs_failed_to_be_setup_list_l& ue_context_setup_resp_ies_o::value_c::sl_drbs_failed_to_be_setup_list() const
{
  assert_choice_type(types::sl_drbs_failed_to_be_setup_list, type_, "Value");
  return c.get<sl_drbs_failed_to_be_setup_list_l>();
}
const nr_cgi_s& ue_context_setup_resp_ies_o::value_c::requested_target_cell_global_id() const
{
  assert_choice_type(types::requested_target_cell_global_id, type_, "Value");
  return c.get<nr_cgi_s>();
}
const scg_activation_status_e& ue_context_setup_resp_ies_o::value_c::scg_activation_status() const
{
  assert_choice_type(types::scg_activation_status, type_, "Value");
  return c.get<scg_activation_status_e>();
}
const uu_rlc_ch_setup_list_l& ue_context_setup_resp_ies_o::value_c::uu_rlc_ch_setup_list() const
{
  assert_choice_type(types::uu_rlc_ch_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_setup_list_l>();
}
const uu_rlc_ch_failed_to_be_setup_list_l&
ue_context_setup_resp_ies_o::value_c::uu_rlc_ch_failed_to_be_setup_list() const
{
  assert_choice_type(types::uu_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<uu_rlc_ch_failed_to_be_setup_list_l>();
}
const pc5_rlc_ch_setup_list_l& ue_context_setup_resp_ies_o::value_c::pc5_rlc_ch_setup_list() const
{
  assert_choice_type(types::pc5_rlc_ch_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_setup_list_l>();
}
const pc5_rlc_ch_failed_to_be_setup_list_l&
ue_context_setup_resp_ies_o::value_c::pc5_rlc_ch_failed_to_be_setup_list() const
{
  assert_choice_type(types::pc5_rlc_ch_failed_to_be_setup_list, type_, "Value");
  return c.get<pc5_rlc_ch_failed_to_be_setup_list_l>();
}
const serving_cell_mo_encoded_in_cgc_list_l&
ue_context_setup_resp_ies_o::value_c::serving_cell_mo_encoded_in_cgc_list() const
{
  assert_choice_type(types::serving_cell_mo_encoded_in_cgc_list, type_, "Value");
  return c.get<serving_cell_mo_encoded_in_cgc_list_l>();
}
void ue_context_setup_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::du_to_cu_rrc_info:
      j.write_fieldname("DUtoCURRCInformation");
      c.get<du_to_cu_rrc_info_s>().to_json(j);
      break;
    case types::c_rnti:
      j.write_int("INTEGER (0..65535,...)", c.get<uint32_t>());
      break;
    case types::res_coordination_transfer_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::full_cfg:
      j.write_str("FullConfiguration", "full");
      break;
    case types::drbs_setup_list:
      j.start_array("DRBs-Setup-List");
      for (const auto& e1 : c.get<drbs_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::srbs_failed_to_be_setup_list:
      j.start_array("SRBs-FailedToBeSetup-List");
      for (const auto& e1 : c.get<srbs_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::drbs_failed_to_be_setup_list:
      j.start_array("DRBs-FailedToBeSetup-List");
      for (const auto& e1 : c.get<drbs_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::scell_failedto_setup_list:
      j.start_array("SCell-FailedtoSetup-List");
      for (const auto& e1 : c.get<scell_failedto_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::inactivity_monitoring_resp:
      j.write_str("InactivityMonitoringResponse", "not-supported");
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    case types::srbs_setup_list:
      j.start_array("SRBs-Setup-List");
      for (const auto& e1 : c.get<srbs_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::bh_chs_setup_list:
      j.start_array("BHChannels-Setup-List");
      for (const auto& e1 : c.get<bh_chs_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::bh_chs_failed_to_be_setup_list:
      j.start_array("BHChannels-FailedToBeSetup-List");
      for (const auto& e1 : c.get<bh_chs_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_setup_list:
      j.start_array("SLDRBs-Setup-List");
      for (const auto& e1 : c.get<sl_drbs_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      j.start_array("SLDRBs-FailedToBeSetup-List");
      for (const auto& e1 : c.get<sl_drbs_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::requested_target_cell_global_id:
      j.write_fieldname("NRCGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    case types::scg_activation_status:
      j.write_str("SCGActivationStatus", c.get<scg_activation_status_e>().to_string());
      break;
    case types::uu_rlc_ch_setup_list:
      j.start_array("UuRLCChannelSetupList");
      for (const auto& e1 : c.get<uu_rlc_ch_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      j.start_array("UuRLCChannelFailedToBeSetupList");
      for (const auto& e1 : c.get<uu_rlc_ch_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_setup_list:
      j.start_array("PC5RLCChannelSetupList");
      for (const auto& e1 : c.get<pc5_rlc_ch_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      j.start_array("PC5RLCChannelFailedToBeSetupList");
      for (const auto& e1 : c.get<pc5_rlc_ch_failed_to_be_setup_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      j.start_array("ServingCellMO-encoded-in-CGC-List");
      for (const auto& e1 : c.get<serving_cell_mo_encoded_in_cgc_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_context_setup_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::du_to_cu_rrc_info:
      HANDLE_CODE(c.get<du_to_cu_rrc_info_s>().pack(bref));
      break;
    case types::c_rnti:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u, true, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::full_cfg:
      HANDLE_CODE(c.get<full_cfg_e>().pack(bref));
      break;
    case types::drbs_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_setup_list_l>(), 1, 64, true));
      break;
    case types::srbs_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_failed_to_be_setup_list_l>(), 1, 8, true));
      break;
    case types::drbs_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drbs_failed_to_be_setup_list_l>(), 1, 64, true));
      break;
    case types::scell_failedto_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<scell_failedto_setup_list_l>(), 1, 32, true));
      break;
    case types::inactivity_monitoring_resp:
      HANDLE_CODE(c.get<inactivity_monitoring_resp_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::srbs_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<srbs_setup_list_l>(), 1, 8, true));
      break;
    case types::bh_chs_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_setup_list_l>(), 1, 65536, true));
      break;
    case types::bh_chs_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<bh_chs_failed_to_be_setup_list_l>(), 1, 65536, true));
      break;
    case types::sl_drbs_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_setup_list_l>(), 1, 512, true));
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<sl_drbs_failed_to_be_setup_list_l>(), 1, 512, true));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    case types::scg_activation_status:
      HANDLE_CODE(c.get<scg_activation_status_e>().pack(bref));
      break;
    case types::uu_rlc_ch_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_setup_list_l>(), 1, 32, true));
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<uu_rlc_ch_failed_to_be_setup_list_l>(), 1, 32, true));
      break;
    case types::pc5_rlc_ch_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_setup_list_l>(), 1, 512, true));
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<pc5_rlc_ch_failed_to_be_setup_list_l>(), 1, 512, true));
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<serving_cell_mo_encoded_in_cgc_list_l>(), 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_setup_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::du_to_cu_rrc_info:
      HANDLE_CODE(c.get<du_to_cu_rrc_info_s>().unpack(bref));
      break;
    case types::c_rnti:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u, true, true));
      break;
    case types::res_coordination_transfer_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::full_cfg:
      HANDLE_CODE(c.get<full_cfg_e>().unpack(bref));
      break;
    case types::drbs_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_setup_list_l>(), bref, 1, 64, true));
      break;
    case types::srbs_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_failed_to_be_setup_list_l>(), bref, 1, 8, true));
      break;
    case types::drbs_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drbs_failed_to_be_setup_list_l>(), bref, 1, 64, true));
      break;
    case types::scell_failedto_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<scell_failedto_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::inactivity_monitoring_resp:
      HANDLE_CODE(c.get<inactivity_monitoring_resp_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::srbs_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<srbs_setup_list_l>(), bref, 1, 8, true));
      break;
    case types::bh_chs_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_setup_list_l>(), bref, 1, 65536, true));
      break;
    case types::bh_chs_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<bh_chs_failed_to_be_setup_list_l>(), bref, 1, 65536, true));
      break;
    case types::sl_drbs_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<sl_drbs_failed_to_be_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::requested_target_cell_global_id:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    case types::scg_activation_status:
      HANDLE_CODE(c.get<scg_activation_status_e>().unpack(bref));
      break;
    case types::uu_rlc_ch_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<uu_rlc_ch_failed_to_be_setup_list_l>(), bref, 1, 32, true));
      break;
    case types::pc5_rlc_ch_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<pc5_rlc_ch_failed_to_be_setup_list_l>(), bref, 1, 512, true));
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<serving_cell_mo_encoded_in_cgc_list_l>(), bref, 1, 8, true));
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UEInactivityNotificationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ue_inactivity_notif_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 100, 635};
  return map_enum_number(names, 4, idx, "id");
}
bool ue_inactivity_notif_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 100, 635};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ue_inactivity_notif_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 100:
      return crit_e::reject;
    case 635:
      return crit_e::ignore;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ue_inactivity_notif_ies_o::value_c ue_inactivity_notif_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 100:
      ret.set(value_c::types::drb_activity_list);
      break;
    case 635:
      ret.set(value_c::types::sdt_termination_request);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ue_inactivity_notif_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 100:
      return presence_e::mandatory;
    case 635:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ue_inactivity_notif_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::drb_activity_list:
      c.destroy<drb_activity_list_l>();
      break;
    default:
      break;
  }
}
void ue_inactivity_notif_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::drb_activity_list:
      c.init<drb_activity_list_l>();
      break;
    case types::sdt_termination_request:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_inactivity_notif_ies_o::value_c");
  }
}
ue_inactivity_notif_ies_o::value_c::value_c(const ue_inactivity_notif_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::drb_activity_list:
      c.init(other.c.get<drb_activity_list_l>());
      break;
    case types::sdt_termination_request:
      c.init(other.c.get<sdt_termination_request_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_inactivity_notif_ies_o::value_c");
  }
}
ue_inactivity_notif_ies_o::value_c&
ue_inactivity_notif_ies_o::value_c::operator=(const ue_inactivity_notif_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::drb_activity_list:
      c.set(other.c.get<drb_activity_list_l>());
      break;
    case types::sdt_termination_request:
      c.set(other.c.get<sdt_termination_request_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_inactivity_notif_ies_o::value_c");
  }

  return *this;
}
uint64_t& ue_inactivity_notif_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ue_inactivity_notif_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
drb_activity_list_l& ue_inactivity_notif_ies_o::value_c::drb_activity_list()
{
  assert_choice_type(types::drb_activity_list, type_, "Value");
  return c.get<drb_activity_list_l>();
}
sdt_termination_request_e& ue_inactivity_notif_ies_o::value_c::sdt_termination_request()
{
  assert_choice_type(types::sdt_termination_request, type_, "Value");
  return c.get<sdt_termination_request_e>();
}
const uint64_t& ue_inactivity_notif_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ue_inactivity_notif_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const drb_activity_list_l& ue_inactivity_notif_ies_o::value_c::drb_activity_list() const
{
  assert_choice_type(types::drb_activity_list, type_, "Value");
  return c.get<drb_activity_list_l>();
}
const sdt_termination_request_e& ue_inactivity_notif_ies_o::value_c::sdt_termination_request() const
{
  assert_choice_type(types::sdt_termination_request, type_, "Value");
  return c.get<sdt_termination_request_e>();
}
void ue_inactivity_notif_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::drb_activity_list:
      j.start_array("DRB-Activity-List");
      for (const auto& e1 : c.get<drb_activity_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::sdt_termination_request:
      j.write_str("SDT-Termination-Request", c.get<sdt_termination_request_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ue_inactivity_notif_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ue_inactivity_notif_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drb_activity_list:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<drb_activity_list_l>(), 1, 64, true));
      break;
    case types::sdt_termination_request:
      HANDLE_CODE(c.get<sdt_termination_request_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_inactivity_notif_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_inactivity_notif_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::drb_activity_list:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<drb_activity_list_l>(), bref, 1, 64, true));
      break;
    case types::sdt_termination_request:
      HANDLE_CODE(c.get<sdt_termination_request_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ue_inactivity_notif_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ULRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
uint32_t ul_rrc_msg_transfer_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t names[] = {40, 41, 64, 50, 224, 219};
  return map_enum_number(names, 6, idx, "id");
}
bool ul_rrc_msg_transfer_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t names[] = {40, 41, 64, 50, 224, 219};
  for (const auto& o : names) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ul_rrc_msg_transfer_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 64:
      return crit_e::reject;
    case 50:
      return crit_e::reject;
    case 224:
      return crit_e::reject;
    case 219:
      return crit_e::reject;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}
ul_rrc_msg_transfer_ies_o::value_c ul_rrc_msg_transfer_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 40:
      ret.set(value_c::types::gnb_cu_ue_f1ap_id);
      break;
    case 41:
      ret.set(value_c::types::gnb_du_ue_f1ap_id);
      break;
    case 64:
      ret.set(value_c::types::srb_id);
      break;
    case 50:
      ret.set(value_c::types::rrc_container);
      break;
    case 224:
      ret.set(value_c::types::sel_plmn_id);
      break;
    case 219:
      ret.set(value_c::types::new_gnb_du_ue_f1ap_id);
      break;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return ret;
}
presence_e ul_rrc_msg_transfer_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 40:
      return presence_e::mandatory;
    case 41:
      return presence_e::mandatory;
    case 64:
      return presence_e::mandatory;
    case 50:
      return presence_e::mandatory;
    case 224:
      return presence_e::optional;
    case 219:
      return presence_e::optional;
    default:
      asn1::log_error("The id={} is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ul_rrc_msg_transfer_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::rrc_container:
      c.destroy<unbounded_octstring<true>>();
      break;
    case types::sel_plmn_id:
      c.destroy<fixed_octstring<3, true>>();
      break;
    default:
      break;
  }
}
void ul_rrc_msg_transfer_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      break;
    case types::gnb_du_ue_f1ap_id:
      break;
    case types::srb_id:
      break;
    case types::rrc_container:
      c.init<unbounded_octstring<true>>();
      break;
    case types::sel_plmn_id:
      c.init<fixed_octstring<3, true>>();
      break;
    case types::new_gnb_du_ue_f1ap_id:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_rrc_msg_transfer_ies_o::value_c");
  }
}
ul_rrc_msg_transfer_ies_o::value_c::value_c(const ul_rrc_msg_transfer_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::srb_id:
      c.init(other.c.get<uint8_t>());
      break;
    case types::rrc_container:
      c.init(other.c.get<unbounded_octstring<true>>());
      break;
    case types::sel_plmn_id:
      c.init(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::new_gnb_du_ue_f1ap_id:
      c.init(other.c.get<uint64_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_rrc_msg_transfer_ies_o::value_c");
  }
}
ul_rrc_msg_transfer_ies_o::value_c&
ul_rrc_msg_transfer_ies_o::value_c::operator=(const ul_rrc_msg_transfer_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::srb_id:
      c.set(other.c.get<uint8_t>());
      break;
    case types::rrc_container:
      c.set(other.c.get<unbounded_octstring<true>>());
      break;
    case types::sel_plmn_id:
      c.set(other.c.get<fixed_octstring<3, true>>());
      break;
    case types::new_gnb_du_ue_f1ap_id:
      c.set(other.c.get<uint64_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_rrc_msg_transfer_ies_o::value_c");
  }

  return *this;
}
uint64_t& ul_rrc_msg_transfer_ies_o::value_c::gnb_cu_ue_f1ap_id()
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint64_t& ul_rrc_msg_transfer_ies_o::value_c::gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
uint8_t& ul_rrc_msg_transfer_ies_o::value_c::srb_id()
{
  assert_choice_type(types::srb_id, type_, "Value");
  return c.get<uint8_t>();
}
unbounded_octstring<true>& ul_rrc_msg_transfer_ies_o::value_c::rrc_container()
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
fixed_octstring<3, true>& ul_rrc_msg_transfer_ies_o::value_c::sel_plmn_id()
{
  assert_choice_type(types::sel_plmn_id, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
uint64_t& ul_rrc_msg_transfer_ies_o::value_c::new_gnb_du_ue_f1ap_id()
{
  assert_choice_type(types::new_gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ul_rrc_msg_transfer_ies_o::value_c::gnb_cu_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_cu_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint64_t& ul_rrc_msg_transfer_ies_o::value_c::gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
const uint8_t& ul_rrc_msg_transfer_ies_o::value_c::srb_id() const
{
  assert_choice_type(types::srb_id, type_, "Value");
  return c.get<uint8_t>();
}
const unbounded_octstring<true>& ul_rrc_msg_transfer_ies_o::value_c::rrc_container() const
{
  assert_choice_type(types::rrc_container, type_, "Value");
  return c.get<unbounded_octstring<true>>();
}
const fixed_octstring<3, true>& ul_rrc_msg_transfer_ies_o::value_c::sel_plmn_id() const
{
  assert_choice_type(types::sel_plmn_id, type_, "Value");
  return c.get<fixed_octstring<3, true>>();
}
const uint64_t& ul_rrc_msg_transfer_ies_o::value_c::new_gnb_du_ue_f1ap_id() const
{
  assert_choice_type(types::new_gnb_du_ue_f1ap_id, type_, "Value");
  return c.get<uint64_t>();
}
void ul_rrc_msg_transfer_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    case types::srb_id:
      j.write_int("INTEGER (0..3,...)", c.get<uint8_t>());
      break;
    case types::rrc_container:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true>>().to_string());
      break;
    case types::sel_plmn_id:
      j.write_str("OCTET STRING", c.get<fixed_octstring<3, true>>().to_string());
      break;
    case types::new_gnb_du_ue_f1ap_id:
      j.write_int("INTEGER (0..4294967295)", c.get<uint64_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ul_rrc_msg_transfer_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_rrc_msg_transfer_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::srb_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)3u, true, true));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().pack(bref));
      break;
    case types::sel_plmn_id:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().pack(bref)));
      break;
    case types::new_gnb_du_ue_f1ap_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ul_rrc_msg_transfer_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_rrc_msg_transfer_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::srb_id:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)3u, true, true));
      break;
    case types::rrc_container:
      HANDLE_CODE(c.get<unbounded_octstring<true>>().unpack(bref));
      break;
    case types::sel_plmn_id:
      HANDLE_CODE((c.get<fixed_octstring<3, true>>().unpack(bref)));
      break;
    case types::new_gnb_du_ue_f1ap_id:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ul_rrc_msg_transfer_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

init_ul_rrc_msg_transfer_ies_container::init_ul_rrc_msg_transfer_ies_container() :
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  nr_cgi(111, crit_e::reject),
  c_rnti(95, crit_e::reject),
  rrc_container(50, crit_e::reject),
  du_to_cu_rrc_container(128, crit_e::reject),
  sul_access_ind(178, crit_e::ignore),
  transaction_id(78, crit_e::ignore),
  ran_ue_id(226, crit_e::ignore),
  rrc_container_rrc_setup_complete(241, crit_e::ignore),
  nr_red_cap_ue_ind(578, crit_e::ignore),
  sdt_info(592, crit_e::ignore),
  sidelink_relay_cfg(617, crit_e::ignore)
{
}
SRSASN_CODE init_ul_rrc_msg_transfer_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 5;
  nof_ies += du_to_cu_rrc_container_present ? 1 : 0;
  nof_ies += sul_access_ind_present ? 1 : 0;
  nof_ies += ran_ue_id_present ? 1 : 0;
  nof_ies += rrc_container_rrc_setup_complete_present ? 1 : 0;
  nof_ies += nr_red_cap_ue_ind_present ? 1 : 0;
  nof_ies += sdt_info_present ? 1 : 0;
  nof_ies += sidelink_relay_cfg_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(nr_cgi.pack(bref));
  HANDLE_CODE(c_rnti.pack(bref));
  HANDLE_CODE(rrc_container.pack(bref));
  if (du_to_cu_rrc_container_present) {
    HANDLE_CODE(du_to_cu_rrc_container.pack(bref));
  }
  if (sul_access_ind_present) {
    HANDLE_CODE(sul_access_ind.pack(bref));
  }
  HANDLE_CODE(transaction_id.pack(bref));
  if (ran_ue_id_present) {
    HANDLE_CODE(ran_ue_id.pack(bref));
  }
  if (rrc_container_rrc_setup_complete_present) {
    HANDLE_CODE(rrc_container_rrc_setup_complete.pack(bref));
  }
  if (nr_red_cap_ue_ind_present) {
    HANDLE_CODE(nr_red_cap_ue_ind.pack(bref));
  }
  if (sdt_info_present) {
    HANDLE_CODE(sdt_info.pack(bref));
  }
  if (sidelink_relay_cfg_present) {
    HANDLE_CODE(sidelink_relay_cfg.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_ul_rrc_msg_transfer_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 5;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 111: {
        nof_mandatory_ies--;
        nr_cgi.id = id;
        HANDLE_CODE(nr_cgi.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_cgi.value.unpack(bref));
        break;
      }
      case 95: {
        nof_mandatory_ies--;
        c_rnti.id = id;
        HANDLE_CODE(c_rnti.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(c_rnti.value.unpack(bref));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        rrc_container.id = id;
        HANDLE_CODE(rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.value.unpack(bref));
        break;
      }
      case 128: {
        du_to_cu_rrc_container_present = true;
        du_to_cu_rrc_container.id      = id;
        HANDLE_CODE(du_to_cu_rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_container.value.unpack(bref));
        break;
      }
      case 178: {
        sul_access_ind_present = true;
        sul_access_ind.id      = id;
        HANDLE_CODE(sul_access_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sul_access_ind.value.unpack(bref));
        break;
      }
      case 78: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 226: {
        ran_ue_id_present = true;
        ran_ue_id.id      = id;
        HANDLE_CODE(ran_ue_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ran_ue_id.value.unpack(bref));
        break;
      }
      case 241: {
        rrc_container_rrc_setup_complete_present = true;
        rrc_container_rrc_setup_complete.id      = id;
        HANDLE_CODE(rrc_container_rrc_setup_complete.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container_rrc_setup_complete.value.unpack(bref));
        break;
      }
      case 578: {
        nr_red_cap_ue_ind_present = true;
        nr_red_cap_ue_ind.id      = id;
        HANDLE_CODE(nr_red_cap_ue_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_red_cap_ue_ind.value.unpack(bref));
        break;
      }
      case 592: {
        sdt_info_present = true;
        sdt_info.id      = id;
        HANDLE_CODE(sdt_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_info.value.unpack(bref));
        break;
      }
      case 617: {
        sidelink_relay_cfg_present = true;
        sidelink_relay_cfg.id      = id;
        HANDLE_CODE(sidelink_relay_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sidelink_relay_cfg.value.unpack(bref));
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
void init_ul_rrc_msg_transfer_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  nr_cgi.to_json(j);
  j.write_fieldname("");
  c_rnti.to_json(j);
  j.write_fieldname("");
  rrc_container.to_json(j);
  if (du_to_cu_rrc_container_present) {
    j.write_fieldname("");
    du_to_cu_rrc_container.to_json(j);
  }
  if (sul_access_ind_present) {
    j.write_fieldname("");
    sul_access_ind.to_json(j);
  }
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (ran_ue_id_present) {
    j.write_fieldname("");
    ran_ue_id.to_json(j);
  }
  if (rrc_container_rrc_setup_complete_present) {
    j.write_fieldname("");
    rrc_container_rrc_setup_complete.to_json(j);
  }
  if (nr_red_cap_ue_ind_present) {
    j.write_fieldname("");
    nr_red_cap_ue_ind.to_json(j);
  }
  if (sdt_info_present) {
    j.write_fieldname("");
    sdt_info.to_json(j);
  }
  if (sidelink_relay_cfg_present) {
    j.write_fieldname("");
    sidelink_relay_cfg.to_json(j);
  }
  j.end_obj();
}

ue_context_mod_confirm_ies_container::ue_context_mod_confirm_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  res_coordination_transfer_container(49, crit_e::ignore),
  drbs_modified_conf_list(19, crit_e::ignore),
  rrc_container(50, crit_e::ignore),
  crit_diagnostics(7, crit_e::ignore),
  execute_dupl(109, crit_e::ignore),
  res_coordination_transfer_info(195, crit_e::ignore),
  sl_drbs_modified_conf_list(337, crit_e::ignore),
  uu_rlc_ch_modified_list(604, crit_e::reject),
  pc5_rlc_ch_modified_list(616, crit_e::reject),
  ue_multicast_m_rbs_confirmed_to_be_modified_list(653, crit_e::reject)
{
}
SRSASN_CODE ue_context_mod_confirm_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += res_coordination_transfer_container_present ? 1 : 0;
  nof_ies += drbs_modified_conf_list_present ? 1 : 0;
  nof_ies += rrc_container_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += execute_dupl_present ? 1 : 0;
  nof_ies += res_coordination_transfer_info_present ? 1 : 0;
  nof_ies += sl_drbs_modified_conf_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_modified_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_modified_list_present ? 1 : 0;
  nof_ies += ue_multicast_m_rbs_confirmed_to_be_modified_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (drbs_modified_conf_list_present) {
    HANDLE_CODE(drbs_modified_conf_list.pack(bref));
  }
  if (rrc_container_present) {
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (execute_dupl_present) {
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  if (res_coordination_transfer_info_present) {
    HANDLE_CODE(res_coordination_transfer_info.pack(bref));
  }
  if (sl_drbs_modified_conf_list_present) {
    HANDLE_CODE(sl_drbs_modified_conf_list.pack(bref));
  }
  if (uu_rlc_ch_modified_list_present) {
    HANDLE_CODE(uu_rlc_ch_modified_list.pack(bref));
  }
  if (pc5_rlc_ch_modified_list_present) {
    HANDLE_CODE(pc5_rlc_ch_modified_list.pack(bref));
  }
  if (ue_multicast_m_rbs_confirmed_to_be_modified_list_present) {
    HANDLE_CODE(ue_multicast_m_rbs_confirmed_to_be_modified_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_confirm_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        res_coordination_transfer_container.id      = id;
        HANDLE_CODE(res_coordination_transfer_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.value.unpack(bref));
        break;
      }
      case 19: {
        drbs_modified_conf_list_present = true;
        drbs_modified_conf_list.id      = id;
        HANDLE_CODE(drbs_modified_conf_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_modified_conf_list.value.unpack(bref));
        break;
      }
      case 50: {
        rrc_container_present = true;
        rrc_container.id      = id;
        HANDLE_CODE(rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.value.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        execute_dupl.id      = id;
        HANDLE_CODE(execute_dupl.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.value.unpack(bref));
        break;
      }
      case 195: {
        res_coordination_transfer_info_present = true;
        res_coordination_transfer_info.id      = id;
        HANDLE_CODE(res_coordination_transfer_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_info.value.unpack(bref));
        break;
      }
      case 337: {
        sl_drbs_modified_conf_list_present = true;
        sl_drbs_modified_conf_list.id      = id;
        HANDLE_CODE(sl_drbs_modified_conf_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_modified_conf_list.value.unpack(bref));
        break;
      }
      case 604: {
        uu_rlc_ch_modified_list_present = true;
        uu_rlc_ch_modified_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_modified_list.value.unpack(bref));
        break;
      }
      case 616: {
        pc5_rlc_ch_modified_list_present = true;
        pc5_rlc_ch_modified_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_modified_list.value.unpack(bref));
        break;
      }
      case 653: {
        ue_multicast_m_rbs_confirmed_to_be_modified_list_present = true;
        ue_multicast_m_rbs_confirmed_to_be_modified_list.id      = id;
        HANDLE_CODE(ue_multicast_m_rbs_confirmed_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_multicast_m_rbs_confirmed_to_be_modified_list.value.unpack(bref));
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
void ue_context_mod_confirm_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  if (res_coordination_transfer_container_present) {
    j.write_fieldname("");
    res_coordination_transfer_container.to_json(j);
  }
  if (drbs_modified_conf_list_present) {
    j.write_fieldname("");
    drbs_modified_conf_list.to_json(j);
  }
  if (rrc_container_present) {
    j.write_fieldname("");
    rrc_container.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  if (execute_dupl_present) {
    j.write_fieldname("");
    execute_dupl.to_json(j);
  }
  if (res_coordination_transfer_info_present) {
    j.write_fieldname("");
    res_coordination_transfer_info.to_json(j);
  }
  if (sl_drbs_modified_conf_list_present) {
    j.write_fieldname("");
    sl_drbs_modified_conf_list.to_json(j);
  }
  if (uu_rlc_ch_modified_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_modified_list.to_json(j);
  }
  if (pc5_rlc_ch_modified_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_modified_list.to_json(j);
  }
  if (ue_multicast_m_rbs_confirmed_to_be_modified_list_present) {
    j.write_fieldname("");
    ue_multicast_m_rbs_confirmed_to_be_modified_list.to_json(j);
  }
  j.end_obj();
}

ue_context_mod_fail_ies_container::ue_context_mod_fail_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  cause(0, crit_e::ignore),
  crit_diagnostics(7, crit_e::ignore),
  requested_target_cell_global_id(376, crit_e::reject)
{
}
SRSASN_CODE ue_context_mod_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += requested_target_cell_global_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(requested_target_cell_global_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        requested_target_cell_global_id.id      = id;
        HANDLE_CODE(requested_target_cell_global_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.value.unpack(bref));
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
void ue_context_mod_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  if (requested_target_cell_global_id_present) {
    j.write_fieldname("");
    requested_target_cell_global_id.to_json(j);
  }
  j.end_obj();
}

ue_context_mod_refuse_ies_container::ue_context_mod_refuse_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  cause(0, crit_e::ignore),
  crit_diagnostics(7, crit_e::ignore)
{
}
SRSASN_CODE ue_context_mod_refuse_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_refuse_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
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
void ue_context_mod_refuse_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

ue_context_mod_request_ies_container::ue_context_mod_request_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  sp_cell_id(63, crit_e::ignore),
  serv_cell_idx(107, crit_e::reject),
  sp_cell_ul_cfg(96, crit_e::ignore),
  drx_cycle(38, crit_e::ignore),
  cu_to_du_rrc_info(9, crit_e::reject),
  tx_action_ind(79, crit_e::ignore),
  res_coordination_transfer_container(49, crit_e::ignore),
  rrc_recfg_complete_ind(87, crit_e::ignore),
  rrc_container(50, crit_e::reject),
  scell_to_be_setup_mod_list(56, crit_e::ignore),
  scell_to_be_remd_list(52, crit_e::ignore),
  srbs_to_be_setup_mod_list(76, crit_e::reject),
  drbs_to_be_setup_mod_list(37, crit_e::reject),
  drbs_to_be_modified_list(31, crit_e::reject),
  srbs_to_be_released_list(72, crit_e::reject),
  drbs_to_be_released_list(33, crit_e::reject),
  inactivity_monitoring_request(97, crit_e::reject),
  rat_freq_prio_info(108, crit_e::reject),
  drx_cfg_ind(159, crit_e::ignore),
  rlc_fail_ind(174, crit_e::ignore),
  ul_tx_direct_current_list_info(175, crit_e::ignore),
  gnb_du_cfg_query(162, crit_e::reject),
  gnb_du_ue_ambr_ul(158, crit_e::ignore),
  execute_dupl(109, crit_e::ignore),
  rrc_delivery_status_request(184, crit_e::ignore),
  res_coordination_transfer_info(195, crit_e::ignore),
  serving_cell_mo(182, crit_e::ignore),
  needfor_gap(215, crit_e::ignore),
  full_cfg(94, crit_e::reject),
  add_rrm_prio_idx(248, crit_e::ignore),
  lower_layer_presence_status_change(253, crit_e::ignore),
  bh_chs_to_be_setup_mod_list(267, crit_e::reject),
  bh_chs_to_be_modified_list(263, crit_e::reject),
  bh_chs_to_be_released_list(265, crit_e::reject),
  nr_v2x_services_authorized(306, crit_e::ignore),
  ltev2x_services_authorized(307, crit_e::ignore),
  nr_ue_sidelink_aggr_max_bitrate(308, crit_e::ignore),
  lte_ue_sidelink_aggr_max_bitrate(309, crit_e::ignore),
  pc5_link_ambr(340, crit_e::ignore),
  sl_drbs_to_be_setup_mod_list(332, crit_e::reject),
  sl_drbs_to_be_modified_list(326, crit_e::reject),
  sl_drbs_to_be_released_list(328, crit_e::reject),
  conditional_intra_du_mob_info(374, crit_e::reject),
  f1_c_transfer_path(428, crit_e::reject),
  scg_ind(432, crit_e::ignore),
  ul_tx_direct_current_two_carrier_list_info(684, crit_e::ignore),
  iab_conditional_rrc_msg_delivery_ind(510, crit_e::reject),
  f1_c_transfer_path_nr_dc(511, crit_e::reject),
  mdt_polluted_meas_ind(536, crit_e::ignore),
  scg_activation_request(547, crit_e::ignore),
  cg_sdt_query_ind(586, crit_e::ignore),
  five_g_pro_se_authorized(594, crit_e::ignore),
  five_g_pro_se_ue_pc5_aggr_max_bitrate(595, crit_e::ignore),
  five_g_pro_se_pc5_link_ambr(596, crit_e::ignore),
  upd_remote_ue_local_id(618, crit_e::ignore),
  uu_rlc_ch_to_be_setup_list(599, crit_e::reject),
  uu_rlc_ch_to_be_modified_list(600, crit_e::reject),
  uu_rlc_ch_to_be_released_list(601, crit_e::reject),
  pc5_rlc_ch_to_be_setup_list(608, crit_e::reject),
  pc5_rlc_ch_to_be_modified_list(609, crit_e::reject),
  pc5_rlc_ch_to_be_released_list(610, crit_e::reject),
  path_switch_cfg(619, crit_e::ignore),
  gnb_du_ue_slice_max_bit_rate_list(626, crit_e::ignore),
  multicast_mbs_session_setup_list(632, crit_e::reject),
  multicast_mbs_session_rem_list(633, crit_e::reject),
  ue_multicast_m_rbs_to_be_setup_at_modify_list(685, crit_e::reject),
  ue_multicast_m_rbs_to_be_released_list(628, crit_e::reject),
  sldrx_cycle_list(643, crit_e::ignore),
  management_based_mdt_plmn_mod_list(647, crit_e::ignore),
  sdt_bearer_cfg_query_ind(675, crit_e::ignore),
  daps_ho_status(683, crit_e::ignore),
  serving_cell_mo_list(695, crit_e::ignore),
  ul_tx_direct_current_more_carrier_info(690, crit_e::ignore),
  cp_acmcg_info(691, crit_e::ignore)
{
}
SRSASN_CODE ue_context_mod_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += sp_cell_id_present ? 1 : 0;
  nof_ies += serv_cell_idx_present ? 1 : 0;
  nof_ies += sp_cell_ul_cfg_present ? 1 : 0;
  nof_ies += drx_cycle_present ? 1 : 0;
  nof_ies += cu_to_du_rrc_info_present ? 1 : 0;
  nof_ies += tx_action_ind_present ? 1 : 0;
  nof_ies += res_coordination_transfer_container_present ? 1 : 0;
  nof_ies += rrc_recfg_complete_ind_present ? 1 : 0;
  nof_ies += rrc_container_present ? 1 : 0;
  nof_ies += scell_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += scell_to_be_remd_list_present ? 1 : 0;
  nof_ies += srbs_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += drbs_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += drbs_to_be_modified_list_present ? 1 : 0;
  nof_ies += srbs_to_be_released_list_present ? 1 : 0;
  nof_ies += drbs_to_be_released_list_present ? 1 : 0;
  nof_ies += inactivity_monitoring_request_present ? 1 : 0;
  nof_ies += rat_freq_prio_info_present ? 1 : 0;
  nof_ies += drx_cfg_ind_present ? 1 : 0;
  nof_ies += rlc_fail_ind_present ? 1 : 0;
  nof_ies += ul_tx_direct_current_list_info_present ? 1 : 0;
  nof_ies += gnb_du_cfg_query_present ? 1 : 0;
  nof_ies += gnb_du_ue_ambr_ul_present ? 1 : 0;
  nof_ies += execute_dupl_present ? 1 : 0;
  nof_ies += rrc_delivery_status_request_present ? 1 : 0;
  nof_ies += res_coordination_transfer_info_present ? 1 : 0;
  nof_ies += serving_cell_mo_present ? 1 : 0;
  nof_ies += needfor_gap_present ? 1 : 0;
  nof_ies += full_cfg_present ? 1 : 0;
  nof_ies += add_rrm_prio_idx_present ? 1 : 0;
  nof_ies += lower_layer_presence_status_change_present ? 1 : 0;
  nof_ies += bh_chs_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += bh_chs_to_be_modified_list_present ? 1 : 0;
  nof_ies += bh_chs_to_be_released_list_present ? 1 : 0;
  nof_ies += nr_v2x_services_authorized_present ? 1 : 0;
  nof_ies += ltev2x_services_authorized_present ? 1 : 0;
  nof_ies += nr_ue_sidelink_aggr_max_bitrate_present ? 1 : 0;
  nof_ies += lte_ue_sidelink_aggr_max_bitrate_present ? 1 : 0;
  nof_ies += pc5_link_ambr_present ? 1 : 0;
  nof_ies += sl_drbs_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += sl_drbs_to_be_modified_list_present ? 1 : 0;
  nof_ies += sl_drbs_to_be_released_list_present ? 1 : 0;
  nof_ies += conditional_intra_du_mob_info_present ? 1 : 0;
  nof_ies += f1_c_transfer_path_present ? 1 : 0;
  nof_ies += scg_ind_present ? 1 : 0;
  nof_ies += ul_tx_direct_current_two_carrier_list_info_present ? 1 : 0;
  nof_ies += iab_conditional_rrc_msg_delivery_ind_present ? 1 : 0;
  nof_ies += f1_c_transfer_path_nr_dc_present ? 1 : 0;
  nof_ies += mdt_polluted_meas_ind_present ? 1 : 0;
  nof_ies += scg_activation_request_present ? 1 : 0;
  nof_ies += cg_sdt_query_ind_present ? 1 : 0;
  nof_ies += five_g_pro_se_authorized_present ? 1 : 0;
  nof_ies += five_g_pro_se_ue_pc5_aggr_max_bitrate_present ? 1 : 0;
  nof_ies += five_g_pro_se_pc5_link_ambr_present ? 1 : 0;
  nof_ies += upd_remote_ue_local_id_present ? 1 : 0;
  nof_ies += uu_rlc_ch_to_be_setup_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_to_be_modified_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_to_be_released_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_to_be_setup_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_to_be_modified_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_to_be_released_list_present ? 1 : 0;
  nof_ies += path_switch_cfg_present ? 1 : 0;
  nof_ies += gnb_du_ue_slice_max_bit_rate_list_present ? 1 : 0;
  nof_ies += multicast_mbs_session_setup_list_present ? 1 : 0;
  nof_ies += multicast_mbs_session_rem_list_present ? 1 : 0;
  nof_ies += ue_multicast_m_rbs_to_be_setup_at_modify_list_present ? 1 : 0;
  nof_ies += ue_multicast_m_rbs_to_be_released_list_present ? 1 : 0;
  nof_ies += sldrx_cycle_list_present ? 1 : 0;
  nof_ies += management_based_mdt_plmn_mod_list_present ? 1 : 0;
  nof_ies += sdt_bearer_cfg_query_ind_present ? 1 : 0;
  nof_ies += daps_ho_status_present ? 1 : 0;
  nof_ies += serving_cell_mo_list_present ? 1 : 0;
  nof_ies += ul_tx_direct_current_more_carrier_info_present ? 1 : 0;
  nof_ies += cp_acmcg_info_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  if (sp_cell_id_present) {
    HANDLE_CODE(sp_cell_id.pack(bref));
  }
  if (serv_cell_idx_present) {
    HANDLE_CODE(serv_cell_idx.pack(bref));
  }
  if (sp_cell_ul_cfg_present) {
    HANDLE_CODE(sp_cell_ul_cfg.pack(bref));
  }
  if (drx_cycle_present) {
    HANDLE_CODE(drx_cycle.pack(bref));
  }
  if (cu_to_du_rrc_info_present) {
    HANDLE_CODE(cu_to_du_rrc_info.pack(bref));
  }
  if (tx_action_ind_present) {
    HANDLE_CODE(tx_action_ind.pack(bref));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (rrc_recfg_complete_ind_present) {
    HANDLE_CODE(rrc_recfg_complete_ind.pack(bref));
  }
  if (rrc_container_present) {
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (scell_to_be_setup_mod_list_present) {
    HANDLE_CODE(scell_to_be_setup_mod_list.pack(bref));
  }
  if (scell_to_be_remd_list_present) {
    HANDLE_CODE(scell_to_be_remd_list.pack(bref));
  }
  if (srbs_to_be_setup_mod_list_present) {
    HANDLE_CODE(srbs_to_be_setup_mod_list.pack(bref));
  }
  if (drbs_to_be_setup_mod_list_present) {
    HANDLE_CODE(drbs_to_be_setup_mod_list.pack(bref));
  }
  if (drbs_to_be_modified_list_present) {
    HANDLE_CODE(drbs_to_be_modified_list.pack(bref));
  }
  if (srbs_to_be_released_list_present) {
    HANDLE_CODE(srbs_to_be_released_list.pack(bref));
  }
  if (drbs_to_be_released_list_present) {
    HANDLE_CODE(drbs_to_be_released_list.pack(bref));
  }
  if (inactivity_monitoring_request_present) {
    HANDLE_CODE(inactivity_monitoring_request.pack(bref));
  }
  if (rat_freq_prio_info_present) {
    HANDLE_CODE(rat_freq_prio_info.pack(bref));
  }
  if (drx_cfg_ind_present) {
    HANDLE_CODE(drx_cfg_ind.pack(bref));
  }
  if (rlc_fail_ind_present) {
    HANDLE_CODE(rlc_fail_ind.pack(bref));
  }
  if (ul_tx_direct_current_list_info_present) {
    HANDLE_CODE(ul_tx_direct_current_list_info.pack(bref));
  }
  if (gnb_du_cfg_query_present) {
    HANDLE_CODE(gnb_du_cfg_query.pack(bref));
  }
  if (gnb_du_ue_ambr_ul_present) {
    HANDLE_CODE(gnb_du_ue_ambr_ul.pack(bref));
  }
  if (execute_dupl_present) {
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (res_coordination_transfer_info_present) {
    HANDLE_CODE(res_coordination_transfer_info.pack(bref));
  }
  if (serving_cell_mo_present) {
    HANDLE_CODE(serving_cell_mo.pack(bref));
  }
  if (needfor_gap_present) {
    HANDLE_CODE(needfor_gap.pack(bref));
  }
  if (full_cfg_present) {
    HANDLE_CODE(full_cfg.pack(bref));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }
  if (lower_layer_presence_status_change_present) {
    HANDLE_CODE(lower_layer_presence_status_change.pack(bref));
  }
  if (bh_chs_to_be_setup_mod_list_present) {
    HANDLE_CODE(bh_chs_to_be_setup_mod_list.pack(bref));
  }
  if (bh_chs_to_be_modified_list_present) {
    HANDLE_CODE(bh_chs_to_be_modified_list.pack(bref));
  }
  if (bh_chs_to_be_released_list_present) {
    HANDLE_CODE(bh_chs_to_be_released_list.pack(bref));
  }
  if (nr_v2x_services_authorized_present) {
    HANDLE_CODE(nr_v2x_services_authorized.pack(bref));
  }
  if (ltev2x_services_authorized_present) {
    HANDLE_CODE(ltev2x_services_authorized.pack(bref));
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (pc5_link_ambr_present) {
    HANDLE_CODE(pc5_link_ambr.pack(bref));
  }
  if (sl_drbs_to_be_setup_mod_list_present) {
    HANDLE_CODE(sl_drbs_to_be_setup_mod_list.pack(bref));
  }
  if (sl_drbs_to_be_modified_list_present) {
    HANDLE_CODE(sl_drbs_to_be_modified_list.pack(bref));
  }
  if (sl_drbs_to_be_released_list_present) {
    HANDLE_CODE(sl_drbs_to_be_released_list.pack(bref));
  }
  if (conditional_intra_du_mob_info_present) {
    HANDLE_CODE(conditional_intra_du_mob_info.pack(bref));
  }
  if (f1_c_transfer_path_present) {
    HANDLE_CODE(f1_c_transfer_path.pack(bref));
  }
  if (scg_ind_present) {
    HANDLE_CODE(scg_ind.pack(bref));
  }
  if (ul_tx_direct_current_two_carrier_list_info_present) {
    HANDLE_CODE(ul_tx_direct_current_two_carrier_list_info.pack(bref));
  }
  if (iab_conditional_rrc_msg_delivery_ind_present) {
    HANDLE_CODE(iab_conditional_rrc_msg_delivery_ind.pack(bref));
  }
  if (f1_c_transfer_path_nr_dc_present) {
    HANDLE_CODE(f1_c_transfer_path_nr_dc.pack(bref));
  }
  if (mdt_polluted_meas_ind_present) {
    HANDLE_CODE(mdt_polluted_meas_ind.pack(bref));
  }
  if (scg_activation_request_present) {
    HANDLE_CODE(scg_activation_request.pack(bref));
  }
  if (cg_sdt_query_ind_present) {
    HANDLE_CODE(cg_sdt_query_ind.pack(bref));
  }
  if (five_g_pro_se_authorized_present) {
    HANDLE_CODE(five_g_pro_se_authorized.pack(bref));
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.pack(bref));
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    HANDLE_CODE(five_g_pro_se_pc5_link_ambr.pack(bref));
  }
  if (upd_remote_ue_local_id_present) {
    HANDLE_CODE(upd_remote_ue_local_id.pack(bref));
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(uu_rlc_ch_to_be_setup_list.pack(bref));
  }
  if (uu_rlc_ch_to_be_modified_list_present) {
    HANDLE_CODE(uu_rlc_ch_to_be_modified_list.pack(bref));
  }
  if (uu_rlc_ch_to_be_released_list_present) {
    HANDLE_CODE(uu_rlc_ch_to_be_released_list.pack(bref));
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(pc5_rlc_ch_to_be_setup_list.pack(bref));
  }
  if (pc5_rlc_ch_to_be_modified_list_present) {
    HANDLE_CODE(pc5_rlc_ch_to_be_modified_list.pack(bref));
  }
  if (pc5_rlc_ch_to_be_released_list_present) {
    HANDLE_CODE(pc5_rlc_ch_to_be_released_list.pack(bref));
  }
  if (path_switch_cfg_present) {
    HANDLE_CODE(path_switch_cfg.pack(bref));
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    HANDLE_CODE(gnb_du_ue_slice_max_bit_rate_list.pack(bref));
  }
  if (multicast_mbs_session_setup_list_present) {
    HANDLE_CODE(multicast_mbs_session_setup_list.pack(bref));
  }
  if (multicast_mbs_session_rem_list_present) {
    HANDLE_CODE(multicast_mbs_session_rem_list.pack(bref));
  }
  if (ue_multicast_m_rbs_to_be_setup_at_modify_list_present) {
    HANDLE_CODE(ue_multicast_m_rbs_to_be_setup_at_modify_list.pack(bref));
  }
  if (ue_multicast_m_rbs_to_be_released_list_present) {
    HANDLE_CODE(ue_multicast_m_rbs_to_be_released_list.pack(bref));
  }
  if (sldrx_cycle_list_present) {
    HANDLE_CODE(sldrx_cycle_list.pack(bref));
  }
  if (management_based_mdt_plmn_mod_list_present) {
    HANDLE_CODE(management_based_mdt_plmn_mod_list.pack(bref));
  }
  if (sdt_bearer_cfg_query_ind_present) {
    HANDLE_CODE(sdt_bearer_cfg_query_ind.pack(bref));
  }
  if (daps_ho_status_present) {
    HANDLE_CODE(daps_ho_status.pack(bref));
  }
  if (serving_cell_mo_list_present) {
    HANDLE_CODE(serving_cell_mo_list.pack(bref));
  }
  if (ul_tx_direct_current_more_carrier_info_present) {
    HANDLE_CODE(ul_tx_direct_current_more_carrier_info.pack(bref));
  }
  if (cp_acmcg_info_present) {
    HANDLE_CODE(cp_acmcg_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 63: {
        sp_cell_id_present = true;
        sp_cell_id.id      = id;
        HANDLE_CODE(sp_cell_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_id.value.unpack(bref));
        break;
      }
      case 107: {
        serv_cell_idx_present = true;
        serv_cell_idx.id      = id;
        HANDLE_CODE(serv_cell_idx.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serv_cell_idx.value.unpack(bref));
        break;
      }
      case 96: {
        sp_cell_ul_cfg_present = true;
        sp_cell_ul_cfg.id      = id;
        HANDLE_CODE(sp_cell_ul_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_ul_cfg.value.unpack(bref));
        break;
      }
      case 38: {
        drx_cycle_present = true;
        drx_cycle.id      = id;
        HANDLE_CODE(drx_cycle.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drx_cycle.value.unpack(bref));
        break;
      }
      case 9: {
        cu_to_du_rrc_info_present = true;
        cu_to_du_rrc_info.id      = id;
        HANDLE_CODE(cu_to_du_rrc_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cu_to_du_rrc_info.value.unpack(bref));
        break;
      }
      case 79: {
        tx_action_ind_present = true;
        tx_action_ind.id      = id;
        HANDLE_CODE(tx_action_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(tx_action_ind.value.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        res_coordination_transfer_container.id      = id;
        HANDLE_CODE(res_coordination_transfer_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.value.unpack(bref));
        break;
      }
      case 87: {
        rrc_recfg_complete_ind_present = true;
        rrc_recfg_complete_ind.id      = id;
        HANDLE_CODE(rrc_recfg_complete_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_recfg_complete_ind.value.unpack(bref));
        break;
      }
      case 50: {
        rrc_container_present = true;
        rrc_container.id      = id;
        HANDLE_CODE(rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.value.unpack(bref));
        break;
      }
      case 56: {
        scell_to_be_setup_mod_list_present = true;
        scell_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(scell_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scell_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 52: {
        scell_to_be_remd_list_present = true;
        scell_to_be_remd_list.id      = id;
        HANDLE_CODE(scell_to_be_remd_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scell_to_be_remd_list.value.unpack(bref));
        break;
      }
      case 76: {
        srbs_to_be_setup_mod_list_present = true;
        srbs_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(srbs_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 37: {
        drbs_to_be_setup_mod_list_present = true;
        drbs_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(drbs_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 31: {
        drbs_to_be_modified_list_present = true;
        drbs_to_be_modified_list.id      = id;
        HANDLE_CODE(drbs_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 72: {
        srbs_to_be_released_list_present = true;
        srbs_to_be_released_list.id      = id;
        HANDLE_CODE(srbs_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_to_be_released_list.value.unpack(bref));
        break;
      }
      case 33: {
        drbs_to_be_released_list_present = true;
        drbs_to_be_released_list.id      = id;
        HANDLE_CODE(drbs_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_to_be_released_list.value.unpack(bref));
        break;
      }
      case 97: {
        inactivity_monitoring_request_present = true;
        inactivity_monitoring_request.id      = id;
        HANDLE_CODE(inactivity_monitoring_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_request.value.unpack(bref));
        break;
      }
      case 108: {
        rat_freq_prio_info_present = true;
        rat_freq_prio_info.id      = id;
        HANDLE_CODE(rat_freq_prio_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rat_freq_prio_info.value.unpack(bref));
        break;
      }
      case 159: {
        drx_cfg_ind_present = true;
        drx_cfg_ind.id      = id;
        HANDLE_CODE(drx_cfg_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drx_cfg_ind.value.unpack(bref));
        break;
      }
      case 174: {
        rlc_fail_ind_present = true;
        rlc_fail_ind.id      = id;
        HANDLE_CODE(rlc_fail_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rlc_fail_ind.value.unpack(bref));
        break;
      }
      case 175: {
        ul_tx_direct_current_list_info_present = true;
        ul_tx_direct_current_list_info.id      = id;
        HANDLE_CODE(ul_tx_direct_current_list_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ul_tx_direct_current_list_info.value.unpack(bref));
        break;
      }
      case 162: {
        gnb_du_cfg_query_present = true;
        gnb_du_cfg_query.id      = id;
        HANDLE_CODE(gnb_du_cfg_query.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_cfg_query.value.unpack(bref));
        break;
      }
      case 158: {
        gnb_du_ue_ambr_ul_present = true;
        gnb_du_ue_ambr_ul.id      = id;
        HANDLE_CODE(gnb_du_ue_ambr_ul.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_ambr_ul.value.unpack(bref));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        execute_dupl.id      = id;
        HANDLE_CODE(execute_dupl.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.value.unpack(bref));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        rrc_delivery_status_request.id      = id;
        HANDLE_CODE(rrc_delivery_status_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.value.unpack(bref));
        break;
      }
      case 195: {
        res_coordination_transfer_info_present = true;
        res_coordination_transfer_info.id      = id;
        HANDLE_CODE(res_coordination_transfer_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_info.value.unpack(bref));
        break;
      }
      case 182: {
        serving_cell_mo_present = true;
        serving_cell_mo.id      = id;
        HANDLE_CODE(serving_cell_mo.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_cell_mo.value.unpack(bref));
        break;
      }
      case 215: {
        needfor_gap_present = true;
        needfor_gap.id      = id;
        HANDLE_CODE(needfor_gap.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(needfor_gap.value.unpack(bref));
        break;
      }
      case 94: {
        full_cfg_present = true;
        full_cfg.id      = id;
        HANDLE_CODE(full_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(full_cfg.value.unpack(bref));
        break;
      }
      case 248: {
        add_rrm_prio_idx_present = true;
        add_rrm_prio_idx.id      = id;
        HANDLE_CODE(add_rrm_prio_idx.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.value.unpack(bref));
        break;
      }
      case 253: {
        lower_layer_presence_status_change_present = true;
        lower_layer_presence_status_change.id      = id;
        HANDLE_CODE(lower_layer_presence_status_change.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(lower_layer_presence_status_change.value.unpack(bref));
        break;
      }
      case 267: {
        bh_chs_to_be_setup_mod_list_present = true;
        bh_chs_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(bh_chs_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 263: {
        bh_chs_to_be_modified_list_present = true;
        bh_chs_to_be_modified_list.id      = id;
        HANDLE_CODE(bh_chs_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 265: {
        bh_chs_to_be_released_list_present = true;
        bh_chs_to_be_released_list.id      = id;
        HANDLE_CODE(bh_chs_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_to_be_released_list.value.unpack(bref));
        break;
      }
      case 306: {
        nr_v2x_services_authorized_present = true;
        nr_v2x_services_authorized.id      = id;
        HANDLE_CODE(nr_v2x_services_authorized.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_v2x_services_authorized.value.unpack(bref));
        break;
      }
      case 307: {
        ltev2x_services_authorized_present = true;
        ltev2x_services_authorized.id      = id;
        HANDLE_CODE(ltev2x_services_authorized.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ltev2x_services_authorized.value.unpack(bref));
        break;
      }
      case 308: {
        nr_ue_sidelink_aggr_max_bitrate_present = true;
        nr_ue_sidelink_aggr_max_bitrate.id      = id;
        HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.value.unpack(bref));
        break;
      }
      case 309: {
        lte_ue_sidelink_aggr_max_bitrate_present = true;
        lte_ue_sidelink_aggr_max_bitrate.id      = id;
        HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.value.unpack(bref));
        break;
      }
      case 340: {
        pc5_link_ambr_present = true;
        pc5_link_ambr.id      = id;
        HANDLE_CODE(pc5_link_ambr.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_link_ambr.value.unpack(bref));
        break;
      }
      case 332: {
        sl_drbs_to_be_setup_mod_list_present = true;
        sl_drbs_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(sl_drbs_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 326: {
        sl_drbs_to_be_modified_list_present = true;
        sl_drbs_to_be_modified_list.id      = id;
        HANDLE_CODE(sl_drbs_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 328: {
        sl_drbs_to_be_released_list_present = true;
        sl_drbs_to_be_released_list.id      = id;
        HANDLE_CODE(sl_drbs_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_to_be_released_list.value.unpack(bref));
        break;
      }
      case 374: {
        conditional_intra_du_mob_info_present = true;
        conditional_intra_du_mob_info.id      = id;
        HANDLE_CODE(conditional_intra_du_mob_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(conditional_intra_du_mob_info.value.unpack(bref));
        break;
      }
      case 428: {
        f1_c_transfer_path_present = true;
        f1_c_transfer_path.id      = id;
        HANDLE_CODE(f1_c_transfer_path.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path.value.unpack(bref));
        break;
      }
      case 432: {
        scg_ind_present = true;
        scg_ind.id      = id;
        HANDLE_CODE(scg_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_ind.value.unpack(bref));
        break;
      }
      case 684: {
        ul_tx_direct_current_two_carrier_list_info_present = true;
        ul_tx_direct_current_two_carrier_list_info.id      = id;
        HANDLE_CODE(ul_tx_direct_current_two_carrier_list_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ul_tx_direct_current_two_carrier_list_info.value.unpack(bref));
        break;
      }
      case 510: {
        iab_conditional_rrc_msg_delivery_ind_present = true;
        iab_conditional_rrc_msg_delivery_ind.id      = id;
        HANDLE_CODE(iab_conditional_rrc_msg_delivery_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(iab_conditional_rrc_msg_delivery_ind.value.unpack(bref));
        break;
      }
      case 511: {
        f1_c_transfer_path_nr_dc_present = true;
        f1_c_transfer_path_nr_dc.id      = id;
        HANDLE_CODE(f1_c_transfer_path_nr_dc.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path_nr_dc.value.unpack(bref));
        break;
      }
      case 536: {
        mdt_polluted_meas_ind_present = true;
        mdt_polluted_meas_ind.id      = id;
        HANDLE_CODE(mdt_polluted_meas_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(mdt_polluted_meas_ind.value.unpack(bref));
        break;
      }
      case 547: {
        scg_activation_request_present = true;
        scg_activation_request.id      = id;
        HANDLE_CODE(scg_activation_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_request.value.unpack(bref));
        break;
      }
      case 586: {
        cg_sdt_query_ind_present = true;
        cg_sdt_query_ind.id      = id;
        HANDLE_CODE(cg_sdt_query_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cg_sdt_query_ind.value.unpack(bref));
        break;
      }
      case 594: {
        five_g_pro_se_authorized_present = true;
        five_g_pro_se_authorized.id      = id;
        HANDLE_CODE(five_g_pro_se_authorized.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_authorized.value.unpack(bref));
        break;
      }
      case 595: {
        five_g_pro_se_ue_pc5_aggr_max_bitrate_present = true;
        five_g_pro_se_ue_pc5_aggr_max_bitrate.id      = id;
        HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.value.unpack(bref));
        break;
      }
      case 596: {
        five_g_pro_se_pc5_link_ambr_present = true;
        five_g_pro_se_pc5_link_ambr.id      = id;
        HANDLE_CODE(five_g_pro_se_pc5_link_ambr.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_pc5_link_ambr.value.unpack(bref));
        break;
      }
      case 618: {
        upd_remote_ue_local_id_present = true;
        upd_remote_ue_local_id.id      = id;
        HANDLE_CODE(upd_remote_ue_local_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(upd_remote_ue_local_id.value.unpack(bref));
        break;
      }
      case 599: {
        uu_rlc_ch_to_be_setup_list_present = true;
        uu_rlc_ch_to_be_setup_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 600: {
        uu_rlc_ch_to_be_modified_list_present = true;
        uu_rlc_ch_to_be_modified_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 601: {
        uu_rlc_ch_to_be_released_list_present = true;
        uu_rlc_ch_to_be_released_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_to_be_released_list.value.unpack(bref));
        break;
      }
      case 608: {
        pc5_rlc_ch_to_be_setup_list_present = true;
        pc5_rlc_ch_to_be_setup_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 609: {
        pc5_rlc_ch_to_be_modified_list_present = true;
        pc5_rlc_ch_to_be_modified_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 610: {
        pc5_rlc_ch_to_be_released_list_present = true;
        pc5_rlc_ch_to_be_released_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_to_be_released_list.value.unpack(bref));
        break;
      }
      case 619: {
        path_switch_cfg_present = true;
        path_switch_cfg.id      = id;
        HANDLE_CODE(path_switch_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(path_switch_cfg.value.unpack(bref));
        break;
      }
      case 626: {
        gnb_du_ue_slice_max_bit_rate_list_present = true;
        gnb_du_ue_slice_max_bit_rate_list.id      = id;
        HANDLE_CODE(gnb_du_ue_slice_max_bit_rate_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_slice_max_bit_rate_list.value.unpack(bref));
        break;
      }
      case 632: {
        multicast_mbs_session_setup_list_present = true;
        multicast_mbs_session_setup_list.id      = id;
        HANDLE_CODE(multicast_mbs_session_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(multicast_mbs_session_setup_list.value.unpack(bref));
        break;
      }
      case 633: {
        multicast_mbs_session_rem_list_present = true;
        multicast_mbs_session_rem_list.id      = id;
        HANDLE_CODE(multicast_mbs_session_rem_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(multicast_mbs_session_rem_list.value.unpack(bref));
        break;
      }
      case 685: {
        ue_multicast_m_rbs_to_be_setup_at_modify_list_present = true;
        ue_multicast_m_rbs_to_be_setup_at_modify_list.id      = id;
        HANDLE_CODE(ue_multicast_m_rbs_to_be_setup_at_modify_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_multicast_m_rbs_to_be_setup_at_modify_list.value.unpack(bref));
        break;
      }
      case 628: {
        ue_multicast_m_rbs_to_be_released_list_present = true;
        ue_multicast_m_rbs_to_be_released_list.id      = id;
        HANDLE_CODE(ue_multicast_m_rbs_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_multicast_m_rbs_to_be_released_list.value.unpack(bref));
        break;
      }
      case 643: {
        sldrx_cycle_list_present = true;
        sldrx_cycle_list.id      = id;
        HANDLE_CODE(sldrx_cycle_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sldrx_cycle_list.value.unpack(bref));
        break;
      }
      case 647: {
        management_based_mdt_plmn_mod_list_present = true;
        management_based_mdt_plmn_mod_list.id      = id;
        HANDLE_CODE(management_based_mdt_plmn_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(management_based_mdt_plmn_mod_list.value.unpack(bref));
        break;
      }
      case 675: {
        sdt_bearer_cfg_query_ind_present = true;
        sdt_bearer_cfg_query_ind.id      = id;
        HANDLE_CODE(sdt_bearer_cfg_query_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_bearer_cfg_query_ind.value.unpack(bref));
        break;
      }
      case 683: {
        daps_ho_status_present = true;
        daps_ho_status.id      = id;
        HANDLE_CODE(daps_ho_status.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(daps_ho_status.value.unpack(bref));
        break;
      }
      case 695: {
        serving_cell_mo_list_present = true;
        serving_cell_mo_list.id      = id;
        HANDLE_CODE(serving_cell_mo_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_cell_mo_list.value.unpack(bref));
        break;
      }
      case 690: {
        ul_tx_direct_current_more_carrier_info_present = true;
        ul_tx_direct_current_more_carrier_info.id      = id;
        HANDLE_CODE(ul_tx_direct_current_more_carrier_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ul_tx_direct_current_more_carrier_info.value.unpack(bref));
        break;
      }
      case 691: {
        cp_acmcg_info_present = true;
        cp_acmcg_info.id      = id;
        HANDLE_CODE(cp_acmcg_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cp_acmcg_info.value.unpack(bref));
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
void ue_context_mod_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  if (sp_cell_id_present) {
    j.write_fieldname("");
    sp_cell_id.to_json(j);
  }
  if (serv_cell_idx_present) {
    j.write_fieldname("");
    serv_cell_idx.to_json(j);
  }
  if (sp_cell_ul_cfg_present) {
    j.write_fieldname("");
    sp_cell_ul_cfg.to_json(j);
  }
  if (drx_cycle_present) {
    j.write_fieldname("");
    drx_cycle.to_json(j);
  }
  if (cu_to_du_rrc_info_present) {
    j.write_fieldname("");
    cu_to_du_rrc_info.to_json(j);
  }
  if (tx_action_ind_present) {
    j.write_fieldname("");
    tx_action_ind.to_json(j);
  }
  if (res_coordination_transfer_container_present) {
    j.write_fieldname("");
    res_coordination_transfer_container.to_json(j);
  }
  if (rrc_recfg_complete_ind_present) {
    j.write_fieldname("");
    rrc_recfg_complete_ind.to_json(j);
  }
  if (rrc_container_present) {
    j.write_fieldname("");
    rrc_container.to_json(j);
  }
  if (scell_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    scell_to_be_setup_mod_list.to_json(j);
  }
  if (scell_to_be_remd_list_present) {
    j.write_fieldname("");
    scell_to_be_remd_list.to_json(j);
  }
  if (srbs_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    srbs_to_be_setup_mod_list.to_json(j);
  }
  if (drbs_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    drbs_to_be_setup_mod_list.to_json(j);
  }
  if (drbs_to_be_modified_list_present) {
    j.write_fieldname("");
    drbs_to_be_modified_list.to_json(j);
  }
  if (srbs_to_be_released_list_present) {
    j.write_fieldname("");
    srbs_to_be_released_list.to_json(j);
  }
  if (drbs_to_be_released_list_present) {
    j.write_fieldname("");
    drbs_to_be_released_list.to_json(j);
  }
  if (inactivity_monitoring_request_present) {
    j.write_fieldname("");
    inactivity_monitoring_request.to_json(j);
  }
  if (rat_freq_prio_info_present) {
    j.write_fieldname("");
    rat_freq_prio_info.to_json(j);
  }
  if (drx_cfg_ind_present) {
    j.write_fieldname("");
    drx_cfg_ind.to_json(j);
  }
  if (rlc_fail_ind_present) {
    j.write_fieldname("");
    rlc_fail_ind.to_json(j);
  }
  if (ul_tx_direct_current_list_info_present) {
    j.write_fieldname("");
    ul_tx_direct_current_list_info.to_json(j);
  }
  if (gnb_du_cfg_query_present) {
    j.write_fieldname("");
    gnb_du_cfg_query.to_json(j);
  }
  if (gnb_du_ue_ambr_ul_present) {
    j.write_fieldname("");
    gnb_du_ue_ambr_ul.to_json(j);
  }
  if (execute_dupl_present) {
    j.write_fieldname("");
    execute_dupl.to_json(j);
  }
  if (rrc_delivery_status_request_present) {
    j.write_fieldname("");
    rrc_delivery_status_request.to_json(j);
  }
  if (res_coordination_transfer_info_present) {
    j.write_fieldname("");
    res_coordination_transfer_info.to_json(j);
  }
  if (serving_cell_mo_present) {
    j.write_fieldname("");
    serving_cell_mo.to_json(j);
  }
  if (needfor_gap_present) {
    j.write_fieldname("");
    needfor_gap.to_json(j);
  }
  if (full_cfg_present) {
    j.write_fieldname("");
    full_cfg.to_json(j);
  }
  if (add_rrm_prio_idx_present) {
    j.write_fieldname("");
    add_rrm_prio_idx.to_json(j);
  }
  if (lower_layer_presence_status_change_present) {
    j.write_fieldname("");
    lower_layer_presence_status_change.to_json(j);
  }
  if (bh_chs_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    bh_chs_to_be_setup_mod_list.to_json(j);
  }
  if (bh_chs_to_be_modified_list_present) {
    j.write_fieldname("");
    bh_chs_to_be_modified_list.to_json(j);
  }
  if (bh_chs_to_be_released_list_present) {
    j.write_fieldname("");
    bh_chs_to_be_released_list.to_json(j);
  }
  if (nr_v2x_services_authorized_present) {
    j.write_fieldname("");
    nr_v2x_services_authorized.to_json(j);
  }
  if (ltev2x_services_authorized_present) {
    j.write_fieldname("");
    ltev2x_services_authorized.to_json(j);
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    j.write_fieldname("");
    nr_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    j.write_fieldname("");
    lte_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (pc5_link_ambr_present) {
    j.write_fieldname("");
    pc5_link_ambr.to_json(j);
  }
  if (sl_drbs_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    sl_drbs_to_be_setup_mod_list.to_json(j);
  }
  if (sl_drbs_to_be_modified_list_present) {
    j.write_fieldname("");
    sl_drbs_to_be_modified_list.to_json(j);
  }
  if (sl_drbs_to_be_released_list_present) {
    j.write_fieldname("");
    sl_drbs_to_be_released_list.to_json(j);
  }
  if (conditional_intra_du_mob_info_present) {
    j.write_fieldname("");
    conditional_intra_du_mob_info.to_json(j);
  }
  if (f1_c_transfer_path_present) {
    j.write_fieldname("");
    f1_c_transfer_path.to_json(j);
  }
  if (scg_ind_present) {
    j.write_fieldname("");
    scg_ind.to_json(j);
  }
  if (ul_tx_direct_current_two_carrier_list_info_present) {
    j.write_fieldname("");
    ul_tx_direct_current_two_carrier_list_info.to_json(j);
  }
  if (iab_conditional_rrc_msg_delivery_ind_present) {
    j.write_fieldname("");
    iab_conditional_rrc_msg_delivery_ind.to_json(j);
  }
  if (f1_c_transfer_path_nr_dc_present) {
    j.write_fieldname("");
    f1_c_transfer_path_nr_dc.to_json(j);
  }
  if (mdt_polluted_meas_ind_present) {
    j.write_fieldname("");
    mdt_polluted_meas_ind.to_json(j);
  }
  if (scg_activation_request_present) {
    j.write_fieldname("");
    scg_activation_request.to_json(j);
  }
  if (cg_sdt_query_ind_present) {
    j.write_fieldname("");
    cg_sdt_query_ind.to_json(j);
  }
  if (five_g_pro_se_authorized_present) {
    j.write_fieldname("");
    five_g_pro_se_authorized.to_json(j);
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    j.write_fieldname("");
    five_g_pro_se_ue_pc5_aggr_max_bitrate.to_json(j);
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    j.write_fieldname("");
    five_g_pro_se_pc5_link_ambr.to_json(j);
  }
  if (upd_remote_ue_local_id_present) {
    j.write_fieldname("");
    upd_remote_ue_local_id.to_json(j);
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_to_be_setup_list.to_json(j);
  }
  if (uu_rlc_ch_to_be_modified_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_to_be_modified_list.to_json(j);
  }
  if (uu_rlc_ch_to_be_released_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_to_be_released_list.to_json(j);
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_to_be_setup_list.to_json(j);
  }
  if (pc5_rlc_ch_to_be_modified_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_to_be_modified_list.to_json(j);
  }
  if (pc5_rlc_ch_to_be_released_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_to_be_released_list.to_json(j);
  }
  if (path_switch_cfg_present) {
    j.write_fieldname("");
    path_switch_cfg.to_json(j);
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    j.write_fieldname("");
    gnb_du_ue_slice_max_bit_rate_list.to_json(j);
  }
  if (multicast_mbs_session_setup_list_present) {
    j.write_fieldname("");
    multicast_mbs_session_setup_list.to_json(j);
  }
  if (multicast_mbs_session_rem_list_present) {
    j.write_fieldname("");
    multicast_mbs_session_rem_list.to_json(j);
  }
  if (ue_multicast_m_rbs_to_be_setup_at_modify_list_present) {
    j.write_fieldname("");
    ue_multicast_m_rbs_to_be_setup_at_modify_list.to_json(j);
  }
  if (ue_multicast_m_rbs_to_be_released_list_present) {
    j.write_fieldname("");
    ue_multicast_m_rbs_to_be_released_list.to_json(j);
  }
  if (sldrx_cycle_list_present) {
    j.write_fieldname("");
    sldrx_cycle_list.to_json(j);
  }
  if (management_based_mdt_plmn_mod_list_present) {
    j.write_fieldname("");
    management_based_mdt_plmn_mod_list.to_json(j);
  }
  if (sdt_bearer_cfg_query_ind_present) {
    j.write_fieldname("");
    sdt_bearer_cfg_query_ind.to_json(j);
  }
  if (daps_ho_status_present) {
    j.write_fieldname("");
    daps_ho_status.to_json(j);
  }
  if (serving_cell_mo_list_present) {
    j.write_fieldname("");
    serving_cell_mo_list.to_json(j);
  }
  if (ul_tx_direct_current_more_carrier_info_present) {
    j.write_fieldname("");
    ul_tx_direct_current_more_carrier_info.to_json(j);
  }
  if (cp_acmcg_info_present) {
    j.write_fieldname("");
    cp_acmcg_info.to_json(j);
  }
  j.end_obj();
}

ue_context_mod_required_ies_container::ue_context_mod_required_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  res_coordination_transfer_container(49, crit_e::ignore),
  du_to_cu_rrc_info(39, crit_e::reject),
  drbs_required_to_be_modified_list(23, crit_e::reject),
  srbs_required_to_be_released_list(70, crit_e::reject),
  drbs_required_to_be_released_list(25, crit_e::reject),
  cause(0, crit_e::ignore),
  bh_chs_required_to_be_released_list(277, crit_e::reject),
  sl_drbs_required_to_be_modified_list(320, crit_e::reject),
  sl_drbs_required_to_be_released_list(322, crit_e::reject),
  target_cells_to_cancel(375, crit_e::reject),
  uu_rlc_ch_required_to_be_modified_list(606, crit_e::reject),
  uu_rlc_ch_required_to_be_released_list(607, crit_e::reject),
  pc5_rlc_ch_required_to_be_modified_list(614, crit_e::reject),
  pc5_rlc_ch_required_to_be_released_list(615, crit_e::reject),
  ue_multicast_m_rbs_required_to_be_modified_list(655, crit_e::reject),
  ue_multicast_m_rbs_required_to_be_released_list(657, crit_e::reject)
{
}
SRSASN_CODE ue_context_mod_required_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += res_coordination_transfer_container_present ? 1 : 0;
  nof_ies += du_to_cu_rrc_info_present ? 1 : 0;
  nof_ies += drbs_required_to_be_modified_list_present ? 1 : 0;
  nof_ies += srbs_required_to_be_released_list_present ? 1 : 0;
  nof_ies += drbs_required_to_be_released_list_present ? 1 : 0;
  nof_ies += bh_chs_required_to_be_released_list_present ? 1 : 0;
  nof_ies += sl_drbs_required_to_be_modified_list_present ? 1 : 0;
  nof_ies += sl_drbs_required_to_be_released_list_present ? 1 : 0;
  nof_ies += target_cells_to_cancel_present ? 1 : 0;
  nof_ies += uu_rlc_ch_required_to_be_modified_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_required_to_be_released_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_required_to_be_modified_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_required_to_be_released_list_present ? 1 : 0;
  nof_ies += ue_multicast_m_rbs_required_to_be_modified_list_present ? 1 : 0;
  nof_ies += ue_multicast_m_rbs_required_to_be_released_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (du_to_cu_rrc_info_present) {
    HANDLE_CODE(du_to_cu_rrc_info.pack(bref));
  }
  if (drbs_required_to_be_modified_list_present) {
    HANDLE_CODE(drbs_required_to_be_modified_list.pack(bref));
  }
  if (srbs_required_to_be_released_list_present) {
    HANDLE_CODE(srbs_required_to_be_released_list.pack(bref));
  }
  if (drbs_required_to_be_released_list_present) {
    HANDLE_CODE(drbs_required_to_be_released_list.pack(bref));
  }
  HANDLE_CODE(cause.pack(bref));
  if (bh_chs_required_to_be_released_list_present) {
    HANDLE_CODE(bh_chs_required_to_be_released_list.pack(bref));
  }
  if (sl_drbs_required_to_be_modified_list_present) {
    HANDLE_CODE(sl_drbs_required_to_be_modified_list.pack(bref));
  }
  if (sl_drbs_required_to_be_released_list_present) {
    HANDLE_CODE(sl_drbs_required_to_be_released_list.pack(bref));
  }
  if (target_cells_to_cancel_present) {
    HANDLE_CODE(target_cells_to_cancel.pack(bref));
  }
  if (uu_rlc_ch_required_to_be_modified_list_present) {
    HANDLE_CODE(uu_rlc_ch_required_to_be_modified_list.pack(bref));
  }
  if (uu_rlc_ch_required_to_be_released_list_present) {
    HANDLE_CODE(uu_rlc_ch_required_to_be_released_list.pack(bref));
  }
  if (pc5_rlc_ch_required_to_be_modified_list_present) {
    HANDLE_CODE(pc5_rlc_ch_required_to_be_modified_list.pack(bref));
  }
  if (pc5_rlc_ch_required_to_be_released_list_present) {
    HANDLE_CODE(pc5_rlc_ch_required_to_be_released_list.pack(bref));
  }
  if (ue_multicast_m_rbs_required_to_be_modified_list_present) {
    HANDLE_CODE(ue_multicast_m_rbs_required_to_be_modified_list.pack(bref));
  }
  if (ue_multicast_m_rbs_required_to_be_released_list_present) {
    HANDLE_CODE(ue_multicast_m_rbs_required_to_be_released_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_required_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        res_coordination_transfer_container.id      = id;
        HANDLE_CODE(res_coordination_transfer_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.value.unpack(bref));
        break;
      }
      case 39: {
        du_to_cu_rrc_info_present = true;
        du_to_cu_rrc_info.id      = id;
        HANDLE_CODE(du_to_cu_rrc_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_info.value.unpack(bref));
        break;
      }
      case 23: {
        drbs_required_to_be_modified_list_present = true;
        drbs_required_to_be_modified_list.id      = id;
        HANDLE_CODE(drbs_required_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_required_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 70: {
        srbs_required_to_be_released_list_present = true;
        srbs_required_to_be_released_list.id      = id;
        HANDLE_CODE(srbs_required_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_required_to_be_released_list.value.unpack(bref));
        break;
      }
      case 25: {
        drbs_required_to_be_released_list_present = true;
        drbs_required_to_be_released_list.id      = id;
        HANDLE_CODE(drbs_required_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_required_to_be_released_list.value.unpack(bref));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 277: {
        bh_chs_required_to_be_released_list_present = true;
        bh_chs_required_to_be_released_list.id      = id;
        HANDLE_CODE(bh_chs_required_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_required_to_be_released_list.value.unpack(bref));
        break;
      }
      case 320: {
        sl_drbs_required_to_be_modified_list_present = true;
        sl_drbs_required_to_be_modified_list.id      = id;
        HANDLE_CODE(sl_drbs_required_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_required_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 322: {
        sl_drbs_required_to_be_released_list_present = true;
        sl_drbs_required_to_be_released_list.id      = id;
        HANDLE_CODE(sl_drbs_required_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_required_to_be_released_list.value.unpack(bref));
        break;
      }
      case 375: {
        target_cells_to_cancel_present = true;
        target_cells_to_cancel.id      = id;
        HANDLE_CODE(target_cells_to_cancel.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(target_cells_to_cancel.value.unpack(bref));
        break;
      }
      case 606: {
        uu_rlc_ch_required_to_be_modified_list_present = true;
        uu_rlc_ch_required_to_be_modified_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_required_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_required_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 607: {
        uu_rlc_ch_required_to_be_released_list_present = true;
        uu_rlc_ch_required_to_be_released_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_required_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_required_to_be_released_list.value.unpack(bref));
        break;
      }
      case 614: {
        pc5_rlc_ch_required_to_be_modified_list_present = true;
        pc5_rlc_ch_required_to_be_modified_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_required_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_required_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 615: {
        pc5_rlc_ch_required_to_be_released_list_present = true;
        pc5_rlc_ch_required_to_be_released_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_required_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_required_to_be_released_list.value.unpack(bref));
        break;
      }
      case 655: {
        ue_multicast_m_rbs_required_to_be_modified_list_present = true;
        ue_multicast_m_rbs_required_to_be_modified_list.id      = id;
        HANDLE_CODE(ue_multicast_m_rbs_required_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_multicast_m_rbs_required_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 657: {
        ue_multicast_m_rbs_required_to_be_released_list_present = true;
        ue_multicast_m_rbs_required_to_be_released_list.id      = id;
        HANDLE_CODE(ue_multicast_m_rbs_required_to_be_released_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_multicast_m_rbs_required_to_be_released_list.value.unpack(bref));
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
void ue_context_mod_required_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  if (res_coordination_transfer_container_present) {
    j.write_fieldname("");
    res_coordination_transfer_container.to_json(j);
  }
  if (du_to_cu_rrc_info_present) {
    j.write_fieldname("");
    du_to_cu_rrc_info.to_json(j);
  }
  if (drbs_required_to_be_modified_list_present) {
    j.write_fieldname("");
    drbs_required_to_be_modified_list.to_json(j);
  }
  if (srbs_required_to_be_released_list_present) {
    j.write_fieldname("");
    srbs_required_to_be_released_list.to_json(j);
  }
  if (drbs_required_to_be_released_list_present) {
    j.write_fieldname("");
    drbs_required_to_be_released_list.to_json(j);
  }
  j.write_fieldname("");
  cause.to_json(j);
  if (bh_chs_required_to_be_released_list_present) {
    j.write_fieldname("");
    bh_chs_required_to_be_released_list.to_json(j);
  }
  if (sl_drbs_required_to_be_modified_list_present) {
    j.write_fieldname("");
    sl_drbs_required_to_be_modified_list.to_json(j);
  }
  if (sl_drbs_required_to_be_released_list_present) {
    j.write_fieldname("");
    sl_drbs_required_to_be_released_list.to_json(j);
  }
  if (target_cells_to_cancel_present) {
    j.write_fieldname("");
    target_cells_to_cancel.to_json(j);
  }
  if (uu_rlc_ch_required_to_be_modified_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_required_to_be_modified_list.to_json(j);
  }
  if (uu_rlc_ch_required_to_be_released_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_required_to_be_released_list.to_json(j);
  }
  if (pc5_rlc_ch_required_to_be_modified_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_required_to_be_modified_list.to_json(j);
  }
  if (pc5_rlc_ch_required_to_be_released_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_required_to_be_released_list.to_json(j);
  }
  if (ue_multicast_m_rbs_required_to_be_modified_list_present) {
    j.write_fieldname("");
    ue_multicast_m_rbs_required_to_be_modified_list.to_json(j);
  }
  if (ue_multicast_m_rbs_required_to_be_released_list_present) {
    j.write_fieldname("");
    ue_multicast_m_rbs_required_to_be_released_list.to_json(j);
  }
  j.end_obj();
}

ue_context_mod_resp_ies_container::ue_context_mod_resp_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  res_coordination_transfer_container(49, crit_e::ignore),
  du_to_cu_rrc_info(39, crit_e::reject),
  drbs_setup_mod_list(29, crit_e::ignore),
  drbs_modified_list(21, crit_e::ignore),
  srbs_failed_to_be_setup_mod_list(68, crit_e::ignore),
  drbs_failed_to_be_setup_mod_list(17, crit_e::ignore),
  scell_failedto_setup_mod_list(85, crit_e::ignore),
  drbs_failed_to_be_modified_list(13, crit_e::ignore),
  inactivity_monitoring_resp(98, crit_e::reject),
  crit_diagnostics(7, crit_e::ignore),
  c_rnti(95, crit_e::ignore),
  associated_scell_list(198, crit_e::ignore),
  srbs_setup_mod_list(204, crit_e::ignore),
  srbs_modified_list(206, crit_e::ignore),
  full_cfg(94, crit_e::reject),
  bh_chs_setup_mod_list(275, crit_e::ignore),
  bh_chs_modified_list(273, crit_e::ignore),
  bh_chs_failed_to_be_setup_mod_list(271, crit_e::ignore),
  bh_chs_failed_to_be_modified_list(269, crit_e::ignore),
  sl_drbs_setup_mod_list(333, crit_e::ignore),
  sl_drbs_modified_list(318, crit_e::ignore),
  sl_drbs_failed_to_be_setup_mod_list(334, crit_e::ignore),
  sl_drbs_failed_to_be_modified_list(314, crit_e::ignore),
  requested_target_cell_global_id(376, crit_e::reject),
  scg_activation_status(548, crit_e::ignore),
  uu_rlc_ch_setup_list(602, crit_e::ignore),
  uu_rlc_ch_failed_to_be_setup_list(603, crit_e::ignore),
  uu_rlc_ch_modified_list(604, crit_e::ignore),
  uu_rlc_ch_failed_to_be_modified_list(605, crit_e::ignore),
  pc5_rlc_ch_setup_list(611, crit_e::ignore),
  pc5_rlc_ch_failed_to_be_setup_list(612, crit_e::ignore),
  pc5_rlc_ch_modified_list(616, crit_e::ignore),
  pc5_rlc_ch_failed_to_be_modified_list(613, crit_e::ignore),
  sdt_bearer_cfg_info(676, crit_e::ignore),
  ue_multicast_m_rbs_setup_list(679, crit_e::reject),
  serving_cell_mo_encoded_in_cgc_list(697, crit_e::ignore)
{
}
SRSASN_CODE ue_context_mod_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += res_coordination_transfer_container_present ? 1 : 0;
  nof_ies += du_to_cu_rrc_info_present ? 1 : 0;
  nof_ies += drbs_setup_mod_list_present ? 1 : 0;
  nof_ies += drbs_modified_list_present ? 1 : 0;
  nof_ies += srbs_failed_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += drbs_failed_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += scell_failedto_setup_mod_list_present ? 1 : 0;
  nof_ies += drbs_failed_to_be_modified_list_present ? 1 : 0;
  nof_ies += inactivity_monitoring_resp_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += c_rnti_present ? 1 : 0;
  nof_ies += associated_scell_list_present ? 1 : 0;
  nof_ies += srbs_setup_mod_list_present ? 1 : 0;
  nof_ies += srbs_modified_list_present ? 1 : 0;
  nof_ies += full_cfg_present ? 1 : 0;
  nof_ies += bh_chs_setup_mod_list_present ? 1 : 0;
  nof_ies += bh_chs_modified_list_present ? 1 : 0;
  nof_ies += bh_chs_failed_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += bh_chs_failed_to_be_modified_list_present ? 1 : 0;
  nof_ies += sl_drbs_setup_mod_list_present ? 1 : 0;
  nof_ies += sl_drbs_modified_list_present ? 1 : 0;
  nof_ies += sl_drbs_failed_to_be_setup_mod_list_present ? 1 : 0;
  nof_ies += sl_drbs_failed_to_be_modified_list_present ? 1 : 0;
  nof_ies += requested_target_cell_global_id_present ? 1 : 0;
  nof_ies += scg_activation_status_present ? 1 : 0;
  nof_ies += uu_rlc_ch_setup_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_modified_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_failed_to_be_modified_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_setup_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_modified_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_failed_to_be_modified_list_present ? 1 : 0;
  nof_ies += sdt_bearer_cfg_info_present ? 1 : 0;
  nof_ies += ue_multicast_m_rbs_setup_list_present ? 1 : 0;
  nof_ies += serving_cell_mo_encoded_in_cgc_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (du_to_cu_rrc_info_present) {
    HANDLE_CODE(du_to_cu_rrc_info.pack(bref));
  }
  if (drbs_setup_mod_list_present) {
    HANDLE_CODE(drbs_setup_mod_list.pack(bref));
  }
  if (drbs_modified_list_present) {
    HANDLE_CODE(drbs_modified_list.pack(bref));
  }
  if (srbs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(srbs_failed_to_be_setup_mod_list.pack(bref));
  }
  if (drbs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(drbs_failed_to_be_setup_mod_list.pack(bref));
  }
  if (scell_failedto_setup_mod_list_present) {
    HANDLE_CODE(scell_failedto_setup_mod_list.pack(bref));
  }
  if (drbs_failed_to_be_modified_list_present) {
    HANDLE_CODE(drbs_failed_to_be_modified_list.pack(bref));
  }
  if (inactivity_monitoring_resp_present) {
    HANDLE_CODE(inactivity_monitoring_resp.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (c_rnti_present) {
    HANDLE_CODE(c_rnti.pack(bref));
  }
  if (associated_scell_list_present) {
    HANDLE_CODE(associated_scell_list.pack(bref));
  }
  if (srbs_setup_mod_list_present) {
    HANDLE_CODE(srbs_setup_mod_list.pack(bref));
  }
  if (srbs_modified_list_present) {
    HANDLE_CODE(srbs_modified_list.pack(bref));
  }
  if (full_cfg_present) {
    HANDLE_CODE(full_cfg.pack(bref));
  }
  if (bh_chs_setup_mod_list_present) {
    HANDLE_CODE(bh_chs_setup_mod_list.pack(bref));
  }
  if (bh_chs_modified_list_present) {
    HANDLE_CODE(bh_chs_modified_list.pack(bref));
  }
  if (bh_chs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(bh_chs_failed_to_be_setup_mod_list.pack(bref));
  }
  if (bh_chs_failed_to_be_modified_list_present) {
    HANDLE_CODE(bh_chs_failed_to_be_modified_list.pack(bref));
  }
  if (sl_drbs_setup_mod_list_present) {
    HANDLE_CODE(sl_drbs_setup_mod_list.pack(bref));
  }
  if (sl_drbs_modified_list_present) {
    HANDLE_CODE(sl_drbs_modified_list.pack(bref));
  }
  if (sl_drbs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(sl_drbs_failed_to_be_setup_mod_list.pack(bref));
  }
  if (sl_drbs_failed_to_be_modified_list_present) {
    HANDLE_CODE(sl_drbs_failed_to_be_modified_list.pack(bref));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(requested_target_cell_global_id.pack(bref));
  }
  if (scg_activation_status_present) {
    HANDLE_CODE(scg_activation_status.pack(bref));
  }
  if (uu_rlc_ch_setup_list_present) {
    HANDLE_CODE(uu_rlc_ch_setup_list.pack(bref));
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(uu_rlc_ch_failed_to_be_setup_list.pack(bref));
  }
  if (uu_rlc_ch_modified_list_present) {
    HANDLE_CODE(uu_rlc_ch_modified_list.pack(bref));
  }
  if (uu_rlc_ch_failed_to_be_modified_list_present) {
    HANDLE_CODE(uu_rlc_ch_failed_to_be_modified_list.pack(bref));
  }
  if (pc5_rlc_ch_setup_list_present) {
    HANDLE_CODE(pc5_rlc_ch_setup_list.pack(bref));
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(pc5_rlc_ch_failed_to_be_setup_list.pack(bref));
  }
  if (pc5_rlc_ch_modified_list_present) {
    HANDLE_CODE(pc5_rlc_ch_modified_list.pack(bref));
  }
  if (pc5_rlc_ch_failed_to_be_modified_list_present) {
    HANDLE_CODE(pc5_rlc_ch_failed_to_be_modified_list.pack(bref));
  }
  if (sdt_bearer_cfg_info_present) {
    HANDLE_CODE(sdt_bearer_cfg_info.pack(bref));
  }
  if (ue_multicast_m_rbs_setup_list_present) {
    HANDLE_CODE(ue_multicast_m_rbs_setup_list.pack(bref));
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    HANDLE_CODE(serving_cell_mo_encoded_in_cgc_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_mod_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        res_coordination_transfer_container.id      = id;
        HANDLE_CODE(res_coordination_transfer_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.value.unpack(bref));
        break;
      }
      case 39: {
        du_to_cu_rrc_info_present = true;
        du_to_cu_rrc_info.id      = id;
        HANDLE_CODE(du_to_cu_rrc_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_info.value.unpack(bref));
        break;
      }
      case 29: {
        drbs_setup_mod_list_present = true;
        drbs_setup_mod_list.id      = id;
        HANDLE_CODE(drbs_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_setup_mod_list.value.unpack(bref));
        break;
      }
      case 21: {
        drbs_modified_list_present = true;
        drbs_modified_list.id      = id;
        HANDLE_CODE(drbs_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_modified_list.value.unpack(bref));
        break;
      }
      case 68: {
        srbs_failed_to_be_setup_mod_list_present = true;
        srbs_failed_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(srbs_failed_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_failed_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 17: {
        drbs_failed_to_be_setup_mod_list_present = true;
        drbs_failed_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(drbs_failed_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_failed_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 85: {
        scell_failedto_setup_mod_list_present = true;
        scell_failedto_setup_mod_list.id      = id;
        HANDLE_CODE(scell_failedto_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scell_failedto_setup_mod_list.value.unpack(bref));
        break;
      }
      case 13: {
        drbs_failed_to_be_modified_list_present = true;
        drbs_failed_to_be_modified_list.id      = id;
        HANDLE_CODE(drbs_failed_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_failed_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 98: {
        inactivity_monitoring_resp_present = true;
        inactivity_monitoring_resp.id      = id;
        HANDLE_CODE(inactivity_monitoring_resp.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_resp.value.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      case 95: {
        c_rnti_present = true;
        c_rnti.id      = id;
        HANDLE_CODE(c_rnti.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(c_rnti.value.unpack(bref));
        break;
      }
      case 198: {
        associated_scell_list_present = true;
        associated_scell_list.id      = id;
        HANDLE_CODE(associated_scell_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(associated_scell_list.value.unpack(bref));
        break;
      }
      case 204: {
        srbs_setup_mod_list_present = true;
        srbs_setup_mod_list.id      = id;
        HANDLE_CODE(srbs_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_setup_mod_list.value.unpack(bref));
        break;
      }
      case 206: {
        srbs_modified_list_present = true;
        srbs_modified_list.id      = id;
        HANDLE_CODE(srbs_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_modified_list.value.unpack(bref));
        break;
      }
      case 94: {
        full_cfg_present = true;
        full_cfg.id      = id;
        HANDLE_CODE(full_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(full_cfg.value.unpack(bref));
        break;
      }
      case 275: {
        bh_chs_setup_mod_list_present = true;
        bh_chs_setup_mod_list.id      = id;
        HANDLE_CODE(bh_chs_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_setup_mod_list.value.unpack(bref));
        break;
      }
      case 273: {
        bh_chs_modified_list_present = true;
        bh_chs_modified_list.id      = id;
        HANDLE_CODE(bh_chs_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_modified_list.value.unpack(bref));
        break;
      }
      case 271: {
        bh_chs_failed_to_be_setup_mod_list_present = true;
        bh_chs_failed_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(bh_chs_failed_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_failed_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 269: {
        bh_chs_failed_to_be_modified_list_present = true;
        bh_chs_failed_to_be_modified_list.id      = id;
        HANDLE_CODE(bh_chs_failed_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_failed_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 333: {
        sl_drbs_setup_mod_list_present = true;
        sl_drbs_setup_mod_list.id      = id;
        HANDLE_CODE(sl_drbs_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_setup_mod_list.value.unpack(bref));
        break;
      }
      case 318: {
        sl_drbs_modified_list_present = true;
        sl_drbs_modified_list.id      = id;
        HANDLE_CODE(sl_drbs_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_modified_list.value.unpack(bref));
        break;
      }
      case 334: {
        sl_drbs_failed_to_be_setup_mod_list_present = true;
        sl_drbs_failed_to_be_setup_mod_list.id      = id;
        HANDLE_CODE(sl_drbs_failed_to_be_setup_mod_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_failed_to_be_setup_mod_list.value.unpack(bref));
        break;
      }
      case 314: {
        sl_drbs_failed_to_be_modified_list_present = true;
        sl_drbs_failed_to_be_modified_list.id      = id;
        HANDLE_CODE(sl_drbs_failed_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_failed_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        requested_target_cell_global_id.id      = id;
        HANDLE_CODE(requested_target_cell_global_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.value.unpack(bref));
        break;
      }
      case 548: {
        scg_activation_status_present = true;
        scg_activation_status.id      = id;
        HANDLE_CODE(scg_activation_status.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_status.value.unpack(bref));
        break;
      }
      case 602: {
        uu_rlc_ch_setup_list_present = true;
        uu_rlc_ch_setup_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_setup_list.value.unpack(bref));
        break;
      }
      case 603: {
        uu_rlc_ch_failed_to_be_setup_list_present = true;
        uu_rlc_ch_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 604: {
        uu_rlc_ch_modified_list_present = true;
        uu_rlc_ch_modified_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_modified_list.value.unpack(bref));
        break;
      }
      case 605: {
        uu_rlc_ch_failed_to_be_modified_list_present = true;
        uu_rlc_ch_failed_to_be_modified_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_failed_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_failed_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 611: {
        pc5_rlc_ch_setup_list_present = true;
        pc5_rlc_ch_setup_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_setup_list.value.unpack(bref));
        break;
      }
      case 612: {
        pc5_rlc_ch_failed_to_be_setup_list_present = true;
        pc5_rlc_ch_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 616: {
        pc5_rlc_ch_modified_list_present = true;
        pc5_rlc_ch_modified_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_modified_list.value.unpack(bref));
        break;
      }
      case 613: {
        pc5_rlc_ch_failed_to_be_modified_list_present = true;
        pc5_rlc_ch_failed_to_be_modified_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_failed_to_be_modified_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_failed_to_be_modified_list.value.unpack(bref));
        break;
      }
      case 676: {
        sdt_bearer_cfg_info_present = true;
        sdt_bearer_cfg_info.id      = id;
        HANDLE_CODE(sdt_bearer_cfg_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_bearer_cfg_info.value.unpack(bref));
        break;
      }
      case 679: {
        ue_multicast_m_rbs_setup_list_present = true;
        ue_multicast_m_rbs_setup_list.id      = id;
        HANDLE_CODE(ue_multicast_m_rbs_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_multicast_m_rbs_setup_list.value.unpack(bref));
        break;
      }
      case 697: {
        serving_cell_mo_encoded_in_cgc_list_present = true;
        serving_cell_mo_encoded_in_cgc_list.id      = id;
        HANDLE_CODE(serving_cell_mo_encoded_in_cgc_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_cell_mo_encoded_in_cgc_list.value.unpack(bref));
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
void ue_context_mod_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  if (res_coordination_transfer_container_present) {
    j.write_fieldname("");
    res_coordination_transfer_container.to_json(j);
  }
  if (du_to_cu_rrc_info_present) {
    j.write_fieldname("");
    du_to_cu_rrc_info.to_json(j);
  }
  if (drbs_setup_mod_list_present) {
    j.write_fieldname("");
    drbs_setup_mod_list.to_json(j);
  }
  if (drbs_modified_list_present) {
    j.write_fieldname("");
    drbs_modified_list.to_json(j);
  }
  if (srbs_failed_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    srbs_failed_to_be_setup_mod_list.to_json(j);
  }
  if (drbs_failed_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    drbs_failed_to_be_setup_mod_list.to_json(j);
  }
  if (scell_failedto_setup_mod_list_present) {
    j.write_fieldname("");
    scell_failedto_setup_mod_list.to_json(j);
  }
  if (drbs_failed_to_be_modified_list_present) {
    j.write_fieldname("");
    drbs_failed_to_be_modified_list.to_json(j);
  }
  if (inactivity_monitoring_resp_present) {
    j.write_fieldname("");
    inactivity_monitoring_resp.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  if (c_rnti_present) {
    j.write_fieldname("");
    c_rnti.to_json(j);
  }
  if (associated_scell_list_present) {
    j.write_fieldname("");
    associated_scell_list.to_json(j);
  }
  if (srbs_setup_mod_list_present) {
    j.write_fieldname("");
    srbs_setup_mod_list.to_json(j);
  }
  if (srbs_modified_list_present) {
    j.write_fieldname("");
    srbs_modified_list.to_json(j);
  }
  if (full_cfg_present) {
    j.write_fieldname("");
    full_cfg.to_json(j);
  }
  if (bh_chs_setup_mod_list_present) {
    j.write_fieldname("");
    bh_chs_setup_mod_list.to_json(j);
  }
  if (bh_chs_modified_list_present) {
    j.write_fieldname("");
    bh_chs_modified_list.to_json(j);
  }
  if (bh_chs_failed_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    bh_chs_failed_to_be_setup_mod_list.to_json(j);
  }
  if (bh_chs_failed_to_be_modified_list_present) {
    j.write_fieldname("");
    bh_chs_failed_to_be_modified_list.to_json(j);
  }
  if (sl_drbs_setup_mod_list_present) {
    j.write_fieldname("");
    sl_drbs_setup_mod_list.to_json(j);
  }
  if (sl_drbs_modified_list_present) {
    j.write_fieldname("");
    sl_drbs_modified_list.to_json(j);
  }
  if (sl_drbs_failed_to_be_setup_mod_list_present) {
    j.write_fieldname("");
    sl_drbs_failed_to_be_setup_mod_list.to_json(j);
  }
  if (sl_drbs_failed_to_be_modified_list_present) {
    j.write_fieldname("");
    sl_drbs_failed_to_be_modified_list.to_json(j);
  }
  if (requested_target_cell_global_id_present) {
    j.write_fieldname("");
    requested_target_cell_global_id.to_json(j);
  }
  if (scg_activation_status_present) {
    j.write_fieldname("");
    scg_activation_status.to_json(j);
  }
  if (uu_rlc_ch_setup_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_setup_list.to_json(j);
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_failed_to_be_setup_list.to_json(j);
  }
  if (uu_rlc_ch_modified_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_modified_list.to_json(j);
  }
  if (uu_rlc_ch_failed_to_be_modified_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_failed_to_be_modified_list.to_json(j);
  }
  if (pc5_rlc_ch_setup_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_setup_list.to_json(j);
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_failed_to_be_setup_list.to_json(j);
  }
  if (pc5_rlc_ch_modified_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_modified_list.to_json(j);
  }
  if (pc5_rlc_ch_failed_to_be_modified_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_failed_to_be_modified_list.to_json(j);
  }
  if (sdt_bearer_cfg_info_present) {
    j.write_fieldname("");
    sdt_bearer_cfg_info.to_json(j);
  }
  if (ue_multicast_m_rbs_setup_list_present) {
    j.write_fieldname("");
    ue_multicast_m_rbs_setup_list.to_json(j);
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    j.write_fieldname("");
    serving_cell_mo_encoded_in_cgc_list.to_json(j);
  }
  j.end_obj();
}

ue_context_release_cmd_ies_container::ue_context_release_cmd_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  cause(0, crit_e::ignore),
  rrc_container(50, crit_e::ignore),
  srb_id(64, crit_e::ignore),
  old_gnb_du_ue_f1ap_id(47, crit_e::ignore),
  execute_dupl(109, crit_e::ignore),
  rrc_delivery_status_request(184, crit_e::ignore),
  target_cells_to_cancel(375, crit_e::reject),
  pos_conext_rev_ind(576, crit_e::reject),
  cg_sdt_kept_ind(588, crit_e::ignore)
{
}
SRSASN_CODE ue_context_release_cmd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += rrc_container_present ? 1 : 0;
  nof_ies += srb_id_present ? 1 : 0;
  nof_ies += old_gnb_du_ue_f1ap_id_present ? 1 : 0;
  nof_ies += execute_dupl_present ? 1 : 0;
  nof_ies += rrc_delivery_status_request_present ? 1 : 0;
  nof_ies += target_cells_to_cancel_present ? 1 : 0;
  nof_ies += pos_conext_rev_ind_present ? 1 : 0;
  nof_ies += cg_sdt_kept_ind_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (rrc_container_present) {
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (srb_id_present) {
    HANDLE_CODE(srb_id.pack(bref));
  }
  if (old_gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(old_gnb_du_ue_f1ap_id.pack(bref));
  }
  if (execute_dupl_present) {
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (target_cells_to_cancel_present) {
    HANDLE_CODE(target_cells_to_cancel.pack(bref));
  }
  if (pos_conext_rev_ind_present) {
    HANDLE_CODE(pos_conext_rev_ind.pack(bref));
  }
  if (cg_sdt_kept_ind_present) {
    HANDLE_CODE(cg_sdt_kept_ind.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_release_cmd_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 50: {
        rrc_container_present = true;
        rrc_container.id      = id;
        HANDLE_CODE(rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.value.unpack(bref));
        break;
      }
      case 64: {
        srb_id_present = true;
        srb_id.id      = id;
        HANDLE_CODE(srb_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srb_id.value.unpack(bref));
        break;
      }
      case 47: {
        old_gnb_du_ue_f1ap_id_present = true;
        old_gnb_du_ue_f1ap_id.id      = id;
        HANDLE_CODE(old_gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(old_gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        execute_dupl.id      = id;
        HANDLE_CODE(execute_dupl.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.value.unpack(bref));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        rrc_delivery_status_request.id      = id;
        HANDLE_CODE(rrc_delivery_status_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.value.unpack(bref));
        break;
      }
      case 375: {
        target_cells_to_cancel_present = true;
        target_cells_to_cancel.id      = id;
        HANDLE_CODE(target_cells_to_cancel.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(target_cells_to_cancel.value.unpack(bref));
        break;
      }
      case 576: {
        pos_conext_rev_ind_present = true;
        pos_conext_rev_ind.id      = id;
        HANDLE_CODE(pos_conext_rev_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pos_conext_rev_ind.value.unpack(bref));
        break;
      }
      case 588: {
        cg_sdt_kept_ind_present = true;
        cg_sdt_kept_ind.id      = id;
        HANDLE_CODE(cg_sdt_kept_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cg_sdt_kept_ind.value.unpack(bref));
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
void ue_context_release_cmd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (rrc_container_present) {
    j.write_fieldname("");
    rrc_container.to_json(j);
  }
  if (srb_id_present) {
    j.write_fieldname("");
    srb_id.to_json(j);
  }
  if (old_gnb_du_ue_f1ap_id_present) {
    j.write_fieldname("");
    old_gnb_du_ue_f1ap_id.to_json(j);
  }
  if (execute_dupl_present) {
    j.write_fieldname("");
    execute_dupl.to_json(j);
  }
  if (rrc_delivery_status_request_present) {
    j.write_fieldname("");
    rrc_delivery_status_request.to_json(j);
  }
  if (target_cells_to_cancel_present) {
    j.write_fieldname("");
    target_cells_to_cancel.to_json(j);
  }
  if (pos_conext_rev_ind_present) {
    j.write_fieldname("");
    pos_conext_rev_ind.to_json(j);
  }
  if (cg_sdt_kept_ind_present) {
    j.write_fieldname("");
    cg_sdt_kept_ind.to_json(j);
  }
  j.end_obj();
}

ue_context_release_complete_ies_container::ue_context_release_complete_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject), gnb_du_ue_f1ap_id(41, crit_e::reject), crit_diagnostics(7, crit_e::ignore)
{
}
SRSASN_CODE ue_context_release_complete_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_release_complete_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
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
void ue_context_release_complete_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

ue_context_release_request_ies_container::ue_context_release_request_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  cause(0, crit_e::ignore),
  target_cells_to_cancel(375, crit_e::reject)
{
}
SRSASN_CODE ue_context_release_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += target_cells_to_cancel_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (target_cells_to_cancel_present) {
    HANDLE_CODE(target_cells_to_cancel.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_release_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 375: {
        target_cells_to_cancel_present = true;
        target_cells_to_cancel.id      = id;
        HANDLE_CODE(target_cells_to_cancel.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(target_cells_to_cancel.value.unpack(bref));
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
void ue_context_release_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (target_cells_to_cancel_present) {
    j.write_fieldname("");
    target_cells_to_cancel.to_json(j);
  }
  j.end_obj();
}

ue_context_setup_fail_ies_container::ue_context_setup_fail_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::ignore),
  cause(0, crit_e::ignore),
  crit_diagnostics(7, crit_e::ignore),
  potential_sp_cell_list(92, crit_e::ignore),
  requested_target_cell_global_id(376, crit_e::reject)
{
}
SRSASN_CODE ue_context_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += gnb_du_ue_f1ap_id_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += potential_sp_cell_list_present ? 1 : 0;
  nof_ies += requested_target_cell_global_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  if (gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  }
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (potential_sp_cell_list_present) {
    HANDLE_CODE(potential_sp_cell_list.pack(bref));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(requested_target_cell_global_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_setup_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        gnb_du_ue_f1ap_id_present = true;
        gnb_du_ue_f1ap_id.id      = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      case 92: {
        potential_sp_cell_list_present = true;
        potential_sp_cell_list.id      = id;
        HANDLE_CODE(potential_sp_cell_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(potential_sp_cell_list.value.unpack(bref));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        requested_target_cell_global_id.id      = id;
        HANDLE_CODE(requested_target_cell_global_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.value.unpack(bref));
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
void ue_context_setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  if (gnb_du_ue_f1ap_id_present) {
    j.write_fieldname("");
    gnb_du_ue_f1ap_id.to_json(j);
  }
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  if (potential_sp_cell_list_present) {
    j.write_fieldname("");
    potential_sp_cell_list.to_json(j);
  }
  if (requested_target_cell_global_id_present) {
    j.write_fieldname("");
    requested_target_cell_global_id.to_json(j);
  }
  j.end_obj();
}

ue_context_setup_request_ies_container::ue_context_setup_request_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::ignore),
  sp_cell_id(63, crit_e::reject),
  serv_cell_idx(107, crit_e::reject),
  sp_cell_ul_cfg(96, crit_e::ignore),
  cu_to_du_rrc_info(9, crit_e::reject),
  candidate_sp_cell_list(90, crit_e::ignore),
  drx_cycle(38, crit_e::ignore),
  res_coordination_transfer_container(49, crit_e::ignore),
  scell_to_be_setup_list(54, crit_e::ignore),
  srbs_to_be_setup_list(74, crit_e::reject),
  drbs_to_be_setup_list(35, crit_e::reject),
  inactivity_monitoring_request(97, crit_e::reject),
  rat_freq_prio_info(108, crit_e::reject),
  rrc_container(50, crit_e::ignore),
  masked_imeisv(126, crit_e::ignore),
  serving_plmn(165, crit_e::ignore),
  gnb_du_ue_ambr_ul(158, crit_e::ignore),
  rrc_delivery_status_request(184, crit_e::ignore),
  res_coordination_transfer_info(195, crit_e::ignore),
  serving_cell_mo(182, crit_e::ignore),
  new_gnb_cu_ue_f1ap_id(217, crit_e::reject),
  ran_ue_id(226, crit_e::ignore),
  trace_activation(242, crit_e::ignore),
  add_rrm_prio_idx(248, crit_e::ignore),
  bh_chs_to_be_setup_list(258, crit_e::reject),
  cfg_bap_address(282, crit_e::reject),
  nr_v2x_services_authorized(306, crit_e::ignore),
  ltev2x_services_authorized(307, crit_e::ignore),
  nr_ue_sidelink_aggr_max_bitrate(308, crit_e::ignore),
  lte_ue_sidelink_aggr_max_bitrate(309, crit_e::ignore),
  pc5_link_ambr(340, crit_e::ignore),
  sl_drbs_to_be_setup_list(330, crit_e::reject),
  conditional_inter_du_mob_info(373, crit_e::reject),
  management_based_mdt_plmn_list(377, crit_e::ignore),
  serving_n_id(382, crit_e::reject),
  f1_c_transfer_path(428, crit_e::reject),
  f1_c_transfer_path_nr_dc(511, crit_e::reject),
  mdt_polluted_meas_ind(536, crit_e::ignore),
  scg_activation_request(547, crit_e::ignore),
  cg_sdt_session_info_old(591, crit_e::ignore),
  five_g_pro_se_authorized(594, crit_e::ignore),
  five_g_pro_se_ue_pc5_aggr_max_bitrate(595, crit_e::ignore),
  five_g_pro_se_pc5_link_ambr(596, crit_e::ignore),
  uu_rlc_ch_to_be_setup_list(599, crit_e::reject),
  pc5_rlc_ch_to_be_setup_list(608, crit_e::reject),
  path_switch_cfg(619, crit_e::ignore),
  gnb_du_ue_slice_max_bit_rate_list(626, crit_e::ignore),
  multicast_mbs_session_setup_list(632, crit_e::reject),
  ue_multicast_m_rbs_to_be_setup_list(630, crit_e::reject),
  serving_cell_mo_list(695, crit_e::ignore)
{
}
SRSASN_CODE ue_context_setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += gnb_du_ue_f1ap_id_present ? 1 : 0;
  nof_ies += sp_cell_ul_cfg_present ? 1 : 0;
  nof_ies += candidate_sp_cell_list_present ? 1 : 0;
  nof_ies += drx_cycle_present ? 1 : 0;
  nof_ies += res_coordination_transfer_container_present ? 1 : 0;
  nof_ies += scell_to_be_setup_list_present ? 1 : 0;
  nof_ies += srbs_to_be_setup_list_present ? 1 : 0;
  nof_ies += drbs_to_be_setup_list_present ? 1 : 0;
  nof_ies += inactivity_monitoring_request_present ? 1 : 0;
  nof_ies += rat_freq_prio_info_present ? 1 : 0;
  nof_ies += rrc_container_present ? 1 : 0;
  nof_ies += masked_imeisv_present ? 1 : 0;
  nof_ies += serving_plmn_present ? 1 : 0;
  nof_ies += gnb_du_ue_ambr_ul_present ? 1 : 0;
  nof_ies += rrc_delivery_status_request_present ? 1 : 0;
  nof_ies += res_coordination_transfer_info_present ? 1 : 0;
  nof_ies += serving_cell_mo_present ? 1 : 0;
  nof_ies += new_gnb_cu_ue_f1ap_id_present ? 1 : 0;
  nof_ies += ran_ue_id_present ? 1 : 0;
  nof_ies += trace_activation_present ? 1 : 0;
  nof_ies += add_rrm_prio_idx_present ? 1 : 0;
  nof_ies += bh_chs_to_be_setup_list_present ? 1 : 0;
  nof_ies += cfg_bap_address_present ? 1 : 0;
  nof_ies += nr_v2x_services_authorized_present ? 1 : 0;
  nof_ies += ltev2x_services_authorized_present ? 1 : 0;
  nof_ies += nr_ue_sidelink_aggr_max_bitrate_present ? 1 : 0;
  nof_ies += lte_ue_sidelink_aggr_max_bitrate_present ? 1 : 0;
  nof_ies += pc5_link_ambr_present ? 1 : 0;
  nof_ies += sl_drbs_to_be_setup_list_present ? 1 : 0;
  nof_ies += conditional_inter_du_mob_info_present ? 1 : 0;
  nof_ies += management_based_mdt_plmn_list_present ? 1 : 0;
  nof_ies += serving_n_id_present ? 1 : 0;
  nof_ies += f1_c_transfer_path_present ? 1 : 0;
  nof_ies += f1_c_transfer_path_nr_dc_present ? 1 : 0;
  nof_ies += mdt_polluted_meas_ind_present ? 1 : 0;
  nof_ies += scg_activation_request_present ? 1 : 0;
  nof_ies += cg_sdt_session_info_old_present ? 1 : 0;
  nof_ies += five_g_pro_se_authorized_present ? 1 : 0;
  nof_ies += five_g_pro_se_ue_pc5_aggr_max_bitrate_present ? 1 : 0;
  nof_ies += five_g_pro_se_pc5_link_ambr_present ? 1 : 0;
  nof_ies += uu_rlc_ch_to_be_setup_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_to_be_setup_list_present ? 1 : 0;
  nof_ies += path_switch_cfg_present ? 1 : 0;
  nof_ies += gnb_du_ue_slice_max_bit_rate_list_present ? 1 : 0;
  nof_ies += multicast_mbs_session_setup_list_present ? 1 : 0;
  nof_ies += ue_multicast_m_rbs_to_be_setup_list_present ? 1 : 0;
  nof_ies += serving_cell_mo_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  if (gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  }
  HANDLE_CODE(sp_cell_id.pack(bref));
  HANDLE_CODE(serv_cell_idx.pack(bref));
  if (sp_cell_ul_cfg_present) {
    HANDLE_CODE(sp_cell_ul_cfg.pack(bref));
  }
  HANDLE_CODE(cu_to_du_rrc_info.pack(bref));
  if (candidate_sp_cell_list_present) {
    HANDLE_CODE(candidate_sp_cell_list.pack(bref));
  }
  if (drx_cycle_present) {
    HANDLE_CODE(drx_cycle.pack(bref));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (scell_to_be_setup_list_present) {
    HANDLE_CODE(scell_to_be_setup_list.pack(bref));
  }
  if (srbs_to_be_setup_list_present) {
    HANDLE_CODE(srbs_to_be_setup_list.pack(bref));
  }
  if (drbs_to_be_setup_list_present) {
    HANDLE_CODE(drbs_to_be_setup_list.pack(bref));
  }
  if (inactivity_monitoring_request_present) {
    HANDLE_CODE(inactivity_monitoring_request.pack(bref));
  }
  if (rat_freq_prio_info_present) {
    HANDLE_CODE(rat_freq_prio_info.pack(bref));
  }
  if (rrc_container_present) {
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (masked_imeisv_present) {
    HANDLE_CODE(masked_imeisv.pack(bref));
  }
  if (serving_plmn_present) {
    HANDLE_CODE(serving_plmn.pack(bref));
  }
  if (gnb_du_ue_ambr_ul_present) {
    HANDLE_CODE(gnb_du_ue_ambr_ul.pack(bref));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (res_coordination_transfer_info_present) {
    HANDLE_CODE(res_coordination_transfer_info.pack(bref));
  }
  if (serving_cell_mo_present) {
    HANDLE_CODE(serving_cell_mo.pack(bref));
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    HANDLE_CODE(new_gnb_cu_ue_f1ap_id.pack(bref));
  }
  if (ran_ue_id_present) {
    HANDLE_CODE(ran_ue_id.pack(bref));
  }
  if (trace_activation_present) {
    HANDLE_CODE(trace_activation.pack(bref));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }
  if (bh_chs_to_be_setup_list_present) {
    HANDLE_CODE(bh_chs_to_be_setup_list.pack(bref));
  }
  if (cfg_bap_address_present) {
    HANDLE_CODE(cfg_bap_address.pack(bref));
  }
  if (nr_v2x_services_authorized_present) {
    HANDLE_CODE(nr_v2x_services_authorized.pack(bref));
  }
  if (ltev2x_services_authorized_present) {
    HANDLE_CODE(ltev2x_services_authorized.pack(bref));
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (pc5_link_ambr_present) {
    HANDLE_CODE(pc5_link_ambr.pack(bref));
  }
  if (sl_drbs_to_be_setup_list_present) {
    HANDLE_CODE(sl_drbs_to_be_setup_list.pack(bref));
  }
  if (conditional_inter_du_mob_info_present) {
    HANDLE_CODE(conditional_inter_du_mob_info.pack(bref));
  }
  if (management_based_mdt_plmn_list_present) {
    HANDLE_CODE(management_based_mdt_plmn_list.pack(bref));
  }
  if (serving_n_id_present) {
    HANDLE_CODE(serving_n_id.pack(bref));
  }
  if (f1_c_transfer_path_present) {
    HANDLE_CODE(f1_c_transfer_path.pack(bref));
  }
  if (f1_c_transfer_path_nr_dc_present) {
    HANDLE_CODE(f1_c_transfer_path_nr_dc.pack(bref));
  }
  if (mdt_polluted_meas_ind_present) {
    HANDLE_CODE(mdt_polluted_meas_ind.pack(bref));
  }
  if (scg_activation_request_present) {
    HANDLE_CODE(scg_activation_request.pack(bref));
  }
  if (cg_sdt_session_info_old_present) {
    HANDLE_CODE(cg_sdt_session_info_old.pack(bref));
  }
  if (five_g_pro_se_authorized_present) {
    HANDLE_CODE(five_g_pro_se_authorized.pack(bref));
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.pack(bref));
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    HANDLE_CODE(five_g_pro_se_pc5_link_ambr.pack(bref));
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(uu_rlc_ch_to_be_setup_list.pack(bref));
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(pc5_rlc_ch_to_be_setup_list.pack(bref));
  }
  if (path_switch_cfg_present) {
    HANDLE_CODE(path_switch_cfg.pack(bref));
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    HANDLE_CODE(gnb_du_ue_slice_max_bit_rate_list.pack(bref));
  }
  if (multicast_mbs_session_setup_list_present) {
    HANDLE_CODE(multicast_mbs_session_setup_list.pack(bref));
  }
  if (ue_multicast_m_rbs_to_be_setup_list_present) {
    HANDLE_CODE(ue_multicast_m_rbs_to_be_setup_list.pack(bref));
  }
  if (serving_cell_mo_list_present) {
    HANDLE_CODE(serving_cell_mo_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_setup_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        gnb_du_ue_f1ap_id_present = true;
        gnb_du_ue_f1ap_id.id      = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 63: {
        nof_mandatory_ies--;
        sp_cell_id.id = id;
        HANDLE_CODE(sp_cell_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_id.value.unpack(bref));
        break;
      }
      case 107: {
        nof_mandatory_ies--;
        serv_cell_idx.id = id;
        HANDLE_CODE(serv_cell_idx.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serv_cell_idx.value.unpack(bref));
        break;
      }
      case 96: {
        sp_cell_ul_cfg_present = true;
        sp_cell_ul_cfg.id      = id;
        HANDLE_CODE(sp_cell_ul_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_ul_cfg.value.unpack(bref));
        break;
      }
      case 9: {
        nof_mandatory_ies--;
        cu_to_du_rrc_info.id = id;
        HANDLE_CODE(cu_to_du_rrc_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cu_to_du_rrc_info.value.unpack(bref));
        break;
      }
      case 90: {
        candidate_sp_cell_list_present = true;
        candidate_sp_cell_list.id      = id;
        HANDLE_CODE(candidate_sp_cell_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(candidate_sp_cell_list.value.unpack(bref));
        break;
      }
      case 38: {
        drx_cycle_present = true;
        drx_cycle.id      = id;
        HANDLE_CODE(drx_cycle.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drx_cycle.value.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        res_coordination_transfer_container.id      = id;
        HANDLE_CODE(res_coordination_transfer_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.value.unpack(bref));
        break;
      }
      case 54: {
        scell_to_be_setup_list_present = true;
        scell_to_be_setup_list.id      = id;
        HANDLE_CODE(scell_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scell_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 74: {
        srbs_to_be_setup_list_present = true;
        srbs_to_be_setup_list.id      = id;
        HANDLE_CODE(srbs_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 35: {
        drbs_to_be_setup_list_present = true;
        drbs_to_be_setup_list.id      = id;
        HANDLE_CODE(drbs_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 97: {
        inactivity_monitoring_request_present = true;
        inactivity_monitoring_request.id      = id;
        HANDLE_CODE(inactivity_monitoring_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_request.value.unpack(bref));
        break;
      }
      case 108: {
        rat_freq_prio_info_present = true;
        rat_freq_prio_info.id      = id;
        HANDLE_CODE(rat_freq_prio_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rat_freq_prio_info.value.unpack(bref));
        break;
      }
      case 50: {
        rrc_container_present = true;
        rrc_container.id      = id;
        HANDLE_CODE(rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.value.unpack(bref));
        break;
      }
      case 126: {
        masked_imeisv_present = true;
        masked_imeisv.id      = id;
        HANDLE_CODE(masked_imeisv.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(masked_imeisv.value.unpack(bref));
        break;
      }
      case 165: {
        serving_plmn_present = true;
        serving_plmn.id      = id;
        HANDLE_CODE(serving_plmn.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_plmn.value.unpack(bref));
        break;
      }
      case 158: {
        gnb_du_ue_ambr_ul_present = true;
        gnb_du_ue_ambr_ul.id      = id;
        HANDLE_CODE(gnb_du_ue_ambr_ul.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_ambr_ul.value.unpack(bref));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        rrc_delivery_status_request.id      = id;
        HANDLE_CODE(rrc_delivery_status_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.value.unpack(bref));
        break;
      }
      case 195: {
        res_coordination_transfer_info_present = true;
        res_coordination_transfer_info.id      = id;
        HANDLE_CODE(res_coordination_transfer_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_info.value.unpack(bref));
        break;
      }
      case 182: {
        serving_cell_mo_present = true;
        serving_cell_mo.id      = id;
        HANDLE_CODE(serving_cell_mo.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_cell_mo.value.unpack(bref));
        break;
      }
      case 217: {
        new_gnb_cu_ue_f1ap_id_present = true;
        new_gnb_cu_ue_f1ap_id.id      = id;
        HANDLE_CODE(new_gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(new_gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 226: {
        ran_ue_id_present = true;
        ran_ue_id.id      = id;
        HANDLE_CODE(ran_ue_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ran_ue_id.value.unpack(bref));
        break;
      }
      case 242: {
        trace_activation_present = true;
        trace_activation.id      = id;
        HANDLE_CODE(trace_activation.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_activation.value.unpack(bref));
        break;
      }
      case 248: {
        add_rrm_prio_idx_present = true;
        add_rrm_prio_idx.id      = id;
        HANDLE_CODE(add_rrm_prio_idx.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.value.unpack(bref));
        break;
      }
      case 258: {
        bh_chs_to_be_setup_list_present = true;
        bh_chs_to_be_setup_list.id      = id;
        HANDLE_CODE(bh_chs_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 282: {
        cfg_bap_address_present = true;
        cfg_bap_address.id      = id;
        HANDLE_CODE(cfg_bap_address.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cfg_bap_address.value.unpack(bref));
        break;
      }
      case 306: {
        nr_v2x_services_authorized_present = true;
        nr_v2x_services_authorized.id      = id;
        HANDLE_CODE(nr_v2x_services_authorized.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_v2x_services_authorized.value.unpack(bref));
        break;
      }
      case 307: {
        ltev2x_services_authorized_present = true;
        ltev2x_services_authorized.id      = id;
        HANDLE_CODE(ltev2x_services_authorized.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ltev2x_services_authorized.value.unpack(bref));
        break;
      }
      case 308: {
        nr_ue_sidelink_aggr_max_bitrate_present = true;
        nr_ue_sidelink_aggr_max_bitrate.id      = id;
        HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.value.unpack(bref));
        break;
      }
      case 309: {
        lte_ue_sidelink_aggr_max_bitrate_present = true;
        lte_ue_sidelink_aggr_max_bitrate.id      = id;
        HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.value.unpack(bref));
        break;
      }
      case 340: {
        pc5_link_ambr_present = true;
        pc5_link_ambr.id      = id;
        HANDLE_CODE(pc5_link_ambr.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_link_ambr.value.unpack(bref));
        break;
      }
      case 330: {
        sl_drbs_to_be_setup_list_present = true;
        sl_drbs_to_be_setup_list.id      = id;
        HANDLE_CODE(sl_drbs_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 373: {
        conditional_inter_du_mob_info_present = true;
        conditional_inter_du_mob_info.id      = id;
        HANDLE_CODE(conditional_inter_du_mob_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(conditional_inter_du_mob_info.value.unpack(bref));
        break;
      }
      case 377: {
        management_based_mdt_plmn_list_present = true;
        management_based_mdt_plmn_list.id      = id;
        HANDLE_CODE(management_based_mdt_plmn_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(management_based_mdt_plmn_list.value.unpack(bref));
        break;
      }
      case 382: {
        serving_n_id_present = true;
        serving_n_id.id      = id;
        HANDLE_CODE(serving_n_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_n_id.value.unpack(bref));
        break;
      }
      case 428: {
        f1_c_transfer_path_present = true;
        f1_c_transfer_path.id      = id;
        HANDLE_CODE(f1_c_transfer_path.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path.value.unpack(bref));
        break;
      }
      case 511: {
        f1_c_transfer_path_nr_dc_present = true;
        f1_c_transfer_path_nr_dc.id      = id;
        HANDLE_CODE(f1_c_transfer_path_nr_dc.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path_nr_dc.value.unpack(bref));
        break;
      }
      case 536: {
        mdt_polluted_meas_ind_present = true;
        mdt_polluted_meas_ind.id      = id;
        HANDLE_CODE(mdt_polluted_meas_ind.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(mdt_polluted_meas_ind.value.unpack(bref));
        break;
      }
      case 547: {
        scg_activation_request_present = true;
        scg_activation_request.id      = id;
        HANDLE_CODE(scg_activation_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_request.value.unpack(bref));
        break;
      }
      case 591: {
        cg_sdt_session_info_old_present = true;
        cg_sdt_session_info_old.id      = id;
        HANDLE_CODE(cg_sdt_session_info_old.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cg_sdt_session_info_old.value.unpack(bref));
        break;
      }
      case 594: {
        five_g_pro_se_authorized_present = true;
        five_g_pro_se_authorized.id      = id;
        HANDLE_CODE(five_g_pro_se_authorized.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_authorized.value.unpack(bref));
        break;
      }
      case 595: {
        five_g_pro_se_ue_pc5_aggr_max_bitrate_present = true;
        five_g_pro_se_ue_pc5_aggr_max_bitrate.id      = id;
        HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.value.unpack(bref));
        break;
      }
      case 596: {
        five_g_pro_se_pc5_link_ambr_present = true;
        five_g_pro_se_pc5_link_ambr.id      = id;
        HANDLE_CODE(five_g_pro_se_pc5_link_ambr.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_pc5_link_ambr.value.unpack(bref));
        break;
      }
      case 599: {
        uu_rlc_ch_to_be_setup_list_present = true;
        uu_rlc_ch_to_be_setup_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 608: {
        pc5_rlc_ch_to_be_setup_list_present = true;
        pc5_rlc_ch_to_be_setup_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 619: {
        path_switch_cfg_present = true;
        path_switch_cfg.id      = id;
        HANDLE_CODE(path_switch_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(path_switch_cfg.value.unpack(bref));
        break;
      }
      case 626: {
        gnb_du_ue_slice_max_bit_rate_list_present = true;
        gnb_du_ue_slice_max_bit_rate_list.id      = id;
        HANDLE_CODE(gnb_du_ue_slice_max_bit_rate_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_slice_max_bit_rate_list.value.unpack(bref));
        break;
      }
      case 632: {
        multicast_mbs_session_setup_list_present = true;
        multicast_mbs_session_setup_list.id      = id;
        HANDLE_CODE(multicast_mbs_session_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(multicast_mbs_session_setup_list.value.unpack(bref));
        break;
      }
      case 630: {
        ue_multicast_m_rbs_to_be_setup_list_present = true;
        ue_multicast_m_rbs_to_be_setup_list.id      = id;
        HANDLE_CODE(ue_multicast_m_rbs_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_multicast_m_rbs_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 695: {
        serving_cell_mo_list_present = true;
        serving_cell_mo_list.id      = id;
        HANDLE_CODE(serving_cell_mo_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_cell_mo_list.value.unpack(bref));
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
void ue_context_setup_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  if (gnb_du_ue_f1ap_id_present) {
    j.write_fieldname("");
    gnb_du_ue_f1ap_id.to_json(j);
  }
  j.write_fieldname("");
  sp_cell_id.to_json(j);
  j.write_fieldname("");
  serv_cell_idx.to_json(j);
  if (sp_cell_ul_cfg_present) {
    j.write_fieldname("");
    sp_cell_ul_cfg.to_json(j);
  }
  j.write_fieldname("");
  cu_to_du_rrc_info.to_json(j);
  if (candidate_sp_cell_list_present) {
    j.write_fieldname("");
    candidate_sp_cell_list.to_json(j);
  }
  if (drx_cycle_present) {
    j.write_fieldname("");
    drx_cycle.to_json(j);
  }
  if (res_coordination_transfer_container_present) {
    j.write_fieldname("");
    res_coordination_transfer_container.to_json(j);
  }
  if (scell_to_be_setup_list_present) {
    j.write_fieldname("");
    scell_to_be_setup_list.to_json(j);
  }
  if (srbs_to_be_setup_list_present) {
    j.write_fieldname("");
    srbs_to_be_setup_list.to_json(j);
  }
  if (drbs_to_be_setup_list_present) {
    j.write_fieldname("");
    drbs_to_be_setup_list.to_json(j);
  }
  if (inactivity_monitoring_request_present) {
    j.write_fieldname("");
    inactivity_monitoring_request.to_json(j);
  }
  if (rat_freq_prio_info_present) {
    j.write_fieldname("");
    rat_freq_prio_info.to_json(j);
  }
  if (rrc_container_present) {
    j.write_fieldname("");
    rrc_container.to_json(j);
  }
  if (masked_imeisv_present) {
    j.write_fieldname("");
    masked_imeisv.to_json(j);
  }
  if (serving_plmn_present) {
    j.write_fieldname("");
    serving_plmn.to_json(j);
  }
  if (gnb_du_ue_ambr_ul_present) {
    j.write_fieldname("");
    gnb_du_ue_ambr_ul.to_json(j);
  }
  if (rrc_delivery_status_request_present) {
    j.write_fieldname("");
    rrc_delivery_status_request.to_json(j);
  }
  if (res_coordination_transfer_info_present) {
    j.write_fieldname("");
    res_coordination_transfer_info.to_json(j);
  }
  if (serving_cell_mo_present) {
    j.write_fieldname("");
    serving_cell_mo.to_json(j);
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    j.write_fieldname("");
    new_gnb_cu_ue_f1ap_id.to_json(j);
  }
  if (ran_ue_id_present) {
    j.write_fieldname("");
    ran_ue_id.to_json(j);
  }
  if (trace_activation_present) {
    j.write_fieldname("");
    trace_activation.to_json(j);
  }
  if (add_rrm_prio_idx_present) {
    j.write_fieldname("");
    add_rrm_prio_idx.to_json(j);
  }
  if (bh_chs_to_be_setup_list_present) {
    j.write_fieldname("");
    bh_chs_to_be_setup_list.to_json(j);
  }
  if (cfg_bap_address_present) {
    j.write_fieldname("");
    cfg_bap_address.to_json(j);
  }
  if (nr_v2x_services_authorized_present) {
    j.write_fieldname("");
    nr_v2x_services_authorized.to_json(j);
  }
  if (ltev2x_services_authorized_present) {
    j.write_fieldname("");
    ltev2x_services_authorized.to_json(j);
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    j.write_fieldname("");
    nr_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    j.write_fieldname("");
    lte_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (pc5_link_ambr_present) {
    j.write_fieldname("");
    pc5_link_ambr.to_json(j);
  }
  if (sl_drbs_to_be_setup_list_present) {
    j.write_fieldname("");
    sl_drbs_to_be_setup_list.to_json(j);
  }
  if (conditional_inter_du_mob_info_present) {
    j.write_fieldname("");
    conditional_inter_du_mob_info.to_json(j);
  }
  if (management_based_mdt_plmn_list_present) {
    j.write_fieldname("");
    management_based_mdt_plmn_list.to_json(j);
  }
  if (serving_n_id_present) {
    j.write_fieldname("");
    serving_n_id.to_json(j);
  }
  if (f1_c_transfer_path_present) {
    j.write_fieldname("");
    f1_c_transfer_path.to_json(j);
  }
  if (f1_c_transfer_path_nr_dc_present) {
    j.write_fieldname("");
    f1_c_transfer_path_nr_dc.to_json(j);
  }
  if (mdt_polluted_meas_ind_present) {
    j.write_fieldname("");
    mdt_polluted_meas_ind.to_json(j);
  }
  if (scg_activation_request_present) {
    j.write_fieldname("");
    scg_activation_request.to_json(j);
  }
  if (cg_sdt_session_info_old_present) {
    j.write_fieldname("");
    cg_sdt_session_info_old.to_json(j);
  }
  if (five_g_pro_se_authorized_present) {
    j.write_fieldname("");
    five_g_pro_se_authorized.to_json(j);
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    j.write_fieldname("");
    five_g_pro_se_ue_pc5_aggr_max_bitrate.to_json(j);
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    j.write_fieldname("");
    five_g_pro_se_pc5_link_ambr.to_json(j);
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_to_be_setup_list.to_json(j);
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_to_be_setup_list.to_json(j);
  }
  if (path_switch_cfg_present) {
    j.write_fieldname("");
    path_switch_cfg.to_json(j);
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    j.write_fieldname("");
    gnb_du_ue_slice_max_bit_rate_list.to_json(j);
  }
  if (multicast_mbs_session_setup_list_present) {
    j.write_fieldname("");
    multicast_mbs_session_setup_list.to_json(j);
  }
  if (ue_multicast_m_rbs_to_be_setup_list_present) {
    j.write_fieldname("");
    ue_multicast_m_rbs_to_be_setup_list.to_json(j);
  }
  if (serving_cell_mo_list_present) {
    j.write_fieldname("");
    serving_cell_mo_list.to_json(j);
  }
  j.end_obj();
}

ue_context_setup_resp_ies_container::ue_context_setup_resp_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  du_to_cu_rrc_info(39, crit_e::reject),
  c_rnti(95, crit_e::ignore),
  res_coordination_transfer_container(49, crit_e::ignore),
  full_cfg(94, crit_e::reject),
  drbs_setup_list(27, crit_e::ignore),
  srbs_failed_to_be_setup_list(66, crit_e::ignore),
  drbs_failed_to_be_setup_list(15, crit_e::ignore),
  scell_failedto_setup_list(83, crit_e::ignore),
  inactivity_monitoring_resp(98, crit_e::reject),
  crit_diagnostics(7, crit_e::ignore),
  srbs_setup_list(202, crit_e::ignore),
  bh_chs_setup_list(260, crit_e::ignore),
  bh_chs_failed_to_be_setup_list(279, crit_e::ignore),
  sl_drbs_setup_list(324, crit_e::ignore),
  sl_drbs_failed_to_be_setup_list(316, crit_e::ignore),
  requested_target_cell_global_id(376, crit_e::reject),
  scg_activation_status(548, crit_e::ignore),
  uu_rlc_ch_setup_list(602, crit_e::ignore),
  uu_rlc_ch_failed_to_be_setup_list(603, crit_e::ignore),
  pc5_rlc_ch_setup_list(611, crit_e::ignore),
  pc5_rlc_ch_failed_to_be_setup_list(612, crit_e::ignore),
  serving_cell_mo_encoded_in_cgc_list(697, crit_e::ignore)
{
}
SRSASN_CODE ue_context_setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += c_rnti_present ? 1 : 0;
  nof_ies += res_coordination_transfer_container_present ? 1 : 0;
  nof_ies += full_cfg_present ? 1 : 0;
  nof_ies += drbs_setup_list_present ? 1 : 0;
  nof_ies += srbs_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += drbs_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += scell_failedto_setup_list_present ? 1 : 0;
  nof_ies += inactivity_monitoring_resp_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += srbs_setup_list_present ? 1 : 0;
  nof_ies += bh_chs_setup_list_present ? 1 : 0;
  nof_ies += bh_chs_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += sl_drbs_setup_list_present ? 1 : 0;
  nof_ies += sl_drbs_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += requested_target_cell_global_id_present ? 1 : 0;
  nof_ies += scg_activation_status_present ? 1 : 0;
  nof_ies += uu_rlc_ch_setup_list_present ? 1 : 0;
  nof_ies += uu_rlc_ch_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_setup_list_present ? 1 : 0;
  nof_ies += pc5_rlc_ch_failed_to_be_setup_list_present ? 1 : 0;
  nof_ies += serving_cell_mo_encoded_in_cgc_list_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(du_to_cu_rrc_info.pack(bref));
  if (c_rnti_present) {
    HANDLE_CODE(c_rnti.pack(bref));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (full_cfg_present) {
    HANDLE_CODE(full_cfg.pack(bref));
  }
  if (drbs_setup_list_present) {
    HANDLE_CODE(drbs_setup_list.pack(bref));
  }
  if (srbs_failed_to_be_setup_list_present) {
    HANDLE_CODE(srbs_failed_to_be_setup_list.pack(bref));
  }
  if (drbs_failed_to_be_setup_list_present) {
    HANDLE_CODE(drbs_failed_to_be_setup_list.pack(bref));
  }
  if (scell_failedto_setup_list_present) {
    HANDLE_CODE(scell_failedto_setup_list.pack(bref));
  }
  if (inactivity_monitoring_resp_present) {
    HANDLE_CODE(inactivity_monitoring_resp.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (srbs_setup_list_present) {
    HANDLE_CODE(srbs_setup_list.pack(bref));
  }
  if (bh_chs_setup_list_present) {
    HANDLE_CODE(bh_chs_setup_list.pack(bref));
  }
  if (bh_chs_failed_to_be_setup_list_present) {
    HANDLE_CODE(bh_chs_failed_to_be_setup_list.pack(bref));
  }
  if (sl_drbs_setup_list_present) {
    HANDLE_CODE(sl_drbs_setup_list.pack(bref));
  }
  if (sl_drbs_failed_to_be_setup_list_present) {
    HANDLE_CODE(sl_drbs_failed_to_be_setup_list.pack(bref));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(requested_target_cell_global_id.pack(bref));
  }
  if (scg_activation_status_present) {
    HANDLE_CODE(scg_activation_status.pack(bref));
  }
  if (uu_rlc_ch_setup_list_present) {
    HANDLE_CODE(uu_rlc_ch_setup_list.pack(bref));
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(uu_rlc_ch_failed_to_be_setup_list.pack(bref));
  }
  if (pc5_rlc_ch_setup_list_present) {
    HANDLE_CODE(pc5_rlc_ch_setup_list.pack(bref));
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(pc5_rlc_ch_failed_to_be_setup_list.pack(bref));
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    HANDLE_CODE(serving_cell_mo_encoded_in_cgc_list.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_context_setup_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 39: {
        nof_mandatory_ies--;
        du_to_cu_rrc_info.id = id;
        HANDLE_CODE(du_to_cu_rrc_info.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_info.value.unpack(bref));
        break;
      }
      case 95: {
        c_rnti_present = true;
        c_rnti.id      = id;
        HANDLE_CODE(c_rnti.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(c_rnti.value.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        res_coordination_transfer_container.id      = id;
        HANDLE_CODE(res_coordination_transfer_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.value.unpack(bref));
        break;
      }
      case 94: {
        full_cfg_present = true;
        full_cfg.id      = id;
        HANDLE_CODE(full_cfg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(full_cfg.value.unpack(bref));
        break;
      }
      case 27: {
        drbs_setup_list_present = true;
        drbs_setup_list.id      = id;
        HANDLE_CODE(drbs_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_setup_list.value.unpack(bref));
        break;
      }
      case 66: {
        srbs_failed_to_be_setup_list_present = true;
        srbs_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(srbs_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 15: {
        drbs_failed_to_be_setup_list_present = true;
        drbs_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(drbs_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drbs_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 83: {
        scell_failedto_setup_list_present = true;
        scell_failedto_setup_list.id      = id;
        HANDLE_CODE(scell_failedto_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scell_failedto_setup_list.value.unpack(bref));
        break;
      }
      case 98: {
        inactivity_monitoring_resp_present = true;
        inactivity_monitoring_resp.id      = id;
        HANDLE_CODE(inactivity_monitoring_resp.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_resp.value.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      case 202: {
        srbs_setup_list_present = true;
        srbs_setup_list.id      = id;
        HANDLE_CODE(srbs_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srbs_setup_list.value.unpack(bref));
        break;
      }
      case 260: {
        bh_chs_setup_list_present = true;
        bh_chs_setup_list.id      = id;
        HANDLE_CODE(bh_chs_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_setup_list.value.unpack(bref));
        break;
      }
      case 279: {
        bh_chs_failed_to_be_setup_list_present = true;
        bh_chs_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(bh_chs_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(bh_chs_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 324: {
        sl_drbs_setup_list_present = true;
        sl_drbs_setup_list.id      = id;
        HANDLE_CODE(sl_drbs_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_setup_list.value.unpack(bref));
        break;
      }
      case 316: {
        sl_drbs_failed_to_be_setup_list_present = true;
        sl_drbs_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(sl_drbs_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sl_drbs_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        requested_target_cell_global_id.id      = id;
        HANDLE_CODE(requested_target_cell_global_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.value.unpack(bref));
        break;
      }
      case 548: {
        scg_activation_status_present = true;
        scg_activation_status.id      = id;
        HANDLE_CODE(scg_activation_status.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_status.value.unpack(bref));
        break;
      }
      case 602: {
        uu_rlc_ch_setup_list_present = true;
        uu_rlc_ch_setup_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_setup_list.value.unpack(bref));
        break;
      }
      case 603: {
        uu_rlc_ch_failed_to_be_setup_list_present = true;
        uu_rlc_ch_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(uu_rlc_ch_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(uu_rlc_ch_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 611: {
        pc5_rlc_ch_setup_list_present = true;
        pc5_rlc_ch_setup_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_setup_list.value.unpack(bref));
        break;
      }
      case 612: {
        pc5_rlc_ch_failed_to_be_setup_list_present = true;
        pc5_rlc_ch_failed_to_be_setup_list.id      = id;
        HANDLE_CODE(pc5_rlc_ch_failed_to_be_setup_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pc5_rlc_ch_failed_to_be_setup_list.value.unpack(bref));
        break;
      }
      case 697: {
        serving_cell_mo_encoded_in_cgc_list_present = true;
        serving_cell_mo_encoded_in_cgc_list.id      = id;
        HANDLE_CODE(serving_cell_mo_encoded_in_cgc_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_cell_mo_encoded_in_cgc_list.value.unpack(bref));
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
void ue_context_setup_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  du_to_cu_rrc_info.to_json(j);
  if (c_rnti_present) {
    j.write_fieldname("");
    c_rnti.to_json(j);
  }
  if (res_coordination_transfer_container_present) {
    j.write_fieldname("");
    res_coordination_transfer_container.to_json(j);
  }
  if (full_cfg_present) {
    j.write_fieldname("");
    full_cfg.to_json(j);
  }
  if (drbs_setup_list_present) {
    j.write_fieldname("");
    drbs_setup_list.to_json(j);
  }
  if (srbs_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    srbs_failed_to_be_setup_list.to_json(j);
  }
  if (drbs_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    drbs_failed_to_be_setup_list.to_json(j);
  }
  if (scell_failedto_setup_list_present) {
    j.write_fieldname("");
    scell_failedto_setup_list.to_json(j);
  }
  if (inactivity_monitoring_resp_present) {
    j.write_fieldname("");
    inactivity_monitoring_resp.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  if (srbs_setup_list_present) {
    j.write_fieldname("");
    srbs_setup_list.to_json(j);
  }
  if (bh_chs_setup_list_present) {
    j.write_fieldname("");
    bh_chs_setup_list.to_json(j);
  }
  if (bh_chs_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    bh_chs_failed_to_be_setup_list.to_json(j);
  }
  if (sl_drbs_setup_list_present) {
    j.write_fieldname("");
    sl_drbs_setup_list.to_json(j);
  }
  if (sl_drbs_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    sl_drbs_failed_to_be_setup_list.to_json(j);
  }
  if (requested_target_cell_global_id_present) {
    j.write_fieldname("");
    requested_target_cell_global_id.to_json(j);
  }
  if (scg_activation_status_present) {
    j.write_fieldname("");
    scg_activation_status.to_json(j);
  }
  if (uu_rlc_ch_setup_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_setup_list.to_json(j);
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    uu_rlc_ch_failed_to_be_setup_list.to_json(j);
  }
  if (pc5_rlc_ch_setup_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_setup_list.to_json(j);
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    j.write_fieldname("");
    pc5_rlc_ch_failed_to_be_setup_list.to_json(j);
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    j.write_fieldname("");
    serving_cell_mo_encoded_in_cgc_list.to_json(j);
  }
  j.end_obj();
}

ue_inactivity_notif_ies_container::ue_inactivity_notif_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  drb_activity_list(100, crit_e::reject),
  sdt_termination_request(635, crit_e::ignore)
{
}
SRSASN_CODE ue_inactivity_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += sdt_termination_request_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(drb_activity_list.pack(bref));
  if (sdt_termination_request_present) {
    HANDLE_CODE(sdt_termination_request.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_inactivity_notif_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 100: {
        nof_mandatory_ies--;
        drb_activity_list.id = id;
        HANDLE_CODE(drb_activity_list.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drb_activity_list.value.unpack(bref));
        break;
      }
      case 635: {
        sdt_termination_request_present = true;
        sdt_termination_request.id      = id;
        HANDLE_CODE(sdt_termination_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_termination_request.value.unpack(bref));
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
void ue_inactivity_notif_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  drb_activity_list.to_json(j);
  if (sdt_termination_request_present) {
    j.write_fieldname("");
    sdt_termination_request.to_json(j);
  }
  j.end_obj();
}

ul_rrc_msg_transfer_ies_container::ul_rrc_msg_transfer_ies_container() :
  gnb_cu_ue_f1ap_id(40, crit_e::reject),
  gnb_du_ue_f1ap_id(41, crit_e::reject),
  srb_id(64, crit_e::reject),
  rrc_container(50, crit_e::reject),
  sel_plmn_id(224, crit_e::reject),
  new_gnb_du_ue_f1ap_id(219, crit_e::reject)
{
}
SRSASN_CODE ul_rrc_msg_transfer_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += sel_plmn_id_present ? 1 : 0;
  nof_ies += new_gnb_du_ue_f1ap_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(gnb_cu_ue_f1ap_id.pack(bref));
  HANDLE_CODE(gnb_du_ue_f1ap_id.pack(bref));
  HANDLE_CODE(srb_id.pack(bref));
  HANDLE_CODE(rrc_container.pack(bref));
  if (sel_plmn_id_present) {
    HANDLE_CODE(sel_plmn_id.pack(bref));
  }
  if (new_gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(new_gnb_du_ue_f1ap_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_rrc_msg_transfer_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        gnb_cu_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_cu_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_cu_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        gnb_du_ue_f1ap_id.id = id;
        HANDLE_CODE(gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_ue_f1ap_id.value.unpack(bref));
        break;
      }
      case 64: {
        nof_mandatory_ies--;
        srb_id.id = id;
        HANDLE_CODE(srb_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srb_id.value.unpack(bref));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        rrc_container.id = id;
        HANDLE_CODE(rrc_container.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.value.unpack(bref));
        break;
      }
      case 224: {
        sel_plmn_id_present = true;
        sel_plmn_id.id      = id;
        HANDLE_CODE(sel_plmn_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sel_plmn_id.value.unpack(bref));
        break;
      }
      case 219: {
        new_gnb_du_ue_f1ap_id_present = true;
        new_gnb_du_ue_f1ap_id.id      = id;
        HANDLE_CODE(new_gnb_du_ue_f1ap_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(new_gnb_du_ue_f1ap_id.value.unpack(bref));
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
void ul_rrc_msg_transfer_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  gnb_cu_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  gnb_du_ue_f1ap_id.to_json(j);
  j.write_fieldname("");
  srb_id.to_json(j);
  j.write_fieldname("");
  rrc_container.to_json(j);
  if (sel_plmn_id_present) {
    j.write_fieldname("");
    sel_plmn_id.to_json(j);
  }
  if (new_gnb_du_ue_f1ap_id_present) {
    j.write_fieldname("");
    new_gnb_du_ue_f1ap_id.to_json(j);
  }
  j.end_obj();
}
