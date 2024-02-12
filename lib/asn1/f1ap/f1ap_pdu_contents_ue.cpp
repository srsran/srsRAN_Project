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

#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
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
void dl_rrc_msg_transfer_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::old_gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::srb_id:
      c = uint8_t{};
      break;
    case types::execute_dupl:
      c = execute_dupl_e{};
      break;
    case types::rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::rat_freq_prio_info:
      c = rat_freq_prio_info_c{};
      break;
    case types::rrc_delivery_status_request:
      c = rrc_delivery_status_request_e{};
      break;
    case types::ue_context_not_retrievable:
      c = ue_context_not_retrievable_e{};
      break;
    case types::redirected_rrc_msg:
      c = unbounded_octstring<true>{};
      break;
    case types::plmn_assist_info_for_net_shar:
      c = fixed_octstring<3, true>{};
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::add_rrm_prio_idx:
      c = fixed_bitstring<32, false, true>{};
      break;
    case types::srb_map_info:
      c = fixed_bitstring<16, false, true>{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "dl_rrc_msg_transfer_ies_o::value_c");
  }
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

const char* dl_rrc_msg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (0..3,...)",
                                "ExecuteDuplication",
                                "OCTET STRING",
                                "RAT-FrequencyPriorityInformation",
                                "RRCDeliveryStatusRequest",
                                "UEContextNotRetrievable",
                                "OCTET STRING",
                                "OCTET STRING",
                                "INTEGER (0..4294967295)",
                                "BIT STRING",
                                "BIT STRING"};
  return convert_enum_idx(names, 14, value, "dl_rrc_msg_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_rrc_msg_transfer_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (old_gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)47, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, old_gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)64, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, srb_id, (uint8_t)0u, (uint8_t)3u, true, true));
  }
  if (execute_dupl_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)109, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (rat_freq_prio_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)108, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rat_freq_prio_info.pack(bref));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)184, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (ue_context_not_retrievable_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)222, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ue_context_not_retrievable.pack(bref));
  }
  if (redirected_rrc_msg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)218, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(redirected_rrc_msg.pack(bref));
  }
  if (plmn_assist_info_for_net_shar_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)221, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(plmn_assist_info_for_net_shar.pack(bref));
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)217, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, new_gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)248, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }
  if (srb_map_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)597, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 47: {
        old_gnb_du_ue_f1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(old_gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 64: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(srb_id, bref, (uint8_t)0u, (uint8_t)3u, true, true));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.unpack(bref));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.unpack(bref));
        break;
      }
      case 108: {
        rat_freq_prio_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rat_freq_prio_info.unpack(bref));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.unpack(bref));
        break;
      }
      case 222: {
        ue_context_not_retrievable_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ue_context_not_retrievable.unpack(bref));
        break;
      }
      case 218: {
        redirected_rrc_msg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(redirected_rrc_msg.unpack(bref));
        break;
      }
      case 221: {
        plmn_assist_info_for_net_shar_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(plmn_assist_info_for_net_shar.unpack(bref));
        break;
      }
      case 217: {
        new_gnb_cu_ue_f1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(new_gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 248: {
        add_rrm_prio_idx_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.unpack(bref));
        break;
      }
      case 597: {
        srb_map_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(srb_map_info.unpack(bref));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  if (old_gnb_du_ue_f1ap_id_present) {
    j.write_int("id", 47);
    j.write_str("criticality", "reject");
    j.write_int("Value", old_gnb_du_ue_f1ap_id);
  }
  j.write_int("id", 64);
  j.write_str("criticality", "reject");
  j.write_int("Value", srb_id);
  if (execute_dupl_present) {
    j.write_int("id", 109);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  j.write_int("id", 50);
  j.write_str("criticality", "reject");
  j.write_str("Value", rrc_container.to_string());
  if (rat_freq_prio_info_present) {
    j.write_int("id", 108);
    j.write_str("criticality", "reject");
    rat_freq_prio_info.to_json(j);
  }
  if (rrc_delivery_status_request_present) {
    j.write_int("id", 184);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (ue_context_not_retrievable_present) {
    j.write_int("id", 222);
    j.write_str("criticality", "reject");
    j.write_str("Value", "true");
  }
  if (redirected_rrc_msg_present) {
    j.write_int("id", 218);
    j.write_str("criticality", "reject");
    j.write_str("Value", redirected_rrc_msg.to_string());
  }
  if (plmn_assist_info_for_net_shar_present) {
    j.write_int("id", 221);
    j.write_str("criticality", "ignore");
    j.write_str("Value", plmn_assist_info_for_net_shar.to_string());
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    j.write_int("id", 217);
    j.write_str("criticality", "reject");
    j.write_int("Value", new_gnb_cu_ue_f1ap_id);
  }
  if (add_rrm_prio_idx_present) {
    j.write_int("id", 248);
    j.write_str("criticality", "ignore");
    j.write_str("Value", add_rrm_prio_idx.to_string());
  }
  if (srb_map_info_present) {
    j.write_int("id", 597);
    j.write_str("criticality", "ignore");
    j.write_str("Value", srb_map_info.to_string());
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
void init_ul_rrc_msg_transfer_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::nr_cgi:
      c = nr_cgi_s{};
      break;
    case types::c_rnti:
      c = uint32_t{};
      break;
    case types::rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::du_to_cu_rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::sul_access_ind:
      c = sul_access_ind_e{};
      break;
    case types::transaction_id:
      c = uint16_t{};
      break;
    case types::ran_ue_id:
      c = fixed_octstring<8, true>{};
      break;
    case types::rrc_container_rrc_setup_complete:
      c = unbounded_octstring<true>{};
      break;
    case types::nr_red_cap_ue_ind:
      c = nr_red_cap_ue_ind_e{};
      break;
    case types::sdt_info:
      c = sdt_info_s{};
      break;
    case types::sidelink_relay_cfg:
      c = sidelink_relay_cfg_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ul_rrc_msg_transfer_ies_o::value_c");
  }
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

const char* init_ul_rrc_msg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "NRCGI",
                                "INTEGER (0..65535,...)",
                                "OCTET STRING",
                                "OCTET STRING",
                                "SULAccessIndication",
                                "INTEGER (0..255,...)",
                                "OCTET STRING",
                                "OCTET STRING",
                                "NRRedCapUEIndication",
                                "SDTInformation",
                                "SidelinkRelayConfiguration"};
  return convert_enum_idx(names, 12, value, "init_ul_rrc_msg_transfer_ies_o::value_c::types");
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
void ue_context_mod_confirm_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::res_coordination_transfer_container:
      c = unbounded_octstring<true>{};
      break;
    case types::drbs_modified_conf_list:
      c = drbs_modified_conf_list_l{};
      break;
    case types::rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::execute_dupl:
      c = execute_dupl_e{};
      break;
    case types::res_coordination_transfer_info:
      c = res_coordination_transfer_info_s{};
      break;
    case types::sl_drbs_modified_conf_list:
      c = sl_drbs_modified_conf_list_l{};
      break;
    case types::uu_rlc_ch_modified_list:
      c = uu_rlc_ch_modified_list_l{};
      break;
    case types::pc5_rlc_ch_modified_list:
      c = pc5_rlc_ch_modified_list_l{};
      break;
    case types::ue_multicast_m_rbs_confirmed_to_be_modified_list:
      c = ue_multicast_m_rbs_confirmed_to_be_modified_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_confirm_ies_o::value_c");
  }
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

const char* ue_context_mod_confirm_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "DRBs-ModifiedConf-List",
                                "OCTET STRING",
                                "CriticalityDiagnostics",
                                "ExecuteDuplication",
                                "ResourceCoordinationTransferInformation",
                                "SLDRBs-ModifiedConf-List",
                                "UuRLCChannelModifiedList",
                                "PC5RLCChannelModifiedList",
                                "UE-MulticastMRBs-ConfirmedToBeModified-List"};
  return convert_enum_idx(names, 12, value, "ue_context_mod_confirm_ies_o::value_c::types");
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
void ue_context_mod_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::requested_target_cell_global_id:
      c = nr_cgi_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_fail_ies_o::value_c");
  }
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

const char* ue_context_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics", "NRCGI"};
  return convert_enum_idx(names, 5, value, "ue_context_mod_fail_ies_o::value_c::types");
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
void ue_context_mod_refuse_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
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
      log_invalid_choice_id(type_, "ue_context_mod_refuse_ies_o::value_c");
  }
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

const char* ue_context_mod_refuse_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ue_context_mod_refuse_ies_o::value_c::types");
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
void ue_context_mod_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::sp_cell_id:
      c = nr_cgi_s{};
      break;
    case types::serv_cell_idx:
      c = uint8_t{};
      break;
    case types::sp_cell_ul_cfg:
      c = cell_ul_cfg_e{};
      break;
    case types::drx_cycle:
      c = drx_cycle_s{};
      break;
    case types::cu_to_du_rrc_info:
      c = cu_to_du_rrc_info_s{};
      break;
    case types::tx_action_ind:
      c = tx_action_ind_e{};
      break;
    case types::res_coordination_transfer_container:
      c = unbounded_octstring<true>{};
      break;
    case types::rrc_recfg_complete_ind:
      c = rrc_recfg_complete_ind_e{};
      break;
    case types::rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::scell_to_be_setup_mod_list:
      c = scell_to_be_setup_mod_list_l{};
      break;
    case types::scell_to_be_remd_list:
      c = scell_to_be_remd_list_l{};
      break;
    case types::srbs_to_be_setup_mod_list:
      c = srbs_to_be_setup_mod_list_l{};
      break;
    case types::drbs_to_be_setup_mod_list:
      c = drbs_to_be_setup_mod_list_l{};
      break;
    case types::drbs_to_be_modified_list:
      c = drbs_to_be_modified_list_l{};
      break;
    case types::srbs_to_be_released_list:
      c = srbs_to_be_released_list_l{};
      break;
    case types::drbs_to_be_released_list:
      c = drbs_to_be_released_list_l{};
      break;
    case types::inactivity_monitoring_request:
      c = inactivity_monitoring_request_e{};
      break;
    case types::rat_freq_prio_info:
      c = rat_freq_prio_info_c{};
      break;
    case types::drx_cfg_ind:
      c = drx_cfg_ind_e{};
      break;
    case types::rlc_fail_ind:
      c = rlc_fail_ind_s{};
      break;
    case types::ul_tx_direct_current_list_info:
      c = unbounded_octstring<true>{};
      break;
    case types::gnb_du_cfg_query:
      c = gnb_du_cfg_query_e{};
      break;
    case types::gnb_du_ue_ambr_ul:
      c = uint64_t{};
      break;
    case types::execute_dupl:
      c = execute_dupl_e{};
      break;
    case types::rrc_delivery_status_request:
      c = rrc_delivery_status_request_e{};
      break;
    case types::res_coordination_transfer_info:
      c = res_coordination_transfer_info_s{};
      break;
    case types::serving_cell_mo:
      c = uint8_t{};
      break;
    case types::needfor_gap:
      c = needfor_gap_e{};
      break;
    case types::full_cfg:
      c = full_cfg_e{};
      break;
    case types::add_rrm_prio_idx:
      c = fixed_bitstring<32, false, true>{};
      break;
    case types::lower_layer_presence_status_change:
      c = lower_layer_presence_status_change_e{};
      break;
    case types::bh_chs_to_be_setup_mod_list:
      c = bh_chs_to_be_setup_mod_list_l{};
      break;
    case types::bh_chs_to_be_modified_list:
      c = bh_chs_to_be_modified_list_l{};
      break;
    case types::bh_chs_to_be_released_list:
      c = bh_chs_to_be_released_list_l{};
      break;
    case types::nr_v2x_services_authorized:
      c = nr_v2x_services_authorized_s{};
      break;
    case types::ltev2x_services_authorized:
      c = ltev2x_services_authorized_s{};
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c = nr_ue_sidelink_aggr_max_bitrate_s{};
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c = lte_ue_sidelink_aggr_max_bitrate_s{};
      break;
    case types::pc5_link_ambr:
      c = uint64_t{};
      break;
    case types::sl_drbs_to_be_setup_mod_list:
      c = sl_drbs_to_be_setup_mod_list_l{};
      break;
    case types::sl_drbs_to_be_modified_list:
      c = sl_drbs_to_be_modified_list_l{};
      break;
    case types::sl_drbs_to_be_released_list:
      c = sl_drbs_to_be_released_list_l{};
      break;
    case types::conditional_intra_du_mob_info:
      c = conditional_intra_du_mob_info_s{};
      break;
    case types::f1_c_transfer_path:
      c = f1_c_transfer_path_s{};
      break;
    case types::scg_ind:
      c = scg_ind_e{};
      break;
    case types::ul_tx_direct_current_two_carrier_list_info:
      c = unbounded_octstring<true>{};
      break;
    case types::iab_conditional_rrc_msg_delivery_ind:
      c = iab_conditional_rrc_msg_delivery_ind_e{};
      break;
    case types::f1_c_transfer_path_nr_dc:
      c = f1_c_transfer_path_nr_dc_s{};
      break;
    case types::mdt_polluted_meas_ind:
      c = mdt_polluted_meas_ind_e{};
      break;
    case types::scg_activation_request:
      c = scg_activation_request_e{};
      break;
    case types::cg_sdt_query_ind:
      c = cg_sdt_query_ind_e{};
      break;
    case types::five_g_pro_se_authorized:
      c = five_g_pro_se_authorized_s{};
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c = nr_ue_sidelink_aggr_max_bitrate_s{};
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      c = uint64_t{};
      break;
    case types::upd_remote_ue_local_id:
      c = uint16_t{};
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c = uu_rlc_ch_to_be_setup_list_l{};
      break;
    case types::uu_rlc_ch_to_be_modified_list:
      c = uu_rlc_ch_to_be_modified_list_l{};
      break;
    case types::uu_rlc_ch_to_be_released_list:
      c = uu_rlc_ch_to_be_released_list_l{};
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c = pc5_rlc_ch_to_be_setup_list_l{};
      break;
    case types::pc5_rlc_ch_to_be_modified_list:
      c = pc5_rlc_ch_to_be_modified_list_l{};
      break;
    case types::pc5_rlc_ch_to_be_released_list:
      c = pc5_rlc_ch_to_be_released_list_l{};
      break;
    case types::path_switch_cfg:
      c = path_switch_cfg_s{};
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c = gnb_du_ue_slice_max_bit_rate_list_l{};
      break;
    case types::multicast_mbs_session_setup_list:
      c = multicast_mbs_session_list_l{};
      break;
    case types::multicast_mbs_session_rem_list:
      c = multicast_mbs_session_list_l{};
      break;
    case types::ue_multicast_m_rbs_to_be_setup_at_modify_list:
      c = ue_multicast_m_rbs_to_be_setup_at_modify_list_l{};
      break;
    case types::ue_multicast_m_rbs_to_be_released_list:
      c = ue_multicast_m_rbs_to_be_released_list_l{};
      break;
    case types::sldrx_cycle_list:
      c = sldrx_cycle_list_l{};
      break;
    case types::management_based_mdt_plmn_mod_list:
      c = mdt_plmn_mod_list_l{};
      break;
    case types::sdt_bearer_cfg_query_ind:
      c = sdt_bearer_cfg_query_ind_e{};
      break;
    case types::daps_ho_status:
      c = daps_ho_status_e{};
      break;
    case types::serving_cell_mo_list:
      c = serving_cell_mo_list_l{};
      break;
    case types::ul_tx_direct_current_more_carrier_info:
      c = unbounded_octstring<true>{};
      break;
    case types::cp_acmcg_info:
      c = cp_acmcg_info_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_request_ies_o::value_c");
  }
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

const char* ue_context_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "NRCGI",
                                "INTEGER (0..31,...)",
                                "CellULConfigured",
                                "DRXCycle",
                                "CUtoDURRCInformation",
                                "TransmissionActionIndicator",
                                "OCTET STRING",
                                "RRCReconfigurationCompleteIndicator",
                                "OCTET STRING",
                                "SCell-ToBeSetupMod-List",
                                "SCell-ToBeRemoved-List",
                                "SRBs-ToBeSetupMod-List",
                                "DRBs-ToBeSetupMod-List",
                                "DRBs-ToBeModified-List",
                                "SRBs-ToBeReleased-List",
                                "DRBs-ToBeReleased-List",
                                "InactivityMonitoringRequest",
                                "RAT-FrequencyPriorityInformation",
                                "DRXConfigurationIndicator",
                                "RLCFailureIndication",
                                "OCTET STRING",
                                "GNB-DUConfigurationQuery",
                                "INTEGER (0..4000000000000,...)",
                                "ExecuteDuplication",
                                "RRCDeliveryStatusRequest",
                                "ResourceCoordinationTransferInformation",
                                "INTEGER (1..64,...)",
                                "NeedforGap",
                                "FullConfiguration",
                                "BIT STRING",
                                "LowerLayerPresenceStatusChange",
                                "BHChannels-ToBeSetupMod-List",
                                "BHChannels-ToBeModified-List",
                                "BHChannels-ToBeReleased-List",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "SLDRBs-ToBeSetupMod-List",
                                "SLDRBs-ToBeModified-List",
                                "SLDRBs-ToBeReleased-List",
                                "ConditionalIntraDUMobilityInformation",
                                "F1CTransferPath",
                                "SCGIndicator",
                                "OCTET STRING",
                                "IABConditionalRRCMessageDeliveryIndication",
                                "F1CTransferPathNRDC",
                                "MDTPollutedMeasurementIndicator",
                                "SCGActivationRequest",
                                "CG-SDTQueryIndication",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "INTEGER (0..255,...)",
                                "UuRLCChannelToBeSetupList",
                                "UuRLCChannelToBeModifiedList",
                                "UuRLCChannelToBeReleasedList",
                                "PC5RLCChannelToBeSetupList",
                                "PC5RLCChannelToBeModifiedList",
                                "PC5RLCChannelToBeReleasedList",
                                "PathSwitchConfiguration",
                                "GNBDUUESliceMaximumBitRateList",
                                "MulticastMBSSessionList",
                                "MulticastMBSSessionList",
                                "UE-MulticastMRBs-ToBeSetup-atModify-List",
                                "UE-MulticastMRBs-ToBeReleased-List",
                                "SLDRXCycleList",
                                "MDTPLMNModificationList",
                                "SDTBearerConfigurationQueryIndication",
                                "DAPS-HO-Status",
                                "ServingCellMO-List",
                                "OCTET STRING",
                                "CPACMCGInformation"};
  return convert_enum_idx(names, 76, value, "ue_context_mod_request_ies_o::value_c::types");
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
void ue_context_mod_required_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::res_coordination_transfer_container:
      c = unbounded_octstring<true>{};
      break;
    case types::du_to_cu_rrc_info:
      c = du_to_cu_rrc_info_s{};
      break;
    case types::drbs_required_to_be_modified_list:
      c = drbs_required_to_be_modified_list_l{};
      break;
    case types::srbs_required_to_be_released_list:
      c = srbs_required_to_be_released_list_l{};
      break;
    case types::drbs_required_to_be_released_list:
      c = drbs_required_to_be_released_list_l{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::bh_chs_required_to_be_released_list:
      c = bh_chs_required_to_be_released_list_l{};
      break;
    case types::sl_drbs_required_to_be_modified_list:
      c = sl_drbs_required_to_be_modified_list_l{};
      break;
    case types::sl_drbs_required_to_be_released_list:
      c = sl_drbs_required_to_be_released_list_l{};
      break;
    case types::target_cells_to_cancel:
      c = target_cell_list_l{};
      break;
    case types::uu_rlc_ch_required_to_be_modified_list:
      c = uu_rlc_ch_required_to_be_modified_list_l{};
      break;
    case types::uu_rlc_ch_required_to_be_released_list:
      c = uu_rlc_ch_required_to_be_released_list_l{};
      break;
    case types::pc5_rlc_ch_required_to_be_modified_list:
      c = pc5_rlc_ch_required_to_be_modified_list_l{};
      break;
    case types::pc5_rlc_ch_required_to_be_released_list:
      c = pc5_rlc_ch_required_to_be_released_list_l{};
      break;
    case types::ue_multicast_m_rbs_required_to_be_modified_list:
      c = ue_multicast_m_rbs_required_to_be_modified_list_l{};
      break;
    case types::ue_multicast_m_rbs_required_to_be_released_list:
      c = ue_multicast_m_rbs_required_to_be_released_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_required_ies_o::value_c");
  }
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

const char* ue_context_mod_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "DUtoCURRCInformation",
                                "DRBs-Required-ToBeModified-List",
                                "SRBs-Required-ToBeReleased-List",
                                "DRBs-Required-ToBeReleased-List",
                                "Cause",
                                "BHChannels-Required-ToBeReleased-List",
                                "SLDRBs-Required-ToBeModified-List",
                                "SLDRBs-Required-ToBeReleased-List",
                                "TargetCellList",
                                "UuRLCChannelRequiredToBeModifiedList",
                                "UuRLCChannelRequiredToBeReleasedList",
                                "PC5RLCChannelRequiredToBeModifiedList",
                                "PC5RLCChannelRequiredToBeReleasedList",
                                "UE-MulticastMRBs-RequiredToBeModified-List",
                                "UE-MulticastMRBs-RequiredToBeReleased-List"};
  return convert_enum_idx(names, 18, value, "ue_context_mod_required_ies_o::value_c::types");
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
void ue_context_mod_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::res_coordination_transfer_container:
      c = unbounded_octstring<true>{};
      break;
    case types::du_to_cu_rrc_info:
      c = du_to_cu_rrc_info_s{};
      break;
    case types::drbs_setup_mod_list:
      c = drbs_setup_mod_list_l{};
      break;
    case types::drbs_modified_list:
      c = drbs_modified_list_l{};
      break;
    case types::srbs_failed_to_be_setup_mod_list:
      c = srbs_failed_to_be_setup_mod_list_l{};
      break;
    case types::drbs_failed_to_be_setup_mod_list:
      c = drbs_failed_to_be_setup_mod_list_l{};
      break;
    case types::scell_failedto_setup_mod_list:
      c = scell_failedto_setup_mod_list_l{};
      break;
    case types::drbs_failed_to_be_modified_list:
      c = drbs_failed_to_be_modified_list_l{};
      break;
    case types::inactivity_monitoring_resp:
      c = inactivity_monitoring_resp_e{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::c_rnti:
      c = uint32_t{};
      break;
    case types::associated_scell_list:
      c = associated_scell_list_l{};
      break;
    case types::srbs_setup_mod_list:
      c = srbs_setup_mod_list_l{};
      break;
    case types::srbs_modified_list:
      c = srbs_modified_list_l{};
      break;
    case types::full_cfg:
      c = full_cfg_e{};
      break;
    case types::bh_chs_setup_mod_list:
      c = bh_chs_setup_mod_list_l{};
      break;
    case types::bh_chs_modified_list:
      c = bh_chs_modified_list_l{};
      break;
    case types::bh_chs_failed_to_be_setup_mod_list:
      c = bh_chs_failed_to_be_setup_mod_list_l{};
      break;
    case types::bh_chs_failed_to_be_modified_list:
      c = bh_chs_failed_to_be_modified_list_l{};
      break;
    case types::sl_drbs_setup_mod_list:
      c = sl_drbs_setup_mod_list_l{};
      break;
    case types::sl_drbs_modified_list:
      c = sl_drbs_modified_list_l{};
      break;
    case types::sl_drbs_failed_to_be_setup_mod_list:
      c = sl_drbs_failed_to_be_setup_mod_list_l{};
      break;
    case types::sl_drbs_failed_to_be_modified_list:
      c = sl_drbs_failed_to_be_modified_list_l{};
      break;
    case types::requested_target_cell_global_id:
      c = nr_cgi_s{};
      break;
    case types::scg_activation_status:
      c = scg_activation_status_e{};
      break;
    case types::uu_rlc_ch_setup_list:
      c = uu_rlc_ch_setup_list_l{};
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c = uu_rlc_ch_failed_to_be_setup_list_l{};
      break;
    case types::uu_rlc_ch_modified_list:
      c = uu_rlc_ch_modified_list_l{};
      break;
    case types::uu_rlc_ch_failed_to_be_modified_list:
      c = uu_rlc_ch_failed_to_be_modified_list_l{};
      break;
    case types::pc5_rlc_ch_setup_list:
      c = pc5_rlc_ch_setup_list_l{};
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c = pc5_rlc_ch_failed_to_be_setup_list_l{};
      break;
    case types::pc5_rlc_ch_modified_list:
      c = pc5_rlc_ch_modified_list_l{};
      break;
    case types::pc5_rlc_ch_failed_to_be_modified_list:
      c = pc5_rlc_ch_failed_to_be_modified_list_l{};
      break;
    case types::sdt_bearer_cfg_info:
      c = sdt_bearer_cfg_info_s{};
      break;
    case types::ue_multicast_m_rbs_setup_list:
      c = ue_multicast_m_rbs_setup_list_l{};
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c = serving_cell_mo_encoded_in_cgc_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_mod_resp_ies_o::value_c");
  }
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

const char* ue_context_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "DUtoCURRCInformation",
                                "DRBs-SetupMod-List",
                                "DRBs-Modified-List",
                                "SRBs-FailedToBeSetupMod-List",
                                "DRBs-FailedToBeSetupMod-List",
                                "SCell-FailedtoSetupMod-List",
                                "DRBs-FailedToBeModified-List",
                                "InactivityMonitoringResponse",
                                "CriticalityDiagnostics",
                                "INTEGER (0..65535,...)",
                                "Associated-SCell-List",
                                "SRBs-SetupMod-List",
                                "SRBs-Modified-List",
                                "FullConfiguration",
                                "BHChannels-SetupMod-List",
                                "BHChannels-Modified-List",
                                "BHChannels-FailedToBeSetupMod-List",
                                "BHChannels-FailedToBeModified-List",
                                "SLDRBs-SetupMod-List",
                                "SLDRBs-Modified-List",
                                "SLDRBs-FailedToBeSetupMod-List",
                                "SLDRBs-FailedToBeModified-List",
                                "NRCGI",
                                "SCGActivationStatus",
                                "UuRLCChannelSetupList",
                                "UuRLCChannelFailedToBeSetupList",
                                "UuRLCChannelModifiedList",
                                "UuRLCChannelFailedToBeModifiedList",
                                "PC5RLCChannelSetupList",
                                "PC5RLCChannelFailedToBeSetupList",
                                "PC5RLCChannelModifiedList",
                                "PC5RLCChannelFailedToBeModifiedList",
                                "SDTBearerConfigurationInfo",
                                "UE-MulticastMRBs-Setup-List",
                                "ServingCellMO-encoded-in-CGC-List"};
  return convert_enum_idx(names, 38, value, "ue_context_mod_resp_ies_o::value_c::types");
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
void ue_context_release_cmd_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::srb_id:
      c = uint8_t{};
      break;
    case types::old_gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::execute_dupl:
      c = execute_dupl_e{};
      break;
    case types::rrc_delivery_status_request:
      c = rrc_delivery_status_request_e{};
      break;
    case types::target_cells_to_cancel:
      c = target_cell_list_l{};
      break;
    case types::pos_conext_rev_ind:
      c = pos_conext_rev_ind_e{};
      break;
    case types::cg_sdt_kept_ind:
      c = cg_sdt_kept_ind_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_cmd_ies_o::value_c");
  }
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

const char* ue_context_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "Cause",
                                "OCTET STRING",
                                "INTEGER (0..3,...)",
                                "INTEGER (0..4294967295)",
                                "ExecuteDuplication",
                                "RRCDeliveryStatusRequest",
                                "TargetCellList",
                                "PosConextRevIndication",
                                "CG-SDTKeptIndicator"};
  return convert_enum_idx(names, 11, value, "ue_context_release_cmd_ies_o::value_c::types");
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
void ue_context_release_complete_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_complete_ies_o::value_c");
  }
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

const char* ue_context_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "ue_context_release_complete_ies_o::value_c::types");
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
void ue_context_release_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::target_cells_to_cancel:
      c = target_cell_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_release_request_ies_o::value_c");
  }
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

const char* ue_context_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "TargetCellList"};
  return convert_enum_idx(names, 4, value, "ue_context_release_request_ies_o::value_c::types");
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
void ue_context_setup_fail_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::cause:
      c = cause_c{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::potential_sp_cell_list:
      c = potential_sp_cell_list_l{};
      break;
    case types::requested_target_cell_global_id:
      c = nr_cgi_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_fail_ies_o::value_c");
  }
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

const char* ue_context_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "Cause",
                                "CriticalityDiagnostics",
                                "Potential-SpCell-List",
                                "NRCGI"};
  return convert_enum_idx(names, 6, value, "ue_context_setup_fail_ies_o::value_c::types");
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
void ue_context_setup_request_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::sp_cell_id:
      c = nr_cgi_s{};
      break;
    case types::serv_cell_idx:
      c = uint8_t{};
      break;
    case types::sp_cell_ul_cfg:
      c = cell_ul_cfg_e{};
      break;
    case types::cu_to_du_rrc_info:
      c = cu_to_du_rrc_info_s{};
      break;
    case types::candidate_sp_cell_list:
      c = candidate_sp_cell_list_l{};
      break;
    case types::drx_cycle:
      c = drx_cycle_s{};
      break;
    case types::res_coordination_transfer_container:
      c = unbounded_octstring<true>{};
      break;
    case types::scell_to_be_setup_list:
      c = scell_to_be_setup_list_l{};
      break;
    case types::srbs_to_be_setup_list:
      c = srbs_to_be_setup_list_l{};
      break;
    case types::drbs_to_be_setup_list:
      c = drbs_to_be_setup_list_l{};
      break;
    case types::inactivity_monitoring_request:
      c = inactivity_monitoring_request_e{};
      break;
    case types::rat_freq_prio_info:
      c = rat_freq_prio_info_c{};
      break;
    case types::rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::masked_imeisv:
      c = fixed_bitstring<64, false, true>{};
      break;
    case types::serving_plmn:
      c = fixed_octstring<3, true>{};
      break;
    case types::gnb_du_ue_ambr_ul:
      c = uint64_t{};
      break;
    case types::rrc_delivery_status_request:
      c = rrc_delivery_status_request_e{};
      break;
    case types::res_coordination_transfer_info:
      c = res_coordination_transfer_info_s{};
      break;
    case types::serving_cell_mo:
      c = uint8_t{};
      break;
    case types::new_gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::ran_ue_id:
      c = fixed_octstring<8, true>{};
      break;
    case types::trace_activation:
      c = trace_activation_s{};
      break;
    case types::add_rrm_prio_idx:
      c = fixed_bitstring<32, false, true>{};
      break;
    case types::bh_chs_to_be_setup_list:
      c = bh_chs_to_be_setup_list_l{};
      break;
    case types::cfg_bap_address:
      c = fixed_bitstring<10, false, true>{};
      break;
    case types::nr_v2x_services_authorized:
      c = nr_v2x_services_authorized_s{};
      break;
    case types::ltev2x_services_authorized:
      c = ltev2x_services_authorized_s{};
      break;
    case types::nr_ue_sidelink_aggr_max_bitrate:
      c = nr_ue_sidelink_aggr_max_bitrate_s{};
      break;
    case types::lte_ue_sidelink_aggr_max_bitrate:
      c = lte_ue_sidelink_aggr_max_bitrate_s{};
      break;
    case types::pc5_link_ambr:
      c = uint64_t{};
      break;
    case types::sl_drbs_to_be_setup_list:
      c = sl_drbs_to_be_setup_list_l{};
      break;
    case types::conditional_inter_du_mob_info:
      c = conditional_inter_du_mob_info_s{};
      break;
    case types::management_based_mdt_plmn_list:
      c = mdt_plmn_list_l{};
      break;
    case types::serving_n_id:
      c = fixed_bitstring<44, false, true>{};
      break;
    case types::f1_c_transfer_path:
      c = f1_c_transfer_path_s{};
      break;
    case types::f1_c_transfer_path_nr_dc:
      c = f1_c_transfer_path_nr_dc_s{};
      break;
    case types::mdt_polluted_meas_ind:
      c = mdt_polluted_meas_ind_e{};
      break;
    case types::scg_activation_request:
      c = scg_activation_request_e{};
      break;
    case types::cg_sdt_session_info_old:
      c = cg_sdt_session_info_s{};
      break;
    case types::five_g_pro_se_authorized:
      c = five_g_pro_se_authorized_s{};
      break;
    case types::five_g_pro_se_ue_pc5_aggr_max_bitrate:
      c = nr_ue_sidelink_aggr_max_bitrate_s{};
      break;
    case types::five_g_pro_se_pc5_link_ambr:
      c = uint64_t{};
      break;
    case types::uu_rlc_ch_to_be_setup_list:
      c = uu_rlc_ch_to_be_setup_list_l{};
      break;
    case types::pc5_rlc_ch_to_be_setup_list:
      c = pc5_rlc_ch_to_be_setup_list_l{};
      break;
    case types::path_switch_cfg:
      c = path_switch_cfg_s{};
      break;
    case types::gnb_du_ue_slice_max_bit_rate_list:
      c = gnb_du_ue_slice_max_bit_rate_list_l{};
      break;
    case types::multicast_mbs_session_setup_list:
      c = multicast_mbs_session_list_l{};
      break;
    case types::ue_multicast_m_rbs_to_be_setup_list:
      c = ue_multicast_m_rbs_to_be_setup_list_l{};
      break;
    case types::serving_cell_mo_list:
      c = serving_cell_mo_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_request_ies_o::value_c");
  }
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

const char* ue_context_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "NRCGI",
                                "INTEGER (0..31,...)",
                                "CellULConfigured",
                                "CUtoDURRCInformation",
                                "Candidate-SpCell-List",
                                "DRXCycle",
                                "OCTET STRING",
                                "SCell-ToBeSetup-List",
                                "SRBs-ToBeSetup-List",
                                "DRBs-ToBeSetup-List",
                                "InactivityMonitoringRequest",
                                "RAT-FrequencyPriorityInformation",
                                "OCTET STRING",
                                "BIT STRING",
                                "OCTET STRING",
                                "INTEGER (0..4000000000000,...)",
                                "RRCDeliveryStatusRequest",
                                "ResourceCoordinationTransferInformation",
                                "INTEGER (1..64,...)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "TraceActivation",
                                "BIT STRING",
                                "BHChannels-ToBeSetup-List",
                                "BIT STRING",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "SLDRBs-ToBeSetup-List",
                                "ConditionalInterDUMobilityInformation",
                                "MDTPLMNList",
                                "BIT STRING",
                                "F1CTransferPath",
                                "F1CTransferPathNRDC",
                                "MDTPollutedMeasurementIndicator",
                                "SCGActivationRequest",
                                "CG-SDTSessionInfo",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "UuRLCChannelToBeSetupList",
                                "PC5RLCChannelToBeSetupList",
                                "PathSwitchConfiguration",
                                "GNBDUUESliceMaximumBitRateList",
                                "MulticastMBSSessionList",
                                "UE-MulticastMRBs-ToBeSetup-List",
                                "ServingCellMO-List"};
  return convert_enum_idx(names, 51, value, "ue_context_setup_request_ies_o::value_c::types");
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
void ue_context_setup_resp_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::du_to_cu_rrc_info:
      c = du_to_cu_rrc_info_s{};
      break;
    case types::c_rnti:
      c = uint32_t{};
      break;
    case types::res_coordination_transfer_container:
      c = unbounded_octstring<true>{};
      break;
    case types::full_cfg:
      c = full_cfg_e{};
      break;
    case types::drbs_setup_list:
      c = drbs_setup_list_l{};
      break;
    case types::srbs_failed_to_be_setup_list:
      c = srbs_failed_to_be_setup_list_l{};
      break;
    case types::drbs_failed_to_be_setup_list:
      c = drbs_failed_to_be_setup_list_l{};
      break;
    case types::scell_failedto_setup_list:
      c = scell_failedto_setup_list_l{};
      break;
    case types::inactivity_monitoring_resp:
      c = inactivity_monitoring_resp_e{};
      break;
    case types::crit_diagnostics:
      c = crit_diagnostics_s{};
      break;
    case types::srbs_setup_list:
      c = srbs_setup_list_l{};
      break;
    case types::bh_chs_setup_list:
      c = bh_chs_setup_list_l{};
      break;
    case types::bh_chs_failed_to_be_setup_list:
      c = bh_chs_failed_to_be_setup_list_l{};
      break;
    case types::sl_drbs_setup_list:
      c = sl_drbs_setup_list_l{};
      break;
    case types::sl_drbs_failed_to_be_setup_list:
      c = sl_drbs_failed_to_be_setup_list_l{};
      break;
    case types::requested_target_cell_global_id:
      c = nr_cgi_s{};
      break;
    case types::scg_activation_status:
      c = scg_activation_status_e{};
      break;
    case types::uu_rlc_ch_setup_list:
      c = uu_rlc_ch_setup_list_l{};
      break;
    case types::uu_rlc_ch_failed_to_be_setup_list:
      c = uu_rlc_ch_failed_to_be_setup_list_l{};
      break;
    case types::pc5_rlc_ch_setup_list:
      c = pc5_rlc_ch_setup_list_l{};
      break;
    case types::pc5_rlc_ch_failed_to_be_setup_list:
      c = pc5_rlc_ch_failed_to_be_setup_list_l{};
      break;
    case types::serving_cell_mo_encoded_in_cgc_list:
      c = serving_cell_mo_encoded_in_cgc_list_l{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_context_setup_resp_ies_o::value_c");
  }
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

const char* ue_context_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "DUtoCURRCInformation",
                                "INTEGER (0..65535,...)",
                                "OCTET STRING",
                                "FullConfiguration",
                                "DRBs-Setup-List",
                                "SRBs-FailedToBeSetup-List",
                                "DRBs-FailedToBeSetup-List",
                                "SCell-FailedtoSetup-List",
                                "InactivityMonitoringResponse",
                                "CriticalityDiagnostics",
                                "SRBs-Setup-List",
                                "BHChannels-Setup-List",
                                "BHChannels-FailedToBeSetup-List",
                                "SLDRBs-Setup-List",
                                "SLDRBs-FailedToBeSetup-List",
                                "NRCGI",
                                "SCGActivationStatus",
                                "UuRLCChannelSetupList",
                                "UuRLCChannelFailedToBeSetupList",
                                "PC5RLCChannelSetupList",
                                "PC5RLCChannelFailedToBeSetupList",
                                "ServingCellMO-encoded-in-CGC-List"};
  return convert_enum_idx(names, 24, value, "ue_context_setup_resp_ies_o::value_c::types");
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
void ue_inactivity_notif_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::drb_activity_list:
      c = drb_activity_list_l{};
      break;
    case types::sdt_termination_request:
      c = sdt_termination_request_e{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ue_inactivity_notif_ies_o::value_c");
  }
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

const char* ue_inactivity_notif_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "DRB-Activity-List", "SDT-Termination-Request"};
  return convert_enum_idx(names, 4, value, "ue_inactivity_notif_ies_o::value_c::types");
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
void ul_rrc_msg_transfer_ies_o::value_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::srb_id:
      c = uint8_t{};
      break;
    case types::rrc_container:
      c = unbounded_octstring<true>{};
      break;
    case types::sel_plmn_id:
      c = fixed_octstring<3, true>{};
      break;
    case types::new_gnb_du_ue_f1ap_id:
      c = uint64_t{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_rrc_msg_transfer_ies_o::value_c");
  }
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

const char* ul_rrc_msg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (0..3,...)",
                                "OCTET STRING",
                                "OCTET STRING",
                                "INTEGER (0..4294967295)"};
  return convert_enum_idx(names, 6, value, "ul_rrc_msg_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<init_ul_rrc_msg_transfer_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)111, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(nr_cgi.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)95, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, c_rnti, (uint32_t)0u, (uint32_t)65535u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (du_to_cu_rrc_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)128, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(du_to_cu_rrc_container.pack(bref));
  }
  if (sul_access_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)178, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sul_access_ind.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)78, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, transaction_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (ran_ue_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)226, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ran_ue_id.pack(bref));
  }
  if (rrc_container_rrc_setup_complete_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)241, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container_rrc_setup_complete.pack(bref));
  }
  if (nr_red_cap_ue_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)578, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(nr_red_cap_ue_ind.pack(bref));
  }
  if (sdt_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)592, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sdt_info.pack(bref));
  }
  if (sidelink_relay_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)617, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 111: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_cgi.unpack(bref));
        break;
      }
      case 95: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(c_rnti, bref, (uint32_t)0u, (uint32_t)65535u, true, true));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.unpack(bref));
        break;
      }
      case 128: {
        du_to_cu_rrc_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_container.unpack(bref));
        break;
      }
      case 178: {
        sul_access_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sul_access_ind.unpack(bref));
        break;
      }
      case 78: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(transaction_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 226: {
        ran_ue_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ran_ue_id.unpack(bref));
        break;
      }
      case 241: {
        rrc_container_rrc_setup_complete_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container_rrc_setup_complete.unpack(bref));
        break;
      }
      case 578: {
        nr_red_cap_ue_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_red_cap_ue_ind.unpack(bref));
        break;
      }
      case 592: {
        sdt_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_info.unpack(bref));
        break;
      }
      case 617: {
        sidelink_relay_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sidelink_relay_cfg.unpack(bref));
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
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 111);
  j.write_str("criticality", "reject");
  nr_cgi.to_json(j);
  j.write_int("id", 95);
  j.write_str("criticality", "reject");
  j.write_int("Value", c_rnti);
  j.write_int("id", 50);
  j.write_str("criticality", "reject");
  j.write_str("Value", rrc_container.to_string());
  if (du_to_cu_rrc_container_present) {
    j.write_int("id", 128);
    j.write_str("criticality", "reject");
    j.write_str("Value", du_to_cu_rrc_container.to_string());
  }
  if (sul_access_ind_present) {
    j.write_int("id", 178);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  j.write_int("id", 78);
  j.write_str("criticality", "ignore");
  j.write_int("Value", transaction_id);
  if (ran_ue_id_present) {
    j.write_int("id", 226);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ran_ue_id.to_string());
  }
  if (rrc_container_rrc_setup_complete_present) {
    j.write_int("id", 241);
    j.write_str("criticality", "ignore");
    j.write_str("Value", rrc_container_rrc_setup_complete.to_string());
  }
  if (nr_red_cap_ue_ind_present) {
    j.write_int("id", 578);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (sdt_info_present) {
    j.write_int("id", 592);
    j.write_str("criticality", "ignore");
    sdt_info.to_json(j);
  }
  if (sidelink_relay_cfg_present) {
    j.write_int("id", 617);
    j.write_str("criticality", "ignore");
    sidelink_relay_cfg.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_mod_confirm_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (drbs_modified_conf_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)19, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_modified_conf_list, 1, 64, true));
  }
  if (rrc_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (execute_dupl_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)109, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  if (res_coordination_transfer_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)195, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_info.pack(bref));
  }
  if (sl_drbs_modified_conf_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)337, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_modified_conf_list, 1, 512, true));
  }
  if (uu_rlc_ch_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)604, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_modified_list, 1, 32, true));
  }
  if (pc5_rlc_ch_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)616, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_modified_list, 1, 512, true));
  }
  if (ue_multicast_m_rbs_confirmed_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)653, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_multicast_m_rbs_confirmed_to_be_modified_list, 1, 64, true));
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.unpack(bref));
        break;
      }
      case 19: {
        drbs_modified_conf_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_modified_conf_list, bref, 1, 64, true));
        break;
      }
      case 50: {
        rrc_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.unpack(bref));
        break;
      }
      case 195: {
        res_coordination_transfer_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_info.unpack(bref));
        break;
      }
      case 337: {
        sl_drbs_modified_conf_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_modified_conf_list, bref, 1, 512, true));
        break;
      }
      case 604: {
        uu_rlc_ch_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_modified_list, bref, 1, 32, true));
        break;
      }
      case 616: {
        pc5_rlc_ch_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_modified_list, bref, 1, 512, true));
        break;
      }
      case 653: {
        ue_multicast_m_rbs_confirmed_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_multicast_m_rbs_confirmed_to_be_modified_list, bref, 1, 64, true));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  if (res_coordination_transfer_container_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "ignore");
    j.write_str("Value", res_coordination_transfer_container.to_string());
  }
  if (drbs_modified_conf_list_present) {
    j.write_int("id", 19);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drbs_modified_conf_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (rrc_container_present) {
    j.write_int("id", 50);
    j.write_str("criticality", "ignore");
    j.write_str("Value", rrc_container.to_string());
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 7);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (execute_dupl_present) {
    j.write_int("id", 109);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (res_coordination_transfer_info_present) {
    j.write_int("id", 195);
    j.write_str("criticality", "ignore");
    res_coordination_transfer_info.to_json(j);
  }
  if (sl_drbs_modified_conf_list_present) {
    j.write_int("id", 337);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_modified_conf_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_modified_list_present) {
    j.write_int("id", 604);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_modified_list_present) {
    j.write_int("id", 616);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ue_multicast_m_rbs_confirmed_to_be_modified_list_present) {
    j.write_int("id", 653);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ue_multicast_m_rbs_confirmed_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_mod_fail_ies_o>;

SRSASN_CODE ue_context_mod_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += requested_target_cell_global_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)376, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.unpack(bref));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 7);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (requested_target_cell_global_id_present) {
    j.write_int("id", 376);
    j.write_str("criticality", "reject");
    requested_target_cell_global_id.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_mod_refuse_ies_o>;

SRSASN_CODE ue_context_mod_refuse_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 7: {
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
void ue_context_mod_refuse_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 7);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_mod_request_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (sp_cell_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)63, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sp_cell_id.pack(bref));
  }
  if (serv_cell_idx_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)107, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u, true, true));
  }
  if (sp_cell_ul_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)96, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sp_cell_ul_cfg.pack(bref));
  }
  if (drx_cycle_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)38, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(drx_cycle.pack(bref));
  }
  if (cu_to_du_rrc_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cu_to_du_rrc_info.pack(bref));
  }
  if (tx_action_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)79, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(tx_action_ind.pack(bref));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (rrc_recfg_complete_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)87, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_recfg_complete_ind.pack(bref));
  }
  if (rrc_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (scell_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)56, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_be_setup_mod_list, 1, 32, true));
  }
  if (scell_to_be_remd_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)52, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_be_remd_list, 1, 32, true));
  }
  if (srbs_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)76, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_to_be_setup_mod_list, 1, 8, true));
  }
  if (drbs_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)37, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_to_be_setup_mod_list, 1, 64, true));
  }
  if (drbs_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)31, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_to_be_modified_list, 1, 64, true));
  }
  if (srbs_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)72, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_to_be_released_list, 1, 8, true));
  }
  if (drbs_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)33, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_to_be_released_list, 1, 64, true));
  }
  if (inactivity_monitoring_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)97, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(inactivity_monitoring_request.pack(bref));
  }
  if (rat_freq_prio_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)108, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rat_freq_prio_info.pack(bref));
  }
  if (drx_cfg_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)159, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(drx_cfg_ind.pack(bref));
  }
  if (rlc_fail_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)174, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rlc_fail_ind.pack(bref));
  }
  if (ul_tx_direct_current_list_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)175, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ul_tx_direct_current_list_info.pack(bref));
  }
  if (gnb_du_cfg_query_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)162, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(gnb_du_cfg_query.pack(bref));
  }
  if (gnb_du_ue_ambr_ul_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)158, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_ambr_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (execute_dupl_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)109, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)184, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (res_coordination_transfer_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)195, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_info.pack(bref));
  }
  if (serving_cell_mo_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)182, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, serving_cell_mo, (uint8_t)1u, (uint8_t)64u, true, true));
  }
  if (needfor_gap_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)215, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(needfor_gap.pack(bref));
  }
  if (full_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)94, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(full_cfg.pack(bref));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)248, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }
  if (lower_layer_presence_status_change_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)253, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(lower_layer_presence_status_change.pack(bref));
  }
  if (bh_chs_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)267, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_to_be_setup_mod_list, 1, 65536, true));
  }
  if (bh_chs_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)263, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_to_be_modified_list, 1, 65536, true));
  }
  if (bh_chs_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)265, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_to_be_released_list, 1, 65536, true));
  }
  if (nr_v2x_services_authorized_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)306, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(nr_v2x_services_authorized.pack(bref));
  }
  if (ltev2x_services_authorized_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)307, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ltev2x_services_authorized.pack(bref));
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)308, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)309, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (pc5_link_ambr_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)340, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, pc5_link_ambr, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (sl_drbs_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)332, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_to_be_setup_mod_list, 1, 512, true));
  }
  if (sl_drbs_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)326, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_to_be_modified_list, 1, 512, true));
  }
  if (sl_drbs_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)328, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_to_be_released_list, 1, 512, true));
  }
  if (conditional_intra_du_mob_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)374, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(conditional_intra_du_mob_info.pack(bref));
  }
  if (f1_c_transfer_path_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)428, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(f1_c_transfer_path.pack(bref));
  }
  if (scg_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)432, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(scg_ind.pack(bref));
  }
  if (ul_tx_direct_current_two_carrier_list_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)684, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ul_tx_direct_current_two_carrier_list_info.pack(bref));
  }
  if (iab_conditional_rrc_msg_delivery_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)510, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(iab_conditional_rrc_msg_delivery_ind.pack(bref));
  }
  if (f1_c_transfer_path_nr_dc_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)511, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(f1_c_transfer_path_nr_dc.pack(bref));
  }
  if (mdt_polluted_meas_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)536, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(mdt_polluted_meas_ind.pack(bref));
  }
  if (scg_activation_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)547, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(scg_activation_request.pack(bref));
  }
  if (cg_sdt_query_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)586, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cg_sdt_query_ind.pack(bref));
  }
  if (five_g_pro_se_authorized_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)594, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(five_g_pro_se_authorized.pack(bref));
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)595, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.pack(bref));
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)596, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, five_g_pro_se_pc5_link_ambr, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (upd_remote_ue_local_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)618, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, upd_remote_ue_local_id, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)599, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_to_be_setup_list, 1, 32, true));
  }
  if (uu_rlc_ch_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)600, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_to_be_modified_list, 1, 32, true));
  }
  if (uu_rlc_ch_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)601, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_to_be_released_list, 1, 32, true));
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)608, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_to_be_setup_list, 1, 512, true));
  }
  if (pc5_rlc_ch_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)609, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_to_be_modified_list, 1, 512, true));
  }
  if (pc5_rlc_ch_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)610, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_to_be_released_list, 1, 512, true));
  }
  if (path_switch_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)619, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(path_switch_cfg.pack(bref));
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)626, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_du_ue_slice_max_bit_rate_list, 1, 8, true));
  }
  if (multicast_mbs_session_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)632, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, multicast_mbs_session_setup_list, 1, 256, true));
  }
  if (multicast_mbs_session_rem_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)633, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, multicast_mbs_session_rem_list, 1, 256, true));
  }
  if (ue_multicast_m_rbs_to_be_setup_at_modify_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)685, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_multicast_m_rbs_to_be_setup_at_modify_list, 1, 64, true));
  }
  if (ue_multicast_m_rbs_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)628, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_multicast_m_rbs_to_be_released_list, 1, 64, true));
  }
  if (sldrx_cycle_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)643, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sldrx_cycle_list, 1, 32, true));
  }
  if (management_based_mdt_plmn_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)647, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, management_based_mdt_plmn_mod_list, 0, 16, true));
  }
  if (sdt_bearer_cfg_query_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)675, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sdt_bearer_cfg_query_ind.pack(bref));
  }
  if (daps_ho_status_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)683, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(daps_ho_status.pack(bref));
  }
  if (serving_cell_mo_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)695, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, serving_cell_mo_list, 1, 16, true));
  }
  if (ul_tx_direct_current_more_carrier_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)690, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ul_tx_direct_current_more_carrier_info.pack(bref));
  }
  if (cp_acmcg_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)691, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 63: {
        sp_cell_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_id.unpack(bref));
        break;
      }
      case 107: {
        serv_cell_idx_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u, true, true));
        break;
      }
      case 96: {
        sp_cell_ul_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_ul_cfg.unpack(bref));
        break;
      }
      case 38: {
        drx_cycle_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drx_cycle.unpack(bref));
        break;
      }
      case 9: {
        cu_to_du_rrc_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cu_to_du_rrc_info.unpack(bref));
        break;
      }
      case 79: {
        tx_action_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(tx_action_ind.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.unpack(bref));
        break;
      }
      case 87: {
        rrc_recfg_complete_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_recfg_complete_ind.unpack(bref));
        break;
      }
      case 50: {
        rrc_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.unpack(bref));
        break;
      }
      case 56: {
        scell_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(scell_to_be_setup_mod_list, bref, 1, 32, true));
        break;
      }
      case 52: {
        scell_to_be_remd_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(scell_to_be_remd_list, bref, 1, 32, true));
        break;
      }
      case 76: {
        srbs_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_to_be_setup_mod_list, bref, 1, 8, true));
        break;
      }
      case 37: {
        drbs_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_to_be_setup_mod_list, bref, 1, 64, true));
        break;
      }
      case 31: {
        drbs_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_to_be_modified_list, bref, 1, 64, true));
        break;
      }
      case 72: {
        srbs_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_to_be_released_list, bref, 1, 8, true));
        break;
      }
      case 33: {
        drbs_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_to_be_released_list, bref, 1, 64, true));
        break;
      }
      case 97: {
        inactivity_monitoring_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_request.unpack(bref));
        break;
      }
      case 108: {
        rat_freq_prio_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rat_freq_prio_info.unpack(bref));
        break;
      }
      case 159: {
        drx_cfg_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drx_cfg_ind.unpack(bref));
        break;
      }
      case 174: {
        rlc_fail_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rlc_fail_ind.unpack(bref));
        break;
      }
      case 175: {
        ul_tx_direct_current_list_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ul_tx_direct_current_list_info.unpack(bref));
        break;
      }
      case 162: {
        gnb_du_cfg_query_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(gnb_du_cfg_query.unpack(bref));
        break;
      }
      case 158: {
        gnb_du_ue_ambr_ul_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_ambr_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.unpack(bref));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.unpack(bref));
        break;
      }
      case 195: {
        res_coordination_transfer_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_info.unpack(bref));
        break;
      }
      case 182: {
        serving_cell_mo_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(serving_cell_mo, bref, (uint8_t)1u, (uint8_t)64u, true, true));
        break;
      }
      case 215: {
        needfor_gap_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(needfor_gap.unpack(bref));
        break;
      }
      case 94: {
        full_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(full_cfg.unpack(bref));
        break;
      }
      case 248: {
        add_rrm_prio_idx_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.unpack(bref));
        break;
      }
      case 253: {
        lower_layer_presence_status_change_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(lower_layer_presence_status_change.unpack(bref));
        break;
      }
      case 267: {
        bh_chs_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_to_be_setup_mod_list, bref, 1, 65536, true));
        break;
      }
      case 263: {
        bh_chs_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_to_be_modified_list, bref, 1, 65536, true));
        break;
      }
      case 265: {
        bh_chs_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_to_be_released_list, bref, 1, 65536, true));
        break;
      }
      case 306: {
        nr_v2x_services_authorized_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_v2x_services_authorized.unpack(bref));
        break;
      }
      case 307: {
        ltev2x_services_authorized_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ltev2x_services_authorized.unpack(bref));
        break;
      }
      case 308: {
        nr_ue_sidelink_aggr_max_bitrate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.unpack(bref));
        break;
      }
      case 309: {
        lte_ue_sidelink_aggr_max_bitrate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.unpack(bref));
        break;
      }
      case 340: {
        pc5_link_ambr_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(pc5_link_ambr, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 332: {
        sl_drbs_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_to_be_setup_mod_list, bref, 1, 512, true));
        break;
      }
      case 326: {
        sl_drbs_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_to_be_modified_list, bref, 1, 512, true));
        break;
      }
      case 328: {
        sl_drbs_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_to_be_released_list, bref, 1, 512, true));
        break;
      }
      case 374: {
        conditional_intra_du_mob_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(conditional_intra_du_mob_info.unpack(bref));
        break;
      }
      case 428: {
        f1_c_transfer_path_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path.unpack(bref));
        break;
      }
      case 432: {
        scg_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_ind.unpack(bref));
        break;
      }
      case 684: {
        ul_tx_direct_current_two_carrier_list_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ul_tx_direct_current_two_carrier_list_info.unpack(bref));
        break;
      }
      case 510: {
        iab_conditional_rrc_msg_delivery_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(iab_conditional_rrc_msg_delivery_ind.unpack(bref));
        break;
      }
      case 511: {
        f1_c_transfer_path_nr_dc_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path_nr_dc.unpack(bref));
        break;
      }
      case 536: {
        mdt_polluted_meas_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(mdt_polluted_meas_ind.unpack(bref));
        break;
      }
      case 547: {
        scg_activation_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_request.unpack(bref));
        break;
      }
      case 586: {
        cg_sdt_query_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cg_sdt_query_ind.unpack(bref));
        break;
      }
      case 594: {
        five_g_pro_se_authorized_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_authorized.unpack(bref));
        break;
      }
      case 595: {
        five_g_pro_se_ue_pc5_aggr_max_bitrate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.unpack(bref));
        break;
      }
      case 596: {
        five_g_pro_se_pc5_link_ambr_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(
            unpack_integer(five_g_pro_se_pc5_link_ambr, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 618: {
        upd_remote_ue_local_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(upd_remote_ue_local_id, bref, (uint16_t)0u, (uint16_t)255u, true, true));
        break;
      }
      case 599: {
        uu_rlc_ch_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_to_be_setup_list, bref, 1, 32, true));
        break;
      }
      case 600: {
        uu_rlc_ch_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_to_be_modified_list, bref, 1, 32, true));
        break;
      }
      case 601: {
        uu_rlc_ch_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_to_be_released_list, bref, 1, 32, true));
        break;
      }
      case 608: {
        pc5_rlc_ch_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_to_be_setup_list, bref, 1, 512, true));
        break;
      }
      case 609: {
        pc5_rlc_ch_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_to_be_modified_list, bref, 1, 512, true));
        break;
      }
      case 610: {
        pc5_rlc_ch_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_to_be_released_list, bref, 1, 512, true));
        break;
      }
      case 619: {
        path_switch_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(path_switch_cfg.unpack(bref));
        break;
      }
      case 626: {
        gnb_du_ue_slice_max_bit_rate_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_du_ue_slice_max_bit_rate_list, bref, 1, 8, true));
        break;
      }
      case 632: {
        multicast_mbs_session_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(multicast_mbs_session_setup_list, bref, 1, 256, true));
        break;
      }
      case 633: {
        multicast_mbs_session_rem_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(multicast_mbs_session_rem_list, bref, 1, 256, true));
        break;
      }
      case 685: {
        ue_multicast_m_rbs_to_be_setup_at_modify_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_multicast_m_rbs_to_be_setup_at_modify_list, bref, 1, 64, true));
        break;
      }
      case 628: {
        ue_multicast_m_rbs_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_multicast_m_rbs_to_be_released_list, bref, 1, 64, true));
        break;
      }
      case 643: {
        sldrx_cycle_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sldrx_cycle_list, bref, 1, 32, true));
        break;
      }
      case 647: {
        management_based_mdt_plmn_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(management_based_mdt_plmn_mod_list, bref, 0, 16, true));
        break;
      }
      case 675: {
        sdt_bearer_cfg_query_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_bearer_cfg_query_ind.unpack(bref));
        break;
      }
      case 683: {
        daps_ho_status_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(daps_ho_status.unpack(bref));
        break;
      }
      case 695: {
        serving_cell_mo_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(serving_cell_mo_list, bref, 1, 16, true));
        break;
      }
      case 690: {
        ul_tx_direct_current_more_carrier_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ul_tx_direct_current_more_carrier_info.unpack(bref));
        break;
      }
      case 691: {
        cp_acmcg_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cp_acmcg_info.unpack(bref));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  if (sp_cell_id_present) {
    j.write_int("id", 63);
    j.write_str("criticality", "ignore");
    sp_cell_id.to_json(j);
  }
  if (serv_cell_idx_present) {
    j.write_int("id", 107);
    j.write_str("criticality", "reject");
    j.write_int("Value", serv_cell_idx);
  }
  if (sp_cell_ul_cfg_present) {
    j.write_int("id", 96);
    j.write_str("criticality", "ignore");
    j.write_str("Value", sp_cell_ul_cfg.to_string());
  }
  if (drx_cycle_present) {
    j.write_int("id", 38);
    j.write_str("criticality", "ignore");
    drx_cycle.to_json(j);
  }
  if (cu_to_du_rrc_info_present) {
    j.write_int("id", 9);
    j.write_str("criticality", "reject");
    cu_to_du_rrc_info.to_json(j);
  }
  if (tx_action_ind_present) {
    j.write_int("id", 79);
    j.write_str("criticality", "ignore");
    j.write_str("Value", tx_action_ind.to_string());
  }
  if (res_coordination_transfer_container_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "ignore");
    j.write_str("Value", res_coordination_transfer_container.to_string());
  }
  if (rrc_recfg_complete_ind_present) {
    j.write_int("id", 87);
    j.write_str("criticality", "ignore");
    j.write_str("Value", rrc_recfg_complete_ind.to_string());
  }
  if (rrc_container_present) {
    j.write_int("id", 50);
    j.write_str("criticality", "reject");
    j.write_str("Value", rrc_container.to_string());
  }
  if (scell_to_be_setup_mod_list_present) {
    j.write_int("id", 56);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : scell_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_to_be_remd_list_present) {
    j.write_int("id", 52);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : scell_to_be_remd_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_to_be_setup_mod_list_present) {
    j.write_int("id", 76);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : srbs_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_to_be_setup_mod_list_present) {
    j.write_int("id", 37);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drbs_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_to_be_modified_list_present) {
    j.write_int("id", 31);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drbs_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_to_be_released_list_present) {
    j.write_int("id", 72);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : srbs_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_to_be_released_list_present) {
    j.write_int("id", 33);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drbs_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inactivity_monitoring_request_present) {
    j.write_int("id", 97);
    j.write_str("criticality", "reject");
    j.write_str("Value", "true");
  }
  if (rat_freq_prio_info_present) {
    j.write_int("id", 108);
    j.write_str("criticality", "reject");
    rat_freq_prio_info.to_json(j);
  }
  if (drx_cfg_ind_present) {
    j.write_int("id", 159);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "release");
  }
  if (rlc_fail_ind_present) {
    j.write_int("id", 174);
    j.write_str("criticality", "ignore");
    rlc_fail_ind.to_json(j);
  }
  if (ul_tx_direct_current_list_info_present) {
    j.write_int("id", 175);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ul_tx_direct_current_list_info.to_string());
  }
  if (gnb_du_cfg_query_present) {
    j.write_int("id", 162);
    j.write_str("criticality", "reject");
    j.write_str("Value", "true");
  }
  if (gnb_du_ue_ambr_ul_present) {
    j.write_int("id", 158);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_du_ue_ambr_ul);
  }
  if (execute_dupl_present) {
    j.write_int("id", 109);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (rrc_delivery_status_request_present) {
    j.write_int("id", 184);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (res_coordination_transfer_info_present) {
    j.write_int("id", 195);
    j.write_str("criticality", "ignore");
    res_coordination_transfer_info.to_json(j);
  }
  if (serving_cell_mo_present) {
    j.write_int("id", 182);
    j.write_str("criticality", "ignore");
    j.write_int("Value", serving_cell_mo);
  }
  if (needfor_gap_present) {
    j.write_int("id", 215);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (full_cfg_present) {
    j.write_int("id", 94);
    j.write_str("criticality", "reject");
    j.write_str("Value", "full");
  }
  if (add_rrm_prio_idx_present) {
    j.write_int("id", 248);
    j.write_str("criticality", "ignore");
    j.write_str("Value", add_rrm_prio_idx.to_string());
  }
  if (lower_layer_presence_status_change_present) {
    j.write_int("id", 253);
    j.write_str("criticality", "ignore");
    j.write_str("Value", lower_layer_presence_status_change.to_string());
  }
  if (bh_chs_to_be_setup_mod_list_present) {
    j.write_int("id", 267);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bh_chs_to_be_modified_list_present) {
    j.write_int("id", 263);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bh_chs_to_be_released_list_present) {
    j.write_int("id", 265);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (nr_v2x_services_authorized_present) {
    j.write_int("id", 306);
    j.write_str("criticality", "ignore");
    nr_v2x_services_authorized.to_json(j);
  }
  if (ltev2x_services_authorized_present) {
    j.write_int("id", 307);
    j.write_str("criticality", "ignore");
    ltev2x_services_authorized.to_json(j);
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    j.write_int("id", 308);
    j.write_str("criticality", "ignore");
    nr_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    j.write_int("id", 309);
    j.write_str("criticality", "ignore");
    lte_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (pc5_link_ambr_present) {
    j.write_int("id", 340);
    j.write_str("criticality", "ignore");
    j.write_int("Value", pc5_link_ambr);
  }
  if (sl_drbs_to_be_setup_mod_list_present) {
    j.write_int("id", 332);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_to_be_modified_list_present) {
    j.write_int("id", 326);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_to_be_released_list_present) {
    j.write_int("id", 328);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (conditional_intra_du_mob_info_present) {
    j.write_int("id", 374);
    j.write_str("criticality", "reject");
    conditional_intra_du_mob_info.to_json(j);
  }
  if (f1_c_transfer_path_present) {
    j.write_int("id", 428);
    j.write_str("criticality", "reject");
    f1_c_transfer_path.to_json(j);
  }
  if (scg_ind_present) {
    j.write_int("id", 432);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "released");
  }
  if (ul_tx_direct_current_two_carrier_list_info_present) {
    j.write_int("id", 684);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ul_tx_direct_current_two_carrier_list_info.to_string());
  }
  if (iab_conditional_rrc_msg_delivery_ind_present) {
    j.write_int("id", 510);
    j.write_str("criticality", "reject");
    j.write_str("Value", "true");
  }
  if (f1_c_transfer_path_nr_dc_present) {
    j.write_int("id", 511);
    j.write_str("criticality", "reject");
    f1_c_transfer_path_nr_dc.to_json(j);
  }
  if (mdt_polluted_meas_ind_present) {
    j.write_int("id", 536);
    j.write_str("criticality", "ignore");
    j.write_str("Value", mdt_polluted_meas_ind.to_string());
  }
  if (scg_activation_request_present) {
    j.write_int("id", 547);
    j.write_str("criticality", "ignore");
    j.write_str("Value", scg_activation_request.to_string());
  }
  if (cg_sdt_query_ind_present) {
    j.write_int("id", 586);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (five_g_pro_se_authorized_present) {
    j.write_int("id", 594);
    j.write_str("criticality", "ignore");
    five_g_pro_se_authorized.to_json(j);
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    j.write_int("id", 595);
    j.write_str("criticality", "ignore");
    five_g_pro_se_ue_pc5_aggr_max_bitrate.to_json(j);
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    j.write_int("id", 596);
    j.write_str("criticality", "ignore");
    j.write_int("Value", five_g_pro_se_pc5_link_ambr);
  }
  if (upd_remote_ue_local_id_present) {
    j.write_int("id", 618);
    j.write_str("criticality", "ignore");
    j.write_int("Value", upd_remote_ue_local_id);
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    j.write_int("id", 599);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_to_be_modified_list_present) {
    j.write_int("id", 600);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_to_be_released_list_present) {
    j.write_int("id", 601);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    j.write_int("id", 608);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_to_be_modified_list_present) {
    j.write_int("id", 609);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_to_be_released_list_present) {
    j.write_int("id", 610);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (path_switch_cfg_present) {
    j.write_int("id", 619);
    j.write_str("criticality", "ignore");
    path_switch_cfg.to_json(j);
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    j.write_int("id", 626);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : gnb_du_ue_slice_max_bit_rate_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multicast_mbs_session_setup_list_present) {
    j.write_int("id", 632);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : multicast_mbs_session_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multicast_mbs_session_rem_list_present) {
    j.write_int("id", 633);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : multicast_mbs_session_rem_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ue_multicast_m_rbs_to_be_setup_at_modify_list_present) {
    j.write_int("id", 685);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ue_multicast_m_rbs_to_be_setup_at_modify_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ue_multicast_m_rbs_to_be_released_list_present) {
    j.write_int("id", 628);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ue_multicast_m_rbs_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sldrx_cycle_list_present) {
    j.write_int("id", 643);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sldrx_cycle_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (management_based_mdt_plmn_mod_list_present) {
    j.write_int("id", 647);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : management_based_mdt_plmn_mod_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (sdt_bearer_cfg_query_ind_present) {
    j.write_int("id", 675);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (daps_ho_status_present) {
    j.write_int("id", 683);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "initiation");
  }
  if (serving_cell_mo_list_present) {
    j.write_int("id", 695);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : serving_cell_mo_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ul_tx_direct_current_more_carrier_info_present) {
    j.write_int("id", 690);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ul_tx_direct_current_more_carrier_info.to_string());
  }
  if (cp_acmcg_info_present) {
    j.write_int("id", 691);
    j.write_str("criticality", "ignore");
    cp_acmcg_info.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_mod_required_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (du_to_cu_rrc_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)39, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(du_to_cu_rrc_info.pack(bref));
  }
  if (drbs_required_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)23, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_required_to_be_modified_list, 1, 64, true));
  }
  if (srbs_required_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)70, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_required_to_be_released_list, 1, 8, true));
  }
  if (drbs_required_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)25, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_required_to_be_released_list, 1, 64, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (bh_chs_required_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)277, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_required_to_be_released_list, 1, 65536, true));
  }
  if (sl_drbs_required_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)320, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_required_to_be_modified_list, 1, 512, true));
  }
  if (sl_drbs_required_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)322, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_required_to_be_released_list, 1, 512, true));
  }
  if (target_cells_to_cancel_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)375, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, target_cells_to_cancel, 1, 8, true));
  }
  if (uu_rlc_ch_required_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)606, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_required_to_be_modified_list, 1, 32, true));
  }
  if (uu_rlc_ch_required_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)607, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_required_to_be_released_list, 1, 32, true));
  }
  if (pc5_rlc_ch_required_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)614, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_required_to_be_modified_list, 1, 512, true));
  }
  if (pc5_rlc_ch_required_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)615, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_required_to_be_released_list, 1, 512, true));
  }
  if (ue_multicast_m_rbs_required_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)655, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_multicast_m_rbs_required_to_be_modified_list, 1, 64, true));
  }
  if (ue_multicast_m_rbs_required_to_be_released_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)657, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_multicast_m_rbs_required_to_be_released_list, 1, 64, true));
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.unpack(bref));
        break;
      }
      case 39: {
        du_to_cu_rrc_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_info.unpack(bref));
        break;
      }
      case 23: {
        drbs_required_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_required_to_be_modified_list, bref, 1, 64, true));
        break;
      }
      case 70: {
        srbs_required_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_required_to_be_released_list, bref, 1, 8, true));
        break;
      }
      case 25: {
        drbs_required_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_required_to_be_released_list, bref, 1, 64, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 277: {
        bh_chs_required_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_required_to_be_released_list, bref, 1, 65536, true));
        break;
      }
      case 320: {
        sl_drbs_required_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_required_to_be_modified_list, bref, 1, 512, true));
        break;
      }
      case 322: {
        sl_drbs_required_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_required_to_be_released_list, bref, 1, 512, true));
        break;
      }
      case 375: {
        target_cells_to_cancel_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(target_cells_to_cancel, bref, 1, 8, true));
        break;
      }
      case 606: {
        uu_rlc_ch_required_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_required_to_be_modified_list, bref, 1, 32, true));
        break;
      }
      case 607: {
        uu_rlc_ch_required_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_required_to_be_released_list, bref, 1, 32, true));
        break;
      }
      case 614: {
        pc5_rlc_ch_required_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_required_to_be_modified_list, bref, 1, 512, true));
        break;
      }
      case 615: {
        pc5_rlc_ch_required_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_required_to_be_released_list, bref, 1, 512, true));
        break;
      }
      case 655: {
        ue_multicast_m_rbs_required_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_multicast_m_rbs_required_to_be_modified_list, bref, 1, 64, true));
        break;
      }
      case 657: {
        ue_multicast_m_rbs_required_to_be_released_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_multicast_m_rbs_required_to_be_released_list, bref, 1, 64, true));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  if (res_coordination_transfer_container_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "ignore");
    j.write_str("Value", res_coordination_transfer_container.to_string());
  }
  if (du_to_cu_rrc_info_present) {
    j.write_int("id", 39);
    j.write_str("criticality", "reject");
    du_to_cu_rrc_info.to_json(j);
  }
  if (drbs_required_to_be_modified_list_present) {
    j.write_int("id", 23);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drbs_required_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_required_to_be_released_list_present) {
    j.write_int("id", 70);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : srbs_required_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_required_to_be_released_list_present) {
    j.write_int("id", 25);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drbs_required_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (bh_chs_required_to_be_released_list_present) {
    j.write_int("id", 277);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_required_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_required_to_be_modified_list_present) {
    j.write_int("id", 320);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_required_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_required_to_be_released_list_present) {
    j.write_int("id", 322);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_required_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (target_cells_to_cancel_present) {
    j.write_int("id", 375);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : target_cells_to_cancel) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_required_to_be_modified_list_present) {
    j.write_int("id", 606);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_required_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_required_to_be_released_list_present) {
    j.write_int("id", 607);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_required_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_required_to_be_modified_list_present) {
    j.write_int("id", 614);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_required_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_required_to_be_released_list_present) {
    j.write_int("id", 615);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_required_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ue_multicast_m_rbs_required_to_be_modified_list_present) {
    j.write_int("id", 655);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ue_multicast_m_rbs_required_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ue_multicast_m_rbs_required_to_be_released_list_present) {
    j.write_int("id", 657);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ue_multicast_m_rbs_required_to_be_released_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_mod_resp_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (du_to_cu_rrc_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)39, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(du_to_cu_rrc_info.pack(bref));
  }
  if (drbs_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)29, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_setup_mod_list, 1, 64, true));
  }
  if (drbs_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)21, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_modified_list, 1, 64, true));
  }
  if (srbs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)68, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_failed_to_be_setup_mod_list, 1, 8, true));
  }
  if (drbs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)17, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_failed_to_be_setup_mod_list, 1, 64, true));
  }
  if (scell_failedto_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)85, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_failedto_setup_mod_list, 1, 32, true));
  }
  if (drbs_failed_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)13, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_failed_to_be_modified_list, 1, 64, true));
  }
  if (inactivity_monitoring_resp_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)98, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(inactivity_monitoring_resp.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (c_rnti_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)95, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, c_rnti, (uint32_t)0u, (uint32_t)65535u, true, true));
  }
  if (associated_scell_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)198, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, associated_scell_list, 1, 32, true));
  }
  if (srbs_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)204, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_setup_mod_list, 1, 8, true));
  }
  if (srbs_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)206, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_modified_list, 1, 8, true));
  }
  if (full_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)94, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(full_cfg.pack(bref));
  }
  if (bh_chs_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)275, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_setup_mod_list, 1, 65536, true));
  }
  if (bh_chs_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)273, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_modified_list, 1, 65536, true));
  }
  if (bh_chs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)271, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_failed_to_be_setup_mod_list, 1, 65536, true));
  }
  if (bh_chs_failed_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)269, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_failed_to_be_modified_list, 1, 65536, true));
  }
  if (sl_drbs_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)333, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_setup_mod_list, 1, 512, true));
  }
  if (sl_drbs_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)318, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_modified_list, 1, 512, true));
  }
  if (sl_drbs_failed_to_be_setup_mod_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)334, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_failed_to_be_setup_mod_list, 1, 512, true));
  }
  if (sl_drbs_failed_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)314, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_failed_to_be_modified_list, 1, 512, true));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)376, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(requested_target_cell_global_id.pack(bref));
  }
  if (scg_activation_status_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)548, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(scg_activation_status.pack(bref));
  }
  if (uu_rlc_ch_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)602, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_setup_list, 1, 32, true));
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)603, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_failed_to_be_setup_list, 1, 32, true));
  }
  if (uu_rlc_ch_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)604, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_modified_list, 1, 32, true));
  }
  if (uu_rlc_ch_failed_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)605, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_failed_to_be_modified_list, 1, 32, true));
  }
  if (pc5_rlc_ch_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)611, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_setup_list, 1, 512, true));
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)612, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_failed_to_be_setup_list, 1, 512, true));
  }
  if (pc5_rlc_ch_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)616, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_modified_list, 1, 512, true));
  }
  if (pc5_rlc_ch_failed_to_be_modified_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)613, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_failed_to_be_modified_list, 1, 512, true));
  }
  if (sdt_bearer_cfg_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)676, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sdt_bearer_cfg_info.pack(bref));
  }
  if (ue_multicast_m_rbs_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)679, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_multicast_m_rbs_setup_list, 1, 64, true));
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)697, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, serving_cell_mo_encoded_in_cgc_list, 1, 8, true));
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.unpack(bref));
        break;
      }
      case 39: {
        du_to_cu_rrc_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_info.unpack(bref));
        break;
      }
      case 29: {
        drbs_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_setup_mod_list, bref, 1, 64, true));
        break;
      }
      case 21: {
        drbs_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_modified_list, bref, 1, 64, true));
        break;
      }
      case 68: {
        srbs_failed_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_failed_to_be_setup_mod_list, bref, 1, 8, true));
        break;
      }
      case 17: {
        drbs_failed_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_failed_to_be_setup_mod_list, bref, 1, 64, true));
        break;
      }
      case 85: {
        scell_failedto_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(scell_failedto_setup_mod_list, bref, 1, 32, true));
        break;
      }
      case 13: {
        drbs_failed_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_failed_to_be_modified_list, bref, 1, 64, true));
        break;
      }
      case 98: {
        inactivity_monitoring_resp_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_resp.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 95: {
        c_rnti_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(c_rnti, bref, (uint32_t)0u, (uint32_t)65535u, true, true));
        break;
      }
      case 198: {
        associated_scell_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(associated_scell_list, bref, 1, 32, true));
        break;
      }
      case 204: {
        srbs_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_setup_mod_list, bref, 1, 8, true));
        break;
      }
      case 206: {
        srbs_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_modified_list, bref, 1, 8, true));
        break;
      }
      case 94: {
        full_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(full_cfg.unpack(bref));
        break;
      }
      case 275: {
        bh_chs_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_setup_mod_list, bref, 1, 65536, true));
        break;
      }
      case 273: {
        bh_chs_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_modified_list, bref, 1, 65536, true));
        break;
      }
      case 271: {
        bh_chs_failed_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_failed_to_be_setup_mod_list, bref, 1, 65536, true));
        break;
      }
      case 269: {
        bh_chs_failed_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_failed_to_be_modified_list, bref, 1, 65536, true));
        break;
      }
      case 333: {
        sl_drbs_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_setup_mod_list, bref, 1, 512, true));
        break;
      }
      case 318: {
        sl_drbs_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_modified_list, bref, 1, 512, true));
        break;
      }
      case 334: {
        sl_drbs_failed_to_be_setup_mod_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_failed_to_be_setup_mod_list, bref, 1, 512, true));
        break;
      }
      case 314: {
        sl_drbs_failed_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_failed_to_be_modified_list, bref, 1, 512, true));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.unpack(bref));
        break;
      }
      case 548: {
        scg_activation_status_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_status.unpack(bref));
        break;
      }
      case 602: {
        uu_rlc_ch_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_setup_list, bref, 1, 32, true));
        break;
      }
      case 603: {
        uu_rlc_ch_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_failed_to_be_setup_list, bref, 1, 32, true));
        break;
      }
      case 604: {
        uu_rlc_ch_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_modified_list, bref, 1, 32, true));
        break;
      }
      case 605: {
        uu_rlc_ch_failed_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_failed_to_be_modified_list, bref, 1, 32, true));
        break;
      }
      case 611: {
        pc5_rlc_ch_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_setup_list, bref, 1, 512, true));
        break;
      }
      case 612: {
        pc5_rlc_ch_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_failed_to_be_setup_list, bref, 1, 512, true));
        break;
      }
      case 616: {
        pc5_rlc_ch_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_modified_list, bref, 1, 512, true));
        break;
      }
      case 613: {
        pc5_rlc_ch_failed_to_be_modified_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_failed_to_be_modified_list, bref, 1, 512, true));
        break;
      }
      case 676: {
        sdt_bearer_cfg_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_bearer_cfg_info.unpack(bref));
        break;
      }
      case 679: {
        ue_multicast_m_rbs_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_multicast_m_rbs_setup_list, bref, 1, 64, true));
        break;
      }
      case 697: {
        serving_cell_mo_encoded_in_cgc_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(serving_cell_mo_encoded_in_cgc_list, bref, 1, 8, true));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  if (res_coordination_transfer_container_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "ignore");
    j.write_str("Value", res_coordination_transfer_container.to_string());
  }
  if (du_to_cu_rrc_info_present) {
    j.write_int("id", 39);
    j.write_str("criticality", "reject");
    du_to_cu_rrc_info.to_json(j);
  }
  if (drbs_setup_mod_list_present) {
    j.write_int("id", 29);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drbs_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_modified_list_present) {
    j.write_int("id", 21);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drbs_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_failed_to_be_setup_mod_list_present) {
    j.write_int("id", 68);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : srbs_failed_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_failed_to_be_setup_mod_list_present) {
    j.write_int("id", 17);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drbs_failed_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_failedto_setup_mod_list_present) {
    j.write_int("id", 85);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : scell_failedto_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_failed_to_be_modified_list_present) {
    j.write_int("id", 13);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drbs_failed_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inactivity_monitoring_resp_present) {
    j.write_int("id", 98);
    j.write_str("criticality", "reject");
    j.write_str("Value", "not-supported");
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 7);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (c_rnti_present) {
    j.write_int("id", 95);
    j.write_str("criticality", "ignore");
    j.write_int("Value", c_rnti);
  }
  if (associated_scell_list_present) {
    j.write_int("id", 198);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : associated_scell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_setup_mod_list_present) {
    j.write_int("id", 204);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : srbs_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_modified_list_present) {
    j.write_int("id", 206);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : srbs_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (full_cfg_present) {
    j.write_int("id", 94);
    j.write_str("criticality", "reject");
    j.write_str("Value", "full");
  }
  if (bh_chs_setup_mod_list_present) {
    j.write_int("id", 275);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bh_chs_modified_list_present) {
    j.write_int("id", 273);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bh_chs_failed_to_be_setup_mod_list_present) {
    j.write_int("id", 271);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_failed_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bh_chs_failed_to_be_modified_list_present) {
    j.write_int("id", 269);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_failed_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_setup_mod_list_present) {
    j.write_int("id", 333);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_modified_list_present) {
    j.write_int("id", 318);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_failed_to_be_setup_mod_list_present) {
    j.write_int("id", 334);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_failed_to_be_setup_mod_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_failed_to_be_modified_list_present) {
    j.write_int("id", 314);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_failed_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (requested_target_cell_global_id_present) {
    j.write_int("id", 376);
    j.write_str("criticality", "reject");
    requested_target_cell_global_id.to_json(j);
  }
  if (scg_activation_status_present) {
    j.write_int("id", 548);
    j.write_str("criticality", "ignore");
    j.write_str("Value", scg_activation_status.to_string());
  }
  if (uu_rlc_ch_setup_list_present) {
    j.write_int("id", 602);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    j.write_int("id", 603);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_modified_list_present) {
    j.write_int("id", 604);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_failed_to_be_modified_list_present) {
    j.write_int("id", 605);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_failed_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_setup_list_present) {
    j.write_int("id", 611);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    j.write_int("id", 612);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_modified_list_present) {
    j.write_int("id", 616);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_failed_to_be_modified_list_present) {
    j.write_int("id", 613);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_failed_to_be_modified_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sdt_bearer_cfg_info_present) {
    j.write_int("id", 676);
    j.write_str("criticality", "ignore");
    sdt_bearer_cfg_info.to_json(j);
  }
  if (ue_multicast_m_rbs_setup_list_present) {
    j.write_int("id", 679);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ue_multicast_m_rbs_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    j.write_int("id", 697);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : serving_cell_mo_encoded_in_cgc_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_release_cmd_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (rrc_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (srb_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)64, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, srb_id, (uint8_t)0u, (uint8_t)3u, true, true));
  }
  if (old_gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)47, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, old_gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (execute_dupl_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)109, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(execute_dupl.pack(bref));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)184, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (target_cells_to_cancel_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)375, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, target_cells_to_cancel, 1, 8, true));
  }
  if (pos_conext_rev_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)576, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pos_conext_rev_ind.pack(bref));
  }
  if (cg_sdt_kept_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)588, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 50: {
        rrc_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.unpack(bref));
        break;
      }
      case 64: {
        srb_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(srb_id, bref, (uint8_t)0u, (uint8_t)3u, true, true));
        break;
      }
      case 47: {
        old_gnb_du_ue_f1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(old_gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 109: {
        execute_dupl_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(execute_dupl.unpack(bref));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.unpack(bref));
        break;
      }
      case 375: {
        target_cells_to_cancel_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(target_cells_to_cancel, bref, 1, 8, true));
        break;
      }
      case 576: {
        pos_conext_rev_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(pos_conext_rev_ind.unpack(bref));
        break;
      }
      case 588: {
        cg_sdt_kept_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cg_sdt_kept_ind.unpack(bref));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (rrc_container_present) {
    j.write_int("id", 50);
    j.write_str("criticality", "ignore");
    j.write_str("Value", rrc_container.to_string());
  }
  if (srb_id_present) {
    j.write_int("id", 64);
    j.write_str("criticality", "ignore");
    j.write_int("Value", srb_id);
  }
  if (old_gnb_du_ue_f1ap_id_present) {
    j.write_int("id", 47);
    j.write_str("criticality", "ignore");
    j.write_int("Value", old_gnb_du_ue_f1ap_id);
  }
  if (execute_dupl_present) {
    j.write_int("id", 109);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (rrc_delivery_status_request_present) {
    j.write_int("id", 184);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (target_cells_to_cancel_present) {
    j.write_int("id", 375);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : target_cells_to_cancel) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pos_conext_rev_ind_present) {
    j.write_int("id", 576);
    j.write_str("criticality", "reject");
    j.write_str("Value", "true");
  }
  if (cg_sdt_kept_ind_present) {
    j.write_int("id", 588);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_release_complete_ies_o>;

SRSASN_CODE ue_context_release_complete_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 7: {
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
void ue_context_release_complete_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  if (crit_diagnostics_present) {
    j.write_int("id", 7);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_release_request_ies_o>;

SRSASN_CODE ue_context_release_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += target_cells_to_cancel_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (target_cells_to_cancel_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)375, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, target_cells_to_cancel, 1, 8, true));
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 375: {
        target_cells_to_cancel_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(target_cells_to_cancel, bref, 1, 8, true));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (target_cells_to_cancel_present) {
    j.write_int("id", 375);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : target_cells_to_cancel) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_setup_fail_ies_o>;

SRSASN_CODE ue_context_setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += gnb_du_ue_f1ap_id_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += potential_sp_cell_list_present ? 1 : 0;
  nof_ies += requested_target_cell_global_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)0, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (potential_sp_cell_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)92, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, potential_sp_cell_list, 0, 64, true));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)376, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        gnb_du_ue_f1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 0: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 92: {
        potential_sp_cell_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(potential_sp_cell_list, bref, 0, 64, true));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.unpack(bref));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  if (gnb_du_ue_f1ap_id_present) {
    j.write_int("id", 41);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_du_ue_f1ap_id);
  }
  j.write_int("id", 0);
  j.write_str("criticality", "ignore");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_int("id", 7);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (potential_sp_cell_list_present) {
    j.write_int("id", 92);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : potential_sp_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (requested_target_cell_global_id_present) {
    j.write_int("id", 376);
    j.write_str("criticality", "reject");
    requested_target_cell_global_id.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_setup_request_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)63, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sp_cell_id.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)107, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, serv_cell_idx, (uint8_t)0u, (uint8_t)31u, true, true));
  }
  if (sp_cell_ul_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)96, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sp_cell_ul_cfg.pack(bref));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)9, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cu_to_du_rrc_info.pack(bref));
  }
  if (candidate_sp_cell_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)90, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, candidate_sp_cell_list, 1, 64, true));
  }
  if (drx_cycle_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)38, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(drx_cycle.pack(bref));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (scell_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)54, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_be_setup_list, 1, 32, true));
  }
  if (srbs_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)74, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_to_be_setup_list, 1, 8, true));
  }
  if (drbs_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)35, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_to_be_setup_list, 1, 64, true));
  }
  if (inactivity_monitoring_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)97, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(inactivity_monitoring_request.pack(bref));
  }
  if (rat_freq_prio_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)108, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rat_freq_prio_info.pack(bref));
  }
  if (rrc_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (masked_imeisv_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)126, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(masked_imeisv.pack(bref));
  }
  if (serving_plmn_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)165, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(serving_plmn.pack(bref));
  }
  if (gnb_du_ue_ambr_ul_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)158, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_ambr_ul, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (rrc_delivery_status_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)184, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_delivery_status_request.pack(bref));
  }
  if (res_coordination_transfer_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)195, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_info.pack(bref));
  }
  if (serving_cell_mo_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)182, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, serving_cell_mo, (uint8_t)1u, (uint8_t)64u, true, true));
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)217, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, new_gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (ran_ue_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)226, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ran_ue_id.pack(bref));
  }
  if (trace_activation_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)242, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(trace_activation.pack(bref));
  }
  if (add_rrm_prio_idx_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)248, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(add_rrm_prio_idx.pack(bref));
  }
  if (bh_chs_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)258, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_to_be_setup_list, 1, 65536, true));
  }
  if (cfg_bap_address_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)282, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cfg_bap_address.pack(bref));
  }
  if (nr_v2x_services_authorized_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)306, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(nr_v2x_services_authorized.pack(bref));
  }
  if (ltev2x_services_authorized_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)307, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(ltev2x_services_authorized.pack(bref));
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)308, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)309, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.pack(bref));
  }
  if (pc5_link_ambr_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)340, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, pc5_link_ambr, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (sl_drbs_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)330, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_to_be_setup_list, 1, 512, true));
  }
  if (conditional_inter_du_mob_info_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)373, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(conditional_inter_du_mob_info.pack(bref));
  }
  if (management_based_mdt_plmn_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)377, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, management_based_mdt_plmn_list, 1, 16, true));
  }
  if (serving_n_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)382, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(serving_n_id.pack(bref));
  }
  if (f1_c_transfer_path_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)428, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(f1_c_transfer_path.pack(bref));
  }
  if (f1_c_transfer_path_nr_dc_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)511, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(f1_c_transfer_path_nr_dc.pack(bref));
  }
  if (mdt_polluted_meas_ind_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)536, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(mdt_polluted_meas_ind.pack(bref));
  }
  if (scg_activation_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)547, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(scg_activation_request.pack(bref));
  }
  if (cg_sdt_session_info_old_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)591, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(cg_sdt_session_info_old.pack(bref));
  }
  if (five_g_pro_se_authorized_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)594, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(five_g_pro_se_authorized.pack(bref));
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)595, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.pack(bref));
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)596, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, five_g_pro_se_pc5_link_ambr, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)599, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_to_be_setup_list, 1, 32, true));
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)608, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_to_be_setup_list, 1, 512, true));
  }
  if (path_switch_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)619, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(path_switch_cfg.pack(bref));
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)626, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_du_ue_slice_max_bit_rate_list, 1, 8, true));
  }
  if (multicast_mbs_session_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)632, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, multicast_mbs_session_setup_list, 1, 256, true));
  }
  if (ue_multicast_m_rbs_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)630, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, ue_multicast_m_rbs_to_be_setup_list, 1, 64, true));
  }
  if (serving_cell_mo_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)695, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, serving_cell_mo_list, 1, 16, true));
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        gnb_du_ue_f1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 63: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_id.unpack(bref));
        break;
      }
      case 107: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(serv_cell_idx, bref, (uint8_t)0u, (uint8_t)31u, true, true));
        break;
      }
      case 96: {
        sp_cell_ul_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sp_cell_ul_cfg.unpack(bref));
        break;
      }
      case 9: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cu_to_du_rrc_info.unpack(bref));
        break;
      }
      case 90: {
        candidate_sp_cell_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(candidate_sp_cell_list, bref, 1, 64, true));
        break;
      }
      case 38: {
        drx_cycle_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(drx_cycle.unpack(bref));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.unpack(bref));
        break;
      }
      case 54: {
        scell_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(scell_to_be_setup_list, bref, 1, 32, true));
        break;
      }
      case 74: {
        srbs_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_to_be_setup_list, bref, 1, 8, true));
        break;
      }
      case 35: {
        drbs_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_to_be_setup_list, bref, 1, 64, true));
        break;
      }
      case 97: {
        inactivity_monitoring_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_request.unpack(bref));
        break;
      }
      case 108: {
        rat_freq_prio_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rat_freq_prio_info.unpack(bref));
        break;
      }
      case 50: {
        rrc_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.unpack(bref));
        break;
      }
      case 126: {
        masked_imeisv_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(masked_imeisv.unpack(bref));
        break;
      }
      case 165: {
        serving_plmn_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_plmn.unpack(bref));
        break;
      }
      case 158: {
        gnb_du_ue_ambr_ul_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_ambr_ul, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 184: {
        rrc_delivery_status_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_delivery_status_request.unpack(bref));
        break;
      }
      case 195: {
        res_coordination_transfer_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_info.unpack(bref));
        break;
      }
      case 182: {
        serving_cell_mo_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(serving_cell_mo, bref, (uint8_t)1u, (uint8_t)64u, true, true));
        break;
      }
      case 217: {
        new_gnb_cu_ue_f1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(new_gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 226: {
        ran_ue_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ran_ue_id.unpack(bref));
        break;
      }
      case 242: {
        trace_activation_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(trace_activation.unpack(bref));
        break;
      }
      case 248: {
        add_rrm_prio_idx_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(add_rrm_prio_idx.unpack(bref));
        break;
      }
      case 258: {
        bh_chs_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_to_be_setup_list, bref, 1, 65536, true));
        break;
      }
      case 282: {
        cfg_bap_address_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cfg_bap_address.unpack(bref));
        break;
      }
      case 306: {
        nr_v2x_services_authorized_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_v2x_services_authorized.unpack(bref));
        break;
      }
      case 307: {
        ltev2x_services_authorized_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ltev2x_services_authorized.unpack(bref));
        break;
      }
      case 308: {
        nr_ue_sidelink_aggr_max_bitrate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(nr_ue_sidelink_aggr_max_bitrate.unpack(bref));
        break;
      }
      case 309: {
        lte_ue_sidelink_aggr_max_bitrate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(lte_ue_sidelink_aggr_max_bitrate.unpack(bref));
        break;
      }
      case 340: {
        pc5_link_ambr_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(pc5_link_ambr, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 330: {
        sl_drbs_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_to_be_setup_list, bref, 1, 512, true));
        break;
      }
      case 373: {
        conditional_inter_du_mob_info_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(conditional_inter_du_mob_info.unpack(bref));
        break;
      }
      case 377: {
        management_based_mdt_plmn_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(management_based_mdt_plmn_list, bref, 1, 16, true));
        break;
      }
      case 382: {
        serving_n_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(serving_n_id.unpack(bref));
        break;
      }
      case 428: {
        f1_c_transfer_path_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path.unpack(bref));
        break;
      }
      case 511: {
        f1_c_transfer_path_nr_dc_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(f1_c_transfer_path_nr_dc.unpack(bref));
        break;
      }
      case 536: {
        mdt_polluted_meas_ind_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(mdt_polluted_meas_ind.unpack(bref));
        break;
      }
      case 547: {
        scg_activation_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_request.unpack(bref));
        break;
      }
      case 591: {
        cg_sdt_session_info_old_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cg_sdt_session_info_old.unpack(bref));
        break;
      }
      case 594: {
        five_g_pro_se_authorized_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_authorized.unpack(bref));
        break;
      }
      case 595: {
        five_g_pro_se_ue_pc5_aggr_max_bitrate_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(five_g_pro_se_ue_pc5_aggr_max_bitrate.unpack(bref));
        break;
      }
      case 596: {
        five_g_pro_se_pc5_link_ambr_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(
            unpack_integer(five_g_pro_se_pc5_link_ambr, bref, (uint64_t)0u, (uint64_t)4000000000000u, true, true));
        break;
      }
      case 599: {
        uu_rlc_ch_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_to_be_setup_list, bref, 1, 32, true));
        break;
      }
      case 608: {
        pc5_rlc_ch_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_to_be_setup_list, bref, 1, 512, true));
        break;
      }
      case 619: {
        path_switch_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(path_switch_cfg.unpack(bref));
        break;
      }
      case 626: {
        gnb_du_ue_slice_max_bit_rate_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(gnb_du_ue_slice_max_bit_rate_list, bref, 1, 8, true));
        break;
      }
      case 632: {
        multicast_mbs_session_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(multicast_mbs_session_setup_list, bref, 1, 256, true));
        break;
      }
      case 630: {
        ue_multicast_m_rbs_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(ue_multicast_m_rbs_to_be_setup_list, bref, 1, 64, true));
        break;
      }
      case 695: {
        serving_cell_mo_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(serving_cell_mo_list, bref, 1, 16, true));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  if (gnb_du_ue_f1ap_id_present) {
    j.write_int("id", 41);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_du_ue_f1ap_id);
  }
  j.write_int("id", 63);
  j.write_str("criticality", "reject");
  sp_cell_id.to_json(j);
  j.write_int("id", 107);
  j.write_str("criticality", "reject");
  j.write_int("Value", serv_cell_idx);
  if (sp_cell_ul_cfg_present) {
    j.write_int("id", 96);
    j.write_str("criticality", "ignore");
    j.write_str("Value", sp_cell_ul_cfg.to_string());
  }
  j.write_int("id", 9);
  j.write_str("criticality", "reject");
  cu_to_du_rrc_info.to_json(j);
  if (candidate_sp_cell_list_present) {
    j.write_int("id", 90);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : candidate_sp_cell_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drx_cycle_present) {
    j.write_int("id", 38);
    j.write_str("criticality", "ignore");
    drx_cycle.to_json(j);
  }
  if (res_coordination_transfer_container_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "ignore");
    j.write_str("Value", res_coordination_transfer_container.to_string());
  }
  if (scell_to_be_setup_list_present) {
    j.write_int("id", 54);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : scell_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_to_be_setup_list_present) {
    j.write_int("id", 74);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : srbs_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_to_be_setup_list_present) {
    j.write_int("id", 35);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : drbs_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inactivity_monitoring_request_present) {
    j.write_int("id", 97);
    j.write_str("criticality", "reject");
    j.write_str("Value", "true");
  }
  if (rat_freq_prio_info_present) {
    j.write_int("id", 108);
    j.write_str("criticality", "reject");
    rat_freq_prio_info.to_json(j);
  }
  if (rrc_container_present) {
    j.write_int("id", 50);
    j.write_str("criticality", "ignore");
    j.write_str("Value", rrc_container.to_string());
  }
  if (masked_imeisv_present) {
    j.write_int("id", 126);
    j.write_str("criticality", "ignore");
    j.write_str("Value", masked_imeisv.to_string());
  }
  if (serving_plmn_present) {
    j.write_int("id", 165);
    j.write_str("criticality", "ignore");
    j.write_str("Value", serving_plmn.to_string());
  }
  if (gnb_du_ue_ambr_ul_present) {
    j.write_int("id", 158);
    j.write_str("criticality", "ignore");
    j.write_int("Value", gnb_du_ue_ambr_ul);
  }
  if (rrc_delivery_status_request_present) {
    j.write_int("id", 184);
    j.write_str("criticality", "ignore");
    j.write_str("Value", "true");
  }
  if (res_coordination_transfer_info_present) {
    j.write_int("id", 195);
    j.write_str("criticality", "ignore");
    res_coordination_transfer_info.to_json(j);
  }
  if (serving_cell_mo_present) {
    j.write_int("id", 182);
    j.write_str("criticality", "ignore");
    j.write_int("Value", serving_cell_mo);
  }
  if (new_gnb_cu_ue_f1ap_id_present) {
    j.write_int("id", 217);
    j.write_str("criticality", "reject");
    j.write_int("Value", new_gnb_cu_ue_f1ap_id);
  }
  if (ran_ue_id_present) {
    j.write_int("id", 226);
    j.write_str("criticality", "ignore");
    j.write_str("Value", ran_ue_id.to_string());
  }
  if (trace_activation_present) {
    j.write_int("id", 242);
    j.write_str("criticality", "ignore");
    trace_activation.to_json(j);
  }
  if (add_rrm_prio_idx_present) {
    j.write_int("id", 248);
    j.write_str("criticality", "ignore");
    j.write_str("Value", add_rrm_prio_idx.to_string());
  }
  if (bh_chs_to_be_setup_list_present) {
    j.write_int("id", 258);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (cfg_bap_address_present) {
    j.write_int("id", 282);
    j.write_str("criticality", "reject");
    j.write_str("Value", cfg_bap_address.to_string());
  }
  if (nr_v2x_services_authorized_present) {
    j.write_int("id", 306);
    j.write_str("criticality", "ignore");
    nr_v2x_services_authorized.to_json(j);
  }
  if (ltev2x_services_authorized_present) {
    j.write_int("id", 307);
    j.write_str("criticality", "ignore");
    ltev2x_services_authorized.to_json(j);
  }
  if (nr_ue_sidelink_aggr_max_bitrate_present) {
    j.write_int("id", 308);
    j.write_str("criticality", "ignore");
    nr_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (lte_ue_sidelink_aggr_max_bitrate_present) {
    j.write_int("id", 309);
    j.write_str("criticality", "ignore");
    lte_ue_sidelink_aggr_max_bitrate.to_json(j);
  }
  if (pc5_link_ambr_present) {
    j.write_int("id", 340);
    j.write_str("criticality", "ignore");
    j.write_int("Value", pc5_link_ambr);
  }
  if (sl_drbs_to_be_setup_list_present) {
    j.write_int("id", 330);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (conditional_inter_du_mob_info_present) {
    j.write_int("id", 373);
    j.write_str("criticality", "reject");
    conditional_inter_du_mob_info.to_json(j);
  }
  if (management_based_mdt_plmn_list_present) {
    j.write_int("id", 377);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : management_based_mdt_plmn_list) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (serving_n_id_present) {
    j.write_int("id", 382);
    j.write_str("criticality", "reject");
    j.write_str("Value", serving_n_id.to_string());
  }
  if (f1_c_transfer_path_present) {
    j.write_int("id", 428);
    j.write_str("criticality", "reject");
    f1_c_transfer_path.to_json(j);
  }
  if (f1_c_transfer_path_nr_dc_present) {
    j.write_int("id", 511);
    j.write_str("criticality", "reject");
    f1_c_transfer_path_nr_dc.to_json(j);
  }
  if (mdt_polluted_meas_ind_present) {
    j.write_int("id", 536);
    j.write_str("criticality", "ignore");
    j.write_str("Value", mdt_polluted_meas_ind.to_string());
  }
  if (scg_activation_request_present) {
    j.write_int("id", 547);
    j.write_str("criticality", "ignore");
    j.write_str("Value", scg_activation_request.to_string());
  }
  if (cg_sdt_session_info_old_present) {
    j.write_int("id", 591);
    j.write_str("criticality", "ignore");
    cg_sdt_session_info_old.to_json(j);
  }
  if (five_g_pro_se_authorized_present) {
    j.write_int("id", 594);
    j.write_str("criticality", "ignore");
    five_g_pro_se_authorized.to_json(j);
  }
  if (five_g_pro_se_ue_pc5_aggr_max_bitrate_present) {
    j.write_int("id", 595);
    j.write_str("criticality", "ignore");
    five_g_pro_se_ue_pc5_aggr_max_bitrate.to_json(j);
  }
  if (five_g_pro_se_pc5_link_ambr_present) {
    j.write_int("id", 596);
    j.write_str("criticality", "ignore");
    j.write_int("Value", five_g_pro_se_pc5_link_ambr);
  }
  if (uu_rlc_ch_to_be_setup_list_present) {
    j.write_int("id", 599);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_to_be_setup_list_present) {
    j.write_int("id", 608);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (path_switch_cfg_present) {
    j.write_int("id", 619);
    j.write_str("criticality", "ignore");
    path_switch_cfg.to_json(j);
  }
  if (gnb_du_ue_slice_max_bit_rate_list_present) {
    j.write_int("id", 626);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : gnb_du_ue_slice_max_bit_rate_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (multicast_mbs_session_setup_list_present) {
    j.write_int("id", 632);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : multicast_mbs_session_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ue_multicast_m_rbs_to_be_setup_list_present) {
    j.write_int("id", 630);
    j.write_str("criticality", "reject");
    j.start_array("Value");
    for (const auto& e1 : ue_multicast_m_rbs_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (serving_cell_mo_list_present) {
    j.write_int("id", 695);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : serving_cell_mo_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_context_setup_resp_ies_o>;

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

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)39, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(du_to_cu_rrc_info.pack(bref));
  }
  if (c_rnti_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)95, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, c_rnti, (uint32_t)0u, (uint32_t)65535u, true, true));
  }
  if (res_coordination_transfer_container_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)49, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(res_coordination_transfer_container.pack(bref));
  }
  if (full_cfg_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)94, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(full_cfg.pack(bref));
  }
  if (drbs_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)27, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_setup_list, 1, 64, true));
  }
  if (srbs_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)66, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_failed_to_be_setup_list, 1, 8, true));
  }
  if (drbs_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)15, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drbs_failed_to_be_setup_list, 1, 64, true));
  }
  if (scell_failedto_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)83, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_failedto_setup_list, 1, 32, true));
  }
  if (inactivity_monitoring_resp_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)98, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(inactivity_monitoring_resp.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)7, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (srbs_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)202, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, srbs_setup_list, 1, 8, true));
  }
  if (bh_chs_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)260, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_setup_list, 1, 65536, true));
  }
  if (bh_chs_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)279, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, bh_chs_failed_to_be_setup_list, 1, 65536, true));
  }
  if (sl_drbs_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)324, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_setup_list, 1, 512, true));
  }
  if (sl_drbs_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)316, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, sl_drbs_failed_to_be_setup_list, 1, 512, true));
  }
  if (requested_target_cell_global_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)376, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(requested_target_cell_global_id.pack(bref));
  }
  if (scg_activation_status_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)548, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(scg_activation_status.pack(bref));
  }
  if (uu_rlc_ch_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)602, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_setup_list, 1, 32, true));
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)603, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, uu_rlc_ch_failed_to_be_setup_list, 1, 32, true));
  }
  if (pc5_rlc_ch_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)611, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_setup_list, 1, 512, true));
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)612, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, pc5_rlc_ch_failed_to_be_setup_list, 1, 512, true));
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)697, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, serving_cell_mo_encoded_in_cgc_list, 1, 8, true));
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 39: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(du_to_cu_rrc_info.unpack(bref));
        break;
      }
      case 95: {
        c_rnti_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(c_rnti, bref, (uint32_t)0u, (uint32_t)65535u, true, true));
        break;
      }
      case 49: {
        res_coordination_transfer_container_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(res_coordination_transfer_container.unpack(bref));
        break;
      }
      case 94: {
        full_cfg_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(full_cfg.unpack(bref));
        break;
      }
      case 27: {
        drbs_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_setup_list, bref, 1, 64, true));
        break;
      }
      case 66: {
        srbs_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_failed_to_be_setup_list, bref, 1, 8, true));
        break;
      }
      case 15: {
        drbs_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drbs_failed_to_be_setup_list, bref, 1, 64, true));
        break;
      }
      case 83: {
        scell_failedto_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(scell_failedto_setup_list, bref, 1, 32, true));
        break;
      }
      case 98: {
        inactivity_monitoring_resp_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(inactivity_monitoring_resp.unpack(bref));
        break;
      }
      case 7: {
        crit_diagnostics_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.unpack(bref));
        break;
      }
      case 202: {
        srbs_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(srbs_setup_list, bref, 1, 8, true));
        break;
      }
      case 260: {
        bh_chs_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_setup_list, bref, 1, 65536, true));
        break;
      }
      case 279: {
        bh_chs_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(bh_chs_failed_to_be_setup_list, bref, 1, 65536, true));
        break;
      }
      case 324: {
        sl_drbs_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_setup_list, bref, 1, 512, true));
        break;
      }
      case 316: {
        sl_drbs_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(sl_drbs_failed_to_be_setup_list, bref, 1, 512, true));
        break;
      }
      case 376: {
        requested_target_cell_global_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(requested_target_cell_global_id.unpack(bref));
        break;
      }
      case 548: {
        scg_activation_status_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(scg_activation_status.unpack(bref));
        break;
      }
      case 602: {
        uu_rlc_ch_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_setup_list, bref, 1, 32, true));
        break;
      }
      case 603: {
        uu_rlc_ch_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(uu_rlc_ch_failed_to_be_setup_list, bref, 1, 32, true));
        break;
      }
      case 611: {
        pc5_rlc_ch_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_setup_list, bref, 1, 512, true));
        break;
      }
      case 612: {
        pc5_rlc_ch_failed_to_be_setup_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(pc5_rlc_ch_failed_to_be_setup_list, bref, 1, 512, true));
        break;
      }
      case 697: {
        serving_cell_mo_encoded_in_cgc_list_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(serving_cell_mo_encoded_in_cgc_list, bref, 1, 8, true));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 39);
  j.write_str("criticality", "reject");
  du_to_cu_rrc_info.to_json(j);
  if (c_rnti_present) {
    j.write_int("id", 95);
    j.write_str("criticality", "ignore");
    j.write_int("Value", c_rnti);
  }
  if (res_coordination_transfer_container_present) {
    j.write_int("id", 49);
    j.write_str("criticality", "ignore");
    j.write_str("Value", res_coordination_transfer_container.to_string());
  }
  if (full_cfg_present) {
    j.write_int("id", 94);
    j.write_str("criticality", "reject");
    j.write_str("Value", "full");
  }
  if (drbs_setup_list_present) {
    j.write_int("id", 27);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drbs_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (srbs_failed_to_be_setup_list_present) {
    j.write_int("id", 66);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : srbs_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drbs_failed_to_be_setup_list_present) {
    j.write_int("id", 15);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : drbs_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_failedto_setup_list_present) {
    j.write_int("id", 83);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : scell_failedto_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (inactivity_monitoring_resp_present) {
    j.write_int("id", 98);
    j.write_str("criticality", "reject");
    j.write_str("Value", "not-supported");
  }
  if (crit_diagnostics_present) {
    j.write_int("id", 7);
    j.write_str("criticality", "ignore");
    crit_diagnostics.to_json(j);
  }
  if (srbs_setup_list_present) {
    j.write_int("id", 202);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : srbs_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bh_chs_setup_list_present) {
    j.write_int("id", 260);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bh_chs_failed_to_be_setup_list_present) {
    j.write_int("id", 279);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : bh_chs_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_setup_list_present) {
    j.write_int("id", 324);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (sl_drbs_failed_to_be_setup_list_present) {
    j.write_int("id", 316);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : sl_drbs_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (requested_target_cell_global_id_present) {
    j.write_int("id", 376);
    j.write_str("criticality", "reject");
    requested_target_cell_global_id.to_json(j);
  }
  if (scg_activation_status_present) {
    j.write_int("id", 548);
    j.write_str("criticality", "ignore");
    j.write_str("Value", scg_activation_status.to_string());
  }
  if (uu_rlc_ch_setup_list_present) {
    j.write_int("id", 602);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (uu_rlc_ch_failed_to_be_setup_list_present) {
    j.write_int("id", 603);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : uu_rlc_ch_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_setup_list_present) {
    j.write_int("id", 611);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (pc5_rlc_ch_failed_to_be_setup_list_present) {
    j.write_int("id", 612);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : pc5_rlc_ch_failed_to_be_setup_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (serving_cell_mo_encoded_in_cgc_list_present) {
    j.write_int("id", 697);
    j.write_str("criticality", "ignore");
    j.start_array("Value");
    for (const auto& e1 : serving_cell_mo_encoded_in_cgc_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ue_inactivity_notif_ies_o>;

SRSASN_CODE ue_inactivity_notif_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += sdt_termination_request_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)100, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_activity_list, 1, 64, true));
  }
  if (sdt_termination_request_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)635, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::ignore}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 100: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_dyn_seq_of(drb_activity_list, bref, 1, 64, true));
        break;
      }
      case 635: {
        sdt_termination_request_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sdt_termination_request.unpack(bref));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 100);
  j.write_str("criticality", "reject");
  j.start_array("Value");
  for (const auto& e1 : drb_activity_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (sdt_termination_request_present) {
    j.write_int("id", 635);
    j.write_str("criticality", "ignore");
    j.write_str("Value", sdt_termination_request.to_string());
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ul_rrc_msg_transfer_ies_o>;

SRSASN_CODE ul_rrc_msg_transfer_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += sel_plmn_id_present ? 1 : 0;
  nof_ies += new_gnb_du_ue_f1ap_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)40, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)41, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)64, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, srb_id, (uint8_t)0u, (uint8_t)3u, true, true));
  }
  {
    HANDLE_CODE(pack_integer(bref, (uint32_t)50, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(rrc_container.pack(bref));
  }
  if (sel_plmn_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)224, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(sel_plmn_id.pack(bref));
  }
  if (new_gnb_du_ue_f1ap_id_present) {
    HANDLE_CODE(pack_integer(bref, (uint32_t)219, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(crit_e{crit_e::reject}.pack(bref));
    varlength_field_pack_guard varlen_scope(bref, true);
    HANDLE_CODE(pack_integer(bref, new_gnb_du_ue_f1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
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
    crit_e crit;
    HANDLE_CODE(crit.unpack(bref));

    switch (id) {
      case 40: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_cu_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 41: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
        break;
      }
      case 64: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(srb_id, bref, (uint8_t)0u, (uint8_t)3u, true, true));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(rrc_container.unpack(bref));
        break;
      }
      case 224: {
        sel_plmn_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(sel_plmn_id.unpack(bref));
        break;
      }
      case 219: {
        new_gnb_du_ue_f1ap_id_present = true;
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(unpack_integer(new_gnb_du_ue_f1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
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
  j.write_int("id", 40);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_cu_ue_f1ap_id);
  j.write_int("id", 41);
  j.write_str("criticality", "reject");
  j.write_int("Value", gnb_du_ue_f1ap_id);
  j.write_int("id", 64);
  j.write_str("criticality", "reject");
  j.write_int("Value", srb_id);
  j.write_int("id", 50);
  j.write_str("criticality", "reject");
  j.write_str("Value", rrc_container.to_string());
  if (sel_plmn_id_present) {
    j.write_int("id", 224);
    j.write_str("criticality", "reject");
    j.write_str("Value", sel_plmn_id.to_string());
  }
  if (new_gnb_du_ue_f1ap_id_present) {
    j.write_int("id", 219);
    j.write_str("criticality", "reject");
    j.write_int("Value", new_gnb_du_ue_f1ap_id);
  }
  j.end_obj();
}
